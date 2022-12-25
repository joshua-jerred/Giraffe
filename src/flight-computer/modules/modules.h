/**
 * @file modules.h
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
#include <thread>

#include "utility-status.h"
#include "utility-data-stream.h"
#include "utility-configurables.h"
#include "utility-socket.h"

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

/**
 * @brief This class is responsible for handling the radio
 * and transmitting data upon request. Currently only supports PSK.
 * @todo Two way communication, AFSK, APRS, etc.
 */
class TelemetryModule : public Module {
public:
    TelemetryModule(ConfigData config_data, DataStream *data_stream);
    TelemetryModule(const TelemetryModule &other) = delete; // No copy constructor
    TelemetryModule &operator=(const TelemetryModule &other) = delete; // No copy assignment
    ~TelemetryModule();

    void start();
    void stop();

    void sendDataPacket();
    void sendAFSK(std::string message);
    void sendPSK(std::string message);
    void sendAPRS();
    void sendSSTVImage();

private:
    int getNextTXNumber();

    void addToTXQueue(Transmission transmission);

    std::string generateAFSK(std::string message);
    std::string generatePSK(std::string message);
    std::string generateAPRS();
    std::string generateSSTV();

    void runner();
    void playWav(std::string wav_location, std::string tx_type, int tx_length);
    FILE *aplay_fp_ = nullptr;


    int tx_number_ = 1;
    std::string call_sign_ = CALLSIGN_FAILSAFE;

    std::thread tx_thread_ = std::thread();
    std::atomic<int> stop_flag_ = 0;

    ConfigData config_data_;
    DataStream *p_data_stream_;

    int psk_length_ = 0;
};

class ServerModule : public Module {
public:
    ServerModule(const ConfigData config_data, DataStream *data);
    ServerModule(const ServerModule&) = delete; // No copy constructor
    ServerModule& operator=(const ServerModule&) = delete; // No copy assignment
    ~ServerModule();

    void start();
    void stop();

    int checkShutdown();

private:
    void runner();
    void sendStaticData(ServerSocket &socket);
    void sendDynamicData(ServerSocket &socket);

    ConfigData config_data_;
    DataStream* p_data_stream_;

    std::thread runner_thread_ = std::thread();
    //std::thread py_runner_thread_;

    /**
     * @details This flag is an atomic so it can be accessed by both the thread
     * and the main thread. It is set to 1 to signal the thread to stop.
     */
    std::atomic <int> stop_flag_ = 0;
    std::atomic <int> gfs_shutdown_flag_ = 0;
};
#endif