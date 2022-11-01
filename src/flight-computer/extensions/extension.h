/**
 * @file extension.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief This file declares the Extension class.
 * 
 * @version 0.0.9
 * @date 2022-10-09
 * @copyright Copyright (c) 2022
 */

#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>
#include <ctime>

#include "utility-configurables.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

/**
 * @brief This is the enum used to communicate with the extension runner.
 * @details When an extension is constructed it's status will be set to stopped.
 * When Extension::start() is called the status will be set to starting.
 * The Extension::runner() (of the derived class) will be called and the status
 * should be changed to running. When the ExtensionModule calls 
 * Extension::stop() the status will change to 'STOPPING'. The 
 * Extension::runner() is then responsible for deconstructing and changing the
 * status to 'STOPPED' right before it exits.
 * 
 * If any errors occur the status will be set to 'ERROR' and there will be an
 * error message in the data stream.
 */
enum class ExtensionStatus { 
    ERROR = 0,
    STOPPED = 1, 
    STARTING = 2, 
    RUNNING = 3,
    STOPPING = 4
    };

/**
 * @brief This class is the base class for all extensions. Extensions include
 * all sensors, actuators, radios, internal data collection tools, etc.
 * 
 * This class is responsible for setting up the extension. The extension
 * will have a runner function that will be called in it's own thread.
 * 
 * The runner function is responsible for reading data from sensors and
 * date collection tools and writing that data to the data stream.
 * @see simple-extension.cpp for example usage.
 */
class Extension {
public:
    Extension(DataStream *p_data_stream, ExtensionMetadata extension_metadata);
    virtual ~Extension();

    void start();
    void stop();
    void restart();
    ExtensionStatus getStatus();

    int getID();
    std::string getName();
    std::string getType();
    ExtensionMetadata::Category getCategory();
    ExtensionMetadata::Interface getInterface();
    int getUpdateInterval();
    int getCritical();
    std::vector<std::string> getExtraArgs(); /** @todo Implement this. */

protected:
    virtual int runner(); // Must be overridden
    void setStatus(ExtensionStatus status);

    void sendData(std::string unit, std::string value);
    void sendData(std::string unit, int value);
    void sendData(std::string unit, float value);

    template <typename T>
    void error(std::string error_code, T info);
    void error(std::string error_code, std::string info);
    void error(std::string error_code);

    std::atomic<int> stop_flag_; // 0 = continue, 1 = stop

private:
    void setID(int num);
    void setName(std::string name);
    void setType(std::string type);
    void setCategory(ExtensionMetadata::Category category);
    void setInterface(ExtensionMetadata::Interface interface);
    void setUpdateInterval(int interval);
    void setCritical(int critical);
    void setExtraArgs(std::vector<std::string> extra_args); /** @todo Implement this. */


    virtual void spawnRunner();

    DataStream  *p_data_stream_;

    std::thread runner_thread_;
    std::atomic<ExtensionStatus> status_;

    int id_;
    std::string name_;
    std::string type_;
    ExtensionMetadata::Category category_;
    ExtensionMetadata::Interface interface_;
    int update_interval_;
    int critical_;
};

class ExtensionException {
public:
    ExtensionException(std::string s) : m_s(s) {};
    ~ExtensionException() {};

    std::string description() { return m_s; }

private:
    std::string m_s;
};

#endif // EXTENSION_H_
