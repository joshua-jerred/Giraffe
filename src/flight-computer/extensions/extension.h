#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>
#include <ctime>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

enum class ExtensionStatus { ERROR = 0, STOPPED = 1, STARTING = 2, RUNNING = 3, STOPPED = 4 };

class Extension {
public:
    Extension(DataStream *pDS, ExtensionMetadata configData);
    virtual ~Extension();
    void start();
    void stop();
    void restart();
    int getID();
    std::string getName();
    std::string getType();
    int getUpdateInterval();
    ExtensionMetadata::Interface getInterface();
    int getCritical();
    ExtensionStatus getStatus();

protected:
    void sendData(std::string unit, std::string value);
    void sendData(std::string unit, int value);
    void sendData(std::string unit, float value);

    std::atomic<int> flag_;
    DataStream  *m_data_stream;
    virtual int runner(); // Must be overridden
    virtual void spawnRunner();
    void setStatus(ExtensionStatus status);


private:
    void setID(int num);
    void setName(std::string name);
    void setType(std::string type);
    void setCategory(ExtensionMetadata::Category category);
    void setInterface(ExtensionMetadata::Interface interface);
    void setUpdateInterval(int interval);
    void setCritical(int critical);


    std::thread runner_thread_;
    int id_;
    std::string name_;
    std::string type_;
    int update_interval_;
    ExtensionMetadata::Interface interface_;
    int critical_;
    ExtensionStatus status_;
};

#endif