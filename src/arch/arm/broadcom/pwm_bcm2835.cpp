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
#include <climits>
#include <cmath>
#include "pwm_bcm2835.h"
#include "gpio/socpwm_p.h"
#include <piduino/clock.h>
#include "config.h"

namespace Piduino {

  namespace Bcm2835 {

    // -------------------------------------------------------------------------
    //
    //                     Bcm2835::PwmEngine Class
    //
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // public
    PwmEngine::PwmEngine (SocPwm::Private * d, Pin * p) :
      SocPwm::Engine (d, p),
      rngReg (RNG1),
      dataReg (DAT1),
      ctlMasqInit (RPTL1 | SBIT1 | POLA1),
      ctlMasqStartStop (MSEN1 | PWEN1) {

      if (p) {

        if (p->mode() == Pin::ModePwm) {

          if ( (p->mcuNumber() == 13) || (p->mcuNumber() == 19)) {

            rngReg = RNG2;
            dataReg = DAT2;
            ctlMasqStartStop = MSEN2 | PWEN2;
            ctlMasqInit = RPTL2 | SBIT2 | POLA2;
          }
        }
        else {

          pin = nullptr;
        }
      }
    }

    // -------------------------------------------------------------------------
    PwmEngine::~PwmEngine() = default;

    // -------------------------------------------------------------------------
    bool
    PwmEngine::open (IoDevice::OpenMode m) {

      return (pwm.open (iobase() + PwmOffset, MapBlockSize) &&
              clock.open (iobase() + ClockOffset, MapBlockSize)) ;
    }

    // -------------------------------------------------------------------------
    void
    PwmEngine::close() {

      pwm.close();
      clock.close();
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    long
    PwmEngine::read() {

      return readPwm (dataReg);
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    bool
    PwmEngine::write (long value) {

      writePwm (dataReg, value);
      return true;
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    void
    PwmEngine::setEnable (bool enable) {
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
    // hasPin() checked before calling this function
    bool
    PwmEngine::isEnabled () const {

      return (readPwm (CTL) & ctlMasqStartStop) == ctlMasqStartStop;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    int
    PwmEngine::resolution() const {

      if (hasPin()) {
        long r = range();

        if (r > 0) {
          return log2 (r);
        }
      }
      return -1;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    long
    PwmEngine::max() const {

      if (hasPin()) {
        return range();
      }
      return -1;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    long
    PwmEngine::min() const {

      return 0;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    bool
    PwmEngine::setResolution (int resolution) {

      if (hasPin()) {
        if (resolution < PWM_RESOLUTION_MIN) {

          resolution = PWM_RESOLUTION_MIN;
        }
        else if (resolution > PWM_RESOLUTION_MAX) {

          resolution = PWM_RESOLUTION_MAX;
        }
        return setRange(1L << resolution);
      }
      return false;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    long
    PwmEngine::range() const {

      if (hasPin()) {

        return readPwm (rngReg);
      }
      return -1;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    bool
    PwmEngine::setRange (long r) {
      
      if (hasPin()) {

        writePwm (static_cast<uint32_t>(rngReg),  r);
        return true;
      }
      return false;
    }
    
    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    long
    PwmEngine::frequency() const {

      if (hasPin()) {
        uint32_t div = clockDivisor();

        if (div != 0) {
          return PLL_CLOCK_FREQ / div / range();
        }
      }
      return 0;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    bool
    PwmEngine::setFrequency (long frequency) {

      if ( (frequency > 0) && hasPin()) {

        setClockDivisor (frequencyDivisor (frequency));
        return true;
      }
      return false;
    }

    // -------------------------------------------------------------------------
    // private
    uint32_t
    PwmEngine::clockDivisor() const {

      uint32_t div = readClock (CM_PWMDIV) >> 12;
      return div;
    }

    // -------------------------------------------------------------------------
    // private
    void
    PwmEngine::setClockDivisor (uint32_t div) {

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

      //  clk.delayMicroseconds(10);
      while (readClock (CM_PWMCTL) & BUSY)
        ; // Wait for generator to stop

      // set frequency
      writeClock (CM_PWMDIV, PASSWD | (div << 12));

      // enable clock
      writeClock (CM_PWMCTL, PASSWD | ENAB | SRC_OSC);

      writePwm (CTL, backupctl);
      while (! (readClock (CM_PWMCTL) & BUSY))
        ;  // Wait for generator to start
    }

    // -------------------------------------------------------------------------
    // private
    uint32_t PwmEngine::frequencyDivisor (long freq) {
      double value = ceil (static_cast<double> (PLL_CLOCK_FREQ) / freq /
                           range());

      return value;
    }

    // -------------------------------------------------------------------------
    const std::string &
    PwmEngine::deviceName() const {
      static std::string dn ("Bcm2835Pwm");

      return dn;
    }

  }
}
/* ========================================================================== */
