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


// Offsets pour accéder aux registres GPIO (lecture/écriture, set, clear, xor)
#define RP1_RW_OFFSET     0x0000
#define RP1_XOR_OFFSET      0x1000
// #define RP1_SET_OFFSET      0x2000
// #define RP1_CLR_OFFSET      0x3000

// Offsets des registres de statut et de contrôle GPIO
#define RP1_GPIO_STATUS     0x0000
#define RP1_GPIO_CTRL     0x0004

// Décalages et masques pour les événements GPIO (bruts et filtrés)
#define RP1_GPIO_EVENTS_SHIFT_RAW 20
#define RP1_GPIO_STATUS_FALLING   BIT(20)
#define RP1_GPIO_STATUS_RISING    BIT(21)
#define RP1_GPIO_STATUS_LOW   BIT(22)
#define RP1_GPIO_STATUS_HIGH    BIT(23)

#define RP1_GPIO_EVENTS_SHIFT_FILTERED  24
#define RP1_GPIO_STATUS_F_FALLING BIT(24)
#define RP1_GPIO_STATUS_F_RISING  BIT(25)
#define RP1_GPIO_STATUS_F_LOW   BIT(26)
#define RP1_GPIO_STATUS_F_HIGH    BIT(27)

// Décalages et masques pour les champs du registre CTRL GPIO
// #define RP1_GPIO_CTRL_FUNCSEL_LSB 0
// #define RP1_GPIO_CTRL_FUNCSEL_MASK  0x0000001f
#define RP1_GPIO_CTRL_OUTOVER_LSB 12
#define RP1_GPIO_CTRL_OUTOVER_MASK  0x00003000
#define RP1_GPIO_CTRL_OEOVER_LSB  14
#define RP1_GPIO_CTRL_OEOVER_MASK 0x0000c000
#define RP1_GPIO_CTRL_INOVER_LSB  16
#define RP1_GPIO_CTRL_INOVER_MASK 0x00030000
#define RP1_GPIO_CTRL_IRQEN_FALLING BIT(20)
#define RP1_GPIO_CTRL_IRQEN_RISING  BIT(21)
#define RP1_GPIO_CTRL_IRQEN_LOW   BIT(22)
#define RP1_GPIO_CTRL_IRQEN_HIGH  BIT(23)
#define RP1_GPIO_CTRL_IRQEN_F_FALLING BIT(24)
#define RP1_GPIO_CTRL_IRQEN_F_RISING  BIT(25)
#define RP1_GPIO_CTRL_IRQEN_F_LOW BIT(26)
#define RP1_GPIO_CTRL_IRQEN_F_HIGH  BIT(27)
#define RP1_GPIO_CTRL_IRQRESET    BIT(28)
#define RP1_GPIO_CTRL_IRQOVER_LSB 30
#define RP1_GPIO_CTRL_IRQOVER_MASK  0xc0000000

// Configuration des résistances de tirage (pull-up/pull-down)
// #define RP1_PUD_OFF     0
// #define RP1_PAD_PULL_DOWN      1
// #define RP1_PAD_PULL_UP      2

// Nombre de fonctions alternatives possibles par GPIO
// #define RP1_FSEL_COUNT      9

// // Direction de la broche (entrée/sortie)
// #define RP1_DIR_OUTPUT      0
// #define RP1_DIR_INPUT     1

// // Valeurs pour le champ OUTOVER (contrôle de la sortie)
// #define RP1_OUTOVER_PERI    0
// #define RP1_OUTOVER_INVPERI   1
// #define RP1_OUTOVER_LOW     2
// #define RP1_OUTOVER_HIGH    3

// // Valeurs pour le champ OEOVER (contrôle de l'activation de la sortie)
// #define RP1_OEOVER_PERI     0
// #define RP1_OEOVER_INVPERI    1
// #define RP1_OEOVER_DISABLE    2
// #define RP1_OEOVER_ENABLE   3

