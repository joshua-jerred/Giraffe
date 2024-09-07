/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   detection_model.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <functional>
#include <map>
#include <vector>

#include "prediction_parameters.hpp"

/// @brief A structure to contain the rules for adding points to a phase with
/// a given parameter id.
struct Rule {
  PredictionParameters::Parameter::Id id;
  double points;
  std::function<bool(double)> filter;
};

/// @brief A set of rules for a phase.
typedef std::vector<Rule> PhaseRuleSet;

/// @brief The map that contains all rules for every phase.
typedef std::map<FlightPhase, PhaseRuleSet> PhaseRuleSetMap;

/// @brief A function to get the rules for a given phase.
const PhaseRuleSet &getRulesFromModel(FlightPhase phase);

/// @brief A function to get the entire rules map.
const PhaseRuleSetMap &getPhaseRuleSetMap();