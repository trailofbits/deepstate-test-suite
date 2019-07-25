git clone https://github.com/trezor/trezor-crypto.git
cd trezor-crypto
make && cd ..

make test_openssl
make test_properties
make test_bignum
