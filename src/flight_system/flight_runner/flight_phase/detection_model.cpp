/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   detection_model.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#include <functional>
#include <map>
#include <vector>

#include "detection_model.hpp"

using Id = DetectionData::Parameter::Id;

static const PhaseRuleSet EMPTY_RULES = {};

static const PhaseRuleSet LAUNCH_RULES = {
    {Id::DISTANCE_FROM_LAUNCH, 40, [](double val) { return val < 10; }},
    {Id::DISTANCE_FROM_GROUND, 20, [](double val) { return val < 5; }},
    {Id::GPS_HORIZONTAL_SPEED, 10, [](double val) { return val < 2.5; }},
    {Id::GPS_HORIZONTAL_SPEED_1MIN, 10, [](double val) { return val < 2.5; }},
    {Id::GPS_VERTICAL_SPEED, 10, [](double val) { return val < 1; }},
    {Id::GPS_VERTICAL_SPEED_1MIN, 10, [](double val) { return val < 1; }},
};

static const PhaseRuleSet ASCENT_RULES = {
    {Id::DISTANCE_FROM_LAUNCH, 20, [](double val) { return val > 10; }},
    {Id::DISTANCE_FROM_GROUND, 15, [](double val) { return val > 5; }},
    {Id::GPS_HORIZONTAL_SPEED, 10, [](double val) { return val > 2.5; }},
    {Id::GPS_HORIZONTAL_SPEED_1MIN, 0, [](double val) { return val > 2.5; }},
    {Id::GPS_VERTICAL_SPEED, 30, [](double val) { return val > 1; }},
    {Id::GPS_VERTICAL_SPEED_1MIN, 5, [](double val) { return val > 1; }},
};

static const PhaseRuleSet DESCENT_RULES = {
    {Id::DISTANCE_FROM_LAUNCH, 10, [](double val) { return val > 10; }},
    {Id::DISTANCE_FROM_GROUND, 10, [](double val) { return val > 1500; }},
    {Id::GPS_HORIZONTAL_SPEED, 10, [](double val) { return val > 0.5; }},
    {Id::GPS_HORIZONTAL_SPEED_1MIN, 10, [](double val) { return val > 0.5; }},
    {Id::GPS_VERTICAL_SPEED, 50, [](double val) { return val < -1; }},
    {Id::GPS_VERTICAL_SPEED_1MIN, 10, [](double val) { return val < -1; }},
};

static const PhaseRuleSet RECOVERY_RULES = {
    {Id::DISTANCE_FROM_LAUNCH, 10, [](double val) { return val > 10; }},
    {Id::DISTANCE_FROM_GROUND, 10, [](double val) { return val > 5; }},
    {Id::GPS_HORIZONTAL_SPEED, 15, [](double val) { return val < 1; }},
    {Id::GPS_HORIZONTAL_SPEED_1MIN, 15, [](double val) { return val < 1; }},
    {Id::GPS_VERTICAL_SPEED, 20, [](double val) { return val < 1; }},
    {Id::GPS_VERTICAL_SPEED_1MIN, 30, [](double val) { return val < 1; }},
};

static const std::map<FlightPhase, PhaseRuleSet> RULES_MAP = {
    {FlightPhase::LAUNCH, LAUNCH_RULES},
    {FlightPhase::ASCENT, ASCENT_RULES},
    {FlightPhase::DESCENT, DESCENT_RULES},
    {FlightPhase::RECOVERY, RECOVERY_RULES},
};

const PhaseRuleSet &getRulesFromModel(FlightPhase phase) {
  auto it = RULES_MAP.find(phase);
  if (it != RULES_MAP.end()) {
    return it->second;
  }

  return EMPTY_RULES;
};

const PhaseRuleSetMap &getPhaseRuleSetMap() {
  return RULES_MAP;
}