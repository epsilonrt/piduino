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
#include <cmath>
#include <piduino/database.h>
#include "socpwm_p.h"
#include "arch/arm/broadcom/pwm_bcm2835.h"
#include "arch/arm/broadcom/pwm_rp1.h"
#include "arch/arm/allwinner/pwm_hx.h"
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                             SocPwm Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SocPwm::SocPwm (SocPwm::Private &dd) : Pwm (dd) {}

  // ---------------------------------------------------------------------------
  SocPwm::SocPwm (Pin *p) :
    Pwm (*new Private (this, p)) {}

  // ---------------------------------------------------------------------------
  SocPwm::~SocPwm() = default;

  // ---------------------------------------------------------------------------
  bool
  SocPwm::hasEngine() const {
    PIMP_D (const SocPwm);

    return d->hasEngine();
  }

  // ---------------------------------------------------------------------------
  bool
  SocPwm::hasPin() const {
    PIMP_D (const SocPwm);

    return d->hasPin();
  }

  // ---------------------------------------------------------------------------
  const Pin *
  SocPwm::pin() const {

    if (hasEngine()) {
      PIMP_D (const SocPwm);

      return d->engine->pin;
    }
    return nullptr;
  }

  // ---------------------------------------------------------------------------
  // hasPin() must be checked !
  void
  SocPwm::setEnable (bool enable) {

    if (isOpen() && hasPin()) {
      PIMP_D (SocPwm);

      d->engine->setEnable (enable);
      if (d->isDebug) {
        std::cout << "SocPwm::setEnable(" << enable << ") on pin "
                  << d->engine->pin->logicalNumber() << std::endl;
      }
    }
  }

  // ---------------------------------------------------------------------------
  // hasPin() must be checked !
  bool
  SocPwm::isEnabled () const {
    PIMP_D (const SocPwm);

    if (isOpen() && hasPin()) {

      return d->engine->isEnabled();
    }

    if (d->isDebug) {
      std::cerr << "SocPwm::isEnabled() called without an open engine or pin." << std::endl;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::frequency() const {
    PIMP_D (const SocPwm);

    if (isOpen()) {

      return d->engine->frequency();
    }
    if (d->isDebug) {
      std::cerr << "SocPwm::frequency() called without an open engine." << std::endl;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::setFrequency (long freq) {
    PIMP_D (SocPwm);

    if (isOpen()) {

      if (d->engine->setFrequency (freq)) {

        if (d->isDebug) {
          std::cout << "SocPwm::setFrequency(" << freq << ") called, new frequency: " << d->engine->frequency() << "Hz" << std::endl;
          std::cout << " Note: modification of frequency implicitly affects all PWM outputs, set range on each output if necessary!" << std::endl;
        }
        return frequency();
      }
    }
    if (d->isDebug) {
      std::cerr << "SocPwm::setFrequency(" << freq << ") called without an open engine." << std::endl;
    }
    return -1;
  }


  // ---------------------------------------------------------------------------
  long
  SocPwm::range() const {
    PIMP_D (const SocPwm);

    if (isOpen()) {

      return d->engine->range();
    }
    if (d->isDebug) {
      std::cerr << "SocPwm::range() called without an open engine." << std::endl;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::setRange (long rg) {
    PIMP_D (SocPwm);

    if (isOpen()) {
      long r = d->engine->range();
      // long f = d->engine->frequency();
      long v = d->engine->read();

      if (d->engine->setRange (rg)) {
        long nr = d->engine->range();
        long nv = (v * nr) / r; // scale value to new range

        if (nv > nr || v > r) {
          nv = nr + 1; // ensure value does not exceed new range
        }
        // d->engine->setFrequency (f);
        if (!d->engine->write (nv)) {
          if (d->isDebug) {
            std::cerr << "SocPwm::setRange(" << rg << ") failed to write value: " << nv << std::endl;
          }
          return -1; // return error if write fails
        }
        if (d->isDebug) {
          std::cout << "SocPwm::setRange(" << rg << ") called, new range: " << nr
                    << ", scaled value: " << nv << std::endl;
          std::cout << " Note: modification of range implicitly affects the frequency, new frequency: " << d->engine->frequency() << "Hz, may be modified if necessary!" << std::endl;
        }
        return nr;
      }
    }
    if (d->isDebug) {
      std::cerr << "SocPwm::setRange(" << rg << ") called without an open engine." << std::endl;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int
  SocPwm::resolution() const {
    PIMP_D (const SocPwm);

    if (isOpen()) {

      return d->engine->resolution();
    }
    if (d->isDebug) {
      std::cerr << "SocPwm::resolution() called without an open engine." << std::endl;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  int
  SocPwm::setResolution (int r)  {
    PIMP_D (SocPwm);

    if (isOpen()) {

      if (d->engine->setResolution (r)) {

        return resolution();
      }
      if (d->isDebug) {
        std::cerr << "SocPwm::setResolution(" << r << ") failed to set resolution." << std::endl;
      }
    }
    if (d->isDebug) {
      std::cerr << "SocPwm::setResolution(" << r << ") called without an open engine." << std::endl;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  const std::string &
  SocPwm::deviceName() const {
    static std::string dn;

    if (hasEngine()) {
      PIMP_D (const SocPwm);

      dn = d->engine->deviceName ();
    }
    return dn;
  }

  // ---------------------------------------------------------------------------
  //
  //                     SocPwm::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SocPwm::Private::Private (SocPwm *q, Pin *p) :
    Pwm::Private (q) {

    switch (db.board().soc().id()) {
        #if PIDUINO_DRIVER_BCM2835 != 0
      case SoC::Bcm2708 :
      case SoC::Bcm2709 :
      case SoC::Bcm2710 :
      case SoC::Bcm2711 :
        engine = std::make_unique<Bcm2835::PwmEngine> (this, p);
        break;

      case SoC::Bcm2712 :
        engine = std::make_unique<Rp1::PwmEngine> (this, p);
        break;
        #endif /* PIDUINO_DRIVER_BCM2835 */

        #if PIDUINO_DRIVER_ALLWINNERH != 0
      case SoC::H3 :
      case SoC::H5 :
        engine = std::make_unique<AllWinnerHx::PwmEngine> (this, p);
        break;
        #endif /* PIDUINO_DRIVER_ALLWINNERH */

      default:
        break;
    }
  }

  // ---------------------------------------------------------------------------
  SocPwm::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  bool
  SocPwm::Private::hasEngine() const {

    return engine != nullptr;
  }

  // ---------------------------------------------------------------------------
  bool
  SocPwm::Private::hasPin() const {

    if (hasEngine()) {
      return engine->pin != nullptr;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool
  SocPwm::Private::isOpen() const {

    return hasEngine() && Pwm::Private::isOpen();
  }

  // ---------------------------------------------------------------------------
  // isOpen() == false
  bool
  SocPwm::Private::open (OpenMode m) {

    if (hasEngine()) {

      if (engine->open (m)) {

        return Pwm::Private::open (m);
      }
    }
    if (isDebug) {
      std::cerr << "SocPwm::open() called without an engine or failed to open engine." << std::endl;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // isOpen() == true
  void
  SocPwm::Private::close() {

    if (hasEngine()) {

      engine->close();
      Pwm::Private::close();
    }
  }

  // ---------------------------------------------------------------------------
  // isOpen() == true
  // hasPin() must be checked !
  long
  SocPwm::Private::read() {

    if (hasPin()) {

      return engine->read();
    }
    if (isDebug) {
      std::cerr << "SocPwm::read() called without an open engine or pin." << std::endl;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  // isOpen() == true
  // hasPin() must be checked !
  // called by Converter::write(), value is modified to fit the range
  bool
  SocPwm::Private::write (long value) {

    if (hasPin()) {

      bool ret = engine->write (value);
      if (isDebug) {
        if (ret) {
          std::cout << "SocPwm::write(" << value << ") called on pin "
                    << engine->pin->logicalNumber() << std::endl;
        } else {
          std::cerr << "SocPwm::write(" << value << ") failed on pin "
                    << engine->pin->logicalNumber() << std::endl;
        }
      }
      return ret;
    }
    if (isDebug) {
      std::cerr << "SocPwm::write(" << value << ") called without an open engine or pin." << std::endl;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::Private::max() const {

    if (isOpen()) {

      return engine->max();
    }
    if (isDebug) {
      std::cerr << "SocPwm::max() called without an open engine." << std::endl;
    }
    return LONG_MIN;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::Private::min() const {

    if (isOpen()) {

      return engine->min();
    }
    if (isDebug) {
      std::cerr << "SocPwm::min() called without an open engine." << std::endl;
    }
    return LONG_MAX;
  }

  // ---------------------------------------------------------------------------
  //
  //                     SocPwm::Engine Class
  //
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  // default implementation
  long
  SocPwm::Engine::max() const {

    return (1L << (resolution() - (parent->bipolar ? 1 : 0))) - 1;
  }

  // ---------------------------------------------------------------------------
  // default implementation
  long
  SocPwm::Engine::min() const {

    return parent->bipolar ? - (1L << (resolution() - 1)) : 0;
  }

}
/* ========================================================================== */
