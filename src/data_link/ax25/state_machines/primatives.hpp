/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   primatives.hpp
 * @brief  Primatives from the AX.25 standard
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef AX25_PRIMATIVES_HPP_
#define AX25_PRIMATIVES_HPP_

namespace ax25 {

enum class Primitive {
  // PH = Physical Layer
  PH_SEIZE,
  PH_DATA,
  PH_RELEASE,
  PH_EXPEDITED_DATA,
  PH_BUSY,
  PH_QUIET,
  // DL = Data Link Layer
  DL_CONNECT,
  DL_DISCONNECT,
  DL_DATA,
  DL_UNIT_DATA,
  DL_ERROR,
  DL_FLOW_ON,
  DL_FLOW_OFF,
  LM_SEIZE,
  LM_RELEASE,
  LM_DATA,
  LM_EXPEDITED_DATA
};

enum class PrimitiveAction {
  INDICATION,
  REQUEST,
  CONFIRM,
};

} // namespace ax25

#endif /* AX25_PRIMATIVES_HPP_ */