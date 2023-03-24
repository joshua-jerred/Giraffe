/**
 * @file gfs-types.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Special types used throughout the GFS.
 * @details There any many structs and enums used throughout all of GFS. They
 * are defined here to avoid needing to include unnecessary headers.
 * @date 2023-01-14
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#ifndef GFS_TYPES_H_
#define GFS_TYPES_H_

#include <string>
#include <unordered_map>
#include <ctime>

struct GFSCommand {
    enum class CommandCategory {
        unknown,
        CFG, // config
        TLM, // telemetry
        MDL, // modules
        EXT, // extensions
        FLR, // flight runner
        DAT  // data module
    };
    CommandCategory category = CommandCategory::unknown;
    std::string id = "";
    std::string arg = "";
};

/**
 * @brief Used by the telemetry module in it's queue and by the data stream.
 * @todo move this
 */
struct Transmission {
    enum class Type {
        ERROR,
        APRS,
        DATA,
        SSTV,
    };
    Type type = Type::ERROR;
    std::string message = "";
    std::string wav_location = "";
    int length = 0; // Length in seconds
    int tx_num = 0; // ID for the transmission
};

enum class FlightPhase {
    ERROR,
    PRE_LAUNCH,
    ASCENT,
    DESCENT,
    RECOVERY,
};

enum class GPSFixType {
    ERROR,
    UNKNOWN,
    NO_FIX,
    FIX_2D,
    FIX_3D,
};

struct GPSFrame {    
    std::string source = ""; // The source of the GPS data
    std::string time = ""; // UTC time string


    GPSFixType fix = GPSFixType::ERROR;
    int num_satellites = 0;

    double latitude = 0.0;   // degrees
    double longitude = 0.0;  // degrees
    double horz_accuracy = 0.0; // meters

    double altitude = 0.0;  // meters
    double vert_accuracy = 0.0; // meters

    double vertical_speed = 0.0; // meters/second
    double ground_speed = 0.0; // meters/second
    double speed_accuracy = 0.0; // meters/second

    double heading_of_motion = 0.0; // degrees
    double heading_accuracy = 0.0;  // degrees
};

/**
 * @brief Actionable data that is used by the flight runner for health checks.
 */
struct CriticalData {
    FlightPhase phase = FlightPhase::ERROR;

    bool gps_data_valid = false;
    GPSFrame latest_valid_gps_data = GPSFrame(); // Last valid GPS data

    bool pressure_data_valid = false;
    float pressure_mbar = 0.0; // millibars

    bool battery_data_valid = false;
    float battery_voltage = 0.0; // volts

    bool system_data_valid = false;
    float ram_usage = 0.0; // percent 0.0 - 100.0
    float disk_usage = 0.0; // percent 0.0 - 100.0

    bool radio_status = false; // If the radio is functioning
};

static const std::unordered_map<GFSCommand::CommandCategory, std::string> GFS_COMMAND_CATEGORY_TO_STRING = {
    {GFSCommand::CommandCategory::unknown, "unknown"},
    {GFSCommand::CommandCategory::TLM, "TLM"},
    {GFSCommand::CommandCategory::MDL, "MDL"},
    {GFSCommand::CommandCategory::EXT, "EXT"},
    {GFSCommand::CommandCategory::FLR, "FLR"},
    {GFSCommand::CommandCategory::DAT, "DAT"},
};

static const std::unordered_map<Transmission::Type, std::string> TRANSMISSION_TYPE_TO_STRING = {
    {Transmission::Type::ERROR, "ERROR"},
    {Transmission::Type::APRS, "APRS"},
    {Transmission::Type::DATA, "DATA"},
    {Transmission::Type::SSTV, "SSTV"},
};

static const std::unordered_map<FlightPhase, std::string> FLIGHT_PHASE_TO_STRING = {
    {FlightPhase::ERROR, "ERROR"},
    {FlightPhase::PRE_LAUNCH, "PRE_LAUNCH"},
    {FlightPhase::ASCENT, "ASCENT"},
    {FlightPhase::DESCENT, "DESCENT"},
    {FlightPhase::RECOVERY, "RECOVERY"},
};

static const std::unordered_map<GPSFixType, std::string> GPS_FIX_TO_STRING = {
    {GPSFixType::ERROR, "ERROR"},
    {GPSFixType::UNKNOWN, "UNKNOWN"},
    {GPSFixType::NO_FIX, "NO_FIX"},
    {GPSFixType::FIX_2D, "FIX_2D"},
    {GPSFixType::FIX_3D, "FIX_3D"},
};

#endif