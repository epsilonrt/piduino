/* Copyright © 2018 Pascal JEAN, All rights reserved.
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
    const size_t IoBankAddr       = 0x400d0000; // RP1 start address of the GPIO registers
    const size_t SysRio0BankAddr  = 0x400e0000; // RP1 start address of the RIO registers
    const size_t PadsBankAddr     = 0x400f0000; // RP1 start address of the pads registers
    const size_t IoBaseAddr       = 0x40000000; // RP1 base address of the memory mapped I/O

    const off_t PCIeGpioOffset    = (IoBankAddr - IoBaseAddr) / sizeof (uint32_t); // only for PCIe access, for gpiomem access, base address is 0
    const off_t PadsOffset        = (PadsBankAddr - IoBankAddr) / sizeof (uint32_t);
    const off_t RioOffset         = (SysRio0BankAddr - IoBankAddr) / sizeof (uint32_t);

    const size_t GpioMemBlockSize  = 0x00030000;

    const char PCIeDevPath[]      = "/sys/bus/pci/devices";
    const char PCIeRp1VendorId[]  = "0x1de4";
    const char PCIeRp1DeviceId[]  = "0x0001";
    const char PCIeRp1File[]      = "resource1";
    const size_t PCIeBlockSize  = 0x00400000;

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
    const uint32_t GPIO_RIO_OE  = (0x0004 / sizeof (uint32_t));
    const uint32_t GPIO_RIO_IN  = (0x0008 / sizeof (uint32_t));

    const uint32_t GPIO_RIO_SET_OFFSET = (0x2000 / sizeof (uint32_t));
    const uint32_t GPIO_RIO_CLR_OFFSET = (0x3000 / sizeof (uint32_t));

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

    const uint32_t RP1_GPIO_CTRL_FUNCSEL_MASK = 0x0000001f; // Masque pour la sélection de fonction

    const uint32_t RP1_GPIO_CTRL_DEBOUNCE_LSB = 5; // Filter Debounce LSB
    const uint32_t RP1_GPIO_CTRL_DEBOUNCE_MASK = 0x0000007F << RP1_GPIO_CTRL_DEBOUNCE_LSB; // Masque pour le filtre de rebond
    const uint32_t RP1_GPIO_CTRL_DEBOUNCE_DEFAULT = (4 << RP1_GPIO_CTRL_DEBOUNCE_LSB); // Valeur par défaut du filtre de rebond


    const uint32_t RP1_GPIO_CTRL_IRQRESET = BIT(28); // Bit 28: Reset IRQ
  }
}


/* ========================================================================== */
