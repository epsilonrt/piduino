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

#include <piduino/gpiodevice.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                        Device Class
//
// -----------------------------------------------------------------------------
  Device::Device() : _isopen (false), _isdebug (false) {

  }

// -----------------------------------------------------------------------------
  Device::~Device() {

  }

// -----------------------------------------------------------------------------
  bool
  Device::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  void
  Device::setOpen (bool open) {
    _isopen = open;
  }

// -----------------------------------------------------------------------------
  bool
  Device::isDebug() const {
    return _isdebug;
  }

// -----------------------------------------------------------------------------
  void
  Device::setDebug (bool enable) {
    _isdebug = enable;
  }

// -----------------------------------------------------------------------------
  void
  Device::toggle (const Pin * pin) {
  }

// -----------------------------------------------------------------------------
  Pin::Pull
  Device::pull (const Pin * pin) const {
    return Pin::PullUnknown;
  }

// -----------------------------------------------------------------------------
  unsigned int
  Device::flags() const {
    return 0;
  }
// -----------------------------------------------------------------------------
}
/* ========================================================================== */
