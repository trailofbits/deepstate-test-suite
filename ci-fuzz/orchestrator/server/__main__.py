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
import docker
import redis

import .config

# instantiate flask web server
app = flask.Flask(__name__)
app.secret_key = config.SECRET_KEY

# instantiate Docker engine
client = docker.from_env()

# start redis with copnfiguration
store = redis.Redis(host=config.REDIS_HOST, port=config.REDIS_PORT, db=0)


@app.route("/api/init", methods=["POST"])
def init_container():
    """
    /api/init (POST)
        Provisions a new Docker container from a stored test
        in the shared volume.

        Params:
            job_name: identifier for container job
            test: name of target created in shared volume
    """

    method = flask.request.method
    if method != "POST":
        return flask.jsonify({
            "status": "failed",
            "reason": "cannot communicate with {}".format(method)
        })

    # DO IT!

    return flask.jsonify({
        "status": "success",
        "reason": None
    })


@app.route("/api/info", methods=["GET"])
def ps_info():
    """
    /api/info (GET)
        Provides general information about all
        running container processes.
    """
    active = client.containers.list()
    return flask.jsonify(active)


@app.route("/api/info/<query>", methods=["GET"])
def query(query):
    """
    /api/info/<query> (GET)
        Provides information about a specific running process.

        Params:
            query: represents type of information to list,
                can either be `container
    """

    # TODO: communicate with redis store

    response = dict({
        "alive": True
        "uptime": 100,
        "run_cmd": "",
        "crashes_found": "",
        "hangs_found": ""
    })
    return flask.jsonify(response)



@app.route("/")
def index():
    return "Hi! You are talking with the fuzzbed orchestrator service"


if __name__ == "__main__":
    debug_env = os.environ.get("SERVER_DEBUG")
    app.run(debug=debug_env != None)
