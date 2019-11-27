"""
templates.py

    DESCRIPTION:
        Represents templates for files to be generated in a test workspace.
        Placeholders should be replaced with actual analysis information from
        a harness and configuration.
"""


DEFAULT_HARNESS = "test_default.cpp"


DOCKERFILE = """
FROM deepstate:latest

# FOR USER: specify additional dependencies to install and/or build

RUN chown -R {USER}:{USER} /home/{USER}
USER {USER}
COPY tests/{TEST} .

RUN deepstate-{TOOL} {OPTIONS}
RUN deepstate-{TOOL} {OPTIONS}

CMD ["/bin/bash"]
"""


HEADER = """
// FOR USER: include other headers

#include <deepstate/DeepState.hpp>

using namespace deepstate;
"""


TEST_FUNC = """
TEST({UNIT}, {TESTNAME}) {

}
"""
