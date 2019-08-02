#!/bin/bash

# TODO: refactor

# Clone and build openssl versions
if [ ! -d "OpenSSL_1_0_2d" ]; then
  wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_2d.tar.gz
  tar xvf OpenSSL_1_0_2d.tar.gz
  cd OpenSSL_1_0_2d/
  make && cd ..
fi

if [ ! -d "OpenSSL_1_0_1f" ]; then
  wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_1f.tar.gz
  tar xvf OpenSSL_1_0_1f.tar.gz
  cd OpenSSL_1_0_1f/
  make && cd ..
fi


# Build with DeepState compiler wrappers
WORKSPACE="${1:-bins}"
TESTS=("bn_modexp", "heartbleed")

mkdir "$WORKSPACE"
for test in "${TESTS[@]}"; do
  
  printf "\nBuilding test_$test.cpp with AFL\n"
  deepstate-afl --compile_test test_"$test".cpp --compiler_args="-lssl -lcrypto" --out_test_name "$WORKSPACE"/"$test"
  
  printf "\nBuilding test_$test.cpp with Angora\n"
  deepstate-angora --compile_test test_"$test".cpp --compiler_args="-lssl -lcrypto" --out_test_name "$WORKSPACE"/"$test" \
	  --ignore_calls /usr/lib/x86_64-linux-gnu/libssl.a:/usr/lib/x86_64-linux-gnu/libcrypto.a
done
