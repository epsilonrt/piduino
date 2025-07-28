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

#include <piduino/global.h>

namespace Piduino {

  namespace Rp1 {

    // RP1 is connected to the PCIe bus, so we need to find the PCIe device
    std::string findPCIeDevice();
    bool isPCIeFileContain (const char *dirname, const char *filename, const char *content);

    // Constants for the RP1 SoC
    const uint32_t RegisterSize = sizeof (uint32_t); // Size of a register in bytes

    const size_t IoBaseAddr       = 0x40000000; // RP1 base address of the memory mapped I/O

    const size_t IoBankAddr       = 0x400d0000; // RP1 start address of the GPIO registers
    const size_t SysRio0BankAddr  = 0x400e0000; // RP1 start address of the RIO registers
    const size_t PadsBankAddr     = 0x400f0000; // RP1 start address of the pads registers
    const size_t EthBankAddr      = 0x40100000; // RP1 start address of the Ethernet registers
    const size_t GpioBlockSize   = (EthBankAddr - IoBankAddr); // Size of the GPIO block in bytes

    const off_t GpioOffset = (IoBankAddr - IoBaseAddr) / RegisterSize; // only for PCIe access, for gpiomem access, base address is 0
    const off_t PadsOffset = (PadsBankAddr - IoBankAddr) / RegisterSize;
    const off_t RioOffset  = (SysRio0BankAddr - IoBankAddr) / RegisterSize;

    // PCIe constants
    const size_t PCIeBlockSize  = 0x00400000;
    const char PCIeDevPath[]      = "/sys/bus/pci/devices";
    const char PCIeRp1VendorId[]  = "0x1de4";
    const char PCIeRp1DeviceId[]  = "0x0001";
    const char PCIeRp1File[]      = "resource1";

    // Function Select Register (FUNCSEL) values in GPIO_CTRL register
    enum {
      FselAlt0 = 0,  ///< Alternate function 0
      FselAlt1,      ///< Alternate function 1
      FselAlt2,      ///< Alternate function 2
      FselAlt3,      ///< Alternate function 3
      FselAlt4,      ///< Alternate function 4
      FselAlt5,      ///< Alternate function 5
      FselAlt6,      ///< Alternate function 6
      FselAlt7,      ///< Alternate function 7
      FselAlt8,      ///< Alternate function 8
      FselNone,      ///< No function
      FselGpio = FselAlt5, ///< GPIO function
      FselNoneHw = 0x1f ///< Hardware function (default, mask)
    };

    const uint32_t GPIO_RIO_OUT = 0x0000;
    const uint32_t GPIO_RIO_OE  = (0x0004 / RegisterSize);
    const uint32_t GPIO_RIO_IN  = (0x0008 / RegisterSize);

    const uint32_t GPIO_RIO_SET_OFFSET = (0x2000 / RegisterSize); // Atomic set operation
    const uint32_t GPIO_RIO_CLR_OFFSET = (0x3000 / RegisterSize); // Atomic clear operation
    const uint32_t GPIO_RIO_XOR_OFFSET = (0x1000 / RegisterSize); // Atomic toggle operation

    const uint32_t GPIO_PAD_PULL_LSB     = 2;
    const uint32_t GPIO_PAD_DRIVE_LSB    = 4;

    const uint32_t GPIO_PAD_SLEWFAST     = 0x00000001; // Contrôle du slew rate rapide
    const uint32_t GPIO_PAD_SCHMITT      = 0x00000002; // Activation du trigger Schmitt
    const uint32_t GPIO_PAD_PULL_OFF     = 0x00000000;
    const uint32_t GPIO_PAD_PULL_DOWN    = 0x00000004;
    const uint32_t GPIO_PAD_PULL_UP      = 0x00000008;
    const uint32_t GPIO_PAD_DRIVE_2MA    = 0x00000000;
    const uint32_t GPIO_PAD_DRIVE_4MA    = 0x00000010;
    const uint32_t GPIO_PAD_DRIVE_8MA    = 0x00000020;
    const uint32_t GPIO_PAD_DRIVE_12MA   = 0x00000030;
    const uint32_t GPIO_PAD_IN_ENABLE    = 0x00000040; // Activation de l'entrée
    const uint32_t GPIO_PAD_OUT_DISABLE  = 0x00000080; // Désactivation de la sortie

