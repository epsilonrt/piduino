/*
  TwoWire.h - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2018 by Pascal JEAN for Piduino Library
*/

#ifndef TwoWire_h
#define TwoWire_h

#include <piduino/i2cdev.h>
#include <Arduino.h>

#define BUFFER_LENGTH 32

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire : public Piduino::I2cDev {

  public:
    TwoWire(int id = Info::defaultBus().id()) : Piduino::I2cDev(id) {}
    virtual ~TwoWire() {}
    void begin() {
      if (!open ()) {
        throw std::system_error (errno, std::system_category(),
                                 "Error when opening the I2C bus " +
                                 bus().path());
      }
    }
    void begin (int id) {
      setBus (id);
      if (!open ()) {
        throw std::system_error (errno, std::system_category(),
                                 "Error when opening the I2C bus " +
                                 bus().path());
      }
    }
    void end() {
      close();
    }
    void setClock (uint32_t dummy) {}
    inline byte endTransmission (bool stop = true) {
      return I2cDev::endTransmission (stop) ? 0 : 4;
    }
    int requestFrom (uint16_t address, uint16_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop);
    inline int requestFrom (uint16_t slave, uint16_t max, bool stop = true) {
      return Piduino::I2cDev::requestFrom (slave, max, stop);
    }
    inline int requestFrom (int slave, int max, int stop = 1) {
      return Piduino::I2cDev::requestFrom (slave, max, stop);
    }
};

extern TwoWire Wire;
#endif
