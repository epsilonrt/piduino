/*
  Print.h - Base class that provides print() and println()
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
  
  Modified 2018 by Pascal JEAN (epsilonrt@gmail.com) to piduino library
*/

#ifndef Print_h
#define Print_h

#include <cstdio> // for size_t
#include <cstring>
#include <ostream>

#include "WString.h"
#include "Printable.h"
#include <Arduino.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print {
  private:
    size_t printNumber (unsigned long, uint8_t);
    size_t printFloat (double, uint8_t);

  protected:
    virtual std::ostream & os() = 0;
    virtual void setWriteError (int err = 1);

  public:
    Print();
    virtual ~Print();

    virtual int getWriteError();
    virtual void clearWriteError();

    virtual size_t write (uint8_t b);
    virtual size_t write (const uint8_t *buffer, size_t size);
    virtual size_t write (const char *str);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    size_t print (const String &);
    size_t print (const char[]);
    size_t print (char);
    size_t print (unsigned char, int = DEC);
    size_t print (int, int = DEC);
    size_t print (unsigned int, int = DEC);
    size_t print (long, int = DEC);
    size_t print (unsigned long, int = DEC);
    size_t print (double, int = 2);
    size_t print (double);
    size_t print (const Printable&);

    virtual size_t println ();
    size_t println (const String &s);
    size_t println (const char[]);
    size_t println (char);
    size_t println (unsigned char, int = DEC);
    size_t println (int, int = DEC);
    size_t println (unsigned int, int = DEC);
    size_t println (long, int = DEC);
    size_t println (unsigned long, int = DEC);
    size_t println (double, int = 2);
    size_t println (const Printable&);
};

#endif
