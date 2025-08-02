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
#include <climits>
#include <cmath>
#include "pwm_bcm2835.h"
#include <piduino/clock.h>
#include <piduino/database.h>
#include "config.h"

namespace Piduino {

  using namespace Bcm2835;

  // -------------------------------------------------------------------------
  //
  //                     SocPwm::Bcm2835Engine Class
  //
  // -------------------------------------------------------------------------

  // -------------------------------------------------------------------------
  // public
  SocPwm::Bcm2835Engine::Bcm2835Engine (SocPwm *q, Pin *p) :
    SocPwm::Private (q, p, hasFrequency | hasSetFrequency | hasResolution | hasSetResolution | hasRange | hasSetRange),
    rngReg (RNG1),
    dataReg (DAT1),
    ctlMasqInit (RPTL1 | SBIT1 | POLA1),
    ctlMasqStartStop (MSEN1 | PWEN1) { // PWM0 by default

    if (db.board().soc().id() == SoC::Bcm2711) {

      is2711 = true;
      clkFreq = PLL_CLOCK_FREQ_2711;
    }
    else {

      clkFreq = PLL_CLOCK_FREQ;
    }

    if ( (p->mcuNumber() == 13) || (p->mcuNumber() == 19)) { // PWM1

      p->setMode (Pin::ModePwm);
      rngReg = RNG2;
      dataReg = DAT2;
      ctlMasqStartStop = MSEN2 | PWEN2;
      ctlMasqInit = RPTL2 | SBIT2 | POLA2;
    }
    else {

      throw std::runtime_error (EXCEPTION_MSG ("Hardware PWM not supported on this pin"));
    }
  }

  // -------------------------------------------------------------------------
  SocPwm::Bcm2835Engine::~Bcm2835Engine() = default;

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::Bcm2835Engine::open (IoDevice::OpenMode m) {

    if (!pwm.open (iobase() + PwmOffset, MapBlockSize)) {

      return false;
    }
    if (!clock.open (iobase() + ClockOffset, MapBlockSize)) {

      pwm.close();
      return false;
    }
    return SocPwm::Private::open (m);
  }

  // -------------------------------------------------------------------------
  // override
  void
  SocPwm::Bcm2835Engine::close() {

    pwm.close();
    clock.close();
    SocPwm::Private::close();
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Bcm2835Engine::read() {

    return readPwm (dataReg);
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::Bcm2835Engine::write (long value) {

    writePwm (dataReg, value);
    return true;
  }

  // -------------------------------------------------------------------------
  // override
  void
  SocPwm::Bcm2835Engine::setEnable (bool enable) {
    uint32_t ctl = readPwm (CTL);

    if (enable) {

      ctl &= ~ctlMasqInit;
      ctl |= ctlMasqStartStop;
    }
    else {

      ctl &= ~ctlMasqStartStop;
    }
    writePwm (CTL, ctl);
    clk.delayMicroseconds (10);
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::Bcm2835Engine::isEnabled () const {

    return (readPwm (CTL) & ctlMasqStartStop) == ctlMasqStartStop;
  }


  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Bcm2835Engine::max() const {

    return readPwm (rngReg);
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Bcm2835Engine::setRange (long r) {

    writePwm (static_cast<uint32_t> (rngReg),  r);
    return range();
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Bcm2835Engine::frequency() const {
    uint32_t div = clockDivisor();

    if (div != 0) {
      return clkFreq / div / range();
    }
    return 0;
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Bcm2835Engine::setFrequency (long f) {

    if (f > 0) {
      setClockDivisor (frequencyDivisor (f));
    }
    return frequency();
  }

  // -------------------------------------------------------------------------
  // internal
  uint32_t
  SocPwm::Bcm2835Engine::clockDivisor() const {

    uint32_t div = readClock (CM_PWMDIV) >> 12;
    return div;
  }

  // -------------------------------------------------------------------------
  // internal
  void
  SocPwm::Bcm2835Engine::setClockDivisor (uint32_t div) {

    if (div < PLL_CLOCK_DIVISOR_MIN) {

      div = PLL_CLOCK_DIVISOR_MIN;
    }
    else if (div > PLL_CLOCK_DIVISOR_MAX) {

      div = PLL_CLOCK_DIVISOR_MAX;
    }

    uint32_t backupctl = readPwm (CTL);

    writePwm (CTL, 0); // Turn off PWM before changing
    clk.delayMicroseconds (10);

    writeClock (CM_PWMCTL, PASSWD | KILL); // Turn off clock generator
    clk.delayMicroseconds (110);

    //  clk.delayMicroseconds(10);
    while (readClock (CM_PWMCTL) & BUSY) {
      clk.delayMicroseconds (1);  // Wait for generator to stop
    }

    // set frequency
    writeClock (CM_PWMDIV, PASSWD | (div << 12));

    // enable clock
    writeClock (CM_PWMCTL, PASSWD | ENAB | SRC_OSC);

    writePwm (CTL, backupctl);
    while (! (readClock (CM_PWMCTL) & BUSY)) {
      clk.delayMicroseconds (1);  // Wait for generator to start
    }
  }

  // -------------------------------------------------------------------------
  // internal
  uint32_t SocPwm::Bcm2835Engine::frequencyDivisor (long freq) {
    double value = ceil (static_cast<double> (clkFreq) / freq /
                         range());

    return value;
  }

  // -------------------------------------------------------------------------
  const std::string &
  SocPwm::Bcm2835Engine::deviceName() const {
    static std::string dn ("Bcm2835Pwm");

    return dn;
  }
}
/* ========================================================================== */
