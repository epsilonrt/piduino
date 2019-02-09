/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
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
#pragma once

#ifdef __DOXYGEN__

/**
 * @addtogroup piduino_arduino
 * @{
 * 
 *  @defgroup piduino_sketch Sketch
 *
 *  This module provides the context for an Arduino sketch.
 *  @{
 */

/**
 * @brief Sketch setup function
 * 
 * This function is called when a sketch starts. Use it to initialize variables, 
 * pin modes, start using libraries, etc. This function will only run once, 
 * after program launch.
 */
void setup();

/**
 * @brief Sketch loop function.
 * 
 * After creating a setup() function, which initializes and sets the initial 
 * values, the loop() function does precisely what its name suggests, and loops 
 * consecutively, allowing your program to change and respond. 
 * Use it to actively control the main program.
 */
void loop();

/**
 * @brief Array of arguments passed by the command line
 * 
 * argv is an array of strings (character pointers) representing the individual 
 * arguments provided on the command line.
 */
extern char * argv[];

/**
 * @brief Number of parameters passed by the command line
 * 
 * argc will be the number of strings pointed to by argv. 
 * This will (in practice) be 1 plus the number of arguments.
 */
extern int argc;

/**
 * @brief TTY Console 
 * 
 * Object derived from the Stream class to access the TTY terminal.
 */
extern PiConsole Console;

/**
 * @brief Command Line Options Parser
 * 
 * The default options are :
 * - -h / --help : Switch (bool)
 * - -v / --verbose: Switch (bool)
 * .
 * 
 * available only if ARDUINO_NOOPTIONS is undefined.
 */
extern Piduino::OptionParser CmdLine;

/**
 * @def ARDUINO_NOOPTIONS
 * @brief Disable command-line options parsing
 * 
 * Must be defined before the inclusion of the `Piduino.h` header file to 
 * disable command line options parsing.
 */
 
/**
*   @}
* @}
*/

#else /* End of the documentation, __DOXYGEN__ not defined ------------------ */

#include <Arduino.h>

// Arduino sketch emulation
EXTERN_C void setup();
EXTERN_C void loop();

// Piduino global variables
int argc;
char **argv;
#ifdef __cplusplus
PiConsole Console;
#ifndef ARDUINO_NOOPTIONS
Piduino::OptionParser CmdLine ("Allowed options");
#endif
#endif

// ---
int main (int __arduino_argc, char ** __arduino_argv) {
  argc = __arduino_argc;
  argv = __arduino_argv;
  
#ifdef __cplusplus
// C++ only
#ifndef ARDUINO_NOOPTIONS
  CmdLine.add<Piduino::Switch> ("h", "help", "produce help message");
  CmdLine.add<Piduino::Switch> ("v", "verbose", "be verbose");
  CmdLine.parse (argc, argv);
#endif /* ARDUINO_NOOPTIONS not defined */
  ::HardwareSerial::setupAvailablePorts();
#endif /*  __cplusplus defined */

  setup();
  for (;;) {
    loop();
  }
  return 0;
}

/* ========================================================================== */
#endif /* __DOXYGEN__ not defined */
