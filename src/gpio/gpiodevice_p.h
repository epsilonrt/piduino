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

#include <piduino/gpiodevice.h>

namespace Piduino {

  class GpioDevice::Private {

    public:
      Private (GpioDevice * q);
      virtual ~Private();

      GpioDevice * const q_ptr;
      bool isopen;
      bool isdebug;

      PIMP_DECLARE_PUBLIC (GpioDevice)
  };
}

/* ========================================================================== */
