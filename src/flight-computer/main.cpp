/**
 * @file main.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Defines the entry point/main function of the Flight Computer software.
 * @version 0.1
 * @date 2022-09-20
 * @copyright Copyright (c) 2022
 * @addtogroup flight-computer
 */

#include "flight-runner.h"

/**
 * @brief This is the main function that starts the flight runner. It will
 * return a call to flight.start() which will not return until the flight 
 * computer either shuts down or experiences an error.
 * @return int - The exit status of the flight runner 0, any other value
 * indicates an error.
 * @bug FlightRunner currently only returns 0.
 */ 
int main() {
    FlightRunner flight;
    return flight.start();
}

// File: src/flight-computer/main.cpp