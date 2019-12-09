"""
templates.py

    DESCRIPTION:
        Represents templates for files to be generated in a test workspace.
        Placeholders should be replaced with actual analysis information from
        a harness and configuration.
"""


DEFAULT_HARNESS_NAME = "test_default.cpp"
DEFAULT_CONFIG_NAME = "config.ini"


MANIFEST_CONFIG = {
    "name": "Name of software tested",
    "fuzzer": "afl",
}

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


DOCKERFILE = """FROM deepstate:latest

# FOR USER: specify additional dependencies to install and/or build

RUN chown -R {USER}:{USER} /home/{USER}
USER {USER}
COPY tests/{TEST} .

RUN deepstate-{TOOL} {OPTIONS}
RUN deepstate-{TOOL} {OPTIONS}

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
