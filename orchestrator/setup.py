#!/usr/bin/env python3.6
"""
setup.py

    DESCRIPTION:
        Builds orchestration service for fuzzbed environment.

"""
import setuptools

FUZZBED_VERSION = "0.1"

setuptools.setup(
    name = "fuzzbed-orchestrator",
    version = FUZZBED_VERSION,
    description = "Orchestration service for fuzzbed container instantiation",
    url = "https://github.com/trailofbits/fuzzbed",
    author = "Alan Cao",
    author_email = "alan.cao@trailofbits.com",
    license = "Apache-2.0",
    packages = ['fuzzbed_orchestrator'],
    install_requires = ['flask', 'mypy'],
    entry_points = {
        "console_scripts" : ["fuzzbed-orchestrator = fuzzbed_orchestrator.__main__:main"]
    }
)
