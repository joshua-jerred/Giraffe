/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   external_data_dictionary.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-02
/// @copyright  2025 (license to be defined)

#pragma once

#include <array>
#include <cstdint>

namespace power_board {

class DataDictionary {
public:
  enum class EntryId : uint16_t {
    UNKNOWN = 0,
    VERSION_INFO = 1,
    SYSTEM_STATUS = 2,
    GPS_STATUS = 3,

    _NUM_ENTRIES
  };

  DataDictionary() = default;
  ~DataDictionary() = default;

  uint32_t getValue(EntryId id) const {
    if (id >= EntryId::_NUM_ENTRIES) {
      return 0;
    }

    return entries_[static_cast<size_t>(id)].value;
  }

private:
  enum class EntryType : uint8_t {
    UNKNOWN = 0,
    BOOL = 1,
    UINT8 = 2,
  };

  struct Entry {
    EntryId id;
    EntryType type;

    // All values are stored as uint32_t
    uint32_t value;
  };

  std::array<EntryType, static_cast<size_t>(ENTRY_ID::_NUM_ENTRIES)>
      entry_types_ = {
          EntryType::UNKNOWN,
          EntryType::UINT8,
          EntryType::UINT8,
          EntryType::UINT8,
  };
};

} // namespace power_board