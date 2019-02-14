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
#include <piduino/system.h>
#include <piduino/database.h>
#include <piduino/clock.h>
#include <piduino/board.h>
#include <piduino/gpio.h>
#include <piduino/syslog.h>

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <memory>

#if PIDUINO_WITH_I2C
#if PIDUINO_WITH_ARDUINO
#include <Wire.h>
#else
#include <piduino/i2cdev.h>
#endif
#endif

#if PIDUINO_WITH_SPI
#if PIDUINO_WITH_ARDUINO
#include <SPI.h>
#else
#include <piduino/spidev.h>
#endif
#endif
#include "config.h"

using namespace std;

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                           Global Objects
//
// -----------------------------------------------------------------------------
  System system;
  Database db;
  Clock clk;
  SysLog Syslog;
#if PIDUINO_WITH_GPIO
  Gpio gpio;
#if PIDUINO_WITH_I2C && ! PIDUINO_WITH_ARDUINO
  I2cDev Wire;
#endif // PIDUINO_WITH_I2C && ! PIDUINO_WITH_ARDUINO
#if PIDUINO_WITH_SPI && ! PIDUINO_WITH_ARDUINO
  SpiDev SPI;
#endif // PIDUINO_WITH_SPI && ! PIDUINO_WITH_ARDUINO
#endif // PIDUINO_WITH_GPIO

// -----------------------------------------------------------------------------
  void closeall () {

#if PIDUINO_WITH_GPIO
    gpio.close();
#endif
  }

// -----------------------------------------------------------------------------
// Signal Handler
  void
  sighandler (int sig) {

    closeall();
#ifndef NDEBUG
    cout << "everything was closed." << endl << "Have a nice day !" << endl;
#endif
    exit (EXIT_SUCCESS);
  }


// -----------------------------------------------------------------------------
  void __attribute__ ( (constructor (65535))) begin () {

    // sighandler() intercepts CTRL+C
    signal (SIGINT, sighandler);
    signal (SIGTERM, sighandler);
  }

// -----------------------------------------------------------------------------
  void __attribute__ ( (destructor (65535))) end () {

    closeall();
  }
}

/* ========================================================================== */
