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

#include <iostream>
#include <iomanip>
#include <exception>
#include <piduino/clock.h>
#include <piduino/database.h>
#include "gpio_bcm2835_p.h"
#include "config.h"


namespace Piduino {

  using namespace Bcm2835;

  // -------------------------------------------------------------------------
  //
  //                        Bcm2835Gpio Class
  //
  // -------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Bcm2835Gpio::Bcm2835Gpio (Bcm2835Gpio::Private &dd) : GpioDevice (dd) {}

  // ---------------------------------------------------------------------------
  Bcm2835Gpio::Bcm2835Gpio () :
    GpioDevice (*new Private (this)) {
    PIMP_D (Bcm2835Gpio);

    d->piobase =iobase () + PioOffset;
    Private::is2711 = (db.board().soc().id() == SoC::Bcm2711);
  }

  // ---------------------------------------------------------------------------
  Bcm2835Gpio::~Bcm2835Gpio() = default;

  // -------------------------------------------------------------------------
  unsigned int
  Bcm2835Gpio::flags() const {
    return  hasAltRead | (Private::is2711  ? hasPullRead : 0);
  }

  // -------------------------------------------------------------------------
  AccessLayer
  Bcm2835Gpio::preferedAccessLayer() const {
    return AccessLayerAll;
  }

  // -------------------------------------------------------------------------
  bool
  Bcm2835Gpio::open() {

    if (!isOpen()) {
      PIMP_D (Bcm2835Gpio);

      d->iomap.open (d->piobase, MapBlockSize);
      d->isopen = true;
    }
    return isOpen();
  }

  // -------------------------------------------------------------------------
  void
  Bcm2835Gpio::close() {

    if (isOpen()) {
      PIMP_D (Bcm2835Gpio);

      d->iomap.close();
      d->isopen = false;
    }
  }

  // -------------------------------------------------------------------------
  Pin::Mode
  Bcm2835Gpio::mode (const Pin *pin) const {
    PIMP_D (const Bcm2835Gpio);
    Pin::Mode m;
    int g;
    unsigned int r;

    g = pin->mcuNumber();
    r = d->iomap.atomicRead (GFPSEL0 + g / 10) >> ( (g % 10) * 3) & 7;
    m = Private::int2mode.at (r);
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
  Bcm2835Gpio::setMode (const Pin *pin, Pin::Mode m) {
    PIMP_D (Bcm2835Gpio);
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

    rval = d->iomap.atomicRead (offset);
    rval &= ~ (7 << lsr); // clear
    mval = Private::mode2int.at (m);
    rval |= mval << lsr;
    d->iomap.atomicWrite (offset, rval);
  }

  // -------------------------------------------------------------------------
  Pin::Pull
  Bcm2835Gpio::pull (const Pin *pin) const {

    if (Private::is2711) {
      PIMP_D (const Bcm2835Gpio);
      int g = pin->mcuNumber();
      uint32_t v = (d->iomap.atomicRead (GPPUPPDN0 + (g >> 4)) >> ( (g & 0xf) << 1)) & 0x3;

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
  Bcm2835Gpio::setPull (const Pin *pin, Pin::Pull p) {
    PIMP_D (Bcm2835Gpio);
    uint32_t pval;
    int g = pin->mcuNumber();

    if (Private::is2711) {
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

      pullbits = d->iomap.atomicRead (r);
      pullbits &= ~ (3 << pullshift);
      pullbits |= (pval << pullshift);
      d->iomap.atomicWrite (r, pullbits);
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
      d->iomap.atomicWrite (GPPUD, pval);
      Clock::delayMicroseconds (10);
      d->iomap.atomicWrite (pclk, 1 << g);
      Clock::delayMicroseconds (10);
      d->iomap.atomicWrite (GPPUD, 0);
      d->iomap.atomicWrite (pclk, 0);
    }
  }

  // -------------------------------------------------------------------------
  void
  Bcm2835Gpio::write (const Pin *pin, bool v) {
    PIMP_D (Bcm2835Gpio);
    unsigned int offset = v ? GPSET0 : GPCLR0;
    int g = pin->mcuNumber();

    if (g > 31) {

      offset++;
      g -= 32;
    }
    d->iomap.atomicWrite (offset, 1 << g);
  }

  // -------------------------------------------------------------------------
  bool
  Bcm2835Gpio::read (const Pin *pin) const {
    PIMP_D (const Bcm2835Gpio);
    unsigned int offset = GPLEV0;
    int g = pin->mcuNumber();

    if (g > 31) {

      offset++;
      g -= 32;
    }

    return (d->iomap.atomicRead (offset) & (1 << g)) != 0;
  }

  // -------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  Bcm2835Gpio::modes() const {

    return Private::modes;
  }


  // -----------------------------------------------------------------------------
  //
  //                         Bcm2835Gpio::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Bcm2835Gpio::Private::Private (Bcm2835Gpio *q) :
    GpioDevice::Private (q) {}

  // ---------------------------------------------------------------------------
  Bcm2835Gpio::Private::~Private() = default;

  // -------------------------------------------------------------------------
  // static
  const std::map<Pin::Mode, std::string> Bcm2835Gpio::Private::modes = {
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
  // static
  const std::map<unsigned int, Pin::Mode> Bcm2835Gpio::Private::int2mode = {
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
  // static
  const std::map<Pin::Mode, unsigned int> Bcm2835Gpio::Private::mode2int = {
    {Pin::ModeInput, 0},
    {Pin::ModeOutput, 1},
    {Pin::ModeAlt0, 4},
    {Pin::ModeAlt1, 5},
    {Pin::ModeAlt2, 6},
    {Pin::ModeAlt3, 7},
    {Pin::ModeAlt4, 3},
    {Pin::ModeAlt5, 2},
  };

  // static
  bool Bcm2835Gpio::Private::is2711;
}
/* ========================================================================== */
