FROM deepstate:latest

# Initialize container host with workspace configurations
RUN useradd -ms /bin/bash fuzzer && echo "fuzzer:fuzzer" | chpasswd && adduser fuzzer sudo
RUN chown -R fuzzer:fuzzer /home/fuzzer

# Switch to user work directory, and copy over workspace
USER fuzzer
WORKDIR /home/fuzzer
COPY . /home/fuzzer/tweetnacl


# Pre-execution provisioning steps. These are formatted over
# either from the manifest's provisioning
RUN wget http://seb.dbzteam.org/crypto/tweetnacl_bug.c
RUN gcc -g -O0 -rdynamic tweetnacl_bug.c -o verifier
RUN echo "5a22083abc7bdb6371f4e40a6e562b7d3fe3ba87131ab6e9feb341f909f3f581" >> crash



# Run the fuzzer executor with the target's corresponding
# configuration path
CMD ["deepstate-afl", "--config", "config.ini"]
