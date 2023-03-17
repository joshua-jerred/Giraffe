/**
 * @file config-types.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief The types/structs used for configuring the flight computer.
 * 
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 */

#ifndef UTILITY_COMMON_TYPES_H_
#define UTILITY_COMMON_TYPES_H_

#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Struct containing the procedure configuration. One for each type.
 * 
 */
struct FlightProcedure {

    enum class Type {
        ERROR,
        TESTING,
        PRE_LAUNCH,
        STANDARD,
        DESCENT,
        RECOVERY,
        FAILSAFE
    };

    struct Intervals {
        int data_log = 5; // Hard Coded Default, may change to macro
        int data_packet = 5;
        int sstv = 5;
        int aprs = 5;
        int picture = 5;
        int health_check = 5;
    };

    bool enabled = false;
    Type type = Type::ERROR;
    Intervals intervals = Intervals();
};


/**
 * @brief This struct is passed in for all new extension objects. It
 * contains all metadata from the config file related to the extension.
 */
struct ExtensionMetadata {
    
    enum class Category {
        ERROR,
        OTHER, 
        GPS ,
        CAMERA,
        BATTERY,
        SYSTEM,
        INTERNAL_SENSOR,
        EXTERNAL_SENSOR
    };

    enum class Interface {
        ERROR,
        OTHER, 
        INTERNAL, 
        I2C, 
        UART, 
        ONEWIRE, 
        USB,
        GPIO
    };

    struct ExtraArgs {
        int I2C_bus = -1;
        std::string I2C_device_address = "";
        std::string one_wire_id = "";
    };

    int id = 0; // User defined ID for the extension
    std::string name = ""; // Can be set to anything between 2 and 20 characters
    std::string extension_type = ""; // Unique string for each extension type, like "BMP180"
    Category category = (Category) 0;
    Interface interface = (Interface) 0;
    int update_interval = 0; // How often the extension polls for data and sends it to the data stream
    bool critical = false; // Indicates if this extension is critical to operation, this
                  // will be used by the Flight Runner during the healthCheck
    ExtraArgs extra_args {};
};

struct RadioMetadata {
    int radio_id = 0;
    std::string radio_type = "";
    int priority = 0;

    // Frequency ranges in MHz [min, max] (inclusive)
    std::vector<std::pair<std::string, std::string>> frequency_ranges {};

    // Modes
    bool TX_enabled = false;
    bool RX_enabled = false;
    bool APRS_enabled = false;
    bool SSTV_enabled = false;
    bool DATA_enabled = false;

    // Capabilities
    bool frequency_switching_capable = false;
    bool power_saving_capable = false;
    bool high_low_power_capable = false;
    bool separate_tx_rx_capable = false;
    bool bandwidth_switching_capable = false;
    bool rssi_capable = false;
    bool volume_adjust_capable = false;

    // Interface
    ExtensionMetadata::Interface interface = ExtensionMetadata::Interface::ERROR;
    ExtensionMetadata::ExtraArgs extra_args {};
    int PTT_pin = -1;     // If it's a raspberry pi, these are BCM pin numbers
    int power_pin = -1;
};


/**
 * @brief Container for all config data. Mirrors the config file.
 * @see config.json
 */
struct ConfigData {

    enum Mainboard {
        ERROR ,
        OTHER ,
        PI_ZERO_W_2,
        PI_4,
    };

    struct General {
        std::string project_name = "GFS Balloon";
        Mainboard main_board = Mainboard::ERROR;
        FlightProcedure::Type starting_proc = FlightProcedure::Type::STANDARD; // Default is standard
    };

    struct Extensions {
        std::vector<ExtensionMetadata> extensions_list {};
        std::string gps_data_name = "";
        std::string battery_data_name = "";
        std::string system_data_name = "";
        std::string radio_data_name = "";
        std::string pressure_data_name = "";
    };

    struct Debugging {
        bool console_enabled = false;
        int console_update_interval = 1000;

        bool web_server_enabled = false;
        int web_server_socket_port = 8779;
    };

    struct Telemetry {
        bool telemetry_enabled = false;
        std::string call_sign = "error";

        std::vector<RadioMetadata> radios {};

        bool aprs_enabled = false;
        std::string aprs_freq {};
        int aprs_ssid = 0;
        std::string aprs_destination_address = "APRS";
        int aprs_destination_ssid = 0;
        bool aprs_alternate_symbol_table = false;
        char aprs_symbol = 'O';
        std::string aprs_comment = "GFS Balloon";
        bool aprs_position_packets = false;
        bool aprs_telemetry_packets = false;

        bool sstv_enabled = false;
        std::string sstv_mode {};
        std::string sstv_freq {};
        std::string sstv_comment = "";
        bool sstv_save_images = false;
        bool sstv_overlay_data = true;

        bool data_packets_enabled = false;
        std::string data_packets_freq {};
        std::string data_packets_mode {};

    };

    struct DataTypes {
        struct DataType {
            std::string source = "";
            std::string unit = "";
            int include_in_telemetry = 0;
            std::string telemetry_name = "";
        };
        std::vector<DataType> types {};
    };

    struct Procs {
        FlightProcedure testing {};
        FlightProcedure standard {};
        FlightProcedure recovery {};
        FlightProcedure failsafe {};
    };

    time_t start_time = 0;

    General general {};
    Extensions extensions {};
    Debugging debug {};
    Telemetry telemetry {};
    DataTypes data_types {};
    Procs flight_procs {};
};

static const std::unordered_map<ExtensionMetadata::Category, std::string> 
        kExtensionCategoryToString = {
    {ExtensionMetadata::Category::ERROR, "ERROR"},
    {ExtensionMetadata::Category::OTHER, "OTHER"},
    {ExtensionMetadata::Category::GPS, "GPS"},
    {ExtensionMetadata::Category::CAMERA, "CAMERA"},
    {ExtensionMetadata::Category::BATTERY, "BATTERY"},
    {ExtensionMetadata::Category::SYSTEM, "SYSTEM"},
    {ExtensionMetadata::Category::INTERNAL_SENSOR, "INTERNAL_SENSOR"},
    {ExtensionMetadata::Category::EXTERNAL_SENSOR, "EXTERNAL_SENSOR"}
};

static const std::unordered_map<ExtensionMetadata::Interface, std::string> 
        kExtensionInterfaceToString = {
    {ExtensionMetadata::Interface::ERROR, "ERROR"},
    {ExtensionMetadata::Interface::OTHER, "OTHER"},
    {ExtensionMetadata::Interface::INTERNAL, "INTERNAL"},
    {ExtensionMetadata::Interface::I2C, "I2C"},
    {ExtensionMetadata::Interface::UART, "UART"},
    {ExtensionMetadata::Interface::ONEWIRE, "ONEWIRE"},
    {ExtensionMetadata::Interface::USB, "USB"},
    {ExtensionMetadata::Interface::GPIO, "GPIO"}
};

static const std::unordered_map<FlightProcedure::Type, std::string> 
        kFlightProcedureTypeToString = {
    {FlightProcedure::Type::ERROR, "ERROR"},
    {FlightProcedure::Type::TESTING, "TESTING"},
    {FlightProcedure::Type::PRE_LAUNCH, "PRE_LAUNCH"},
    {FlightProcedure::Type::STANDARD, "STANDARD"},
    {FlightProcedure::Type::DESCENT, "DESCENT"},
    {FlightProcedure::Type::RECOVERY, "RECOVERY"},
    {FlightProcedure::Type::FAILSAFE, "FAILSAFE"}
};

#endif