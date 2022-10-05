#ifndef TEST_EXT_H_
#define TEST_EXT_H_

#include "utility-config-types.h"

#include "extension.h"

class TestExtension : public Extension {
 public:
  TestExtension(DataStream *pDS, ExtensionMetadata config_data);
  ~TestExtension();
  int runner();
};

class BMP180_SIM : public Extension {
 public:
  BMP180_SIM(DataStream *pDS, ExtensionMetadata config_data);
  ~BMP180_SIM();
  int runner();
};

class SAMM8Q_SIM : public Extension {
 public:
  SAMM8Q_SIM(DataStream *pDS, ExtensionMetadata config_data);
  ~SAMM8Q_SIM();
  int runner();
};

class DS18B20_SIM :public Extension {
  public:
  DS18B20_SIM(DataStream *pDS, ExtensionMetadata config_data);
  ~DS18B20_SIM();
  int runner();
};

class DRA818V_SIM :public Extension {
  public:
  DRA818V_SIM(DataStream *pDS, ExtensionMetadata config_data);
  ~DRA818V_SIM();
  int runner();
};

#endif