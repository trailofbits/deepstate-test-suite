#!/bin/bash

# Clone and build openssl versions
if [ ! -d "openssl-OpenSSL_1_0_2d" ]; then
  wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_2d.tar.gz
  tar xvf OpenSSL_1_0_2d.tar.gz

  cd openssl-OpenSSL_1_0_2d/

  CC=afl-clang-fast CXX=afl-clang-fast++ ./config
  make
  cp libcrypto.a ../libcrypto_1_0_2d_afl.a
  cp libssl.a ../libssl_1_0_2d_afl.a
  make

  cd ..
  rm *.tar.gz
fi


if [ ! -d "openssl-OpenSSL_1_0_1f" ]; then
  wget https://github.com/openssl/openssl/archive/OpenSSL_1_0_1f.tar.gz
  tar xvf OpenSSL_1_0_1f.tar.gz

  cd openssl-OpenSSL_1_0_1f/

  CC=afl-clang-fast CXX=afl-clang-fast++ ./config
  make
  cp libcrypto.a ../libcrypto_1_0_1f.a
  cp libssl.a ../libssl_1_0_1f.a

  cd ..
  rm *.tar.gz
fi


# Build with DeepState compiler wrappers
WORKSPACE="${1:-bins}"

mkdir "$WORKSPACE"

printf "\nBuilding test_bn_modexp.cpp with Eclipser (uninstrumented binary)\n"
deepstate-eclipser --compile_test test_bn_modexp.cpp --compiler_args="libcrypto_1_0_2d.a -lgcrypt -lgpg-error" --out_test_name "$WORKSPACE"/bn_modexp

printf "\nBuilding test_bn_modexp.cpp with AFL\n"
deepstate-afl --compile_test test_bn_modexp.cpp --compiler_args="libcrypto_1_0_2d.a -lgcrypt -lgpg-error" --out_test_name "$WORKSPACE"/bn_modexp

printf "\nBuilding test_bn_modexp.cpp with Angora\n"
deepstate-angora --compile_test test_bn_modexp.cpp --compiler_args="libcrypto_1_0_2d.a -lgcrypt -lgpg-error" --out_test_name "$WORKSPACE"/bn_modexp \
	--ignore_calls /usr/lib/x86_64-linux-gnu/libgcrypt.a:libcrypto_1_0_2d.a

######

printf "\nBuilding test_heartbleed.cpp with Eclipser (uninstrumented binary)\n"
deepstate-eclipser --compile_test test_heartbleed.cpp --compiler_args="libssl_1_0_1f.a libcrypto_1_0_1f.a" --out_test_name "$WORKSPACE"/heartbleed

printf "\nBuilding test_heartbleed.cpp with AFL\n"
deepstate-afl --compile_test test_heartbleed.cpp --compiler_args="libssl_1_0_1f.a libcrypto_1_0_1f.a" --out_test_name "$WORKSPACE"/heartbleed

printf "\nBuilding test_heartbleed.cpp with Angora\n"
deepstate-angora --compile_test test_heartbleed.cpp --compiler_args="libssl_1_0_1f.a libcrypto_1_0_1f.a" --out_test_name "$WORKSPACE"/heartbleed \
	--ignore_calls /usr/lib/x86_64-linux-gnu/libssl.a:/usr/lib/x86_64-linux-gnu/libcrypto.a

######

printf "\nBuilding test_bn_sqr.cpp with Eclipser (uninstrumented binary)\n"
deepstate-eclipser --compile_test test_bn_sqr.cpp --compiler_args="libcrypto_1_0_1f.a -lgcrypt -lgpg-error" --out_test_name "$WORKSPACE"/bn_sqr

printf "\nBuilding test_bn_sqr.cpp with AFL\n"
deepstate-afl --compile_test test_bn_sqr.cpp --compiler_args="libssl_1_0_1f.a libcrypto_1_0_1f.a -lgpg-error" --out_test_name "$WORKSPACE"/bn_sqr

printf "\nBuilding test_bn_sqr.cpp with Angora\n"
deepstate-angora --compile_test test_bn_sqr.cpp --compiler_args="libssl_1_0_1f.a libcrypto_1_0_1f.a -lgpg-error" --out_test_name "$WORKSPACE"/bn_sqr \
	--ignore_calls /usr/lib/x86_64-linux-gnu/libssl.a:/usr/lib/x86_64-linux-gnu/libcrypto.a


