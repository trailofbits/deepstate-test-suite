"""
client.py

    DESCRIPTION:
        API interface for interacting with test workspaces and
        Docker for containerized fuzzing.

    USAGE:
        client = Client()
        client.functionality()

"""
import os
import random
import string
import subprocess
import requests

from typing import Optional, List, Dict

#from deepstate.core import config


class Client(object):
    """
    A Client is an object that encapsulates an interface for interacting with
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
        self.job_name: str = "worker_" + "".join([random.choice(string.ascii_letters + string.digits) for n in xrange(5)])

        # testbed directory root
        self.env: str = env

        # get all test workspaces from testbed directory
        self.test_paths: List[str] = [testdir[0] for testdir in os.walk(env)]


    def _parse_ws(self, ws_name) -> Dict[str, str]:
        """
        From an input workspace directory, parse out all of the necessary components of the workspace to pass back to the
        user for for further manipulation.

        :param ws_name: name of workspace directory to parse.
        """

        if ws_name not in self.test_paths:
            raise Exception("workspace directory not find in testbed path.")

        ws_conf = Dict[str, str]
        return ws_config


    def init_ws(self, ws_name: str, num_tests: int, config_path: str) -> None:
        """
        Creates a new workspace in the testbed environment path. Consumes a configuration path in order to
        generate necessary components, including the templated build scripts.

        :param ws_name: name of workspace directory.
        :param num_tests: num of tests to generate for default harness.
        :param config_path: path to configuration file to consume be consumed by DeepState executor.
        """

        if ws_name in self.test_paths:
            raise Exception("workspace directory already exists in testbed path.")


    def list_tests(job: Optional[str]) -> str:
        return "some"