    const uint32_t GPIO_PAD_PULL_MASK    = GPIO_PAD_PULL_UP | GPIO_PAD_PULL_DOWN;
    const uint32_t GPIO_PAD_DRIVE_MASK   = 0x00000030;   // Force de drive (courant)

    const uint32_t GPIO_PAD_DEFAULT_0TO8  = (GPIO_PAD_IN_ENABLE   | GPIO_PAD_DRIVE_4MA | GPIO_PAD_PULL_UP   | GPIO_PAD_SCHMITT | GPIO_PAD_SLEWFAST); // 0x7b
    const uint32_t GPIO_PAD_DEFAULT_FROM9 = (GPIO_PAD_IN_ENABLE   | GPIO_PAD_DRIVE_4MA | GPIO_PAD_PULL_DOWN | GPIO_PAD_SCHMITT | GPIO_PAD_SLEWFAST); // 0x77
    const uint32_t GPIO_PAD_HW_0TO8       = (GPIO_PAD_OUT_DISABLE | GPIO_PAD_DRIVE_4MA | GPIO_PAD_PULL_UP   | GPIO_PAD_SCHMITT); // 0x9a
    const uint32_t GPIO_PAD_HW_FROM9      = (GPIO_PAD_OUT_DISABLE | GPIO_PAD_DRIVE_4MA | GPIO_PAD_PULL_DOWN | GPIO_PAD_SCHMITT); // 0x96

    const uint32_t GPIO_STATUS_OEFROMPERI = BIT (12); // Bit 12: OE from peripheral
    const uint32_t GPIO_STATUS_OETOPAD = BIT (13);
    const uint32_t GPIO_STATUS_OE = (GPIO_STATUS_OEFROMPERI | GPIO_STATUS_OETOPAD);

    const uint32_t GPIO_CTRL_FUNCSEL_MASK = 0x0000001f; // Masque pour la sélection de fonction

    const uint32_t GPIO_CTRL_DEBOUNCE_LSB = 5; // Filter Debounce LSB
    const uint32_t GPIO_CTRL_DEBOUNCE_MASK = 0x0000007F << GPIO_CTRL_DEBOUNCE_LSB; // Masque pour le filtre de rebond
    const uint32_t GPIO_CTRL_DEBOUNCE_DEFAULT = (4 << GPIO_CTRL_DEBOUNCE_LSB); // Valeur par défaut du filtre de rebond

    const uint32_t GPIO_CTRL_IRQRESET = BIT (28); // Bit 28: Reset IRQ

    // PWM & Clock
    const size_t ClockBankAddr    = 0x40018000;
    const size_t Pwm0BankAddr     = 0x40098000; // Only PWM0 is implemented in this version
    const size_t Pwm1BankAddr     = 0x4009C000; // Not used in this implementation

    const off_t ClockOffset = (ClockBankAddr - IoBaseAddr) / RegisterSize; // GPIO_CLOCK_ADR
    const off_t Pwm0Offset  = (Pwm0BankAddr - IoBaseAddr) / RegisterSize; // GPIO_PWM

    // PWM registers
    const off_t PwmGlobalCtrlReg   = 0x00 / RegisterSize;
    const off_t PwmFifoCtrlReg     = 0x04 / RegisterSize;
    const off_t PwmCommonRangeReg  = 0x08 / RegisterSize;
    const off_t PwmCommonDutyReg   = 0x0C / RegisterSize;
    const off_t PwmDutyFifoReg     = 0x10 / RegisterSize;
    const off_t PwmChanBlockBase   = 0x14 / RegisterSize;
    const size_t PwmChanBlockSize  = 0x10 / RegisterSize; // Size of one PWM channel block

