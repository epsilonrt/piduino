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
#include <thread>
#include <future>
#include <memory>
#include <atomic>
#include <cmath>
#include <piduino/softpwm.h>
#include "pwm_p.h"

namespace Piduino {

  class SoftPwm::Private  : public Pwm::Private {

    public:
      Pin *pin; // pointer to the pin associated with this device
      std::atomic<long> mark; // current mark value
      std::atomic<long> range; // current range value
      std::atomic<long> tick; // range * tick = period
      std::promise<void> killGenThread; // promise to signal the generator thread to stop
      std::thread genThread; // thread for generating the PWM signal
      static std::string deviceName; // static device name for all instances

      // SoftPwm::Private
      Private (SoftPwm *q, Pin *pin, long range, long frequency);
      virtual ~Private();
      static void *generator (std::future<void> run, SoftPwm::Private *d);
      // Calculate the tick or frequency
      static long calculate (long o1, long o2) {
        double r = ceil (1e6 / (static_cast<double> (o1) * o2));
        return static_cast<long> (r);
      }

      // IoDevice::Private
      // virtual bool isOpen() const;

      // Converter::Private
      virtual bool open (OpenMode mode);
      virtual void close();
      virtual long read();
      virtual bool write (long value);
      virtual long max() const;
      virtual long min() const;

      PIMP_DECLARE_PUBLIC (SoftPwm)
  };
}

/* ========================================================================== */
