#include <atomic>
#include <thread>
#include <string>
#include <vector>
#include <iostream>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

Extension::Extension(DataStream *pDS, ExtensionMetadata extension_metadata) {
    mDataStream = pDS;
    setStatus(ExtensionStatus::kStopped);
    setName(extension_metadata.name);
    setID(extension_metadata.id);
    setType(extension_metadata.extension_type);
    setUpdateInterval(extension_metadata.update_interval);
    setInterface(extension_metadata.interface);
    setCritical(extension_metadata.critical);
}

Extension::~Extension() {
}

void Extension::start() {
    if (getStatus() == ExtensionStatus::kStopped) {
        setStatus(ExtensionStatus::kStarting);
        spawnRunner();
    }
}

void Extension::stop() {
}

void Extension::restart() {
}

std::string Extension::getName() {
    return name_;
}

int Extension::getID() {
    return id_;
}

ExtensionStatus Extension::getStatus() {
    return status_;
}

std::string Extension::getType() {
    return type_;
}

int Extension::getUpdateInterval() {
    return update_interval_;
}

ExtensionMetadata::Interface Extension::getInterface() {
    return interface_;
}

int Extension::getCritical() {
    return critical_;
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