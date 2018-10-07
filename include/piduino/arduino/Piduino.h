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
#ifndef __PIDUINO_H__
#define __PIDUINO_H__

#include <Arduino.h>
#include <piduino/config.h>

#ifdef __cplusplus
#include <Console.h>
#if PIDUINO_WITH_SERIAL
#include <HardwareSerial.h>
#endif
PiConsole Console;
#endif

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else /* __cplusplus not defined */
#define EXTERN_C
#endif /* __cplusplus not defined */
#endif /* EXTERN_C not defined */

EXTERN_C void setup();
EXTERN_C void loop();

#if !(defined(ARDUINO_NOSETUP) && defined(ARDUINO_NOLOOP))
#ifndef ARDUINO_NOARGS
static int argc;
static char **argv;
int main (int __arduino_argc, char **__arduino_argv) {
  argc = __arduino_argc;
  argv = __arduino_argv;
#else
int main () {
#endif

#if PIDUINO_WITH_SERIAL && defined(__cplusplus)
    ::HardwareSerial::setupAvailablePorts();
#endif

#ifndef ARDUINO_NOSETUP
  setup();
#endif /* ARDUINO_NOSETUP not defined */
#ifndef ARDUINO_NOLOOP
  for (;;) {
    loop();
  }
#endif  /* ARDUINO_NOLOOP not defined */
  return 0;
}
#endif /* !(defined(ARDUINO_NOSETUP) && defined(ARDUINO_NOLOOP)) */

#endif /*__PIDUINO_H__ defined */
