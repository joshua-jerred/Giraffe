/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   segmenter.cpp
 * @brief  Segmenter State Machine
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include "segmenter.hpp"

namespace ax25 {

void SegmenterStateMachine::state_ready() {
  // SR - DL-DATA request

  // SR - DL-UNIT-DATA indication

  // SR - all other DL primitives
}

} // namespace ax25