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
#include "pwm_p.h"

namespace Piduino {

  /**
   * @class SocPwm::Private
   * @brief
   */
  class SocPwm::Private  : public Pwm::Private {

    public:
      // SocPwm::Private
      Private (SocPwm * q, Pin * pin);
      virtual ~Private();
      bool hasEngine() const;
      bool hasPin() const;

      // IoDevice::Private
      virtual bool isOpen() const;

      // Converter::Private
      virtual bool open (OpenMode mode);
      virtual void close();
      virtual long read();
      virtual bool write (long value);
      virtual long max() const;
      virtual long min() const;

      // SocPwm::Private
      std::unique_ptr<Engine> engine;

      PIMP_DECLARE_PUBLIC (SocPwm)
  };
}

/* ========================================================================== */
#endif /* PIDUINO_SOCPWM_PRIVATE_H defined */
