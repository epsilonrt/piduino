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

#include <iostream>
#include <iomanip>
#include <exception>
#include <piduino/clock.h>
#include <piduino/database.h>
#include "gpio_bcm2835.h"
#include "config.h"

namespace Piduino {

  namespace Bcm2835 {
    // -------------------------------------------------------------------------
    //
    //                        Bcm2835::GpioDevice Class
    //
    // -------------------------------------------------------------------------


    // -------------------------------------------------------------------------
    GpioDevice::GpioDevice() : Piduino::GpioDevice () {

      _piobase = iobase () + PioOffset;
      _is2711 = (db.board().soc().id() == SoC::Bcm2711);
    }

    // -------------------------------------------------------------------------
    GpioDevice::~GpioDevice() = default;

    // -------------------------------------------------------------------------
    unsigned int
    GpioDevice::flags() const {
      return  hasAltRead | (_is2711  ? hasPullRead : 0);
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

        _iomap.open (_piobase, MapBlockSize);
        setOpen (true);
      }
      return isOpen();
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::close() {

      if (isOpen()) {

        _iomap.close();
        setOpen (false);
      }
    }

    // -------------------------------------------------------------------------
    Pin::Mode
    GpioDevice::mode (const Pin *pin) const {
      Pin::Mode m;
      int g;
      unsigned int r;

      g = pin->mcuNumber();
      r = readReg (GFPSEL0 + g / 10) >> ( (g % 10) * 3) & 7;
      m = _int2mode.at (r);
      /*
          - BCM12 ALT0 -> PWM0
          - BCM13 ALT0 -> PWM1
          - BCM18 ALT5 -> PWM0
          - BCM19 ALT5 -> PWM1
      */

      if ( ( (m == Pin::ModeAlt0) && ( (g == 12) || (g == 13))) ||
           ( (m == Pin::ModeAlt5) && ( (g == 18) || (g == 19)))) {
        m = Pin::ModePwm;
      }
      return m;
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::setMode (const Pin *pin, Pin::Mode m) {
      int g;
      unsigned int offset, rval, lsr, mval;

      g = pin->mcuNumber();

      if (m == Pin::ModePwm) {

        if ( (g == 12) || (g == 13)) {

          m = Pin::ModeAlt0;
        }
        else if ( (g == 18) || (g == 19)) {

          m = Pin::ModeAlt5;
        }
        else {

          throw std::invalid_argument ("ModePwm can only be set for GPIO12, GPIO13, GPIO18 or GPIO19");
        }
      }

      offset = GFPSEL0 + g / 10;
      lsr = (g % 10) * 3;

      rval = readReg (offset);
      rval &= ~ (7 << lsr); // clear
      mval = _mode2int.at (m);
      rval |= mval << lsr;
      writeReg (offset, rval);
    }

    // -------------------------------------------------------------------------
    Pin::Pull
    GpioDevice::pull (const Pin *pin) const {

      if (_is2711) {
        int g = pin->mcuNumber();
        uint32_t v = (readReg (GPPUPPDN0 + (g >> 4)) >> ( (g & 0xf) << 1)) & 0x3;

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
            break; // illegal
        }
      }
      return Pin::PullUnknown;
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::setPull (const Pin *pin, Pin::Pull p) {
      uint32_t pval;
      int g = pin->mcuNumber();

      if (_is2711) {
        size_t r = GPPUPPDN0 + (g >> 4);
        int pullshift = (g & 0xf) << 1;
        uint32_t pullbits;

        switch (p) {
          case Pin::PullOff:
            pval = 0;
            break;
          case Pin::PullUp:
            pval = 1;
            break;
          case Pin::PullDown:
            pval = 2;
            break;
          default:
            return; // illegal
        }

        pullbits = readReg (r);
        pullbits &= ~ (3 << pullshift);
        pullbits |= (pval << pullshift);
        writeReg (r, pullbits);
      }
      else {
        size_t pclk = GPPUDCLK0;
        if (g > 31) {

          pclk++;
          g -= 32;
        }

        /*
          PUD - GPIO Pin Pull-up/down
          00 = Off – disable pull-up/down
          01 = Enable Pull Down control
          10 = Enable Pull Up control
          11 = Reserved
        */
        switch (p) {
          case Pin::PullOff:
            pval = 0;
            break;
          case Pin::PullDown:
            pval = 1;
            break;
          case Pin::PullUp:
            pval = 2;
            break;
          default:
            return; // illegal
        }

        /*
          required:
          1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
          to remove the current Pull-up/down)
          2. Wait 150 cycles – this provides the required set-up time for the control signal
          3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
          modify – NOTE only the pads which receive a clock will be modified, all others will
          retain their previous state.
          4. Wait 150 cycles – this provides the required hold time for the control signal
          5. Write to GPPUD to remove the control signal
          6. Write to GPPUDCLK0/1 to remove the clock
        */
        writeReg (GPPUD, pval);
        Clock::delayMicroseconds (10);
        writeReg (pclk, 1 << g);
        Clock::delayMicroseconds (10);
        writeReg (GPPUD, 0);
        writeReg (pclk, 0);
      }
    }

    // -------------------------------------------------------------------------
    void
    GpioDevice::write (const Pin *pin, bool v) {
      unsigned int offset = v ? GPSET0 : GPCLR0;
      int g = pin->mcuNumber();

      if (g > 31) {

        offset++;
        g -= 32;
      }
      writeReg (offset, 1 << g);
    }

    // -------------------------------------------------------------------------
    bool
    GpioDevice::read (const Pin *pin) const {
      unsigned int offset = GPLEV0;
      int g = pin->mcuNumber();

      if (g > 31) {

        offset++;
        g -= 32;
      }

      return (readReg (offset) & (1 << g)) != 0;
    }

    // -------------------------------------------------------------------------
    const std::map<Pin::Mode, std::string> &
    GpioDevice::modes() const {
      return _modes;
    }

    // -------------------------------------------------------------------------
    int 
    GpioDevice::systemNumberOffset() const {
      // TODO : check if this is correct
      return 512;
    }

    // -------------------------------------------------------------------------
    const std::map<Pin::Mode, std::string> GpioDevice::_modes = {
      {Pin::ModeInput, "in"},
      {Pin::ModeOutput, "out"},
      {Pin::ModeAlt0, "alt0"},
      {Pin::ModeAlt1, "alt1"},
      {Pin::ModeAlt2, "alt2"},
      {Pin::ModeAlt3, "alt3"},
      {Pin::ModeAlt4, "alt4"},
      {Pin::ModeAlt5, "alt5"},
      {Pin::ModePwm, "pwm"},
    };

    // -------------------------------------------------------------------------
    const std::map<unsigned int, Pin::Mode> GpioDevice::_int2mode = {
      {0, Pin::ModeInput},
      {1, Pin::ModeOutput},
      {4, Pin::ModeAlt0},
      {5, Pin::ModeAlt1},
      {6, Pin::ModeAlt2},
      {7, Pin::ModeAlt3},
      {3, Pin::ModeAlt4},
      {2, Pin::ModeAlt5},
    };

    // -------------------------------------------------------------------------
    const std::map<Pin::Mode, unsigned int> GpioDevice::_mode2int = {
      {Pin::ModeInput, 0},
      {Pin::ModeOutput, 1},
      {Pin::ModeAlt0, 4},
      {Pin::ModeAlt1, 5},
      {Pin::ModeAlt2, 6},
      {Pin::ModeAlt3, 7},
      {Pin::ModeAlt4, 3},
      {Pin::ModeAlt5, 2},
    };

    bool GpioDevice::_is2711;
  }
}
/* ========================================================================== */
