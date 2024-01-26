#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define MAX7219_REG_NOOP 0x00
#define MAX7219_REG_DIGIT0 0x01
#define MAX7219_REG_DIGIT1 0x02
#define MAX7219_REG_DIGIT2 0x03
#define MAX7219_REG_DIGIT3 0x04
#define MAX7219_REG_DIGIT4 0x05
#define MAX7219_REG_DIGIT5 0x06
#define MAX7219_REG_DIGIT6 0x07
#define MAX7219_REG_DIGIT7 0x08
#define MAX7219_REG_DECODEMODE 0x09
#define MAX7219_REG_INTENSITY 0x0A
#define MAX7219_REG_SCANLIMIT 0x0B
#define MAX7219_REG_SHUTDOWN 0x0C
#define MAX7219_REG_DISPLAYTEST 0x0F

#define CMD_WRITE 0x40
#define CMD_READ 0b10000000

typedef struct {

  int digits[8];

} max7219;

max7219 header;

int spi_fd;
struct sigaction act;
const std::string spiDevice = "/dev/spidev0.1";
static uint8_t spiBPW = 8;
static uint32_t spiSpeed = 100000;
static uint16_t spiDelay = 0;

// void writeBytes(max7219 *header, int cmd, int data) {
//   if (!header)
//     return;
//   writeByte(cmd, data);
// }

int spiOpen(std::string dev) {
  if ((spi_fd = open(dev.c_str(), O_RDWR)) < 0) {
    std::cout << "Error opening " << dev << std::endl;
    return -1;
  }
  std::cout << "Successfully opened " << dev << std::endl;
  return 0;
}

void receive(uint8_t reg) {
  std::cout << "Receiving..." << std::endl;
  uint8_t spiBufTx[2];
  uint8_t spiBufRx[3];
  struct spi_ioc_transfer spi;
  memset(&spi, 0, sizeof(spi));
  spiBufTx[0] = CMD_READ;
  spiBufTx[1] = reg;
  spi.tx_buf = (unsigned long)spiBufTx;
  spi.rx_buf = (unsigned long)spiBufRx;
  spi.len = 3;
  spi.delay_usecs = spiDelay;
  spi.speed_hz = spiSpeed;
  spi.bits_per_word = spiBPW;
  ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);

  for (uint8_t rec_byte : spiBufRx) {
    std::cout << std::hex << (int)rec_byte << " ";
  }
}

int main() {
  spiOpen(spiDevice);

  while (1) {
    receive(0x00);
    sleep(1);
  }

  return 0;
}
