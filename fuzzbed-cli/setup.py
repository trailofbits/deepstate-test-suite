#!/usr/bin/env python3.6
"""
setup.py

    DESCRIPTION:
        Builds CLI application for use in environment with setuptools.

"""

import setuptools

setuptools.setup(
    name="fuzzbed-cli",
    version="0.1",
    description="Command-line application for interfacing fuzzbed infrastructure for containerized testing.",
    url="https://github.com/trailofbits/fuzzbed",
    author="Alan Cao",
    author_email="alan.cao@trailofbits.com",
    license="Apache-2.0",
    packages=['cli'],
    scripts=["bin/fuzzbed-cli.py"]
)
