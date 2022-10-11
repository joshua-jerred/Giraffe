/**
 * @file utility-data-stream.h
 * @author Joshua Jerred (github.com/joshua-jerred)
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

#include "utility-config-types.h"


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

typedef std::unordered_map<std::string, DataStreamPacket> DataFrame;

/**
 * @brief This struct is used by the DataStrean and data module.
 * Individual extensions/modules do not need to be aware of this
 * struct as it is handled by the DataStream class. 
 * 
 * @see DataStream::addError()
 */
struct ErrorStreamPacket {
    std::string error_source = "";
    std::string error_name  = "";
    std::string error_info  = "";
    std::time_t expiration_time = 0;
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
    void addData(std::string data_source, 
	std::string data_name, std::string data_value, int seconds_until_expiry);

    void addError(std::string error_source, std::string error_name, 
                  std::string error_info, int seconds_until_expiry);

    void updateDataFrame(DataFrame data_frame);

    DataStreamPacket getNextDataPacket();
    ErrorStreamPacket getNextErrorPacket();
    DataFrame getDataFrameCopy();

    int getNumDataPackets();
    int getNumErrorPackets();
    int getTotalDataPackets();
    int getTotalErrorPackets();

private:
    int num_data_packets_;
    int total_data_packets_;

    int num_error_packets_;
    int total_error_packets_;

    std::mutex data_stream_lock_;
    std::mutex error_stream_lock_;
    std::mutex data_frame_lock_;

    std::queue<DataStreamPacket> data_stream_;
    std::queue<ErrorStreamPacket> error_stream_;

    DataFrame data_frame_;
};

#endif // UTILITY_DATA_STREAM_H_