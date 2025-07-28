/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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

#include "gpiodevice_p.h"
#include "config.h"

namespace Piduino {
  // -----------------------------------------------------------------------------
  //
  //                        GpioDevice::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  GpioDevice::Private::Private (GpioDevice *q) :
    q_ptr (q), isopen (false), isdebug (false) {}

  // ---------------------------------------------------------------------------
  GpioDevice::Private::~Private()  {}

  // -----------------------------------------------------------------------------
  //
  //                        GpioDevice Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  GpioDevice::GpioDevice (GpioDevice::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  GpioDevice::GpioDevice () :
    d_ptr (new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  GpioDevice::~GpioDevice() = default;

  // -----------------------------------------------------------------------------
  bool
  GpioDevice::isOpen() const {
    PIMP_D (const GpioDevice);

    return d->isopen;
  }


  // -----------------------------------------------------------------------------
  bool
  GpioDevice::isDebug() const {
    PIMP_D (const GpioDevice);

    return d->isdebug;
  }

  // -----------------------------------------------------------------------------
  void
  GpioDevice::setDebug (bool enable) {
    PIMP_D (GpioDevice);

    d->isdebug = enable;
  }

  // -----------------------------------------------------------------------------
  unsigned int
  GpioDevice::flags() const {

    return 0;
  }

  // -----------------------------------------------------------------------------
  void
  GpioDevice::toggle (const Pin *pin) {

    write (pin, !read (pin));
  }

  // -----------------------------------------------------------------------------
  Pin::Pull
  GpioDevice::pull (const Pin *pin) const {

    return Pin::PullUnknown;
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
  int GpioDevice::waitForInterrupt (const Pin *pin, Pin::Edge edge, int timeout_ms) {
    return -1;
  }

  // -----------------------------------------------------------------------------
  void GpioDevice::setDebounce (const Pin *pin, uint32_t debounce_ms) {
  }

  // -----------------------------------------------------------------------------
  uint32_t GpioDevice::debounce (const Pin *pin) const {
    return 0;
  }

  // -----------------------------------------------------------------------------
  void GpioDevice::setActiveLow (const Pin *pin, bool activeLow) {
  }

  // -----------------------------------------------------------------------------
  bool GpioDevice::isActiveLow (const Pin *pin) const {
    return false;
  }

}
/* ========================================================================== */
