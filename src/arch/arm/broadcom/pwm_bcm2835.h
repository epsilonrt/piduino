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
#ifndef PIDUINO_ARCH_ARM_BROADCOM_PWM_BCM2835_H
#define PIDUINO_ARCH_ARM_BROADCOM_PWM_BCM2835_H

#include <piduino/socpwm.h>
#include <piduino/iomap.h>
#include "bcm2835.h"

namespace Piduino {

  namespace Bcm2835 {

    /**
     * @class Bcm2835::PwmEngine
     * @brief
     */
    class PwmEngine  : public SocPwm::Engine {

      public:
        PwmEngine (SocPwm::Private * d, Pin * p);
        ~PwmEngine();

        bool open (IoDevice::OpenMode mode);
        void close();
        const std::string & deviceName() const;

        // isOpen() checked before calling this functions
        long frequency() const;
        int  resolution() const;
        long range() const;
        long max() const;
        long min() const;
        bool setFrequency (long freq);
        bool setResolution (int resolution);
        bool setRange (long range);

        // hasPin() checked before calling this functions
        long read();
        bool write (long value);
        void setEnable (bool enable);
        bool isEnabled () const;

      private:
        IoMap pwm;
        IoMap clock;
        size_t rngReg;
        size_t dataReg;
        uint32_t ctlMasqInit;
        uint32_t ctlMasqStartStop;
        bool is2711;
        uint32_t clkFreq;

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
    };
  }
}

/* ========================================================================== */
#endif /* PIDUINO_ARCH_ARM_BROADCOM_PWM_BCM2835_H defined */
