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
#include <cerrno>
#include <Terminal.h>

using namespace std;

// -----------------------------------------------------------------------------
Terminal::Terminal() : read_error (0), os_state (ios::goodbit) {
}

// -----------------------------------------------------------------------------
void Terminal::begin (__attribute__ ( (__unused__)) unsigned long,
                      __attribute__ ( (__unused__)) uint8_t) {

  os_state = os().exceptions();
  os().exceptions (ios::failbit | ios::badbit);
  os().clear();
}

// -----------------------------------------------------------------------------
void Terminal::end () {

  os().exceptions (os_state);
}

// -----------------------------------------------------------------------------
int Terminal::available() {
  int ret = notifier().available();
  if (ret < 0) {
    setReadError (errno);
  }
  return ret;
}

// -----------------------------------------------------------------------------
int Terminal::read() {
  char c;

  if (notifier().read (c) == 1) {
    return static_cast<unsigned char> (c);
  }
  setReadError (errno);
  return -1;
}

// -----------------------------------------------------------------------------
// protected method to read stream with timeout
int Terminal::timedRead() {
  char c;

  if (notifier().read (c, _timeout) == 1) {
    return static_cast<unsigned char> (c);
  }
  setReadError (errno);
  return -1;
}

// -----------------------------------------------------------------------------
int Terminal::peek() {
  char c;

  if (notifier().peek (c) == 1) {
    return static_cast<unsigned char> (c);
  }
  setReadError (errno);
  return -1;
}

// -----------------------------------------------------------------------------
// protected method to peek stream with timeout
int Terminal::timedPeek() {
  char c;

  if (notifier().peek (c, _timeout) == 1) {
    return static_cast<unsigned char> (c);
  }
  setReadError (errno);
  return -1;
}

// -----------------------------------------------------------------------------
size_t Terminal::write (uint8_t c) {

  try {
    os().put (c);
  }
  catch (ios_base::failure e) {
    setWriteError (e.code().value());
    os().clear();
    return -1;
  }

  return 1;
}

// -----------------------------------------------------------------------------
size_t Terminal::write (const String & str) {

  try {
    os() << str;
  }
  catch (ios_base::failure e) {
    setWriteError (e.code().value());
    os().clear();
    return -1;
  }

  return str.length();
}

// -----------------------------------------------------------------------------
size_t Terminal::write (const uint8_t *buffer, size_t size) {

  try {
    os().write (reinterpret_cast<const char *> (buffer), size);
  }
  catch (ios_base::failure e) {
    setWriteError (e.code().value());
    os().clear();
    return -1;
  }

  return size;
}

// -----------------------------------------------------------------------------
void Terminal::flush() {
  try {
    os().flush();
  }
  catch (ios_base::failure e) {
    setWriteError (e.code().value());
    os().clear();
  }
}

// -----------------------------------------------------------------------------
size_t Terminal::writeln() {
  
  try {
    os() << std::endl;
  }
  catch (ios_base::failure e) {
    setWriteError (e.code().value());
    os().clear();
    return -1;
  }
  return 2;
}

/* ========================================================================== */
