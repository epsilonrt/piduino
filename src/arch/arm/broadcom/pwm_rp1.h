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

#include <atomic>
#include <piduino/iomap.h>
#include "gpio/socpwm_p.h"
#include "rp1.h"

namespace Piduino {

  class SocPwm::Rp1Engine  : public SocPwm::Private {

    public:
      Rp1Engine (SocPwm *q, Pin *p);
      ~Rp1Engine();

      virtual bool open (IoDevice::OpenMode mode) override;
      virtual void close() override;
      virtual const std::string &deviceName() const override;

      virtual long read() override;
      virtual bool write (long value) override;
      virtual void setEnable (bool enable) override;
      virtual bool isEnabled () const override;
      virtual long range() const override;
      virtual long setRange (long range) override;
      virtual long max (bool differential = false) const override;
      virtual long min (bool differential = false) const override;
      virtual long frequency() const override;
      virtual long setFrequency (long freq) override;

      // ----------- internal methods -----------
      uint32_t clockDivisor() const;
      void setClockDivisor (uint32_t div);
      uint32_t frequencyDivisor (long freq);

      inline uint32_t readPwm (size_t offset) const {
        return pwm[offset];
      }
      inline void writePwm (size_t offset, uint32_t value) {
        pwm[offset] = value;
      }
      inline uint32_t readClock (size_t offset) const {
        return clock[offset];
      }
      inline void writeClock (size_t offset, uint32_t value) {
        clock[offset] = value;
      }

      //----------- data members -----------
      // the private data members
      volatile uint32_t *pwm;  // Pointer to the PWM registers.

      uint32_t channel; // PWM channel number (0 to 3).
      off_t base; // Base address for the PWM registers.

      off_t rngReg;
      off_t dataReg;
      off_t ctlReg; // Control register for the PWM channel.
      uint32_t clkFreq;
      Rp1::PwmChanCtrlMode mode;

      // --------------------------------------------------------------------
      // the private static data members
      static IoMap iomap; // Shared IoMap instance for memory mapping.
      static volatile uint32_t *clock;  // Pointer to the clock registers.
      static std::atomic<int> instanceCount; // Counter for active instances

      PIMP_DECLARE_PUBLIC (SocPwm)
  };
}

/* ========================================================================== */
