/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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
  Modified 2018 by Pascal JEAN (epsilonrt@gmail.com) for piduino library
*/
#include <Wire.h>

// Public Methods //////////////////////////////////////////////////////////////
int TwoWire::requestFrom (uint16_t address, uint16_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) {
  
  if (isize > 0) {
    // send internal address; this mode allows sending a repeated start to access
    // some devices' internal registers. This function is executed by the hardware
    // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)

    beginTransmission (address);

    // the maximum size of internal address is 3 bytes
    if (isize > 3) {
      isize = 3;
    }

    // write internal register address - most significant byte first
    while (isize-- > 0) {
      write ( (uint8_t) (iaddress >> (isize * 8)));
    }
    endTransmission (false);
  }

  // clamp to buffer length
  if (quantity > BUFFER_LENGTH) {
    quantity = BUFFER_LENGTH;
  }
  
  // perform blocking read into buffer
  return I2cDev::requestFrom (address, quantity, sendStop != 0);
}

TwoWire Wire;
