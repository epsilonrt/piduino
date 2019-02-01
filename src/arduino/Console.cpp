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
#include <Console.h>
#include <unistd.h>
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
PiConsole::PiConsole() : _keyb(STDIN_FILENO), _notifier(&_keyb) {

}

// -----------------------------------------------------------------------------
void PiConsole::begin (__attribute__ ( (__unused__)) unsigned long,
                       __attribute__ ( (__unused__)) uint8_t) {

  Terminal::begin();
  _notifier.start ();
}

// -----------------------------------------------------------------------------
void PiConsole::end () {

  _notifier.terminate();
  Terminal::end();
}

// -----------------------------------------------------------------------------
ostream & PiConsole::os() {

  return cout;
}

// -----------------------------------------------------------------------------
Piduino::TerminalNotifier & PiConsole::notifier() {

  return _notifier;
}

/* ========================================================================== */
