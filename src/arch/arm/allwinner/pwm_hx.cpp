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
#include <map>
#include <piduino/clock.h>
#include "pwm_hx.h"
#include "gpio/socpwm_p.h"
#include "config.h"

namespace Piduino {

  using namespace AllWinnerHx;

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
  //                     SocPwm::HxEngine Class
  //
  // -------------------------------------------------------------------------

  // -------------------------------------------------------------------------
  // public
  SocPwm::HxEngine::HxEngine (SocPwm *q, Pin *p) :
    SocPwm::Private (q, p, hasFrequency | hasSetFrequency | hasResolution | hasSetResolution | hasRange | hasSetRange),
    periodReg (PWM_CH0_PERIOD),
    timeout_us (10000) {

    if (p->mcuNumber() == 5) {
      
      p->setMode (Pin::ModePwm);
      open (IoDevice::ReadWrite); // set active state to 1
      close();
    }
    else {

      throw std::runtime_error (EXCEPTION_MSG ("Hardware PWM not supported on this pin"));
    }
  }

  // -------------------------------------------------------------------------
  SocPwm::HxEngine::~HxEngine() = default;

  // -------------------------------------------------------------------------
  // override
  const std::string &
  SocPwm::HxEngine::deviceName() const {
    static std::string dn ("AllWinnerHxPwm");

    return dn;
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::HxEngine::open (IoDevice::OpenMode m) {

    if (pwm.open (Io1Base + PwmOffset, IoMap::pageSize())) {
      uint32_t ctl = readReg (PWM_CH_CTRL);

      ctl &= ~ (PWM0_RDY | PWM0_BYPASS | PWM_CHANNEL0_MODE);
      ctl |= PWM_CH0_ACT_STA;
      writeReg (PWM_CH_CTRL, ctl);

      if (ctl == (readReg (PWM_CH_CTRL) & ~PWM0_RDY)) {
        return SocPwm::Private::open (m);
      }
    }
    return false;
  }

  // -------------------------------------------------------------------------
  // override
  void
  SocPwm::HxEngine::close() {

    pwm.close();
    SocPwm::Private::close();
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::HxEngine::read() {

    return value();
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::HxEngine::write (long v) {

    return setValue (v);
  }

  // -------------------------------------------------------------------------
  // override
  void
  SocPwm::HxEngine::setEnable (bool enable) {
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
  // override
  bool
  SocPwm::HxEngine::isEnabled () const {

    return (readReg (PWM_CH_CTRL) & (PWM_CH0_EN | SCLK_CH0_GATING)) ==
           (PWM_CH0_EN | SCLK_CH0_GATING);
  }


  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::HxEngine::range() const {

    long r = period() >> 16;
    return (r == 0) ? 0 : r + 1;
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::HxEngine::max() const {

    return range() - 1;
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::HxEngine::setRange (long r) {

    if (r < 2) {
      r = 2;
    }
    r &= 0xFFFF;
    uint32_t p = (period() & ~0xFFFF0000) | (static_cast<uint32_t> (r - 1) << 16);
    if (setPeriod (p)) {
      if (value() > max()) {
        return setValue (max());
      }
      return range();
    }
    return -1; // error
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::HxEngine::frequency() const {
    uint32_t d = clockDivisor();
    uint16_t r = range();

    if ( (d != 0) && (r != 0)) {
      return PLL_CLOCK_FREQ / d / r;
    }
    return 0;
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::HxEngine::setFrequency (long f) {

    if (f > 0)  {

      setClockDivisor (frequencyDivisor (f));
      return frequency(); // Return the new frequency
    }
    return 0; // Invalid frequency, return 0
  }

  // -------------------------------------------------------------------------
  // internal
  unsigned long
  SocPwm::HxEngine::clockDivisor() const {

    uint8_t divBits = readReg (PWM_CH_CTRL) & PWM_CH0_PRESCAL_MASK;
    for (auto it = divisorMap.cbegin(); it != divisorMap.cend(); ++it) {
      if (it->second == divBits) {
        return it->first;
      }
    }
    return 0; // error
  }

  // -------------------------------------------------------------------------
  // internal
  void
  SocPwm::HxEngine::setClockDivisor (unsigned long div) {

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
  // internal
  unsigned long SocPwm::HxEngine::frequencyDivisor (long freq) {
    double div = ceil (static_cast<double> (PLL_CLOCK_FREQ) / freq /
                       range());

    return findValidDivisor (div);
  }

  // -------------------------------------------------------------------------
  // internal
  long
  SocPwm::HxEngine::divisorFrequency (unsigned long divisor) {

    return PLL_CLOCK_FREQ / divisor / range();
  }

  // -------------------------------------------------------------------------
  // internal
  bool
  SocPwm::HxEngine::setPeriod (uint32_t p) {
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
  // internal
  uint16_t
  SocPwm::HxEngine::value() const {

    return period() & 0xFFFF;
  }

  // -------------------------------------------------------------------------
  // internal
  bool
  SocPwm::HxEngine::setValue (uint16_t v)  {
    uint32_t p = (period() & ~0x0000FFFF) | static_cast<uint32_t> (v);
    return setPeriod (p);
  }

  // -------------------------------------------------------------------------
  // internal
  unsigned long
  SocPwm::HxEngine::findValidDivisor (unsigned long div) {
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
  // internal
  void
  SocPwm::HxEngine::writeReg (size_t offset, uint32_t value) {
    *pwm.io (offset) = value;
    clk.delay (1);
  }

  // -------------------------------------------------------------------------
  // internal
  uint32_t
  SocPwm::HxEngine::readReg (size_t offset) const {
    return *pwm.io (offset);
  }

  // -------------------------------------------------------------------------
  // internal
  uint32_t
  SocPwm::HxEngine::period() const {
    return readReg (periodReg);
  }

}
/* ========================================================================== */
