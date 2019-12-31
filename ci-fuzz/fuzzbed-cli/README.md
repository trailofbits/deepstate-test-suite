# fuzzbed-cli

Client-side command line application for interfacing with the testbed environment and the orchestration engine.

## Usage

To build and install locally:

```
$ python setup.py install
```

Once installed as a package, we can now initialize a new workspace in our testbed environment. This testbed environment
is determined by the `$TESTBED` environment variable, and should point to `/path/to/repo/tests` if manually setting up a local
environment.

```
# set TESTBED
$ export TESTBED=/path/to/repo/tests

# creates a default new workspace in $TESTBED
$ fuzzbed-cli init --name my_workspace_name

# creates a workspace based on pre-existing configuration and harnesses
$ fuzzbed-cli init --name my_workspace_name --config my_config.ini --tests test1.cpp test2.cpp
```
