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
#include "converter_p.h"

namespace Piduino {

  /**
   * @brief Private implementation class for Pwm.
   *
   * This class encapsulates the private data and implementation details for the Pwm class,
   * following the PIMPL (Pointer to IMPLementation) idiom. It inherits from Converter::Private.
   */
  class Pwm::Private  : public Converter::Private {

    public:
      /**
       * @brief Constructs the private implementation for Pwm.
       * @param q Pointer to the associated Pwm object.
       */
      Private (Pwm * q);

      /**
       * @brief Destructor for the private implementation.
       */
      virtual ~Private();

      /**
       * @brief Macro to declare public interface access for Pwm.
       */
      PIMP_DECLARE_PUBLIC (Pwm)
  };
}

/* ========================================================================== */
