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

import os
import shutil
import subprocess
import requests

import .templates

from deepstate.core import config
from typing import Optional, List, Dict, Union



class Client(object):
    """
    A Client is an object that encapsulates an interface for interacting with the testing environment,
    """

    def __init__(self, test_env: str = "TESTBED") -> None:
        """
        Initializes a client to interface testing. Uses a default envvar to specify
        the path to the "testbed" of testing harnesses and artifacts.

        :param test_env: envvar to path of test harnesses and artifacts
        """

        env: str = os.environ.get(test_env)
        if env is None:
            raise Exception("no tests found in testbed envvar `${}`.".format(test_env))
        if ':' in env:
            raise Exception("should not have multiple paths set with testbed envvar `${}`.".format(env))

        # initialize a default job name
        self.job_name: str = "worker_" + "".join([random.choice(string.ascii_letters + string.digits) for n in range(5)])

        # testbed directory root
        self.env: str = env

        # get all test workspaces from testbed directory
        self.test_paths: List[str] = [testdir[0] for testdir in os.walk(env)]

    @staticmethod
    def _init_config() -> str:
        return "ok"


    def init_ws(self, ws_name: str, config_path: Optional[str] = None, harness_paths: List[str] = []) -> None:
        """
        Creates a new workspace in the testbed environment path. If no configuration and harness(es) is provided,
        the client will initialize default ones for the user.

        :param ws_name: name of workspace directory.
        :param config_path: optional path to configuration file to consume be consumed by DeepState executor.
        :param harness_paths: optional paths to existing DeepState test harnesses
        """

        if ws_name in self.test_paths:
            raise Exception("workspace directory already exists in testbed path.")

        # initialize workspace directory with ws_name
        ws: str = os.path.join(self.env + '/' + ws_name)
        os.mkdir(ws)

        # initialize new config or from user-specified input
        config = Client._init_config() if config_path is None else AnalysisBackend.build_from_config(config_path)

        # if no existing harnesses are specified, write a single default one
        if len(harness_paths) == 0:
            with open(ws + '/' + templates.DEFAULT_HARNESS_NAME, "w") as f:
                f.write(templates.TEST_HARNESS)

        # if not, copy harnesses over to workspace
        else:
            for path in harness_paths:
                shutil.copyfile(path, ws)
