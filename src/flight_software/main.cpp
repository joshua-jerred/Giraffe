/**
 * @file main.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Defines the entry point/main function of the Flight Computer software.
 * @version 0.1
 * @date 2022-09-20
 * @copyright Copyright (c) 2022
 */

#include <csignal>

#include "flight_runner.hpp"

FlightRunner flight;

auto signalHandler(int signal_number) -> void {
    signal(SIGINT, signalHandler);
    if (signal_number == SIGINT) {
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
auto main() -> int {
    signal(SIGINT, signalHandler); // Register signal handler
    return flight.start();
}