// // Valeurs pour le champ INOVER (contrôle de l'entrée)
// #define RP1_INOVER_PERI     0
// #define RP1_INOVER_INVPERI    1
// #define RP1_INOVER_LOW      2
// #define RP1_INOVER_HIGH     3

// Offsets pour les registres RIO (contrôle bas niveau des GPIO)
// #define RP1_RIO_OUT     0x00
// #define RP1_RIO_OE      0x04
// #define RP1_RIO_IN      0x08

// Masques et décalages pour le registre PAD (contrôle du pad physique)
// #define RP1_PAD_SLEWFAST_MASK   0x00000001 // Contrôle du slew rate rapide
// #define RP1_PAD_SLEWFAST_LSB    0
// #define RP1_PAD_SCHMITT_MASK    0x00000002 // Activation du trigger Schmitt
// #define RP1_PAD_SCHMITT_LSB   1
// #define RP1_PAD_PULL_MASK   0x0000000c     // Configuration du pull-up/pull-down
// #define RP1_PAD_PULL_LSB    2
// #define RP1_PAD_DRIVE_MASK    0x00000030   // Force de drive (courant)
// #define RP1_PAD_DRIVE_LSB   4
// #define RP1_PAD_IN_ENABLE_MASK    0x00000040 // Activation de l'entrée
// #define RP1_PAD_IN_ENABLE_LSB   6
// #define RP1_PAD_OUT_DISABLE_MASK  0x00000080 // Désactivation de la sortie
// #define RP1_PAD_OUT_DISABLE_LSB   7

// Valeurs pour la force de drive (courant de sortie)
// #define RP1_PAD_DRIVE_2MA   0x00000000
// #define RP1_PAD_DRIVE_4MA   0x00000010
// #define RP1_PAD_DRIVE_8MA   0x00000020
// #define RP1_PAD_DRIVE_12MA    0x00000030

