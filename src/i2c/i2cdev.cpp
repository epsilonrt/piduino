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
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <piduino/gpio.h>
#include <piduino/database.h>
#include <piduino/system.h>
#include <piduino/private/i2cdev_p.h>

namespace Piduino {

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

      q->clearError();
      ret = ::ioctl (fd, I2C_RDWR, &msgset);
      if (ret < 0) {

        q->setError();
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

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev (I2cDev::Private &dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  I2cDev::I2cDev () :
    IoDevice (*new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  I2cDev::~I2cDev() {
  }

  // ---------------------------------------------------------------------------
  bool I2cDev::open (OpenMode mode) {
    
    return IoDevice::open (mode);
  }

  // ---------------------------------------------------------------------------
  bool
  I2cDev::open (const char * path) {


    if (!isOpen()) {
      unsigned long i2c_funcs;
      PIMP_D (I2cDev);

      d->fd = ::open (path, O_RDWR);
      if (d->fd < 0) {

        setError();
        return false;
      }

      if (::ioctl (d->fd, I2C_FUNCS, &i2c_funcs) < 0) {

        setError();
        return false;
      }

      open (OpenMode::ReadWrite);

      if (! (i2c_funcs & I2C_FUNC_I2C)) {

        setError (EOPNOTSUPP);
        close();
      }

      d->flush();
      d->rxbuf.clear();
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  bool
  I2cDev::open (const std::string & path) {

    return open (path.c_str());
  }

  // ---------------------------------------------------------------------------
  bool
  I2cDev::open (const Info & bus) {

    return open (bus.path);
  }

  // ---------------------------------------------------------------------------
  bool
  I2cDev::open (int idBus) {

    return open (busPath (idBus));
  }

  // ---------------------------------------------------------------------------
  void
  I2cDev::close() {

    if (isOpen()) {
      PIMP_D (I2cDev);

      if (::close (d->fd)) {

        setError();
      }
      d->fd = -1;
      d->state = Private::Idle;
      IoDevice::close();
    }
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
          struct i2c_msg & rxmsg = d->i2c_msgs.back();

          d->rxbuf.clear();
          if (!d->transfer()) {

            return -1;
          }

          d->rxbuf.seek (max);
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

  // ---------------------------------------------------------------------------
  std::string
  I2cDev::busPath (int idbus) {
    char path[256];

    ::snprintf (path, sizeof (path), db.board().soc().i2cSysPath().c_str(), idbus);
    return std::string (path);
  }

  // ---------------------------------------------------------------------------
  std::map<int, I2cDev::Info>
  I2cDev::availableBuses () {
    std::map<int, I2cDev::Info> buses;

    for (int id = 0; id < db.board().soc().i2cCount() ; id++) {
      std::string path = busPath (id);

      if (System::fileExist (path)) {
        Info bus;

        bus.id = id;
        bus.path = path;
        buses[id] = bus;
      }

    }
    return buses;
  }

  // ---------------------------------------------------------------------------
  I2cDev::Info
  I2cDev::defaultBus () {
    Info bus;

    bus.id = gpio.defaultI2cBus();
    bus.path = busPath (bus.id);
    return bus;
  }

}

/* ========================================================================== */
