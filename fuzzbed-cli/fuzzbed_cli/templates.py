"""
templates.py

    DESCRIPTION:
        Represents templates for files to be generated in a test workspace.
        Placeholders should be replaced with actual analysis information from
        a harness and configuration.
"""



# default filenames for testbed workspace components
DEFAULT_HARNESS_NAME = "test_default.cpp"
DEFAULT_CONFIG_NAME = "config.ini"


# allowed tools for analysis in manifest
ALLOWED = [
    "afl", "eclipser", "honggfuzz", "angora",
    "ensemble"
]


# not yet supported tools for analysis
NOT_SUPPORTED = ["manticore", "angr"]


# defines the default manifest section to write to harness
# TODO: define as custom AttrDict type
MANIFEST_CONFIG = {

    # identifying software being tested for worker
    "name": "{TEST}",

    # name of docker host
    "hostname": "fuzzer",

    # used to build up proper Dockerfile with executor to use
    "executor": "afl",

    # defines steps for provisioning the workspace
    "provision_steps": []
}


# default configuration file to be generated for a fresh workspace
DEFAULT_CONFIG = {
    "manifest": MANIFEST_CONFIG,
    "compile": {
        "compile_test": DEFAULT_HARNESS_NAME,
        "compile_args": [],
    },
    "test": {
        "input_seeds": "in",
        "output_test_dir": "out",
    }
}


# templated Dockerfile
DOCKERFILE = """FROM deepstate:latest

# Pre-execution provisioning
{PROVISION_STEPS}

# Initialize container host with workspace
RUN chown -R {USER}:{USER} /home/{USER}
USER {USER}
COPY . /home/{USER}/{WS_NAME}

RUN deepstate-{TOOL} --config {CONF_FILE}

CMD ["/bin/bash"]
"""


DEFAULT_TEST_HARNESS = """// {HARNESS_NAME}
//

#include <deepstate/DeepState.hpp>

using namespace deepstate;


TEST(Unit, TestName) {
    LOG(TRACE) << "Running unit test";

    // .. include test logic here
}
"""
