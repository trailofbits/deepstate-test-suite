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

from typing import Optional

#from deepstate.core import config


class Client(object):
    """
    A Client is an object that encapsulates an interface for interacting with
    """

    def __init__(test_env: str = "TESTBED") -> None:
        """
        Initializes a client to interface testing. Uses a default envvar to specify
        the path to the "testbed" of testing harnesses and artifacts.

        :param test_env: envvar to path of test harnesses and artifacts
        """

        env: str = os.environ.get[test_env]
        if env is None:
            raise Exception("no tests found in testbed envvar `${}`.".format(env))
        if ':' in env:
            raise Exception("should not have multiple paths set with testbed envvar `${}`.".format(env))

        # get all test workspaces from testbed directory
        self.test_paths = [testdir[0] for testdir in os.walk(env)]


    def init_ws(ws_name: str, num_tests: int, config_path: str) -> None:
        """
        Creates a new workspace in the
        """
        #self.config = config.consume(config_path)
