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
    description="CLI application for interfacing fuzzbed for containerized fuzzing.",
    url="https://github.com/trailofbits/fuzzbed",
    entry_points={
        "console_scripts": [
            "fuzzbed-cli = fuzzbed-cli:main"
        ]
    }
)
