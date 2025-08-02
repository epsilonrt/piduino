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
#include <climits>
#include "softpwm_p.h"
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                             SoftPwm Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SoftPwm::SoftPwm (SoftPwm::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  SoftPwm::SoftPwm (Pin *p, long range, long frequency) :
    Converter (*new Private (this, p, range, frequency)) {}

  // ---------------------------------------------------------------------------
  // The Converter destructor calls Private::close() if isOpen() == true, nothing to do here
  SoftPwm::~SoftPwm() = default;

  // ---------------------------------------------------------------------------
  long
  SoftPwm::tick() const {
    PIMP_D (const SoftPwm);

    return d->tick.load(); // return the current tick value
  }

  // ---------------------------------------------------------------------------
  //
  //                     SoftPwm::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SoftPwm::Private::Private (SoftPwm *q, Pin *p, long range, long frequency) :
    Converter::Private (q, DigitalToAnalog, hasFrequency | hasSetFrequency | hasRange | hasSetRange | hasResolution | hasSetResolution),
    pin (p), mark (0), range (range), tick (calculate (frequency, range)) {}

  // ---------------------------------------------------------------------------
  SoftPwm::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  void
  SoftPwm::Private::setEnable (bool enable) {

    if (enable) {

      if (!genThread.joinable()) { // if the generator thread is not already running, start it
        // Set thread priority to maximum (requires pthread and appropriate privileges)
        pthread_t native_handle;
        sched_param sch_params;
        int policy;

        std::future<void> running = killGenThread.get_future(); // create a future to control the thread
        // Start the thread first, then set priority
        genThread = std::thread (Private::generator, std::move (running), this);
        native_handle = genThread.native_handle();

        pthread_getschedparam (native_handle, &policy, &sch_params);
        sch_params.sched_priority = sched_get_priority_max (policy);
        pthread_setschedparam (native_handle, policy, &sch_params);
      }

    }
    else {

      if (genThread.joinable()) { // the generator thread is running, stop it
        killGenThread.set_value(); // signal the thread to stop
        genThread.join(); // wait for the thread to finish
        genThread = std::thread(); // reset the thread object
        killGenThread = std::promise<void>(); // reset the promise for the next call
      }
    }
  }

  // ---------------------------------------------------------------------------
  long
  SoftPwm::Private::setFrequency (long freq) {
    long newTick = calculate (freq, range.load()); // calculate new tick value

    if (newTick < minTick) { // ensure tick is not less than minimum
      newTick = minTick;
    }

    tick.store (newTick); // update the tick value
    return frequency();
  }

  // ---------------------------------------------------------------------------
  long
  SoftPwm::Private::setRange (long rg) {

    if (rg < 2) {
      rg = 2;
    }

    range.store (rg); // update the range value
    if (mark.load()  > rg) { // if the mark is greater than the new range, adjust it
      mark.store (rg); // set mark to the new range value
    }
    return rg;
  }

  // ---------------------------------------------------------------------------
  // static
  void *
  SoftPwm::Private::generator (std::future<void> run, SoftPwm::Private *d) {
    bool isMark = false; // mark space sequencer: true = mark, false = space
    long pw;

    pw = d->mark.load() * d->tick.load(); // initial pulse width, mark duration
    d->pin->write (false); // ensure pin is LOW at start, start with space
    try {

      while (run.wait_for (std::chrono::microseconds (pw)) == std::future_status::timeout) { // wait for the signal to stop
        long tick = d->tick.load(); // get the current tick value
        long range = d->range.load(); // get the current range value
        long mark = d->mark.load(); // get the current mark value

        if (mark == 0) { // if mark is zero, set pin LOW and skip to next iteration
          d->pin->write (false);
          isMark = false; // next pulse will be a mark
          pw = range * tick; // update pulse width to range value
          continue;
        }

        if (mark >= range) { // if mark is greater than or equal to range, set pin HIGH and skip to next iteration
          d->pin->write (true);
          isMark = true; // next pulse will be a space
          pw = range * tick; // update pulse width to range value
          continue;
        }

        if (isMark) { // next pulse is a space

          d->pin->write (false); // set pin LOW for space duration
          pw = (range - mark) * tick; // update pulse width to space value
          isMark = false;
        }
        else {

          d->pin->write (true); // set pin HIGH for mark duration
          pw = mark * tick; // update pulse width to mark value
          isMark = true; // next pulse will be a space
        }
      }
    }
    catch (std::system_error &e) {

      // handle system errors
      std::cerr << e.what() << "(code " << e.code() << ")" << std::endl;
      std::terminate();
    }
    catch (...) {

    }

    #ifndef NDEBUG
    std::cout << std::endl << "SoftPwm::generator stopped !" << std::endl;
    #endif
    return 0;
  }
} // namespace Piduino
/* ========================================================================== */
