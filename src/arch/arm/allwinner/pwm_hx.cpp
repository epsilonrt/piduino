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
#include <map>
#include <piduino/clock.h>
#include "pwm_hx.h"
#include "gpio/socpwm_p.h"
#include "config.h"

namespace Piduino {

  namespace AllWinnerHx {

    // -------------------------------------------------------------------------
    const std::map<unsigned long, uint8_t> divisorMap = {
      { 1, PWM_CH0_PRESCAL_1},
      { 120, PWM_CH0_PRESCAL_120},
      { 180, PWM_CH0_PRESCAL_180},
      { 240, PWM_CH0_PRESCAL_240},
      { 360, PWM_CH0_PRESCAL_360},
      { 480, PWM_CH0_PRESCAL_480},
      { 12000, PWM_CH0_PRESCAL_12K},
      { 24000, PWM_CH0_PRESCAL_24K},
      { 36000, PWM_CH0_PRESCAL_36K},
      { 48000, PWM_CH0_PRESCAL_48K},
      { 72000, PWM_CH0_PRESCAL_72K}
    };

    // -------------------------------------------------------------------------
    //
    //                     AllWinnerHx::PwmEngine Class
    //
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // public
    PwmEngine::PwmEngine (SocPwm::Private * d, Pin * p) :
      SocPwm::Engine (d, p),
      periodReg (PWM_CH0_PERIOD),
      timeout_us (10000) {

      if (p) {

        if (p->mode() != Pin::ModePwm) {

          pin = nullptr;
        }
        else {

          open (IoDevice::ReadWrite); // set active state to 1
          close();
        }
      }
    }

    // -------------------------------------------------------------------------
    PwmEngine::~PwmEngine() = default;

    // -------------------------------------------------------------------------
    bool
    PwmEngine::open (IoDevice::OpenMode m) {

      if (pwm.open (Io1Base + PwmOffset, IoMap::pageSize())) {
        uint32_t ctl = readReg (PWM_CH_CTRL);

        ctl &= ~ (PWM0_RDY | PWM0_BYPASS | PWM_CHANNEL0_MODE);
        ctl |= PWM_CH0_ACT_STA;
        writeReg (PWM_CH_CTRL, ctl);

        return (ctl == (readReg (PWM_CH_CTRL) & ~PWM0_RDY));
      }
      return false;
    }

