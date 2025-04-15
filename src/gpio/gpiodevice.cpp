/* Copyright © 2018 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/

#include <piduino/gpiodevice.h>
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                        GpioDevice Class
  //
  // -----------------------------------------------------------------------------
  GpioDevice::GpioDevice() : _isopen (false), _isdebug (false) {

  }

  // -----------------------------------------------------------------------------
  GpioDevice::~GpioDevice() {

  }

  // -----------------------------------------------------------------------------
  bool
  GpioDevice::isOpen() const {

    return _isopen;
  }

  // -----------------------------------------------------------------------------
  void
  GpioDevice::setOpen (bool open) {
    _isopen = open;
  }

  // -----------------------------------------------------------------------------
  bool
  GpioDevice::isDebug() const {
    return _isdebug;
  }

  // -----------------------------------------------------------------------------
  void
  GpioDevice::setDebug (bool enable) {
    _isdebug = enable;
  }

  // -----------------------------------------------------------------------------
  void
  GpioDevice::toggle (const Pin *pin) {
  }

  // -----------------------------------------------------------------------------
  Pin::Pull
  GpioDevice::pull (const Pin *pin) const {
    return Pin::PullUnknown;
  }

  // -----------------------------------------------------------------------------
  unsigned int
  GpioDevice::flags() const {
    return 0;
  }

  // -----------------------------------------------------------------------------
  void
  GpioDevice::setDrive (const Pin *pin, int d) {
  }

  // -----------------------------------------------------------------------------
  int
  GpioDevice::drive (const Pin *pin) const {
    return -1;
  }

  // -----------------------------------------------------------------------------
  int
  GpioDevice::systemNumberOffset() const {
    return 0;
  }
  // -----------------------------------------------------------------------------
}
/* ========================================================================== */
