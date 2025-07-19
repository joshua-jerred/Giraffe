/**
 * @file main.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Defines the entry point/main function of the Flight Computer software.
 * @version 0.1
 * @date 2022-09-20
 * @copyright Copyright (c) 2022
 */

// Suppress exceptions from BoosterSeat
//#define BOOSTERSEAT_NO_EXCEPTION

#include <csignal>

#include "flight_runner.hpp"
#include "giraffe_assert.hpp"

#if RUN_IN_SIMULATOR == 1
#include "gfs_simulator.hpp"
gfs_sim::GfsSimulator g_GFS_SIMULATOR{};

static FlightRunner flight{&g_GFS_SIMULATOR};
#else
static FlightRunner flight{};
#endif

auto signalHandler(int signal_number) -> void {
  signal(SIGINT, signalHandler);
  if (signal_number == SIGINT || signal_number == SIGTERM) {
    std::cout << "received shutdown signal" << std::endl;

#if RUN_IN_SIMULATOR == 1
    g_GFS_SIMULATOR.stop();
    std::cout << "simulator stopped" << std::endl;
#endif

    flight.shutdown();
  }
}

/**
 * @brief This is the main function that starts the flight runner. It will
 * return a call to flight.start() which will not return until the flight
 * computer either shuts down or experiences an error.
 * @return int - The exit status of the flight runner 0, any other value
 * indicates an error.
 * @todo update docs
 * @bug FlightRunner currently only returns 0.
 */
int main() {
  signal(SIGINT, signalHandler);  // Register signal handler
  signal(SIGTERM, signalHandler); // Register signal handler for termination

#if RUN_IN_SIMULATOR == 1
  g_GFS_SIMULATOR.start();
#endif

  return flight.start(); // Return the exit code from flight.start()
}