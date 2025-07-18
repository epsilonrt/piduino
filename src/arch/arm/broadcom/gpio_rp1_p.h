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

#include <vector>
#include <string>
#include <piduino/iomap.h>
#include "gpio_rp1.h"
#include "../../../gpio/gpiodevice_p.h"

namespace Piduino {

  // This is the private class of the Rp1Gpio class
  // It contains the private data and methods of the Rp1Gpio class
  // It is used to separate the implementation of the Rp1Gpio class
  // from its interface
  class Rp1Gpio::Private  : public GpioDevice::Private {

    public:
      Private (Rp1Gpio *q);
      virtual ~Private();

      // the private data
      IoMap  iomap;
      volatile uint32_t *gpio;  ///< Pointer to the GPIO registers.
      volatile uint32_t *pad;  ///< Pointer to the pad registers.
      volatile uint32_t *rio; ///< Pointer to the RIO registers.
      unsigned int flags; ///< Flags for the GPIO device.

      // the private methods
      std::string findPCIeDevice();

      inline void setCtrlReg (int p, uint32_t v) {
        gpio[p * 2 + 1] = v;
      }
      inline uint32_t ctrlReg (int p) const {
        return gpio[p * 2 + 1];
      }
      inline uint32_t statusReg (int p) const {
        return gpio[p * 2];
      }
      inline void setPadReg (int p, uint32_t v) {
        pad[p + 1] = v; // 1 offset due to the voltage select register
      }
      inline uint32_t padReg (int p) const {
        return pad[p + 1]; // 1 offset due to the voltage select register
      }

      // the private static methods
      static bool isPCIeFileContain (const char *dirname, const char *filename, const char *content);

      // the private static data
      static const std::map<Pin::Mode, std::string> modes;
      static const std::map<uint32_t, Pin::Mode> fsel2mode;
      static const std::map<Pin::Mode, uint32_t> mode2fsel;

      // it is used by the PIMP_DECLARE_PRIVATE macro
      PIMP_DECLARE_PUBLIC (Rp1Gpio)
  };
}