    // -------------------------------------------------------------------------
    void
    PwmEngine::close() {

      pwm.close();
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    long
    PwmEngine::read() {

      return value();
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    bool
    PwmEngine::write (long v) {

      return setValue (v);
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    void
    PwmEngine::setEnable (bool enable) {
      uint32_t ctl = readReg (PWM_CH_CTRL);

      if (enable) {

        ctl |= PWM_CH0_EN | SCLK_CH0_GATING;
      }
      else {

        ctl &= ~ (PWM_CH0_EN | SCLK_CH0_GATING);
      }
      writeReg (PWM_CH_CTRL, ctl);
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    bool
    PwmEngine::isEnabled () const {

      return (readReg (PWM_CH_CTRL) & (PWM_CH0_EN | SCLK_CH0_GATING)) ==
             (PWM_CH0_EN | SCLK_CH0_GATING);
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
    PwmEngine::range() const {

      if (hasPin()) {
        long r = period() >> 16;
        return (r == 0) ? 0 : r + 1;
      }
      return -1;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    bool
    PwmEngine::setRange (long r) {

      if (hasPin()) {
        
        if (r < 2) {
          r = 2;
        }
        r &= 0xFFFF;
        uint32_t p = (period() & ~0xFFFF0000) | (static_cast<uint32_t> (r - 1) << 16);
        if (setPeriod (p)) {
          if (value() > max()) {
            return setValue(max());
          }
          return true;
        }
      }
      return false;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    long
    PwmEngine::max() const {

      if (hasPin()) {
        return range() - 1;
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
        return setRange (1L << resolution);
      }
      return false;
    }

    // -------------------------------------------------------------------------
    // isOpen() checked before calling this function
    // hasPin() NOT checked before calling this function
    long
    PwmEngine::frequency() const {

      if (hasPin()) {
        uint32_t d = clockDivisor();
        uint16_t r = range();

        if ( (d != 0) && (r != 0)) {
          return PLL_CLOCK_FREQ / d / r;
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
    unsigned long
    PwmEngine::clockDivisor() const {

      uint8_t divBits = readReg (PWM_CH_CTRL) & PWM_CH0_PRESCAL_MASK;
      for (auto it = divisorMap.cbegin(); it != divisorMap.cend(); ++it) {
        if (it->second == divBits) {
          return it->first;
        }
      }
      return 0; // error
    }

    // -------------------------------------------------------------------------
    // private
    void
    PwmEngine::setClockDivisor (unsigned long div) {

      if (div < PLL_CLOCK_DIVISOR_MIN) {

        div = PLL_CLOCK_DIVISOR_MIN;
      }
      else if (div > PLL_CLOCK_DIVISOR_MAX) {

        div = PLL_CLOCK_DIVISOR_MAX;
      }

      div = findValidDivisor (div);

      bool previousState = isEnabled();
      if (previousState) {
        setEnable (false);
      }

      uint32_t ctl = (readReg (PWM_CH_CTRL) & ~PWM_CH0_PRESCAL_MASK) | divisorMap.at (div);
      writeReg (PWM_CH_CTRL, ctl);

      if (previousState) {
        setEnable (previousState);
      }
    }

    // -------------------------------------------------------------------------
    // private
    unsigned long PwmEngine::frequencyDivisor (long freq) {
      double div = ceil (static_cast<double> (PLL_CLOCK_FREQ) / freq /
                         range());

      return findValidDivisor (div);
    }

    // -------------------------------------------------------------------------
    // private
    long
    PwmEngine::divisorFrequency (unsigned long divisor) {

      return PLL_CLOCK_FREQ / divisor / range();
    }

    // -------------------------------------------------------------------------
    // private
    bool
    PwmEngine::setPeriod (uint32_t p) {
#if 0
      // I did not understand why the ready bit remains high after the first
      // write in the period register ...
      int timeout = timeout_us;
      while (readReg (PWM_CH_CTRL) & PWM0_RDY) {
        if (timeout <= 0) {
          return false;
        }
        timeout -= 10;
        clk.delayMicroseconds (10);
      }
#endif
      writeReg (periodReg, p);
      return true;
    }

    // -------------------------------------------------------------------------
    // private
    uint16_t
    PwmEngine::value() const {

      return period() & 0xFFFF;
    }

    // -------------------------------------------------------------------------
    // private
    bool
    PwmEngine::setValue (uint16_t v)  {
      uint32_t p = (period() & ~0x0000FFFF) | static_cast<uint32_t> (v);
      return setPeriod (p);
    }

    // -------------------------------------------------------------------------
    // private
    unsigned long
    PwmEngine::findValidDivisor (unsigned long div) {
      unsigned long ret = 0;

      if (div) {

        auto search = divisorMap.find (div);
        if (search == divisorMap.end()) {
          long fTarget = divisorFrequency (div);
          long delta;

          auto it = divisorMap.cbegin();
          ret = it->first;
          delta = std::abs (fTarget - divisorFrequency (ret));
          ++it;

          for (; it != divisorMap.cend(); ++it) {
            long d = std::abs (fTarget - divisorFrequency (it->first));
            if (d < delta) {

              ret = it->first;
              delta = d;
            }
          }
        }
        else {

          ret = div;
        }
      }
      return ret;
    }

    // -------------------------------------------------------------------------
    // private
    void
    PwmEngine::writeReg (size_t offset, uint32_t value) {
      *pwm.io (offset) = value;
      clk.delay (1);
    }

    // -------------------------------------------------------------------------
    // private
    uint32_t
    PwmEngine::readReg (size_t offset) const {
      return *pwm.io (offset);
    }

    // -------------------------------------------------------------------------
    // private
    uint32_t
    PwmEngine::period() const {
      return readReg (periodReg);
    }

    // -------------------------------------------------------------------------
    const std::string &
    PwmEngine::deviceName() const {
      static std::string dn ("AllWinnerHxPwm");

      return dn;
    }
  }
}
/* ========================================================================== */
