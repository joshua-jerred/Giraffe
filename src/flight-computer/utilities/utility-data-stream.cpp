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
 * @brief Construct a new DataStream object
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
	error_frame_lock_.lock();
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
	std::time_t expiry_time = current_time + seconds_until_expiry;
	data_stream_lock_.lock(); // Lock the data stream to prevent other threads from accessing it when adding an item to it
	if (seconds_until_expiry == 0) {
		expiry_time = 0; // Set the expiry time to 0 if the data should never expire
	}
	data_stream_.push({data_source, data_name, data_value, expiry_time});
	num_data_packets_++;
	total_data_packets_++;
	data_stream_lock_.unlock(); // Unlock the data stream to make it available
}

/**
 * @brief Add an error to the error stream. This function is thread safe.
 * @param error_source The file or function that the error occurred in.
 * @param error_code The error code or name.
 * @param error_info Human readable information about the error.
 * @param seconds_until_expiry The number of seconds until the error
 * should be considered expired. If this is 0, the error will never expire.
 * @return void
 */
void DataStream::addError(std::string error_source, 
	std::string error_code, std::string error_info,
	int seconds_until_expiry) {
	std::time_t current_time = std::time(nullptr);
	error_stream_lock_.lock();
	if (seconds_until_expiry > 1) {
		error_stream_.push({error_source, error_code, error_info, current_time + seconds_until_expiry});
	} else {
		error_stream_.push({error_source, error_code, error_info, 0});
	}
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

void DataStream::updateErrorFrame(ErrorFrame error_frame) {
	error_frame_lock_.lock();
	error_frame_ = error_frame;
	error_frame_lock_.unlock();	
}

void DataStream::updateFlightProcedure(FlightProcedure flight_procedure) {
	flight_procedure_lock_.lock();
	flight_procedure_ = flight_procedure;
	flight_procedure_lock_.unlock();
}

void DataStream::lockTXQueue() {
	tx_queue_lock_.lock();
}

const std::queue<Transmission>& DataStream::getTXQueue() {
	return tx_queue_;
}

void DataStream::unlockTXQueue() {
	tx_queue_lock_.unlock();
}

void DataStream::addToTxQueue(Transmission tx) {
	tx_queue_lock_.lock();
	tx_queue_.push(tx);
	tx_queue_lock_.unlock();
}

Transmission DataStream::getNextTX() {
	Transmission tx;
	tx_queue_lock_.lock();
	if (!tx_queue_.empty()) {
		tx = tx_queue_.front();
		tx_queue_.pop();
	}
	tx_queue_lock_.unlock();
	return tx;
}

int DataStream::getTXQueueSize() {
	int size;
	tx_queue_lock_.lock();
	size = tx_queue_.size();
	tx_queue_lock_.unlock();
	return size;
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

std::string DataStream::getData(std::string data_source, std::string data_name) 
	{
	std::string value;

	data_frame_lock_.lock();
	if (!data_frame_.contains(data_source + ":" + data_name)) {
		value = "NO-DATA";
	} else {
		value = data_frame_[data_source + ":" + data_name].value;
	}
	data_frame_lock_.unlock();
	
	return value;
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

ErrorFrame DataStream::getErrorFrameCopy() {
	error_frame_lock_.lock();
	ErrorFrame error_frame(error_frame_);
	error_frame_lock_.unlock();
	return error_frame;
}

FlightProcedure DataStream::getFlightProcedureCopy() {
	flight_procedure_lock_.lock();
	FlightProcedure flight_procedure(flight_procedure_);
	flight_procedure_lock_.unlock();
	return flight_procedure;
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

void DataStream::updateExtensionStatus(std::string extension_name, ExtensionStatus status) {
	extension_status_lock_.lock();
	extension_status_[extension_name] = status;
	extension_status_lock_.unlock();
}

void DataStream::updateModuleStatus(std::string module_name, ModuleStatus status) {
	module_status_lock_.lock();
	module_status_[module_name] = status;
	module_status_lock_.unlock();
}

std::unordered_map<std::string, ExtensionStatus> DataStream::getExtensionStatuses() {
	extension_status_lock_.lock();
	std::unordered_map<std::string, ExtensionStatus> extension_status(extension_status_);
	extension_status_lock_.unlock();
	return extension_status;
}

std::ostream& operator << (std::ostream& o, const ErrorStreamPacket& e)
{
    o << "Source: " << e.source << "\tError Code: " << e.error_code <<
    "\t Info: " << e.info << std::endl;
    return o;
}
