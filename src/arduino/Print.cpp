/*
 Print.cpp - Base class that provides print() and println()
 Copyright (c) 2008 David A. Mellis.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Modified 23 November 2006 by David A. Mellis
 Modified 2018 by Pascal JEAN (pascal.jean@piduino.org) for piduino library
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <sstream>

#include <Print.h>

using namespace std;

// Public Methods //////////////////////////////////////////////////////////////

/* virtual: default implementation: may be overridden */
size_t Print::write (const uint8_t *buffer, size_t size) {
  size_t n = 0;
  while (size--) {
    if (write (*buffer++)) {
      n++;
    }
    else {
      break;
    }
  }
  return n;
}

/* virtual: default implementation: may be overridden */
size_t Print::writeln() {
  return write ("\r\n");
}

/* virtual: default implementation: may be overridden */
size_t Print::writeln (const uint8_t *buffer, size_t size) {
  size_t n = write (buffer, size);
  return n + writeln();
}

size_t Print::print (const String &s) {

  return write (s.c_str());
}

size_t Print::print (const char str[]) {

  return write (str);
}

size_t Print::print (char c) {

  return write (c);
}

size_t Print::print (const __FlashStringHelper *ifsh) {

  return print (reinterpret_cast<PGM_P> (ifsh));
}

size_t Print::print (unsigned long n, int base) {

  return print (toString (n, base));
}

size_t Print::print (long n, int base) {
  String str;
  
  if (base == 10 && n < 0) {

    str.concat ('-');
    n = -n;
  }
  str.concat(toString ((unsigned long) n, base));
  return print (str);
}

size_t Print::print (unsigned char b, int base) {

  return print ( (unsigned long) b, base);
}

size_t Print::print (int n, int base) {

  return print ( (long) n, base);
}

size_t Print::print (unsigned int n, int base) {

  return print ( (unsigned long) n, base);
}

size_t Print::print (double n, int digits) {

  return print (toString (n, digits));
}

size_t Print::print (const Printable& x) {

  return x.printTo (*this);
}

// -----------------------------------------------------------------------------
// println
//

size_t Print::println() {
  return writeln();
}

size_t Print::println (const String &s) {

  return writeln (s.c_str());
}

size_t Print::println (const char str[]) {

  return writeln (str);
}

size_t Print::println (char c) {

  return writeln (&c, 1);
}

size_t Print::println (const __FlashStringHelper *ifsh) {

  return println (reinterpret_cast<PGM_P> (ifsh));
}


size_t Print::println (unsigned long n, int base) {

  return println (toString (n, base));
}

size_t Print::println (long n, int base) {
  String str;
  
  if (base == 10 && n < 0) {

    str.concat ('-');
    n = -n;
  }
  str.concat(toString ((unsigned long) n, base));
  return println (str);
}

size_t Print::println (unsigned char b, int base) {

  return println ( (unsigned long) b, base);
}

size_t Print::println (int n, int base) {

  return println ( (long) n, base);
}

size_t Print::println (unsigned int n, int base) {

  return println ( (unsigned long) n, base);
}

size_t Print::println (double n, int digits) {

  return println (toString (n, digits));
}

size_t Print::println (const Printable& x) {
  size_t n = print (x);
  n += println();
  return n;
}


// Private Methods /////////////////////////////////////////////////////////////

String Print::toString (unsigned long n, uint8_t base) {
  char buf[8 * sizeof (long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof (buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) {
    base = 10;
  }

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  }
  while (n);

  return String (str);
}

String Print::toString (double number, uint8_t digits) {

  if (std::isnan (number)) {

    return String ("nan");
  }
  if (std::isinf (number)) {

    return String ("inf");
  }

  String str;

  // Handle negative numbers
  if (number < 0.0) {

    str.concat ('-');
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;

  for (uint8_t i = 0; i < digits; ++i) {

    rounding /= 10.0;
  }

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long) number;
  double remainder = number - (double) int_part;
  str.concat (int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    str.concat ('.');
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {

    remainder *= 10.0;
    int toPrint = int (remainder);
    str.concat (toPrint);
    remainder -= toPrint;
  }

  return str;
}
