/**
 * @file extension.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file contains the implementation of the Extension class.
 * 
 * @version 0.0.9
 * @date 2022-10-09
 * @copyright Copyright (c) 2022
 */

#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-status.h"
#include "extensions.h"

/**
 * @brief Constructor for the Extension base class.
 * @details This constructor sets all needed members and configures the 
 * extension but it does not start it.
 * The status of the extension is initially set to STOPPED but may be set to
 * 'ERROR' if the extension configuration fails.
 * @param p_data_stream - Pointer to a DataSteam object.
 * @param extension_metadata - ExtensionMetadata struct with the extension 
 * config info
 */
extension::Extension::Extension(DataStream *p_data_stream, 
                     ExtensionMetadata extension_metadata):
    p_data_stream_(p_data_stream),
    status_(ExtensionStatus::STOPPED) {

    setID(extension_metadata.id);
    setName(extension_metadata.name);
    setType(extension_metadata.extension_type);
    setCategory(extension_metadata.category);
    setInterface(extension_metadata.interface);
    setUpdateInterval(extension_metadata.update_interval);
    setCritical(extension_metadata.critical);

    setStatus(ExtensionStatus::STOPPED);
}

/**
 * @brief Destructor. Stop must be called first.
 * @todo Stop the thread if it is running, check for errors.
 * @bug Does not attempt to stop the thread.
 */
extension::Extension::~Extension() {

}

/**
 * @brief Starts the extension thread if it is stopped, changes status. 
 * @details This function attempts to start the extension thread with 
 * spawnRunner().
 * This will change the status to 'STARTING'. The thread is responsible 
 * for the later change to 'RUNNING' or 'ERROR'.
 * @param None
 * @return void
 */
void extension::Extension::start() {
    if (getStatus() == ExtensionStatus::STOPPED) {
        setStatus(ExtensionStatus::STARTING);
        spawnRunner();
    }
}

/**
 * @brief Signals the thread to stop.
 * @details Sets the status to 'STOPPING' which signals the thread to stop.
 * The thread is responsible for the later change to 'STOPPED' or 'ERROR'.
 * This function does not wait for the thread to stop, it only changes the
 * status. This functionality needs to be implemented in each extension's
 * runner() method.
 * @todo Implement thread joining.
 * @param None
 * @return void
 */
void extension::Extension::stop() {
    if (getStatus() == ExtensionStatus::RUNNING) {
        setStatus(ExtensionStatus::STOPPING);
        stop_flag_ = 1;
        runner_thread_.join();
    } else if (getStatus() == ExtensionStatus::ERROR) {
        runner_thread_.join();
    }
}

/**
 * @brief Not implemented.
 * @details Restarts the extension thread and sets the status based on the
 * result.
 * @todo Implement this.
 */
void extension::Extension::restart() {
}

/**
 * @return ExtensionStatus The status of the extension. Status is an atomic,
 * so it is thread safe.
 */
ExtensionStatus extension::Extension::getStatus() {
    return status_;
}

/**
 * @return int The id of the extension.
 */
int extension::Extension::getID() {
    return id_;
}

/**
 * @return std::string The name of the extension.
 */
std::string extension::Extension::getName() {
    return name_;
}

/**
 * @return std::string The type of the extension.
 */
std::string extension::Extension::getType() {
    return type_;
}

/**
 * @return ExtensionMetadata::Category The category of the extension.
 */
ExtensionMetadata::Category extension::Extension::getCategory() {
    return category_;
}

/**
 * @return ExtensionMetadata::Interface The interface of the extension.
 */
ExtensionMetadata::Interface extension::Extension::getInterface() {
    return interface_;
}

/**
 * @return int The update interval in milliseconds of the extension.
 */
int extension::Extension::getUpdateInterval() {
    return update_interval_;
}

/**
 * @return int Whether or not the extension is flight critical 0=no, 1=yes.
 */
int extension::Extension::getCritical() {
    return critical_;
}

/**
 * @brief This must be overridden by each extension.
 * @details This function is called by by spawnRunner() to start the thread.
 * The runner belongs to each extension so the base class runner should never
 * be called. The runner function override should contain an endless loop that
 * generates data for the data stream. It should only continue the loop at a
 * rate that is equal to 'update_interval_'. The runner must also check to see
 * if the status is 'stopping', if it is it must shutdown and return 0.
 * If this function is not overridden, it will add an error to the data stream 
 * and return -1.
 * @return int -1, an error value. 0 is for a clean stop.
 */
int extension::Extension::runner() {
    setStatus(ExtensionStatus::ERROR);
    p_data_stream_->addError(name_, "EXT_RUNNER", "Runner not implemented in child class.", 0);
    return -1;
}

/**
 * @brief Sets the status. Thread safe.
 * @details Sets the status, no error checking is done. The status member
 * is protected as a std::atomic<> so it is thread safe. This is called within
 * the base class and parent class.
 * @param status 
 */
void extension::Extension::setStatus(ExtensionStatus status) {
    status_ = status;
    p_data_stream_->updateExtensionStatus(getName(), status);
}

