#!/bin/bash

# Clone and build trezor
if [ ! -d "trezor-crypto" ]; then
  git clone https://github.com/trezor/trezor-crypto.git
  cd trezor-crypto/
  cp options.h ed25519-donna/
  make && cd ..
fi

# Build with DeepState compiler wrappers
WORKSPACE="${1:-bins}"
TESTS=("bignum" "properties" "openssl")

mkdir "$WORKSPACE"
for test in "${TESTS[@]}"; do
  
  printf "\nBuilding test_$test.cpp with AFL\n"
  deepstate-afl --compile_test test_"$test".cpp --compiler_args="-ltrezor-crypto -lssl -lcrypto" --out_test_name "$WORKSPACE"/"$test"
  
  printf "\nBuilding test_$test.cpp with Angora\n"
  deepstate-angora --compile_test test_"$test".cpp --compiler_args="-ltrezor-crypto -lssl -lcrypto" --out_test_name "$WORKSPACE"/"$test" \
	  --ignore_calls /usr/lib/x86_64-linux-gnu/libssl.a:$PWD/trezor-crypto/tests/libtrezor-crypto.so:/usr/lib/x86_64-linux-gnu/libcrypto.a
done

# $(CC) -std=c++11 -g test_* -ltrezor-crypto -lssl -lcrypto
