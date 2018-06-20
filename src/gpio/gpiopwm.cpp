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
#include <iostream>
#include <chrono>
#include <piduino/scheduler.h>
#include <piduino/gpiopwm.h>

using namespace std::chrono;

namespace Piduino {

  std::string GpioPwm::_name = "GpioPwm";
  typedef time_point<high_resolution_clock, nanoseconds> nanos_t;
  typedef duration<double, std::nano> nanod_t;

  // ---------------------------------------------------------------------------
  GpioPwm::GpioPwm (Pin * p, unsigned int r, unsigned long c) :
    Pwm (r), _pin (p), _value (0), _clk (c), _flag (0)  {

  }

  // ---------------------------------------------------------------------------
  GpioPwm::~GpioPwm() {

  }

  // ---------------------------------------------------------------------------
  bool
  GpioPwm::isOpen() const {

    return _thread.joinable();
  }

  // ---------------------------------------------------------------------------
  bool
  GpioPwm::open () {

    if (!isOpen()) {

      if (Pwm::open (OpenMode::ReadWrite)) {

        _flag = FlagRun | FlagValueUpdated;
        _thread = std::thread (generator, std::ref (_flag), this);
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  GpioPwm::close() {

    if (isOpen()) {

      _flag &= ~FlagRun;
      _thread.join();
      Pwm::close();
    }
  }

  // ---------------------------------------------------------------------------
  Pin *
  GpioPwm::pin() const {

    return _pin;
  }

  // ---------------------------------------------------------------------------
  const std::string &
  GpioPwm::name() const {

    return _name;
  }

  // ---------------------------------------------------------------------------
  void *
  GpioPwm::generator (std::atomic<int> & flag, GpioPwm * pwm) {
    nanod_t T (1E9 / static_cast<double> (pwm->clock())) ;
    double D = pwm->max() - pwm->min();
    Pin * p = pwm->pin();

    if ( (T > nanod_t (0)) && (D > 0)) {
      nanod_t ton;
      nanod_t toff;

      nanos_t next = time_point_cast<nanos_t::duration>
                     (high_resolution_clock::time_point (high_resolution_clock::now()));
      p->write (false);

      Scheduler::setRtPriority (90);

      try {
        while (flag & FlagRun) {

          if (flag & FlagValueUpdated) {

            ton = nanod_t ( (static_cast<double> (pwm->read()) * T) / D) ;
            toff = T - ton;
            flag &= ~FlagValueUpdated;
          }

          if (ton > nanod_t (0)) {

            next += duration_cast<nanoseconds> (toff);
            std::this_thread::sleep_until (next);
            p->write (true);
          }

          if (toff > nanod_t (0)) {
            
            next += duration_cast<nanoseconds> (ton);
            std::this_thread::sleep_until (next);
            p->write (false);
          }
        }
      }
      catch (std::system_error & e) {

        std::cerr << e.what() << "(code " << e.code() << ")" << std::endl;
        std::terminate();
      }
      catch (...) {

      }
    }
    p->write (false);
    return 0;
  }

  // ---------------------------------------------------------------------------
  void
  GpioPwm::write (long v) {
    if (v < min()) {
      v = min();
    }
    if (v > max()) {
      v = max();
    }
    _value = v;
    _flag |= FlagValueUpdated;
  }

  // ---------------------------------------------------------------------------
  void
  GpioPwm::setClock (long c) {

    _clk = c;
  }

  // ---------------------------------------------------------------------------
  long
  GpioPwm::clock() const {

    return _clk;
  }

  // ---------------------------------------------------------------------------
  long
  GpioPwm::read() {

    return _value;
  }
}

/* ========================================================================== */
