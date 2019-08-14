# Test Suite Tools

Contains extraneous tools, scripts, etc. that can be used to supplement analysis. These were things we used to help guide some of the debugging when bug-hunting and benchmarking, but can also be used for general-purpose reasons.


* `crash_analysis.py` is a Manticore script that provides instruction traces in order to debug specific points where a program may crash.

* (TODO) `bin_info.py` is a Manticore script that provides program insight through dynamic binary analysis.

* `abilist.txt` is a generated file used by Angora to blackbox functions from static libraries during data-flow and taint analysis.

* (TODO) `deploy.py` is a script used to generate fuzzer-ready containers for benchmarking.

