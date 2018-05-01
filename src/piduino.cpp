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
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <memory>

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
#ifdef PIDUINO_WITH_GPIO
  //Gpio gpio;
#endif

// -----------------------------------------------------------------------------
  void closeall () {

#ifdef PIDUINO_WITH_GPIO
    //gpio.close();
#endif
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
