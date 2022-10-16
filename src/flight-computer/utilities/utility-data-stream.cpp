/**
 * @file utility-data-stream.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Contains the implementation of the DataStream class.
 * @see utility-data-stream.h
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 */

#include "utility-data-stream.h"

/**
 * @brief Construct a new DataStreamobject
 * @param None
 */
DataStream::DataStream() {
	num_data_packets_ = 0;
	total_data_packets_ = 0;

	num_error_packets_ = 0;
	total_error_packets_ = 0;

	data_stream_lock_.unlock(); // Unlock the data stream to make it available
	error_stream_lock_.unlock(); // Unlock the error stream to make it available
}

/**
 * @brief Destroys the DataStream object. Waits for other threads to finish
 * first. Other threads that access the data should be stopped before this is 
 * called.
 */
DataStream::~DataStream() {
	data_stream_lock_.lock();
	error_stream_lock_.lock();
	data_frame_lock_.lock();
}

/**
 * @brief Add data to the data stream. This function is thread safe.
 * @param data_source - The source of the data, ie "BMP180"
 * @param data_name - The name of the data, ie "TEMP_F"
 * @param data_value - The value of the data, ie "72.5"
 * @param seconds_until_expiry - The number of seconds until the data should
 * be considered expired. If this is 0, the data will never expire.
 */
void DataStream::addData(
		std::string data_source, 
		std::string data_name, 
		std::string data_value, 
		int seconds_until_expiry
		) {
	std::time_t current_time = std::time(nullptr);
	data_stream_lock_.lock(); // Lock the data stream to prevent other threads from accessing it when adding an item to it
	data_stream_.push({data_source, data_name, data_value,
		(current_time + seconds_until_expiry)});
	num_data_packets_++;
	total_data_packets_++;
	data_stream_lock_.unlock(); // Unlock the data stream to make it available
}

/**
 * @brief Add an error to the error stream. This function is thread safe.
 * @param error_source The file or function that the error occured in.
 * @param error_name The error code or name.
 * @param error_info Human readable information about the error.
 * @param seconds_until_expiry The number of seconds until the error
 * should be considered expired. If this is 0, the error will never expire.
 * @return void
 */
void DataStream::addError(std::string error_source, 
	std::string error_name, std::string error_info,
	int seconds_until_expiry) {
	std::time_t current_time = std::time(nullptr);
	error_stream_lock_.lock();
	error_stream_.push(
		{error_source, error_name, error_info, 
		current_time + seconds_until_expiry}
		);
	num_error_packets_++;
	total_error_packets_++;
	error_stream_lock_.unlock();
}

/**
 * @brief This is called externally by the data module. It is used to
 * update the data frame that can be accessed by everyone. Thread safe.
 * @param data_frame
 * @return void
 */
void DataStream::updateDataFrame(DataFrame data_frame) {
	data_frame_lock_.lock();
	data_frame_ = data_frame;
	data_frame_lock_.unlock();
}

/**
 * @brief Returns the next data packet. Thread safe.
 * @param None
 * @return DataStreamPacket The next data packet, or an empty one if there
 * are no more data packets.
 */
DataStreamPacket DataStream::getNextDataPacket() {
	data_stream_lock_.lock(); // lock while reading
	DataStreamPacket packet;
	if (data_stream_.size() == 0) { // If there are no packets in the stream, return an empty packet.
		data_stream_lock_.unlock();
		return packet;
	}
	packet = data_stream_.front();
	data_stream_.pop();
	num_data_packets_--;
	data_stream_lock_.unlock();
	return packet;
}

/**
 * @brief Returns the next error. Thread safe.
 * @param None 
 * @return ErrorStreamPacket An error stream packet with an error. If there are
 * no more errors it will return an empty packet.
 */
ErrorStreamPacket DataStream::getNextErrorPacket() {
	error_stream_lock_.lock();
	ErrorStreamPacket packet;
	if (error_stream_.size() == 0) {
		error_stream_lock_.unlock();
		return packet;
	}
	packet = error_stream_.front();
	error_stream_.pop();
	num_error_packets_--;
	error_stream_lock_.unlock();
	return packet;
}

/**
 * @brief Makes a copy of the DataFrame and returns it. This is generally
 * called by a thread in a different module. This is thread safe.
 * @param None
 * @return DataFrame 
 */
DataFrame DataStream::getDataFrameCopy() {
	data_frame_lock_.lock();
	DataFrame data_frame(data_frame_);
	data_frame_lock_.unlock();
	return data_frame;
}

/**
 * @return int The number of data packets in the data stream.
 */
int DataStream::getNumDataPackets() {
	return num_data_packets_;
}

/**
 * @return int The number of error packets in the error stream.
 */
int DataStream::getNumErrorPackets() {
	return num_error_packets_;
}

/**
 * @return int The total number of data packets that have been added to the
 * data stream since the data stream was created.
 */
int DataStream::getTotalDataPackets() {
	return total_data_packets_;
}

/**
 * @return int The total number of error packets that have been added to the
 * error stream since the error stream was created.
 */
int DataStream::getTotalErrorPackets() {
	return total_error_packets_;
}