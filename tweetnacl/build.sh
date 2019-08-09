# Builds tweetnacl test harnesses


WORKSPACE="${1:-bins}"
FUZZERS=("afl" "angora" "eclipser")

for fuzzer in "${FUZZERS[@]}"; do
	deepstate-"$fuzzer" --compile_test test_tweetnacl_bug.cpp --out_test_name "$WORKSPACE"/tweetnacl_bug
done
