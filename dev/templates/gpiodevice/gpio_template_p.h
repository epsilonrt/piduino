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
#include <piduino/iomap.h> // only if /dev/mem will be necessary
#include "gpio_template.h"
#include "../../gpio/gpiodevice_p.h"

namespace Piduino {

  // This is the private class of the GpioTemplate class
  // It contains the private data and methods of the GpioTemplate class
  // It is used to separate the implementation of the GpioTemplate class
  // from its interface
  class GpioTemplate::Private  : public GpioDevice::Private {

    public:
      Private (GpioTemplate *q);
      virtual ~Private();

      // place here the code to declare the private data

      // place here the code to declare the private methods

      // place here the code to declare the private static methods

      //---------------------------------------------------------------
      // This following methods are used to wait for an interrupt
      // may be redefined, in this case set the flag hasWfi

      // -----------------------------------------------------------------------------
      inline int irqLine (const Pin *pin) const {

        // replace this with the code to get the line
        return pin->systemNumber();
      }

      // -----------------------------------------------------------------------------
      inline bool setPinEdge (const Pin *pin, Pin::Edge edge) {
        // this function should set the edge for the pin
        // if the device supports it
        // this function should also set the hasWfi or hasIrq flag

        // place here the code to set the edge of the pin
        return false;
      }

      // -----------------------------------------------------------------------------
      inline int waitForInterrupt (const Pin *pin, int timeout_ms) {
        // timeout_ms = -1 for infinite wait
        // this function should block until the interrupt occurs
        // or the timeout occurs
        // this function should also set the hasWfi flag

        // wait for an event
        // place here the code to wait for an interrupt on the pin
        return -1;
      }
      
      //---------------------------------------------------------------
      // The following methods are used to manage the ISR (Interrupt Service Routine)
      // must defined only if the device has this feature hasIrq

      // may be redefined, in this case set the flag hasIrq
      bool attachInterrupt (const Pin *pin, Pin::Isr isr, void *userData);
      void detachInterrupt (const Pin *pin);

      std::thread &isrThreadOfPin (const Pin *pin);
      std::promise<void> &killThreadOfPin (const Pin *pin) {
        return killIsrThread[irqLine (pin)];
      }

      std::map<int, std::promise<void>> killIsrThread;
      std::map<int, std::thread> isrThread;

      // defined only if the device has this feature hasIrq
      // replace this with the code to get the line
      static void *isrFunc (std::future<void> run, const Pin *pin, GpioTemplate::Private *dev, Pin::Isr userIsr, void *userData);
      // ----------------------------------------------------------------

      // do not remove the following lines
      static const std::map<Pin::Mode, std::string> modes;

      // it is used by the PIMP_DECLARE_PRIVATE macro
      PIMP_DECLARE_PUBLIC (GpioTemplate)
  };
}