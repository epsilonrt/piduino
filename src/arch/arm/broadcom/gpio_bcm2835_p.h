/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#pragma once

#include <piduino/iomap.h>
#include "bcm2835.h"
#include "gpio_bcm2835.h"
#include "../../../gpio/gpiodevice_p.h"

namespace Piduino {

  class Bcm2835Gpio::Private  : public GpioDevice::Private {

    public:
      Private (Bcm2835Gpio *q);
      virtual ~Private();

      off_t piobase;
      IoMap  iomap;

      inline uint32_t readReg (size_t offset) const {
        return *iomap.io (offset);
      }

      inline void writeReg (size_t offset, uint32_t value) {
        *iomap.io (offset) = value;
      }

      static const std::map<unsigned int, Pin::Mode> int2mode;
      static const std::map<Pin::Mode, unsigned int> mode2int;
      static const std::map<Pin::Mode, std::string> modes;
      static bool is2711;

      PIMP_DECLARE_PUBLIC (Bcm2835Gpio)
  };
}