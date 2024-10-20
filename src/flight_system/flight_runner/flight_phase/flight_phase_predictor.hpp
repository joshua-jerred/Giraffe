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

#include "flight_phase.hpp"
#include "prediction_model.hpp"
#include "prediction_parameters.hpp"
#include "shared_data.hpp"

/// @brief Uses data provided by PredictionParameters to predict the current
/// flight phase.
class FlightPhasePredictor {
public:
  static constexpr double MINIMUM_PROBABILITY_QUALITY = 50.0;

  /// @brief A stucture that contains the probabilities of being in each phase.
  /// The probabilities range from 0 to 100.
  struct Probability {
    /// @brief The probability of being in the launch phase. 0 to 100.
    double launch = 0.0;
    /// @brief The probability of being in the ascent phase. 0 to 100.
    double ascent = 0.0;
    /// @brief The probability of being in the descent phase. 0 to 100.
    double descent = 0.0;
    /// @brief The probability of being in the recovery phase. 0 to 100.
    double recovery = 0.0;

    // @brief A value from 0 to 100 that represents the quality
    double data_quality = 0.0;
  };

  using Parameter = PredictionParameters::Parameter;

  FlightPhasePredictor(data::SharedData &shared_data)
      : detection_data_(shared_data) {
  }
  ~FlightPhasePredictor() = default;
  /// @brief Delete the copy constructor.
  FlightPhasePredictor(const FlightPhasePredictor &) = delete;
  /// @brief Delete the copy assignment operator.
  FlightPhasePredictor &operator=(const FlightPhasePredictor &) = delete;
  /// @brief Delete the move constructor.
  FlightPhasePredictor(FlightPhasePredictor &&) = delete;

  /// @brief Update the flight phase predictor.
  /// @return \c true if the flight phase has changed, \c false if it has stayed
  /// the same.
  bool update();

  /// @brief Get the phase probabilities structure.
  Probability getPhaseProbability() const {
    return phase_probability_;
  }

  /// @brief Get the predicted flight phase.
  FlightPhase getPredictedPhase() const {
    return predicted_phase_;
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
  FlightPhase predicted_phase_{FlightPhase::UNKNOWN};

  /// @brief The last calculated probabilities of being in each phase.
  Probability phase_probability_{};

  /// @brief The number of parameters that exist.
  double num_total_parameters_{0};

  /// @brief The number of valid parameters.
  double num_invalid_parameters_{0};

  PredictionParameters detection_data_;
};