/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   test_error_frame.cpp
 * @brief  Test of the error frame.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#include <BoosterSeat/sleep.hpp>

#include "error_frame.hpp"

#include "unit_test.hpp"

DiagnosticId id1 = DiagnosticId::DATA_LOG_appendToDataFile;
DiagnosticId id2 = DiagnosticId::EXTENSION_startCall;
DiagnosticId id3 = DiagnosticId::CONFIG_configFileDoesNotExist;

TEST(ErrorFrameTest, initialState) {
  data::ErrorFrame f{};
  data::ErrorFrameItem item{};
  bst::clck::TimePoint time{};
  EXPECT_EQ(f.numActiveErrors(), 0);
  EXPECT_EQ(f.numTotalErrors(), 0);
  EXPECT_EQ(f.numOccurrences(id1), 0);
  EXPECT_FALSE(f.getError(id1, item));
  EXPECT_EQ(f.getActiveErrorIds().size(), 0);
  EXPECT_FALSE(f.isActive(id1));
  EXPECT_FALSE(f.lastOccurrence(id1, time));
}

TEST(ErrorFrameTest, addSingleError) {
  data::ErrorFrame f{};
  data::ErrorFrameItem item{};
  bst::clck::TimePoint time{};

  data::LogPacket packet{};
  packet.id = id1;
  packet.level = data::LogPacket::Level::ERROR;

  f.addError(packet);

  EXPECT_EQ(f.numActiveErrors(), 1);
  EXPECT_EQ(f.numTotalErrors(), 1);
  EXPECT_EQ(f.numOccurrences(id1), 1);
  EXPECT_TRUE(f.getError(id1, item));

  EXPECT_EQ(item.occurrences, 1);
  EXPECT_EQ(item.packet.id, id1);

  EXPECT_EQ(f.getActiveErrorIds().size(), 1);
  EXPECT_TRUE(f.isActive(id1));
  EXPECT_TRUE(f.numOccurrences(id1));
  EXPECT_TRUE(f.lastOccurrence(id1, time));

  f.addError(packet);
  EXPECT_EQ(f.numActiveErrors(), 1);
  EXPECT_EQ(f.numTotalErrors(), 2);
  EXPECT_EQ(f.numOccurrences(id1), 2);
  EXPECT_TRUE(f.getError(id1, item));

  EXPECT_EQ(item.occurrences, 2);
  EXPECT_EQ(item.packet.id, id1);

  EXPECT_EQ(f.getActiveErrorIds().size(), 1);
  EXPECT_TRUE(f.isActive(id1));
  EXPECT_TRUE(f.lastOccurrence(id1, time));

  for (int i = 2; i < 100; i++) {
    f.addError(packet);
  }

  EXPECT_EQ(f.numActiveErrors(), 1);
  EXPECT_EQ(f.numTotalErrors(), 100);
  EXPECT_EQ(f.numOccurrences(id1), 100);
  EXPECT_TRUE(f.getError(id1, item));
}

TEST(ErrorFrameTest, addMultipleErrors) {
  data::ErrorFrame f{};
  data::ErrorFrameItem item{};
  bst::clck::TimePoint time{};

  data::LogPacket packet{};
  packet.id = id1;
  packet.level = data::LogPacket::Level::ERROR;

  f.addError(packet);
  packet.id = id2;
  f.addError(packet);

  EXPECT_EQ(f.numActiveErrors(), 2);
  EXPECT_EQ(f.numTotalErrors(), 2);
  EXPECT_EQ(f.numOccurrences(id1), 1);
  EXPECT_EQ(f.numOccurrences(id2), 1);
  EXPECT_TRUE(f.getError(id1, item));
  EXPECT_TRUE(f.getError(id2, item));
  EXPECT_FALSE(f.getError(id3, item));

  auto ids = f.getActiveErrorIds();
  EXPECT_EQ(ids.size(), 2);
  for (auto id : ids) {
    EXPECT_TRUE(id == id1 || id == id2);
  }
}

