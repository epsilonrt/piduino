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
#pragma once

#include <piduino/converter.h>
#include <piduino/gpiopin.h>

namespace Piduino {

  /**
     @class GpioPwm
     @brief Provides PWM (Pulse Width Modulation) control for a GPIO pin.

     This class allows configuration and control of PWM signals on a specified GPIO pin.
     It inherits from the Converter class and manages PWM parameters such as range and frequency.
     The GpioPwm class is designed to work with the Piduino library, providing a software-based PWM implementation
     for platforms that do not have hardware PWM support or when additional PWM channels are needed.
  */
  class GpioPwm : public Converter {

    public:
      /**
         @brief Constructs a GpioPwm object for the specified pin, with optional range and frequency.
         @param pin Pointer to the Pin object to be used for PWM output.
         @param range The PWM range (default is 1024).
         @param freq The PWM frequency in Hz (default is 100).
      */
      GpioPwm (Pin *pin, long range = 1024, long freq = 100);

      /**
         @brief Destructor for the GpioPwm class.
      */
      virtual ~GpioPwm();

      /**
         @brief Returns a constant reference to the associated Pin object.
         @return const Pin& Reference to the Pin used for PWM.
      */
      const Pin &pin() const;

    protected:
      /**
         @brief Protected constructor for internal use with a Private implementation.
         @param dd Reference to the Private implementation object.
      */
      class Private;
      GpioPwm (Private &dd);

    private:
      /**
         @brief Macro to declare the private implementation pointer for GpioPwm.
      */
      PIMP_DECLARE_PRIVATE (GpioPwm)
  };
}

/* ========================================================================== */
