/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * I2cDev is a modified and simplified version of QIODevice,
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
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <linux/i2c.h>
#include <piduino/i2cdev.h>
#include <piduino/fifo.h>
#include "iodevice_p.h"

#ifndef I2C_BLOCK_MAX
#define I2C_BLOCK_MAX I2C_SMBUS_BLOCK_MAX
#endif

namespace Piduino {

  class I2cDev::Private : public IoDevice::Private {

    public:
      enum {
        Idle = 0,
        Write,
        Read
      };

      Private (I2cDev * q);
      virtual ~Private();
      bool transfer();
      void flush();

      int fd;
      int state;
      std::vector<i2c_msg> i2c_msgs;
      Fifo txbuf;
      Fifo rxbuf;
      Info bus;

      PIMP_DECLARE_PUBLIC (I2cDev)
  };
}
/* ========================================================================== */
