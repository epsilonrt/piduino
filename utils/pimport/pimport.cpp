// pimport

// You should replace its source code with yours and explain what it does here:
// This program flashes an LED connected to a GPIO pin with a frequency of 2 Hz.

#include <iostream>
#include <piduino/clock.h>
#include <piduino/gpio.h>

using namespace std;
using namespace Piduino;


// -----------------------------------------------------------------------------
map<int, Gpio::Descriptor>
gpioDescriptors = {
  //----------------------------------------------------------------------------
  // GPIO Rev. 1
  // Model B PCB V1.0, Q3 2012, Board Rev <= 3 --->
  {
    1,
    {
      "rpi-rev1", 1,
      {
        // Connecteurs
        {
          "p1", 1, 13, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {8, 0, 0, 2, 1}, -1, {{Pin::ModeInput, "GPIO0"}, {Pin::ModeOutput, "GPIO0"}, {Pin::ModeAlt0, "SDA0"}, {Pin::ModeAlt1, "SA5"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {9, 1, 1, 3, 1}, -1, {{Pin::ModeInput, "GPIO1"}, {Pin::ModeOutput, "GPIO1"}, {Pin::ModeAlt0, "SCL0"}, {Pin::ModeAlt1, "SA4"}}},
            {Pin::TypePower, { -1, -1, -1, 3, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {7, 4, 4, 4, 1}, -1, {{Pin::ModeInput, "GPIO4"}, {Pin::ModeOutput, "GPIO4"}, {Pin::ModeAlt0, "GPCLK0"}, {Pin::ModeAlt1, "SA1"}, {Pin::ModeAlt5, "ARMTDI"}}},
            {Pin::TypeGpio, {15, 14, 14, 4, 2}, -1, {{Pin::ModeInput, "GPIO14"}, {Pin::ModeOutput, "GPIO14"}, {Pin::ModeAlt0, "TXD0"}, {Pin::ModeAlt1, "SD6"}, {Pin::ModeAlt5, "TXD1"}}},
            {Pin::TypePower, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {16, 15, 15, 5, 2}, -1, {{Pin::ModeInput, "GPIO15"}, {Pin::ModeOutput, "GPIO15"}, {Pin::ModeAlt0, "RXD0"}, {Pin::ModeAlt1, "SD7"}, {Pin::ModeAlt5, "RXD1"}}},
            {Pin::TypeGpio, {0, 17, 17, 6, 1}, -1, {{Pin::ModeInput, "GPIO17"}, {Pin::ModeOutput, "GPIO17"}, {Pin::ModeAlt1, "SD9"}, {Pin::ModeAlt3, "RTS0"}, {Pin::ModeAlt4, "SPI1CE1"}, {Pin::ModeAlt5, "RTS1"}}},
            {Pin::TypeGpio, {1, 18, 18, 6, 2}, -1, {{Pin::ModeInput, "GPIO18"}, {Pin::ModeOutput, "GPIO18"}, {Pin::ModeAlt0, "PCMCLK"}, {Pin::ModeAlt1, "SD10"}, {Pin::ModeAlt3, "BSMOSI"}, {Pin::ModeAlt4, "SPI1CE0"}, {Pin::ModeAlt5, "PWM0"}}},
            {Pin::TypeGpio, {2, 21, 21, 7, 1}, -1, {{Pin::ModeInput, "GPIO21"}, {Pin::ModeOutput, "GPIO21"}, {Pin::ModeAlt0, "PCMDOUT"}, {Pin::ModeAlt1, "SD13"}, {Pin::ModeAlt3, "BSCE"}, {Pin::ModeAlt4, "SPI1SCLK"}, {Pin::ModeAlt5, "GPCLK1"}}},
            {Pin::TypePower, { -1, -1, -1, 7, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {3, 22, 22, 8, 1}, -1, {{Pin::ModeInput, "GPIO22"}, {Pin::ModeOutput, "GPIO22"}, {Pin::ModeAlt1, "SD14"}, {Pin::ModeAlt3, "SD1CLK"}, {Pin::ModeAlt4, "ARMTRST"}}},
            {Pin::TypeGpio, {4, 23, 23, 8, 2}, -1, {{Pin::ModeInput, "GPIO23"}, {Pin::ModeOutput, "GPIO23"}, {Pin::ModeAlt1, "SD15"}, {Pin::ModeAlt3, "SD1CMD"}, {Pin::ModeAlt4, "ARMRTCK"}}},
            {Pin::TypePower, { -1, -1, -1, 9, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {5, 24, 24, 9, 2}, -1, {{Pin::ModeInput, "GPIO24"}, {Pin::ModeOutput, "GPIO24"}, {Pin::ModeAlt1, "SD16"}, {Pin::ModeAlt3, "SD1DAT0"}, {Pin::ModeAlt4, "ARMTDO"}}},
            {Pin::TypeGpio, {12, 10, 10, 10, 1}, -1, {{Pin::ModeInput, "GPIO10"}, {Pin::ModeOutput, "GPIO10"}, {Pin::ModeAlt0, "SPI0MOSI"}, {Pin::ModeAlt1, "SD2"}}},
            {Pin::TypePower, { -1, -1, -1, 10, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {13, 9, 9, 11, 1}, -1, {{Pin::ModeInput, "GPIO9"}, {Pin::ModeOutput, "GPIO9"}, {Pin::ModeAlt0, "SPI0MISO"}, {Pin::ModeAlt1, "SD1"}}},
            {Pin::TypeGpio, {6, 25, 25, 11, 2}, -1, {{Pin::ModeInput, "GPIO25"}, {Pin::ModeOutput, "GPIO25"}, {Pin::ModeAlt1, "SD17"}, {Pin::ModeAlt3, "SD1DAT1"}, {Pin::ModeAlt4, "ARMTCK"}}},
            {Pin::TypeGpio, {14, 11, 11, 12, 1}, -1, {{Pin::ModeInput, "GPIO11"}, {Pin::ModeOutput, "GPIO11"}, {Pin::ModeAlt0, "SPI0SCLK"}, {Pin::ModeAlt1, "SD3"}}},
            {Pin::TypeGpio, {10, 8, 8, 12, 2}, -1, {{Pin::ModeInput, "GPIO8"}, {Pin::ModeOutput, "GPIO8"}, {Pin::ModeAlt0, "SPI0CE0"}, {Pin::ModeAlt1, "SD0"}}},
            {Pin::TypePower, { -1, -1, -1, 13, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {11, 7, 7, 13, 2}, -1, {{Pin::ModeInput, "GPIO7"}, {Pin::ModeOutput, "GPIO7"}, {Pin::ModeAlt0, "SPI0CE1"}, {Pin::ModeAlt1, "SWE"}}},
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
      "rpi-rev2", 2,
      {
        // Connecteurs
        {
          "p1", 1, 13, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {8, 2, 2, 2, 1}, -1, {{Pin::ModeInput, "GPIO2"}, {Pin::ModeOutput, "GPIO2"}, {Pin::ModeAlt0, "SDA1"}, {Pin::ModeAlt1, "SA3"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {9, 3, 3, 3, 1}, -1, {{Pin::ModeInput, "GPIO3"}, {Pin::ModeOutput, "GPIO3"}, {Pin::ModeAlt0, "SCL1"}, {Pin::ModeAlt1, "SA2"}}},
            {Pin::TypePower, { -1, -1, -1, 3, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {7, 4, 4, 4, 1}, -1, {{Pin::ModeInput, "GPIO4"}, {Pin::ModeOutput, "GPIO4"}, {Pin::ModeAlt0, "GPCLK0"}, {Pin::ModeAlt1, "SA1"}, {Pin::ModeAlt5, "ARMTDI"}}},
            {Pin::TypeGpio, {15, 14, 14, 4, 2}, -1, {{Pin::ModeInput, "GPIO14"}, {Pin::ModeOutput, "GPIO14"}, {Pin::ModeAlt0, "TXD0"}, {Pin::ModeAlt1, "SD6"}, {Pin::ModeAlt5, "TXD1"}}},
            {Pin::TypePower, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {16, 15, 15, 5, 2}, -1, {{Pin::ModeInput, "GPIO15"}, {Pin::ModeOutput, "GPIO15"}, {Pin::ModeAlt0, "RXD0"}, {Pin::ModeAlt1, "SD7"}, {Pin::ModeAlt5, "RXD1"}}},
            {Pin::TypeGpio, {0, 17, 17, 6, 1}, -1, {{Pin::ModeInput, "GPIO17"}, {Pin::ModeOutput, "GPIO17"}, {Pin::ModeAlt1, "SD9"}, {Pin::ModeAlt3, "RTS0"}, {Pin::ModeAlt4, "SPI1CE1"}, {Pin::ModeAlt5, "RTS1"}}},
            {Pin::TypeGpio, {1, 18, 18, 6, 2}, -1, {{Pin::ModeInput, "GPIO18"}, {Pin::ModeOutput, "GPIO18"}, {Pin::ModeAlt0, "PCMCLK"}, {Pin::ModeAlt1, "SD10"}, {Pin::ModeAlt3, "BSMOSI"}, {Pin::ModeAlt4, "SPI1CE0"}, {Pin::ModeAlt5, "PWM0"}}},
            {Pin::TypeGpio, {2, 27, 27, 7, 1}, -1, {{Pin::ModeInput, "GPIO27"}, {Pin::ModeOutput, "GPIO27"}, {Pin::ModeAlt3, "SD1DAT3"}, {Pin::ModeAlt4, "ARMTMS"}}},
            {Pin::TypePower, { -1, -1, -1, 7, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {3, 22, 22, 8, 1}, -1, {{Pin::ModeInput, "GPIO22"}, {Pin::ModeOutput, "GPIO22"}, {Pin::ModeAlt1, "SD14"}, {Pin::ModeAlt3, "SD1CLK"}, {Pin::ModeAlt4, "ARMTRST"}}},
            {Pin::TypeGpio, {4, 23, 23, 8, 2}, -1, {{Pin::ModeInput, "GPIO23"}, {Pin::ModeOutput, "GPIO23"}, {Pin::ModeAlt1, "SD15"}, {Pin::ModeAlt3, "SD1CMD"}, {Pin::ModeAlt4, "ARMRTCK"}}},
            {Pin::TypePower, { -1, -1, -1, 9, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {5, 24, 24, 9, 2}, -1, {{Pin::ModeInput, "GPIO24"}, {Pin::ModeOutput, "GPIO24"}, {Pin::ModeAlt1, "SD16"}, {Pin::ModeAlt3, "SD1DAT0"}, {Pin::ModeAlt4, "ARMTDO"}}},
            {Pin::TypeGpio, {12, 10, 10, 10, 1}, -1, {{Pin::ModeInput, "GPIO10"}, {Pin::ModeOutput, "GPIO10"}, {Pin::ModeAlt0, "SPI0MOSI"}, {Pin::ModeAlt1, "SD2"}}},
            {Pin::TypePower, { -1, -1, -1, 10, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {13, 9, 9, 11, 1}, -1, {{Pin::ModeInput, "GPIO9"}, {Pin::ModeOutput, "GPIO9"}, {Pin::ModeAlt0, "SPI0MISO"}, {Pin::ModeAlt1, "SD1"}}},
            {Pin::TypeGpio, {6, 25, 25, 11, 2}, -1, {{Pin::ModeInput, "GPIO25"}, {Pin::ModeOutput, "GPIO25"}, {Pin::ModeAlt1, "SD17"}, {Pin::ModeAlt3, "SD1DAT1"}, {Pin::ModeAlt4, "ARMTCK"}}},
            {Pin::TypeGpio, {14, 11, 11, 12, 1}, -1, {{Pin::ModeInput, "GPIO11"}, {Pin::ModeOutput, "GPIO11"}, {Pin::ModeAlt0, "SPI0SCLK"}, {Pin::ModeAlt1, "SD3"}}},
            {Pin::TypeGpio, {10, 8, 8, 12, 2}, -1, {{Pin::ModeInput, "GPIO8"}, {Pin::ModeOutput, "GPIO8"}, {Pin::ModeAlt0, "SPI0CE0"}, {Pin::ModeAlt1, "SD0"}}},
            {Pin::TypePower, { -1, -1, -1, 13, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {11, 7, 7, 13, 2}, -1, {{Pin::ModeInput, "GPIO7"}, {Pin::ModeOutput, "GPIO7"}, {Pin::ModeAlt0, "SPI0CE1"}, {Pin::ModeAlt1, "SWE"}}},
          }
        },
        {
          "p5", 2, 4, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {17, 28, 28, 2, 1}, -1, {{Pin::ModeInput, "GPIO28"}, {Pin::ModeOutput, "GPIO28"}}},
            {Pin::TypeGpio, {18, 29, 29, 2, 2}, -1, {{Pin::ModeInput, "GPIO29"}, {Pin::ModeOutput, "GPIO29"}}},
            {Pin::TypeGpio, {19, 30, 30, 3, 1}, -1, {{Pin::ModeInput, "GPIO30"}, {Pin::ModeOutput, "GPIO30"}}},
            {Pin::TypeGpio, {20, 31, 31, 3, 2}, -1, {{Pin::ModeInput, "GPIO31"}, {Pin::ModeOutput, "GPIO31"}}},
            {Pin::TypePower, { -1, -1, -1, 4, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypePower, { -1, -1, -1, 4, 2}, -1, {{Pin::ModeInput, "GND"}}},
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
      "rpi-rev3", 3,
      {
        // Connecteurs
        {
          "j8", 1, 20, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {8, 2, 2, 2, 1}, -1, {{Pin::ModeInput, "GPIO2"}, {Pin::ModeOutput, "GPIO2"}, {Pin::ModeAlt0, "SDA1"}, {Pin::ModeAlt1, "SA3"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {9, 3, 3, 3, 1}, -1, {{Pin::ModeInput, "GPIO3"}, {Pin::ModeOutput, "GPIO3"}, {Pin::ModeAlt0, "SCL1"}, {Pin::ModeAlt1, "SA2"}}},
            {Pin::TypePower, { -1, -1, -1, 3, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {7, 4, 4, 4, 1}, -1, {{Pin::ModeInput, "GPIO4"}, {Pin::ModeOutput, "GPIO4"}, {Pin::ModeAlt0, "GPCLK0"}, {Pin::ModeAlt1, "SA1"}, {Pin::ModeAlt5, "ARMTDI"}}},
            {Pin::TypeGpio, {15, 14, 14, 4, 2}, -1, {{Pin::ModeInput, "GPIO14"}, {Pin::ModeOutput, "GPIO14"}, {Pin::ModeAlt0, "TXD0"}, {Pin::ModeAlt1, "SD6"}, {Pin::ModeAlt5, "TXD1"}}},
            {Pin::TypePower, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {16, 15, 15, 5, 2}, -1, {{Pin::ModeInput, "GPIO15"}, {Pin::ModeOutput, "GPIO15"}, {Pin::ModeAlt0, "RXD0"}, {Pin::ModeAlt1, "SD7"}, {Pin::ModeAlt5, "RXD1"}}},
            {Pin::TypeGpio, {0, 17, 17, 6, 1}, -1, {{Pin::ModeInput, "GPIO17"}, {Pin::ModeOutput, "GPIO17"}, {Pin::ModeAlt1, "SD9"}, {Pin::ModeAlt3, "RTS0"}, {Pin::ModeAlt4, "SPI1CE1"}, {Pin::ModeAlt5, "RTS1"}}},
            {Pin::TypeGpio, {1, 18, 18, 6, 2}, -1, {{Pin::ModeInput, "GPIO18"}, {Pin::ModeOutput, "GPIO18"}, {Pin::ModeAlt0, "PCMCLK"}, {Pin::ModeAlt1, "SD10"}, {Pin::ModeAlt3, "BSMOSI"}, {Pin::ModeAlt4, "SPI1CE0"}, {Pin::ModeAlt5, "PWM0"}}},
            {Pin::TypeGpio, {2, 27, 27, 7, 1}, -1, {{Pin::ModeInput, "GPIO27"}, {Pin::ModeOutput, "GPIO27"}, {Pin::ModeAlt3, "SD1DAT3"}, {Pin::ModeAlt4, "ARMTMS"}}},
            {Pin::TypePower, { -1, -1, -1, 7, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {3, 22, 22, 8, 1}, -1, {{Pin::ModeInput, "GPIO22"}, {Pin::ModeOutput, "GPIO22"}, {Pin::ModeAlt1, "SD14"}, {Pin::ModeAlt3, "SD1CLK"}, {Pin::ModeAlt4, "ARMTRST"}}},
            {Pin::TypeGpio, {4, 23, 23, 8, 2}, -1, {{Pin::ModeInput, "GPIO23"}, {Pin::ModeOutput, "GPIO23"}, {Pin::ModeAlt1, "SD15"}, {Pin::ModeAlt3, "SD1CMD"}, {Pin::ModeAlt4, "ARMRTCK"}}},
            {Pin::TypePower, { -1, -1, -1, 9, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {5, 24, 24, 9, 2}, -1, {{Pin::ModeInput, "GPIO24"}, {Pin::ModeOutput, "GPIO24"}, {Pin::ModeAlt1, "SD16"}, {Pin::ModeAlt3, "SD1DAT0"}, {Pin::ModeAlt4, "ARMTDO"}}},
            {Pin::TypeGpio, {12, 10, 10, 10, 1}, -1, {{Pin::ModeInput, "GPIO10"}, {Pin::ModeOutput, "GPIO10"}, {Pin::ModeAlt0, "SPI0MOSI"}, {Pin::ModeAlt1, "SD2"}}},
            {Pin::TypePower, { -1, -1, -1, 10, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {13, 9, 9, 11, 1}, -1, {{Pin::ModeInput, "GPIO9"}, {Pin::ModeOutput, "GPIO9"}, {Pin::ModeAlt0, "SPI0MISO"}, {Pin::ModeAlt1, "SD1"}}},
            {Pin::TypeGpio, {6, 25, 25, 11, 2}, -1, {{Pin::ModeInput, "GPIO25"}, {Pin::ModeOutput, "GPIO25"}, {Pin::ModeAlt1, "SD17"}, {Pin::ModeAlt3, "SD1DAT1"}, {Pin::ModeAlt4, "ARMTCK"}}},
            {Pin::TypeGpio, {14, 11, 11, 12, 1}, -1, {{Pin::ModeInput, "GPIO11"}, {Pin::ModeOutput, "GPIO11"}, {Pin::ModeAlt0, "SPI0SCLK"}, {Pin::ModeAlt1, "SD3"}}},
            {Pin::TypeGpio, {10, 8, 8, 12, 2}, -1, {{Pin::ModeInput, "GPIO8"}, {Pin::ModeOutput, "GPIO8"}, {Pin::ModeAlt0, "SPI0CE0"}, {Pin::ModeAlt1, "SD0"}}},
            {Pin::TypePower, { -1, -1, -1, 13, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {11, 7, 7, 13, 2}, -1, {{Pin::ModeInput, "GPIO7"}, {Pin::ModeOutput, "GPIO7"}, {Pin::ModeAlt0, "SPI0CE1"}, {Pin::ModeAlt1, "SWE"}}},
            {Pin::TypeGpio, {30, 0, 0, 14, 1}, -1, {{Pin::ModeInput, "GPIO0"}, {Pin::ModeOutput, "GPIO0"}, {Pin::ModeAlt0, "SDA0"}, {Pin::ModeAlt1, "SA5"}}},
            {Pin::TypeGpio, {31, 1, 1, 14, 2}, -1, {{Pin::ModeInput, "GPIO1"}, {Pin::ModeOutput, "GPIO1"}, {Pin::ModeAlt0, "SCL0"}, {Pin::ModeAlt1, "SA4"}}},
            {Pin::TypeGpio, {21, 5, 5, 15, 1}, -1, {{Pin::ModeInput, "GPIO5"}, {Pin::ModeOutput, "GPIO5"}, {Pin::ModeAlt0, "GPCLK1"}, {Pin::ModeAlt1, "SA0"}, {Pin::ModeAlt5, "ARMTDO"}}},
            {Pin::TypePower, { -1, -1, -1, 15, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {22, 6, 6, 16, 1}, -1, {{Pin::ModeInput, "GPIO6"}, {Pin::ModeOutput, "LAN_RUN"}, {Pin::ModeAlt0, "GPCLK2"}, {Pin::ModeAlt1, "SOE"}, {Pin::ModeAlt5, "ARMRTCK"}}},
            {Pin::TypeGpio, {26, 12, 12, 16, 2}, -1, {{Pin::ModeInput, "GPIO12"}, {Pin::ModeOutput, "GPIO12"}, {Pin::ModeAlt0, "PWM0"}, {Pin::ModeAlt1, "SD4"}, {Pin::ModeAlt5, "ARMTMS"}}},
            {Pin::TypeGpio, {23, 13, 13, 17, 1}, -1, {{Pin::ModeInput, "GPIO13"}, {Pin::ModeOutput, "GPIO13"}, {Pin::ModeAlt0, "PWM1"}, {Pin::ModeAlt1, "SD5"}, {Pin::ModeAlt5, "ARMTCK"}}},
            {Pin::TypePower, { -1, -1, -1, 17, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {24, 19, 19, 18, 1}, -1, {{Pin::ModeInput, "GPIO19"}, {Pin::ModeOutput, "GPIO19"}, {Pin::ModeAlt0, "PCMFS"}, {Pin::ModeAlt1, "SD11"}, {Pin::ModeAlt3, "BSSCLK"}, {Pin::ModeAlt4, "SPI1MISO"}, {Pin::ModeAlt5, "PWM1"}}},
            {Pin::TypeGpio, {27, 16, 16, 18, 2}, -1, {{Pin::ModeInput, "GPIO16"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt1, "SD8"}, {Pin::ModeAlt3, "CTS0"}, {Pin::ModeAlt4, "SPI1CE2"}, {Pin::ModeAlt5, "CTS1"}}},
            {Pin::TypeGpio, {25, 26, 26, 19, 1}, -1, {{Pin::ModeInput, "GPIO26"}, {Pin::ModeOutput, "GPIO26"}, {Pin::ModeAlt3, "SD1DAT2"}, {Pin::ModeAlt4, "ARMTDI"}}},
            {Pin::TypeGpio, {28, 20, 20, 19, 2}, -1, {{Pin::ModeInput, "GPIO20"}, {Pin::ModeOutput, "GPIO20"}, {Pin::ModeAlt0, "PCMDIN"}, {Pin::ModeAlt1, "SD12"}, {Pin::ModeAlt3, "BSMISO"}, {Pin::ModeAlt4, "SPI1MOSI"}, {Pin::ModeAlt5, "GPCLK0"}}},
            {Pin::TypePower, { -1, -1, -1, 20, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {29, 21, 21, 20, 2}, -1, {{Pin::ModeInput, "GPIO21"}, {Pin::ModeOutput, "GPIO21"}, {Pin::ModeAlt0, "PCMDOUT"}, {Pin::ModeAlt1, "SD13"}, {Pin::ModeAlt3, "BSCE"}, {Pin::ModeAlt4, "SPI1SCLK"}, {Pin::ModeAlt5, "GPCLK1"}}},
          }
        },
      }
    }
  },
  // <--- Modèle A+, B+, 2B, 3B, Zero
  //----------------------------------------------------------------------------
  // GPIO ID 4
  // NanoPi Neo, Neo2, Neo Plus2, Neo Air --->
  {
    4,
    {
      "nanopineo", 4,
      {
        // Connecteurs
        {
          "con1", 1, 12, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {8, 12, 12, 2, 1}, -1, {{Pin::ModeInput, "GPIOA12"}, {Pin::ModeOutput, "GPIOA12"}, {Pin::ModeAlt2, "I2C0SDA"}, {Pin::ModeAlt3, "DIRX"}, {Pin::ModeAlt6, "PAEINT12"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {9, 11, 11, 3, 1}, -1, {{Pin::ModeInput, "GPIOA11"}, {Pin::ModeOutput, "GPIOA11"}, {Pin::ModeAlt2, "I2C0SCK"}, {Pin::ModeAlt3, "DITX"}, {Pin::ModeAlt6, "PAEINT11"}}},
            {Pin::TypePower, { -1, -1, -1, 3, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {7, 91, 203, 4, 1}, -1, {{Pin::ModeInput, "GPIOG11"}, {Pin::ModeOutput, "GPIOG11"}, {Pin::ModeAlt2, "PCM1CLK"}, {Pin::ModeAlt6, "PGEINT11"}}},
            {Pin::TypeGpio, {15, 86, 198, 4, 2}, -1, {{Pin::ModeInput, "GPIOG6"}, {Pin::ModeOutput, "GPIOG6"}, {Pin::ModeAlt2, "UART1TX"}, {Pin::ModeAlt6, "PGEINT6"}}},
            {Pin::TypePower, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {16, 87, 199, 5, 2}, -1, {{Pin::ModeInput, "GPIOG7"}, {Pin::ModeOutput, "GPIOG7"}, {Pin::ModeAlt2, "UART1RX"}, {Pin::ModeAlt6, "PGEINT7"}}},
            {Pin::TypeGpio, {0, 0, 0, 6, 1}, -1, {{Pin::ModeInput, "GPIOA0"}, {Pin::ModeOutput, "GPIOA0"}, {Pin::ModeAlt2, "UART2TX"}, {Pin::ModeAlt3, "JTAGMS"}, {Pin::ModeAlt6, "PAEINT0"}}},
            {Pin::TypeGpio, {1, 6, 6, 6, 2}, -1, {{Pin::ModeInput, "GPIOA6"}, {Pin::ModeOutput, "GPIOA6"}, {Pin::ModeAlt2, "SIMPWREN"}, {Pin::ModeAlt3, "PWM1"}, {Pin::ModeAlt6, "PAEINT6"}}},
            {Pin::TypeGpio, {2, 2, 2, 7, 1}, -1, {{Pin::ModeInput, "GPIOA2"}, {Pin::ModeOutput, "GPIOA2"}, {Pin::ModeAlt2, "UART2RTS"}, {Pin::ModeAlt3, "JTAGDO"}, {Pin::ModeAlt6, "PAEINT2"}}},
            {Pin::TypePower, { -1, -1, -1, 7, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {3, 3, 3, 8, 1}, -1, {{Pin::ModeInput, "GPIOA3"}, {Pin::ModeOutput, "GPIOA3"}, {Pin::ModeAlt2, "UART2CTS"}, {Pin::ModeAlt3, "JTAGDI"}, {Pin::ModeAlt6, "PAEINT3"}}},
            {Pin::TypeGpio, {4, 88, 200, 8, 2}, -1, {{Pin::ModeInput, "GPIOG8"}, {Pin::ModeOutput, "GPIOG8"}, {Pin::ModeAlt2, "UART1RTS"}, {Pin::ModeAlt6, "PGEINT8"}}},
            {Pin::TypePower, { -1, -1, -1, 9, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {5, 89, 201, 9, 2}, -1, {{Pin::ModeInput, "GPIOG9"}, {Pin::ModeOutput, "GPIOG9"}, {Pin::ModeAlt2, "UART1CTS"}, {Pin::ModeAlt6, "PGEINT9"}}},
            {Pin::TypeGpio, {12, 22, 64, 10, 1}, -1, {{Pin::ModeInput, "GPIOC0"}, {Pin::ModeOutput, "GPIOC0"}, {Pin::ModeAlt2, "NANDWE"}, {Pin::ModeAlt3, "SPI0MOSI"}}},
            {Pin::TypePower, { -1, -1, -1, 10, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {13, 23, 65, 11, 1}, -1, {{Pin::ModeInput, "GPIOC1"}, {Pin::ModeOutput, "GPIOC1"}, {Pin::ModeAlt2, "NANDALE"}, {Pin::ModeAlt3, "SPI0MISO"}}},
            {Pin::TypeGpio, {6, 1, 1, 11, 2}, -1, {{Pin::ModeInput, "GPIOA1"}, {Pin::ModeOutput, "GPIOA1"}, {Pin::ModeAlt2, "UART2RX"}, {Pin::ModeAlt3, "JTAGCK"}, {Pin::ModeAlt6, "PAEINT1"}}},
            {Pin::TypeGpio, {14, 24, 66, 12, 1}, -1, {{Pin::ModeInput, "GPIOC2"}, {Pin::ModeOutput, "GPIOC2"}, {Pin::ModeAlt2, "NANDCLE"}, {Pin::ModeAlt3, "SPI0CLK"}}},
            {Pin::TypeGpio, {10, 25, 67, 12, 2}, -1, {{Pin::ModeInput, "GPIOC3"}, {Pin::ModeOutput, "GPIOC3"}, {Pin::ModeAlt2, "NANDCE1"}, {Pin::ModeAlt3, "SPI0CS"}}},
          }
        },
        {
          "dbg_uart", 2, 4, Connector::Family::Hex1, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 1}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {17, 4, 4, 3, 1}, -1, {{Pin::ModeInput, "GPIOA4"}, {Pin::ModeOutput, "GPIOA4"}, {Pin::ModeAlt2, "UART0TX"}, {Pin::ModeAlt6, "PAEINT4"}}},
            {Pin::TypeGpio, {18, 5, 5, 4, 1}, -1, {{Pin::ModeInput, "GPIOA5"}, {Pin::ModeOutput, "GPIOA5"}, {Pin::ModeAlt2, "UART0RX"}, {Pin::ModeAlt3, "PWM0"}, {Pin::ModeAlt6, "PAEINT5"}}},
          }
        },
        {
          "inner", 3, 2, Connector::Family::Hex1, -1,
          {
            {Pin::TypeGpio, {19, 10, 10, 1, 1}, -1, {{Pin::ModeInput, "GPIOA10"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt2, "SIMDET"}, {Pin::ModeAlt6, "PAEINT10"}}},
            {Pin::TypeGpio, {32, 104, 362, 2, 1}, -1, {{Pin::ModeInput, "GPIOL10"}, {Pin::ModeOutput, "PWR_LED"}, {Pin::ModeAlt2, "SPWM"}, {Pin::ModeAlt6, "PLEINT10"}}},
          }
        },
        {
          "con2", 4, 12, Connector::Family::Hex1, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeUsb, { -1, -1, -1, 2, 1}, -1, {{Pin::ModeInput, "USB-DP1"}}},
            {Pin::TypeUsb, { -1, -1, -1, 3, 1}, -1, {{Pin::ModeInput, "USB-DM1"}}},
            {Pin::TypeUsb, { -1, -1, -1, 4, 1}, -1, {{Pin::ModeInput, "USB-DP2"}}},
            {Pin::TypeUsb, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "USB-DM2"}}},
            {Pin::TypeGpio, {20, 105, 363, 6, 1}, -1, {{Pin::ModeInput, "GPIOL11"}, {Pin::ModeOutput, "GPIOL11"}, {Pin::ModeAlt2, "SCIRRX"}, {Pin::ModeAlt6, "PLEINT11"}}},
            {Pin::TypeGpio, {11, 17, 17, 7, 1}, -1, {{Pin::ModeInput, "GPIOA17"}, {Pin::ModeOutput, "GPIOA17"}, {Pin::ModeAlt2, "OWAOUT"}, {Pin::ModeAlt6, "PAEINT17"}}},
            {Pin::TypeGpio, {31, 18, 18, 8, 1}, -1, {{Pin::ModeInput, "GPIOA18"}, {Pin::ModeOutput, "GPIOA18"}, {Pin::ModeAlt2, "PCM0SYNC"}, {Pin::ModeAlt3, "I2C1SCK"}, {Pin::ModeAlt6, "PAEINT18"}}},
            {Pin::TypeGpio, {30, 19, 19, 9, 1}, -1, {{Pin::ModeInput, "GPIOA19"}, {Pin::ModeOutput, "GPIOA19"}, {Pin::ModeAlt2, "PCM0CLK"}, {Pin::ModeAlt3, "I2C1SDA"}, {Pin::ModeAlt6, "PAEINT19"}}},
            {Pin::TypeGpio, {21, 20, 20, 10, 1}, -1, {{Pin::ModeInput, "GPIOA20"}, {Pin::ModeOutput, "GPIOA20"}, {Pin::ModeAlt2, "PCM0DOUT"}, {Pin::ModeAlt3, "SIMVPPEN"}, {Pin::ModeAlt6, "PAEINT20"}}},
            {Pin::TypeGpio, {22, 21, 21, 11, 1}, -1, {{Pin::ModeInput, "GPIOA21"}, {Pin::ModeOutput, "GPIOA21"}, {Pin::ModeAlt2, "PCM0DIN"}, {Pin::ModeAlt3, "SIMVPPPP"}, {Pin::ModeAlt6, "PAEINT21"}}},
            {Pin::TypePower, { -1, -1, -1, 12, 1}, -1, {{Pin::ModeInput, "GND"}}},
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
      "nanopim1", 5,
      {
        // Connecteurs
        {
          "con1", 1, 20, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {8, 12, 12, 2, 1}, -1, {{Pin::ModeInput, "GPIOA12"}, {Pin::ModeOutput, "GPIOA12"}, {Pin::ModeAlt2, "I2C0SDA"}, {Pin::ModeAlt3, "DIRX"}, {Pin::ModeAlt6, "PAEINT12"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {9, 11, 11, 3, 1}, -1, {{Pin::ModeInput, "GPIOA11"}, {Pin::ModeOutput, "GPIOA11"}, {Pin::ModeAlt2, "I2C0SCK"}, {Pin::ModeAlt3, "DITX"}, {Pin::ModeAlt6, "PAEINT11"}}},
            {Pin::TypePower, { -1, -1, -1, 3, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {7, 91, 203, 4, 1}, -1, {{Pin::ModeInput, "GPIOG11"}, {Pin::ModeOutput, "GPIOG11"}, {Pin::ModeAlt2, "PCM1CLK"}, {Pin::ModeAlt6, "PGEINT11"}}},
            {Pin::TypeGpio, {15, 86, 198, 4, 2}, -1, {{Pin::ModeInput, "GPIOG6"}, {Pin::ModeOutput, "GPIOG6"}, {Pin::ModeAlt2, "UART1TX"}, {Pin::ModeAlt6, "PGEINT6"}}},
            {Pin::TypePower, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {16, 87, 199, 5, 2}, -1, {{Pin::ModeInput, "GPIOG7"}, {Pin::ModeOutput, "GPIOG7"}, {Pin::ModeAlt2, "UART1RX"}, {Pin::ModeAlt6, "PGEINT7"}}},
            {Pin::TypeGpio, {0, 0, 0, 6, 1}, -1, {{Pin::ModeInput, "GPIOA0"}, {Pin::ModeOutput, "GPIOA0"}, {Pin::ModeAlt2, "UART2TX"}, {Pin::ModeAlt3, "JTAGMS"}, {Pin::ModeAlt6, "PAEINT0"}}},
            {Pin::TypeGpio, {1, 6, 6, 6, 2}, -1, {{Pin::ModeInput, "GPIOA6"}, {Pin::ModeOutput, "GPIOA6"}, {Pin::ModeAlt2, "SIMPWREN"}, {Pin::ModeAlt3, "PWM1"}, {Pin::ModeAlt6, "PAEINT6"}}},
            {Pin::TypeGpio, {2, 2, 2, 7, 1}, -1, {{Pin::ModeInput, "GPIOA2"}, {Pin::ModeOutput, "GPIOA2"}, {Pin::ModeAlt2, "UART2RTS"}, {Pin::ModeAlt3, "JTAGDO"}, {Pin::ModeAlt6, "PAEINT2"}}},
            {Pin::TypePower, { -1, -1, -1, 7, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {3, 3, 3, 8, 1}, -1, {{Pin::ModeInput, "GPIOA3"}, {Pin::ModeOutput, "GPIOA3"}, {Pin::ModeAlt2, "UART2CTS"}, {Pin::ModeAlt3, "JTAGDI"}, {Pin::ModeAlt6, "PAEINT3"}}},
            {Pin::TypeGpio, {4, 88, 200, 8, 2}, -1, {{Pin::ModeInput, "GPIOG8"}, {Pin::ModeOutput, "GPIOG8"}, {Pin::ModeAlt2, "UART1RTS"}, {Pin::ModeAlt6, "PGEINT8"}}},
            {Pin::TypePower, { -1, -1, -1, 9, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {5, 89, 201, 9, 2}, -1, {{Pin::ModeInput, "GPIOG9"}, {Pin::ModeOutput, "GPIOG9"}, {Pin::ModeAlt2, "UART1CTS"}, {Pin::ModeAlt6, "PGEINT9"}}},
            {Pin::TypeGpio, {12, 22, 64, 10, 1}, -1, {{Pin::ModeInput, "GPIOC0"}, {Pin::ModeOutput, "GPIOC0"}, {Pin::ModeAlt2, "NANDWE"}, {Pin::ModeAlt3, "SPI0MOSI"}}},
            {Pin::TypePower, { -1, -1, -1, 10, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {13, 23, 65, 11, 1}, -1, {{Pin::ModeInput, "GPIOC1"}, {Pin::ModeOutput, "GPIOC1"}, {Pin::ModeAlt2, "NANDALE"}, {Pin::ModeAlt3, "SPI0MISO"}}},
            {Pin::TypeGpio, {6, 1, 1, 11, 2}, -1, {{Pin::ModeInput, "GPIOA1"}, {Pin::ModeOutput, "GPIOA1"}, {Pin::ModeAlt2, "UART2RX"}, {Pin::ModeAlt3, "JTAGCK"}, {Pin::ModeAlt6, "PAEINT1"}}},
            {Pin::TypeGpio, {14, 24, 66, 12, 1}, -1, {{Pin::ModeInput, "GPIOC2"}, {Pin::ModeOutput, "GPIOC2"}, {Pin::ModeAlt2, "NANDCLE"}, {Pin::ModeAlt3, "SPI0CLK"}}},
            {Pin::TypeGpio, {10, 25, 67, 12, 2}, -1, {{Pin::ModeInput, "GPIOC3"}, {Pin::ModeOutput, "GPIOC3"}, {Pin::ModeAlt2, "NANDCE1"}, {Pin::ModeAlt3, "SPI0CS"}}},
            {Pin::TypePower, { -1, -1, -1, 13, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {11, 17, 17, 13, 2}, -1, {{Pin::ModeInput, "GPIOA17"}, {Pin::ModeOutput, "GPIOA17"}, {Pin::ModeAlt2, "OWAOUT"}, {Pin::ModeAlt6, "PAEINT17"}}},
            {Pin::TypeGpio, {30, 19, 19, 14, 1}, -1, {{Pin::ModeInput, "GPIOA19"}, {Pin::ModeOutput, "GPIOA19"}, {Pin::ModeAlt2, "PCM0CLK"}, {Pin::ModeAlt3, "I2C1SDA"}, {Pin::ModeAlt6, "PAEINT19"}}},
            {Pin::TypeGpio, {31, 18, 18, 14, 2}, -1, {{Pin::ModeInput, "GPIOA18"}, {Pin::ModeOutput, "GPIOA18"}, {Pin::ModeAlt2, "PCM0SYNC"}, {Pin::ModeAlt3, "I2C1SCK"}, {Pin::ModeAlt6, "PAEINT18"}}},
            {Pin::TypeGpio, {21, 20, 20, 15, 1}, -1, {{Pin::ModeInput, "GPIOA20"}, {Pin::ModeOutput, "GPIOA20"}, {Pin::ModeAlt2, "PCM0DOUT"}, {Pin::ModeAlt3, "SIMVPPEN"}, {Pin::ModeAlt6, "PAEINT20"}}},
            {Pin::TypePower, { -1, -1, -1, 15, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {22, 21, 21, 16, 1}, -1, {{Pin::ModeInput, "GPIOA21"}, {Pin::ModeOutput, "GPIOA21"}, {Pin::ModeAlt2, "PCM0DIN"}, {Pin::ModeAlt3, "SIMVPPPP"}, {Pin::ModeAlt6, "PAEINT21"}}},
            {Pin::TypeGpio, {26, 7, 7, 16, 2}, -1, {{Pin::ModeInput, "GPIOA7"}, {Pin::ModeOutput, "GPIOA7"}, {Pin::ModeAlt2, "SIMCLK"}, {Pin::ModeAlt6, "PAEINT7"}}},
            {Pin::TypeGpio, {23, 8, 8, 17, 1}, -1, {{Pin::ModeInput, "GPIOA8"}, {Pin::ModeOutput, "GPIOA8"}, {Pin::ModeAlt2, "SIMDATA"}, {Pin::ModeAlt6, "PAEINT8"}}},
            {Pin::TypePower, { -1, -1, -1, 17, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {24, 16, 16, 18, 1}, -1, {{Pin::ModeInput, "GPIOA16"}, {Pin::ModeOutput, "GPIOA16"}, {Pin::ModeAlt2, "SPI1MISO"}, {Pin::ModeAlt3, "UART3CTS"}, {Pin::ModeAlt6, "PAEINT16"}}},
            {Pin::TypeGpio, {27, 13, 13, 18, 2}, -1, {{Pin::ModeInput, "GPIOA13"}, {Pin::ModeOutput, "GPIOA13"}, {Pin::ModeAlt2, "SPI1CS"}, {Pin::ModeAlt3, "UART3TX"}, {Pin::ModeAlt6, "PAEINT13"}}},
            {Pin::TypeGpio, {25, 9, 9, 19, 1}, -1, {{Pin::ModeInput, "GPIOA9"}, {Pin::ModeOutput, "GPIOA9"}, {Pin::ModeAlt2, "SIMRST"}, {Pin::ModeAlt6, "PAEINT9"}}},
            {Pin::TypeGpio, {28, 15, 15, 19, 2}, -1, {{Pin::ModeInput, "GPIOA15"}, {Pin::ModeOutput, "GPIOA15"}, {Pin::ModeAlt2, "SPI1MOSI"}, {Pin::ModeAlt3, "UART3RTS"}, {Pin::ModeAlt6, "PAEINT15"}}},
            {Pin::TypePower, { -1, -1, -1, 20, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {29, 14, 14, 20, 2}, -1, {{Pin::ModeInput, "GPIOA14"}, {Pin::ModeOutput, "GPIOA14"}, {Pin::ModeAlt2, "SPI1CLK"}, {Pin::ModeAlt3, "UART3RX"}, {Pin::ModeAlt6, "PAEINT14"}}},
          }
        },
        {
          "dbg_uart", 2, 4, Connector::Family::Hex1, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 1}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {17, 4, 4, 3, 1}, -1, {{Pin::ModeInput, "GPIOA4"}, {Pin::ModeOutput, "GPIOA4"}, {Pin::ModeAlt2, "UART0TX"}, {Pin::ModeAlt6, "PAEINT4"}}},
            {Pin::TypeGpio, {18, 5, 5, 4, 1}, -1, {{Pin::ModeInput, "GPIOA5"}, {Pin::ModeOutput, "GPIOA5"}, {Pin::ModeAlt2, "UART0RX"}, {Pin::ModeAlt3, "PWM0"}, {Pin::ModeAlt6, "PAEINT5"}}},
          }
        },
        {
          "inner", 3, 2, Connector::Family::Hex1, -1,
          {
            {Pin::TypeGpio, {19, 10, 10, 1, 1}, -1, {{Pin::ModeInput, "GPIOA10"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt2, "SIMDET"}, {Pin::ModeAlt6, "PAEINT10"}}},
            {Pin::TypeGpio, {32, 104, 362, 2, 1}, -1, {{Pin::ModeInput, "GPIOL10"}, {Pin::ModeOutput, "PWR_LED"}, {Pin::ModeAlt2, "SPWM"}, {Pin::ModeAlt6, "PLEINT10"}}},
            {Pin::TypeGpio, {33, 97, 355, 3, 1}, -1, {{Pin::ModeInput, "BUT_K1"}, {Pin::ModeOutput, "GPIOL3"}, {Pin::ModeAlt2, "SUARTRX"}, {Pin::ModeAlt6, "PLEINT3"}}},
            {Pin::TypeGpio, {20, 105, 363, 4, 1}, -1, {{Pin::ModeInput, "IR_RX"}, {Pin::ModeOutput, "GPIOL11"}, {Pin::ModeAlt2, "SCIRRX"}, {Pin::ModeAlt6, "PLEINT11"}}},
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
      "nanopim1plus", 6,
      {
        // Connecteurs
        {
          "con1", 1, 20, Connector::Family::Hex2, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypePower, { -1, -1, -1, 1, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {8, 12, 12, 2, 1}, -1, {{Pin::ModeInput, "GPIOA12"}, {Pin::ModeOutput, "GPIOA12"}, {Pin::ModeAlt2, "I2C0SDA"}, {Pin::ModeAlt3, "DIRX"}, {Pin::ModeAlt6, "PAEINT12"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 2}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {9, 11, 11, 3, 1}, -1, {{Pin::ModeInput, "GPIOA11"}, {Pin::ModeOutput, "GPIOA11"}, {Pin::ModeAlt2, "I2C0SCK"}, {Pin::ModeAlt3, "DITX"}, {Pin::ModeAlt6, "PAEINT11"}}},
            {Pin::TypePower, { -1, -1, -1, 3, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {7, 91, 203, 4, 1}, -1, {{Pin::ModeInput, "GPIOG11"}, {Pin::ModeOutput, "GPIOG11"}, {Pin::ModeAlt2, "PCM1CLK"}, {Pin::ModeAlt6, "PGEINT11"}}},
            {Pin::TypeGpio, {15, 86, 198, 4, 2}, -1, {{Pin::ModeInput, "GPIOG6"}, {Pin::ModeOutput, "GPIOG6"}, {Pin::ModeAlt2, "UART1TX"}, {Pin::ModeAlt6, "PGEINT6"}}},
            {Pin::TypePower, { -1, -1, -1, 5, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {16, 87, 199, 5, 2}, -1, {{Pin::ModeInput, "GPIOG7"}, {Pin::ModeOutput, "GPIOG7"}, {Pin::ModeAlt2, "UART1RX"}, {Pin::ModeAlt6, "PGEINT7"}}},
            {Pin::TypeGpio, {0, 0, 0, 6, 1}, -1, {{Pin::ModeInput, "GPIOA0"}, {Pin::ModeOutput, "GPIOA0"}, {Pin::ModeAlt2, "UART2TX"}, {Pin::ModeAlt3, "JTAGMS"}, {Pin::ModeAlt6, "PAEINT0"}}},
            {Pin::TypeGpio, {1, 6, 6, 6, 2}, -1, {{Pin::ModeInput, "GPIOA6"}, {Pin::ModeOutput, "GPIOA6"}, {Pin::ModeAlt2, "SIMPWREN"}, {Pin::ModeAlt3, "PWM1"}, {Pin::ModeAlt6, "PAEINT6"}}},
            {Pin::TypeGpio, {2, 2, 2, 7, 1}, -1, {{Pin::ModeInput, "GPIOA2"}, {Pin::ModeOutput, "GPIOA2"}, {Pin::ModeAlt2, "UART2RTS"}, {Pin::ModeAlt3, "JTAGDO"}, {Pin::ModeAlt6, "PAEINT2"}}},
            {Pin::TypePower, { -1, -1, -1, 7, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {3, 3, 3, 8, 1}, -1, {{Pin::ModeInput, "GPIOA3"}, {Pin::ModeOutput, "GPIOA3"}, {Pin::ModeAlt2, "UART2CTS"}, {Pin::ModeAlt3, "JTAGDI"}, {Pin::ModeAlt6, "PAEINT3"}}},
            {Pin::TypeGpio, {4, 88, 200, 8, 2}, -1, {{Pin::ModeInput, "GPIOG8"}, {Pin::ModeOutput, "GPIOG8"}, {Pin::ModeAlt2, "UART1RTS"}, {Pin::ModeAlt6, "PGEINT8"}}},
            {Pin::TypePower, { -1, -1, -1, 9, 1}, -1, {{Pin::ModeInput, "3.3V"}}},
            {Pin::TypeGpio, {5, 89, 201, 9, 2}, -1, {{Pin::ModeInput, "GPIOG9"}, {Pin::ModeOutput, "GPIOG9"}, {Pin::ModeAlt2, "UART1CTS"}, {Pin::ModeAlt6, "PGEINT9"}}},
            {Pin::TypeGpio, {12, 22, 64, 10, 1}, -1, {{Pin::ModeInput, "GPIOC0"}, {Pin::ModeOutput, "GPIOC0"}, {Pin::ModeAlt2, "NANDWE"}, {Pin::ModeAlt3, "SPI0MOSI"}}},
            {Pin::TypePower, { -1, -1, -1, 10, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {13, 23, 65, 11, 1}, -1, {{Pin::ModeInput, "GPIOC1"}, {Pin::ModeOutput, "GPIOC1"}, {Pin::ModeAlt2, "NANDALE"}, {Pin::ModeAlt3, "SPI0MISO"}}},
            {Pin::TypeGpio, {6, 1, 1, 11, 2}, -1, {{Pin::ModeInput, "GPIOA1"}, {Pin::ModeOutput, "GPIOA1"}, {Pin::ModeAlt2, "UART2RX"}, {Pin::ModeAlt3, "JTAGCK"}, {Pin::ModeAlt6, "PAEINT1"}}},
            {Pin::TypeGpio, {14, 24, 66, 12, 1}, -1, {{Pin::ModeInput, "GPIOC2"}, {Pin::ModeOutput, "GPIOC2"}, {Pin::ModeAlt2, "NANDCLE"}, {Pin::ModeAlt3, "SPI0CLK"}}},
            {Pin::TypeGpio, {10, 25, 67, 12, 2}, -1, {{Pin::ModeInput, "GPIOC3"}, {Pin::ModeOutput, "GPIOC3"}, {Pin::ModeAlt2, "NANDCE1"}, {Pin::ModeAlt3, "SPI0CS"}}},
            {Pin::TypePower, { -1, -1, -1, 13, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {11, 17, 17, 13, 2}, -1, {{Pin::ModeInput, "GPIOA17"}, {Pin::ModeOutput, "GPIOA17"}, {Pin::ModeAlt2, "OWAOUT"}, {Pin::ModeAlt6, "PAEINT17"}}},
            {Pin::TypeGpio, {30, 19, 19, 14, 1}, -1, {{Pin::ModeInput, "GPIOA19"}, {Pin::ModeOutput, "GPIOA19"}, {Pin::ModeAlt2, "PCM0CLK"}, {Pin::ModeAlt3, "I2C1SDA"}, {Pin::ModeAlt6, "PAEINT19"}}},
            {Pin::TypeGpio, {31, 18, 18, 14, 2}, -1, {{Pin::ModeInput, "GPIOA18"}, {Pin::ModeOutput, "GPIOA18"}, {Pin::ModeAlt2, "PCM0SYNC"}, {Pin::ModeAlt3, "I2C1SCK"}, {Pin::ModeAlt6, "PAEINT18"}}},
            {Pin::TypeGpio, {21, 20, 20, 15, 1}, -1, {{Pin::ModeInput, "GPIOA20"}, {Pin::ModeOutput, "GPIOA20"}, {Pin::ModeAlt2, "PCM0DOUT"}, {Pin::ModeAlt3, "SIMVPPEN"}, {Pin::ModeAlt6, "PAEINT20"}}},
            {Pin::TypePower, { -1, -1, -1, 15, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeGpio, {22, 21, 21, 16, 1}, -1, {{Pin::ModeInput, "GPIOA21"}, {Pin::ModeOutput, "GPIOA21"}, {Pin::ModeAlt2, "PCM0DIN"}, {Pin::ModeAlt3, "SIMVPPPP"}, {Pin::ModeAlt6, "PAEINT21"}}},
            {Pin::TypeNotConnected, { -1, -1, -1, 16, 2}, -1, {{Pin::ModeInput, "NC"}}},
            {Pin::TypeNotConnected, { -1, -1, -1, 17, 1}, -1, {{Pin::ModeInput, "NC"}}},
            {Pin::TypePower, { -1, -1, -1, 17, 2}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeNotConnected, { -1, -1, -1, 18, 1}, -1, {{Pin::ModeInput, "NC"}}},
            {Pin::TypeNotConnected, { -1, -1, -1, 18, 2}, -1, {{Pin::ModeInput, "NC"}}},
            {Pin::TypeGpio, {25, 9, 9, 19, 1}, -1, {{Pin::ModeInput, "GPIOA9"}, {Pin::ModeOutput, "GPIOA9"}, {Pin::ModeAlt2, "SIMRST"}, {Pin::ModeAlt6, "PAEINT9"}}},
            {Pin::TypeNotConnected, { -1, -1, -1, 19, 2}, -1, {{Pin::ModeInput, "NC"}}},
            {Pin::TypePower, { -1, -1, -1, 20, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypeNotConnected, { -1, -1, -1, 20, 2}, -1, {{Pin::ModeInput, "NC"}}},
          }
        },
        {
          "dbg_uart", 2, 4, Connector::Family::Hex1, -1,
          {
            {Pin::TypePower, { -1, -1, -1, 1, 1}, -1, {{Pin::ModeInput, "GND"}}},
            {Pin::TypePower, { -1, -1, -1, 2, 1}, -1, {{Pin::ModeInput, "5V"}}},
            {Pin::TypeGpio, {17, 4, 4, 3, 1}, -1, {{Pin::ModeInput, "GPIOA4"}, {Pin::ModeOutput, "GPIOA4"}, {Pin::ModeAlt2, "UART0TX"}, {Pin::ModeAlt6, "PAEINT4"}}},
            {Pin::TypeGpio, {18, 5, 5, 4, 1}, -1, {{Pin::ModeInput, "GPIOA5"}, {Pin::ModeOutput, "GPIOA5"}, {Pin::ModeAlt2, "UART0RX"}, {Pin::ModeAlt3, "PWM0"}, {Pin::ModeAlt6, "PAEINT5"}}},
          }
        },
        {
          "inner", 3, 2, Connector::Family::Hex1, -1,
          {
            {Pin::TypeGpio, {19, 10, 10, 1, 1}, -1, {{Pin::ModeInput, "GPIOA10"}, {Pin::ModeOutput, "STAT_LED"}, {Pin::ModeAlt2, "SIMDET"}, {Pin::ModeAlt6, "PAEINT10"}}},
            {Pin::TypeGpio, {32, 104, 362, 2, 1}, -1, {{Pin::ModeInput, "GPIOL10"}, {Pin::ModeOutput, "PWR_LED"}, {Pin::ModeAlt2, "SPWM"}, {Pin::ModeAlt6, "PLEINT10"}}},
            {Pin::TypeGpio, {33, 97, 355, 3, 1}, -1, {{Pin::ModeInput, "BUT_K1"}, {Pin::ModeOutput, "GPIOL3"}, {Pin::ModeAlt2, "SUARTRX"}, {Pin::ModeAlt6, "PLEINT3"}}},
            {Pin::TypeGpio, {20, 105, 363, 4, 1}, -1, {{Pin::ModeInput, "IR_RX"}, {Pin::ModeOutput, "GPIOL11"}, {Pin::ModeAlt2, "SCIRRX"}, {Pin::ModeAlt6, "PLEINT11"}}},
          }
        },
      }
    }
  },
  // <--- NanoPi M1 Plus
  //----------------------------------------------------------------------------
};

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {

  try {
    for (auto it = gpioDescriptors.begin(); it != gpioDescriptors.end(); ++it) {
      Gpio::Descriptor & d = it->second;

      cout << d.name << " GPIO processing in progress... " << flush;
      if (d.insert()) {
        cout << "inserted in database." << endl;
      }
      else {
        cout << "found in database (not inserted)." << endl;
      }
    }
  }
  catch (exception const &e) {
    cerr << "ERROR: " << e.what() << endl;
    return 1;
  }

  return 0;
}
/* ========================================================================== */
