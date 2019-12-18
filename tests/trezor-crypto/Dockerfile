FROM deepstate:latest

# Initialize container host with workspace configurations
RUN useradd -ms /bin/bash fuzzer && echo "fuzzer:fuzzer" | chpasswd && adduser fuzzer sudo
RUN chown -R fuzzer:fuzzer /home/fuzzer

# Switch to user work directory, and copy over workspace
USER fuzzer
WOKRDIR /home/fuzzer
COPY . /home/fuzzer/trezor-crypto


# Pre-execution provisioning steps. These are formatted over
# either from the manifest's provisioning
RUN apt-get install libssl-dev libcrypto-dev
RUN git clone https://github.com/trezor/trezor-crypto.git
RUN cd trezor-crypto/
RUN cp options.h ed25519-donna/
RUN make
RUN cp tests/libtrezor-crypto.so ../libtrezor-crypto.so
RUN make clean
RUN cd ..



# Run the fuzzer executor with the target's corresponding
# configuration path
CMD ["deepstate-eclipser", "--config", "config.ini"]
