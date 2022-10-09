/**
 * @file extension.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the implementation the Extension class.
 * @details The Extension class is the base class for all extensions.
 * @date 2022-10-08
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */

#include "extension.h"

/**
 * @brief Constructor for the Extension base class.
 * @details This constructor sets all needed members and configures the 
 * extension but it does not start it.
 * The status of the extension is initially set to STOPPED but may be set to
 * 'ERROR' if the extension configuration fails.
 * @param DataStream* p_data_stream - Pointer to the data stream.
 * @param ExtensionMetadata extension_metadata - Metadata for the extension.
 */
Extension::Extension(DataStream *p_data_stream, 
                     ExtensionMetadata extension_metadata) {
    
    m_data_stream = p_data_stream;

    setStatus(ExtensionStatus::STOPPED);
    setName(extension_metadata.name);
    setID(extension_metadata.id);
    setType(extension_metadata.extension_type);
    setUpdateInterval(extension_metadata.update_interval);
    setInterface(extension_metadata.interface);
    setCritical(extension_metadata.critical);

}

/**
 * @brief Destructor for the Extension base class.
 * @todo Stop the thread if it is running.
 */
Extension::~Extension() {

}

/**
 * @brief Starts the extension thread if it is stopped. This should change the
 * status to 'STARTING' and the thread will change it to 'RUNNING' or 'ERROR'.
 * @param None.
 * @return void
 */
void Extension::start() {
    if (getStatus() == ExtensionStatus::STOPPED) {
        setStatus(ExtensionStatus::STARTING);
        spawnRunner();
    }
}

/**
 * @brief Not implemented.
 * @details Stops the extension thread and sets the status based on the result.
 * @todo Implement this.
 */
void Extension::stop() {
}

/**
 * @brief Not implemented.
 * @details Restarts the extension thread and sets the status based on the
 * result.
 * @todo Implement this.
 */
void Extension::restart() {
}

/**
 * @return std::string The name of the extension.
 */
std::string Extension::getName() {
    return name_;
}

/**
 * @return int The id of the extension.
 */
int Extension::getID() {
    return id_;
}

/**
 * @return std::string The type of the extension.
 */
std::string Extension::getType() {
    return type_;
}

/**
 * @return int The update interval in milliseconds of the extension.
 */
int Extension::getUpdateInterval() {
    return update_interval_;
}

/**
 * @return ExtensionMetadata::Interface The interface of the extension.
 */
ExtensionMetadata::Interface Extension::getInterface() {
    return interface_;
}

/**
 * @return int Whether or not the extension is flight critical 0=no, 1=yes.
 */
int Extension::getCritical() {
    return critical_;
}

/**
 * @return ExtensionStatus The status of the extension.
 */
ExtensionStatus Extension::getStatus() {
    return status_;
}

/**
 * @brief Sends a the data to the data stream.
 * @details This function is used to send data from the extension to the data
 * stream. It is a wrapper for the DataStream::addData() function. It fills
 * in the extension name and the valid time in seconds which is defined as
 * 'getUpdateInterval() / 1000'.
 * @bug This function does not take into account the time it may take an
 * extension to collect it's data.
 * @param std::string unit - The unit of the data. Example "TF" for temperature 
 * in fahrenheit. This is defined in the configuration.
 * @param std::string value
 * @param int value
 * @param float value
 * 
 */
void Extension::sendData(std::string unit, std::string value) {
    std::time_t t = std::time(0);
    mDataStream->addData(getName(), unit, value, getUpdateInterval() / 1000);
}
/**
 * @brief Override 
 */
void Extension::sendData(std::string unit, int value) {
    mDataStream->addData(getName(), unit, std::to_string(value), getUpdateInterval() / 1000);
}

/**
 * @brief Override 
 */
void Extension::sendData(std::string unit, float value) {
    mDataStream->addData(getName(), unit, std::to_string(value), getUpdateInterval() / 1000);
}

/**
 * @todo Range checking on all setters
 */
void Extension::setName(std::string name) {
    if ((name.size() <= 10) && (name.size() > 2)) {
        name_ = name;
        return;
    } else {
        mDataStream->addError(name, "EXT_NAME", "Specified name out of range.");
        name_ = "NULL";
        setStatus(ExtensionStatus::kError);
        return;
    }
}

void Extension::setID(int num) {
    if ((num > 0) && (num <= 255)) {
        id_ = num;
        return;
    } else {
        id_ = 0;
        setStatus(ExtensionStatus::kError);
        mDataStream->addError(name_, "EXT_ID", "Specified ID out of range.");
        return;
    }
}

void Extension::setStatus(ExtensionStatus status) {
    status_ = status;
    return;
}

void Extension::setType(std::string type){
    type_ = type;
    return;
}

void Extension::setUpdateInterval(int interval){
    update_interval_ = interval;
}

void Extension::setInterface(ExtensionMetadata::Interface interface){

    interface_ = interface;
    return;
}

void Extension::setCritical(int critical){
    critical_ = critical;
    return;
}



int Extension::runner() {
    std::cout << "Exthension: " << name_ << " has a misconfigured runner() method." << std::endl;
    std::cout << "This is from extension.cpp runner." << std::endl;
    return -1;
}

void Extension::spawnRunner() {
    runner_thread_ = std::thread(&Extension::runner, this);
}