#ifndef UBX_H_
#define UBX_H_

#include <cstdint>
#include <vector>
#include "extension-interface.h"

namespace ubx
{
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

    bool checkForAck(
        I2C &i2c, 
        const uint8_t msg_class, 
        const uint8_t msg_id);
    bool setProtocolDDC(I2C &i2c, bool extended_timeout);
    // bool setMessageRate
    // bool setMeasurementRate
    // bool setDynamicModel
    // UBXMessage getConfiguration
} // namespace ubx

#endif // UBX_H_