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
#if 0
#include <cstdio>
#include <stdlib.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <libudev.h>
#include <algorithm>
#include <piduino/gpio.h>
#include <piduino/database.h>
#include <piduino/serialport.h>
#include "serialportinfo_p.h"
#include "serialport_p.h"

namespace Piduino {


// -----------------------------------------------------------------------------
//
//                         SerialPort::Info Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SerialPort::Info::Private::Private () :
    hasVendorIdentifier (false), hasProductIdentifier (false) {}


  // ---------------------------------------------------------------------------
  bool SerialPort::Info::Private::isValid8250Device (const char * path) {
    mode_t flags = O_RDWR | O_NONBLOCK | O_NOCTTY;
    int fd = ::open (path, flags);

    if (fd != -1) {
      struct serial_struct serinfo;

      int retval = ::ioctl (fd, TIOCGSERIAL, &serinfo);
      ::close (fd);

      if (retval != -1 && serinfo.type != PORT_UNKNOWN) {

        return true;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Info::Private::isValidSerialPort (const char * path) {
    mode_t flags = O_RDWR | O_NONBLOCK | O_NOCTTY;
    int fd = ::open (path, flags);

    if (fd != -1) {
      termios tio;

      int retval = ::tcgetattr (fd, &tio);
      ::close (fd);
      if (retval != -1) {
        return true;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  uint16_t SerialPort::Info::Private::identifier (const char * value, bool & hasIdentifier) {
    int id = 0;
    std::string str (value);

    try {

      id = std::stoi (str, 0, 0);
      hasIdentifier = true;
    }
    catch (...) {

      hasIdentifier = false;
    }
    return id;
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::Private::portIdToSystemLocation (int portId) {
    char path[256];

    ::snprintf (path, sizeof (path), db.board().soc().uartSysPath().c_str(), portId);
    return std::string (path);
  }

  // ---------------------------------------------------------------------------
  SerialPort::Info::Info (SerialPort::Info::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  SerialPort::Info::Info ()  {

  }

  // ---------------------------------------------------------------------------
  SerialPort::Info::Info (const Info & other) :
    d_ptr (other.d_ptr ? new Private (*other.d_ptr) : nullptr) {

  }

  // ---------------------------------------------------------------------------
  SerialPort::Info::Info (const std::string & name)  {
    const auto ports = availablePorts();

    for (const Info & port : ports) {
      if (port.portName() == name) {

        *this = port;
        break;
      }
    }
  }

  // ---------------------------------------------------------------------------
  SerialPort::Info::Info (const SerialPort & port) : Info (port.portName()) {

  }

  // ---------------------------------------------------------------------------
  SerialPort::Info::~Info() = default;

  // ---------------------------------------------------------------------------
  void SerialPort::Info::swap (Info &other) {

    d_ptr.swap (other.d_ptr);
  }

  // ---------------------------------------------------------------------------
  SerialPort::Info& SerialPort::Info::operator= (const Info &other) {

    Info (other).swap (*this);
    return *this;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Info::operator== (const Info & other) {

    if (!isNull() && !other.isNull()) {

      return systemLocation() == other.systemLocation();
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::portName() const {
    PIMP_D (const Info);

    return !d ? std::string() : d->name;
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::systemLocation() const {
    PIMP_D (const Info);

    return !d ? std::string() : d->path;
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::driverName() const {
    PIMP_D (const Info);

    return !d ? std::string() : d->driver;
  }
  
  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::description() const {
    PIMP_D (const Info);

    return !d ? std::string() : d->description;
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::manufacturer() const {
    PIMP_D (const Info);

    return !d ? std::string() : d->manufacturer;
  }

  // ---------------------------------------------------------------------------
  std::string SerialPort::Info::serialNumber() const {
    PIMP_D (const Info);

    return !d ? std::string() : d->serialNumber;
  }

  // ---------------------------------------------------------------------------
  uint16_t SerialPort::Info::vendorIdentifier() const {
    PIMP_D (const Info);

    return !d ? 0 : d->vendorIdentifier;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Info::hasVendorIdentifier() const {
    PIMP_D (const Info);

    return !d ? false : d->hasVendorIdentifier;
  }

  // ---------------------------------------------------------------------------
  uint16_t SerialPort::Info::productIdentifier() const {
    PIMP_D (const Info);

    return !d ? 0 : d->productIdentifier;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Info::hasProductIdentifier() const {
    PIMP_D (const Info);

    return !d ? false : d->hasProductIdentifier;
  }

  // ---------------------------------------------------------------------------
  bool SerialPort::Info::isNull() const {

    return !d_ptr;
  }

  // ---------------------------------------------------------------------------
  std::deque<SerialPort::Info>
  SerialPort::Info::availablePorts () {
    std::deque<SerialPort::Info> ports;
    struct udev *udev;

    udev = udev_new();
    if (udev) {
      struct udev_enumerate * enumerate;
      struct udev_list_entry * devices, * dev_list_entry;
      struct udev_device * dev, * parent;

      enumerate = udev_enumerate_new (udev);
      udev_enumerate_add_match_subsystem (enumerate, "tty");
      udev_enumerate_scan_devices (enumerate);
      devices = udev_enumerate_get_list_entry (enumerate);

      udev_list_entry_foreach (dev_list_entry, devices) {

        dev = udev_device_new_from_syspath (
                udev, udev_list_entry_get_name (dev_list_entry));

        if (!dev) {
          break;
        }

        parent = udev_device_get_parent (dev);
        if (parent) {
          const char * path = udev_device_get_devnode (dev);

          if (Private::isValidSerialPort (path)) {
            Private * p = new Private();

            p->path.assign (path);
            p->name.assign (udev_device_get_sysname (dev));
            p->driver.assign (udev_device_get_driver (parent));

            if ( (p->driver.compare ("serial8250") == 0) && Private::isValid8250Device (path))  {

              p->description.assign (udev_device_get_property_value (dev, "ID_MODEL"));
              std::replace (p->description.begin(), p->description.end(), '_', ' ');

              p->manufacturer.assign (udev_device_get_property_value (dev, "ID_VENDOR"));
              std::replace (p->manufacturer.begin(), p->manufacturer.end(), '_', ' ');

              p->serialNumber.assign (udev_device_get_property_value (dev, "ID_SERIAL_SHORT"));
              p->vendorIdentifier = Private::identifier (udev_device_get_property_value (dev, "ID_VENDOR_ID"), p->hasVendorIdentifier);
              p->productIdentifier = Private::identifier (udev_device_get_property_value (dev, "ID_MODEL_ID"), p->hasProductIdentifier);
            }

            ports.push_back (*p);
          }
        }
      }
      udev_unref (udev);
    }

    return ports;
  }

  // ---------------------------------------------------------------------------
  SerialPort::Info SerialPort::Info::defaultPort () {
    std::string location = Private::portIdToSystemLocation (gpio.defaultUart());
    const auto ports = availablePorts();

    for (const Info & port : ports) {
      if (port.systemLocation() == location) {

        return port;
      }
    }
    return Info();
  }

  // ---------------------------------------------------------------------------
  std::deque<int32_t> SerialPort::Info::standardBaudRates() {
    
    return SerialPort::Private::standardBaudRates();
  }

}

/* ========================================================================== */
