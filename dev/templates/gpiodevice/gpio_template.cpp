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
#include <iostream>
#include <iomanip>
#include <exception>
#include <piduino/clock.h>
#include <piduino/database.h>
#include "template.h"
#include "gpio_template_p.h"
#include "config.h"

namespace Piduino {

  using namespace Template;

  // -------------------------------------------------------------------------
  //
  //                        GpioTemplate Class
  //
  // -------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  GpioTemplate::GpioTemplate (GpioTemplate::Private &dd) : GpioDevice (dd) {}

  // -------------------------------------------------------------------------
  //
  //                         Mandatory API
  //
  // -------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  GpioTemplate::GpioTemplate () :
    GpioDevice (*new Private (this)) {
    PIMP_D (GpioTemplate);

    // place the code to initialize the device
  }

  // ---------------------------------------------------------------------------
  GpioTemplate::~GpioTemplate() = default;

  // -------------------------------------------------------------------------
  AccessLayer
  GpioTemplate::preferedAccessLayer() const {

    // replace this with the prefered access layer for your platform
    return AccessLayerUnk;
  }


  // -------------------------------------------------------------------------
  bool
  GpioTemplate::open() {

    if (!isOpen()) {
      PIMP_D (GpioTemplate);

      // place here the code to open the device


      d->isopen = true;
    }
    return isOpen();
  }

  // -------------------------------------------------------------------------
  void
  GpioTemplate::close() {

    if (isOpen()) {
      PIMP_D (GpioTemplate);

      // place here the code to close the device

      d->isopen = false;
    }
  }

  // -------------------------------------------------------------------------
  Pin::Mode
  GpioTemplate::mode (const Pin *pin) const {
    PIMP_D (const GpioTemplate);
    Pin::Mode m = Pin::ModeUnknown;

    // place here the code to read the mode of the pin
    return m;
  }

  // -------------------------------------------------------------------------
  void
  GpioTemplate::setMode (const Pin *pin, Pin::Mode m) {
    PIMP_D (GpioTemplate);

    // place here the code to set the mode of the pin
  }

  // -------------------------------------------------------------------------
  void
  GpioTemplate::setPull (const Pin *pin, Pin::Pull p) {
    PIMP_D (GpioTemplate);

    // place here the code to set the pull of the pin

  }

  // -------------------------------------------------------------------------
  void
  GpioTemplate::write (const Pin *pin, bool v) {
    PIMP_D (GpioTemplate);

    // place here the code to write the value to the pin
  }

  // -------------------------------------------------------------------------
  bool
  GpioTemplate::read (const Pin *pin) const {
    PIMP_D (const GpioTemplate);

    // place here the code to read the value of the pin
    return false;
  }

  // -------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  GpioTemplate::modes() const {

    return Private::modes;
  }

  // -------------------------------------------------------------------------
  //
  //                             Optional API
  //
  // -------------------------------------------------------------------------

  // -------------------------------------------------------------------------
  unsigned int
  GpioTemplate::flags() const {

    // Flags indicating the capabilities of the GPIO device.
    // replace this with the flags for your platform
    return  0;
  }

  // -----------------------------------------------------------------------------
  void
  GpioTemplate::toggle (const Pin *pin) {

    // place here the code to toggle the pin
  }

  // -----------------------------------------------------------------------------
  Pin::Pull
  GpioTemplate::pull (const Pin *pin) const {

    // place here the code to read the pull of the pin
    return Pin::PullUnknown;
  }

  // -----------------------------------------------------------------------------
  void
  GpioTemplate::setDrive (const Pin *pin, int d) {
    // place here the code to set the drive of the pin
  }

  // -----------------------------------------------------------------------------
  int
  GpioTemplate::drive (const Pin *pin) const {
    // place here the code to read the drive of the pin
    // return the drive value
    // if the device supports it
    // this function should also set the hasDrive flag
    return -1;
  }

  // -----------------------------------------------------------------------------
  void GpioTemplate::setDebounce (const Pin *pin, uint32_t debounce_ms) {
    // place here the code to set the debounce of the pin
    // this function should set the debounce period for the pin
    // if the device supports it
    // this function should also set the hasDebounce flag
  }

  // -----------------------------------------------------------------------------
  uint32_t GpioTemplate::debounce (const Pin *pin) const {
    // place here the code to read the debounce of the pin
    // this function should return the debounce period for the pin
    // if the device supports it
    // this function should also set the hasDebounce flag
    return 0;
  }

  // -----------------------------------------------------------------------------
  void GpioTemplate::setActiveLow (const Pin *pin, bool activeLow) {

    // place here the code to set the active low of the pin
    // this function should set the active low state for the pin
    // if the device supports it
    // this function should also set the hasActiveLow flag
  }

  // -----------------------------------------------------------------------------
  bool GpioTemplate::isActiveLow (const Pin *pin) const {

    // place here the code to read the active low of the pin
    // this function should return true if the pin is active low
    // if the device supports it
    // this function should also set the hasActiveLow flag
    return false;
  }

  // -----------------------------------------------------------------------------
  int GpioTemplate::waitForInterrupt (const Pin *pin, Pin::Edge edge, int timeout_ms) {
    PIMP_D (GpioTemplate);

    // place here the code to wait for an interrupt on the pin
    return -1;
  }


  // -----------------------------------------------------------------------------
  //
  //                         GpioTemplate::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  GpioTemplate::Private::Private (GpioTemplate *q) :
    GpioDevice::Private (q) {

    // place here the code to initialize the private data
  }

  // ---------------------------------------------------------------------------
  GpioTemplate::Private::~Private() = default;

  // -------------------------------------------------------------------------
  // static
  // change this to match the modes with your device.
  // const std::map<Pin::Mode, unsigned int> GpioTemplate::Private::mode2int = {
  //   {Pin::ModeInput, 0},
  //   {Pin::ModeOutput, 1},
  //   {Pin::ModeAlt0, 4},
  //   {Pin::ModeAlt1, 5},
  //   {Pin::ModeAlt2, 6},
  //   {Pin::ModeAlt3, 7},
  //   {Pin::ModeAlt4, 3},
  //   {Pin::ModeAlt5, 2},
  // };
}
/* ========================================================================== */
