/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_server_database.hpp
 * @brief  Basic database for the GDL Server using SQLiteCPP
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-19
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_SERVER_DATABASE_HPP_
#define GDL_SERVER_DATABASE_HPP_

#include "nlohmann/json.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>

#include "gdl_message.hpp"

using json = nlohmann::json;

namespace giraffe::gdl {

class GdlServerDatabase {
public:
  GdlServerDatabase()
      : db_(DB_NAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    createTables();
  }
  ~GdlServerDatabase() = default;

  void createTables() {
    // create received_message table with autoincrementing id
    db_.exec("CREATE TABLE IF NOT EXISTS message_log ("
             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
             "direction TEXT NOT NULL,"
             "id_string TEXT NOT NULL,"
             "type TEXT NOT NULL,"
             "message TEXT NOT NULL,"
             "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
             ");");
    db_.exec("CREATE TABLE IF NOT EXISTS received_location_packets ("
             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
             "time_code TEXT NOT NULL,"
             "latitude REAL NOT NULL,"
             "longitude REAL NOT NULL,"
             "altitude INTEGER NOT NULL,"
             "speed REAL NOT NULL,"
             "heading INTEGER NOT NULL,"
             "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
             ");");
  }

  /**
   * @brief Insert a message into the database
   * @param msg - the message to insert
   * @param sent - true if the message was sent, false if it was received
   */
  void insertMessage(const Message &msg, bool sent) {
    std::string direction = sent ? "SENT" : "RECEIVED";

    if (msg.getType() == Message::Type::BROADCAST ||
        msg.getType() == Message::Type::EXCHANGE) {
      SQLite::Statement query(
          db_, "INSERT INTO message_log (direction, id_string, type, "
               "message) VALUES (?, ?, ?, ?);");
      query.bind(1, direction);
      query.bind(2, msg.getIdentifierString());
      query.bind(3, msg.getType() == Message::Type::BROADCAST ? "BROADCAST"
                                                              : "EXCHANGE");
      query.bind(4, msg.getData());
      query.exec();
    } else if (msg.getType() == Message::Type::LOCATION) {
      SQLite::Statement query(db_,
                              "INSERT INTO received_location_packets "
                              "(time_code, latitude, longitude, altitude, "
                              "speed, heading) VALUES (?, ?, ?, ?, ?, ?);");
      query.bind(1, msg.getLocation().time_code);
      query.bind(2, msg.getLocation().latitude);
      query.bind(3, msg.getLocation().longitude);
      query.bind(4, msg.getLocation().altitude);
      query.bind(5, msg.getLocation().speed);
      query.bind(6, msg.getLocation().heading);
      query.exec();
    }
  }

  std::vector<Message> getLatestReceivedMessages() {
    std::vector<Message> messages;
    SQLite::Statement query(
        db_,
        "SELECT direction, id_string, type, message, id FROM "
        "message_log WHERE direction = 'RECEIVED' ORDER BY id DESC LIMIT 10;");
    while (query.executeStep()) {
      Message msg;
      msg.setIdentifierFromHex(query.getColumn(1).getString());
      msg.setType(query.getColumn(2).getString() == "BROADCAST"
                      ? Message::Type::BROADCAST
                      : Message::Type::EXCHANGE);
      msg.setData(query.getColumn(3).getString());
      messages.push_back(msg);
    }
    return messages;
  }

  json getLatestReceivedMessagesJson() {
    json messages = json::array();
    SQLite::Statement query(
        db_,
        "SELECT id, direction, id_string, type, message, timestamp FROM "
        "message_log WHERE direction = 'RECEIVED' ORDER BY id DESC LIMIT 10;");
    while (query.executeStep()) {
      json msg;
      msg["id"] = query.getColumn(0).getInt();
      msg["msg_id"] = query.getColumn(2).getString();
      msg["type"] = query.getColumn(3).getString();
      msg["message"] = query.getColumn(4).getString();
      msg["timestamp"] = query.getColumn(5).getString();
      messages.push_back(msg);
    }
    return messages;
  }

  std::vector<Message> getLatestSentMessages() {
    std::vector<Message> messages;
    SQLite::Statement query(
        db_, "SELECT direction, id_string, type, message, id FROM "
             "message_log WHERE direction = 'SENT' ORDER BY id DESC LIMIT 10;");
    while (query.executeStep()) {
      Message msg;
      msg.setIdentifierFromHex(query.getColumn(1).getString());
      msg.setType(query.getColumn(2).getString() == "BROADCAST"
                      ? Message::Type::BROADCAST
                      : Message::Type::EXCHANGE);
      msg.setData(query.getColumn(3).getString());
      messages.push_back(msg);
    }
    return messages;
  }

  json getLatestSentMessagesJson() {
    json messages = json::array();
    SQLite::Statement query(
        db_, "SELECT id, direction, id_string, type, message, timestamp FROM "
             "message_log WHERE direction = 'SENT' ORDER BY id DESC LIMIT 10;");
    while (query.executeStep()) {
      json msg;
      msg["id"] = query.getColumn(0).getInt();
      msg["msg_id"] = query.getColumn(2).getString();
      msg["type"] = query.getColumn(3).getString();
      msg["message"] = query.getColumn(4).getString();
      msg["timestamp"] = query.getColumn(5).getString();
      messages.push_back(msg);
    }
    return messages;
  }

  std::vector<Message> getLatestPositionReports() {
    std::vector<Message> messages;
    SQLite::Statement query(db_, "SELECT time_code, latitude, longitude, "
                                 "altitude, speed, heading, id FROM "
                                 "received_location_packets ORDER BY id DESC "
                                 "LIMIT 10;");
    while (query.executeStep()) {
      Message msg;
      Message::Location loc{.latitude = query.getColumn(1).getDouble(),
                            .longitude = query.getColumn(2).getDouble(),
                            .altitude = query.getColumn(3).getUInt(),
                            .speed = query.getColumn(4).getDouble(),
                            .heading = query.getColumn(5).getInt(),
                            .time_code = query.getColumn(0).getString()};
      msg.setLocation(loc);
      messages.push_back(msg);
    }
    return messages;
  }

  void reset() {
    db_.exec("DROP TABLE IF EXISTS received_message;");
    createTables();
  }

private:
  const std::string DB_NAME = "gdl_server.db";
  SQLite::Database db_;
};

} // namespace giraffe::gdl

#endif /* GDL_SERVER_DATABASE_HPP_ */