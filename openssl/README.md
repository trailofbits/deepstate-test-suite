# openssl

> Original source: https://github.com/google/fuzzer-test-suite/

| Test                  | Version          | Description                                                       | Known Vulnerabilities |   |
|-----------------------|------------------|-------------------------------------------------------------------|-----------------------|---|
| `test_bn_modexp.cpp`  | `openssl-1.0.2d` | Differential test for reproducing miscalculation in `BN_mod_exp`  | CVE-2015-3193         |   |
| `test_heartbleed.cpp` | `openssl-1.0.1f` | Test for reproducing Heartbleed heap overflow vulnerability       | CVE-2014-0160         |   |
|                       |                  |                                                                   |                       |   |
