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
#include <piduino/clock.h>
#include <piduino/database.h>
#include "config.h"

namespace Piduino {

  using namespace Rp1;

  // -------------------------------------------------------------------------
  //
  //                     Pwm::Rp1Engine Class
  //
  // -------------------------------------------------------------------------
  IoMap SocPwm::Rp1Engine::iomap; // Shared IoMap instance for memory mapping.
  volatile uint32_t *SocPwm::Rp1Engine::clock = nullptr; // Pointer to clock registers.
  std::atomic<int> SocPwm::Rp1Engine::instanceCount{0}; // Counter for active instances

  // -------------------------------------------------------------------------
  SocPwm::Rp1Engine::Rp1Engine (SocPwm *q, Pin *p) :
    SocPwm::Private (q, p, hasFrequency | hasSetFrequency | hasResolution | hasSetResolution | hasRange | hasSetRange),
    pwm (nullptr), channel (0), base (Pwm0Offset), clkFreq (PwmClkOscFreq), mode (TrailingEdgeMsMode) {


    p->setMode (Pin::ModePwm);  // throw an exception if pin is not PWM capable
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
        throw std::runtime_error (EXCEPTION_MSG ("Hardware PWM not supported on this pin"));
    }

    ++instanceCount; // Increment instance counter
    dataReg = PwmChanBlockBase + PwmChanBlockSize * channel + PwmChanDutyReg;
    rngReg = PwmChanBlockBase + PwmChanBlockSize * channel + PwmChanRangeReg;
    ctlReg = PwmChanBlockBase + PwmChanBlockSize * channel + PwmChanCtrlReg;
    #ifndef NDEBUG
    // Debugging information
    std::cout << "Rp1Engine initialized on pin iNo" << p->logicalNumber() << std::endl
              << "Channel Number   : " << channel << std::endl
              << "Clock Frequency  : " << clkFreq << " Hz" << std::endl
              << "PWM Base Offset  : 0x" << std::hex << base << std::endl
              << "Control Register Offset: 0x" << ctlReg << std::endl
              << "Range Register Offset  : 0x" << rngReg  << std::endl
              << "Data Register Offset   : 0x" << dataReg << std::dec << std::endl;
    #endif
  }

  // -------------------------------------------------------------------------
  SocPwm::Rp1Engine::~Rp1Engine() {

    --instanceCount; // Decrement instance counter

    // Close iomap when last instance is destroyed
    if (instanceCount == 0 && iomap.isOpen()) {
      iomap.close();
      clock = nullptr;
    }
  }

  // -------------------------------------------------------------------------
  // override
  const std::string &
  SocPwm::Rp1Engine::deviceName() const {
    static std::string dn ("Rp1Pwm");

    return dn;
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::Rp1Engine::open (IoDevice::OpenMode m) {

    if (!iomap.isOpen()) {
      std::string pcieDevice = findPCIeDevice();

      if (pcieDevice.empty()) {
        throw std::system_error (std::make_error_code (std::errc::no_such_device),
                                 "No PCIe device found for PWM access");
      }

      if (!iomap.open (pcieDevice.c_str(), PCIeBlockSize)) {
        return false; // Failed to open IoMap
      }
      clock = iomap.io (ClockOffset);
    }

    pwm = iomap.io (base); // RP1 start address of map memory for pwm
    return SocPwm::Private::open (m);
  }

  // -------------------------------------------------------------------------
  // override
  void
  SocPwm::Rp1Engine::close() {

    pwm = nullptr; // reset the pointer to the PWM registers
    SocPwm::Private::close();
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::read() {

    return readPwm (dataReg);
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::Rp1Engine::write (long value) {

    writePwm (dataReg, value);
    return true;
  }

  // -------------------------------------------------------------------------
  // override
  void
  SocPwm::Rp1Engine::setEnable (bool enable) {
    uint32_t ctl = readPwm (PwmGlobalCtrlReg);

    if (enable) {

      ctl |= BIT (channel) | PwmGlobalCtrlSetUpdate;
      writePwm (ctlReg, mode | PwmChanCtrlFifoPopMask);
    }
    else {
      writePwm (ctlReg, 0); // Disable channel
      ctl = (ctl & ~BIT (channel)) | PwmGlobalCtrlSetUpdate;
    }
    writePwm (PwmGlobalCtrlReg, ctl);
    clk.delayMicroseconds (10);
  }

  // -------------------------------------------------------------------------
  // override
  bool
  SocPwm::Rp1Engine::isEnabled () const {

    return (readPwm (ctlReg) == (mode | PwmChanCtrlFifoPopMask)) &&
           (readPwm (PwmGlobalCtrlReg) & BIT (channel)) == BIT (channel);
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::max (bool differential) const {

    return range() + 1;
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::min (bool differential) const {
    
    return 0; // Minimum value for PWM is always 0
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::range() const {

    return readPwm (rngReg);
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::setRange (long r) {

    writePwm (rngReg,  r);
    return range();
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::frequency() const {
    uint32_t div = clockDivisor();

    if (div != 0) {
      return clkFreq / div / range();
    }
    return 0;
  }

  // -------------------------------------------------------------------------
  // override
  long
  SocPwm::Rp1Engine::setFrequency (long f) {

    if (f > 0) {

      setClockDivisor (frequencyDivisor (f));
      return frequency(); // Return the new frequency
    }
    return 0; // Invalid frequency, return 0
  }

  // -------------------------------------------------------------------------
  // internal
  uint32_t
  SocPwm::Rp1Engine::clockDivisor() const {

    uint32_t div = readClock (Pwm0ClkDivIntReg);
    return div;
  }

  // -------------------------------------------------------------------------
  // internal
  void
  SocPwm::Rp1Engine::setClockDivisor (uint32_t div) {

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
  // internal
  uint32_t SocPwm::Rp1Engine::frequencyDivisor (long freq) {
    double value = ceil (static_cast<double> (clkFreq) / freq /
                         range());

    return value;
  }

} // namespace Piduino
/* ========================================================================== */
