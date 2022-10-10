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
        ERROR = 0,
        TESTING = 1,
        STANDARD = 2,
        RECOVERY = 3,
        FAILSAFE = 4
    };

    struct Intervals {
        int data_log = 5; // Hard Coded Default, may change to macro
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
        ERROR = 0,
        OTHER = 1, 
        RADIO = 2, 
        GPS = 3, 
        CAMERA = 4, 
        INTERNAL_SENSOR = 5, 
        EXTERNAL_SENSOR = 6, 
    };

    enum class Interface {
        ERROR = 0,
        OTHER = 1, 
        INTERNAL = 2, 
        I2C = 3, 
        SERIAL = 4, 
        ONEWIRE = 5, 
        USB = 6,
        GPIO = 7 
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
        ERROR = 0,
        OTHER = 1,
        PI_ZERO = 2, 
        PI_ZERO_W = 3, 
        PI_2 = 4, 
        PI_3 = 5, 
        PI_4 = 6
    };

    struct General {
        std::string project_name {};
        MainboardType main_board {};
        FlightLoop::LoopType starting_loop {}; // Default is standard
    };

    struct Extensions {
        std::vector<ExtensionMetadata> extensions_list {};
    };

    struct Debugging {
        int console_enabled {};
        int console_update_interval {};
        int web_server_enabled {};
        int web_server_update_interval {};
    };

    struct Telemetry {
        int telemetry_enabled = 0;

        std::string callsign {};

        int afsk_enabled {};
        std::string afsk_freq {};

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
            std::string source;
            std::string name;
            std::string unit;
            int include_in_telemtry = 0;
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
    Debugging debug;
    Telemetry telemetry;
    DataTypes data_types;
    Loops flight_loops;
};

#endif