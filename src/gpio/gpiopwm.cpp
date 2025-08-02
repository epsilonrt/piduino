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
#include <chrono>
#include <piduino/scheduler.h>
#include "gpiopwm_p.h"
#include "config.h"

using namespace std::chrono;

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                             GpioPwm Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  GpioPwm::GpioPwm (GpioPwm::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  GpioPwm::GpioPwm (Pin *p, long r,  long f) :
    Converter (*new Private (this, p, r, f)) { }

  // ---------------------------------------------------------------------------
  GpioPwm::~GpioPwm() = default;

  // ---------------------------------------------------------------------------
  const Pin &
  GpioPwm::pin() const {
    PIMP_D (const GpioPwm);

    return *d->pin;
  }

  // -----------------------------------------------------------------------------
  //
  //                         GpioPwm::Private Class
  //
  // -----------------------------------------------------------------------------
  typedef time_point<high_resolution_clock, nanoseconds> nanos_t;
  typedef duration<double, std::nano> nanod_t;


  // ---------------------------------------------------------------------------
  GpioPwm::Private::Private (GpioPwm *q, Pin *p, long r, long f) :
    Converter::Private (q, DigitalToAnalog, hasRange | hasFrequency),  pin (p), value (0), freq (f), range (r), flag (0) {
  }

  // ---------------------------------------------------------------------------
  GpioPwm::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  void *
  GpioPwm::Private::generator (std::atomic<int> &flag, GpioPwm::Private *d) {
    nanod_t T (1E9 / static_cast<double> (d->freq)) ;
    double D = d->max() - d->min();
    Pin *pin = d->pin;

    if ( (T > nanod_t (0)) && (D > 0)) {
      nanod_t ton;
      nanod_t toff;

      nanos_t next = time_point_cast<nanos_t::duration>
                     (high_resolution_clock::time_point (high_resolution_clock::now()));
      pin->write (false);

      Scheduler::setRtPriority (90);

      try {
        while (flag & FlagRun) {

          if (flag & FlagValueUpdated) {

            ton = nanod_t ( (static_cast<double> (d->value) * T) / D) ;
            toff = T - ton;
            flag &= ~FlagValueUpdated;
          }

          if (ton > nanod_t (0)) {

            next += duration_cast<nanoseconds> (toff);
            std::this_thread::sleep_until (next);
            pin->write (true);
          }

          if (toff > nanod_t (0)) {

            next += duration_cast<nanoseconds> (ton);
            std::this_thread::sleep_until (next);
            pin->write (false);
          }
        }
      }
      catch (std::system_error &e) {

        std::cerr << e.what() << "(code " << e.code() << ")" << std::endl;
        std::terminate();
      }
      catch (...) {

      }
    }
    pin->write (false);
    return 0;
  }
}

/* ========================================================================== */
