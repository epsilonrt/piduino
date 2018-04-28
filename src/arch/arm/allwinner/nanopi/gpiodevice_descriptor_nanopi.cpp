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

#include "../allwinner_h.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                     DeviceAllwinnerH Gpio Descriptor
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  static int dilConnector (int row, int column, int columns) {

    return (row - 1) * columns + column;
  }

// -----------------------------------------------------------------------------
  static int silConnector (int row, int column, int columns) {

    return row;
  }

// -----------------------------------------------------------------------------
  const std::map<int, Gpio::Descriptor>
  DeviceAllwinnerH::_gpioDescriptors = {
    //----------------------------------------------------------------------------
    // GPIO ID 4
    // NanoPi Neo, Neo2, Neo Plus2, Neo Air --->
    {
      4,
      {
        "nanopineo",
        {
          // Connecteurs
          {
            "con1", 1, 12, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {8, 12, 12, 2, 1}, {{Pin::ModeInput, "GPIOA12"}, {Pin::ModeOutput, "GPIOA12"}, {Pin::ModeAlt2, "I2C0SDA"}, {Pin::ModeAlt3, "DIRX"}, {Pin::ModeAlt6, "PAEINT12"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {9, 11, 11, 3, 1}, {{Pin::ModeInput, "GPIOA11"}, {Pin::ModeOutput, "GPIOA11"}, {Pin::ModeAlt2, "I2C0SCK"}, {Pin::ModeAlt3, "DITX"}, {Pin::ModeAlt6, "PAEINT11"}}},
              {Pin::TypePower, { -1, -1, -1, 3, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {7, 91, 203, 4, 1}, {{Pin::ModeInput, "GPIOG11"}, {Pin::ModeOutput, "GPIOG11"}, {Pin::ModeAlt2, "PCM1CLK"}, {Pin::ModeAlt6, "PGEINT11"}}},
              {Pin::TypeGpio, {15, 86, 198, 4, 2}, {{Pin::ModeInput, "GPIOG6"}, {Pin::ModeOutput, "GPIOG6"}, {Pin::ModeAlt2, "UART1TX"}, {Pin::ModeAlt6, "PGEINT6"}}},
              {Pin::TypePower, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {16, 87, 199, 5, 2}, {{Pin::ModeInput, "GPIOG7"}, {Pin::ModeOutput, "GPIOG7"}, {Pin::ModeAlt2, "UART1RX"}, {Pin::ModeAlt6, "PGEINT7"}}},
              {Pin::TypeGpio, {0, 0, 0, 6, 1}, {{Pin::ModeInput, "GPIOA0"}, {Pin::ModeOutput, "GPIOA0"}, {Pin::ModeAlt2, "UART2TX"}, {Pin::ModeAlt3, "JTAGMS"}, {Pin::ModeAlt6, "PAEINT0"}}},
              {Pin::TypeGpio, {1, 6, 6, 6, 2}, {{Pin::ModeInput, "GPIOA6"}, {Pin::ModeOutput, "GPIOA6"}, {Pin::ModeAlt2, "SIMPWREN"}, {Pin::ModeAlt3, "PWM1"}, {Pin::ModeAlt6, "PAEINT6"}}},
              {Pin::TypeGpio, {2, 2, 2, 7, 1}, {{Pin::ModeInput, "GPIOA2"}, {Pin::ModeOutput, "GPIOA2"}, {Pin::ModeAlt2, "UART2RTS"}, {Pin::ModeAlt3, "JTAGDO"}, {Pin::ModeAlt6, "PAEINT2"}}},
              {Pin::TypePower, { -1, -1, -1, 7, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {3, 3, 3, 8, 1}, {{Pin::ModeInput, "GPIOA3"}, {Pin::ModeOutput, "GPIOA3"}, {Pin::ModeAlt2, "UART2CTS"}, {Pin::ModeAlt3, "JTAGDI"}, {Pin::ModeAlt6, "PAEINT3"}}},
              {Pin::TypeGpio, {4, 88, 200, 8, 2}, {{Pin::ModeInput, "GPIOG8"}, {Pin::ModeOutput, "GPIOG8"}, {Pin::ModeAlt2, "UART1RTS"}, {Pin::ModeAlt6, "PGEINT8"}}},
              {Pin::TypePower, { -1, -1, -1, 9, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {5, 89, 201, 9, 2}, {{Pin::ModeInput, "GPIOG9"}, {Pin::ModeOutput, "GPIOG9"}, {Pin::ModeAlt2, "UART1CTS"}, {Pin::ModeAlt6, "PGEINT9"}}},
              {Pin::TypeGpio, {12, 22, 64, 10, 1}, {{Pin::ModeInput, "GPIOC0"}, {Pin::ModeOutput, "GPIOC0"}, {Pin::ModeAlt2, "NANDWE"}, {Pin::ModeAlt3, "SPI0MOSI"}}},
              {Pin::TypePower, { -1, -1, -1, 10, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {13, 23, 65, 11, 1}, {{Pin::ModeInput, "GPIOC1"}, {Pin::ModeOutput, "GPIOC1"}, {Pin::ModeAlt2, "NANDALE"}, {Pin::ModeAlt3, "SPI0MISO"}}},
              {Pin::TypeGpio, {6, 1, 1, 11, 2}, {{Pin::ModeInput, "GPIOA1"}, {Pin::ModeOutput, "GPIOA1"}, {Pin::ModeAlt2, "UART2RX"}, {Pin::ModeAlt3, "JTAGCK"}, {Pin::ModeAlt6, "PAEINT1"}}},
              {Pin::TypeGpio, {14, 24, 66, 12, 1}, {{Pin::ModeInput, "GPIOC2"}, {Pin::ModeOutput, "GPIOC2"}, {Pin::ModeAlt2, "NANDCLE"}, {Pin::ModeAlt3, "SPI0CLK"}}},
              {Pin::TypeGpio, {10, 25, 67, 12, 2}, {{Pin::ModeInput, "GPIOC3"}, {Pin::ModeOutput, "GPIOC3"}, {Pin::ModeAlt2, "NANDCE1"}, {Pin::ModeAlt3, "SPI0CS"}}},
            }
          },
          {
            "dbg_uart", 2, 4, 1, silConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 1}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {17, 4, 4, 3, 1}, {{Pin::ModeInput, "GPIOA4"}, {Pin::ModeOutput, "GPIOA4"}, {Pin::ModeAlt2, "UART0TX"}, {Pin::ModeAlt6, "PAEINT4"}}},
              {Pin::TypeGpio, {18, 5, 5, 4, 1}, {{Pin::ModeInput, "GPIOA5"}, {Pin::ModeOutput, "GPIOA5"}, {Pin::ModeAlt2, "UART0RX"}, {Pin::ModeAlt3, "PWM0"}, {Pin::ModeAlt6, "PAEINT5"}}},
            }
          },
          {
            "inner", 3, 2, 1, silConnector,
            {
              {Pin::TypeGpio, {19, 10, 10, 1, 1}, {{Pin::ModeInput, "GPIOA10"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt2, "SIMDET"}, {Pin::ModeAlt6, "PAEINT10"}}},
              {Pin::TypeGpio, {32, 104, 362, 2, 1}, {{Pin::ModeInput, "GPIOL10"}, {Pin::ModeOutput, "PWR_LED"}, {Pin::ModeAlt2, "SPWM"}, {Pin::ModeAlt6, "PLEINT10"}}},
            }
          },
          {
            "con2", 4, 12, 1, silConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeUsb, { -1, -1, -1, 2, 1}, {{Pin::ModeInput, "USB-DP1"}}},
              {Pin::TypeUsb, { -1, -1, -1, 3, 1}, {{Pin::ModeInput, "USB-DM1"}}},
              {Pin::TypeUsb, { -1, -1, -1, 4, 1}, {{Pin::ModeInput, "USB-DP2"}}},
              {Pin::TypeUsb, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "USB-DM2"}}},
              {Pin::TypeGpio, {20, 105, 363, 6, 1}, {{Pin::ModeInput, "GPIOL11"}, {Pin::ModeOutput, "GPIOL11"}, {Pin::ModeAlt2, "SCIRRX"}, {Pin::ModeAlt6, "PLEINT11"}}},
              {Pin::TypeGpio, {11, 17, 17, 7, 1}, {{Pin::ModeInput, "GPIOA17"}, {Pin::ModeOutput, "GPIOA17"}, {Pin::ModeAlt2, "OWAOUT"}, {Pin::ModeAlt6, "PAEINT17"}}},
              {Pin::TypeGpio, {31, 18, 18, 8, 1}, {{Pin::ModeInput, "GPIOA18"}, {Pin::ModeOutput, "GPIOA18"}, {Pin::ModeAlt2, "PCM0SYNC"}, {Pin::ModeAlt3, "I2C1SCK"}, {Pin::ModeAlt6, "PAEINT18"}}},
              {Pin::TypeGpio, {30, 19, 19, 9, 1}, {{Pin::ModeInput, "GPIOA19"}, {Pin::ModeOutput, "GPIOA19"}, {Pin::ModeAlt2, "PCM0CLK"}, {Pin::ModeAlt3, "I2C1SDA"}, {Pin::ModeAlt6, "PAEINT19"}}},
              {Pin::TypeGpio, {21, 20, 20, 10, 1}, {{Pin::ModeInput, "GPIOA20"}, {Pin::ModeOutput, "GPIOA20"}, {Pin::ModeAlt2, "PCM0DOUT"}, {Pin::ModeAlt3, "SIMVPPEN"}, {Pin::ModeAlt6, "PAEINT20"}}},
              {Pin::TypeGpio, {22, 21, 21, 11, 1}, {{Pin::ModeInput, "GPIOA21"}, {Pin::ModeOutput, "GPIOA21"}, {Pin::ModeAlt2, "PCM0DIN"}, {Pin::ModeAlt3, "SIMVPPPP"}, {Pin::ModeAlt6, "PAEINT21"}}},
              {Pin::TypePower, { -1, -1, -1, 12, 1}, {{Pin::ModeInput, "GND"}}},
            }
          },
        }
      }
    },
    // <--- NanoPi Neo, Neo2, Neo Plus2, Neo Air
    //----------------------------------------------------------------------------
    // GPIO ID 2
    // NanoPi M1 --->
    {
      5,
      {
        "nanopim1",
        {
          // Connecteurs
          {
            "con1", 1, 20, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {8, 12, 12, 2, 1}, {{Pin::ModeInput, "GPIOA12"}, {Pin::ModeOutput, "GPIOA12"}, {Pin::ModeAlt2, "I2C0SDA"}, {Pin::ModeAlt3, "DIRX"}, {Pin::ModeAlt6, "PAEINT12"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {9, 11, 11, 3, 1}, {{Pin::ModeInput, "GPIOA11"}, {Pin::ModeOutput, "GPIOA11"}, {Pin::ModeAlt2, "I2C0SCK"}, {Pin::ModeAlt3, "DITX"}, {Pin::ModeAlt6, "PAEINT11"}}},
              {Pin::TypePower, { -1, -1, -1, 3, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {7, 91, 203, 4, 1}, {{Pin::ModeInput, "GPIOG11"}, {Pin::ModeOutput, "GPIOG11"}, {Pin::ModeAlt2, "PCM1CLK"}, {Pin::ModeAlt6, "PGEINT11"}}},
              {Pin::TypeGpio, {15, 86, 198, 4, 2}, {{Pin::ModeInput, "GPIOG6"}, {Pin::ModeOutput, "GPIOG6"}, {Pin::ModeAlt2, "UART1TX"}, {Pin::ModeAlt6, "PGEINT6"}}},
              {Pin::TypePower, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {16, 87, 199, 5, 2}, {{Pin::ModeInput, "GPIOG7"}, {Pin::ModeOutput, "GPIOG7"}, {Pin::ModeAlt2, "UART1RX"}, {Pin::ModeAlt6, "PGEINT7"}}},
              {Pin::TypeGpio, {0, 0, 0, 6, 1}, {{Pin::ModeInput, "GPIOA0"}, {Pin::ModeOutput, "GPIOA0"}, {Pin::ModeAlt2, "UART2TX"}, {Pin::ModeAlt3, "JTAGMS"}, {Pin::ModeAlt6, "PAEINT0"}}},
              {Pin::TypeGpio, {1, 6, 6, 6, 2}, {{Pin::ModeInput, "GPIOA6"}, {Pin::ModeOutput, "GPIOA6"}, {Pin::ModeAlt2, "SIMPWREN"}, {Pin::ModeAlt3, "PWM1"}, {Pin::ModeAlt6, "PAEINT6"}}},
              {Pin::TypeGpio, {2, 2, 2, 7, 1}, {{Pin::ModeInput, "GPIOA2"}, {Pin::ModeOutput, "GPIOA2"}, {Pin::ModeAlt2, "UART2RTS"}, {Pin::ModeAlt3, "JTAGDO"}, {Pin::ModeAlt6, "PAEINT2"}}},
              {Pin::TypePower, { -1, -1, -1, 7, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {3, 3, 3, 8, 1}, {{Pin::ModeInput, "GPIOA3"}, {Pin::ModeOutput, "GPIOA3"}, {Pin::ModeAlt2, "UART2CTS"}, {Pin::ModeAlt3, "JTAGDI"}, {Pin::ModeAlt6, "PAEINT3"}}},
              {Pin::TypeGpio, {4, 88, 200, 8, 2}, {{Pin::ModeInput, "GPIOG8"}, {Pin::ModeOutput, "GPIOG8"}, {Pin::ModeAlt2, "UART1RTS"}, {Pin::ModeAlt6, "PGEINT8"}}},
              {Pin::TypePower, { -1, -1, -1, 9, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {5, 89, 201, 9, 2}, {{Pin::ModeInput, "GPIOG9"}, {Pin::ModeOutput, "GPIOG9"}, {Pin::ModeAlt2, "UART1CTS"}, {Pin::ModeAlt6, "PGEINT9"}}},
              {Pin::TypeGpio, {12, 22, 64, 10, 1}, {{Pin::ModeInput, "GPIOC0"}, {Pin::ModeOutput, "GPIOC0"}, {Pin::ModeAlt2, "NANDWE"}, {Pin::ModeAlt3, "SPI0MOSI"}}},
              {Pin::TypePower, { -1, -1, -1, 10, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {13, 23, 65, 11, 1}, {{Pin::ModeInput, "GPIOC1"}, {Pin::ModeOutput, "GPIOC1"}, {Pin::ModeAlt2, "NANDALE"}, {Pin::ModeAlt3, "SPI0MISO"}}},
              {Pin::TypeGpio, {6, 1, 1, 11, 2}, {{Pin::ModeInput, "GPIOA1"}, {Pin::ModeOutput, "GPIOA1"}, {Pin::ModeAlt2, "UART2RX"}, {Pin::ModeAlt3, "JTAGCK"}, {Pin::ModeAlt6, "PAEINT1"}}},
              {Pin::TypeGpio, {14, 24, 66, 12, 1}, {{Pin::ModeInput, "GPIOC2"}, {Pin::ModeOutput, "GPIOC2"}, {Pin::ModeAlt2, "NANDCLE"}, {Pin::ModeAlt3, "SPI0CLK"}}},
              {Pin::TypeGpio, {10, 25, 67, 12, 2}, {{Pin::ModeInput, "GPIOC3"}, {Pin::ModeOutput, "GPIOC3"}, {Pin::ModeAlt2, "NANDCE1"}, {Pin::ModeAlt3, "SPI0CS"}}},
              {Pin::TypePower, { -1, -1, -1, 13, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {11, 17, 17, 13, 2}, {{Pin::ModeInput, "GPIOA17"}, {Pin::ModeOutput, "GPIOA17"}, {Pin::ModeAlt2, "OWAOUT"}, {Pin::ModeAlt6, "PAEINT17"}}},
              {Pin::TypeGpio, {30, 19, 19, 14, 1}, {{Pin::ModeInput, "GPIOA19"}, {Pin::ModeOutput, "GPIOA19"}, {Pin::ModeAlt2, "PCM0CLK"}, {Pin::ModeAlt3, "I2C1SDA"}, {Pin::ModeAlt6, "PAEINT19"}}},
              {Pin::TypeGpio, {31, 18, 18, 14, 2}, {{Pin::ModeInput, "GPIOA18"}, {Pin::ModeOutput, "GPIOA18"}, {Pin::ModeAlt2, "PCM0SYNC"}, {Pin::ModeAlt3, "I2C1SCK"}, {Pin::ModeAlt6, "PAEINT18"}}},
              {Pin::TypeGpio, {21, 20, 20, 15, 1}, {{Pin::ModeInput, "GPIOA20"}, {Pin::ModeOutput, "GPIOA20"}, {Pin::ModeAlt2, "PCM0DOUT"}, {Pin::ModeAlt3, "SIMVPPEN"}, {Pin::ModeAlt6, "PAEINT20"}}},
              {Pin::TypePower, { -1, -1, -1, 15, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {22, 21, 21, 16, 1}, {{Pin::ModeInput, "GPIOA21"}, {Pin::ModeOutput, "GPIOA21"}, {Pin::ModeAlt2, "PCM0DIN"}, {Pin::ModeAlt3, "SIMVPPPP"}, {Pin::ModeAlt6, "PAEINT21"}}},
              {Pin::TypeGpio, {26, 7, 7, 16, 2}, {{Pin::ModeInput, "GPIOA7"}, {Pin::ModeOutput, "GPIOA7"}, {Pin::ModeAlt2, "SIMCLK"}, {Pin::ModeAlt6, "PAEINT7"}}},
              {Pin::TypeGpio, {23, 8, 8, 17, 1}, {{Pin::ModeInput, "GPIOA8"}, {Pin::ModeOutput, "GPIOA8"}, {Pin::ModeAlt2, "SIMDATA"}, {Pin::ModeAlt6, "PAEINT8"}}},
              {Pin::TypePower, { -1, -1, -1, 17, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {24, 16, 16, 18, 1}, {{Pin::ModeInput, "GPIOA16"}, {Pin::ModeOutput, "GPIOA16"}, {Pin::ModeAlt2, "SPI1MISO"}, {Pin::ModeAlt3, "UART3CTS"}, {Pin::ModeAlt6, "PAEINT16"}}},
              {Pin::TypeGpio, {27, 13, 13, 18, 2}, {{Pin::ModeInput, "GPIOA13"}, {Pin::ModeOutput, "GPIOA13"}, {Pin::ModeAlt2, "SPI1CS"}, {Pin::ModeAlt3, "UART3TX"}, {Pin::ModeAlt6, "PAEINT13"}}},
              {Pin::TypeGpio, {25, 9, 9, 19, 1}, {{Pin::ModeInput, "GPIOA9"}, {Pin::ModeOutput, "GPIOA9"}, {Pin::ModeAlt2, "SIMRST"}, {Pin::ModeAlt6, "PAEINT9"}}},
              {Pin::TypeGpio, {28, 15, 15, 19, 2}, {{Pin::ModeInput, "GPIOA15"}, {Pin::ModeOutput, "GPIOA15"}, {Pin::ModeAlt2, "SPI1MOSI"}, {Pin::ModeAlt3, "UART3RTS"}, {Pin::ModeAlt6, "PAEINT15"}}},
              {Pin::TypePower, { -1, -1, -1, 20, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {29, 14, 14, 20, 2}, {{Pin::ModeInput, "GPIOA14"}, {Pin::ModeOutput, "GPIOA14"}, {Pin::ModeAlt2, "SPI1CLK"}, {Pin::ModeAlt3, "UART3RX"}, {Pin::ModeAlt6, "PAEINT14"}}},
            }
          },
          {
            "dbg_uart", 2, 4, 1, silConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 1}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {17, 4, 4, 3, 1}, {{Pin::ModeInput, "GPIOA4"}, {Pin::ModeOutput, "GPIOA4"}, {Pin::ModeAlt2, "UART0TX"}, {Pin::ModeAlt6, "PAEINT4"}}},
              {Pin::TypeGpio, {18, 5, 5, 4, 1}, {{Pin::ModeInput, "GPIOA5"}, {Pin::ModeOutput, "GPIOA5"}, {Pin::ModeAlt2, "UART0RX"}, {Pin::ModeAlt3, "PWM0"}, {Pin::ModeAlt6, "PAEINT5"}}},
            }
          },
          {
            "inner", 3, 2, 1, silConnector,
            {
              {Pin::TypeGpio, {19, 10, 10, 1, 1}, {{Pin::ModeInput, "GPIOA10"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt2, "SIMDET"}, {Pin::ModeAlt6, "PAEINT10"}}},
              {Pin::TypeGpio, {32, 104, 362, 2, 1}, {{Pin::ModeInput, "GPIOL10"}, {Pin::ModeOutput, "PWR_LED"}, {Pin::ModeAlt2, "SPWM"}, {Pin::ModeAlt6, "PLEINT10"}}},
              {Pin::TypeGpio, {33, 97, 355, 3, 1}, {{Pin::ModeInput, "BUT_K1"}, {Pin::ModeOutput, "GPIOL3"}, {Pin::ModeAlt2, "SUARTRX"}, {Pin::ModeAlt6, "PLEINT3"}}},
              {Pin::TypeGpio, {20, 105, 363, 4, 1}, {{Pin::ModeInput, "IR_RX"}, {Pin::ModeOutput, "GPIOL11"}, {Pin::ModeAlt2, "SCIRRX"}, {Pin::ModeAlt6, "PLEINT11"}}},
            }
          },
        }
      }
    },
    // <--- NanoPi M1
    //----------------------------------------------------------------------------
    // GPIO ID 6
    // NanoPi M1 Plus --->
    {
      6,
      {
        "nanopim1plus",
        {
          // Connecteurs
          {
            "con1", 1, 20, 2, dilConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypePower, { -1, -1, -1, 1, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {8, 12, 12, 2, 1}, {{Pin::ModeInput, "GPIOA12"}, {Pin::ModeOutput, "GPIOA12"}, {Pin::ModeAlt2, "I2C0SDA"}, {Pin::ModeAlt3, "DIRX"}, {Pin::ModeAlt6, "PAEINT12"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 2}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {9, 11, 11, 3, 1}, {{Pin::ModeInput, "GPIOA11"}, {Pin::ModeOutput, "GPIOA11"}, {Pin::ModeAlt2, "I2C0SCK"}, {Pin::ModeAlt3, "DITX"}, {Pin::ModeAlt6, "PAEINT11"}}},
              {Pin::TypePower, { -1, -1, -1, 3, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {7, 91, 203, 4, 1}, {{Pin::ModeInput, "GPIOG11"}, {Pin::ModeOutput, "GPIOG11"}, {Pin::ModeAlt2, "PCM1CLK"}, {Pin::ModeAlt6, "PGEINT11"}}},
              {Pin::TypeGpio, {15, 86, 198, 4, 2}, {{Pin::ModeInput, "GPIOG6"}, {Pin::ModeOutput, "GPIOG6"}, {Pin::ModeAlt2, "UART1TX"}, {Pin::ModeAlt6, "PGEINT6"}}},
              {Pin::TypePower, { -1, -1, -1, 5, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {16, 87, 199, 5, 2}, {{Pin::ModeInput, "GPIOG7"}, {Pin::ModeOutput, "GPIOG7"}, {Pin::ModeAlt2, "UART1RX"}, {Pin::ModeAlt6, "PGEINT7"}}},
              {Pin::TypeGpio, {0, 0, 0, 6, 1}, {{Pin::ModeInput, "GPIOA0"}, {Pin::ModeOutput, "GPIOA0"}, {Pin::ModeAlt2, "UART2TX"}, {Pin::ModeAlt3, "JTAGMS"}, {Pin::ModeAlt6, "PAEINT0"}}},
              {Pin::TypeGpio, {1, 6, 6, 6, 2}, {{Pin::ModeInput, "GPIOA6"}, {Pin::ModeOutput, "GPIOA6"}, {Pin::ModeAlt2, "SIMPWREN"}, {Pin::ModeAlt3, "PWM1"}, {Pin::ModeAlt6, "PAEINT6"}}},
              {Pin::TypeGpio, {2, 2, 2, 7, 1}, {{Pin::ModeInput, "GPIOA2"}, {Pin::ModeOutput, "GPIOA2"}, {Pin::ModeAlt2, "UART2RTS"}, {Pin::ModeAlt3, "JTAGDO"}, {Pin::ModeAlt6, "PAEINT2"}}},
              {Pin::TypePower, { -1, -1, -1, 7, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {3, 3, 3, 8, 1}, {{Pin::ModeInput, "GPIOA3"}, {Pin::ModeOutput, "GPIOA3"}, {Pin::ModeAlt2, "UART2CTS"}, {Pin::ModeAlt3, "JTAGDI"}, {Pin::ModeAlt6, "PAEINT3"}}},
              {Pin::TypeGpio, {4, 88, 200, 8, 2}, {{Pin::ModeInput, "GPIOG8"}, {Pin::ModeOutput, "GPIOG8"}, {Pin::ModeAlt2, "UART1RTS"}, {Pin::ModeAlt6, "PGEINT8"}}},
              {Pin::TypePower, { -1, -1, -1, 9, 1}, {{Pin::ModeInput, "3.3V"}}},
              {Pin::TypeGpio, {5, 89, 201, 9, 2}, {{Pin::ModeInput, "GPIOG9"}, {Pin::ModeOutput, "GPIOG9"}, {Pin::ModeAlt2, "UART1CTS"}, {Pin::ModeAlt6, "PGEINT9"}}},
              {Pin::TypeGpio, {12, 22, 64, 10, 1}, {{Pin::ModeInput, "GPIOC0"}, {Pin::ModeOutput, "GPIOC0"}, {Pin::ModeAlt2, "NANDWE"}, {Pin::ModeAlt3, "SPI0MOSI"}}},
              {Pin::TypePower, { -1, -1, -1, 10, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {13, 23, 65, 11, 1}, {{Pin::ModeInput, "GPIOC1"}, {Pin::ModeOutput, "GPIOC1"}, {Pin::ModeAlt2, "NANDALE"}, {Pin::ModeAlt3, "SPI0MISO"}}},
              {Pin::TypeGpio, {6, 1, 1, 11, 2}, {{Pin::ModeInput, "GPIOA1"}, {Pin::ModeOutput, "GPIOA1"}, {Pin::ModeAlt2, "UART2RX"}, {Pin::ModeAlt3, "JTAGCK"}, {Pin::ModeAlt6, "PAEINT1"}}},
              {Pin::TypeGpio, {14, 24, 66, 12, 1}, {{Pin::ModeInput, "GPIOC2"}, {Pin::ModeOutput, "GPIOC2"}, {Pin::ModeAlt2, "NANDCLE"}, {Pin::ModeAlt3, "SPI0CLK"}}},
              {Pin::TypeGpio, {10, 25, 67, 12, 2}, {{Pin::ModeInput, "GPIOC3"}, {Pin::ModeOutput, "GPIOC3"}, {Pin::ModeAlt2, "NANDCE1"}, {Pin::ModeAlt3, "SPI0CS"}}},
              {Pin::TypePower, { -1, -1, -1, 13, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {11, 17, 17, 13, 2}, {{Pin::ModeInput, "GPIOA17"}, {Pin::ModeOutput, "GPIOA17"}, {Pin::ModeAlt2, "OWAOUT"}, {Pin::ModeAlt6, "PAEINT17"}}},
              {Pin::TypeGpio, {30, 19, 19, 14, 1}, {{Pin::ModeInput, "GPIOA19"}, {Pin::ModeOutput, "GPIOA19"}, {Pin::ModeAlt2, "PCM0CLK"}, {Pin::ModeAlt3, "I2C1SDA"}, {Pin::ModeAlt6, "PAEINT19"}}},
              {Pin::TypeGpio, {31, 18, 18, 14, 2}, {{Pin::ModeInput, "GPIOA18"}, {Pin::ModeOutput, "GPIOA18"}, {Pin::ModeAlt2, "PCM0SYNC"}, {Pin::ModeAlt3, "I2C1SCK"}, {Pin::ModeAlt6, "PAEINT18"}}},
              {Pin::TypeGpio, {21, 20, 20, 15, 1}, {{Pin::ModeInput, "GPIOA20"}, {Pin::ModeOutput, "GPIOA20"}, {Pin::ModeAlt2, "PCM0DOUT"}, {Pin::ModeAlt3, "SIMVPPEN"}, {Pin::ModeAlt6, "PAEINT20"}}},
              {Pin::TypePower, { -1, -1, -1, 15, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeGpio, {22, 21, 21, 16, 1}, {{Pin::ModeInput, "GPIOA21"}, {Pin::ModeOutput, "GPIOA21"}, {Pin::ModeAlt2, "PCM0DIN"}, {Pin::ModeAlt3, "SIMVPPPP"}, {Pin::ModeAlt6, "PAEINT21"}}},
              {Pin::TypeNotConnected, { -1, -1, -1, 16, 2}, {{Pin::ModeInput, "NC"}}},
              {Pin::TypeNotConnected, { -1, -1, -1, 17, 1}, {{Pin::ModeInput, "NC"}}},
              {Pin::TypePower, { -1, -1, -1, 17, 2}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeNotConnected, { -1, -1, -1, 18, 1}, {{Pin::ModeInput, "NC"}}},
              {Pin::TypeNotConnected, { -1, -1, -1, 18, 2}, {{Pin::ModeInput, "NC"}}},
              {Pin::TypeGpio, {25, 9, 9, 19, 1}, {{Pin::ModeInput, "GPIOA9"}, {Pin::ModeOutput, "GPIOA9"}, {Pin::ModeAlt2, "SIMRST"}, {Pin::ModeAlt6, "PAEINT9"}}},
              {Pin::TypeNotConnected, { -1, -1, -1, 19, 2}, {{Pin::ModeInput, "NC"}}},
              {Pin::TypePower, { -1, -1, -1, 20, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypeNotConnected, { -1, -1, -1, 20, 2}, {{Pin::ModeInput, "NC"}}},
            }
          },
          {
            "dbg_uart", 2, 4, 1, silConnector,
            {
              {Pin::TypePower, { -1, -1, -1, 1, 1}, {{Pin::ModeInput, "GND"}}},
              {Pin::TypePower, { -1, -1, -1, 2, 1}, {{Pin::ModeInput, "5V"}}},
              {Pin::TypeGpio, {17, 4, 4, 3, 1}, {{Pin::ModeInput, "GPIOA4"}, {Pin::ModeOutput, "GPIOA4"}, {Pin::ModeAlt2, "UART0TX"}, {Pin::ModeAlt6, "PAEINT4"}}},
              {Pin::TypeGpio, {18, 5, 5, 4, 1}, {{Pin::ModeInput, "GPIOA5"}, {Pin::ModeOutput, "GPIOA5"}, {Pin::ModeAlt2, "UART0RX"}, {Pin::ModeAlt3, "PWM0"}, {Pin::ModeAlt6, "PAEINT5"}}},
            }
          },
          {
            "inner", 3, 2, 1, silConnector,
            {
              {Pin::TypeGpio, {19, 10, 10, 1, 1}, {{Pin::ModeInput, "GPIOA10"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt2, "SIMDET"}, {Pin::ModeAlt6, "PAEINT10"}}},
              {Pin::TypeGpio, {32, 104, 362, 2, 1}, {{Pin::ModeInput, "GPIOL10"}, {Pin::ModeOutput, "PWR_LED"}, {Pin::ModeAlt2, "SPWM"}, {Pin::ModeAlt6, "PLEINT10"}}},
              {Pin::TypeGpio, {33, 97, 355, 3, 1}, {{Pin::ModeInput, "BUT_K1"}, {Pin::ModeOutput, "GPIOL3"}, {Pin::ModeAlt2, "SUARTRX"}, {Pin::ModeAlt6, "PLEINT3"}}},
              {Pin::TypeGpio, {20, 105, 363, 4, 1}, {{Pin::ModeInput, "IR_RX"}, {Pin::ModeOutput, "GPIOL11"}, {Pin::ModeAlt2, "SCIRRX"}, {Pin::ModeAlt6, "PLEINT11"}}},
            }
          },
        }
      }
    },
    // <--- NanoPi M1 Plus
    //----------------------------------------------------------------------------
  };
}
/* ========================================================================== */
