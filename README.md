# deepstate-test-suite

Testbed for DeepState target libraries - contains open-sourced examples and continuous integration build

## Introduction

Automated continuous testing is a practice many security-minded folks have started to integrate into their development and/or bug-finding cycle, and has seen adoption in tandem with containerized microservices. This repository is a collection of unit testing artifacts written in [DeepState](https://github.com/trailofbits/deepstate), with the goal of being able to provision and deploy containerized testing instances for tests locally or in the cloud. Some of these tests are original, while some are converted over from [fuzzer-test-suite](https://github.com/google/fuzzer-test-suite.git).

This is ideal for developers who want to:

* Perform A/B benchmarking with DeepState-powered tools.
* Discover and/or reproduce bugs with DeepState analysis executors.
* Measure performance and "fuzzer diversity" with ensemble-based fuzzing.

## Available Tests

| Test Suite    | Number of Tests |
|---------------|-----------------|
| openssl       | 3				  |
| trezor-crypto | 3				  |
| tweetnacl     | 1				  |
| json          | 1				  |


## Usage

- `/tests`

Default testbed of pre-existing fuzzing targets and artifacts.

## Contributions

Contributions are welcome! Please make a pull request for any improvements to the existing test suites, or for any new tests you would like to contribute!
