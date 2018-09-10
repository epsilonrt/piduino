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
#ifndef PIDUINO_SERIALPORTINFO_PRIVATE_H
#define PIDUINO_SERIALPORTINFO_PRIVATE_H

#include <string>

namespace Piduino {


  class SerialPort::Info::Private {

    public:
      Private ();
      static bool isValidSerialPort (const char * path);
      static bool isValid8250Device (const char * path);
      static uint16_t identifier (const char * value, bool & hasIdentifier);
      static std::string portIdToSystemLocation (int portId);

      std::string path;
      std::string name;
      std::string driver;
      std::string description;
      std::string manufacturer;
      std::string serialNumber;
      uint16_t vendorIdentifier;
      uint16_t productIdentifier;
      bool hasVendorIdentifier;
      bool hasProductIdentifier;
  };

}
/* ========================================================================== */
#endif /* PIDUINO_SERIALPORTINFO_PRIVATE_H defined */
