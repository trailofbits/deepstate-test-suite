FROM deepstate:latest

# Initialize container host with workspace configurations
RUN useradd -ms /bin/bash fuzzer && echo "fuzzer:fuzzer" | chpasswd && adduser fuzzer sudo
RUN chown -R fuzzer:fuzzer /home/fuzzer

# Switch to user work directory, and copy over workspace
USER fuzzer
WORKDIR /home/fuzzer
COPY . /home/fuzzer/json


# Pre-execution provisioning steps. These are formatted over
# either from the manifest's provisioning
RUN git clone https://github.com/nlohmann/json
RUN cp json/single_include/nlohmann/json.hpp .



# Run the fuzzer executor with the target's corresponding
# configuration path
CMD ["deepstate-afl", "--config", "config.ini"]
