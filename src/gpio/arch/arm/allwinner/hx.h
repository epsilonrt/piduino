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
#ifndef PIDUINO_ARCH_ARM_ALLWINNER_HX_H
#define PIDUINO_ARCH_ARM_ALLWINNER_HX_H

#include <piduino/global.h>

namespace Piduino {

  namespace AllWinnerHx {

    const off_t Io1Base = 0x01C20000;
    const off_t Io2Base = 0x01F02000;
    const size_t  MapBlockSize = 4 * 1024;

    // GPIO
    const size_t  Pio1Offset = 0x800;
    const size_t  Pio2Offset = 0xC00;
    const size_t  GpioSize   = 106;

    // Register offsets --------------------------------------------------------

    // PWM ---------------------------------------------------------------------
    const size_t  PwmOffset    = 0x20C000;

    // Register offsets
    //  Bits
    // const uint32_t PWEN1 = 1 << 0;
  }
}
/* ========================================================================== */
#endif /* PIDUINO_ARCH_ARM_ALLWINNER_HX_H defined */
