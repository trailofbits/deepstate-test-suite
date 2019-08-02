# deepstate-test-suite

Software libraries using DeepState unit testing for fuzzing/symbex.

## Introduction

This repository contains unit test suites written with the [DeepState](https://github.com/trailofbits/deepstate) testing framework. Many of these tests are derived and converted from various software library sources, and some are from Google's [fuzzer-test-suite](https://github.com/google/fuzzer-test-suite).

This is ideal for developers who want to

* Perform A/B benchmarking with DeepState-powered tools.
* Discover and/or reproduce bugs with DeepState-powered fuzzers / symbolic executors.
* Measure performance and "fuzzer diversity" with ensemble-based fuzzing.

__Note:__ Many of the test harnesses contain unit tests with concrete test vectors. These are preserved from the previous software test suite in order to ensure verification of the operation being tested.

## Build

Ensure that [DeepState](https://github.com/trailofbits/deepstate) is installed, as each build script relies on using frontend engines to compile and instrument test harnesses.

Each directory contains a `build.sh` script that fetches and builds any necessary dependencies, and compiles the available harnesses. For example:

```
$ cd trezor-crypto
$ ./build.sh

# ... instrumented binaries stored in `bins/`
```

Each build produces several instrumented binaries with DeepState, which can then be invoked as so:

```
$ deepstate-afl -i my_seeds -o out ./bins/MyHarness.afl
$ deepstate-angora -i my_seeds -o out ./bins/MyHarness.taint ./bins/MyHarness.fast
```

## Tests

TODO

## Contributions

