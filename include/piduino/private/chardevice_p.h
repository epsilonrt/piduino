/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * CharDevice is a modified and simplified version of QIODevice, 
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

#ifndef PIDUINO_CHARDEVICE_PRIVATE_H
#define PIDUINO_CHARDEVICE_PRIVATE_H

#include <algorithm>
#include <cstring>
#include <piduino/chardevice.h>
#include <piduino/linearbuffer.h>
#include "iodevice_p.h"


namespace Piduino {

  class CharDevice::Private : public IoDevice::Private {

    public:

      Private (CharDevice * q);
      virtual ~Private();

      inline void incrementPos (long offset) {
        pos += isSequential ? 0 : offset;
      }

      inline void incrementDevicePos (long offset) {
        devicePos += isSequential ? 0 : offset;
      }

      LinearBuffer buffer;
      bool firstRead;
      long pos;
      long devicePos;
      bool baseReadLineDataCalled;

      PIMP_DECLARE_PUBLIC(CharDevice)
  };
}
/* ========================================================================== */
#endif /* PIDUINO_CHARDEVICE_PRIVATE_H defined */
