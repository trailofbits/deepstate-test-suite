FROM deepstate:latest

# Initialize container host with workspace configurations
RUN useradd -ms /bin/bash fuzzer && echo "fuzzer:fuzzer" | chpasswd && adduser fuzzer sudo
RUN chown -R fuzzer:fuzzer /home/fuzzer

# Switch to user work directory, and copy over workspace
USER fuzzer
WOKRDIR /home/fuzzer
COPY . /home/fuzzer/openssl


# Pre-execution provisioning steps. These are formatted over
# either from the manifest's provisioning
RUN wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_2d.tar.gz
RUN tar xvf OpenSSL_1_0_2d.tar.gz
RUN cd openssl-OpenSSL_1_0_2d/
RUN make
RUN cp libcrypto.a ../libcrypto_1_0_2d_afl.a
RUN cp libssl.a ../libssl_1_0_2d_afl.a
RUN make
RUN cd ..



# Run the fuzzer executor with the target's corresponding
# configuration path
CMD ["deepstate-afl", "--config", "config.ini"]
