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
#include "utility-status.h"

#include "extension.h"

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
    Extension(const Extension& other) = delete; // No copy constructor
    Extension& operator=(const Extension& other) = delete; // No copy assignment
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

    //template <typename T>
    //void error(std::string error_code, T info);
    void error(std::string error_code, std::string info);
    void error(std::string error_code);

    std::atomic<int> stop_flag_ = 1; // 0 = continue, 1 = stop

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

    DataStream * const p_data_stream_;

    std::thread runner_thread_ = std::thread();
    std::atomic<ExtensionStatus> status_;

    int id_ = 0;
    std::string name_ = "";
    std::string type_ = "";
    ExtensionMetadata::Category category_ = ExtensionMetadata::Category::ERROR;
    ExtensionMetadata::Interface interface_ = ExtensionMetadata::Interface::ERROR;
    int update_interval_ = 0;
    int data_expiration_time_ = 1000;
    int critical_ = 0;
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
