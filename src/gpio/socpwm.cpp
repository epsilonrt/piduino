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
#include <cmath>
#include <piduino/database.h>
#include "socpwm_p.h"
#include "arch/arm/broadcom/pwm_bcm2835.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                             SocPwm Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SocPwm::SocPwm (SocPwm::Private &dd) : Pwm (dd) {}

  // ---------------------------------------------------------------------------
  SocPwm::SocPwm (Pin * p) :
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

      return d->engine->setEnable (enable);
    }
  }

  // ---------------------------------------------------------------------------
  // hasPin() must be checked !
  bool
  SocPwm::isEnabled () const {

    if (isOpen() && hasPin()) {
      PIMP_D (const SocPwm);

      return d->engine->isEnabled();
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
    return -1;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::setFrequency (long freq) {
    PIMP_D (SocPwm);

    if (isOpen()) {

      if (d->engine->setFrequency (freq)) {

        if (hasPin()) {
          // TODO, modify value ?
        }
        return frequency();
      }
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
    return -1;
  }

  // ---------------------------------------------------------------------------
  int
  SocPwm::setResolution (int r)  {
    PIMP_D (SocPwm);

    if (isOpen()) {

      if (d->engine->setResolution (r)) {

        if (hasPin()) {
          // TODO, modify value ?
        }
        return resolution();
      }
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
  SocPwm::Private::Private (SocPwm * q, Pin * p) :
    Pwm::Private (q) {

    switch (db.board().soc().family().id()) {

      case SoC::Family::Id::BroadcomBcm2835:
        engine = std::make_unique<Bcm2835::PwmEngine> (this, p);
        break;

      case SoC::Family::Id::AllwinnerH:
        // TODO
        // engine = std::make_unique<AllwinnerH::PwmEngine> (this, p);
        break;
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
    return -1;
  }

  // ---------------------------------------------------------------------------
  // isOpen() == true
  // hasPin() must be checked !
  bool
  SocPwm::Private::write (long value) {

    if (hasPin()) {

      return engine->write (value);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::Private::max() const {

    if (isOpen()) {

      return engine->max();
    }
    return LONG_MIN;
  }

  // ---------------------------------------------------------------------------
  long
  SocPwm::Private::min() const {

    if (isOpen()) {

      return engine->min();
    }
    return LONG_MAX;
  }

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
