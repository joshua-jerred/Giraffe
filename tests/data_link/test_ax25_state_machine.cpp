/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_ax25_state_machine.cpp
 * @brief  Test of the AX.25 state machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "gtest/gtest.h"

#include <ax25_state_machine.hpp>

TEST(Ax25StateMachine, BasicTests) {
  gdl::Ax25StateMachine sm;
  EXPECT_EQ(1, 1);
}