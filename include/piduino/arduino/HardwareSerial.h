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
  Modified 2018 by Pascal JEAN (pascal.jean@piduino.org) for piduino library
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <deque>
#include <piduino/serialport.h>
#include "Terminal.h"

class HardwareSerial;

extern HardwareSerial & Serial;
extern HardwareSerial & Serial1;
extern HardwareSerial & Serial2;
extern HardwareSerial & Serial3;
extern HardwareSerial & Serial4;
extern HardwareSerial & Serial5;
extern HardwareSerial & Serial6;
extern HardwareSerial & Serial7;

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


// PiDuino Extension, Not for Arduino !
#define SERIAL_DATA_5 Piduino::SerialPort::DataBits::Data5
#define SERIAL_DATA_6 Piduino::SerialPort::DataBits::Data6
#define SERIAL_DATA_7 Piduino::SerialPort::DataBits::Data7
#define SERIAL_DATA_8 Piduino::SerialPort::DataBits::Data8
#define SERIAL_PARITY_NONE Piduino::SerialPort::Parity::NoParity
#define SERIAL_PARITY_EVEN Piduino::SerialPort::Parity::EvenParity
#define SERIAL_PARITY_ODD Piduino::SerialPort::Parity::OddParity
#define SERIAL_PARITY_SPACE Piduino::SerialPort::Parity::SpaceParity
#define SERIAL_PARITY_MARK Piduino::SerialPort::Parity::MarkParity
#define SERIAL_FLOW_NONE Piduino::SerialPort::FlowControl::NoFlowControl
#define SERIAL_FLOW_HARDWARE Piduino::SerialPort::FlowControl::HardwareControl
#define SERIAL_FLOW_SOFTWARE Piduino::SerialPort::FlowControl::SoftwareControl
#define SERIAL_FLOW_RTS_UP Piduino::SerialPort::FlowControl::Rs485RtsUpControl
#define SERIAL_FLOW_RTS_DOWN Piduino::SerialPort::FlowControl::Rs485RtsDownControl

class HardwareSerial : public ::Terminal {

  public:
    HardwareSerial ();
    explicit HardwareSerial (const Piduino::SerialPort::Info & serialPortInfo);
    explicit HardwareSerial (const String & path);
    virtual ~HardwareSerial();

    void begin (unsigned long baud, uint8_t config = SERIAL_8N1);
    void end();

    operator bool() {
      return true;
    }

    virtual size_t write (uint8_t);
    virtual size_t write (const uint8_t *buffer, size_t size);
    virtual size_t write (const String & str);

    // PiDuino Extension, Not for Arduino !
    void begin (unsigned long baud, const char * portName, uint8_t config = SERIAL_8N1);
    void begin (unsigned long baud, const String & portName, uint8_t config = SERIAL_8N1);

    virtual size_t writeln (uint8_t);
    virtual size_t writeln (const uint8_t *buffer, size_t size);
    virtual size_t writeln (const String & str);

    inline void setWritelnDelay (unsigned long delay);
    inline unsigned long writelnDelay() const;

    inline void setPath (const String & path);
    inline String path() const;
    inline void setPortName (const String & name);
    inline String portName() const;
    inline void setPort (const Piduino::SerialPort::Info & info);

    inline bool setBaudRate (int32_t baudRate, Piduino::SerialPort::Directions directions = Piduino::SerialPort::AllDirections);
    inline int32_t baudRate (Piduino::SerialPort::Directions directions = Piduino::SerialPort::AllDirections) const;
    inline bool setDataBits (Piduino::SerialPort::DataBits dataBits);
    inline Piduino::SerialPort::DataBits dataBits() const;
    inline bool setParity (Piduino::SerialPort::Parity parity);
    inline Piduino::SerialPort::Parity parity() const;
    inline bool setStopBits (Piduino::SerialPort::StopBits stopBits);
    inline Piduino::SerialPort::StopBits stopBits() const;
    inline bool setFlowControl (Piduino::SerialPort::FlowControl flowControl);
    inline Piduino::SerialPort::FlowControl flowControl() const;
    inline bool setDataTerminalReady (bool set);
    inline bool isDataTerminalReady();
    inline bool setRequestToSend (bool set);
    inline bool isRequestToSend();
    inline Piduino::SerialPort::PinoutSignals pinoutSignals();
    inline bool sendBreak (int duration = 0);
    inline bool setBreakEnabled (bool set = true);
    inline bool isBreakEnabled() const;
    inline bool setSettings (const Piduino::SerialPort::Settings & settings);
    inline Piduino::SerialPort::Settings settings() const;

