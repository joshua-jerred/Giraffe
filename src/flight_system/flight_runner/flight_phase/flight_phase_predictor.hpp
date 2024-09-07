/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_phase_predictor.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "detection_data.hpp"
#include "detection_model.hpp"
#include "flight_phase.hpp"
#include "shared_data.hpp"

/// @brief Uses data provided by DetectionData to predict the current flight
/// phase.
class FlightPhasePredictor {
public:
  /// @brief A stucture that contains the probabilities of being in each phase.
  /// The probabilities range from 0 to 255, where 0 is 0% and 255 is 100%.
  struct Probability {
    double launch = 0.0;
    double ascent = 0.0;
    double descent = 0.0;
    double recovery = 0.0;
  };

  using Parameter = DetectionData::Parameter;

  FlightPhasePredictor(data::SharedData &shared_data)
      : shared_data_(shared_data), detection_data_(shared_data) {
  }

  ~FlightPhasePredictor() = default;

  /// @brief Update the flight phase predictor.
  /// @return \c true if the flight phase has changed, \c false if it has stayed
  /// the same.
  bool update() {
    // Update and validate the parameters.
    detection_data_.updateParameters();

    // Process all rules and calculate the probabilities.
    processAllRules();

    // Process the probabilities and predict the flight phase.
    bool change = predictFlightPhase();

    shared_data_.flight_data.setPhasePrediction(
        current_phase_, phase_probability_.launch, phase_probability_.ascent,
        phase_probability_.descent, phase_probability_.recovery);

    return change;
  }

  /// @brief Get the phase probabilities structure.
  Probability getPhaseProbability() const {
    return phase_probability_;
  }

private:
  /// @brief Get the probability points from a rule.
  double getPointsFromRule(const Rule &parameter_rule);

  /// @brief Get the probability points from a rule set.
  /// @param phase_rule_set - The rule set to get the points from.
  /// @return The points from the rule set.
  double getPointsFromRuleSet(const PhaseRuleSet &phase_rule_set);

  /// @brief Process all rule sets of every phase and calculate the
  /// probabilities.
  void processAllRules();

  /// @brief Using the probabilities, determine the current flight phase.
  /// @return \c true if the phase was changed, \c false if it stayed the same.
  bool predictFlightPhase();

  /// @brief The last detected flight phase.
  FlightPhase current_phase_{FlightPhase::UNKNOWN};

  /// @brief The last calculated probabilities of being in each phase.
  Probability phase_probability_{};

  data::SharedData &shared_data_;

  DetectionData detection_data_;
};