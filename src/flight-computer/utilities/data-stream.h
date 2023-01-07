/**
 * @file data-stream.h
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Contains the declaration of the DataStream class.
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef UTILITY_DATA_STREAM_H_
#define UTILITY_DATA_STREAM_H_

#include <mutex>
#include <queue>
#include <ctime>
#include <string>
#include <unordered_map>
#include <iostream>

#include "config-types.h"
#include "status.h"


/**
 * @brief This struct is used by the DataStream and data module.
 * Individual extensions/modules do not need to be aware of this
 * struct as it is handled by the DataStream class.
 * @see DataStream::addData()
 */
struct DataStreamPacket {
    std::string source = "";
    std::string unit = "";
    std::string value = ""; 
    std::time_t expiration_time = 0;
};
std::ostream& operator << (std::ostream& o, const DataStreamPacket& d);

/**
 * @brief This struct is used by the DataStream and data module.
 * Individual extensions/modules do not need to be aware of this
 * struct as it is handled by the DataStream class. 
 * 
 * @see DataStream::addError()
 * @todo clean up var names in struct
 */
struct ErrorStreamPacket {
    std::string source = "";
    std::string error_code  = "";
    std::string info  = "";
    std::time_t expiration_time = 0;
};
std::ostream& operator << (std::ostream& o, const ErrorStreamPacket& e);

typedef std::unordered_map<std::string, DataStreamPacket> DataFrame;
typedef std::unordered_map<std::string, ErrorStreamPacket> ErrorFrame;


struct GFSCommand {
    enum class CommandCategory {
        unknown = 0,
        TLM = 1, // telemetry
        MDL = 2, // modules
        EXT = 3, // extensions
        FLR = 4, // flight runner
    };
    CommandCategory category = CommandCategory::unknown;
    std::string id = "";
    std::string arg = "";
};


/**
 * @brief This class is passed to many extensions/modules. It is used
 * to communicate data and errors to the data module from any
 * location or even from other threads. There are locks when writing
 * or reading from the streams that will allow for data safety when
 * threads access the stream.
 * This class is ultimately the communication bridge between sections of the 
 * flight computer.
 */
class DataStream {
public:
    DataStream();
    ~DataStream();

    // Command Queue
    void addToCommandQueue(std::string command);
    bool getNextCommand(GFSCommand& command);

    void addData(std::string data_source, 
	std::string data_name, std::string data_value, int seconds_until_expiry);

    void addError(std::string error_source, std::string error_code, 
                  std::string error_info, int seconds_until_expiry = 0);

    void updateDataFrame(DataFrame data_frame);
    void updateErrorFrame(ErrorFrame error_frame);
    void updateFlightProcedure(FlightProcedure flight_procedure);

    DataStreamPacket getNextDataPacket();
    ErrorStreamPacket getNextErrorPacket();

    std::string getData(std::string data_source, std::string data_name);
    DataFrame getDataFrameCopy();
    ErrorFrame getErrorFrameCopy();
    FlightProcedure getFlightProcedureCopy();

    int getNumDataPackets();
    int getNumErrorPackets();
    int getTotalDataPackets();
    int getTotalErrorPackets();

    // TX Queue
    void lockTXQueue();
    const std::queue<Transmission>& getTXQueue();
    void unlockTXQueue();

    void addToTxQueue(Transmission tx);
    Transmission getNextTX();
    int getTXQueueSize();
    int getTotalTx();

    // TX Log
    void lockTXLog();
    const std::deque<Transmission>& getTXLog();
    void unlockTXLog();

    struct TXLogInfo {
        int tx_log_size = 0;
        int max_size = 0;
        int first_tx_in_log = 0;
        int last_tx_in_log = 0;
    };
    const TXLogInfo getTXLogInfo();

    bool requestTXFromLog(int tx_id, Transmission& tx);

    // Status
    void updateExtensionStatus(std::string extension_name, ExtensionStatus status);
    void updateModuleStatus(std::string module_name, ModuleStatus status);
    std::unordered_map<std::string, ExtensionStatus> getExtensionStatuses();

    std::mutex& getI2CBusLock();


    
private:
    void error(std::string code);
    void error(std::string code, std::string info);

    int num_data_packets_ = 0;
    int total_data_packets_ = 0;

    int num_error_packets_ = 0;
    int total_error_packets_ = 0;

    std::mutex command_queue_lock_ = std::mutex();
    std::queue<GFSCommand> command_queue_ = std::queue<GFSCommand>(); 

    std::mutex data_stream_lock_ = std::mutex();
    std::queue<DataStreamPacket> data_stream_ = std::queue<DataStreamPacket>();

    std::mutex error_stream_lock_ = std::mutex();
    std::queue<ErrorStreamPacket> error_stream_ = std::queue<ErrorStreamPacket>();

    std::mutex data_frame_lock_ = std::mutex();
    DataFrame data_frame_ = DataFrame();

    std::mutex error_frame_lock_ = std::mutex();
    ErrorFrame error_frame_ = ErrorFrame();

    std::mutex flight_procedure_lock_ = std::mutex();
    FlightProcedure flight_procedure_ = FlightProcedure();

    std::mutex tx_queue_lock_ = std::mutex();
    std::queue<Transmission> tx_queue_ = std::queue<Transmission>();

    std::mutex extension_status_lock_ = std::mutex();
    std::unordered_map<std::string, ExtensionStatus> extension_status_ = 
        std::unordered_map<std::string, ExtensionStatus>();

    std::mutex module_status_lock_ = std::mutex();
    std::unordered_map<std::string, ModuleStatus> module_status_ = 
        std::unordered_map<std::string, ModuleStatus>();

    std::mutex i2c_bus_lock_ = std::mutex();

    // Transmission Log
    std::mutex tx_log_lock_ = std::mutex();
    const int kTXLogSize_ = 10; /** @todo make this user configurable */
    std::deque<Transmission> tx_log_ {};
    int first_tx_in_log_ = 0;
    int last_tx_in_log_ = 0;
};

#endif // UTILITY_DATA_STREAM_H_
