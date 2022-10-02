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

#include "utility-data-stream.h"

DataStream::DataStream() {
	mNumDataPacket = 0;
	mTotalDataPackets = 0;

	mNumErrorPacket = 0;
	mTotalErrorPackets = 0;

	mLock_dataStream.unlock(); // Unlock the data stream to make it available
	mLock_errorStream.unlock();	// Unlock the error stream to make it available
}

DataStream::~DataStream() {

}

void DataStream::addData( std::string dataSource, 
	std::string dataName, std::string dataValue) {
	mLock_dataStream.lock(); // Lock the data stream to prevent other threads from accessing it when adding an item to it
	mDataSteam.push({dataSource, dataName, dataValue});
	mNumDataPacket++;
	mTotalDataPackets++;
	mLock_dataStream.unlock(); // Unlock the data stream to make it available
}

void DataStream::addError( std::string errorSource, 
	std::string errorName, std::string errorInfo) {
	mLock_errorStream.lock();
	mErrorStream.push({errorSource, errorName, errorInfo});
	mNumErrorPacket++;
	mTotalErrorPackets++;
	mLock_errorStream.unlock();
}

data_stream_packet DataStream::getNextDataPacket() {
	mLock_dataStream.lock(); // lock while reading
	data_stream_packet packet;
	if (mDataSteam.size() == 0) { // If there are no packets in the stream, return an empty packet.
		mLock_dataStream.unlock();
		return packet;
	}
	packet = mDataSteam.front();
	mDataSteam.pop();
	mNumDataPacket--;
	mLock_dataStream.unlock();
	return packet;
}

error_stream_packet DataStream::getNextErrorPacket() {
	mLock_errorStream.lock();
	error_stream_packet packet;
	if (mErrorStream.size() == 0) {
		mLock_errorStream.unlock();
		return packet;
	}
	packet = mErrorStream.front();
	mErrorStream.pop();
	mNumErrorPacket--;
	mLock_errorStream.unlock();
	return packet;
}

int DataStream::getNumDataPackets() {
	return mNumDataPacket;
}

int DataStream::getNumErrorPackets() {
	return mNumErrorPacket;
}

int DataStream::getTotalDataPackets() {
	return mTotalDataPackets;
}

int DataStream::getTotalErrorPackets() {
	return mTotalErrorPackets;
}