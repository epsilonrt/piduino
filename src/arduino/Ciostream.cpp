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
#include <Ciostream.h>

using namespace std;

// -----------------------------------------------------------------------------
Ciostream::Ciostream() {

  cout.clear();
  cin.clear();
}

// -----------------------------------------------------------------------------
int Ciostream::available() {
  int length;

  cin.seekg (0, cin.end);
  length = cin.tellg();
  cin.seekg (0, cin.beg);
  return length;
}

// -----------------------------------------------------------------------------
int Ciostream::read() {

  if (cin.peek() != std::iostream::traits_type::eof()) {
    uint8_t c;
    cin >> c;
    return c;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int Ciostream::peek() {

  return cin.peek();
}

// -----------------------------------------------------------------------------
size_t Ciostream::write (uint8_t c) {

  cout.put (c);
  return cout.good() ? 1 : -1;
}

// -----------------------------------------------------------------------------
size_t Ciostream::write (const uint8_t *buffer, size_t size) {

  cout.write (reinterpret_cast<const char *>(buffer), size);
  return cout.good() ? size : -1;
}

// -----------------------------------------------------------------------------
void Ciostream::flush() {
  
  cout.flush();
}

/* ========================================================================== */
