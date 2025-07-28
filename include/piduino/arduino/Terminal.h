/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#ifndef Terminal_h
#define Terminal_h

#include <ostream>
#include <piduino/terminalnotifier.h>
#include "Stream.h"
//#include "Arduino.h"

class Terminal : public Stream {

  public:
    Terminal();
    virtual ~Terminal() {}

    int getReadError() {
      return read_error;
    }
    void clearReadError() {
      setReadError (0);
    }

    virtual void begin (unsigned long speed_dummy = 0, uint8_t config_dummy = 0);
    virtual void end();

    virtual int available();
    virtual int read();
    virtual int peek();

    virtual size_t write (uint8_t);
    virtual size_t write (const uint8_t *buffer, size_t size);
    virtual size_t write (const String & str);
    virtual void flush();

    size_t write (const char *str) {

      return write (String (str));
    }

    inline size_t write (unsigned long n) {

      return write ( (uint8_t) n);
    }

    inline size_t write (long n) {

      return write ( (uint8_t) n);
    }

    inline size_t write (unsigned int n) {

      return write ( (uint8_t) n);
    }

    inline size_t write (int n) {

      return write ( (uint8_t) n);
    }

  protected:
    virtual std::ostream & os() = 0;
    virtual Piduino::TerminalNotifier & notifier() = 0;
    virtual int timedRead();    // read stream with timeout
    virtual int timedPeek();    // peek stream with timeout
    virtual size_t writeln();

    void setReadError (int err = 1) {
      read_error = err;
    }

  private:
    int read_error;
    std::ostream::iostate os_state;
};
#endif
