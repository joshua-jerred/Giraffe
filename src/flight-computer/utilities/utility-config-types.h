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

    enum ProcType {
        ERROR = 0,
        TESTING = 1,
        STANDARD = 2,
        RECOVERY = 3,
        DESCENT = 4,
        FAILSAFE = 4
    };

    struct Intervals {
        int data_log = 5; // Hard Coded Default, may change to macro
        int data_packet = 5;
        int sstv = 5;
        int aprs = 5;
        int picture = 5;
        int health_check = 5;
    };

    int enabled = 0;
    ProcType type = ProcType::ERROR;
    Intervals intervals = Intervals();
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
    int critical = 0; // Indicates if this extension is critical to operation, this
                  // will be used by the Flight Runner during the healthCheck
    ExtraArgs extra_args {};
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
        FlightProcedure::ProcType starting_proc {}; // Default is standard
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

        std::string call_sign {};

        int afsk_enabled = 0;
        std::string afsk_freq {};

        int psk_enabled = 0;
        std::string psk_freq {};
        std::string psk_mode {};
        std::string psk_symbol_rate{};

        int sstv_enabled = 0;
        std::string sstv_freq {};

        int aprs_enabled = 0;
        std::string aprs_freq {};
        int aprs_key = 0;
        int aprs_ssid = 0;
        std::string aprs_symbol = "O";
        std::string aprs_memo = "GFS Balloon";
    };

    struct DataTypes {
        struct ExtensionDataType {
            std::string source = "";
            std::string name = "";
            std::string unit = "";
            int include_in_telemetry = 0;
        };
        std::vector<ExtensionDataType> types {};
    };

    struct Procs {
        FlightProcedure testing {};
        FlightProcedure standard {};
        FlightProcedure recovery {};
        FlightProcedure failsafe {};
    };

    General general {};
    Extensions extensions {};
    Debugging debug {};
    Telemetry telemetry {};
    DataTypes data_types {};
    Procs flight_procs {};
};

/**
 * @brief Used by the telemetry module in it's queue and by the data stream.
 */
struct Transmission {
    enum class Type {
        ERROR = 0,
        APRS = 1,
        AFSK = 2,
        PSK = 3,
        SSTV = 4,
    };
    Type type = Type::ERROR;
    std::string wav_location = "";
    int length = 0; // Length in seconds
};

#endif