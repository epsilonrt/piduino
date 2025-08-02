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
#include "converter_p.h"

namespace Piduino {

  class SoftPwm::Private  : public Converter::Private {

    public:
      Private (SoftPwm *q, Pin *pin, long range, long frequency);
      virtual ~Private();

      // --------------------------------------------------------------------------
      virtual const std::string &deviceName() const override {
        static const std::string name = "SoftPwm";
        return name;
      }

      // ---------------------------------------------------------------------------
      virtual bool open (OpenMode m) override {

        pin->setMode (Pin::ModeOutput); // set the pin mode to Output
        pin->write (false); // ensure the pin is LOW at start
        return Converter::Private::open (m);
      }

      // ---------------------------------------------------------------------------
      virtual void close() override {

        setEnable (false); // stop the generator thread if running
        pin->release(); // release the pin
        Converter::Private::close();
      }

      // ---------------------------------------------------------------------------
      virtual long read() override {
        return mark.load(); // return the current mark value
      }

      // ---------------------------------------------------------------------------
      virtual bool write (long value) override {
        mark.store (value);
        return true;
      }

      // ---------------------------------------------------------------------------
      bool isEnabled () const override {
        return genThread.joinable(); // return true if generator thread is running
      }

      // ---------------------------------------------------------------------------
      long frequency() const override {
        return calculate (tick.load(), range.load()); // return the current frequency in Hz
      }

      // ---------------------------------------------------------------------------
      virtual long max() const override {
        return range.load(); // return the maximum value based on the current range
      }

      void setEnable (bool enable) override;
      long setRange (long range) override;
      long setFrequency (long freq) override;

      // ----------- internal methods -----------
      static void *generator (std::future<void> run, SoftPwm::Private *d);
      // Calculate the tick or frequency
      static long calculate (long o1, long o2) {
        double r = ceil (1e6 / (static_cast<double> (o1) * o2));
        return static_cast<long> (r);
      }

      // ------------- data members -------------
      Pin *pin; // pointer to the pin associated with this device
      std::atomic<long> mark; // current mark value
      std::atomic<long> range; // current range value
      std::atomic<long> tick; // range * tick = period
      std::promise<void> killGenThread; // promise to signal the generator thread to stop
      std::thread genThread; // thread for generating the PWM signal

      PIMP_DECLARE_PUBLIC (SoftPwm)
  };
}

/* ========================================================================== */
