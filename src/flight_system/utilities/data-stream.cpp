/**
 * @file data-stream.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Contains the implementation of the DataStream class.
 * @see data-stream.h
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 */

#include <regex>
#include "data-stream.h"

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

void DataStream::addToCommandQueue(std::string command) {
	const int kMaxCommandSize = 30;
	const int kMinCommandSize = 7;

	const int kMaxCommandQueueSize = 10;

	if (command.size() > kMaxCommandSize) {
		error("CMD_L", command.substr(0, kMaxCommandSize)); // Command too long
		return;
	}
	if (command.size() < kMinCommandSize) {
		error("CMD_S", command); // Command too short
		return;
	}

	// Check if the the command is in the correct format
	const std::regex command_regex("^cmd\\/[a-z]{3}\\/[a-z]{3}\\/[a-z0-9-]{0,20}$");
	if (!std::regex_match(command, command_regex)) {
		error("CMD_B", command); // Command is not in the correct format
		return;
	}

	// Check if the command queue is full
	command_queue_lock_.lock();
	if (command_queue_.size() >= kMaxCommandQueueSize) {
		command_queue_lock_.unlock();
		error("CMD_Q"); // Command queue is full
		return;
	}

	// Parse the command
	GFSCommand gfs_command;
	std::string category = command.substr(4, 3);
	gfs_command.id = command.substr(8, 3);

	if (category == "flr") { // Flight Runner
		gfs_command.category = GFSCommand::CommandCategory::FLR;
	} else if (category == "tlm") { // Telemetry
		gfs_command.category = GFSCommand::CommandCategory::TLM;
	} else if (category == "ext") { // Extension
		gfs_command.category = GFSCommand::CommandCategory::EXT;
	} else if (category == "mdl") { // Module
		gfs_command.category = GFSCommand::CommandCategory::MDL;
	} else if (category == "dat") {
		gfs_command.category = GFSCommand::CommandCategory::DAT;
	} else {
		command_queue_lock_.unlock();
		error("CMD_C", category); // Command category is not valid
		return;
	}

	// Add the command argument if it exists
	int size = command.size(); 
	if (size > 12) {
		gfs_command.arg = command.substr(12, size - 12);
	} else {
		gfs_command.arg = "";
	}

	// Add the command to the queue
	command_queue_.push(gfs_command);
	command_queue_lock_.unlock();
	return;
}

bool DataStream::getNextCommand(GFSCommand &command) {
	command_queue_lock_.lock();
	if (command_queue_.empty()) {
		command_queue_lock_.unlock();
		return false; // Return false if there are no commands in the queue
	}
	command = command_queue_.front(); 
	command_queue_.pop();
	command_queue_lock_.unlock();
	return true;
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
		std::string data_value) {
	std::time_t current_time = std::time(nullptr);
	data_stream_lock_.lock(); // Lock the data stream to prevent other threads from accessing it when adding an item to it
	data_stream_.push({data_source, data_name, data_value, current_time});
	num_data_packets_++;
	total_data_packets_++;
	data_stream_lock_.unlock(); // Unlock the data stream to make it available
}

