import math
import binascii

HDLC = "0x7E 0x69 0x69 0x76 0x51 0x75 0x29 0x07 0x69 0x11 0x4E 0x71 0x11 0x61 0xEE 0xFA 0x07 0x95 0x53 0x67 0x17 0x3A 0x72 0xB9 0xBF 0x00"

def detectFlag(byte: str):
  if (byte == "0x7E"):
    print("FLAG")
  else:
    print("---"+byte+"---: " + str(type(byte)))
    raise Exception("Invalid HDLC frame flag")

def printAddress(address: list):
  for byte in address[:-1]:
    res = "{0:08b}".format(int(byte, 16)) # Convert to binary
    reverse = res[::-1] # Reverse the bits
    binary_int = int(reverse, 2) >> 1 # Shift right 1 bit
    print(chr(binary_int) + " " + byte)
  
  # SSID
  res = "{0:08b}".format(int(address[-1], 16)) # Convert to binary
  reverse = res[::-1] # Reverse the bits
  ssid = reverse[3:-1]
  last = reverse[-1]
  print("-" + str(int(ssid, 2)))
  if (last == "1"):
    print("Last Address")

def controlFields(bytes: list):
  if (len(bytes) != 2):
    raise Exception("Invalid control field")
  
  # Control should be 0x3F
  expected = "0x3F"
  res = "{0:08b}".format(int(bytes[0], 16)) # Convert to binary
  reverse = res[::-1] # Reverse the bits
  control = "{0:08b}".format(int(expected, 16))
  print("Control:  " + reverse)
  print ("Expected: " + control)
  
  # PID Should be 0xF0
  expected = "0xF0"
  res = "{0:08b}".format(int(bytes[1], 16)) # Convert to binary
  reverse = res[::-1] # Reverse the bits
  pid = "{0:08b}".format(int(expected, 16))
  print("PID:      " + reverse)
  print ("Expected: " + pid)

def informationField(bytes: list):
  for byte in bytes:
    res = "{0:08b}".format(int(byte, 16)) # Convert to binary
    reverse = res[::-1] # Reverse the bits
    binary_int = int(reverse, 2) >> 1 # Shift right 1 bit
    print(chr(binary_int) + " " + byte)

def calculateCRC(bytes: list):
  crc = 0xFFFF
  msb = crc >> 8
  lsb = crc & 255
  for c in data:
      x = ord(c) ^ msb
      x ^= (x >> 4)
      msb = (lsb ^ (x >> 3) ^ (x << 4)) & 255
      lsb = (x ^ (x << 5)) & 255
  return (msb << 8) + lsb

if __name__ == "__main__":
  hdlc = HDLC.split(" ")
  detectFlag(hdlc[0])
  printAddress(hdlc[1:8])
  printAddress(hdlc[8:15])
  controlFields(hdlc[15:17])
  informationField(hdlc[17:-4])