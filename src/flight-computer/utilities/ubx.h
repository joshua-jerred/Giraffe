#ifndef UBX_H_
#define UBX_H_

#include <cstdint>
#include <vector>
#include "extension-interface.h"

namespace ubx
{
    enum class ACK {
        ACK,
        NACK,
        NONE,
        WRITE_ERROR,
        READ_ERROR
    };

    enum class DYNAMIC_MODEL {
        PORTABLE = 0x00,
        STATIONARY = 0x02,
        PEDESTRIAN = 0x03,
        AUTOMOTIVE = 0x04,
        SEA = 0x05,
        AIRBORNE_1G = 0x06,
        AIRBORNE_2G = 0x07,
        AIRBORNE_4G = 0x08,
        ERROR = 0xFF
    };

    enum class FIX_TYPE {
        NO_FIX = 0x00,
        DEAD_RECK = 0x01,
        FIX_2D = 0x02,
        FIX_3D = 0x03,
        COMBINED = 0x04,
        TIME_ONLY = 0x05,
        ERROR = 0xFF
    };

    struct NAV_DATA {
        bool valid = false;
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int minute = 0;
        int second = 0;
        
        FIX_TYPE fixType = FIX_TYPE::NO_FIX;

        int num_satellites = 0;
        double longitude = 0.0;  // degrees
        double latitude = 0.0;   // degrees
        double horz_accuracy = 0.0; // meters

        double altitude = 0.0;  // meters
        double vert_accuracy = 0.0; // meters

        double ground_speed = 0.0; // meters/second
        double speed_accuracy = 0.0; // meters/second

        double heading_of_motion = 0.0; // degrees
        double heading_accuracy = 0.0;  // degrees
    };

    typedef struct UBXMessage 
    {
        UBXMessage() {};
        UBXMessage(std::uint8_t class_ID, 
            std::uint8_t msg_ID, 
            std::uint16_t length, 
            std::uint8_t *payload);
        bool calculateChecksum();
        bool verifyChecksum();
        std::uint8_t sync1 = 0;
        std::uint8_t sync2 = 0;
        std::uint8_t mClass = 0;
        std::uint8_t mID = 0;
        std::uint16_t length = 0;
        std::uint8_t *payload = nullptr;
        std::uint8_t ck_a = 0;
        std::uint8_t ck_b = 0;
    } UBXMessage;

    int getStreamSize(I2C &i2c);
    bool writeUBX(I2C &i2c, const UBXMessage &message);
    bool readNextUBX(I2C &i2c, UBXMessage &message);
    bool readSpecificMessage(
        I2C &i2c, 
        ubx::UBXMessage &message); // message should have class, id, and length set
    ACK checkForAck(
        I2C &i2c, 
        const uint8_t msg_class, 
        const uint8_t msg_id);

    bool sendResetCommand(I2C &i2c);

    ACK setProtocolDDC(I2C &i2c, const bool extended_timeout);
    ACK setMessageRate(
        I2C &i2c, 
        uint8_t msg_class, 
        uint8_t msg_id, 
        uint8_t rate);
    ACK setMeasurementRate(I2C &i2c, const uint16_t rate_ms);
    ACK setDynamicModel(
        I2C &i2c,
        const DYNAMIC_MODEL model
    );
    // UBXMessage getConfiguration

    bool parsePVT(
        const UBXMessage &message, 
        NAV_DATA &data);
} // namespace ubx

#endif // UBX_H_