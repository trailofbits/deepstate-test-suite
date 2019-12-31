#!/usr/bin/env python3.6
"""
fuzzbed-cli.py

    DESCRIPTION:
        Entry point to the fuzzbed CLI application, enabling a user to
        locally (or remotely) instantiate and interact with containers for
        fuzz testing.

    USAGE:
        fuzzbed-cli [COMMAND] <flags> ...
"""

import logging
logging.basicConfig()

import os
import sys
import string
import random
import argparse

from fuzzbed_cli.client import Client

LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(os.environ.get("FUZZBED_LOG", "INFO").upper())


def main() -> int:
    parser = argparse.ArgumentParser(description="CLI application for interfacing fuzzbed")
    subparsers = parser.add_subparsers(dest="command")

    # `init` - creates a new testing workspace in $TESTBED. Initializes a templated configuration plus
    #          harness none of those arguments are provided.
    init_parser = subparsers.add_parser("init")
    init_parser.add_argument(
        "-n", "--name", type=str, default="workspace",
        help="Name of testing workspace (default is `workspace`).")

    init_parser.add_argument(
        "-c", "--config", type=str, required = "--tests" in sys.argv,
        help="Defines the configuration file to be consumed for analysis. Will be created if not specified.")

    init_parser.add_argument(
        "--tests", default=[], nargs=argparse.REMAINDER, required = "--config" in sys.argv,
        help="Define harness or harnesses to reside in testbed workspace. Will create a single templated harness if not specified.")


    # `list` - provides different output facilities for reporting various components of
    list_parser = subparsers.add_parser("list")
    list_parser.add_argument(
        "--out_tests", action="store_true", help="List out tests for harness(es) if flag is set.")


    # `start` - provisions a container for analysis.
    start_parser = subparsers.add_parser("start")
    start_parser.add_argument(
        "--target", type=str, required=True,
        help="Name of workspace to parse configuration and spin up container.")

    start_parser.add_argument(
        "--job_name", type=str, default="worker_" + "".join([random.choice(string.ascii_letters + string.digits) for n in range(5)]),
        help="Name of worker job for target workspace that identifies deployed container for testing.")


    # `ps` - lists out worker jobs and their statuses that are deployed and actively fuzzing.
    ps_parser = subparsers.add_parser("ps")
    ps_parser.add_argument(
        "--job_name", type=str,
        help="Name of active worker job to introspect.")

    args = parser.parse_args()

    client = Client()


    if args.command == "init":
        ws_path = client.init_ws(args.name, args.config, args.tests)
        print("\n[*] Initialized new workspace at `{}` [*]\n".format(ws_path))
        sys.exit(0)


    elif args.command == "list":
        if client.workspaces is None:
            print("\n[!] No workspaces available, or testbed environment is littered [!]\n")
            sys.exit(1)

        print("Workspace Name\t|\tWorkspace Path")
        print("".join(["{}\t|\t{}\n".format(name, path) for (name, path) in client.workspaces]))
        print("\n")
        sys.exit(0)


    elif args.command == "start":
        if client.execute(args.target, args.job_name) is None:
            print("\n[!] Unable to start worker job for `{}` target [!]\n".format(args.target))
            sys.exit(1)

        print("[*] Worker job for `{}` successfully started. Call `fuzzbed-cli ps --job_name {}` to view status [*]"
            .format(args.target, args.job_name))
        sys.exit(0)


    elif args.command == "ps":
        job_ps = client.get_process(args.job)
        if job_ps is None:
            print("\n[!] No worker job with name `{}` available [!]\n".format(job_ps))

        sys.exit(0)


    else:
        parser.print_help(sys.stderr)
        sys.exit(1)

    return 0


if __name__ == "__main__":
    exit(main())
