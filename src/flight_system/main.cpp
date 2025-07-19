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

#else
// FlightRunner flight{};
#endif

auto signalHandler(int signal_number) -> void {
  signal(SIGINT, signalHandler);
  if (signal_number == SIGINT) {

#if RUN_IN_SIMULATOR == 1
    g_GFS_SIMULATOR.stop();
#endif

    // flight.shutdown();
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
  signal(SIGINT, signalHandler); // Register signal handler

#if RUN_IN_SIMULATOR == 1
  g_GFS_SIMULATOR.start();
  FlightRunner flight{&g_GFS_SIMULATOR};
#else
  FlightRunner flight{};
#endif
  int exit_code = -1;
  try {
    exit_code = flight.start();
  } catch (const std::exception &e) {
    std::cerr << "GFS UNCAUGHT EXCEPTION: " << e.what() << std::endl;
    return 1; // Return a non-zero exit code to indicate an error
  }

  return exit_code; // Return the exit code from flight.start()
}