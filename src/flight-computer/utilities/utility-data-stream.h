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
};

/**
 * @brief This class is passed to each extension/module. It is used
 * to communicate data and errors to the data module from any
 * location or even from another thread. There is a lock when writing
 * or reading from the streams that will allow for data safety when
 * threads access the stream.
 * 
 */
class DataStream {
public:
    /**
    * @brief Construct a new DataStream::DataStream object
    * 
    */
    DataStream();

    /**
     * @brief Deconstruct the Data Stream object
     * Will deconstruct regardless of lock status.
     */
    ~DataStream();

    /**
     * @brief Add data to the data stream
     * 
     * @param dataSource - The source of the data, ie "BMP180"
     * @param dataName - The name of the data, ie "Pressure - kPa"
     * @param data_value  - The value of the data, ie "1013.25"
     */
    void addData(std::string data_source, 
	std::string data_name, std::string data_value, int seconds_until_expiry);

    /** 
     * @brief Add an error to the error stream. This includes locking and 
     * unlocking the mutex for the data stream, so it call be called by
     * any thread.
     * 
     * @param errorSource - The source of the error, ie "BMP180"
     * @param errorName - The name of the error, ie "I2C"
     * @param errorInfo - The info of the error, ie "Read Error"
     */
    void addError(std::string error_source, std::string error_name, 
                  std::string error_info);

    /** 
     * @brief Get the oldest packet from the data stream.
     * 
     * @return data_stream_packet - The packet.
     */
    DataStreamPacket getNextDataPacket();

    /** 
     * @brief Get the oldest packet from the error stream.
     * 
     * @return error_stream_packet - The packet.
     */
    ErrorStreamPacket getNextErrorPacket();

    /** 
     * @brief Get the number of data packets in the data stream.
     * 
     * @return int - The number of data packets.
     */
    int getNumDataPackets();

    /** 
     * @brief Get the number of error packets in the error stream.
     * 
     * @return int - The number of error packets.
     */
    int getNumErrorPackets();

    /** 
     * @brief Get the total number of data packets since the stream was created.
     * 
     * @return int - The total number of data packets.
     */
    int getTotalDataPackets();

    /** 
     * @brief Get the total number of error packets since the stream was created.
     * 
     * @return int - The total number of error packets.
     */
    int getTotalErrorPackets();

private:
    int num_data_packets_;
    int total_data_packets_;

    int num_error_packets_;
    int total_error_packets_;

    std::mutex data_stream_lock_;
    std::mutex error_stream_lock_;

    std::queue<DataStreamPacket> data_stream_;
    std::queue<ErrorStreamPacket> error_stream_;
};

#endif