TEST(ErrorFrameTest, clearSingleError) {
  data::ErrorFrame f{};
  data::ErrorFrameItem item{};
  bst::clck::TimePoint time{};

  data::LogPacket packet{};
  packet.id = id1;
  packet.level = data::LogPacket::Level::ERROR;

  f.addError(packet);
  packet.id = id2;
  f.addError(packet);

  f.clearError(id1);

  EXPECT_EQ(f.numActiveErrors(), 1);
  EXPECT_EQ(f.numTotalErrors(), 2); // preserved
  EXPECT_EQ(f.numOccurrences(id1), 0);
  EXPECT_EQ(f.numOccurrences(id2), 1);
  EXPECT_FALSE(f.getError(id1, item));
  EXPECT_TRUE(f.getError(id2, item));
  EXPECT_FALSE(f.getError(id3, item));
}

TEST(ErrorFrameTest, clearAllErrors) {
  data::ErrorFrame f{};
  data::ErrorFrameItem item{};
  bst::clck::TimePoint time{};

  data::LogPacket packet{};
  packet.id = id1;
  packet.level = data::LogPacket::Level::ERROR;

  f.addError(packet);
  packet.id = id2;
  f.addError(packet);

  f.clearAllErrors();

  EXPECT_EQ(f.numActiveErrors(), 0);
  EXPECT_EQ(f.numTotalErrors(), 2); // preserved
  EXPECT_EQ(f.numOccurrences(id1), 0);
  EXPECT_EQ(f.numOccurrences(id2), 0);
  EXPECT_FALSE(f.getError(id1, item));
  EXPECT_FALSE(f.getError(id2, item));
  EXPECT_FALSE(f.getError(id3, item));
}

TEST(ErrorFrameTest, getError) {
  data::ErrorFrame f{};
  data::LogPacket packet{};
  packet.id = id1;
  packet.level = data::LogPacket::Level::ERROR;
  packet.created_time = bst::clck::TimePoint{};
  packet.info = "info";
  packet.source = node::Identification::EXTENSION_MODULE;
  packet.secondary_identifier = "secondary_identifier";

  f.addError(packet);

  data::ErrorFrameItem item{};
  EXPECT_TRUE(f.getError(id1, item));
  EXPECT_EQ(item.packet.id, id1);
  EXPECT_EQ(item.packet.level, data::LogPacket::Level::ERROR);
  EXPECT_EQ(item.packet.created_time, packet.created_time);
  EXPECT_EQ(item.packet.info, "info");
  EXPECT_EQ(item.packet.source, node::Identification::EXTENSION_MODULE);
  EXPECT_EQ(item.packet.secondary_identifier, "secondary_identifier");
  EXPECT_EQ(item.occurrences, 1);
  EXPECT_EQ(item.last_reported, packet.created_time);
  EXPECT_EQ(item.first_reported, packet.created_time);

  EXPECT_EQ(f.numActiveErrors(), 1);
  EXPECT_EQ(f.numTotalErrors(), 1);
  EXPECT_EQ(f.numOccurrences(id1), 1);
  EXPECT_EQ(f.numOccurrences(id2), 0);
  EXPECT_FALSE(f.getError(id2, item));
  EXPECT_FALSE(f.getError(id3, item));
}

TEST(ErrorFrameTest, timeOfLastErrorReported) {
  data::ErrorFrame f{};
  data::LogPacket packet{};
  packet.id = id1;
  packet.level = data::LogPacket::Level::ERROR;
  packet.created_time = bst::clck::TimePoint{};
  packet.info = "info";
  packet.source = node::Identification::EXTENSION_MODULE;
  packet.secondary_identifier = "secondary_identifier";

  f.addError(packet);
  auto time1 = f.timeOfLastErrorReported();
  bst::sleep(1);
  f.addError(packet);
  auto time2 = f.timeOfLastErrorReported();
  EXPECT_TRUE(time1 < time2);
}