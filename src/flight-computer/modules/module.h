/**
 * @file module.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Defines the enum module_status and parent class for the modules.
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 * @defgroup flight-computer-modules
 * @addtogroup flight-computer-modules
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <string>
/**
 * @brief module_status is used by the individual modules. Used by FlightRunner.
 * @see FlightRunner
 */
enum class ModuleStatus {
    stopped=0,
    starting=1,
    running=2,
    error_state=3 
};

/**
 * @brief All modules used by the FlightRunner inherit this class. This allows
 * for some simplicity when creating small modules.
 */
class Module {
    public:
        Module( ) { };
        virtual ~Module( ) { };
        ModuleStatus status( ) { return module_status_; };
        virtual void start( ) { module_status_ = ModuleStatus::error_state; };
        virtual void stop( ) { module_status_ = ModuleStatus::error_state; };


    protected:
        ModuleStatus module_status_;
        std::string error_source_;
};

#endif