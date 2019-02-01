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
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <libudev.h>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <piduino/gpio.h>
#include <piduino/database.h>
#include <piduino/system.h>
#include "i2cdev_p.h"
#include "config.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                         I2cDev::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  I2cDev::Private::Private (I2cDev * q) :
    IoDevice::Private (q), fd (-1), state (Idle), txbuf (I2C_BLOCK_MAX), rxbuf (I2C_BLOCK_MAX) {

    isSequential = true;
  }

  // ---------------------------------------------------------------------------
  I2cDev::Private::~Private()  {}

  // ---------------------------------------------------------------------------
  bool
  I2cDev::Private::transfer() {
    PIMP_Q (I2cDev);

    if (q->isOpen() && ! i2c_msgs.empty()) {
      int ret;
      struct i2c_rdwr_ioctl_data msgset;

      msgset.msgs = i2c_msgs.data();
      msgset.nmsgs = i2c_msgs.size();

      clearError();
      ret = ::ioctl (fd, I2C_RDWR, &msgset);
      if (ret < 0) {

        setError();
      }
      return error == 0;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::Private::flush() {

    i2c_msgs.clear();
    txbuf.clear();
  }

// -----------------------------------------------------------------------------
//
//                           I2cDev::Info Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  bool
  I2cDev::Info::setPath (const std::string & p) {

    for (int i = 0; i < MaxBuses; i++) {
      std::string bp = busPath (i);

      if (bp == p) {

        setId (i);
        return true;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  std::string
  I2cDev::Info::busPath (int idbus) {
    char path[256];

    ::snprintf (path, sizeof (path), db.board().family().i2cSysPath().c_str(), idbus);
    return std::string (path);
  }

  // ---------------------------------------------------------------------------
  std::deque<I2cDev::Info>
  I2cDev::Info::availableBuses () {
    std::deque<I2cDev::Info> buses;
    struct udev *udev;

    udev = udev_new();
    if (udev) {
      struct udev_enumerate * enumerate;
      struct udev_list_entry * devices, * dev_list_entry;
      struct udev_device * dev;

      enumerate = udev_enumerate_new (udev);
      udev_enumerate_add_match_subsystem (enumerate, "i2c-dev");
      udev_enumerate_scan_devices (enumerate);
      devices = udev_enumerate_get_list_entry (enumerate);

      udev_list_entry_foreach (dev_list_entry, devices) {

        dev = udev_device_new_from_syspath (
                udev, udev_list_entry_get_name (dev_list_entry));

        if (!dev) {
          break;
        }

        const char * path = udev_device_get_devnode (dev);
        if (path) {
          Info bus;
          if (bus.setPath (path)) {
            buses.push_back (bus);
          }
        }
      }
      udev_unref (udev);
    }

    return buses;
  }

  // ---------------------------------------------------------------------------
  I2cDev::Info
  I2cDev::Info::defaultBus () {

    return Info (db.board().defaultI2cBus());
  }

// -----------------------------------------------------------------------------
//
//                             I2cDev Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev (I2cDev::Private &dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev () :
    IoDevice (*new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev (const char * path) : I2cDev() {

    setBusPath (path);
  }

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev (const std::string & path) : I2cDev() {

    setBusPath (path);
  }

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev (const Info & bus) : I2cDev() {

    setBus (bus);
  }

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev (int id) : I2cDev() {

    setBus (id);
  }

  // ---------------------------------------------------------------------------
  I2cDev::~I2cDev() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool
  I2cDev::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (I2cDev);
      unsigned long i2c_funcs;

      d->flush();
      d->rxbuf.clear();

      d->fd = ::open (d->bus.path().c_str(), d->modeToPosixFlags (mode));
      if (d->fd < 0) {

        d->setError();
        return false;
      }

      if (IoDevice::open (mode)) {

        if (::ioctl (d->fd, I2C_FUNCS, &i2c_funcs) < 0) {

          d->setError();
          close();
        }
        else if (! (i2c_funcs & I2C_FUNC_I2C)) {

          d->setError (EOPNOTSUPP);
          close();
        }
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::close() {

    if (isOpen()) {
      PIMP_D (I2cDev);

      if (::close (d->fd)) {

        d->setError();
      }
      d->fd = -1;
      d->state = Private::Idle;
      IoDevice::close();
    }
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::setBus (const Info & bus) {
    PIMP_D (I2cDev);

    if (d->bus != bus) {

      d->bus = bus;

      if (isOpen()) {
        OpenMode m = openMode();

        close();
        open (m);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::setBus (int id) {
    PIMP_D (I2cDev);

    if (d->bus.id() != id) {

      d->bus.setId (id);
      if (isOpen()) {
        OpenMode m = openMode();

        close();
        open (m);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::setBusPath (const std::string & path) {
    PIMP_D (I2cDev);

    if (d->bus.path() != path) {

      d->bus.setPath (path);
      if (isOpen()) {
        OpenMode m = openMode();

        close();
        open (m);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::setBusPath (const char * path) {
    PIMP_D (I2cDev);

    setBusPath (std::string (path));
  }

  // ---------------------------------------------------------------------------
  const I2cDev::Info &
  I2cDev::bus() const {
    PIMP_D (const I2cDev);

    return d->bus;
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::beginTransmission (uint16_t slave) {

    if (isOpen()) {
      PIMP_D (I2cDev);

      if (d->state == Private::Idle) {
        struct i2c_msg msg;

        msg.addr = slave;
        msg.flags = 0;
        msg.len = 0;
        msg.buf = d->txbuf.data();
        d->i2c_msgs.push_back (msg);
        d->state = Private::Write;
      }
      else {
        struct i2c_msg & msg = d->i2c_msgs.back();

        msg.addr = slave;
      }
    }
  }

  // ---------------------------------------------------------------------------
  int
  I2cDev::write (const uint8_t * buffer, uint16_t len) {
    PIMP_D (I2cDev);

    if (d->state == Private::Write) {
      struct i2c_msg & msg = d->i2c_msgs.back();
      int ret = d->txbuf.push (buffer, len);

      msg.len = d->txbuf.length();
      return ret;
    }
    return -1;
  }


  // ---------------------------------------------------------------------------
  int
  I2cDev::write (uint8_t data) {
    PIMP_D (I2cDev);

    if (d->state == Private::Write) {
      struct i2c_msg & msg = d->i2c_msgs.back();
      int ret = d->txbuf.push (data);

      msg.len = d->txbuf.length();
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  bool
  I2cDev::endTransmission (bool stop) {
    PIMP_D (I2cDev);

    if (d->state == Private::Write) {

      d->state = Private::Idle;
      if (stop) {

        bool success = d->transfer();
        d->flush();
        return success;
      }
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  int
  I2cDev::requestFrom (uint16_t slave, uint16_t max, bool stop) {

    if (isOpen()) {
      PIMP_D (I2cDev);

      if (d->state == Private::Idle) {
        struct i2c_msg msg;

        max = std::min (max, static_cast<uint16_t> (I2C_BLOCK_MAX));
        msg.buf = d->rxbuf.data();
        msg.flags = I2C_M_RD;
        msg.addr = slave;
        msg.len = max;
        d->i2c_msgs.push_back (msg);

        if (stop) {

          d->rxbuf.clear();
          if (!d->transfer()) {

            return -1;
          }

          d->rxbuf.seek (max);
          d->flush();
          return available();
        }
        return 0;
      }

    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  uint16_t
  I2cDev::available() const {
    PIMP_D (const I2cDev);

    return d->rxbuf.length();
  }

  // ---------------------------------------------------------------------------
  int
  I2cDev::read (uint8_t * buffer, uint16_t max) {
    PIMP_D (I2cDev);

    max = std::min (available(), max);
    return d->rxbuf.pull (buffer, max);
  }

  // ---------------------------------------------------------------------------
  int
  I2cDev::read() {
    PIMP_D (I2cDev);

    return d->rxbuf.pull();
  }

  // ---------------------------------------------------------------------------
  int
  I2cDev::peek() const {

    if (available()) {
      PIMP_D (const I2cDev);

      return *d->rxbuf.out();
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::flush() {
    PIMP_D (I2cDev);

    if (d->state == Private::Write) {

      endTransmission (true);
    }
    else {

      d->flush();
    }
  }
}

/* ========================================================================== */
