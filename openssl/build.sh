#!/bin/bash

# Clone and build openssl versions
if [ ! -d "openssl-OpenSSL_1_0_2d" ]; then
  wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_2d.tar.gz
  tar xvf OpenSSL_1_0_2d.tar.gz

  cd openssl-OpenSSL_1_0_2d/
  ./config
  CC=afl-clang-fast CXX=afl-clang-fast++ make

  cp libcrypto.a ../libcrypto_1_0_2d_afl.a
  cp libssl.a ../libssl_1_0_2d_afl.a

  make clean
  CC=$ANGORA/bin/angora-clang CXX=$ANGORA/bin/angora-clang++ make

  cp libcrypto.a ../libcrypto_1_0_2d_fast.a
  cp libssl.a ../libssl_1_0_2d_fast.a

  make clean
  USE_TRACK=1 CC=$ANGORA/bin/angora-clang CXX=$ANGORA/bin/angora-clang++ make

  cp libcrypto.a ../libcrypto_1_0_2d_taint.a
  cp libssl.a ../libssl_1_0_2d_taint.a

  cd ..
  rm *.tar.gz
fi

if [ ! -d "openssl-OpenSSL_1_0_1f" ]; then
  wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_1f.tar.gz
  tar xvf OpenSSL_1_0_1f.tar.gz
  cd openssl-OpenSSL_1_0_1f/

  ./config
  CC=afl-clang-fast CXX=afl-clang-fast++ make

  cp libcrypto.a ../libcrypto_1_0_1f.a
  cp libssl.a ../libssl_1_0_1f.a

  cd ..
  rm *.tar.gz
fi


# Build with DeepState compiler wrappers
WORKSPACE="${1:-bins}"
TESTS=("bn_modexp", "heartbleed")

mkdir "$WORKSPACE"

printf "\nBuilding test_bn_modexp.cpp with AFL\n"
deepstate-afl --compile_test test_bn_modexp.cpp --compiler_args="libcrypto_1_0_2d.a -lgcrypt -lgpg-error" --out_test_name "$WORKSPACE"/"$test"

printf "\nBuilding test_bn_modexp.cpp with Angora\n"
deepstate-angora --compile_test test_bn_modexp.cpp --compiler_args="libcrypto_1_0_2d.a -lgcrypt -lgpg-error" --out_test_name "$WORKSPACE"/"$test" \
	--ignore_calls libcrypto_1_0_2d.a

######

printf "\nBuilding test_heartbleed.cpp with AFL\n"
deepstate-afl --compile_test test_heartbleed.cpp --compiler_args="libgcrypt.a libcrypto.a -lgpg-error" --out_test_name "$WORKSPACE"/"$test"

printf "\nBuilding test_heartbleed.cpp with Angora\n"
deepstate-angora --compile_test test_heartbleed.cpp --compiler_args="libgcrypt.a libcrypto.a -lgpg-error" --out_test_name "$WORKSPACE"/"$test" \
	--ignore_calls /usr/lib/x86_64-linux-gnu/libssl.a:/usr/lib/x86_64-linux-gnu/libcrypto.a


