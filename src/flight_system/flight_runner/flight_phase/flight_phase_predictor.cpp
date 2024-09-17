/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_phase_predictor.cpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#include "flight_phase_predictor.hpp"

/// @brief Update the flight phase predictor.
/// @return \c true if the flight phase has changed, \c false if it has stayed
/// the same.
bool FlightPhasePredictor::update() {
  // Update and validate the parameters.
  detection_data_.updateParameters();

  // Process all rules and calculate the probabilities.
  processAllRules();

  // Process the probabilities and predict the flight phase. Return true if
  // the phase has changed.
  return predictFlightPhase();
}

double FlightPhasePredictor::getPointsFromRule(const Rule &parameter_rule) {
  const Parameter param = detection_data_.getParameter(parameter_rule.id);

  num_total_parameters_++;

  // If the data is not valid, or the filter fails, return 0 points.
  if (!param.is_valid) {
    num_invalid_parameters_++;
    return 0;
  } else if (!parameter_rule.filter(param.value)) {
    // num_invalid_parameters_++;
    return 0;
  }

  return parameter_rule.points;
}

double
FlightPhasePredictor::getPointsFromRuleSet(const PhaseRuleSet &phase_rule_set) {
  double points = 0.0;
  for (const auto &rule : phase_rule_set) {
    points += getPointsFromRule(rule);
  }
  return points;
}

void FlightPhasePredictor::processAllRules() {
  Probability new_probability{};
  num_total_parameters_ = 0;
  num_invalid_parameters_ = 0;

  for (const auto &items : getPhaseRuleSetMap()) {
    const FlightPhase phase = items.first;
    const PhaseRuleSet &phase_rule_set = items.second;

    // Get the points from the rule set and set the probability.
    const double phase_points = getPointsFromRuleSet(phase_rule_set);

    switch (phase) {
    case FlightPhase::LAUNCH:
      new_probability.launch = phase_points;
      break;
    case FlightPhase::ASCENT:
      new_probability.ascent = phase_points;
      break;
    case FlightPhase::DESCENT:
      new_probability.descent = phase_points;
      break;
    case FlightPhase::RECOVERY:
      new_probability.recovery = phase_points;
      break;
    default:
      giraffe_assert(false);
      return;
    }
  }

  // Calculate the data quality.
  new_probability.data_quality =
      (1.0 - (num_invalid_parameters_ / num_total_parameters_)) * 100.0;

  phase_probability_ = new_probability;
}

bool FlightPhasePredictor::predictFlightPhase() {
  if (phase_probability_.data_quality < MINIMUM_PROBABILITY_QUALITY) {
    predicted_phase_ = FlightPhase::UNKNOWN;
    return false;
  }

  FlightPhase new_phase = FlightPhase::UNKNOWN;

  auto &prob = phase_probability_;

  // Determine the new phase.
  if (prob.launch > prob.ascent && prob.launch > prob.descent &&
      prob.launch > prob.recovery) {
    new_phase = FlightPhase::LAUNCH;
  } else if (prob.ascent > prob.launch && prob.ascent > prob.descent &&
             prob.ascent > prob.recovery) {
    new_phase = FlightPhase::ASCENT;
  } else if (prob.descent > prob.launch && prob.descent > prob.ascent &&
             prob.descent > prob.recovery) {
    new_phase = FlightPhase::DESCENT;
  } else if (prob.recovery > prob.launch && prob.recovery > prob.ascent &&
             prob.recovery > prob.descent) {
    new_phase = FlightPhase::RECOVERY;
  } else {
    new_phase = FlightPhase::UNKNOWN;
  }

  bool phase_changed = new_phase != predicted_phase_;

  // Set the new phase.
  predicted_phase_ = new_phase;

  return phase_changed;
}
