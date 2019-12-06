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

import sys
import argparse

from fuzzbed_cli.client import Client


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
        "--config", type=str, required = "--tests" in sys.argv,
        help="Defines the configuration file to be consumed for analysis. Will be created if not specified.")

    init_parser.add_argument(
        "--tests", default=[], nargs=argparse.REMAINDER, required = "--config" in sys.argv,
        help="Define harness or harnesses to reside in testbed workspace. Will create a single templated harness if not specified.")


    # `list` - provides different output facilities for reporting various
    list_parser = subparsers.add_parser("list")
    list_parser.add_argument("--out_tests", action="store_true", help="List out tests for harness(es) if flag is set.")

    # `ps` - lists out worker jobs and their statuses that are deployed and actively fuzzing.
    ps_parser = subparsers.add_parser("ps")
    ps_parser.add_argument("-j", "--job", type=str, help="Check status of an individual worker job.")

    # `start` - provisions a container for analysis.
    start_parser = subparsers.add_parser("start")

    start_parser.add_argument("--target",
        type=str, required=True,
        help="Name of workspace to parse configuration and spin up container.")

    start_parser.add_argument("--job_name",
        type=str, default="worker_" + "".join([random.choice(string.ascii_letters + string.digits) for n in range(5)]),
        help="Name of worker job that deploys container for testing.")

    args = parser.parse_args()

    client = Client()

    if args.command == "init":
        client.init_ws(args.name, args.config, args.harnesses)

    elif args.command == "list":
        print(client.list_tests(args.job))

    elif args.command == "ps":
        client.processes()

    elif args.command == "start":
        client.execute()

    else:
        parser.print_help(sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    exit(main())
