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
    setInterface(extension_metadata.interface);
    setCritical(extension_metadata.critical);
    flag_ = 1;
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

ExtensionMetadata::Interface Extension::getInterface() {
    return interface_;
}

int Extension::getCritical() {
    return critical_;
}

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

void Extension::setInterface(ExtensionMetadata::Interface interface){

    interface_ = interface;
    return;
}

void Extension::setCritical(int critical){
    critical_ = critical;
    return;
}

int Extension::runner() {
    std::cout << "This is from extension.cpp runner." << std::endl;
    return -1;
}

void Extension::spawnRunner() {
    std::cout << "Exthension: " << name_ << " has a misconfigured spawnRunner() method." << std::endl;
    // Must be overridden with something like this
    //runner_thread_ = std::thread(&Extension::runner, this, std::ref(flag_));
}