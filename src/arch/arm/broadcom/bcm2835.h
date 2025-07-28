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

#include <piduino/soc.h>

namespace Piduino {

  namespace Bcm2835 {

    off_t iobase ();

    const off_t Bcm2708Base  = 0x20000000;
    const off_t Bcm2709Base  = 0x3F000000;
    const off_t Bcm2710Base  = 0x3F000000;
    const off_t Bcm2711Base  = 0xFE000000;
    const size_t  MapBlockSize = 4 * 1024;

    // GPIO
    const size_t  PioOffset    = 0x200000;
    const size_t  GpioSize     = 54;

    // Register offsets --------------------------------------------------------
    const size_t GFPSEL0 = 0;
    const size_t GFPSEL1 = 1;
    const size_t GFPSEL2 = 2;
    const size_t GFPSEL3 = 3;
    const size_t GFPSEL4 = 4;
    const size_t GFPSEL5 = 5;
    // Reserved: 6
    const size_t GPSET0  = 7;
    const size_t GPSET1  = 8;
    // Reserved: 9
    const size_t GPCLR0  = 10;
    const size_t GPCLR1  = 11;
    // Reserved: 12
    const size_t GPLEV0  = 13;
    const size_t GPLEV1  = 14;
    const size_t GPPUD   =  37;
    const size_t GPPUDCLK0 =   38;
    const size_t GPPUPPDN0 =   57;
    const size_t GPPUPPDN1 =   58;
    const size_t GPPUPPDN2 =   59;
    const size_t GPPUPPDN3 =   60;

    // PWM ---------------------------------------------------------------------
    const size_t  PwmOffset    = 0x20C000;

    // Register offsets
    const size_t CTL =     0; // PWM Control
    const size_t STA =     1; // PWM Status
    const size_t DMAC =    2; // PWM DMA Configuration
    const size_t RNG1 =    4; // PWM Channel 1 Range
    const size_t DAT1 =    5; // PWM Channel 1 Data
    const size_t FIF1 =    6; // PWM FIFO Input
    const size_t RNG2 =    8; // PWM Channel 2 Range
    const size_t DAT2 =    9; // PWM Channel 2 Data
    //  Bits
    const uint32_t PWEN1 = 1 << 0;
    const uint32_t MODE1 = 1 << 1;
    const uint32_t RPTL1 = 1 << 2;
    const uint32_t SBIT1 = 1 << 3;
    const uint32_t POLA1 = 1 << 4;
    const uint32_t USEF1 = 1 << 5;
    const uint32_t CLRF1 = 1 << 6;
    const uint32_t MSEN1 = 1 << 7;
    const uint32_t PWEN2 = 1 << 8;
    const uint32_t MODE2 = 1 << 9;
    const uint32_t RPTL2 = 1 << 10;
    const uint32_t SBIT2 = 1 << 11;
    const uint32_t POLA2 = 1 << 12;
    const uint32_t USEF2 = 1 << 13;
    const uint32_t MSEN2 = 1 << 15;

    const int PWM_RESOLUTION_MIN = 2;
    const int PWM_RESOLUTION_MAX = 31;

    // Clock Manager -----------------------------------------------------------
    const size_t  ClockOffset    = 0x101000;

    // Register offsets
    const size_t CM_PWMCTL = 40;
    const size_t CM_PWMDIV = 41;
    //  Bits
    const uint32_t PASSWD = 0x5A000000;
    const uint32_t BUSY   = 1 << 7;
    const uint32_t KILL   = 1 << 5;
    const uint32_t ENAB   = 1 << 4;
    const uint32_t SRC_OSC = 1;
    // PLL
    const uint32_t PLL_CLOCK_DIVISOR_MIN  = 2;
    const uint32_t PLL_CLOCK_DIVISOR_MAX  = 4095;
    const uint32_t PLL_CLOCK_FREQ         = 19200000UL;
    const uint32_t PLL_CLOCK_FREQ_2711    = 54000000UL;
  }
}
/* ========================================================================== */
