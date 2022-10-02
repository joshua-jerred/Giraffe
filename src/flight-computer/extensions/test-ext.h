#ifndef TEST_EXT_H_
#define TEST_EXT_H_

#include "utility-config-types.h"

#include "extension.h"

class TestExtension : public Extension {
 public:
  TestExtension(DataStream *pDS, ExtensionMetadata config_data);
  ~TestExtension();
  int runner(std::atomic<bool> stop_flag);
};

#endif