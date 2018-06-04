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

#ifndef PIDUINO_PWM_H
#define PIDUINO_PWM_H

#include <piduino/converter.h>

/**
 *  @defgroup piduino_pwm PWM generator
 *  @{
 */

namespace Piduino {

  class Pwm : public Converter {

    public:
      Pwm (unsigned int resolution);
      virtual ~Pwm();

      virtual void setClock (long clk) = 0;
      virtual long clock() const = 0;
      virtual long read() = 0;
      virtual void write (long value) = 0;

  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_PWM_H defined */
