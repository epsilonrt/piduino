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

#include <piduino/iomap.h>
#include "hx.h"
#include "gpio_hx.h"
#include "../../../gpio/gpiodevice_p.h"

namespace Piduino {

  class AllWinnerHxGpio::Private  : public GpioDevice::Private {
    public:
      Private (AllWinnerHxGpio *q);
      virtual ~Private();

      /*
        H3/H5 PIO Registers:
          Module  Base Address  Port
          PIO1    0x01C20800    A,B,C,D,E,F,G (n=0 to n=6, n=1 no pin)
          PIO2    0x01F02C00    L (n=0)

          Register  Offset          Description
          Pn_CFG0   0x0000 + n*0x24 Port n Configure Register 0 (n from 0 to 6)
          Pn_CFG1   0x0004 + n*0x24 Port n Configure Register 1 (n from 0 to 6)
          Pn_CFG2   0x0008 + n*0x24 Port n Configure Register 2 (n from 0 to 6)
          Pn_CFG3   0x000C + n*0x24 Port n Configure Register 3 (n from 0 to 6)
          Pn_DAT    0x0010 + n*0x24 Port n Data Register (n from 0 to 6)
          Pn_DRV0   0x0014 + n*0x24 Port n Multi-Driving Register 0 (n from 0 to 6)
          Pn_DRV1   0x0018 + n*0x24 Port n Multi-Driving Register 1 (n from 0 to 6)
          Pn_PUL0   0x001C + n*0x24 Port n Pull Register 0 (n from 0 to 6)
          Pn_PUL1   0x0020 + n*0x24 Port n Pull Register 1 (n from 0 to 6)
      */
      struct __attribute__ ( (__packed__)) PioBank  {
        uint32_t CFG[4];
        uint32_t DAT;
        uint32_t DRV[2];
        uint32_t PUL[2];
      };

      void debugPrintBank (const PioBank *b) const;
      void debugPrintAllBanks () const;
      PioBank *pinBank (int *mcupin) const;
      PioBank *bank (unsigned int bkindex) const;

      IoMap iomap[2];
      static const int portSize[];
      static const std::map<int, Pin::Mode> int2mode;
      static const std::map<Pin::Mode, int> mode2int;
      static const std::map<Pin::Mode, std::string> modes;

      PIMP_DECLARE_PUBLIC (AllWinnerHxGpio)
  };
}