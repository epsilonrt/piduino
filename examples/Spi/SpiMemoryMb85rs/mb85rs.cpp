#include "mb85rs.h"

/* constants ================================================================ */
//  Op-code
#define WREN  0x06 // Set Write Enable Latch
#define WRDI  0x04 // Reset Write Enable Latch
#define RDSR  0x05 // Read Status Register
#define WRSR  0x01 // Write Status Register
#define READ  0x03 // Read Memory Code
#define WRITE 0x02 // Write Memory Code
#define RDID  0x9F // Read Device ID
#define FSTRD 0x0B // Fast Read Memory Code


// -----------------------------------------------------------------------------
Mb85rs::Mb85rs (int csPin, size_t size) : _cspin (csPin), _size (size),
  _settings (SPISettings (10000000, MSBFIRST, SPI_MODE0)) {
}

// -----------------------------------------------------------------------------
void Mb85rs::begin (uint32_t fclk) {

  _settings.speedHz = fclk;
  // initialize the chip select pin
  pinMode (_cspin, OUTPUT);
  digitalWrite (_cspin, HIGH);
}

// -----------------------------------------------------------------------------
void Mb85rs::transfer (uint8_t * buf, size_t len) {
  SPI.beginTransaction (_settings);
  digitalWrite (_cspin, LOW);
  SPI.transfer (buf, len);
  digitalWrite (_cspin, HIGH);
  SPI.endTransaction();
}

// -----------------------------------------------------------------------------
void Mb85rs::transfer (uint8_t  b) {
  SPI.beginTransaction (_settings);
  digitalWrite (_cspin, LOW);
  SPI.transfer (b);
  digitalWrite (_cspin, HIGH);
  SPI.endTransaction();
}

// -----------------------------------------------------------------------------
void Mb85rs::write (size_t addr, uint8_t * buf, size_t len) {
  uint8_t * frame = new uint8_t[len + 3];

  frame[0] = WRITE;
  frame[1] = addr << 8;   // MSB address
  frame[2] = addr & 0xFF; // LSB address
  memcpy (&frame[3], buf, len);

  setWriteEnable (true);
  transfer (frame, len + 3);
  delete[] frame;
}

// -----------------------------------------------------------------------------
void Mb85rs::read (size_t addr, uint8_t * buf, size_t len) {
  uint8_t * frame = new uint8_t[len + 3];

  frame[0] = READ;
  frame[1] = addr << 8;   // MSB address
  frame[2] = addr & 0xFF; // LSB address

  transfer (frame, len + 3);
  memcpy (buf, &frame[3], len);
  delete[] frame;
}

// -----------------------------------------------------------------------------
// Lecture Identifiant FRAM, normalement
// Manufacturer ID      : 0x04
// Continuation code    : 0x7F
// Product ID (1st Byte): 0x05 (pour 256kbits)
// Product ID (2nd Byte): 0x09
Mb85rs::DeviceId Mb85rs::deviceId() {
  Mb85rs::DeviceId id;
  uint8_t buf[5] = { RDID };

  transfer (buf, 5);
  id.manufacturerId = buf[1];
  id.continuationCode = buf[2];
  id.productId = (buf[3] <<  8) + buf[4];
  return id;
}

// -----------------------------------------------------------------------------
uint8_t Mb85rs::status()  {
  uint8_t buf[2] = { RDSR };

  transfer (buf, 2);
  return buf[1];
}

// -----------------------------------------------------------------------------
void Mb85rs::setStatus (uint8_t status) {
  uint8_t buf[2] = { WRSR, status };

  transfer (buf, 2);
}

// -----------------------------------------------------------------------------
Mb85rs::BlockProtect Mb85rs::blockProtected() {
  uint8_t b = (status() & ( (1 << BP1) | (1 << BP0))) >> BP0;
  return static_cast<BlockProtect> (b);
}

// -----------------------------------------------------------------------------
void Mb85rs::protectBlock (BlockProtect bp) {
  uint8_t b = (status() & ~ ( (1 << BP1) | (1 << BP0)));
  b |= static_cast<uint8_t> (bp) << BP0;
  setStatus (b);
}

// -----------------------------------------------------------------------------
void Mb85rs::setWriteEnable (bool enable) {

  transfer (enable ? WREN : WRDI);
}

// -----------------------------------------------------------------------------
bool Mb85rs::writeEnabled() {

  return (status() & WEL) != 0;
}

// -----------------------------------------------------------------------------
void Mb85rs::printBlock (Print & console, size_t address, const uint8_t * data, size_t len) {
  const size_t bytes = 16;

  for (size_t i = 0; i < (len < bytes ? 1 : len / bytes); i++) {

    printHex (console, i * bytes + address, 8); // affiche l'adresse de début de ligne
    console.write (' ');
    for (size_t j = 0; j < bytes && len > 0; j++) {

      printHex (console, *data++, 2); // affiche bytes octets par ligne
      --len;
    }
    console.write ('\n');
  }
}

// -----------------------------------------------------------------------------
void Mb85rs::printHex (Print & console, uint32_t n, int width) {
  char str[9];
  const char * list[5] = {"%lX", "%02lX", "%04lX", "%06lX", "%08lX"};
  const char * format = list[0];
  width = width > 8 ? 8 : width;
  if (width > 0) {
    format = list[width >> 1];
  }
  sprintf (str, format, n); // affiche l'adresse de début de ligne
  console.print (str);
}