void DataStream::addData(std::string data_source, GPSFrame gps_frame) {
	gps_data_stream_lock_.lock();
	gps_frame.source = data_source;
	gps_data_stream_.push(gps_frame);
	num_gps_packets_++;
	total_gps_packets_++;
	gps_data_stream_lock_.unlock();
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

	// Handle the transmission log
	tx_log_lock_.lock();
	tx_log_.push_back(tx); // Add the transmission to the log
	last_tx_in_log_ = tx.tx_num;

	int tx_log_size = (last_tx_in_log_ - first_tx_in_log_) + 1;
	if ((unsigned int)tx_log_size != tx_log_.size() && first_tx_in_log_ != 0) {
		addError("DS", "TX_LOG_SZ", std::to_string(tx_log_size) + " " + std::to_string(tx_log_.size()), 0);
	}
	
	// Remove old transmissions from the log if it is too big
	if (tx_log_size > kTXLogSize_) {
        first_tx_in_log_ = tx_log_.front().tx_num + 1;
        tx_log_.pop_front();
    }
	tx_log_lock_.unlock();
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

void DataStream::lockTXLog() {
	tx_log_lock_.lock();
}

const std::deque<Transmission>& DataStream::getTXLog() {
	return tx_log_;
}

void DataStream::unlockTXLog() {
	tx_log_lock_.unlock();
}

const DataStream::TXLogInfo DataStream::getTXLogInfo() {
	TXLogInfo info;
	tx_log_lock_.lock();
	info.first_tx_in_log = first_tx_in_log_;
	info.last_tx_in_log = last_tx_in_log_;
	info.tx_log_size = tx_log_.size();
	info.max_size = kTXLogSize_;
	tx_log_lock_.unlock();
	return info;
}

bool DataStream::requestTXFromLog(int tx_num, Transmission& tx) {
	if (tx_num < 0) {
		return false;
	}
	tx_log_lock_.lock();
	if (tx_num < first_tx_in_log_ || tx_num > last_tx_in_log_) {
		tx_log_lock_.unlock();
		return false;
	}
	int index = tx_num - first_tx_in_log_;
	if (index >= (int)tx_log_.size() || index < 0) {
		tx_log_lock_.unlock();
		return false;
	}

	tx = tx_log_[index];
	tx_log_lock_.unlock();
	return true;
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

bool DataStream::getNextGPSFrame(GPSFrame& frame) {
	gps_data_stream_lock_.lock();
	if (gps_data_stream_.size() == 0) {
		gps_data_stream_lock_.unlock();
		return false;
	}
	frame = gps_data_stream_.front();
	gps_data_stream_.pop();
	num_gps_packets_--;
	gps_data_stream_lock_.unlock();
	return true;
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

FlightProcedure DataStream::getCurrentFlightProcedure() {
	flight_procedure_lock_.lock();
	FlightProcedure flight_procedure(flight_procedure_);
	flight_procedure_lock_.unlock();
	return flight_procedure;
}

int DataStream::getNumGPSPackets() {
	return num_gps_packets_;
}

int DataStream::getTotalGPSPackets() {
	return total_gps_packets_;
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

int DataStream::getNumCurrentErrors() {
	error_frame_lock_.lock();
	int num_errors = error_frame_.size();
	error_frame_lock_.unlock();
	return num_errors;
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
	extension_status_.insert_or_assign(extension_name, status);
	extension_status_lock_.unlock();
}

void DataStream::updateModuleStatus(std::string module_name, ModuleStatus status) {
	//module_status_lock_.lock();
	module_status_.insert_or_assign(module_name, status);
	//module_status_lock_.unlock();
}

std::unordered_map<std::string, ExtensionStatus> DataStream::getExtensionStatuses() {
	extension_status_lock_.lock();
	std::unordered_map<std::string, ExtensionStatus> extension_status(extension_status_);
	extension_status_lock_.unlock();
	return extension_status;
}

std::unordered_map<std::string, ModuleStatus> DataStream::getModuleStatuses() {
	module_status_lock_.lock();
	std::unordered_map<std::string, ModuleStatus> module_status(module_status_);
	module_status_lock_.unlock();
	return module_status;
}

void DataStream::updateCriticalData(CriticalData &critical_data) {
	critical_data_lock_.lock();
	critical_data_ = critical_data;
	critical_data_lock_.unlock();
}

CriticalData DataStream::getCriticalData() {
	critical_data_lock_.lock();
	CriticalData critical_data(critical_data_);
	critical_data_lock_.unlock();
	return critical_data;
}

std::mutex& DataStream::getI2CBusLock() {
	return i2c_bus_lock_;
}

void DataStream::error(std::string code) {
	static const std::string kDataStreamErrorPrefix = "DS_";
	addError(kDataStreamErrorPrefix, code, "", 0);
}

void DataStream::error(std::string code, std::string info) {
	static const std::string kDataStreamErrorPrefix = "DS_";
	addError(kDataStreamErrorPrefix, code, info, 0);
}

void DataStream::UpdateLatestImage(const std::string& path) {
	image_files_lock_.lock();
	latest_image_path_ = path;
	image_files_lock_.unlock();
}

const std::string DataStream::GetLatestImage() {
	image_files_lock_.lock();
	std::string path = latest_image_path_;
	image_files_lock_.unlock();
	return path;
}

std::ostream& operator << (std::ostream& o, const DataStreamPacket& d)
{
	o << "Source: " << d.source << "\tData Name: " << d.unit <<
	"\tValue: " << d.value << std::endl;
	return o;
}

std::ostream& operator << (std::ostream& o, const ErrorStreamPacket& e)
{
    o << "Source: " << e.source << "\tError Code: " << e.error_code <<
    "\t Info: " << e.info << std::endl;
    return o;
}