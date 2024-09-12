/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   flight_phase.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-09-07
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include <BoosterSeat/filters.hpp>
#include <BoosterSeat/timer.hpp>

#include "flight_phase_predictor.hpp"
#include "shared_data.hpp"

/// @brief This class manages everything related to flight phases and
/// sequencing.
class FlightPhaseManager {
public:
  using Probability = FlightPhasePredictor::Probability;

  /// @brief Constructor.
  /// @param shared_data - The shared data object.
  /// @param update_rate_ms - The rate at which the predicted flight phase
  /// should be updated. Calls to update() will need to be at or faster than
  /// this rate. The actual update rate is update_rate_ms * filter_threshold.
  /// @param filter_threshold - The threshold for the phase filter.
  FlightPhaseManager(data::SharedData &shared_data, size_t update_rate_ms = 250,
                     size_t filter_threshold = 10)
      : phase_predictor_(shared_data),
        UPDATE_RATE_(update_rate_ms), PHASE_FILTER_THRESHOLD_{
                                          filter_threshold} {
  }

  /// @brief Destructor.
  ~FlightPhaseManager() = default;

  /// @brief Update the flight phase manager.
  void update() {
    if (!phase_update_timer_.isDone()) {
      return;
    }

    FlightPhase predicted_phase = phase_predictor_.getPredictedPhase();
    Probability probability = phase_predictor_.getPhaseProbability();

    phase_filter_.addValue(predicted_phase);
  }

  /// @brief Get the current flight phase. This is the phase that the entire
  /// system will use.
  /// @return The current flight phase.
  FlightPhase getCurrentFlightPhase() const {
    return flight_phase_;
  }

private:
  /// @brief The current flight phase. Filtered and continually checked.
  FlightPhase flight_phase_ = FlightPhase::UNKNOWN;

  /// @brief The phase predictor. It generates the predicted phase along with a
  /// probability value for each phase.
  FlightPhasePredictor phase_predictor_;

  /// @brief The rate at which the predicted flight phase should be updated.
  const size_t UPDATE_RATE_;

  /// @brief The threshold for the phase filter.
  const size_t PHASE_FILTER_THRESHOLD_;

  /// @brief The filter for the flight phase. Filters out intermittent phase
  /// changes.
  bst::ConsecutiveValuesFilter<FlightPhase> phase_filter_{
      FlightPhase::UNKNOWN, PHASE_FILTER_THRESHOLD_};

  /// @brief The timer for updating the phase.
  bst::Timer phase_update_timer_{static_cast<int>(UPDATE_RATE_)};
};