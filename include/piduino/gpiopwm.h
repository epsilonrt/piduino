/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#pragma once

#include <piduino/pwm.h>
#include <piduino/gpiopin.h>

/**
 *  @defgroup piduino_gpiopwm GPIO PWM generator
 *  @{
 */

namespace Piduino {

  class GpioPwm : public Pwm {

    public:
      GpioPwm (Pin * pin, unsigned int resolution = 10, unsigned long freq = 100);
      virtual ~GpioPwm();

      const Pin & pin() const;
      virtual long frequency() const;
      
      virtual const std::string & deviceName() const;

    protected:
      class Private;
      GpioPwm (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (GpioPwm)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
