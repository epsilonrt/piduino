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
#include <iostream>
#include <iomanip>
#include <exception>
#include <piduino/gpio.h>
#include <piduino/clock.h>
#include <piduino/database.h>
#include "gpio_hx.h"
#include "config.h"

namespace Piduino {

  namespace AllWinnerHx {
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
    typedef struct PioBank PioBank;

    // -------------------------------------------------------------------------
    //
    //                        GpioDevice Class
    //
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    GpioDevice::GpioDevice() : Piduino::GpioDevice () {

      if (db.board().soc().family().id() != SoC::Family::AllwinnerH) {

        throw std::system_error (ENOTSUP, std::system_category(),
                                 "It seems that this system is not a Allwinner H3/H5 board !");
      }
    }

    // -------------------------------------------------------------------------
    GpioDevice::~GpioDevice() {
    }

    // -------------------------------------------------------------------------
    unsigned int
    GpioDevice::flags() const {
      return  hasPullRead | hasToggle | hasDrive;
    }

    // -------------------------------------------------------------------------
    AccessLayer
    GpioDevice::preferedAccessLayer() const {
      return AccessLayerAll;
    }

    // -------------------------------------------------------------------------
    bool
    GpioDevice::open() {

      if (!isOpen()) {

        _iomap[0].open (Io1Base, MapBlockSize);
        _iomap[1].open (Io2Base, MapBlockSize);
        setOpen (true);
        // debugPrintAllBanks ();
      }
      return isOpen();
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::close() {

      if (isOpen()) {
        _iomap[0].close();
        _iomap[1].close();
        setOpen (false);
      }
    }

    // -------------------------------------------------------------------------
    Pin::Mode
    GpioDevice::mode (const Pin * pin) const {
      Pin::Mode ret;
      PioBank * b;
      int i, r, m;
      int g = pin->mcuNumber();
      int f = g;

      b = pinBank (&f);
      r = f >> 3;
      i = (f - (r * 8)) * 4;
      m = b->CFG[r] >> i & 7;
      ret = _int2mode.at (m);

      // PA5: PWM0 FUNC3-> UART0 !
      if ( (ret == Pin::ModeAlt3) && (g == 5)) {
        ret = Pin::ModePwm;
      }
      return ret;
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::setMode (const Pin * pin, Pin::Mode m) {
      PioBank * b;
      int i, r, v;
      int g = pin->mcuNumber();
      int f = g;

      b = pinBank (&f);
      r = f >> 3;
      i = (f - (r * 8)) * 4;

      if (m == Pin::ModePwm) {
        // PA5: PWM0 FUNC3-> UART0 
        if (g == 5) {
          m = Pin::ModeAlt3;
        }
        else {
          throw std::invalid_argument ("ModePwm can only be set for PA5 !");
        }
      }

      v = _mode2int.at (m);
      b->CFG[r] &= ~ (0b1111 << i);  // clear config
      b->CFG[r] |= (v << i);
      debugPrintBank (b);
    }

    // -------------------------------------------------------------------------
    Pin::Pull
    GpioDevice::pull (const Pin * pin) const {
      PioBank * b;
      int i, r, v;
      int g = pin->mcuNumber();

      b = pinBank (&g);
      r = g >> 4;
      i = (g - (r * 16)) * 2;
      v = b->PUL[r] >> i & 3;
      /*
        00: Pull-up/down disable
        10: Pull-down
        01: Pull-up
        11: Reserved
      */
      switch (v) {
        case 0:
          return Pin::PullOff;
        case 2:
          return Pin::PullDown;
        case 1:
          return Pin::PullUp;
        default:
          break;
      }
      return Pin::PullUnknown;
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::setPull (const Pin * pin, Pin::Pull p) {
      PioBank * b;
      int v = -1;
      int g = pin->mcuNumber();
      int f = g;

      b = pinBank (&f);
      /*
        00: Pull-up/down disable
        10: Pull-down
        01: Pull-up
        11: Reserved
      */
      switch (p) {
        case Pin::PullOff:
          v = 0;
          break;
        case Pin::PullDown:
          v = 2;
          break;
        case Pin::PullUp:
          v = 1;
          break;
        default:
          break;
      }

      if (v >= 0) {
        int i, r;

        r = f >> 4;
        i = (f - (r * 16)) * 2;
        b->PUL[r] &= ~ (0b11 << i); // clear config -> disable
        Clock::delayMicroseconds (10);
        b->PUL[r] |= (v << i);
        Clock::delayMicroseconds (10);
        debugPrintBank (b);
      }
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::write (const Pin * pin, bool v) {
      PioBank * b;
      int g = pin->mcuNumber();

      b = pinBank (&g);
      if (v) {

        b->DAT |= 1 << g;;
      }
      else {

        b->DAT &= ~ (1 << g);
      }
      debugPrintBank (b);
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::toggle (const Pin * pin) {
      PioBank * b;
      int g = pin->mcuNumber();

      b = pinBank (&g);
      b->DAT ^= 1 << g;;
      debugPrintBank (b);
    }

    // -------------------------------------------------------------------------
    bool
    GpioDevice::read (const Pin * pin) const {
      PioBank * b;
      int g = pin->mcuNumber();

      b = pinBank (&g);
      return b->DAT & (1 << g) ? true : false;
    }

    // -------------------------------------------------------------------------
    int
    GpioDevice::drive (const Pin * pin) const {
      PioBank * b;
      int i, r, v;
      int g = pin->mcuNumber();

      b = pinBank (&g);
      r = g >> 4;
      i = (g - (r * 16)) * 2;
      v = b->DRV[r] >> i & 3;

      return v;
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::setDrive (const Pin * pin, int v) {
      PioBank * b;
      int g = pin->mcuNumber();
      int f = g;

      b = pinBank (&f);

      if ((v >= 0) && (v <= 3)) {
        int i, r;

        r = f >> 4;
        i = (f - (r * 16)) * 2;
        b->DRV[r] &= ~ (0b11 << i); 
        Clock::delayMicroseconds (10);
        b->DRV[r] |= (v << i);
        Clock::delayMicroseconds (10);
        debugPrintBank (b);
      }
      else {
        
        throw std::invalid_argument ("drive must be between 0 and 3 !");
      }
    }

    // -------------------------------------------------------------------------
    const std::map<Pin::Mode, std::string> &
    GpioDevice::modes() const {
      return _modes;
    }


    // -------------------------------------------------------------------------
    const std::map<Pin::Mode, std::string> GpioDevice::_modes = {
      {Pin::ModeInput, "in"},
      {Pin::ModeOutput, "out"},
      {Pin::ModeAlt2, "alt2"},
      {Pin::ModeAlt3, "alt3"},
      {Pin::ModeAlt4, "alt4"},
      {Pin::ModeAlt5, "alt5"},
      {Pin::ModeAlt6, "alt6"},
      {Pin::ModeDisabled, "off"},
      {Pin::ModePwm, "pwm"},
    };

    // -------------------------------------------------------------------------
    const std::map<int, Pin::Mode> GpioDevice::_int2mode = {
      {0, Pin::ModeInput},
      {1, Pin::ModeOutput},
      {2, Pin::ModeAlt2},
      {3, Pin::ModeAlt3},
      {4, Pin::ModeAlt4},
      {5, Pin::ModeAlt5},
      {6, Pin::ModeAlt6},
      {7, Pin::ModeDisabled}
    };

    // -------------------------------------------------------------------------
    const std::map<Pin::Mode, int> GpioDevice::_mode2int = {
      {Pin::ModeInput, 0},
      {Pin::ModeOutput, 1},
      {Pin::ModeAlt2, 2},
      {Pin::ModeAlt3, 3},
      {Pin::ModeAlt4, 4},
      {Pin::ModeAlt5, 5},
      {Pin::ModeAlt6, 6},
      {Pin::ModeDisabled, 7}
    };

    /* -----------------------------------------------------------------------------
      H3/H5 Ports:
      Port A(PA): 22 input/output port
      Port C(PC): 17 input/output port
      Port D(PD): 18 input/output port
      Port E(PE) : 16 input/output port
      Port F(PF) : 7 input/output port
      Port G(PG) : 14 input/output port
      Port L(PL) : 12 input/output port
    */
    const int GpioDevice::_portSize[] = {
      22, 0, 17, 18, 16, 7, 14, 12, -1
    };

    // -------------------------------------------------------------------------
    PioBank *
    GpioDevice::bank (unsigned int bkindex) const {
      PioBank * bk = nullptr;

      if (isOpen()) {

        if (bkindex < 7) {

          bk = (PioBank *) (_iomap[0].io ( (Pio1Offset + bkindex * 0x24) >> 2));
        }
        else if (bkindex == 7) {

          bk = (PioBank *) (_iomap[1].io (Pio2Offset >> 2));
        }
      }
      else {

        throw std::system_error (ENODEV, std::system_category());
      }

      return bk;
    }

    // -------------------------------------------------------------------------
    PioBank *
    GpioDevice::pinBank (int * mcupin) const {
      PioBank * bk = nullptr;
      const int * p = _portSize;
      unsigned int bkindex = 0;
      int ng = *mcupin;

      while ( (*p >= 0) && (ng >= *p)) {

        ng -= *p;
        do {
          bkindex++;
          p++;
        }
        while (bkindex == 1);   // saute PortB
      }

      if (bkindex < 8) {

        *mcupin = ng;
        bk = bank (bkindex);
        if (isDebug()) {

          char c = (bkindex < 7 ? 'A' + bkindex : 'L');
          std::cout << "---Port " << c << "---" << std::endl;
          debugPrintBank (bk);
        }
      }
      else {

        throw std::out_of_range ("Unable to find Allwinner H3/H5 PIO registers bank");
      }

      return bk;
    }

    // -------------------------------------------------------------------------

    void
    GpioDevice::debugPrintAllBanks () const {

      if (isDebug()) {

        for (int i = 0; i < 8; i++) {

          if (i != 1) {

            PioBank * b = bank (i);
            char c = (i < 7 ? 'A' + i : 'L');
            std::cout << "---Port " << c << "---" << std::endl;
            debugPrintBank (b);
          }
        }
        std::cout << "---------------------------------" << std::endl << std::endl;
      }
    }

#define HEX(width, val) std::setfill('0') << std::setw(width) << std::hex << val

    // -------------------------------------------------------------------------
    void
    GpioDevice::debugPrintBank (const PioBank * b) const {

      if (isDebug()) {

        std::cout << "CFG0=0x" << HEX (8, b->CFG[0]) << " - CFG1=0x" << HEX (8, b->CFG[1]) << std::endl;
        std::cout << "CFG2=0x" << HEX (8, b->CFG[2]) << " - CFG3=0x" << HEX (8, b->CFG[3]) << std::endl;
        std::cout << "DAT =0x" << HEX (8, b->DAT) << std::endl;
        std::cout << "DRV0=0x" << HEX (8, b->DRV[0]) << " - DRV1=0x" << HEX (8, b->DRV[1]) << std::endl;
        std::cout << "PUL0=0x" << HEX (8, b->PUL[0]) << " - PUL1=0x" << HEX (8, b->PUL[1]) << std::endl;
      }
    }
  }
}
/* ========================================================================== */
