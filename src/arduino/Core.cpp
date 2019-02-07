/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * The Piduino Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The Piduino Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include <piduino/clock.h>
#include <piduino/scheduler.h>
#include <piduino/database.h>
#include <piduino/gpiopwm.h>
#include "config.h"

#if PIDUINO_WITH_SPI
#include <SPI.h>
#endif

namespace Piduino {

  // ---------------------------------------------------------------------------
  bool pinLocked (int n) {

#if PIDUINO_WITH_SPI
    if (db.board().soc().family().id() != SoC::Family::BroadcomBcm2835) {

      if (gpio.open()) {
        static SpiDev::Info defaultSpi = SpiDev::Info::defaultBus();
        static const SpiDev * spi = reinterpret_cast<const SpiDev *> (&::SPI);

        if (spi->isOpen() || defaultSpi.exists()) {
          const SpiDev::Cs * cs = spi->isOpen() ?
                                  & spi->bus().cs() :
                                  & defaultSpi.cs();

          return ( (cs->mode() != Pin::ModeUnknown) &&
                   (cs->pin()->logicalNumber() == n) &&
                   (cs->pin()->mode() == cs->mode()));
        }
      }
    }
#endif

    return false;
  }
}

using namespace Piduino;

// -----------------------------------------------------------------------------
void delay (unsigned long ms) {

  clk.delay (ms);
}

// -----------------------------------------------------------------------------
void delayMicroseconds (unsigned long us) {

  clk.delayMicroseconds (us);
}

// -----------------------------------------------------------------------------
unsigned long millis() {

  return clk.millis();
}

// -----------------------------------------------------------------------------
unsigned long micros() {

  return clk.micros();
}

// -----------------------------------------------------------------------------
void setPriority (int priority) {

  Scheduler::setRtPriority (priority);
}

// -----------------------------------------------------------------------------
void pinMode (int n, ArduinoPinMode mode) {

  if (!pinLocked (n) &&     gpio.open()) {
    Pin::Pull p = Pin::PullUnknown;
    Pin::Mode m = Pin::ModeUnknown;

    if (mode == OUTPUT)  {
      
      p = Pin::PullOff;
      m = Pin::ModeOutput;
    }
    else if ( (mode == INPUT) || (mode == INPUT_PULLDOWN) || (mode == INPUT_PULLUP)) {
      
      m = Pin::ModeInput;
    }

    if ( (mode == INPUT_PULLDOWN) || (mode == PULLDOWN)) {
      
      p = Pin::PullDown;
    }
    else if ( (mode == INPUT_PULLUP) || (mode == PULLUP)) {
      
      p = Pin::PullUp;
    }

    if (m != Pin::ModeUnknown) {
      
      gpio.pin (n).setMode (m);
    }
    
    if (p != Pin::PullUnknown) {
      
      gpio.pin (n).setPull (p);
    }
  }
}

// -----------------------------------------------------------------------------
void digitalWrite (int n, int value) {

  gpio.pin (n).write (value);
}

// -----------------------------------------------------------------------------
void digitalToggle (int n) {

  gpio.pin (n).toggle ();
}

// -----------------------------------------------------------------------------
int digitalRead (int n) {

  return gpio.pin (n).read();
}

// -----------------------------------------------------------------------------
void analogWrite (int n, int v) {

  if (gpio.pin (n).dac() == nullptr) {

    if (! gpio.pin (n).setDac (new GpioPwm (&gpio.pin (n), 8))) {
      return;
    }
  }

  if (!pinLocked (n)) {
    unsigned int r = gpio.pin (n).dac()->resolution();

    if (r > 8) {
      v <<= r - 8;
    }
    if (r < 8) {
      v >>= 8 - r;
    }
    gpio.pin (n).analogWrite (v);
  }
}

// -----------------------------------------------------------------------------
void attachInterrupt (int n, Isr isr, ArduinoIntEdge mode) {

  gpio.pin (n).attachInterrupt (reinterpret_cast<Pin::Isr> (isr), static_cast<Pin::Edge> (mode));
}

// -----------------------------------------------------------------------------
void detachInterrupt (int n) {

  gpio.pin (n).detachInterrupt();
}
/* ========================================================================== */
