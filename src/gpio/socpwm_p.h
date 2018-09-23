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

#ifndef PIDUINO_SOCPWM_PRIVATE_H
#define PIDUINO_SOCPWM_PRIVATE_H

#include <piduino/socpwm.h>
#include "../pwm_p.h"

namespace Piduino {

  /**
   * @class SocPwm::Private
   * @brief
   */
  class SocPwm::Private  : public Pwm::Private {

    public:
      Private (SocPwm * q, Pin * pin);
      virtual ~Private();
      
      // IoDevice::Private
      virtual bool isOpen() const;

      // Converter::Private
      virtual bool open (OpenMode mode) { return false; }
      virtual void close() {}
      // virtual long read();
      // virtual bool write (long value);

      // SocPwm::Private
      virtual bool isNull() const { return true; }
      virtual long frequency() const { return 0; }
      virtual bool setFrequency (long freq) const { return false; }

      static bool isPwmPin (const Pin * pin) { return false; }
      static const std::string & deviceName();

      Pin * pin;

      PIMP_DECLARE_PUBLIC (SocPwm)
  };
}

/* ========================================================================== */
#endif /* PIDUINO_SOCPWM_PRIVATE_H defined */
