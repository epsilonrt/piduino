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

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <piduino/arduino.h>

using namespace Piduino;
using namespace std;

namespace Arduino {

  Clock * clk;
  Gpio *  gpio;

// -----------------------------------------------------------------------------
  void closeall () {

    if (gpio) {

      gpio->close();
      delete gpio;
      gpio = 0;
    }

    if (clk) {

      delete clk;
      clk = 0;
    }
  }

// -----------------------------------------------------------------------------
// Signal Handler
  void
  sighandler (int sig) {

    closeall();
#ifndef NDEBUG
    cout << endl << "everything was closed." << endl << "Have a nice day !" << endl;
#endif
    exit (EXIT_SUCCESS);
  }

// -----------------------------------------------------------------------------
  void __attribute__ ( (constructor (65535))) begin () {

    gpio = new Gpio;
    clk = new Clock;
    gpio->open();
    // sighandler() intercepts CTRL+C
    signal (SIGINT, sighandler);
    signal (SIGTERM, sighandler);
  }

// -----------------------------------------------------------------------------
  void __attribute__ ( (destructor (65535))) end () {

    closeall();
  }
}

// -----------------------------------------------------------------------------
void pinMode (int n, ArduinoPinMode mode) {
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

  Arduino::gpio->pin (n).setMode (m);
  Arduino::gpio->pin (n).setPull (p);
}

// -----------------------------------------------------------------------------
void digitalWrite (int n, int value) {

  Arduino::gpio->pin (n).write (value);
}

// -----------------------------------------------------------------------------
void digitalToggle (int n) {

  Arduino::gpio->pin (n).toggle ();
}

// -----------------------------------------------------------------------------
int digitalRead (int n) {

  return Arduino::gpio->pin (n).read();
}

// -----------------------------------------------------------------------------
void attachInterrupt (int n, Pin::Isr isr, ArduinoIntEdge mode) {

  Arduino::gpio->pin (n).attachInterrupt (isr, static_cast<Pin::Edge> (mode));
}

// -----------------------------------------------------------------------------
void detachInterrupt (int n) {

  Arduino::gpio->pin (n).detachInterrupt();
}

// -----------------------------------------------------------------------------
void delay (unsigned long ms) {

  Arduino::clk->delay (ms);
}

// -----------------------------------------------------------------------------
void delayMicroseconds (unsigned long us) {

  Arduino::clk->delayMicroseconds (us);
}

// -----------------------------------------------------------------------------
unsigned long millis() {

  return Arduino::clk->millis();
}

// -----------------------------------------------------------------------------
unsigned long micros() {

  return Arduino::clk->micros();
}
/* ========================================================================== */
