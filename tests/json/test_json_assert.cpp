/*
 * test_json_assert.cpp
 *
 * 	 Test case finds edge cases where performing two serializations on one input
 *	 result in a failure condition.
 *
 *	 Derived from: https://github.com/google/fuzzer-test-suite/tree/master/json-2017-02-12
*/

#include <iostream>
#include <string>
#include <sstream>
#include <json.hpp>

#include <deepstate/DeepState.hpp>

using json = nlohmann::json;
using namespace deepstate;

#define MAXLEN 1000

TEST(JSON, AssertFail) {

    unsigned char * data = (unsigned char *) DeepState_CStrUpToLen(MAXLEN);
    size_t size = strlen((char *) data);

    try {
        // step 1: parse input
        json j1 = json::parse(data, data + size);

        try {
            // step 2: round trip

            // first serialization
            std::string s1 = j1.dump();

            // parse serialization
            json j2 = json::parse(s1);

            // second serialization
            std::string s2 = j2.dump();

            // serializations must match
            ASSERT_EQ(s1, s2);
        }
        catch (const std::invalid_argument&) {
            LOG(ERROR) << "Parsing returned std::invalid_argument exception";
            ASSERT(false);
        }
    }
    catch (const std::invalid_argument&) {
        // parse errors are ok, because input may be random bytes
    }
}
