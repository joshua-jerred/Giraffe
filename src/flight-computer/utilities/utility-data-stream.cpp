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

#include <string>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <ctime>

#include "utility-data-stream.h"


DataStream::DataStream() {
	num_data_packets_ = 0;
	total_data_packets_ = 0;

	num_error_packets_ = 0;
	total_error_packets_ = 0;

	data_stream_lock_.unlock(); // Unlock the data stream to make it available
	error_stream_lock_.unlock();	// Unlock the error stream to make it available
}


DataStream::~DataStream() {

}


void DataStream::addData(
		std::string data_source, 
		std::string data_name, 
		std::string data_value, 
		int seconds_until_expiry
		) {
	data_stream_lock_.lock(); // Lock the data stream to prevent other threads from accessing it when adding an item to it
	data_stream_.push({data_source, data_name, data_value,
		std::time_t(NULL) + seconds_until_expiry});
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