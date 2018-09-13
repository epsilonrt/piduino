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
#include "config.h"

#if PIDUINO_WITH_SPI
#include <SPI.h>
#endif

namespace Piduino {

  // ---------------------------------------------------------------------------
  bool pinLocked (int n) {

#if PIDUINO_WITH_SPI
    if (db.board().soc().family().id() != SoC::Family::BroadcomBcm2835) {
      SpiDev * s = reinterpret_cast<SpiDev *> (&::SPI);
      const SpiDev::Cs * cs;

      if (s->isOpen()) {

        cs = & s->bus().cs();
      }
      else {

        cs = & SpiDev::defaultBus().cs();
      }
      if (gpio.open()) {

        if ( (cs->pin()->logicalNumber() == n) && (cs->pin()->mode() == cs->mode())) {

          return true;
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

  if (!pinLocked (n)) {
    Pin::Pull p = Pin::PullOff;
    Pin::Mode m = Pin::ModeOutput;

    if (mode != OUTPUT) {

      m = Pin::ModeInput;
      if (mode == INPUT_PULLUP) {
        p = Pin::PullUp;
      }
      else if (mode == INPUT_PULLDOWN) {
        p = Pin::PullDown;
      }
    }

    if (gpio.open()) {
      gpio.pin (n).setMode (m);
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

  if (!pinLocked (n)) {
    unsigned int r = gpio.pin (n).dac().resolution();

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
void attachInterrupt (int n, Pin::Isr isr, ArduinoIntEdge mode) {

  gpio.pin (n).attachInterrupt (isr, static_cast<Pin::Edge> (mode));
}

// -----------------------------------------------------------------------------
void detachInterrupt (int n) {

  gpio.pin (n).detachInterrupt();
}
/* ========================================================================== */
