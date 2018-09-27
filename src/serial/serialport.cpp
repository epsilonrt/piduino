/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
  * SerialPort is a modified and simplified version of QSerialPort,
 * from Qt according to the LGPL and Copyright (C) 2015 The Qt Company Ltd.
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
 */
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <libudev.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <linux/serial.h>
#include <piduino/database.h>
#include <piduino/clock.h>
#include <piduino/string.h>
#include "serialport_p.h"
#include "config.h"

struct termios2 {
  tcflag_t c_iflag;       /* input mode flags */
  tcflag_t c_oflag;       /* output mode flags */
  tcflag_t c_cflag;       /* control mode flags */
  tcflag_t c_lflag;       /* local mode flags */
  cc_t c_line;            /* line discipline */
  cc_t c_cc[19];          /* control characters */
  speed_t c_ispeed;       /* input speed */
  speed_t c_ospeed;       /* output speed */
};

#ifndef TCGETS2
#define TCGETS2     _IOR('T', 0x2A, struct termios2)
#endif

#ifndef TCSETS2
#define TCSETS2     _IOW('T', 0x2B, struct termios2)
#endif

#ifndef BOTHER
#define BOTHER      0010000
#endif

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                        SerialPort::Settings Class
//
// -----------------------------------------------------------------------------
  static const std::string strUnknown ("Unknown");
  static const std::string strNone ("none");

  // ---------------------------------------------------------------------------
  SerialPort::Settings::Settings (int32_t baudRate, DataBits dataBits,
                                  Parity parity, StopBits stopBits,
                                  FlowControl flowControl) :
    inputBaudRate (baudRate),
    outputBaudRate (baudRate),
    dataBits (dataBits),
    parity (parity),
    stopBits (stopBits),
    flowControl (flowControl)
  {}

  // ---------------------------------------------------------------------------
  bool SerialPort::Settings::operator== (const Settings & other) {

    return inputBaudRate == other.inputBaudRate &&
           outputBaudRate == other.outputBaudRate &&
           dataBits == other.dataBits &&
           parity == other.parity &&
           stopBits == other.stopBits;
  }

  // ---------------------------------------------------------------------------
  std::string
  SerialPort::Settings::toString() const {
    // BBBBBBBBBB-DPSF
    std::string str;
    std::ostringstream os;

    os << inputBaudRate;
    if (inputBaudRate != outputBaudRate) {
      os << "-" << outputBaudRate;
    }
    os << "-" << static_cast<int> (dataBits) << static_cast<char> (parity) <<
       static_cast<int> (stopBits) << static_cast<char> (flowControl);
    return os.str();
  }


  // ---------------------------------------------------------------------------
  std::string SerialPort::Settings::flowControlString() const {

    return flowControlToString (flowControl);
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Settings::parityString() const {

    return parityToString (parity);
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Settings::dataBitsString() const {

    return dataBitsToString (dataBits);
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Settings::stopBitsString() const {

    return stopBitsToString (stopBits);
  }

  // ---------------------------------------------------------------------------
  std::string  SerialPort::Settings::dataBitsToString (DataBits d) {

    switch (d) {

      case Data5:
        return std::string ("5");
      case Data6:
        return std::string ("6");
      case Data7:
        return std::string ("7");
      case Data8:
        return std::string ("8");
      default:
        break;
    }
    return strUnknown;
  }

  // ---------------------------------------------------------------------------
  std::string  SerialPort::Settings::parityToString (Parity p) {

    switch (p) {

      case NoParity:
        return strNone;
      case EvenParity:
        return std::string ("even");
      case OddParity:
        return std::string ("odd");
      case SpaceParity:
        return std::string ("space");
      case MarkParity:
        return std::string ("mark");
      default:
        break;
    }
    return strUnknown;
  }

  // ---------------------------------------------------------------------------
  std::string  SerialPort::Settings::stopBitsToString (StopBits s) {

    switch (s) {

      case OneStop:
        return std::string ("1");
      case TwoStop:
        return std::string ("2");
      default:
        break;
    }
    return strUnknown;
  }

  // ---------------------------------------------------------------------------
  std::string  SerialPort::Settings::flowControlToString (FlowControl f) {

    switch (f) {

      case NoFlowControl:
        return strNone;
      case HardwareControl:
        return std::string ("hardware");
      case SoftwareControl:
        return std::string ("software");
      case Rs485AfterSendControl:
        return std::string ("rs485 rts after send");
      case Rs485OnSendControl:
        return std::string ("rs485 rts on send");
      default:
        break;
    }
    return strUnknown;
  }

  // ---------------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& os, const  SerialPort::Settings & s)  {

    os << s.toString();
    return os;
  }

// -----------------------------------------------------------------------------
//
//                             SerialPort Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SerialPort::SerialPort (SerialPort::Private &dd) : Terminal (dd) {

  }

  // ---------------------------------------------------------------------------
  SerialPort::SerialPort () :
    Terminal (*new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  SerialPort::SerialPort (const Info & info) : SerialPort() {

    setPort (info);
  }

  // ---------------------------------------------------------------------------
  SerialPort::SerialPort (const std::string & name) : SerialPort() {

    setPortName (name);
  }

  // ---------------------------------------------------------------------------
  SerialPort::~SerialPort() = default;

  // ---------------------------------------------------------------------------
  void SerialPort::setPort (const Info & info) {

    setPath (info.systemLocation());
  }

  // ---------------------------------------------------------------------------
  void SerialPort::setPortName (const std::string & name) {

    setPath (Private::portNameToSystemLocation (name));
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::portName() const {

    return Private::portNameFromSystemLocation (path());
  }

  // ---------------------------------------------------------------------------
  void SerialPort::discard (Directions directions) {

    if (isOpen()) {
      PIMP_D (SerialPort);

      d->tcflush (directions);
    }
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setSettings (const Settings & settings) {
    PIMP_D (SerialPort);
    bool success;
    bool ibc = (d->settings.inputBaudRate != settings.inputBaudRate);
    bool obc = (d->settings.outputBaudRate != settings.outputBaudRate);
    bool beq = (settings.inputBaudRate == settings.outputBaudRate);

    success = setParity (settings.parity);
    success = success && setDataBits (settings.dataBits);
    success = success && setStopBits (settings.stopBits);
    success = success && setFlowControl (settings.flowControl);
    if (ibc && obc && beq) {

      success = success && setBaudRate (settings.inputBaudRate, AllDirections);
    }
    else {
      if (ibc && (!obc || (obc && !beq))) {

        success = success && setBaudRate (settings.inputBaudRate, Input);
      }
      if (obc && (!ibc || (ibc && !beq))) {

        success = success && setBaudRate (settings.outputBaudRate, Output);
      }

    }
    return success;
  }

  // ---------------------------------------------------------------------------
  SerialPort::Settings SerialPort::settings() const {
    PIMP_D (const SerialPort);

    return d->settings;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setBaudRate (int32_t baudRate, Directions directions) {
    PIMP_D (SerialPort);
    bool hasChanged = false;

    if ( (directions & Input) && (d->settings.inputBaudRate != baudRate)) {
      d->settings.inputBaudRate = baudRate;
      hasChanged = true;
    }
    if ( (directions & Output) && (d->settings.outputBaudRate != baudRate)) {
      d->settings.outputBaudRate = baudRate;
      hasChanged = true;
    }
    if (isOpen() && hasChanged) {
      return d->setBaudRate (baudRate, directions);
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  int32_t SerialPort::baudRate (Directions directions) const {
    PIMP_D (const SerialPort);

    if (directions == AllDirections) {

      return d->settings.inputBaudRate == d->settings.outputBaudRate ?
             d->settings.inputBaudRate : -1;
    }
    return (directions & Input) ?
           d->settings.inputBaudRate : d->settings.outputBaudRate;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setDataBits (DataBits dataBits) {
    PIMP_D (SerialPort);
    bool hasChanged = (d->settings.dataBits != dataBits);

    if (hasChanged) {
      d->settings.dataBits = dataBits;
      if (isOpen()) {
        return d->setDataBits ();
      }
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  SerialPort::DataBits SerialPort::dataBits() const {
    PIMP_D (const SerialPort);

    return d->settings.dataBits;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setParity (Parity parity) {
    PIMP_D (SerialPort);
    bool hasChanged = (d->settings.parity != parity);

    if (hasChanged) {
      d->settings.parity = parity;
      if (isOpen()) {
        return d->setParity ();
      }
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  SerialPort::Parity SerialPort::parity() const {
    PIMP_D (const SerialPort);

    return d->settings.parity;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setStopBits (StopBits stopBits) {
    PIMP_D (SerialPort);
    bool hasChanged = (d->settings.stopBits != stopBits);

    if (hasChanged) {
      d->settings.stopBits = stopBits;
      if (isOpen()) {
        return d->setStopBits ();
      }
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  SerialPort::StopBits SerialPort::stopBits() const {
    PIMP_D (const SerialPort);

    return d->settings.stopBits;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setFlowControl (FlowControl flowControl) {
    PIMP_D (SerialPort);
    bool hasChanged = (d->settings.flowControl != flowControl);

    if (hasChanged) {
      d->settings.flowControl = flowControl;
      if (isOpen()) {
        return d->setFlowControl ();
      }
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  SerialPort::FlowControl SerialPort::flowControl() const {
    PIMP_D (const SerialPort);

    return d->settings.flowControl;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setDataTerminalReady (bool set) {
    PIMP_D (SerialPort);

    if (isOpen()) {

      return d->setDataTerminalReady (set);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::isDataTerminalReady() {
    PIMP_D (SerialPort);

    return d->pinoutSignals() & DataTerminalReadySignal;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setRequestToSend (bool set) {

    if (isOpen()) {
      PIMP_D (SerialPort);

      if ( (d->settings.flowControl == NoFlowControl) ||
           (d->settings.flowControl == SoftwareControl)) {

        return  d->setRequestToSend (set);
      }
      d->setError (ENOTSUP);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::isRequestToSend() {
    PIMP_D (SerialPort);

    return d->pinoutSignals() & RequestToSendSignal;
  }

  // ---------------------------------------------------------------------------
  SerialPort::PinoutSignals SerialPort::pinoutSignals() {
    PIMP_D (SerialPort);

    if (isOpen()) {
      return d->pinoutSignals();
    }
    return false;
  }


  // ---------------------------------------------------------------------------
  bool SerialPort::sendBreak (int duration) {
    PIMP_D (SerialPort);

    if (isOpen()) {
      return d->sendBreak (duration);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::setBreakEnabled (bool set) {
    PIMP_D (SerialPort);

    if (isOpen()) {
      return d->setBreakEnabled (set);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::isBreakEnabled() const {
    PIMP_D (const SerialPort);

    return d->isBreakEnabled;
  }

// -----------------------------------------------------------------------------
//
//                         SerialPort::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SerialPort::Private::Private (SerialPort * q) :
    Terminal::Private (q) {

  }

  // ---------------------------------------------------------------------------
  SerialPort::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::open (OpenMode mode, int additionalPosixFlags) {

    if (Terminal::Private::open (mode, O_NOCTTY | O_NONBLOCK)) {

      // Acquire non-blocking exclusive lock
      if (::flock (fd, LOCK_EX | LOCK_NB) == 0) {

        if (initialize (mode)) {

          return true;
        }
      }
      setError();
      close();
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  inline bool SerialPort::Private::initialize (OpenMode mode) {

    // Check if the file descriptor is pointing to a TTY device or not.
    if (!isatty (fd)) {

      return false;
    }

#ifdef TIOCEXCL
    ioctl (TIOCEXCL);
#endif

    termios tio;
    if (!getTermios (&tio)) {
      return false;
    }

    restoredTermios = tio;

    setTioCommonProps (&tio, mode);
    setTioDatabits (&tio, settings.dataBits);
    setTioParity (&tio, settings.parity);
    setTioStopbits (&tio, settings.stopBits);
    setTioFlowcontrol (&tio, settings.flowControl);

    if (!setTermios (&tio)) {
      return false;
    }

    if (!setFlowControl()) {
      return false;
    }

    if (!setBaudRate()) {
      return false;
    }

    tcflush (AllDirections);
    clk.delay (10);
    return true;
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::close () {

    if (settingsRestoredOnClose) {
      ::tcsetattr (fd, TCSANOW, &restoredTermios);
    }

#ifdef TIOCNXCL
    ::ioctl (fd, TIOCNXCL);
#endif

    if (::flock (fd, LOCK_UN | LOCK_NB) < 0) {

      setError();
    }
    Terminal::Private::close();
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::tcflush (Directions directions) {
    int ret;

    ret = ::tcflush (fd, (directions == AllDirections)
                     ? TCIOFLUSH : (directions & Input) ? TCIFLUSH : TCOFLUSH);

    if (ret == -1) {
      setError ();
    }
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::sendBreak (int duration) {
    int ret;

    ret = ::tcsendbreak (fd, duration);

    if (ret < 0) {
      setError ();
      return false;
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setBreakEnabled (bool set) {

    if (ioctl (set ? TIOCSBRK : TIOCCBRK) < 0) {
      return false;
    }

    return true;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setDataBits () {
    termios tio;
    if (!getTermios (&tio)) {
      return false;
    }
    setTioDatabits (&tio, settings.dataBits);
    return setTermios (&tio);
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setParity () {
    termios tio;
    if (!getTermios (&tio)) {
      return false;
    }
    setTioParity (&tio, settings.parity);
    return setTermios (&tio);
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setStopBits () {
    termios tio;
    if (!getTermios (&tio)) {
      return false;
    }
    setTioStopbits (&tio, settings.stopBits);
    return setTermios (&tio);
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setFlowControl () {
    bool success;

    termios tio;
    if (!getTermios (&tio)) {
      return false;
    }
    setTioFlowcontrol (&tio, settings.flowControl);
    success = setTermios (&tio);

    if (success) {
      struct serial_rs485 rs485conf;

      memset (&rs485conf, 0, sizeof (rs485conf));
      if ( (settings.flowControl == Rs485AfterSendControl) ||
           (settings.flowControl == Rs485OnSendControl))  {

        /* Enable RS485 mode: */
        rs485conf.flags = SER_RS485_ENABLED;

        if (settings.flowControl == Rs485AfterSendControl) {

          /* Set logical level for RTS pin equal to 0 (asserted) after sending: */
          rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
        }
        else {

          /* Set logical level for RTS pin equal to 0 (asserted) when sending: */
          rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        }

        /* Set rts delay before send, if needed: */
        // TODO: rs485conf.delay_rts_before_send = ...;

        /* Set rts delay after send, if needed: */
        // TODO: rs485conf.delay_rts_after_send = ...;

        /* Set this flag if you want to receive data even whilst sending data */
        // TODO: rs485conf.flags |= SER_RS485_RX_DURING_TX;

        success = (ioctl (TIOCSRS485, &rs485conf) == 0);
        if (success) {
          int status;

          // Setting-up RTS and DTR Pins
          success = success && (ioctl (TIOCMGET, &status) == 0);
          status |= TIOCM_DTR | TIOCM_RTS; // TODO: RTS On or Off ?
          success = success && (ioctl (TIOCMSET, &status) == 0);
        }
      }
      else {
        /*
         * Désactive le mode RS485 en ignorant les erreurs (renvoyées par les
         * drivers ne gérant pas le RS485)
         */
        (void) ioctl (TIOCSRS485, &rs485conf);
      }
    }
    return success;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setBaudRate() {

    if (settings.inputBaudRate == settings.outputBaudRate) {

      return setBaudRate (settings.inputBaudRate, AllDirections);
    }

    return (setBaudRate (settings.inputBaudRate, Input)
            && setBaudRate (settings.outputBaudRate, Output));
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setBaudRate (int32_t baudRate, Directions directions) {

    if (baudRate <= 0) {
      setError (EOPNOTSUPP, "Invalid baud rate value");
      return false;
    }

    const int32_t unixBaudRate = settingFromBaudRate (baudRate);

    return (unixBaudRate > 0)
           ? setStandardBaudRate (unixBaudRate, directions)
           : setCustomBaudRate (baudRate, directions);

  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setStandardBaudRate (int32_t baudRate, Directions directions) {

    // try to clear custom baud rate, using termios v2
    struct termios2 tio2;
    if (ioctl (TCGETS2, &tio2) != -1) {
      if (tio2.c_cflag & BOTHER) {
        tio2.c_cflag &= ~BOTHER;
        tio2.c_cflag |= CBAUD;
        ioctl (TCSETS2, &tio2);
      }
    }

    // try to clear custom baud rate, using serial_struct (old way)
    struct serial_struct serial;
    ::memset (&serial, 0, sizeof (serial));
    if (ioctl (TIOCGSERIAL, &serial) != -1) {
      if (serial.flags & ASYNC_SPD_CUST) {
        serial.flags &= ~ASYNC_SPD_CUST;
        serial.custom_divisor = 0;
        // we don't check on errors because a driver can has not this feature
        ioctl (TIOCSSERIAL, &serial);
      }
    }

    termios tio;
    if (!getTermios (&tio)) {
      return false;
    }

    if ( (directions & Input) && ::cfsetispeed (&tio, baudRate) < 0) {
      setError ();
      return false;
    }

    if ( (directions & Output) && ::cfsetospeed (&tio, baudRate) < 0) {
      setError ();
      return false;
    }

    return setTermios (&tio);
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setCustomBaudRate (int32_t baudRate, Directions directions) {

    if (directions != AllDirections) {
      setError (EOPNOTSUPP, "Cannot set custom speed for one direction");
      return false;
    }

    struct termios2 tio2;

    if (ioctl (TCGETS2, &tio2) != -1) {
      tio2.c_cflag &= ~CBAUD;
      tio2.c_cflag |= BOTHER;

      tio2.c_ispeed = baudRate;
      tio2.c_ospeed = baudRate;

      if (ioctl (TCSETS2, &tio2) != -1
          && ioctl (TCGETS2, &tio2) != -1) {
        return true;
      }
    }

    struct serial_struct serial;

    if (ioctl (TIOCGSERIAL, &serial) == -1) {
      return false;
    }

    serial.flags &= ~ASYNC_SPD_MASK;
    serial.flags |= (ASYNC_SPD_CUST /* | ASYNC_LOW_LATENCY*/);
    serial.custom_divisor = serial.baud_base / baudRate;

    if (serial.custom_divisor == 0) {
      setError (EOPNOTSUPP, "No suitable custom baud rate divisor");
      return false;
    }

    if (serial.custom_divisor * baudRate != serial.baud_base) {
      std::cerr << "Baud rate of serial port " << path
                << " is set to " << float (serial.baud_base) / serial.custom_divisor
                << " instead of " << baudRate
                << " divisor " << float (serial.baud_base) / baudRate
                << " unsupported" << std::endl;
    }

    if (ioctl (TIOCSSERIAL, &serial) == -1) {

      return false;
    }

    return setStandardBaudRate (B38400, directions);
  }

  // ---------------------------------------------------------------------------
  SerialPort::PinoutSignals SerialPort::Private::pinoutSignals() {
    int arg = 0;

    if (ioctl (TIOCMGET, &arg) == -1) {

      return SerialPort::NoSignal;
    }

    SerialPort::PinoutSignals ret = SerialPort::NoSignal;

#ifdef TIOCM_LE
    if (arg & TIOCM_LE) {
      ret |= SerialPort::DataSetReadySignal;
    }
#endif
#ifdef TIOCM_DTR
    if (arg & TIOCM_DTR) {
      ret |= SerialPort::DataTerminalReadySignal;
    }
#endif
#ifdef TIOCM_RTS
    if (arg & TIOCM_RTS) {
      ret |= SerialPort::RequestToSendSignal;
    }
#endif
#ifdef TIOCM_ST
    if (arg & TIOCM_ST) {
      ret |= SerialPort::SecondaryTransmittedDataSignal;
    }
#endif
#ifdef TIOCM_SR
    if (arg & TIOCM_SR) {
      ret |= SerialPort::SecondaryReceivedDataSignal;
    }
#endif
#ifdef TIOCM_CTS
    if (arg & TIOCM_CTS) {
      ret |= SerialPort::ClearToSendSignal;
    }
#endif
#ifdef TIOCM_CAR
    if (arg & TIOCM_CAR) {
      ret |= SerialPort::DataCarrierDetectSignal;
    }
#elif defined(TIOCM_CD)
    if (arg & TIOCM_CD) {
      ret |= SerialPort::DataCarrierDetectSignal;
    }
#endif
#ifdef TIOCM_RNG
    if (arg & TIOCM_RNG) {
      ret |= SerialPort::RingIndicatorSignal;
    }
#elif defined(TIOCM_RI)
    if (arg & TIOCM_RI) {
      ret |= SerialPort::RingIndicatorSignal;
    }
#endif
#ifdef TIOCM_DSR
    if (arg & TIOCM_DSR) {
      ret |= SerialPort::DataSetReadySignal;
    }
#endif

    return ret;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setDataTerminalReady (bool set) {
    int status = TIOCM_DTR;

    if (ioctl (set ? TIOCMBIS : TIOCMBIC, &status) == -1) {

      return false;
    }

    return true;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setRequestToSend (bool set) {
    int status = TIOCM_RTS;

    if (ioctl (set ? TIOCMBIS : TIOCMBIC, &status) == -1) {

      return false;
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::setTermios (const termios *tio) {
    int ret;

    ret = ::tcsetattr (fd, TCSANOW, tio);

    if (ret < 0) {

      setError ();
      return false;
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Private::getTermios (termios *tio) {
    int ret;

    ::memset (tio, 0, sizeof (termios));

    ret = ::tcgetattr (fd, tio);

    if (ret < 0) {

      setError ();
      return false;
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  std::string
  SerialPort::Private::portNameToSystemLocation (const std::string &port) {
    const String & source = reinterpret_cast<const String &> (port);
    return (source.startsWith ('/')
            || source.startsWith ("./")
            || source.startsWith ("../"))
           ? port : std::string ("/dev/") + port;
  }

  // ---------------------------------------------------------------------------
  std::string
  SerialPort::Private::portNameFromSystemLocation (const std::string &location) {
    const String & source = reinterpret_cast<const String &> (location);

    return source.startsWith ("/dev/")
           ? location.substr (5, std::string::npos) : location;
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::setTioCommonProps (termios *tio, OpenMode m) {

    ::cfmakeraw (tio);
    /* cfmakeraw() ... The terminal attributes are set as follows:
     * termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
     *           | INLCR | IGNCR | ICRNL | IXON);
     * termios_p->c_oflag &= ~OPOST;
     * termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
     * termios_p->c_cflag &= ~(CSIZE | PARENB);
     * termios_p->c_cflag |= CS8;
     */

    // tio->c_oflag = CR3 | NL1 | VT1;
    
    if (!(m & IoDevice::Binary)) {
      tio->c_iflag |= ICRNL;
      tio->c_oflag |= ONLCR;
    }
    
    tio->c_cc[VTIME] = 0;
    tio->c_cc[VMIN] = 0;

    tio->c_cflag |= CLOCAL;
    if (m & ReadOnly) {
      tio->c_cflag |= CREAD;
    }
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::setTioDatabits (termios *tio, DataBits databits) {

    tio->c_cflag &= ~CSIZE;
    switch (databits) {
      case DataBits::Data5:
        tio->c_cflag |= CS5;
        break;
      case DataBits::Data6:
        tio->c_cflag |= CS6;
        break;
      case DataBits::Data7:
        tio->c_cflag |= CS7;
        break;
      case DataBits::Data8:
        tio->c_cflag |= CS8;
        break;
      default:
        tio->c_cflag |= CS8;
        break;
    }
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::setTioParity (termios *tio, Parity parity) {
    tio->c_iflag &= ~ (PARMRK | INPCK);
    tio->c_iflag |= IGNPAR;

    switch (parity) {

#ifdef CMSPAR
        // Here Installation parity only for GNU/Linux where the macro CMSPAR.
      case Parity::SpaceParity:
        tio->c_cflag &= ~PARODD;
        tio->c_cflag |= PARENB | CMSPAR;
        break;
      case Parity::MarkParity:
        tio->c_cflag |= PARENB | CMSPAR | PARODD;
        break;
#endif
      case Parity::NoParity:
        tio->c_cflag &= ~PARENB;
        break;
      case Parity::EvenParity:
        tio->c_cflag &= ~PARODD;
        tio->c_cflag |= PARENB;
        break;
      case Parity::OddParity:
        tio->c_cflag |= PARENB | PARODD;
        break;
      default:
        tio->c_cflag |= PARENB;
        tio->c_iflag |= PARMRK | INPCK;
        tio->c_iflag &= ~IGNPAR;
        break;
    }
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::setTioStopbits (termios *tio, StopBits stopbits) {

    switch (stopbits) {
      case StopBits::OneStop:
        tio->c_cflag &= ~CSTOPB;
        break;
      case StopBits::TwoStop:
        tio->c_cflag |= CSTOPB;
        break;
      default:
        tio->c_cflag &= ~CSTOPB;
        break;
    }
  }

  // ---------------------------------------------------------------------------
  void SerialPort::Private::setTioFlowcontrol (termios *tio, FlowControl flowcontrol) {

    switch (flowcontrol) {
      case FlowControl::NoFlowControl:
        tio->c_cflag &= ~CRTSCTS;
        tio->c_iflag &= ~ (IXON | IXOFF | IXANY);
        break;
      case FlowControl::HardwareControl:
        tio->c_cflag |= CRTSCTS;
        tio->c_iflag &= ~ (IXON | IXOFF | IXANY);
        break;
      case FlowControl::SoftwareControl:
        tio->c_cflag &= ~CRTSCTS;
        tio->c_iflag |= IXON | IXOFF | IXANY;
        break;
      default:
        tio->c_cflag &= ~CRTSCTS;
        tio->c_iflag &= ~ (IXON | IXOFF | IXANY);
        break;
    }
  }

  // ---------------------------------------------------------------------------
  typedef std::map<int32_t, int32_t> BaudRateMap;
  static const BaudRateMap createStandardBaudRateMap() {
    BaudRateMap baudRateMap;

#ifdef B50
    baudRateMap.insert (std::pair<int32_t, int32_t> (50, B50));
#endif

#ifdef B75
    baudRateMap.insert (std::pair<int32_t, int32_t> (75, B75));
#endif

#ifdef B110
    baudRateMap.insert (std::pair<int32_t, int32_t> (110, B110));
#endif

#ifdef B134
    baudRateMap.insert (std::pair<int32_t, int32_t> (134, B134));
#endif

#ifdef B150
    baudRateMap.insert (std::pair<int32_t, int32_t> (150, B150));
#endif

#ifdef B200
    baudRateMap.insert (std::pair<int32_t, int32_t> (200, B200));
#endif

#ifdef B300
    baudRateMap.insert (std::pair<int32_t, int32_t> (300, B300));
#endif

#ifdef B600
    baudRateMap.insert (std::pair<int32_t, int32_t> (600, B600));
#endif

#ifdef B1200
    baudRateMap.insert (std::pair<int32_t, int32_t> (1200, B1200));
#endif

#ifdef B1800
    baudRateMap.insert (std::pair<int32_t, int32_t> (1800, B1800));
#endif

#ifdef B2400
    baudRateMap.insert (std::pair<int32_t, int32_t> (2400, B2400));
#endif

#ifdef B4800
    baudRateMap.insert (std::pair<int32_t, int32_t> (4800, B4800));
#endif

#ifdef B7200
    baudRateMap.insert (std::pair<int32_t, int32_t> (7200, B7200));
#endif

#ifdef B9600
    baudRateMap.insert (std::pair<int32_t, int32_t> (9600, B9600));
#endif

#ifdef B14400
    baudRateMap.insert (std::pair<int32_t, int32_t> (14400, B14400));
#endif

#ifdef B19200
    baudRateMap.insert (std::pair<int32_t, int32_t> (19200, B19200));
#endif

#ifdef B28800
    baudRateMap.insert (std::pair<int32_t, int32_t> (28800, B28800));
#endif

#ifdef B38400
    baudRateMap.insert (std::pair<int32_t, int32_t> (38400, B38400));
#endif

#ifdef B57600
    baudRateMap.insert (std::pair<int32_t, int32_t> (57600, B57600));
#endif

#ifdef B76800
    baudRateMap.insert (std::pair<int32_t, int32_t> (76800, B76800));
#endif

#ifdef B115200
    baudRateMap.insert (std::pair<int32_t, int32_t> (115200, B115200));
#endif

#ifdef B230400
    baudRateMap.insert (std::pair<int32_t, int32_t> (230400, B230400));
#endif

#ifdef B460800
    baudRateMap.insert (std::pair<int32_t, int32_t> (460800, B460800));
#endif

#ifdef B500000
    baudRateMap.insert (std::pair<int32_t, int32_t> (500000, B500000));
#endif

#ifdef B576000
    baudRateMap.insert (std::pair<int32_t, int32_t> (576000, B576000));
#endif

#ifdef B921600
    baudRateMap.insert (std::pair<int32_t, int32_t> (921600, B921600));
#endif

#ifdef B1000000
    baudRateMap.insert (std::pair<int32_t, int32_t> (1000000, B1000000));
#endif

#ifdef B1152000
    baudRateMap.insert (std::pair<int32_t, int32_t> (1152000, B1152000));
#endif

#ifdef B1500000
    baudRateMap.insert (std::pair<int32_t, int32_t> (1500000, B1500000));
#endif

#ifdef B2000000
    baudRateMap.insert (std::pair<int32_t, int32_t> (2000000, B2000000));
#endif

#ifdef B2500000
    baudRateMap.insert (std::pair<int32_t, int32_t> (2500000, B2500000));
#endif

#ifdef B3000000
    baudRateMap.insert (std::pair<int32_t, int32_t> (3000000, B3000000));
#endif

#ifdef B3500000
    baudRateMap.insert (std::pair<int32_t, int32_t> (3500000, B3500000));
#endif

#ifdef B4000000
    baudRateMap.insert (std::pair<int32_t, int32_t> (4000000, B4000000));
#endif

    return baudRateMap;
  }

  // ---------------------------------------------------------------------------
  static const BaudRateMap& standardBaudRateMap() {
    static const BaudRateMap baudRateMap = createStandardBaudRateMap();
    return baudRateMap;
  }

  // ---------------------------------------------------------------------------
  typedef std::deque<int32_t> BaudRateList;
  static const BaudRateList createStandardBaudRateList() {
    BaudRateList baudRateList;
    const BaudRateMap& baudRateMap = standardBaudRateMap();
    for (auto elem : baudRateMap) {
      baudRateList.push_back (elem.first);
    }
    return baudRateList;
  }

  // ---------------------------------------------------------------------------
  static const BaudRateList& standardBaudRateList() {
    static const BaudRateList baudRateList = createStandardBaudRateList();
    return baudRateList;
  }

  // ---------------------------------------------------------------------------
  int32_t SerialPort::Private::settingFromBaudRate (int32_t baudRate) {
    if (standardBaudRateMap().count (baudRate) == 0) {
      return 0;
    }
    return standardBaudRateMap().at (baudRate);
  }

  // ---------------------------------------------------------------------------
  std::deque<int32_t> SerialPort::Private::standardBaudRates() {
    return standardBaudRateList();
  }
}

/* ========================================================================== */
