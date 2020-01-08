"""
client.py

    DESCRIPTION:
        API interface for interacting with test workspaces and Docker for containerized fuzzing.
        Our client object interacts with our testbed environment, manages testing workspaces,
        and spins off container fuzzing workers when necessary.

    USAGE:
        client = Client()
        client.functionality()

"""
import logging
logging.basicConfig()

import os
import json
import string
import random
import shutil
import subprocess
import requests
import configparser

from fuzzbed_cli import templates
from deepstate.core.base import AnalysisBackend

from typing import Optional, List, Dict, Any, Union, Tuple

LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(os.environ.get("FUZZBED_LOG", "INFO").upper())

# TODO: somehow import from core api
ConfigType = Dict[str, Dict[str, Any]]


class ClientError(Exception):
    pass


class Client(object):
    """
    A Client is an object that encapsulates an interface for interacting with the testing environment,
    """

    def __init__(self, test_env: str = "TESTBED", server_env) -> None:
        """
        Initializes a client to interface testing. Uses a default envvar to specify
        the path to the "testbed" of testing harnesses and artifacts.

        :param test_env: envvar to path of test harnesses and artifacts, default is $TESTBED
        """

        env: str = os.environ.get(test_env)
        if env is None:
            raise ClientError("no tests found in testbed envvar `${}`.".format(test_env))
        if ':' in env:
            raise ClientError("should not have multiple paths set with testbed envvar `${}`.".format(env))

        # testbed directory root
        self.env: str = env
        LOGGER.debug("Path to testbed env: {}".format(self.env))

        # get all test workspaces from testbed directory
        self.test_paths: List[str] = [testdir[0] for testdir in os.walk(env)]

        # get env for overwritten service host and port
        server_env: Optional[str] = os.environ.get("SERVER")
        self.server_addr: str = "0.0.0.0:1234" if server_env is None else server_env


    @staticmethod
    def _init_config(ws_path: str) -> Optional[ConfigType]:
        """
        Helper method that takes an input path and generates a serializable configuration from a default dict.

        :param ws_path: absolute path to directory to initialize with default config name
        """

        # define abspath to config file
        conf_path: str = os.path.join(ws_path, templates.DEFAULT_CONFIG_NAME)
        LOGGER.debug("Path to configuration to write: {}".format(conf_path))

        # initialize a parser to write
        parser = configparser.ConfigParser()
        parser.update(templates.DEFAULT_CONFIG)
        with open(conf_path, "w") as conf_file:
            parser.write(conf_file)

        # with file on disk, re-initialize with AnalysisBackend helper
        return AnalysisBackend.build_from_config(conf_path, include_sections=True)


    def init_ws(self, _ws_name: str, config_path: Optional[str] = None, harness_paths: List[str] = []) -> str:
        """
        Creates a new workspace in the testbed environment path. If no configuration and harness(es) is provided,
        the client will initialize default ones for the user. Returns the abspath to the new testbed if successfully
        initialized. Does NOT communicate with the orchestrator API.

        :param ws_name: name of workspace directory.
        :param config_path: optional path to configuration file to consume be consumed by DeepState executor.
        :param harness_paths: optional paths to existing DeepState test harnesses
        """

        # check if abspath to workspace already exists
        ws_name: str = os.path.join(self.env, _ws_name)
        if ws_name in self.test_paths:
            raise ClientError("workspace directory already exists in testbed path.")
        LOGGER.debug("Workspace path to initialize: {}".format(ws_name))

        # initialize workspace directory with ws_name
        LOGGER.info("Creating workspace directory in testbed.")
        os.mkdir(ws_name)

        # initialize new config to workspace or copy over config_path
        if config_path is None:
            LOGGER.info("Initializing a new default configuration.")
            config = Client._init_config(ws_name)
        else:

            # copy configuration file to testbed path
            LOGGER.info("Copying configuration `{}` to `{}`.".format(config_path, ws_name))
            shutil.copy(config_path, ws_name)

            # initialize user-specified path as configuration, and sanity-check
            config = AnalysisBackend.build_from_config(config_path, include_sections=True)

            # ensure manifest section exists
            if "manifest" not in config.keys():
                raise ClientError("No manifest section defined. Cannot instantiation request without one in config.")

            # compare keys against default template. Check if minimum number of required ones exist
            missing = [key for key in templates.MANIFEST_CONFIG.keys() if key not in config["manifest"].keys()]
            if len(missing) > 0:
                raise ClientError("Missing entries in manifest: {}".format(missing))

        # parse out configuration manifest for information for Dockerfile
        LOGGER.debug(config)

        manifest: str = config["manifest"]

        # sanity-check configuration
        executor: str = manifest["executor"]
        if executor in templates.NOT_SUPPORTED:
            raise ClientError("{} executor not yet supported by fuzzbed".format(executor))
        elif executor not in templates.ALLOWED:
            raise ClientError("{} executor not found".format(executor))

        LOGGER.info("Initializing with `{}` executor".format(executor))

        # initialize Dockerfile
        dockerfile: str = templates.DOCKERFILE \
            .replace("{TOOL}", executor) \
            .replace("{WS_NAME}", _ws_name) \
            .replace("{CONF_FILE}", "config.ini")

        # initialize optional arguments or use default
        if "hostname" in manifest.keys():
            dockerfile = dockerfile.replace("{USER}", manifest["hostname"])
        else:
            dockerfile = dockerfile.replace("{USER}", "fuzzer")

        # if provisioning steps were specified, apply to Dockerfile
        LOGGER.info("Checking for provisioning steps to write to Dockerfile")

        provision_list = json.loads(manifest["provision_steps"])
        LOGGER.debug(provision_list)

        if len(provision_list) > 0:
            steps: List[str] = "".join(["RUN {}\n".format(cmd) for cmd in provision_list])
            dockerfile = dockerfile.replace("{PROVISION_STEPS}", steps)
        else:
            dockerfile = dockerfile.replace("{PROVISION_STEPS}", " ")

        # write finalized Dockerfile to workspace for container deployment
        with open(os.path.join(ws_name, "Dockerfile"), "w") as f:
            f.write(dockerfile)

        # initialize initial corpus directory
        os.mkdir(os.path.join(ws_name, config["test"]["input_seeds"]))

        # if no existing harnesses are specified, write a single default one
        if len(harness_paths) == 0:
            LOGGER.info("Writing new default test harness.")
            with open(os.path.join(ws_name, templates.DEFAULT_HARNESS_NAME), "w") as f:
                f.write(templates.DEFAULT_TEST_HARNESS.replace("{HARNESS_NAME}", templates.DEFAULT_HARNESS_NAME))

        # if not, copy harnesses over to workspace
        else:
            LOGGER.info("Initializing workspace with specified harness paths.")
            for _path in harness_paths:
                path = os.path.abspath(_path)

                LOGGER.debug("Copying source harness `{}` to `{}`.".format(_path, ws_name))
                shutil.copy(path, ws_name)

        return ws_name


    def init_container(ws_name: str, _job_name: Optional[str]) -> Union[bool, Tuple[bool, str]]:
        """
        Sends a POST request to /api/init in order to provision a new
        container job.

        :param ws_name: string name of workspace to test
        :param job_name: optional identifier
        """

        # create pseudorandom id if not specified
        job_name: str = "worker_".format(random.choice(string.ascii_lowercase + string.digits) for _ in range(4)) \
                        if not _job_name else _job_name
        LOGGER.debug("Job name: {}".format(job_name))

        # send a POST request with necessary parameters
        # TODO: fine-grained config, like specific harness and/or test
        payload: Dict[str, str] = dict({
            "job_name": job_name,
            "test": ws_name
        })

        LOGGER.debug("Payload info: {}".format(payload))

        r = requests.post(self.server_addr, data=payload)

        # check for correct status code
        status = r.status_code
        if status != 200:
            reason: str = "failed with status {}".format(status)
            return (False, str(reason))

        # now parse out response
        response = vars(r.json())
        if response["status"] != "success":
            return (False, response["reason"])

        return True


    WorkspaceInfo = Dict[str, Union[str, bool]]

    @property
    def workspaces(self) -> Optional[WorkspaceInfo]:
        """
        Parses out information for existing workspaces. Does NOT communicate with orchestrator server,
        since workspaces are all on shared volume.
        """

        # TODO: more verbose - how many harnesses, and tests composing th harnesses
        workspaces: List[WorkspaceInfo] = [
            dict({
                "name": name,
                "path": os.path.join(self.env, name),
                "includes_corpus": os.path.isdir("input")
            })
            for name in self.test_paths
        ]
        return workspaces


    def get_process(self, job_name: Optional[str]) -> Dict[str, Any]:
        """
        Sends a GET request to /api/process/<job_name> in order to retrieve specific information about a process.
        If job_name is not specified, general information will be outputted regarding every single active worker.

        :param job_name: name of worker job that is active to introspect
        """
        r = request.get(self.server_addr)
