#include <drlojekyll/Display/DisplayConfiguration.h>
#include <drlojekyll/Display/DisplayManager.h>
#include <drlojekyll/Parse/ErrorLog.h>
#include <drlojekyll/Parse/Parser.h>
#include <drlojekyll/Parse/Format.h>

#include <deepstate/DeepState.hpp>

using namespace deepstate;

// TODO: symbolic tests
// TODO: boring tests
// TODO: "auxiliary helper" for seed input -> valid tokens


class DrLoJekyll: public Test {
  public:

    hyde::DisplayManager dm;
    hyde::ErrorLog el;

    hyde::DisplayConfiguration config = {
      "deepstate_fuzz", 2, true
    };

    DrLoJekyll() {
      hyde::Parser parser(dm, el);
    }

    void SetUp(void) {
      LOG(TRACE) << "Initializing parser";
    }

    void TearDown(void) {
      LOG(TRACE) << "Cleaning up";
    }
};


TEST_F(DrLoJekyll, ParseAndVerify) {
  char *input = DeepState_CStr(8192);
  auto module = parser.ParseBuffer(input, config);
}


TEST_F(DrLoJekyll, ParseAndVerifyFile) {
    const char * path = DeepState_InputPath(NULL);
    auto module = parser.ParsePath(path, config);
}


TEST_F(DrLoJekyll, ParseAndVerifyStream) {
  char *input = DeepState_CStr(8192);
  auto module = parser.ParseBuffer(input, config);

  std::stringstream format_stream;
  std::stringstream verify_stream;

  hyde::OutputStream os(dm, format_stream);
  hyde::FormatModule(os, module);

  hyde::OutputStream os2(dm, verify_stream);
  hyde::FormatModule(os2, module2);

  el.Render(std::cerr);
  ASSERT(el.IsEmpty());
  ASSERT_EQ(verify_stream.str(), format_stream_str);
}
