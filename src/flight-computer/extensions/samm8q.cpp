/**
 * @file samm8q.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief The extension for the SAM-M8Q GPS module. Uses the ubx utility.
 * 
 * @version 0.3
 * @date 2023-01-06
 * @copyright Copyright (c) 2023
 */

#include "extensions.h"
#include "ubx.h"

#define SAMM8Q_I2C_ADDRESS 0x42

static const uint8_t kNavClass = 0x01;
static const uint8_t kNavPvt = 0x07; 

extension::SAMM8Q::SAMM8Q(DataStream *p_data_stream, ExtensionMetadata extension_metadata) :
    Extension(p_data_stream, extension_metadata),
    bus_number_(extension_metadata.extra_args.I2C_bus),
    device_address_(SAMM8Q_I2C_ADDRESS),
    i2c_(extension_interface::I2C(bus_number_, device_address_, p_data_stream->getI2CBusLock())) {
    
}

extension::SAMM8Q::~SAMM8Q() {

}

int extension::SAMM8Q::runner() {
    int result = i2c_.connect();
    data_expiration_time_ = 20; // Data is good for 20 seconds
	if (result != 0 || i2c_.status() != extension_interface::I2C_STATUS::OK) {
		setStatus(ExtensionStatus::ERROR);

		extension_interface::I2C_STATUS status = i2c_.status();
		if (status == extension_interface::I2C_STATUS::CONFIG_ERROR_BUS) {
			error("I2C_CB");
		} else if (status == extension_interface::I2C_STATUS::CONFIG_ERROR_ADDRESS) {
			error("I2C_CA");
		} else if (status == extension_interface::I2C_STATUS::BUS_ERROR) {
			error("I2C_BE");
		} else if (status == extension_interface::I2C_STATUS::ADDRESS_ERROR) {
			error("I2C_AE");
		} else {
			error("I2C_CU");
		}
		return -1;
	}

    ubx::UBXMessage msg;
    ubx::NAV_DATA nav_data;

    configured_ = false;
    int retry_count = 0;
    const int kMaxRetries = 5;
    while (stop_flag_ == false) {

        if (retry_count >= kMaxRetries) {
            setStatus(ExtensionStatus::ERROR);
            error("CFG_T");
            return -1;
        }

        if (configured_ == false && retry_count < kMaxRetries) {
            configured_ = configure();
            retry_count++;
        } else if (configured_ == true) {
            retry_count = 0;
        }

      while (stop_flag_ == false && configured_ == true) {
        setStatus(ExtensionStatus::RUNNING);
        if (ubx::pollMessage(i2c_, msg, kNavClass, kNavPvt, 92 + 8, 2000)
                && ubx::parsePVT(msg, nav_data)) { // Good Data
            GPSFrame gps_frame;

            gps_frame.time = std::to_string(nav_data.hour) + ":" + std::to_string(nav_data.minute) + ":" + std::to_string(nav_data.second);
            
            gps_frame.num_satellites = nav_data.num_satellites;
            gps_frame.latitude = nav_data.latitude;
            gps_frame.longitude = nav_data.longitude;
            gps_frame.horz_accuracy = nav_data.horz_accuracy;

            gps_frame.altitude = nav_data.altitude;
            gps_frame.vert_accuracy = nav_data.vert_accuracy;

            gps_frame.ground_speed = nav_data.ground_speed;
            gps_frame.speed_accuracy = nav_data.speed_accuracy;

            gps_frame.heading_of_motion = nav_data.heading_of_motion;
            gps_frame.heading_accuracy = nav_data.heading_accuracy;
            
            switch (nav_data.fixType) {
                case ubx::FIX_TYPE::NO_FIX:
                    gps_frame.fix = GPSFixType::NO_FIX;
                    break;
                case ubx::FIX_TYPE::DEAD_RECK:
                    error("DEAD_RECK");
                    gps_frame.fix = GPSFixType::UNKNOWN;
                    break;
                case ubx::FIX_TYPE::FIX_2D:
                    gps_frame.fix = GPSFixType::FIX_2D;
                    break;
                case ubx::FIX_TYPE::FIX_3D:
                    gps_frame.fix = GPSFixType::FIX_3D;
                    break;
                case ubx::FIX_TYPE::COMBINED:
                    error("COMBINED");
                    gps_frame.fix = GPSFixType::FIX_2D;
                    break;
                case ubx::FIX_TYPE::TIME_ONLY:
                    gps_frame.fix = GPSFixType::UNKNOWN;
                    error("TIME_ONLY");
                    break;
                default:
                    gps_frame.fix = GPSFixType::UNKNOWN;
                    break;
            }
            sendData(gps_frame);
        } else { // Bad Data
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            ubx::flushStream(i2c_);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
      }
      std::this_thread::sleep_for(
          std::chrono::milliseconds(getUpdateInterval())
      );
    }
    setStatus(ExtensionStatus::STOPPING);
    return 0;
}

bool extension::SAMM8Q::configure() {
    ubx::ACK ack;
    
    if (ubx::getStreamSize(i2c_) != 0) { // Restart the device if it is already running
        ubx::sendResetCommand(i2c_);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (i2c_.readByteFromReg(0xFF) != 0) {
            if (i2c_.status() != extension_interface::I2C_STATUS::OK) {
                error("UBX_RESTART");
                return false;
            }
        }
    }

    // Configure the device to be UBX only, check for ACK
    ack = ubx::setProtocolDDC(i2c_, false);
    if (ack != ubx::ACK::ACK) {
        error("CFG_PRT");
        return false;
    }

    //// Set the frequency of the messages, check for ACK (1 for every solution)
    //ack = ubx::setMessageRate(i2c_, kNavClass, kNavPvt, 1);
    //if (ack != ubx::ACK::ACK) {
    //    error("CFG_MSG");
    //    return false;
    //}

    // Set the measurement rate
    ack = ubx::setMeasurementRate(i2c_, 100);
    if (ack != ubx::ACK::ACK) {
        error("CFG_RATE");
        return false;
    }

    // Change the dynamic model (high altitude), this provides for high altitude
    // altitude data, as long as we stay below 1G of acceleration, check for ACK
    ack = ubx::setDynamicModel(i2c_, ubx::DYNAMIC_MODEL::AIRBORNE_1G);
    if (ack != ubx::ACK::ACK) {
        error("CFG_NAV5");
        return false;
    }

    /** @todo Read the configuration data and verify */ 
    return true;
}