/**
 * @brief Sends data to the data stream.
 * @details This function is used to send data from the extension to the data
 * stream. It is a wrapper for the DataStream::addData() function. It fills
 * in the extension name and the valid time in seconds which is defined as
 * 'getUpdateInterval() / 1000'.
 * @bug This function does not take into account the time it may take an
 * extension to collect it's data. Also send data could possibly be put
 * into a template.
 * @param std::string unit - The unit of the data. Example "TF" for temperature 
 * in fahrenheit. This is defined in the configuration.
 * @param std::string value
 * @param int value
 * @param float value
 * @return void
 */
void extension::Extension::sendData(std::string unit, std::string value) {
    p_data_stream_->addData(getName(), unit, value, 
                            data_expiration_time_);
}

/**
 * @brief Override for integers. See sendData(std::string, std::string).
 * @todo Limit number size?
 */
void extension::Extension::sendData(std::string unit, int value) {
    p_data_stream_->addData(getName(), unit, std::to_string(value), 
                            data_expiration_time_);
}

/**
 * @brief Override for floats. See sendData(std::string, std::string).
 * @todo Limit to float precision?
 */
void extension::Extension::sendData(std::string unit, float value, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    std::string rounded = stream.str();
    p_data_stream_->addData(getName(), unit, rounded, 
                            data_expiration_time_);
}

/**
 * @brief Sets the ID of the extension, reports errors to the data stream.
 * @details This does not check for duplicates.
 * @param num The id
 * @return void
 */
void extension::Extension::setID(int num) {
    if ((num >= EXTENSION_ID_MIN) && (EXTENSION_ID_MAX <= 255)) {
        id_ = num;
        return;
    } else {
        id_ = 0;
        setStatus(ExtensionStatus::ERROR);
        p_data_stream_->addError(name_, "EXT_ID", 
                                 "ID: " + std::to_string(num), 0);
        return;
    }
}

/**
 * @brief Sets the name of the extension, reports errors to the data stream.
 * @details Uses the extension max length and min length macros to validate the 
 * name. If the name is invalid, the status is set to ERROR, name is set to
 * "NAME_ERROR" and the error is reported to the data stream. This error should
 * be caught by the configuration module before it gets here.
 * @todo Add a check for valid characters (Is there a standard yet?).
 * @param name The name of the extension.
 * @return void
 */
void extension::Extension::setName(std::string name) {
    if ((name.size() <= EXTENSION_NAME_MAX_LENGTH) 
        && (name.size() >= EXTENSION_NAME_MIN_LENGTH)) {
        name_ = name;
        return;
    } else {
        p_data_stream_->addError(name, "EXT_NAME", 
                                 "Specified name out of range.", 0);
        name_ = "NAME_ERROR";
        setStatus(ExtensionStatus::ERROR);
        return;
    }
}

/**
 * @brief Sets the type of the extension.
 * @details Does not do any error checking.
 * @todo Add length and character checking.
 * @param type
 * @return void
 */
void extension::Extension::setType(std::string type){
    type_ = type;
    return;
}

/**
 * @brief Sets the category of the extension, does not do any error checking.
 * @param category 
 * @return void
 */
void extension::Extension::setCategory(ExtensionMetadata::Category category) {
    category_ = category;
    return;
}

/**
 * @brief Sets the interface of the extension that will be used for device
 * communication.
 * @param interface
 * @return void
 * 
 * @todo Add extra args handling here if for interface. Ex: i2c bus and address.
 */
void extension::Extension::setInterface(ExtensionMetadata::Interface interface){
    interface_ = interface;
    return;
}

/**
 * @brief Sets the update interval of the extension.
 * @details Currently does not do any error checking.
 * @todo Add range checking.
 * @param interval The update interval in milliseconds, 0 means the thread will
 * return after a single run.
 */
void extension::Extension::setUpdateInterval(int interval_ms){
    update_interval_ = interval_ms;
    if (update_interval_ < 0) {
        update_interval_ = 10000;
        error("EXT_UPDATE_INTERVAL", "Update interval reverted to 10s");
    }
    data_expiration_time_ = (update_interval_/1000) * 2;
}

/**
 * @brief Sets the critical flag of the extension 1=yes, 0=no.
 * @details If the extension is flight critical and the status is 'ERROR' then
 * the system should attempt to recover. This is/will be implemented in the
 * health_check() function.
 * @see health_check()
 * @param critical 1=yes, 0=no.
 */
void extension::Extension::setCritical(int critical){
    if (critical != 0 && critical != 1) {
        critical_ = 0;
        p_data_stream_->addError(name_, "EXT_CRITICAL", 
                                 "Critical flag out of range.", 0);
        return;
    }
    critical_ = critical;
    return;
}

/**
 * @brief This function will attempt to spawn the runner thread of the
 * parent class.
 */
void extension::Extension::spawnRunner() {
    stop_flag_ = 0;
    runner_thread_ = std::thread(&Extension::runner, this);
}

//template <typename T>
//void Extension::error(std::string error_code, T info) {
//	p_data_stream_->addError(
//        EXTENSION_PREFIX + std::to_string(getID()), 
//        error_code, 
//		std::to_string(info), 
//        update_interval_
//        );
//}

void extension::Extension::error(std::string error_code, std::string info) {
	p_data_stream_->addError(
        EXTENSION_PREFIX + std::to_string(getID()), 
        error_code, 
        info, 
        data_expiration_time_
        );
}

void extension::Extension::error(std::string error_code) {
	p_data_stream_->addError(
        EXTENSION_PREFIX + std::to_string(getID()), 
        error_code, 
        "", 
        data_expiration_time_
        );
}