    // Channel registers, offsets in the PWM channel block
    const off_t PwmChanCtrlReg   = 0;
    const off_t PwmChanRangeReg  = 1;
    const off_t PwmChanPhaseReg  = 2;
    const off_t PwmChanDutyReg   = 3;

    // PWM0 GLOBAL_CTRL bits
    const uint32_t PwmGlobalCtrlSetUpdate     = BIT (31); // Bit 31: Set update
    const uint32_t PwmGlobalCtrlChan3En  = BIT (3); // Bit 3: Channel 3 enable
    const uint32_t PwmGlobalCtrlChan2En  = BIT (2); // Bit 2: Channel 2 enable
    const uint32_t PwmGlobalCtrlChan1En  = BIT (1); // Bit 1: Channel 1 enable
    const uint32_t PwmGlobalCtrlChan0En  = BIT (0); // Bit 0: Channel 0 enable
    const uint32_t PwmGlobalCtrlChanMask = (PwmGlobalCtrlChan3En | PwmGlobalCtrlChan2En | PwmGlobalCtrlChan1En | PwmGlobalCtrlChan0En); // Channel enable mask

    enum PwmChanCtrlMode {

      GeneratesZero = 0x00,             // Generates 0
      TrailingEdgeMsMode = 0x01,        // Trailing-edge mark-space PWM modulation
      PhaseCorrectMsMode = 0x02,        // Phase-correct mark-space PWM modulation
      PulseDensityMode = 0x03,          // Pulse-density encoded output
      MsbSerialiserMode = 0x04,         // MSB Serialiser output
      PulsePositionMode = 0x05,         // Pulse position modulated output - single high pulse per cycle
      LeadingEdgeMsMode = 0x06,         // Leading-edge mark-space PWM modulation
      LsbSerialiserMode = 0x07          // LSB Serialiser output
    };
    const uint32_t PwmChanCtrlFifoPopMask = BIT (8); // Bit 8

    const int PWM_RESOLUTION_MIN = 2;
    const int PWM_RESOLUTION_MAX = 31;

    // CLK
    const uint32_t PwmClkOscFreq = 50000000; // 50 MHz, default oscillator frequency

    // PWM0 Clock registers (reverse engineered, official documentation not available)
    // These registers control the clock source and divisor for PWM0.
    // Addresses and usage are based on RP1 Linux kernel sources and community findings:
    //   - Pwm0ClkCtrlReg: Clock control register (enable/disable, magic values)
    //   - Pwm0ClkDivIntReg: Integer part of the clock divisor
    //   - Pwm0ClkDivFracReg: Fractional part of the clock divisor
    //   - Pwm0SelReg: Clock source selection register
    // References:
    //   - https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/pwm/pwm-rp1.c
    //   - https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/clk/bcm/clk-rp1.c
    const off_t Pwm0ClkCtrlReg    = 0x00074 / RegisterSize; // Clock control register
    const off_t Pwm0ClkDivIntReg  = 0x00078 / RegisterSize; // Integer part of the clock divisor
    const off_t Pwm0ClkDivFracReg = 0x0007C / RegisterSize; // Fractional part of the clock divisor
    const off_t Pwm0SelReg        = 0x00080 / RegisterSize; // Clock source selection register

    const uint32_t PwmClkCtrlMagicDisable = 0x10000000;  // Default after boot
    const uint32_t PwmClkCtrlMagicEnable  = 0x11000840;  // Reverse engineered, because of missing documentation, don't known meaning of bits

    const uint32_t PwmClkDivIntMin    = 1; // Minimum integer divisor, TODO check if this is correct
    const uint32_t PwmClkDivIntMax    = 4095;

  }
}


/* ========================================================================== */
