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

import flask


def main():
    server = flask.Flask()
    server.run()


if __name__ == "__main__":
    exit(main())
