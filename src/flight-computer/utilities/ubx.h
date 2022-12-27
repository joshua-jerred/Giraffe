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

    typedef struct UBXMessage 
    {
        UBXMessage() {};
        UBXMessage(std::uint8_t class_ID, 
            std::uint8_t msg_ID, 
            std::uint16_t length, 
            std::uint8_t *payload);
        bool calculateChecksum();
        std::uint8_t sync1 = 0;
        std::uint8_t sync2 = 0;
        std::uint8_t classID = 0;
        std::uint8_t msgID = 0;
        std::uint16_t length = 0;
        std::uint8_t *payload = nullptr;
        std::uint8_t ck_a = 0;
        std::uint8_t ck_b = 0;
    } UBXMessage;

    int getStreamSize(I2C &i2c);
    bool writeUBX(I2C &i2c, const UBXMessage &message);
    uint8_t* readUBX(I2C &i2c, const int size);
    UBXMessage readMessage(
        I2C &i2c, 
        const uint8_t msg_class, 
        const uint8_t msg_id);
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
} // namespace ubx

#endif // UBX_H_