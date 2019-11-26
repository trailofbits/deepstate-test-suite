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


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="CLI application for interfacing fuzzbed")
    subparsers = parser.add_subparsers(dest="command")

    # `init` - creates a new testing workspace in `tests/` with a default single template harness.
    init_parser = subparsers.add_parser("init")
    init_parser.add_argument("-n", "--name", type=str, default="workspace", help="Name of testing workspace (default is `workspace`)")
    init_parser.add_argument("-t", "--num_tests", type=int, default=1, help="Number of tests to initialize in default harness (default is 1).")
    init_parser.add_argument("-c", "--configuration", type=str, required=True, help="Defines the configuration file to be consumed for analysis.")

    # `list` - outputs available testing workspaces and their contents.
    list_parser = subparsers.add_parser("list")
    list_parser.add_argument("--out_tests", action="store_true", help="List out tests for harness(es) if flag is set.")

    # `ps` - lists out worker jobs and their statuses that are deployed and actively fuzzing.
    ps_parser = subparsers.add_parser("ps")
    ps_parser.add_argument("-j", "--job", type=str, help="Check status of an individual worker job.")

    # `start` - provisions a container for analysis.
    start_parser = subparsers.add_parser("start")
    start_parser.add_argument("--target", type=str, required=True, help="Name of workspace to parse configuration and spin up container.")
    start_parser.add_argument("--job_name", type=str, default="", help="Name of worker job that deploys container for testing.")

    # .. other arguments
    args.add_argument("--override_env", type=str, help="Overrides the envvar that specifies the path to tests (default is $TESTBED).")

    return parser.parse_args()


def main() -> int:
    args = parse_args()

    client = Client(args.override_env)

    # parse subcommands
    if args.command == "init":
        client.init_ws(args.name, args.num_tests)

    elif args.command == "list":

    elif args.start == "start":

    else:
        parser.print_help(sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    exit(main())
