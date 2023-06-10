#include "configuration.hpp"
#include "shared_data.hpp"
#include "test.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

class ConfigurationTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    shared_data = new data::SharedData();
    conf = new cfg::Configuration(shared_data->streams);
  }
  virtual void TearDown() {
    delete shared_data;
    delete conf;
  }

  data::SharedData *shared_data = nullptr;
  cfg::Configuration *conf = nullptr;

  void printStreamErrors() {
    data::LogPacket packet;
    while (shared_data->streams.log.getNumPackets() > 0) {
      shared_data->streams.log.getPacket(packet);
      std::cout << " -- ERROR -- " << (int)packet.id << " info:" << packet.info << std::endl;
    }
  }
};

TEST_F(ConfigurationTest, general_constructor_defaults) {
  EXPECT_EQ("Giraffe Flight 1", conf->general.getProjectName())
      << "project_name";

  EXPECT_EQ(cfg::gEnum::MainBoard::OTHER, conf->general.getMainBoard())
      << "main_board";

  EXPECT_EQ(cfg::gEnum::ProcedureType::OTHER,
            conf->general.getStartingProcedure())
      << "starting_procedure";

  // ensure there are no errors
  EXPECT_EQ(0, shared_data->streams.log.getNumPackets());
}

TEST_F(ConfigurationTest, general_setters_valid_values) {
  const std::string new_name = "test_name";
  conf->general.setProjectName(new_name);
  EXPECT_EQ(new_name, conf->general.getProjectName());

  const cfg::gEnum::MainBoard new_main_board =
      cfg::gEnum::MainBoard::PI_ZERO_W2;
  conf->general.setMainBoard(new_main_board);
  EXPECT_EQ(new_main_board, conf->general.getMainBoard());

  const cfg::gEnum::ProcedureType new_starting_procedure =
      cfg::gEnum::ProcedureType::ASCENT;
  conf->general.setStartingProcedure(new_starting_procedure);
  EXPECT_EQ(new_starting_procedure, conf->general.getStartingProcedure());

  EXPECT_EQ(0, shared_data->streams.log.getNumPackets());
}

TEST_F(ConfigurationTest, saves_default_config_file) {
  const std::string file_path = "saves_config_file_test.json";
  conf->save(file_path);

  ASSERT_TRUE(std::filesystem::exists(file_path));

  std::ifstream in(file_path);
  ASSERT_TRUE(in.good());

  nlohmann::json json_data;
  json_data = nlohmann::json::parse(in);

  EXPECT_EQ("Giraffe Flight 1", json_data["general"]["project_name"])
      << "project_name";

  EXPECT_EQ("other", json_data["general"]["main_board"]) << "main_board";

  EXPECT_EQ("other", json_data["general"]["starting_procedure"])
      << "starting_procedure";
}

TEST_F(ConfigurationTest, loads_default_config_file) {
  const std::string file_path = "loads_config_file_test.json";
  conf->save(file_path);

  cfg::Configuration new_conf(shared_data->streams);
  new_conf.load(file_path);

  EXPECT_EQ("Giraffe Flight 1", new_conf.general.getProjectName())
      << "project_name";

  EXPECT_EQ(cfg::gEnum::MainBoard::OTHER, new_conf.general.getMainBoard())
      << "main_board";

  EXPECT_EQ(cfg::gEnum::ProcedureType::OTHER,
            new_conf.general.getStartingProcedure())
      << "starting_procedure";

  EXPECT_EQ(0, shared_data->streams.log.getNumPackets());
  printStreamErrors();
}

TEST_F(ConfigurationTest, loads_modified_config_file) {
  const std::string file_path = "loads_modified_config_file_test.json";
  conf->save(file_path);

  std::ifstream in(file_path);
  ASSERT_TRUE(in.good());

  nlohmann::json json_data;
  json_data = nlohmann::json::parse(in);

  json_data["general"]["project_name"] = "test_name";
  json_data["general"]["main_board"] = "pi_zero_w2";
  json_data["general"]["starting_procedure"] = "ascent";

  in.close();

  std::ofstream out(file_path);
  out << json_data.dump(4);
  out.close();

  cfg::Configuration new_conf(shared_data->streams);
  new_conf.load(file_path);

  EXPECT_EQ("test_name", new_conf.general.getProjectName()) << "project_name";

  EXPECT_EQ(cfg::gEnum::MainBoard::PI_ZERO_W2, new_conf.general.getMainBoard())
      << "main_board";

  EXPECT_EQ(cfg::gEnum::ProcedureType::ASCENT,
            new_conf.general.getStartingProcedure())
      << "starting_procedure";

  EXPECT_EQ(0, shared_data->streams.log.getNumPackets());
}

TEST_F(ConfigurationTest, set_from_json_valid_values) {
  nlohmann::json json_data =
      nlohmann::json::object({{"project_name", "test_name"},
                              {"main_board", "pi_zero_w2"},
                              {"starting_procedure", "ascent"}});

  conf->general.setFromJson(json_data);

  ASSERT_EQ(0, shared_data->streams.log.getNumPackets());

  EXPECT_EQ("test_name", conf->general.getProjectName());
  EXPECT_EQ(cfg::gEnum::MainBoard::PI_ZERO_W2, conf->general.getMainBoard());
  EXPECT_EQ(cfg::gEnum::ProcedureType::ASCENT,
            conf->general.getStartingProcedure());
}

TEST_F(ConfigurationTest, set_from_json_invalid_values) {
  nlohmann::json json_data =
      nlohmann::json::object({{"project_name", ""},
                              {"main_board", "abc"},
                              {"starting_procedure", "abc"}});

  conf->general.setFromJson(json_data);

  ASSERT_EQ(3, shared_data->streams.log.getNumPackets());

  EXPECT_NE("test_name", conf->general.getProjectName());
  EXPECT_NE(cfg::gEnum::MainBoard::PI_ZERO_W2, conf->general.getMainBoard());
  EXPECT_NE(cfg::gEnum::ProcedureType::ASCENT,
            conf->general.getStartingProcedure());
}