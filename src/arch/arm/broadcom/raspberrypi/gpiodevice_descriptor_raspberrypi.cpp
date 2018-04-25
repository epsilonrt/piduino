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

#include "../broadcom_bcm2835.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                     DeviceBcm2835 Gpio Descriptor
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  static int dilConnector (int row, int column, int columns) {

    return (row - 1) * columns + column;
  }

// -----------------------------------------------------------------------------
  const std::map<int, Gpio::Descriptor>
  DeviceBcm2835::_gpioDescriptors = {
    //----------------------------------------------------------------------------
    // GPIO Rev. 1
    // Model B PCB V1.0, Q3 2012, Board Rev <= 3 --->
    {
      1,
      {
        "rpi-rev1",
        {
          // Connecteurs
          {
            "p1", 1, 13, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {8, 0, 0, 2, 1}, {{Pin::ModeInput, "GPIO0"}, {Pin::ModeOutput, "GPIO0"}, {Pin::ModeAlt0, "SDA0"}, {Pin::ModeAlt1, "SA5"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {9, 1, 1, 3, 1}, {{Pin::ModeInput, "GPIO1"}, {Pin::ModeOutput, "GPIO1"}, {Pin::ModeAlt0, "SCL0"}, {Pin::ModeAlt1, "SA4"}}},
              {Pin::TypePower, { -1, -1, -1, 3, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {7, 4, 4, 4, 1}, {{Pin::ModeInput, "GPIO4"}, {Pin::ModeOutput, "GPIO4"}, {Pin::ModeAlt0, "GPCLK0"}, {Pin::ModeAlt1, "SA1"}, {Pin::ModeAlt5, "ARMTDI"}}},
              {Pin::TypeGpio, {15, 14, 14, 4, 2}, {{Pin::ModeInput, "GPIO14"}, {Pin::ModeOutput, "GPIO14"}, {Pin::ModeAlt0, "TXD0"}, {Pin::ModeAlt1, "SD6"}, {Pin::ModeAlt5, "TXD1"}}},
              {Pin::TypePower, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {16, 15, 15, 5, 2}, {{Pin::ModeInput, "GPIO15"}, {Pin::ModeOutput, "GPIO15"}, {Pin::ModeAlt0, "RXD0"}, {Pin::ModeAlt1, "SD7"}, {Pin::ModeAlt5, "RXD1"}}},
              {Pin::TypeGpio, {0, 17, 17, 6, 1}, {{Pin::ModeInput, "GPIO17"}, {Pin::ModeOutput, "GPIO17"}, {Pin::ModeAlt1, "SD9"}, {Pin::ModeAlt3, "RTS0"}, {Pin::ModeAlt4, "SPI1CE1"}, {Pin::ModeAlt5, "RTS1"}}},
              {Pin::TypeGpio, {1, 18, 18, 6, 2}, {{Pin::ModeInput, "GPIO18"}, {Pin::ModeOutput, "GPIO18"}, {Pin::ModeAlt0, "PCMCLK"}, {Pin::ModeAlt1, "SD10"}, {Pin::ModeAlt3, "BSMOSI"}, {Pin::ModeAlt4, "SPI1CE0"}, {Pin::ModeAlt5, "PWM0"}}},
              {Pin::TypeGpio, {2, 21, 21, 7, 1}, {{Pin::ModeInput, "GPIO21"}, {Pin::ModeOutput, "GPIO21"}, {Pin::ModeAlt0, "PCMDOUT"}, {Pin::ModeAlt1, "SD13"}, {Pin::ModeAlt3, "BSCE"}, {Pin::ModeAlt4, "SPI1SCLK"}, {Pin::ModeAlt5, "GPCLK1"}}},
              {Pin::TypePower, { -1, -1, -1, 7, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {3, 22, 22, 8, 1}, {{Pin::ModeInput, "GPIO22"}, {Pin::ModeOutput, "GPIO22"}, {Pin::ModeAlt1, "SD14"}, {Pin::ModeAlt3, "SD1CLK"}, {Pin::ModeAlt4, "ARMTRST"}}},
              {Pin::TypeGpio, {4, 23, 23, 8, 2}, {{Pin::ModeInput, "GPIO23"}, {Pin::ModeOutput, "GPIO23"}, {Pin::ModeAlt1, "SD15"}, {Pin::ModeAlt3, "SD1CMD"}, {Pin::ModeAlt4, "ARMRTCK"}}},
              {Pin::TypePower, { -1, -1, -1, 9, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {5, 24, 24, 9, 2}, {{Pin::ModeInput, "GPIO24"}, {Pin::ModeOutput, "GPIO24"}, {Pin::ModeAlt1, "SD16"}, {Pin::ModeAlt3, "SD1DAT0"}, {Pin::ModeAlt4, "ARMTDO"}}},
              {Pin::TypeGpio, {12, 10, 10, 10, 1}, {{Pin::ModeInput, "GPIO10"}, {Pin::ModeOutput, "GPIO10"}, {Pin::ModeAlt0, "SPI0MOSI"}, {Pin::ModeAlt1, "SD2"}}},
              {Pin::TypePower, { -1, -1, -1, 10, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {13, 9, 9, 11, 1}, {{Pin::ModeInput, "GPIO9"}, {Pin::ModeOutput, "GPIO9"}, {Pin::ModeAlt0, "SPI0MISO"}, {Pin::ModeAlt1, "SD1"}}},
              {Pin::TypeGpio, {6, 25, 25, 11, 2}, {{Pin::ModeInput, "GPIO25"}, {Pin::ModeOutput, "GPIO25"}, {Pin::ModeAlt1, "SD17"}, {Pin::ModeAlt3, "SD1DAT1"}, {Pin::ModeAlt4, "ARMTCK"}}},
              {Pin::TypeGpio, {14, 11, 11, 12, 1}, {{Pin::ModeInput, "GPIO11"}, {Pin::ModeOutput, "GPIO11"}, {Pin::ModeAlt0, "SPI0SCLK"}, {Pin::ModeAlt1, "SD3"}}},
              {Pin::TypeGpio, {10, 8, 8, 12, 2}, {{Pin::ModeInput, "GPIO8"}, {Pin::ModeOutput, "GPIO8"}, {Pin::ModeAlt0, "SPI0CE0"}, {Pin::ModeAlt1, "SD0"}}},
              {Pin::TypePower, { -1, -1, -1, 13, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {11, 7, 7, 13, 2}, {{Pin::ModeInput, "GPIO7"}, {Pin::ModeOutput, "GPIO7"}, {Pin::ModeAlt0, "SPI0CE1"}, {Pin::ModeAlt1, "SWE"}}},
            }
          },
        }
      }
    },
    // <--- Model B PCB V1.0, Q3 2012, Board Rev <= 3
    //----------------------------------------------------------------------------
    // GPIO Rev. 2
    // Modèle A et B PCB V2.0 --->
    {
      2,
      {
        "rpi-rev2",
        {
          // Connecteurs
          {
            "p1", 1, 23, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {8, 2, 2, 2, 1}, {{Pin::ModeInput, "GPIO2"}, {Pin::ModeOutput, "GPIO2"}, {Pin::ModeAlt0, "SDA1"}, {Pin::ModeAlt1, "SA3"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {9, 3, 3, 3, 1}, {{Pin::ModeInput, "GPIO3"}, {Pin::ModeOutput, "GPIO3"}, {Pin::ModeAlt0, "SCL1"}, {Pin::ModeAlt1, "SA2"}}},
              {Pin::TypePower, { -1, -1, -1, 3, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {7, 4, 4, 4, 1}, {{Pin::ModeInput, "GPIO4"}, {Pin::ModeOutput, "GPIO4"}, {Pin::ModeAlt0, "GPCLK0"}, {Pin::ModeAlt1, "SA1"}, {Pin::ModeAlt5, "ARMTDI"}}},
              {Pin::TypeGpio, {15, 14, 14, 4, 2}, {{Pin::ModeInput, "GPIO14"}, {Pin::ModeOutput, "GPIO14"}, {Pin::ModeAlt0, "TXD0"}, {Pin::ModeAlt1, "SD6"}, {Pin::ModeAlt5, "TXD1"}}},
              {Pin::TypePower, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {16, 15, 15, 5, 2}, {{Pin::ModeInput, "GPIO15"}, {Pin::ModeOutput, "GPIO15"}, {Pin::ModeAlt0, "RXD0"}, {Pin::ModeAlt1, "SD7"}, {Pin::ModeAlt5, "RXD1"}}},
              {Pin::TypeGpio, {0, 17, 17, 6, 1}, {{Pin::ModeInput, "GPIO17"}, {Pin::ModeOutput, "GPIO17"}, {Pin::ModeAlt1, "SD9"}, {Pin::ModeAlt3, "RTS0"}, {Pin::ModeAlt4, "SPI1CE1"}, {Pin::ModeAlt5, "RTS1"}}},
              {Pin::TypeGpio, {1, 18, 18, 6, 2}, {{Pin::ModeInput, "GPIO18"}, {Pin::ModeOutput, "GPIO18"}, {Pin::ModeAlt0, "PCMCLK"}, {Pin::ModeAlt1, "SD10"}, {Pin::ModeAlt3, "BSMOSI"}, {Pin::ModeAlt4, "SPI1CE0"}, {Pin::ModeAlt5, "PWM0"}}},
              {Pin::TypeGpio, {2, 27, 27, 7, 1}, {{Pin::ModeInput, "GPIO27"}, {Pin::ModeOutput, "GPIO27"}, {Pin::ModeAlt3, "SD1DAT3"}, {Pin::ModeAlt4, "ARMTMS"}}},
              {Pin::TypePower, { -1, -1, -1, 7, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {3, 22, 22, 8, 1}, {{Pin::ModeInput, "GPIO22"}, {Pin::ModeOutput, "GPIO22"}, {Pin::ModeAlt1, "SD14"}, {Pin::ModeAlt3, "SD1CLK"}, {Pin::ModeAlt4, "ARMTRST"}}},
              {Pin::TypeGpio, {4, 23, 23, 8, 2}, {{Pin::ModeInput, "GPIO23"}, {Pin::ModeOutput, "GPIO23"}, {Pin::ModeAlt1, "SD15"}, {Pin::ModeAlt3, "SD1CMD"}, {Pin::ModeAlt4, "ARMRTCK"}}},
              {Pin::TypePower, { -1, -1, -1, 9, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {5, 24, 24, 9, 2}, {{Pin::ModeInput, "GPIO24"}, {Pin::ModeOutput, "GPIO24"}, {Pin::ModeAlt1, "SD16"}, {Pin::ModeAlt3, "SD1DAT0"}, {Pin::ModeAlt4, "ARMTDO"}}},
              {Pin::TypeGpio, {12, 10, 10, 10, 1}, {{Pin::ModeInput, "GPIO10"}, {Pin::ModeOutput, "GPIO10"}, {Pin::ModeAlt0, "SPI0MOSI"}, {Pin::ModeAlt1, "SD2"}}},
              {Pin::TypePower, { -1, -1, -1, 10, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {13, 9, 9, 11, 1}, {{Pin::ModeInput, "GPIO9"}, {Pin::ModeOutput, "GPIO9"}, {Pin::ModeAlt0, "SPI0MISO"}, {Pin::ModeAlt1, "SD1"}}},
              {Pin::TypeGpio, {6, 25, 25, 11, 2}, {{Pin::ModeInput, "GPIO25"}, {Pin::ModeOutput, "GPIO25"}, {Pin::ModeAlt1, "SD17"}, {Pin::ModeAlt3, "SD1DAT1"}, {Pin::ModeAlt4, "ARMTCK"}}},
              {Pin::TypeGpio, {14, 11, 11, 12, 1}, {{Pin::ModeInput, "GPIO11"}, {Pin::ModeOutput, "GPIO11"}, {Pin::ModeAlt0, "SPI0SCLK"}, {Pin::ModeAlt1, "SD3"}}},
              {Pin::TypeGpio, {10, 8, 8, 12, 2}, {{Pin::ModeInput, "GPIO8"}, {Pin::ModeOutput, "GPIO8"}, {Pin::ModeAlt0, "SPI0CE0"}, {Pin::ModeAlt1, "SD0"}}},
              {Pin::TypePower, { -1, -1, -1, 13, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {11, 7, 7, 13, 2}, {{Pin::ModeInput, "GPIO7"}, {Pin::ModeOutput, "GPIO7"}, {Pin::ModeAlt0, "SPI0CE1"}, {Pin::ModeAlt1, "SWE"}}},
            }
          },
          {
            "p5", 2, 4, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "5V "}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {17, 28, 28, 2, 1}, {{Pin::ModeInput, "GPIO28"}, {Pin::ModeOutput, "GPIO28"}}},
              {Pin::TypeGpio, {18, 29, 29, 2, 2}, {{Pin::ModeInput, "GPIO29"}, {Pin::ModeOutput, "GPIO29"}}},
              {Pin::TypeGpio, {19, 30, 30, 3, 1}, {{Pin::ModeInput, "GPIO30"}, {Pin::ModeOutput, "GPIO30"}}},
              {Pin::TypeGpio, {20, 31, 31, 3, 2}, {{Pin::ModeInput, "GPIO31"}, {Pin::ModeOutput, "GPIO31"}}},
              {Pin::TypePower, { -1, -1, -1, 4, 1}, {{Pin::ModeInput, "GND "}}},
              {Pin::TypePower, { -1, -1, -1, 4, 2}, {{Pin::ModeInput, "GND "}}},
            }
          },
        }
      }
    },
    // <--- Modèle A et B PCB V2.0
    //----------------------------------------------------------------------------
    // GPIO Rev. 3
    // Modèle A+, B+, 2B, 3B, Zero --->
    {
      3,
      {
        "rpi-rev3",
        {
          // Connecteurs
          {
            "j8", 1, 20, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {8, 2, 2, 2, 1}, {{Pin::ModeInput, "GPIO2"}, {Pin::ModeOutput, "GPIO2"}, {Pin::ModeAlt0, "SDA1"}, {Pin::ModeAlt1, "SA3"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {9, 3, 3, 3, 1}, {{Pin::ModeInput, "GPIO3"}, {Pin::ModeOutput, "GPIO3"}, {Pin::ModeAlt0, "SCL1"}, {Pin::ModeAlt1, "SA2"}}},
              {Pin::TypePower, { -1, -1, -1, 3, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {7, 4, 4, 4, 1}, {{Pin::ModeInput, "GPIO4"}, {Pin::ModeOutput, "GPIO4"}, {Pin::ModeAlt0, "GPCLK0"}, {Pin::ModeAlt1, "SA1"}, {Pin::ModeAlt5, "ARMTDI"}}},
              {Pin::TypeGpio, {15, 14, 14, 4, 2}, {{Pin::ModeInput, "GPIO14"}, {Pin::ModeOutput, "GPIO14"}, {Pin::ModeAlt0, "TXD0"}, {Pin::ModeAlt1, "SD6"}, {Pin::ModeAlt5, "TXD1"}}},
              {Pin::TypePower, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {16, 15, 15, 5, 2}, {{Pin::ModeInput, "GPIO15"}, {Pin::ModeOutput, "GPIO15"}, {Pin::ModeAlt0, "RXD0"}, {Pin::ModeAlt1, "SD7"}, {Pin::ModeAlt5, "RXD1"}}},
              {Pin::TypeGpio, {0, 17, 17, 6, 1}, {{Pin::ModeInput, "GPIO17"}, {Pin::ModeOutput, "GPIO17"}, {Pin::ModeAlt1, "SD9"}, {Pin::ModeAlt3, "RTS0"}, {Pin::ModeAlt4, "SPI1CE1"}, {Pin::ModeAlt5, "RTS1"}}},
              {Pin::TypeGpio, {1, 18, 18, 6, 2}, {{Pin::ModeInput, "GPIO18"}, {Pin::ModeOutput, "GPIO18"}, {Pin::ModeAlt0, "PCMCLK"}, {Pin::ModeAlt1, "SD10"}, {Pin::ModeAlt3, "BSMOSI"}, {Pin::ModeAlt4, "SPI1CE0"}, {Pin::ModeAlt5, "PWM0"}}},
              {Pin::TypeGpio, {2, 27, 27, 7, 1}, {{Pin::ModeInput, "GPIO27"}, {Pin::ModeOutput, "GPIO27"}, {Pin::ModeAlt3, "SD1DAT3"}, {Pin::ModeAlt4, "ARMTMS"}}},
              {Pin::TypePower, { -1, -1, -1, 7, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {3, 22, 22, 8, 1}, {{Pin::ModeInput, "GPIO22"}, {Pin::ModeOutput, "GPIO22"}, {Pin::ModeAlt1, "SD14"}, {Pin::ModeAlt3, "SD1CLK"}, {Pin::ModeAlt4, "ARMTRST"}}},
              {Pin::TypeGpio, {4, 23, 23, 8, 2}, {{Pin::ModeInput, "GPIO23"}, {Pin::ModeOutput, "GPIO23"}, {Pin::ModeAlt1, "SD15"}, {Pin::ModeAlt3, "SD1CMD"}, {Pin::ModeAlt4, "ARMRTCK"}}},
              {Pin::TypePower, { -1, -1, -1, 9, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {5, 24, 24, 9, 2}, {{Pin::ModeInput, "GPIO24"}, {Pin::ModeOutput, "GPIO24"}, {Pin::ModeAlt1, "SD16"}, {Pin::ModeAlt3, "SD1DAT0"}, {Pin::ModeAlt4, "ARMTDO"}}},
              {Pin::TypeGpio, {12, 10, 10, 10, 1}, {{Pin::ModeInput, "GPIO10"}, {Pin::ModeOutput, "GPIO10"}, {Pin::ModeAlt0, "SPI0MOSI"}, {Pin::ModeAlt1, "SD2"}}},
              {Pin::TypePower, { -1, -1, -1, 10, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {13, 9, 9, 11, 1}, {{Pin::ModeInput, "GPIO9"}, {Pin::ModeOutput, "GPIO9"}, {Pin::ModeAlt0, "SPI0MISO"}, {Pin::ModeAlt1, "SD1"}}},
              {Pin::TypeGpio, {6, 25, 25, 11, 2}, {{Pin::ModeInput, "GPIO25"}, {Pin::ModeOutput, "GPIO25"}, {Pin::ModeAlt1, "SD17"}, {Pin::ModeAlt3, "SD1DAT1"}, {Pin::ModeAlt4, "ARMTCK"}}},
              {Pin::TypeGpio, {14, 11, 11, 12, 1}, {{Pin::ModeInput, "GPIO11"}, {Pin::ModeOutput, "GPIO11"}, {Pin::ModeAlt0, "SPI0SCLK"}, {Pin::ModeAlt1, "SD3"}}},
              {Pin::TypeGpio, {10, 8, 8, 12, 2}, {{Pin::ModeInput, "GPIO8"}, {Pin::ModeOutput, "GPIO8"}, {Pin::ModeAlt0, "SPI0CE0"}, {Pin::ModeAlt1, "SD0"}}},
              {Pin::TypePower, { -1, -1, -1, 13, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {11, 7, 7, 13, 2}, {{Pin::ModeInput, "GPIO7"}, {Pin::ModeOutput, "GPIO7"}, {Pin::ModeAlt0, "SPI0CE1"}, {Pin::ModeAlt1, "SWE"}}},
              {Pin::TypeGpio, {30, 0, 0, 14, 1}, {{Pin::ModeInput, "GPIO0"}, {Pin::ModeOutput, "GPIO0"}, {Pin::ModeAlt0, "SDA0"}, {Pin::ModeAlt1, "SA5"}}},
              {Pin::TypeGpio, {31, 1, 1, 14, 2}, {{Pin::ModeInput, "GPIO1"}, {Pin::ModeOutput, "GPIO1"}, {Pin::ModeAlt0, "SCL0"}, {Pin::ModeAlt1, "SA4"}}},
              {Pin::TypeGpio, {21, 5, 5, 15, 1}, {{Pin::ModeInput, "GPIO5"}, {Pin::ModeOutput, "GPIO5"}, {Pin::ModeAlt0, "GPCLK1"}, {Pin::ModeAlt1, "SA0"}, {Pin::ModeAlt5, "ARMTDO"}}},
              {Pin::TypePower, { -1, -1, -1, 15, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {22, 6, 6, 16, 1}, {{Pin::ModeInput, "GPIO6"}, {Pin::ModeOutput, "LAN_RUN"}, {Pin::ModeAlt0, "GPCLK2"}, {Pin::ModeAlt1, "SOE"}, {Pin::ModeAlt5, "ARMRTCK"}}},
              {Pin::TypeGpio, {26, 12, 12, 16, 2}, {{Pin::ModeInput, "GPIO12"}, {Pin::ModeOutput, "GPIO12"}, {Pin::ModeAlt0, "PWM0"}, {Pin::ModeAlt1, "SD4"}, {Pin::ModeAlt5, "ARMTMS"}}},
              {Pin::TypeGpio, {23, 13, 13, 17, 1}, {{Pin::ModeInput, "GPIO13"}, {Pin::ModeOutput, "GPIO13"}, {Pin::ModeAlt0, "PWM1"}, {Pin::ModeAlt1, "SD5"}, {Pin::ModeAlt5, "ARMTCK"}}},
              {Pin::TypePower, { -1, -1, -1, 17, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {24, 19, 19, 18, 1}, {{Pin::ModeInput, "GPIO19"}, {Pin::ModeOutput, "GPIO19"}, {Pin::ModeAlt0, "PCMFS"}, {Pin::ModeAlt1, "SD11"}, {Pin::ModeAlt3, "BSSCLK"}, {Pin::ModeAlt4, "SPI1MISO"}, {Pin::ModeAlt5, "PWM1"}}},
              {Pin::TypeGpio, {27, 16, 16, 18, 2}, {{Pin::ModeInput, "GPIO16"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt1, "SD8"}, {Pin::ModeAlt3, "CTS0"}, {Pin::ModeAlt4, "SPI1CE2"}, {Pin::ModeAlt5, "CTS1"}}},
              {Pin::TypeGpio, {25, 26, 26, 19, 1}, {{Pin::ModeInput, "GPIO26"}, {Pin::ModeOutput, "GPIO26"}, {Pin::ModeAlt3, "SD1DAT2"}, {Pin::ModeAlt4, "ARMTDI"}}},
              {Pin::TypeGpio, {28, 20, 20, 19, 2}, {{Pin::ModeInput, "GPIO20"}, {Pin::ModeOutput, "GPIO20"}, {Pin::ModeAlt0, "PCMDIN"}, {Pin::ModeAlt1, "SD12"}, {Pin::ModeAlt3, "BSMISO"}, {Pin::ModeAlt4, "SPI1MOSI"}, {Pin::ModeAlt5, "GPCLK0"}}},
              {Pin::TypePower, { -1, -1, -1, 20, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {29, 21, 21, 20, 2}, {{Pin::ModeInput, "GPIO21"}, {Pin::ModeOutput, "GPIO21"}, {Pin::ModeAlt0, "PCMDOUT"}, {Pin::ModeAlt1, "SD13"}, {Pin::ModeAlt3, "BSCE"}, {Pin::ModeAlt4, "SPI1SCLK"}, {Pin::ModeAlt5, "GPCLK1"}}},
            }
          },
        }
      }
    },
    // <--- Modèle A+, B+, 2B, 3B, Zero
    //----------------------------------------------------------------------------
  };
}
/* ========================================================================== */
