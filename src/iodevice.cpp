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
#include <piduino/iodevice.h>

namespace Piduino {

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice() : _openMode (NotOpen) {

  }
  
  // ---------------------------------------------------------------------------
  IoDevice::~IoDevice() {

  }

  // ---------------------------------------------------------------------------
  IoDevice::OpenMode
  IoDevice::openMode() const {

    return _openMode;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setOpenMode (OpenMode m) {

    _openMode = m;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isOpen() const {

    return _openMode != NotOpen;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isReadable() const {

    return (openMode() & ReadOnly) != 0;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isWritable() const {

    return (openMode() & WriteOnly) != 0;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::open (OpenMode m) {

    setOpenMode (m);
    return true;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::close() {

    if (isOpen()) {
      _openMode = NotOpen;
    }
  }
}

/* ========================================================================== */
