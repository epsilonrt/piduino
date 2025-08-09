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
#include <thread>
#include <mutex>
#include <piduino/gpiopwm.h>
#include "converter_p.h"

namespace Piduino {

  /**
     @class GpioPwm::Private
     @brief Private implementation class for GpioPwm.
  */
  class GpioPwm::Private  : public Converter::Private {

    public:
      Private (GpioPwm *q, Pin *pin, long range, long freq);
      Private (GpioPwm *q, const std::string &parameters);
      virtual ~Private();

      // --------------------------------------------------------------------------
      virtual const std::string &deviceName() const override {
        static const std::string name = GpioPwm::registeredName();
        return name;
      }

      // --------------------------------------------------------------------------
      virtual bool open (OpenMode mode) override {

        pin->setMode (Pin::ModeOutput);
        setEnable (true); // Enable the PWM generator
        if (isEnabled()) {

          return Converter::Private::open (mode);
        }
        return false;
      }

      // --------------------------------------------------------------------------
      virtual void close() override {

        setEnable (false); // Disable the PWM generator
        Converter::Private::close();
      }

      // --------------------------------------------------------------------------
      virtual long read() override {

        return value;
      }

      // --------------------------------------------------------------------------
      virtual bool write (long v) override {

        value = v;
        flag |= FlagValueUpdated;
        return true;
      }

      // --------------------------------------------------------------------------
      virtual long range() const override {
        return prange;
      }

      // --------------------------------------------------------------------------
      virtual long max (bool differential = false) const override {
        return range();
      }

      // --------------------------------------------------------------------------
      virtual long min (bool differential = false) const override {
        return 0;
      }

      // --------------------------------------------------------------------------
      virtual void setEnable (bool enable) override {
        if (enable != isEnabled()) {

          if (enable) {
            flag = FlagRun | FlagValueUpdated;
            thread = std::thread (generator, std::ref (flag), this);
          }
          else {
            flag &= ~FlagRun;
            thread.join();
          }
        }
      }

      // --------------------------------------------------------------------------
      virtual bool isEnabled () const override {
        return thread.joinable();
      }

      // --------------------------------------------------------------------------
      virtual long frequency() const override {
        return pfreq;
      }


      // ------------------------- internal methods -------------------------

      /**
         @brief Static method to generate PWM signal in a separate thread.
         @param flag Atomic flag to control the thread execution.
         @param d Pointer to the GpioPwm::Private instance.
         @return Pointer to the thread's return value (not used).
         @note This method runs in a loop, generating PWM signals until the flag is cleared
               or the thread is joined. It updates the pin state based on the value and frequency.
      */
      static void *generator (std::atomic<int> &flag, GpioPwm::Private *d);

      // --------------------------- data members ---------------------------
      Pin *pin; ///< Pointer to the associated Pin object
      long value; ///< Current PWM value (0 to range for duty cycle)
      long pfreq; ///< PWM frequency
      long prange; ///< PWM range
      std::atomic<int> flag; ///< Atomic flag to control the PWM generation thread
      std::thread thread; ///< Thread for generating PWM signals

      static const int FlagRun = 1; ///< Flag to indicate the thread should run
      static const int FlagValueUpdated = 2; ///< Flag to indicate the value has been updated

      PIMP_DECLARE_PUBLIC (GpioPwm)
  };
}

/* ========================================================================== */
