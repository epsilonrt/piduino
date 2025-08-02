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

namespace Piduino {

  /**
     @class Pwm
     @brief Abstract base class for Pulse Width Modulation (PWM) converters.

     The Pwm class provides an interface for PWM signal generation and configuration.
     It inherits from the Converter class and defines pure virtual methods for frequency
     management, as well as virtual methods for range configuration. This class is intended
     to be subclassed by specific PWM implementations.
  */
  class Pwm : public Converter {

    public:
      /**
         @brief Default constructor.
      */
      Pwm();

      /**
         @brief Virtual destructor.
      */
      virtual ~Pwm();

    protected:
      /**
         @brief Forward declaration of the Private implementation class.
      */
      class Private;

      /**
         @brief Protected constructor for use by subclasses with private implementation.
         @param dd Reference to the private implementation.
      */
      Pwm (Private &dd);

    private:
      /**
         @brief Macro to declare private implementation pointer.
      */
      PIMP_DECLARE_PRIVATE (Pwm)
  };

} // namespace Piduino

/* ========================================================================== */
