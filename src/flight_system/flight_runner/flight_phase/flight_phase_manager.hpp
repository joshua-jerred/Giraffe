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

#include "flight_data.hpp"
#include "flight_phase_predictor.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

/// @brief This class manages everything related to flight phases and
/// sequencing.
class FlightPhaseManager {
public:
  using Probability = FlightPhasePredictor::Probability;

  /// @brief Constructor.
  /// @param shared_data - The shared data object.
  /// @param initial_phase - The initial phase of the flight. Loaded from
  /// FlightRunnerData at startup. Defaults to UNKNOWN as a safety measure.
  /// @param update_rate_ms - The rate at which the predicted flight phase
  /// should be updated. Calls to update() will need to be at or faster than
  /// this rate. The actual update rate is update_rate_ms * filter_threshold.
  /// @param filter_threshold - The threshold for the phase filter.
  FlightPhaseManager(data::SharedData &shared_data,
                     FlightPhase initial_phase = FlightPhase::UNKNOWN,
                     size_t update_rate_ms = 250, size_t filter_threshold = 10)
      : phase_predictor_(shared_data), UPDATE_RATE_(update_rate_ms),
        PHASE_FILTER_THRESHOLD_{filter_threshold}, shared_data_{shared_data} {

    // Edge case: We don't want to get stuck in the launch phase after an
    // erroneous restart. This may be annoying, but it is safer.
    if (initial_phase == FlightPhase::LAUNCH) {
      flight_phase_ = FlightPhase::RECOVERY;
    } else {
      flight_phase_ = initial_phase;
    }
  }

  /// @brief Destructor.
  ~FlightPhaseManager() = default;

  /// @brief Update the flight phase manager.
  void update() {
    if (!phase_update_timer_.isDone()) {
      return;
    }

    phase_predictor_.update();

    FlightPhase predicted_phase = phase_predictor_.getPredictedPhase();
    phase_filter_.addValue(predicted_phase);
    flightPhaseUpdate(predicted_phase);

    Probability probability = phase_predictor_.getPhaseProbability();
    shared_data_.flight_data.setPhasePrediction(
        predicted_phase, probability.launch, probability.ascent,
        probability.descent, probability.recovery, probability.data_quality);
  }

  /// @brief Get the current flight phase. This is the phase that the entire
  /// system will use.
  /// @return The current flight phase.
  FlightPhase getCurrentFlightPhase() const {
    return flight_phase_;
  }

  /// @brief Request that the flight phase be changed to launch. Only valid if
  /// the current phase is pre-launch.
  bool requestLaunch() {
    if (flight_phase_ == FlightPhase::PRE_LAUNCH) {
      setFlightPhase(FlightPhase::LAUNCH);
      return true;
    }

    return false;
  }

