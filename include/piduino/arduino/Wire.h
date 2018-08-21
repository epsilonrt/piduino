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
*/

#ifndef TwoWire_h
#define TwoWire_h

#include <piduino/i2cdev.h>

#define BUFFER_LENGTH 32

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire : public Piduino::I2cDev {

  public:
    TwoWire() : Piduino::I2cDev() {}
    virtual ~TwoWire() {}
    void begin() {
      open (defaultBus());
    }
    void begin (int id) {
      open (id);
    }
    void end() {
      close();
    }
    void setClock (uint32_t dummy) {}
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
