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
 * @brief Construct a new DataStream::DataStream object
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
 * @brief Add data to the data stream
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


void DataStream::addError(std::string errorSource, 
	std::string errorName, std::string errorInfo) {
	error_stream_lock_.lock();
	error_stream_.push({errorSource, errorName, errorInfo});
	num_error_packets_++;
	total_error_packets_++;
	error_stream_lock_.unlock();
}

void DataStream::updateDataFrame(DataFrame data_frame) {
	data_frame_lock_.lock();
	data_frame_ = data_frame;
	data_frame_lock_.unlock();
}

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

DataFrame DataStream::getDataFrameCopy() {
	data_frame_lock_.lock();
	DataFrame data_frame(data_frame_);
	data_frame_lock_.unlock();
	return data_frame;
}

int DataStream::getNumDataPackets() {
	return num_data_packets_;
}

int DataStream::getNumErrorPackets() {
	return num_data_packets_;
}

int DataStream::getTotalDataPackets() {
	return total_data_packets_;
}

int DataStream::getTotalErrorPackets() {
	return total_error_packets_;
}