#include "unit_test.hpp"

#include <giraffe_diagnostics.hpp>

TEST(GiraffeDiagnostics, Exception) {
  try {
    throw GiraffeException(DiagnosticId::GDL_invalidStartCall, "test");
    FAIL();
  } catch (GiraffeException &e) {
    EXPECT_EQ(e.id(), DiagnosticId::GDL_invalidStartCall);
    std::string expected_what = "A000 - test";
    EXPECT_STREQ(e.what(), expected_what.c_str());
  }
}