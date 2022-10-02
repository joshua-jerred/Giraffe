#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <string>
#include <thread>
#include <atomic>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

enum class ExtensionStatus { kStarting = 0, kNormal = 1, kError = 2, kStopped = 3 };

class Extension {
public:
    Extension(DataStream *pDS, ExtensionMetadata configData);
    virtual ~Extension();
    void start(); // Must be overridden
    void stop();
    void restart();
    int getID();
    std::string getName();
    std::string getType();
    ExtensionMetadata::Interface getInterface();
    int getCritical();
    ExtensionStatus getStatus();

private:
    void setID(int num);
    void setName(std::string name);
    void setType(std::string type);
    void setCategory(ExtensionMetadata::Category category);
    void setInterface(ExtensionMetadata::Interface interface);
    void setUpdateInterval(int interval);
    void setCritical(int critical);
    void setStatus(ExtensionStatus status);

    int runner(std::atomic<int> &flag_); // Must be overridden
    void spawnRunner();

    int id_;
    std::string name_;
    std::string type_;
    ExtensionMetadata::Interface interface_;
    int critical_;
    ExtensionStatus status_;


    std::atomic<int> flag_;
    std::thread runner_thread_;
    DataStream  *mDataStream;
};

#endif