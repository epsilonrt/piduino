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
#ifndef PIDUINO_SERIALPORT_PRIVATE_H
#define PIDUINO_SERIALPORT_PRIVATE_H

#include <termios.h>
#include <string>
#include <piduino/serialport.h>
#include "terminal_p.h"

namespace Piduino {

  class SerialPort::Private : public Terminal::Private {

    public:
      Private (SerialPort * q);
      virtual ~Private();

      virtual bool open (OpenMode mode, int additionalPosixFlags = 0);
      virtual void close();
      
      void tcflush (Directions directions);

      PinoutSignals pinoutSignals();

      bool setDataTerminalReady (bool set);
      bool setRequestToSend (bool set);

      bool setBaudRate (int32_t baudRate, Directions directions);
      bool setBaudRate();
      bool setDataBits ();
      bool setParity ();
      bool setStopBits ();
      bool setFlowControl ();

      bool sendBreak (int duration);
      bool setBreakEnabled (bool set);

      bool initialize (OpenMode mode);
      bool setStandardBaudRate (int32_t baudRate, Directions directions);
      bool setCustomBaudRate (int32_t baudRate, Directions directions);
      bool setTermios (const termios * tio);
      bool getTermios (termios * tio);
      
      static void setTioCommonProps (termios * tio, OpenMode m);
      static void setTioDatabits (termios * tio, DataBits databits);
      static void setTioParity (termios * tio, Parity parity);
      static void setTioStopbits (termios * tio, StopBits stopbits);
      static void setTioFlowcontrol (termios * tio, FlowControl flowcontrol);
      static int32_t settingFromBaudRate (int32_t baudRate);

      static std::string portNameToSystemLocation (const std::string & port);
      static std::string portNameFromSystemLocation (const std::string & location);
      static std::deque<int32_t> standardBaudRates();

      struct termios restoredTermios;
      bool settingsRestoredOnClose = true;
      bool isBreakEnabled = false;
      Settings settings;

      PIMP_DECLARE_PUBLIC (SerialPort)
  };

}
/* ========================================================================== */
#endif /* PIDUINO_SERIALPORT_PRIVATE_H defined */
