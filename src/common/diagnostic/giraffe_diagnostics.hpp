/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_diagnostics.hpp
 * @brief  Giraffe's diagnostic system
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef GIRAFFE_DIAGNOSTICS_HPP_
#define GIRAFFE_DIAGNOSTICS_HPP_

#include "giraffe_diagnostic_ids.hpp"
#include "giraffe_exception.hpp"

std::string diagnosticIdToString(const DiagnosticId diagnostic_id);

#endif /* GIRAFFE_DIAGNOSTICS_HPP_ */
