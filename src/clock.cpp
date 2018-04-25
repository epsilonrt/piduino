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
#include <piduino/clock.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                        Clock Class
//
// -----------------------------------------------------------------------------
  Clock::Clock() {
    struct timespec t;

    clock_gettime (CLOCK_MONOTONIC_RAW, &t);
    _ms = (uint64_t) t.tv_sec * (uint64_t) 1000ULL    + (uint64_t) (t.tv_nsec / 1000000ULL);
    _us = (uint64_t) t.tv_sec * (uint64_t) 1000000ULL + (uint64_t) (t.tv_nsec / 1000ULL);
  }

// -----------------------------------------------------------------------------
  Clock::~Clock() {

  }

// -----------------------------------------------------------------------------
  unsigned long
  Clock::millis () {
    uint64_t now;
    struct timespec t;

    clock_gettime (CLOCK_MONOTONIC_RAW, &t);
    now = (uint64_t) t.tv_sec * (uint64_t) 1000ULL    + (uint64_t) (t.tv_nsec / 1000000ULL);

    return (unsigned long) (now - _ms);
  }

// -----------------------------------------------------------------------------
  unsigned long
  Clock::micros () {
    uint64_t now;
    struct timespec t;

    clock_gettime (CLOCK_MONOTONIC_RAW, &t);
    now = (uint64_t) t.tv_sec * (uint64_t) 1000000ULL + (uint64_t) (t.tv_nsec / 1000ULL);

    return (unsigned long) (now - _us);
  }

// -----------------------------------------------------------------------------
  void
  Clock::delay (unsigned long d) {

    if (d) {

      if (d == -1) {

        sleep (-1);
      }
      else {
        struct timespec dt;

        dt.tv_nsec = (d % 1000UL) * 1000000UL;
        dt.tv_sec  = d / 1000UL;
        nanosleep (&dt, NULL);
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Clock::delayMicroseconds (unsigned long d) {

    if (d) {

      if (d == -1) {

        sleep (-1);
      }
      else {
        unsigned long s, us;
        struct timeval t1;

        gettimeofday (&t1, NULL);
        s  = d / 1000000UL;
        us = d % 1000000UL;

        if (d < 100) {
          struct timeval dt, t2;

          dt.tv_sec = s;
          dt.tv_usec = us;
          timeradd (&t1, &dt, &t2) ; // TODO: NO POSIX COMPLIANT
          while (timercmp (&t1, &t2, <)) {  // TODO: NO POSIX COMPLIANT

            gettimeofday (&t1, NULL);
          }
        }
        else {
          struct timespec dt;

          dt.tv_nsec = us * 1000UL;
          dt.tv_sec = s;
          nanosleep (&dt, NULL);
        }
      }
    }
  }
}
/* ========================================================================== */
