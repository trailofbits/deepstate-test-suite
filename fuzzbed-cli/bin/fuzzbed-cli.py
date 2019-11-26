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

import os
import argparse
import subprocess

from typing import Optional
from deepstate.core import config

from cli import templates
from cli.client import Client


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="CLI application for interfacing fuzzbed")
    subparsers = parser.add_subparsers()

    # `init` - creates a new testing workspace in `tests/` with a template harness
    init_parser = subparsers.add_parser("init")
    init_parser.add_argument("--name", type=str, default="workspace", help="Name of testing workspace (default is `workspace`)")
    init_parser.add_argument("--num_tests", type=int, default=1, help="Number of tests to initialize in default harness (default is 1).")

    # `list` - outputs available testing workspaces and their contents
    list_parser = subparsers.add_parser("list")
    list_parser.add_argument("--out_tests", action="store_true", help="List out tests if flag is set.")

    # `start` - provisions a container for analysis
    start_parser = subparsers.add_parser("start")
    start_parser.add_argument("--name", type=str, required=True, help="Name of workspace to parse config and spin up container.")

    # .. other arguments

    return parser.parse_args()


def main() -> int:
    args = parse_args()
    return 0


if __name__ == "__main__":
    exit(main())
