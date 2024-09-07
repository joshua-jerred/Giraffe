/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_detection_model.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#include "detection_model.hpp"

#include "unit_test.hpp"

TEST(TestDetectionModel, RuleSetGetters) {
  const auto &ruleSetMap = getPhaseRuleSetMap();

  ASSERT_EQ(ruleSetMap.size(), 4)
      << "If more RuleSets are made, they should be added to this unit test.";

  // Iterate through the rule sets
  size_t ruleSetSize = 0;
  bool first = true;
  for (const auto &item : ruleSetMap) {
    FlightPhase phase = item.first;
    const PhaseRuleSet &ruleSet = item.second;

    if (!first) {
      EXPECT_EQ(ruleSetSize, ruleSet.size())
          << "Some rule-sets are missing items.";
    } else {
      first = false;
    }

    ruleSetSize = ruleSet.size();
  }
}