// Macros utilitaires pour manipuler les champs des registres
// #define FLD_GET(r, f) (((r) & (f ## _MASK)) >> (f ## _LSB))
// #define FLD_SET(r, f, v) r = (((r) & ~(f ## _MASK)) | ((v) << (f ## _LSB)))

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

    const uint32_t RP1_RIO_OUT = 0x0000;
    const uint32_t RP1_RIO_OE  = (0x0004 / sizeof (uint32_t));
    const uint32_t RP1_RIO_IN  = (0x0008 / sizeof (uint32_t));

    const uint32_t RP1_SET_OFFSET = (0x2000 / sizeof (uint32_t));
    const uint32_t RP1_CLR_OFFSET = (0x3000 / sizeof (uint32_t));

    const uint32_t RP1_PAD_PULL_LSB     = 2;
    const uint32_t RP1_PAD_DRIVE_LSB    = 4;

    const uint32_t RP1_PAD_SLEWFAST     = 0x00000001; // Contrôle du slew rate rapide
    const uint32_t RP1_PAD_SCHMITT      = 0x00000002; // Activation du trigger Schmitt
    const uint32_t RP1_PAD_PULL_OFF     = 0x00000000;
    const uint32_t RP1_PAD_PULL_DOWN    = 0x00000004;
    const uint32_t RP1_PAD_PULL_UP      = 0x00000008;
    const uint32_t RP1_PAD_DRIVE_2MA    = 0x00000000;
    const uint32_t RP1_PAD_DRIVE_4MA    = 0x00000010;
    const uint32_t RP1_PAD_DRIVE_8MA    = 0x00000020;
    const uint32_t RP1_PAD_DRIVE_12MA   = 0x00000030;
    const uint32_t RP1_PAD_IN_ENABLE    = 0x00000040; // Activation de l'entrée
    const uint32_t RP1_PAD_OUT_DISABLE  = 0x00000080; // Désactivation de la sortie

    const uint32_t RP1_PAD_PULL_MASK    = RP1_PAD_PULL_UP | RP1_PAD_PULL_DOWN;
    const uint32_t RP1_PAD_DRIVE_MASK   = 0x00000030;   // Force de drive (courant)

    const uint32_t RP1_PAD_DEFAULT_0TO8  = (RP1_PAD_IN_ENABLE   | RP1_PAD_DRIVE_12MA | RP1_PAD_PULL_UP   | RP1_PAD_SCHMITT | RP1_PAD_SLEWFAST); // 0x7b
    const uint32_t RP1_PAD_DEFAULT_FROM9 = (RP1_PAD_IN_ENABLE   | RP1_PAD_DRIVE_12MA | RP1_PAD_PULL_DOWN | RP1_PAD_SCHMITT | RP1_PAD_SLEWFAST); // 0x77
    const uint32_t RP1_PAD_DISABLE_0TO8  = (RP1_PAD_OUT_DISABLE | RP1_PAD_DRIVE_4MA  | RP1_PAD_PULL_UP   | RP1_PAD_SCHMITT); // 0x9a
    const uint32_t RP1_PAD_DISABLE_FROM9 = (RP1_PAD_OUT_DISABLE | RP1_PAD_DRIVE_4MA  | RP1_PAD_PULL_DOWN | RP1_PAD_SCHMITT); // 0x96

    const uint32_t RP1_FSEL_COUNT = 9;

    const uint32_t RP1_DIR_OUTPUT = 0;
    const uint32_t RP1_DIR_INPUT = 1;

    const uint32_t RP1_OUTOVER_PERI = 0;
    const uint32_t RP1_OUTOVER_INVPERI = 1;
    const uint32_t RP1_OUTOVER_LOW = 2;
    const uint32_t RP1_OUTOVER_HIGH = 3;

    const uint32_t RP1_OEOVER_PERI = 0;
    const uint32_t RP1_OEOVER_INVPERI = 1;
    const uint32_t RP1_OEOVER_DISABLE = 2;
    const uint32_t RP1_OEOVER_ENABLE = 3;

    const uint32_t RP1_INOVER_PERI = 0;
    const uint32_t RP1_INOVER_INVPERI = 1;
    const uint32_t RP1_INOVER_LOW = 2;
    const uint32_t RP1_INOVER_HIGH = 3;

    const uint32_t RP1_GPIO_STATUS_OEFROMPERI = BIT (12); // Bit 12: OE from peripheral
    const uint32_t RP1_GPIO_STATUS_OETOPAD = BIT (13);
    const uint32_t RP1_GPIO_STATUS_OE = (RP1_GPIO_STATUS_OEFROMPERI | RP1_GPIO_STATUS_OETOPAD);

    const uint32_t RP1_GPIO_CTRL_FUNCSEL_MASK = 0x0000001f; // Masque pour la sélection de fonction

    // const uint32_t RP1_PAD_PULL_UP = (1 << 3);
    // const uint32_t RP1_PAD_PULL_DOWN = (1 << 2);
    // const uint32_t RP1_INV_PUD_MASK = ~ (RP1_PAD_PULL_UP | RP1_PAD_PULL_DOWN); //~0x0C

    // const uint32_t RP1_STATUS_LEVEL_LOW  = 0x00400000;
    // const uint32_t RP1_STATUS_LEVEL_HIGH = 0x00800000;
    // const uint32_t RP1_STATUS_LEVEL_MASK = 0x00C00000;

    // const uint32_t RP1_DEBOUNCE_DEFAULT_VALUE = 4;
    // const uint32_t RP1_DEBOUNCE_MASK    = 0x7f;
    // const uint32_t RP1_DEBOUNCE_DEFAULT = (RP1_DEBOUNCE_DEFAULT_VALUE << 5);

    // const uint32_t RP1_IRQRESET = 0x10000000; //CTRL Bit 28



    // const uint32_t RP1_PAD_DRIVE_MASK   = 0x00000030;
    // const uint32_t RP1_INV_PAD_DRIVE_MASK = ~ (RP1_PAD_DRIVE_MASK);
  }
}


/* ========================================================================== */
