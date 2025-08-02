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
  SocPwm::SocPwm (SocPwm::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  SocPwm::SocPwm (Pin *p) :
    Converter (*Private::engineFactory (this, p)) {}

  // ---------------------------------------------------------------------------
  SocPwm::~SocPwm() = default;

  // ---------------------------------------------------------------------------
  const Pin *
  SocPwm::pin() const {
    PIMP_D (const SocPwm);

    return d->pin;
  }

  // ---------------------------------------------------------------------------
  //
  //                     SocPwm::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SocPwm::Private::Private (SocPwm *q, Pin *p, unsigned int flags) :
    Converter::Private (q, DigitalToAnalog, flags), pin (p) {}

  // ---------------------------------------------------------------------------
  SocPwm::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // Factory method to create the appropriate engine based on the SoC type
  // This method checks the SoC ID and returns the corresponding engine instance.
  // static
  SocPwm::Private *
  SocPwm::Private::engineFactory (SocPwm *q, Pin *pin) {

    switch (db.board().soc().id()) {
        #if PIDUINO_DRIVER_BCM2835 != 0
      case SoC::Bcm2708 :
      case SoC::Bcm2709 :
      case SoC::Bcm2710 :
      case SoC::Bcm2711 :
        return new Bcm2835Engine (q, pin);
        break;

      case SoC::Bcm2712 :
        return new Rp1Engine (q, pin);
        break;
        #endif /* PIDUINO_DRIVER_BCM2835 */

        #if PIDUINO_DRIVER_ALLWINNERH != 0
      case SoC::H3 :
      case SoC::H5 :
        return new HxEngine (q, pin);
        break;
        #endif /* PIDUINO_DRIVER_ALLWINNERH */

      default:
        throw std::runtime_error (EXCEPTION_MSG ("SocPwm::Private::engineFactory(): unsupported SoC for PWM engine"));
        break;
    }
    return nullptr; // This line is never reached, but added to avoid compiler warnings.
  }
}
/* ========================================================================== */
