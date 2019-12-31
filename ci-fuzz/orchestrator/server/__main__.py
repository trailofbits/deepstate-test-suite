#!/usr/bin/env python3
"""
fuzzbed-server

    DESCRIPTION:
        Orchestration server that receives and responds to requests
        from the `fuzzbed-cli` client. This service is seperated in order to
        support a microservice infrastructure if necessary and enables a client
        to interact locally and/or remotely.

    USAGE:
        python setup.py install
        ./orchestrator

"""

import os
import flask

import .config


app = flask.Flask(__name__)


@app.route("/api/start")
def start():
    return "New job"


@app.route("/api/stop")
def stop():
    return "Stopping"


@app.route("/api/list")
def list():
    return "None"


@app.route("/")
def index():
    """
    Throw to a TODO homepage
    """
    return "fuzzbed instance"



if __name__ == "__main__":
    debug_env = os.environ.get("SERVER_DEBUG")
    app.run(debug=debug_env != None)
