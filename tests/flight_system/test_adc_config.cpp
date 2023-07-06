/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_adc_config.cpp
 * @brief  Simple test of the ADC extra args config parser.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-05
 * @copyright  2023 (license to be defined)
 */

#include <array>

#include "unit_test.hpp"

#include "adc_configuration.hpp"

using namespace extension;

TEST(AdcConfig, EmptyStringFailure) {
  std::string empty_string{""};
  AdcConfig config;
  EXPECT_TRUE(parseAdcConfig(empty_string, config));
  EXPECT_EQ(config.type, extension::AdcType::RAW_COUNT);
}

TEST(AdcConfig, InvalidTypes) {
  std::array<std::string, 4> types{"invalid", "type=raw_count", "=raw",
                                   "type="};
  AdcConfig config;
  for (auto &type : types) {
    EXPECT_FALSE(parseAdcConfig(type, config)) << "type: " << type;
    EXPECT_EQ(config.type, extension::AdcType::RAW_COUNT);
  }
}

TEST(AdcConfig, ValidMinimum) {
  AdcConfig config;
  std::string raw = "type=raw";
  std::string percent = "type=percent";

  EXPECT_TRUE(parseAdcConfig(raw, config));
  EXPECT_EQ(config.type, extension::AdcType::RAW_COUNT);

  EXPECT_TRUE(parseAdcConfig(percent, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
}

TEST(AdcConfig, ValidPercent) {
  std::string p1 = "type=percent";
  std::string p2 = "type=percent,min=10";
  std::string p3 = "type=percent,max=100";
  std::string p4 = "type=percent,min=11,max=100";
  std::string p5 = "type=percent,min=52,max=10000,clamp=true";
  std::string p6 = "type=percent,min=0,max=100,clamp=false";

  AdcConfig config;
  EXPECT_TRUE(parseAdcConfig(p1, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 0);
  EXPECT_EQ(config.max, 4095);

  EXPECT_TRUE(parseAdcConfig(p2, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 10);
  EXPECT_EQ(config.max, 4095);

  EXPECT_TRUE(parseAdcConfig(p3, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 0);
  EXPECT_EQ(config.max, 100);

  EXPECT_TRUE(parseAdcConfig(p4, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 11);
  EXPECT_EQ(config.max, 100);
  EXPECT_FALSE(config.clamp);

  EXPECT_TRUE(parseAdcConfig(p5, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 52);
  EXPECT_EQ(config.max, 10000);
  EXPECT_TRUE(config.clamp);

  EXPECT_TRUE(parseAdcConfig(p6, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 0);
  EXPECT_EQ(config.max, 100);
  EXPECT_FALSE(config.clamp);
}

TEST(AdcTest, percentageResolution) {
  std::string p1 = "type=percent";
  AdcConfig config;
  config.resolution = 12;
  EXPECT_TRUE(parseAdcConfig(p1, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 0);
  EXPECT_EQ(config.max, 4095);

  config.resolution = 10;
  EXPECT_TRUE(parseAdcConfig(p1, config));
  EXPECT_EQ(config.type, extension::AdcType::PERCENTAGE);
  EXPECT_EQ(config.min, 0);
  EXPECT_EQ(config.max, 1023);
}

TEST(AdcTest, VoltageDividerValid) {
  std::string v1 = "type=v_div,r1=1,r2=3";
  std::string v2 = "type=v_div,r1=10000,r2=100000,ref=5000";

  AdcConfig config;
  EXPECT_TRUE(parseAdcConfig(v1, config));
  EXPECT_EQ(config.type, extension::AdcType::VOLTAGE_DIVIDER);
  EXPECT_EQ(config.resistor_1, 1);
  EXPECT_EQ(config.resistor_2, 3);
  EXPECT_EQ(config.voltage_reference, 3300);

  EXPECT_TRUE(parseAdcConfig(v2, config));
  EXPECT_EQ(config.type, extension::AdcType::VOLTAGE_DIVIDER);
  EXPECT_EQ(config.resistor_1, 10000);
  EXPECT_EQ(config.resistor_2, 100000);
  EXPECT_EQ(config.voltage_reference, 5000);
}

TEST(AdcTest, InvalidConfigs) {
  std::array<std::string, 10> types{"type=percent,min=-10,max=100",
                                    "type=percent,min=10,max=-100",
                                    "type=percent,min=10,max=10",
                                    "type=percent,min=100,max=10",
                                    "type=v_div,r1=0,r2=1",
                                    "type=v_div,r1=1,r2=0",
                                    "type=v_div,r1=0,r2=0",
                                    "type=v_div,r1=1",
                                    "type=v_div,r2=1",
                                    "type=v_div,r1=1,r2=1,ref=0"};

  AdcConfig config;
  for (auto &type : types) {
    EXPECT_FALSE(parseAdcConfig(type, config)) << "cfg_str: " << type;
    EXPECT_EQ(config.type, extension::AdcType::RAW_COUNT);
  }
}

TEST(AdcTest, VoltageReference) {
  std::string v1 = "type=v_ref,ref=5000";
  std::string v2 = "type=v_ref,ref=3500";
  std::string v3 = "type=v_ref";

  AdcConfig config;
  EXPECT_TRUE(parseAdcConfig(v1, config));
  EXPECT_EQ(config.type, extension::AdcType::VOLTAGE_REFERENCE);
  EXPECT_EQ(config.voltage_reference, 5000);

  EXPECT_TRUE(parseAdcConfig(v2, config));
  EXPECT_EQ(config.type, extension::AdcType::VOLTAGE_REFERENCE);
  EXPECT_EQ(config.voltage_reference, 3500);

  EXPECT_TRUE(parseAdcConfig(v3, config));
  EXPECT_EQ(config.type, extension::AdcType::VOLTAGE_REFERENCE);
  EXPECT_EQ(config.voltage_reference, 3300);
}