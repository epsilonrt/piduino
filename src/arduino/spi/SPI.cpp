/* Copyright © 2018 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/
#include <SPI.h>
#include <system_error>

// -----------------------------------------------------------------------------
// Initialize the SPI library
void SPIClass::begin() {

  if (isOpen() == false) {
    const Info &busInfo = _defaultBus.exists() ? _defaultBus : Info::defaultBus();

    setBus (busInfo);
    setSettings (SPISettings());

    if (!open ()) {
      throw std::system_error (errno, std::system_category(),
                               "Error when opening the SPI bus " +
                               bus().path());
    }
  }
}

// -----------------------------------------------------------------------------
void SPIClass::begin (int idBus, int idCs) {

  if (isOpen() == false) {
    setBus (idBus, idCs);
    setSettings (SPISettings());
    if (!open ()) {
      throw std::system_error (errno, std::system_category(),
                               "Error when opening the SPI bus " +
                               bus().path());
    }
  }
}

//------------------------------------------------------------------------------
// Piduino Only: Set the SPI bus to use
void SPIClass::setDefaultBus (int bus, int cs) {
  Info info (bus, cs);

  if (isOpen() == false) {

    if (info.exists()) {

      _defaultBus = info;
    }
    else {

      throw std::system_error (ENOENT, std::system_category(),
                               info.path() + " not found, please check system configuration !");
    }
  }
  else {

    throw std::system_error (EBUSY, std::system_category(),
                             "SPI bus " + info.path() + " is already opened !");
  }
}

//------------------------------------------------------------------------------
// Piduino Only: Set the default bus path
void SPIClass::setDefaultBusPath (const String &path) {
  Info info;
  if (info.setPath (path)) {
    if (isOpen() == false) {
      _defaultBus = info;
    }
    else {
      throw std::system_error (EBUSY, std::system_category(),
                               "SPI bus " + path + " is already opened !");
    }
  }
  else {
    throw std::system_error (ENOENT, std::system_category(),
                             path + " not found, please check system configuration !");
  }
}


// -----------------------------------------------------------------------------
// Disable the SPI bus
void SPIClass::end() {
  close();
}

// -----------------------------------------------------------------------------
// Before using SPI.transfer() or asserting chip select pins,
// this function is used to gain exclusive access to the SPI bus
// and configure the correct settings.
void SPIClass::beginTransaction (const SPISettings &s) {

  setSettings (s);
  _pendingTransaction.lock();
}

// -----------------------------------------------------------------------------
// After performing a group of transfers and releasing the chip select
// signal, this function allows others to access the SPI bus
void SPIClass::endTransaction (void) {

  _pendingTransaction.unlock();
}

// -----------------------------------------------------------------------------
// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
uint8_t SPIClass::transfer (uint8_t data) {
  uint8_t rxdata = 0;

  Piduino::SpiDev::transfer (&data, &rxdata, 1);
  return rxdata;
}

// -----------------------------------------------------------------------------
uint16_t SPIClass::transfer16 (uint16_t data) {
  uint8_t inbuf[2];
  uint8_t outbuf[2];
  uint8_t msb = (bitOrder() == MSBFIRST) ? 0 : 1;
  uint8_t lsb = msb ? 0 : 1;

  inbuf[msb] = data >> 8;
  inbuf[lsb] = data & 0xFF;
  Piduino::SpiDev::transfer (inbuf, outbuf, 2);
  data = outbuf[msb] << 8 | outbuf[lsb];
  return data;
}

// -----------------------------------------------------------------------------
void SPIClass::transfer (void *buf, size_t count) {
  Piduino::SpiDev::transfer (static_cast<const uint8_t *> (buf), static_cast<uint8_t *> (buf), count);
}

// -----------------------------------------------------------------------------
// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPIClass::setBitOrder (uint8_t b) {

  Piduino::SpiDev::setBitOrder (b != 0);
}

// -----------------------------------------------------------------------------
// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPIClass::setDataMode (uint8_t dataMode) {

  setMode (dataMode | SPI_NO_CS);
}

// -----------------------------------------------------------------------------
// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPIClass::setClockDivider (uint8_t clockDiv) {
  uint32_t baud =  static_cast<uint32_t> (static_cast<double> (16e6) /
                                          static_cast<double> (clockDiv));
  setSpeedHz (baud);
}

SPIClass SPI;

/* ========================================================================== */


