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
#include "socpwm_p.h"
#include <piduino/database.h>

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
    Pwm (*new Private (this, p)) {

    switch (db.board().soc().family().id()) {

      case SoC::Family::Id::BroadcomBcm2835:
        // TODO
        /*
        if (Bcm2835Pwm::isPwmPin (p)) {
          d_ptr.swap (new Bcm2835Pwm (this, p));
        }
         */
        break;
      case SoC::Family::Id::AllwinnerH:
        // TODO
        /*
        if (AllwinnerHPwm::isPwmPin (p)) {
          d_ptr.swap (new AllwinnerHPwm (this, p));
        }
         */
        break;
      default:
        break;
    }
  }

  // ---------------------------------------------------------------------------
  SocPwm::~SocPwm() = default;

  // ---------------------------------------------------------------------------
  // virtual
  long
  SocPwm::frequency() const {
    PIMP_D (const SocPwm);

    return d->frequency();
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool
  SocPwm::setFrequency (long freq) {
    PIMP_D (SocPwm);

    return d->setFrequency (freq);
  }

  // ---------------------------------------------------------------------------
  // virtual
  const std::string & SocPwm::deviceName() const {
    PIMP_D (const SocPwm);

    return d->deviceName();
  }

  // ---------------------------------------------------------------------------
  const Pin *
  SocPwm::pin() const {
    PIMP_D (const SocPwm);

    return d->pin;
  }

  // ---------------------------------------------------------------------------
  bool
  SocPwm::isNull() const {
    PIMP_D (const SocPwm);

    return d->isNull();
  }

  // ---------------------------------------------------------------------------
  bool
  SocPwm::isPwmPin (const Pin * pin) const {
    PIMP_D (const SocPwm);

    return d->isPwmPin (pin);
  }

// -----------------------------------------------------------------------------
//
//                     SocPwm::Private Default Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SocPwm::Private::Private (SocPwm * q, Pin * p) :
    Pwm::Private (q), pin (p) {}

  // ---------------------------------------------------------------------------
  SocPwm::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // virtual
  bool
  SocPwm::Private::isOpen() const {

    return !isNull() && Pwm::Private::isOpen();
  }

  // ---------------------------------------------------------------------------
  // static
  const std::string &
  SocPwm::Private::deviceName() {
    static std::string dn;

    return dn;
  }
}

/* ========================================================================== */
