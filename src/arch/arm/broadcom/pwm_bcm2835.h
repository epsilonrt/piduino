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

#include <piduino/socpwm.h>
#include <piduino/iomap.h>
#include "bcm2835.h"
#include "gpio/socpwm_p.h"

namespace Piduino {

  class SocPwm::Bcm2835Engine  : public SocPwm::Private {

    public:
      Bcm2835Engine (SocPwm *q, Pin *p);
      ~Bcm2835Engine();

      bool open (IoDevice::OpenMode mode) override;
      void close() override;
      const std::string &deviceName() const override;

      long read() override;
      bool write (long value) override;
      void setEnable (bool enable) override;
      bool isEnabled () const override;

      long max() const override;
      long setRange (long range) override;
      long frequency() const override;
      long setFrequency (long freq) override;

      // ----------- internal methods -----------
      uint32_t clockDivisor() const;
      void setClockDivisor (uint32_t div);
      uint32_t frequencyDivisor (long freq);

      inline uint32_t readPwm (size_t offset) const {
        return *pwm.io (offset);
      }
      inline void writePwm (size_t offset, uint32_t value) {
        *pwm.io (offset) = value;
      }
      inline uint32_t readClock (size_t offset) const {
        return *clock.io (offset);
      }
      inline void writeClock (size_t offset, uint32_t value) {
        *clock.io (offset) = value;
      }

      //----------- data members -----------
      IoMap pwm;
      IoMap clock;
      size_t rngReg;
      size_t dataReg;
      uint32_t ctlMasqInit;
      uint32_t ctlMasqStartStop;
      bool is2711;
      uint32_t clkFreq;

      PIMP_DECLARE_PUBLIC (SocPwm)
  };
}

/* ========================================================================== */
