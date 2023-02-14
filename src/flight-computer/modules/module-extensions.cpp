
/**
 * @file module-extensions.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Defines the ExtensionsModule class.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */
#include <vector>
#include "extensions.h"

#include "modules.h"
using namespace modules;

/**
 * @brief Construct a new ExtensionsModule object.
 * @details Uses the config_data to create each extension and add it to the
 * extension vector. This does not start the extensions.
 * @param config_data 
 * @param stream 
 */
ExtensionsModule::ExtensionsModule(const Data config_data, DataStream &stream):
    Module(stream, MODULE_EXTENSION_PREFIX, "extensions"),
    config_data_(config_data) {

    for (ExtensionMetadata extdata : config_data.extensions.extensions_list) {
        addExtension(extdata);
    }
}

/**
 * @brief Destroys the ExtensionsModule object.
 */
ExtensionsModule::~ExtensionsModule() {
    for (extension::Extension *ext : extensions_) {
        delete ext;
    }
}

/**
 * @brief Starts the ExtensionsModule.
 * @details Starts each extension in the extension vector.
 * @param None
 * @return void
 */
void ExtensionsModule::start() {
    updateStatus(ModuleStatus::STARTING);
    stop_flag_ = 0;
    runner_thread_ = std::thread(&ExtensionsModule::runner, this);
}

/**
 * @brief Stops all extensions.
 * @param None
 * @return void
 */
void ExtensionsModule::stop() {
    updateStatus(ModuleStatus::STOPPING);
    std::cout << std::endl;
    if (status() == ModuleStatus::STOPPED && !runner_thread_.joinable()) {
		return;
	}
	stop_flag_ = 1;
	if (runner_thread_.joinable()) {
		runner_thread_.join();
	}
	updateStatus(ModuleStatus::STOPPED);
}

void ExtensionsModule::runner() {
    const int max_restart_count = 5;
    //const int restart_delay_seconds = 10;

    for (extension::Extension *ext : extensions_) {
        ext->start();
    }

    ExtensionStatus ext_status;

    int total_extensions = 0;
    int num_error = 0;
    int num_stopped_error_state = 0;
    int num_stopped = 0;
    int num_starting = 0;
    int num_running = 0;
    int num_stopping = 0;

    while (!stop_flag_) {
        updateStatus(ModuleStatus::RUNNING);
        total_extensions = 0;
        num_error = 0;
        num_stopped_error_state = 0;
        num_stopped = 0;
        num_starting = 0;
        num_running = 0;
        num_stopping = 0;

        for (extension::Extension *ext : extensions_){
            total_extensions++;
            ext_status = ext->getStatus();
            if (ext_status == ExtensionStatus::ERROR) {
                ext->restart_attempts_++;
                ext->stop();
                num_error++;
            } else if (ext_status == ExtensionStatus::STOPPED_ERROR_STATE) {
                if (ext->restart_attempts_ < max_restart_count || ext->getCritical() == true) {
                    ext->restart_attempts_++;
                    ext->start();
                } else {
                    num_stopped_error_state++;
                }
                num_stopped_error_state++;
            } else if (ext_status == ExtensionStatus::STOPPED) {
                num_stopped++;
            } else if (ext_status == ExtensionStatus::STARTING) {
                num_starting++;
            } else if (ext_status == ExtensionStatus::RUNNING) {
                num_running++;
            } else if (ext_status == ExtensionStatus::STOPPING) {
                num_stopping++;
            } else {
                num_error++;
                error("STS");
            }
        }

        data("ext_tot", total_extensions);
        data("ext_err", num_error);
        data("ext_stp", num_stopped);
        data("ext_strt", num_starting);
        data("ext_run", num_running);
        data("ext_stpd", num_stopping);
        data("ext_stp_err", num_stopped_error_state);

        module_sleep();
    }

    module_status_ = ModuleStatus::STOPPING;
    // Shutdown
    for (extension::Extension *ext : extensions_) {
        std::cout << "\tStopping extension " << ext->getName() << " ... ";
        ext->stop();
        if (ext->getStatus() == ExtensionStatus::STOPPED) {
            std::cout << "Stopped" << std::endl;
        } else {
            std::cout << "Error" << std::endl;
        }
    }
}

/**
 * @brief This function uses the extension metadata to create an extension of
 * the correct type.
 * @param meta_data The meta_data for one extension.
 * @return void
 */
void ExtensionsModule::addExtension(ExtensionMetadata meta_data) {
    if (meta_data.extension_type == "PRESS_SENSOR_SIM") {
        extensions_.push_back(new extension::PRESS_SENSOR_SIM(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "TEMP_SENSOR_SIM") {
        extensions_.push_back(new extension::TEMP_SENSOR_SIM(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "RADIO_SIM") {
        extensions_.push_back(new extension::RADIO_SIM(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "GPS_SIM") {
        extensions_.push_back(new extension::GPS_SIM(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "BATT_SENSOR_SIM") {
        extensions_.push_back(new extension::BATT_SENSOR_SIM(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "BMP180") {
        extensions_.push_back(new extension::BMP180(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "DS18B20") {
        extensions_.push_back(new extension::DS18B20(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "SAMM8Q") {
        extensions_.push_back(new extension::SAMM8Q(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "BME280") {
        extensions_.push_back(new extension::BME280(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "SYSINFO") {
        extensions_.push_back(new extension::SYSINFO(&data_stream_, meta_data));
    } else if (meta_data.extension_type == "MCP3021") {
        extensions_.push_back(new extension::MCP3021(&data_stream_, meta_data));
    } else {
        error("Extension type not found", meta_data.name);
    }
}