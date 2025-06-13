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
    const size_t  PwmOffset    = 0x1000;

    // Register offsets
    const size_t PWM_CH_CTRL    = 0x100; // PWM Control
    const size_t PWM_CH0_PERIOD = 0x101; // PWM Channel 0 Range/Value

    //  Bits
    const uint32_t PWM_CH0_PRESCAL_MASK = 0xF;
    const uint8_t PWM_CH0_PRESCAL_120 = 0;
    const uint8_t PWM_CH0_PRESCAL_180 = 1;
    const uint8_t PWM_CH0_PRESCAL_240 = 2;
    const uint8_t PWM_CH0_PRESCAL_360 = 3;
    const uint8_t PWM_CH0_PRESCAL_480 = 4;
    const uint8_t PWM_CH0_PRESCAL_12K = 8;
    const uint8_t PWM_CH0_PRESCAL_24K = 9;
    const uint8_t PWM_CH0_PRESCAL_36K = 10;
    const uint8_t PWM_CH0_PRESCAL_48K = 11;
    const uint8_t PWM_CH0_PRESCAL_72K = 12;
    const uint8_t PWM_CH0_PRESCAL_1   = 15;

    const uint32_t PWM_CH0_EN               = 1 << 4;
    const uint32_t PWM_CH0_ACT_STA          = 1 << 5;
    const uint32_t SCLK_CH0_GATING          = 1 << 6;
    const uint32_t PWM_CHANNEL0_MODE        = 1 << 7;
    const uint32_t PWM_CH0_PUL_START        = 1 << 8;
    const uint32_t PWM0_BYPASS              = 1 << 9;

    const uint32_t PWM0_RDY                 = 1 << 28;

    const int PWM_RESOLUTION_MIN    = 2;
    const int PWM_RESOLUTION_MAX    = 16;
    const uint32_t PLL_CLOCK_FREQ   = 24000000UL;
    const uint32_t PLL_CLOCK_DIVISOR_MIN  = 1;
    const uint32_t PLL_CLOCK_DIVISOR_MAX  = 72000;
  }
}
/* ========================================================================== */
