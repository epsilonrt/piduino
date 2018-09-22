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

#ifndef PIDUINO_PWM_PRIVATE_H
#define PIDUINO_PWM_PRIVATE_H

#include <piduino/pwm.h>
#include "converter_p.h"

namespace Piduino {

  /**
   * @class Pwm::Private
   * @brief
   */
  class Pwm::Private  : public Converter::Private {

    public:
      Private (Pwm * q, unsigned int resolution = 10);
      virtual ~Private();

      PIMP_DECLARE_PUBLIC (Pwm)
  };
}

/* ========================================================================== */
#endif /* PIDUINO_PWM_PRIVATE_H defined */
