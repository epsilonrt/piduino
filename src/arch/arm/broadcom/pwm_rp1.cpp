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
#include <iomanip> // for std::hex
#include <climits>
#include <cmath>
#include "pwm_rp1.h"
#include "gpio/socpwm_p.h"
#include <piduino/clock.h>
#include <piduino/database.h>
#include "config.h"

namespace Piduino {

  namespace Rp1 {

    // -------------------------------------------------------------------------
    //
    //                     Rp1::PwmEngine Class
    //
    // -------------------------------------------------------------------------
    IoMap PwmEngine::iomap; // Shared IoMap instance for memory mapping.
    volatile uint32_t *PwmEngine::clock = nullptr; // Pointer to clock registers.
    std::atomic<int> PwmEngine::instanceCount{0}; // Counter for active instances

    // -------------------------------------------------------------------------
    // public
    PwmEngine::PwmEngine (SocPwm::Private *d, Pin *p) :
      SocPwm::Engine (d, p), pwm (nullptr), channel (0), base (Pwm0Offset), clkFreq (PwmClkOscFreq) {

      ++instanceCount; // Increment instance counter

      if (p) {

        if (p->mode() == Pin::ModePwm) {

          switch (p->mcuNumber()) {
            case 12: // PWM00
              channel = 0;
              break;
            case 13: // PWM01
              channel = 1;
              break;
            case 18: // PWM02
              channel = 2;
              break;
            case 19: // PWM03
              channel = 3;
              break;
            default:
              throw std::invalid_argument ("Invalid PWM pin number: " + std::to_string (p->mcuNumber()));
          }

          dataReg = PwmChanBlockBase + PwmChanBlockSize * channel + PwmChanDutyReg;
          rngReg = PwmChanBlockBase + PwmChanBlockSize * channel + PwmChanRangeReg;
          ctlReg = PwmChanBlockBase + PwmChanBlockSize * channel + PwmChanCtrlReg;
          #ifndef NDEBUG
          // Debugging information
          std::cout << "PwmEngine initialized on pin iNo" << p->logicalNumber() << std::endl
                    << "Channel Number   : " << channel << std::endl
                    << "Clock Frequency  : " << clkFreq << " Hz" << std::endl
                    << "PWM Base Offset  : 0x" << std::hex << base << std::endl
                    << "Control Register Offset: 0x" << ctlReg << std::endl
                    << "Range Register Offset  : 0x" << rngReg  << std::endl
                    << "Data Register Offset   : 0x" << dataReg << std::dec << std::endl;
          #endif
        }
        else {

          // Default construction for non-PWM pins
          pin = nullptr;
        }
      }
    }

    // -------------------------------------------------------------------------
    PwmEngine::~PwmEngine() {

      --instanceCount; // Decrement instance counter

      // Close iomap when last instance is destroyed
      if (instanceCount == 0 && iomap.isOpen()) {
        iomap.close();
        clock = nullptr;
      }
    }

    // -------------------------------------------------------------------------
    bool
    PwmEngine::open (IoDevice::OpenMode m) {

      if (!iomap.isOpen()) {
        std::string pcieDevice = findPCIeDevice();
        if (pcieDevice.empty()) {
          throw std::system_error (std::make_error_code (std::errc::no_such_device),
                                   "No PCIe device found for PWM access");
        }

        iomap.open (pcieDevice.c_str(), PCIeBlockSize);
        clock = iomap.io (ClockOffset);
      }

      pwm = iomap.io (base); // RP1 start address of map memory for pwm
      return true;
    }

    // -------------------------------------------------------------------------
    void
    PwmEngine::close() {

      pwm = nullptr; // reset the pointer to the PWM registers
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
      uint32_t ctl = readPwm (PwmGlobalCtrlReg);

      if (enable) {

        ctl |= BIT (channel) | PwmGlobalCtrlSetUpdate;
        writePwm (ctlReg, PwmChanCtrlModeTraillingEdgeMs | PwmChanCtrlFifoPopMask);
      }
      else {
        writePwm (ctlReg, 0); // Disable channel
        ctl = (ctl & ~BIT (channel)) | PwmGlobalCtrlSetUpdate;
      }
      writePwm (PwmGlobalCtrlReg, ctl);
      clk.delayMicroseconds (10);
    }

    // -------------------------------------------------------------------------
    // hasPin() checked before calling this function
    bool
    PwmEngine::isEnabled () const {

      return (readPwm (ctlReg) == (PwmChanCtrlModeTraillingEdgeMs | PwmChanCtrlFifoPopMask)) &&
             (readPwm (PwmGlobalCtrlReg) & BIT (channel)) == BIT (channel);
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
        return setRange (1L << resolution);
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

        writePwm (rngReg,  r);
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
          return clkFreq / div / range();
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

      uint32_t div = readClock (Pwm0ClkDivIntReg);
      return div;
    }

    // -------------------------------------------------------------------------
    // private
    void
    PwmEngine::setClockDivisor (uint32_t div) {

      if (div > PwmClkDivIntMax) {

        div = PwmClkDivIntMax;
      }

      uint32_t backupctl = readPwm (PwmChanCtrlReg);

      writePwm (PwmChanCtrlReg, 0); // Turn off PWM before changing
      clk.delayMicroseconds (10);

      // Disable clock
      writeClock (Pwm0ClkCtrlReg, PwmClkCtrlMagicDisable);
      clk.delayMicroseconds (110);

      if (div >= PwmClkDivIntMin) {

        // Wait for generator to stop ?

        writeClock (Pwm0ClkDivIntReg, div); // Set integer part of clock divisor
        writeClock (Pwm0ClkDivFracReg, 0); // Set fractional part of clock divisor
        writeClock (Pwm0SelReg, 1); // Select the PWM clock source
        writeClock (Pwm0ClkCtrlReg, PwmClkCtrlMagicEnable); // Enable clock
        writePwm (PwmChanCtrlReg, backupctl); // Restore channel control register
        clk.delayMicroseconds (10); // Wait for clock to stabilize
      }

    }

    // -------------------------------------------------------------------------
    // private
    uint32_t PwmEngine::frequencyDivisor (long freq) {
      double value = ceil (static_cast<double> (clkFreq) / freq /
                           range());

      return value;
    }

    // -------------------------------------------------------------------------
    const std::string &
    PwmEngine::deviceName() const {
      static std::string dn ("Rp1Pwm");

      return dn;
    }

  } // namespace Rp1

} // namespace Piduino
/* ========================================================================== */
