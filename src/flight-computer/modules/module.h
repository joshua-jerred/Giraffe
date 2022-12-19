/**
 * @file module.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Defines the enum module_status and parent class for the modules.
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <string>
#include <atomic>

#include "utility-status.h"
#include "utility-data-stream.h"

/**
 * @brief All modules used by the FlightRunner inherit this class. This allows
 * for some simplicity when creating small modules.
 */
class Module {
    public:
        Module(DataStream *p_data_stream, std::string error_prefix):
            p_data_stream_(p_data_stream),
            module_status_(ModuleStatus::STOPPED),
            error_source_(error_prefix)
            { };

        Module(const Module&) = delete; // No copy constructor
        Module& operator=(const Module&) = delete; // No copy assignment
        virtual ~Module( ) { };

        ModuleStatus status( ) { return module_status_; };
        virtual void start( ) { module_status_ = ModuleStatus::ERROR_STATE; };
        virtual void stop( ) { module_status_ = ModuleStatus::ERROR_STATE; };

    protected:
        void error(std::string error_code, std::string info) {
            if (p_data_stream_ != nullptr) {
                p_data_stream_->addError("M_"+error_source_, error_code, info, 0);
            }
        };

        void error(std::string error_code) {
            if (p_data_stream_ != nullptr) {
                p_data_stream_->addError(error_source_, error_code, "", 0);
            }
        };

        DataStream *p_data_stream_;
        std::atomic<ModuleStatus> module_status_;

    private:
        std::string error_source_;
};

#endif