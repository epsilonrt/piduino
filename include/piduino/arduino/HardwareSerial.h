/*
  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <deque>
#include <piduino/serialport.h>
#include "Stream.h"

class HardwareSerial;

extern HardwareSerial & Serial;
extern HardwareSerial & Serial1;
extern HardwareSerial & Serial2;
extern HardwareSerial & Serial3;

// Define config for Serial.begin(baud, config);
#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E

class HardwareSerial : public Stream {

  public:
    HardwareSerial ();
    explicit HardwareSerial (const Piduino::SerialPort::Info & serialPortInfo);
    explicit HardwareSerial (const String & path);
    virtual ~HardwareSerial();

    void begin (unsigned long baud, uint8_t config = SERIAL_8N1);
    void end();

    virtual int availableForWrite (void);
    operator bool() {
      return true;
    }
    using Print::write;

    // Piduino
    inline void setPath (const std::string & path) {
      port->setPath (path);
    }
    inline std::string path() const {
      return port->path();
    }
    inline void setPortName (const std::string & name) {
      port->setPortName (name);
    }
    inline std::string portName() const {
      return port->portName();
    }
    inline void setPort (const Piduino::SerialPort::Info & info) {
      port->setPort (info);
    }

    static void setupAvailablePorts();
    static const int numberOfPorts = 4;
    static std::deque<HardwareSerial> availablePorts;

  protected:
    virtual std::ostream & os(); // for Print
    virtual std::istream & is(); // for Stream
    std::shared_ptr<Piduino::SerialPort> port;
};

#endif
