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
#include <piduino/iomap.h>
#include "gpio/socpwm_p.h"
#include "hx.h"

namespace Piduino {

  class SocPwm::HxEngine  : public SocPwm::Private {

    public:
      HxEngine (SocPwm *q, Pin *p);
      ~HxEngine();

      bool open (IoDevice::OpenMode mode) override;
      void close() override;
      const std::string &deviceName() const override;

      long read() override;
      bool write (long value) override;
      void setEnable (bool enable) override;
      bool isEnabled () const override;
      long max (bool differential = false) const override;
      long min (bool differential = false) const override;
      long range() const override;
      long setRange (long range) override;
      long frequency() const override;
      long setFrequency (long freq) override;

      // ----------- internal methods -----------
      uint32_t period() const;
      bool setPeriod (uint32_t period);
      uint16_t value() const;
      bool setValue (uint16_t v);

      unsigned long clockDivisor() const;
      void setClockDivisor (unsigned long div);
      unsigned long frequencyDivisor (long freq);
      long divisorFrequency (unsigned long divisor);
      unsigned long findValidDivisor (unsigned long div);

      void writeReg (size_t offset, uint32_t value);
      uint32_t readReg (size_t offset) const;

      // ------------- data members -------------
      IoMap pwm;
      size_t periodReg;
      int timeout_us;
  };
}

/* ========================================================================== */
