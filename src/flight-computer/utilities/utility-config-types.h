#ifndef UTILITY_COMMON_TYPES_H_
#define UTILITY_COMMON_TYPES_H_

#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Struct containing the loop configuration. One for each loop type.
 * 
 */
struct FlightLoop {

    enum LoopType {
        kError = 0,
        kTesting = 1,
        kStandard = 2,
        kRecovery = 3,
        kFailsafe = 4
    };

    struct Intervals {
        int data_log = 5; // Hard Coded Default, may change to macro
        int server_update = 5; // Hard Coded Default, may change to macro
        int data_packet = 0;
        int sstv = 0;
        int aprs = 0;
        int picture = 0;
        int health_check = 0;
    };

    int enabled;
    LoopType type;
    Intervals intervals;
};


/**
 * @brief This struct is passed in for all new extension objects. It
 * contains all metadata from the config file related to the extension.
 */
struct ExtensionMetadata {
    
    enum class Category {
        kError = 0,
        kOther = 1, 
        kRadio = 2, 
        kGPS = 3, 
        kCamera = 4, 
        kInternalSensor = 5, 
        kExternalSensor = 6, 
    };

    enum class Interface {
        kError = 0,
        kOther = 1, 
        kInternal = 2, 
        kI2C = 3, 
        kSerial = 4, 
        kOneWire = 5, 
        kUSB = 6,
        kGPIO = 7 
    };

    int id = 0; // User defined ID for the extension
    std::string name = ""; // Can be set to anything between 2 and 20 characters
    std::string extension_type = ""; // Unique string for each extension type, like "BMP180"
    Category category = (Category) 0;
    Interface interface = (Interface) 0;
    int update_interval = 0; // How often the extension polls for data and sends it to the data stream
    int critical = 0; // Indicates if this extension is critical to operation, this
                  // will be used by the Flight Runner during the healthCheck
    // Extra Arguments
    std::string address = ""; // Used for oneWire and I2C
};


/**
 * @brief Container for all config data. Mirrors the config file.
 * @see config.json
 */
struct ConfigData {

    enum MainboardType {
        kError = 0,
        kOther = 1,
        kPi_zero = 2, 
        kPi_zero_w = 3, 
        kPi_2 = 4, 
        kPi_3 = 5, 
        kPi_4 = 6
    };

    struct General {
        std::string project_name {};
        MainboardType main_board {};
        FlightLoop::LoopType starting_loop {}; // Default is standard
    };

    struct Extensions {
        std::vector<ExtensionMetadata> extensions_list {};
    };

    struct Server {
        int web_server_enabled {};
    };

    struct Telemetry {
        int telemetry_enabled = 0;

        std::string callsign {};

        int afsk_enabled {};
        std::string afsk_freq {};

        // int ssdv_enabled {};

        int sstv_enabled {};
        std::string sstv_freq {};

        int aprs_enabled {};
        std::string aprs_freq {};
        int aprs_key = 0;
        int aprs_ssid = 0;
        char aprs_symbol = 'O';
        std::string aprs_memo {};
    };

    struct DataTypes {
        struct ExtensionDataType {
            std::string name;
            std::string unit;
            int include_in_teletry = 0;
        };
        std::vector<ExtensionDataType> types {};
    };

    struct Loops {
        FlightLoop testing {};
        FlightLoop standard {};
        FlightLoop recovery {};
        FlightLoop failsafe {};
    };

    General general;
    Extensions extensions;
    Server server;
    Telemetry telemetry;
    DataTypes data_types;
    Loops flight_loops;
};

#endif