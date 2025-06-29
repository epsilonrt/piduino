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
#include <thread>
#include <future>
#include <piduino/gpio2.h>
#include "gpio_dev2.h"
#include "../iodevice_p.h"

namespace Piduino {

  class GpioDev2::Private  : public IoDevice::Private {

    public:
      Pin *pin; // pointer to the pin associated with this device
      Gpio2::Chip *chip;
      Gpio2::Line *line;
      uint32_t debounce;
      std::promise<void> killIsrThread;
      std::thread isrThread;
      static std::map<int, Gpio2::Chip *> chips; // map to hold chip instances, key is the chip number

      Private (GpioDev2 *q, Pin *pin);
      virtual ~Private();

      // place here the code to declare the private methods
      bool open (OpenMode mode);
      void close();

      Gpio2::LineConfig pinConfig() const;
      bool attachInterrupt (Pin::Isr isr, void *userData);
      void detachInterrupt ();
      static void *isrFunc (std::future<void> run, GpioDev2::Private *dev, Pin::Isr userIsr, void *userData);

      // -----------------------------------------------------------------------------
      inline bool setPinEdge (Pin::Edge edge) {
        Gpio2::LineConfig config = pinConfig();

        switch (edge) {
          case Pin::EdgeRising:
            config.flags |= GPIO_V2_LINE_FLAG_EDGE_RISING;
            break;
          case Pin::EdgeFalling:
            config.flags |= GPIO_V2_LINE_FLAG_EDGE_FALLING;
            break;
          case Pin::EdgeBoth:
            config.flags |= (GPIO_V2_LINE_FLAG_EDGE_FALLING | GPIO_V2_LINE_FLAG_EDGE_RISING);
            break;
          default:
            return false;
            break;
        }

        if (line->setConfig (config)) {

          clearError();
          return true;
        }
        setError();
        return false;
      }

      // -----------------------------------------------------------------------------
      inline bool setDebounce () {

        return line->setDebounce (debounce);
      }

      // -----------------------------------------------------------------------------
      inline bool waitForInterrupt (Pin::Event & event, int timeout_ms) {

        if (line->waitForEvent (event, timeout_ms)) {

          clearError();
          return true;
        }
        setError (line->errorCode(), line->errorMessage());
        return false; 
      }

      // it is used by the PIMP_DECLARE_PRIVATE macro
      PIMP_DECLARE_PUBLIC (GpioDev2)
  };
}