/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   giraffe_data_link.cpp
 * @brief  The main class for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#include <giraffe_data_link.hpp>
#include <giraffe_exception.hpp>

namespace gdl {
GiraffeDataLink::GiraffeDataLink(GdlConfig config) : config_(config) {
}

GiraffeDataLink::~GiraffeDataLink() {
}

GiraffeDataLink::Status GiraffeDataLink::getStatus() const {
  return status_;
}

void GiraffeDataLink::start() {
  if (status_ != Status::STOPPED) {
    throw GiraffeException(DiagnosticId::GDL_invalidStartCall);
  }
}

} // namespace gdl