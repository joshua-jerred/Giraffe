# SAM-M8Q GPS Module Testing

from smbus import SMBus

address = 0x42 # UBlox default address
bus = SMBus(1) # 1 indicates /dev/i2c-1

STREAM_SIZE_MSB_REG = 0XFD
STREAM_SIZE_LSB_REG = 0XFE
STREAM_REG = 0XFF

# Attempt a read
def getStreamSize():
    msb = bus.read_byte_data(address, STREAM_SIZE_MSB_REG)
    lsb = bus.read_byte_data(address, STREAM_SIZE_LSB_REG)
    return (msb << 8) + lsb

def main():
    print(getStreamSize())