#!/usr/bin/env python3.6
"""
setup.py

    DESCRIPTION:
        Builds CLI application for use in environment with setuptools. Note that
        DeepState is not a dependency, and requires external installation with a build script
        or manually in the environment.

"""
import setuptools

FUZZBED_VERSION = "0.1"

setuptools.setup(
    name = "fuzzbed-cli",
    version = FUZZBED_VERSION,
    description = "Command-line application for interfacing fuzzbed infrastructure for containerized testing.",
    url = "https://github.com/trailofbits/fuzzbed",
    author = "Alan Cao",
    author_email = "alan.cao@trailofbits.com",
    license = "Apache-2.0",
    packages = ['fuzzbed_cli'],
    install_requires = ['requests', 'mypy'],
    entry_points = {
        "console_scripts" : ["fuzzbed-cli = fuzzbed_cli.__main__:main"]
    }
)
