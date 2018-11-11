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
#include <HardwareSerial.h>
#include <piduino/database.h>

using namespace Piduino;
using namespace std;

std::deque<HardwareSerial> HardwareSerial::availablePorts;

HardwareSerial & Serial  = HardwareSerial::availablePorts[0];
HardwareSerial & Serial1 = HardwareSerial::availablePorts[1];
HardwareSerial & Serial2 = HardwareSerial::availablePorts[2];
HardwareSerial & Serial3 = HardwareSerial::availablePorts[3];

// -----------------------------------------------------------------------------
void HardwareSerial::setupAvailablePorts() {
  int n;
  auto ports = SerialPort::Info::availablePorts();
  auto defaultPort = SerialPort::Info::defaultPort();

  availablePorts.push_back (HardwareSerial (defaultPort));
  n = 1;

  for (auto & p : ports) {

    if (n >= numberOfPorts) {

      break;
    }
    if (p != defaultPort) {

      availablePorts.push_back (HardwareSerial (p));
      n++;
    }
  }
  for (; n < numberOfPorts; n++) {
    availablePorts.push_back (HardwareSerial (defaultPort));
  }
}

// -----------------------------------------------------------------------------
HardwareSerial::HardwareSerial () :
  port (new Piduino::SerialPort ()), _writelnDelay (0) {
}

// -----------------------------------------------------------------------------
HardwareSerial::HardwareSerial (const Piduino::SerialPort::Info & serialPortInfo) :
  port (new Piduino::SerialPort (serialPortInfo)), _writelnDelay (0) {
}

// -----------------------------------------------------------------------------
HardwareSerial::HardwareSerial (const String & path) :
  port (new Piduino::SerialPort (path)), _writelnDelay (0) {
}

// -----------------------------------------------------------------------------
HardwareSerial::~HardwareSerial() = default;

// -----------------------------------------------------------------------------
void HardwareSerial::begin (unsigned long baud, uint8_t config) {
  SerialPort::Settings s (baud);

  Terminal::begin();
  switch (config) {
    case SERIAL_5N1:
      s.dataBits = SerialPort::Data5;
      break;
    case SERIAL_6N1:
      s.dataBits = SerialPort::Data6;
      break;
    case SERIAL_7N1:
      s.dataBits = SerialPort::Data7;
      break;
    case SERIAL_5N2:
      s.dataBits = SerialPort::Data5;
      s.stopBits = SerialPort::TwoStop;
      break;
    case SERIAL_6N2:
      s.dataBits = SerialPort::Data6;
      s.stopBits = SerialPort::TwoStop;
      break;
    case SERIAL_7N2:
      s.dataBits = SerialPort::Data7;
      s.stopBits = SerialPort::TwoStop;
      break;
    case SERIAL_8N2:
      s.stopBits = SerialPort::TwoStop;
      break;
    case SERIAL_5E1:
      s.dataBits = SerialPort::Data5;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_6E1:
      s.dataBits = SerialPort::Data6;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_7E1:
      s.dataBits = SerialPort::Data7;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_8E1:
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_5E2:
      s.dataBits = SerialPort::Data5;
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_6E2:
      s.dataBits = SerialPort::Data6;
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_7E2:
      s.dataBits = SerialPort::Data7;
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_8E2:
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::EvenParity;
      break;
    case SERIAL_5O1:
      s.dataBits = SerialPort::Data5;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_6O1:
      s.dataBits = SerialPort::Data6;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_7O1:
      s.dataBits = SerialPort::Data7;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_8O1:
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_5O2:
      s.dataBits = SerialPort::Data5;
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_6O2:
      s.dataBits = SerialPort::Data6;
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_7O2:
      s.dataBits = SerialPort::Data7;
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_8O2:
      s.stopBits = SerialPort::TwoStop;
      s.parity = SerialPort::OddParity;
      break;
    case SERIAL_8N1:
    default:
      break;
  }
  port->setSettings (s);
  port->setBaudRate (baud);
  if (port->open(IoDevice::ReadWrite | IoDevice::Binary)) {
    Terminal::begin();
    if (portName().startsWith ("ttyS")  &&
        (db.board().soc().family().id() == SoC::Family::AllwinnerH)) {

      _writelnDelay = 500; // to avoid buffer overflow on SoC Allwinner
      // TODO: Analyze the sun8i driver code to understand why this is needed !
    }
    else if (portName().startsWith ("ttyAMA")  &&
             (db.board().soc().family().id() == SoC::Family::BroadcomBcm2835)) {

      _writelnDelay = 4000; // to avoid buffer overflow on Bcm2835 
      // TODO: Analyze the Bcm2835 driver code to understand why this is needed !
    }
  }
}

// -----------------------------------------------------------------------------
void HardwareSerial::begin (unsigned long baud, const String & name, uint8_t config) {

  setPortName (name);
  begin (baud, name.c_str());
}

// -----------------------------------------------------------------------------
void HardwareSerial::begin (unsigned long baud, const char * name, uint8_t config) {

  setPortName (name);
  begin (baud, config);
}

// -----------------------------------------------------------------------------
void HardwareSerial::end() {

  Terminal::end();
  port->close();
}

// -----------------------------------------------------------------------------
// virtual
std::ostream & HardwareSerial::os() {

  return *port;
}

// -----------------------------------------------------------------------------
// virtual
Piduino::TerminalNotifier & HardwareSerial::notifier() {

  return port->notifier();
}

// -----------------------------------------------------------------------------
size_t HardwareSerial::writeln() {

  size_t ret = ::Terminal::writeln();
  delayMicroseconds (_writelnDelay);
  return ret;
}

/* ========================================================================== */