    static void setupAvailablePorts();
    static const int numberOfPorts = 8;
    static std::deque<HardwareSerial> availablePorts;

  protected:
    virtual std::ostream & os();
    virtual Piduino::TerminalNotifier & notifier();
    virtual size_t writeln();

  private:
    std::shared_ptr<Piduino::SerialPort> port;
    unsigned long _writelnDelay; // to avoid buffer overflow on SoC Allwinner
};

inline void HardwareSerial::setWritelnDelay (unsigned long d) {
  _writelnDelay = d;
}
inline unsigned long HardwareSerial::writelnDelay() const {
  return _writelnDelay;
}
inline void HardwareSerial::setPath (const String & path) {
  port->setPath (path);
}
inline String HardwareSerial::path() const {
  return port->path();
}
inline void HardwareSerial::setPortName (const String & name) {
  port->setPortName (name);
}
inline String HardwareSerial::portName() const {
  return port->portName();
}
inline void HardwareSerial::setPort (const Piduino::SerialPort::Info & info) {
  port->setPort (info);
}
inline bool HardwareSerial::setBaudRate (int32_t baudRate, Piduino::SerialPort::Directions directions) {
  return port->setBaudRate (baudRate, directions);
}
inline int32_t HardwareSerial::baudRate (Piduino::SerialPort::Directions directions) const {
  return port->baudRate (directions);
}
inline bool HardwareSerial::setDataBits (Piduino::SerialPort::DataBits dataBits) {
  return port->setDataBits (dataBits);
}
inline Piduino::SerialPort::DataBits HardwareSerial::dataBits() const {
  return port->dataBits();
}
inline bool HardwareSerial::setParity (Piduino::SerialPort::Parity parity) {
  return port->setParity (parity);
}
inline Piduino::SerialPort::Parity HardwareSerial::parity() const {
  return port->parity();
}
inline bool HardwareSerial::setStopBits (Piduino::SerialPort::StopBits stopBits) {
  return port->setStopBits (stopBits);
}
inline Piduino::SerialPort::StopBits HardwareSerial::stopBits() const {
  return port->stopBits();
}
inline bool HardwareSerial::setFlowControl (Piduino::SerialPort::FlowControl flowControl) {
  return port->setFlowControl (flowControl);
}
inline Piduino::SerialPort::FlowControl HardwareSerial::flowControl() const {
  return port->flowControl();
}
inline bool HardwareSerial::setDataTerminalReady (bool set) {
  return port->setDataTerminalReady (set);
}
inline bool HardwareSerial::isDataTerminalReady() {
  return port->isDataTerminalReady();
}
inline bool HardwareSerial::setRequestToSend (bool set) {
  return port->setRequestToSend (set);
}
inline bool HardwareSerial::isRequestToSend() {
  return port->isRequestToSend();
}
inline Piduino::SerialPort::PinoutSignals HardwareSerial::pinoutSignals() {
  return port->pinoutSignals();
}
inline bool HardwareSerial::sendBreak (int duration) {
  return port->sendBreak (duration);
}
inline bool HardwareSerial::setBreakEnabled (bool set) {
  return port->setBreakEnabled (set);
}
inline bool HardwareSerial::isBreakEnabled() const {
  return port->isBreakEnabled();
}
inline bool HardwareSerial::setSettings (const Piduino::SerialPort::Settings & s) {
  return port->setSettings(s);
}
inline Piduino::SerialPort::Settings HardwareSerial::settings() const {
  return port->settings();
}

#endif