  /// @brief Set the flight phase to pre-launch.
  /// @details This could be a dangerous operation. It should only be called
  /// via a manual action.
  void setPreLaunch() {
    setFlightPhase(FlightPhase::PRE_LAUNCH);
  }

private:
  void flightPhaseUpdate(FlightPhase new_phase) {
    FlightPhase current_phase = flight_phase_;

    bool change_allowed = true;
    bool unexpected_change = false;

    // Alert when the phase is unknown
    if (new_phase == FlightPhase::UNKNOWN) {
      // Suppress the error if we are in pre-launch.
      if (current_phase == FlightPhase::PRE_LAUNCH) {
        return;
      }

      // Suppress the error if we have already reported it.
      if (unknown_phase_error_reported_) {
        return;
      }

      logger_.error(DiagnosticId::FLIGHT_RUNNER_flightPhaseUnknown);
      unknown_phase_error_reported_ = true;
      return;
    } else {
      unknown_phase_error_reported_ = false;
    }

    // Staying in the same phase.
    if (new_phase == current_phase) {
      phase_valid_watchdog_.reset();
      return;
    }

    static_assert(static_cast<uint8_t>(FlightPhase::NUM_PHASES) == 6,
                  "Update the switch statement");
    switch (current_phase) {
    case FlightPhase::UNKNOWN: {
      // The transition from unknown to launch could be dangerous. It would be
      // safer to go to recovery instead. Edge Cases: Out of Disk Space,
      // FlightSystem restart mid-flight or on recovery.
      if (new_phase == FlightPhase::LAUNCH) {
        change_allowed = false;
      }
    } break;
    case FlightPhase::PRE_LAUNCH: {
      if (new_phase == FlightPhase::LAUNCH) {
        // The transition from pre-launch to launch can only be triggered by the
        // user. If we are set to pre-launch, the phase predictor will report
        // launch.
        phase_valid_watchdog_.reset();
        return;
      }

      // Entering the pre-launch phase is a manual process. However, safety wise
      // we should be able to automatically exit to ascent.
      if (new_phase != FlightPhase::ASCENT) {
        change_allowed = false;
      }

      unexpected_change = true;
    } break;
    case FlightPhase::LAUNCH: {
      // We expect to go to ASCENT.
      if (new_phase != FlightPhase::ASCENT) {
        unexpected_change = true;
      }

      // Allow for transitions forward only.
      if (new_phase != FlightPhase::ASCENT &&
          new_phase != FlightPhase::DESCENT &&
          new_phase != FlightPhase::RECOVERY) {
        change_allowed = false;
      }
    } break;
    case FlightPhase::ASCENT:
      // We expect to go to DESCENT.
      if (new_phase != FlightPhase::DESCENT) {
        unexpected_change = true;
      }

      // Allow for transitions forward only.
      if (new_phase != FlightPhase::DESCENT &&
          new_phase != FlightPhase::RECOVERY) {
        change_allowed = false;
      }
      break;
    case FlightPhase::DESCENT: {
      if (new_phase != FlightPhase::RECOVERY) {
        unexpected_change = true; // We expect to go to RECOVERY.
      }

      // Allow for transition back to ascent or forward to recovery.
      if (new_phase != FlightPhase::ASCENT &&
          new_phase != FlightPhase::RECOVERY) {
        change_allowed = false;
      }
    } break;
    case FlightPhase::RECOVERY: {
      // We expect to stay in RECOVERY.
      unexpected_change = true;

      // Allow for transition back to ascent or descent.
      if (new_phase != FlightPhase::ASCENT &&
          new_phase != FlightPhase::DESCENT) {
        change_allowed = false;
      }
    } break;
    default:
      change_allowed = false;
      break;
    }

    if (!change_allowed) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_phaseChangeInvalid,
                    util::to_string(current_phase) + " -> " +
                        util::to_string(new_phase));
      return;
    }

    if (unexpected_change) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_phaseChangeUnexpected,
                    util::to_string(current_phase) + " -> " +
                        util::to_string(new_phase));
    }

    phase_valid_watchdog_.reset();
    setFlightPhase(new_phase);
  }

  void setFlightPhase(FlightPhase new_phase) {
    flight_phase_ = new_phase;

    // Update the phase in flight_data
    shared_data_.flight_data.setFlightPhase(new_phase);

    // Report the phase change to the system so that other modules can react.
    shared_data_.streams.data.addData(
        node::Identification::FLIGHT_RUNNER,
        data::DataId::FLIGHT_RUNNER_flightPhaseChange,
        util::to_string(new_phase));

    phase_filter_.reset(new_phase);
  }

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

  /// @brief The probability of each phase.
  Probability prob_{};

  /// @brief The timer for updating the phase.
  bst::Timer phase_update_timer_{static_cast<int>(UPDATE_RATE_)};

  constexpr static size_t PHASE_VALID_TIMEOUT_MS = 1000 * 60; // 1 minute
  bst::Timer phase_valid_watchdog_{static_cast<int>(PHASE_VALID_TIMEOUT_MS)};

  data::SharedData &shared_data_;

  giraffe::Logger logger_{shared_data_, node::Identification::FLIGHT_RUNNER,
                          "phase_mgr"};

  bool unknown_phase_error_reported_ = false;
};