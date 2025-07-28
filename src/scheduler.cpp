/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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

#include <piduino/scheduler.h>
#include <system_error>
#include <pthread.h>
#include "config.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                        Scheduler Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void Scheduler::setRtPriority (int priority) {
    struct sched_param sparam;
    int min, max;
    int policy;

    min = sched_get_priority_min (SCHED_FIFO);
    max = sched_get_priority_max (SCHED_FIFO);
    pthread_getschedparam (pthread_self(), &policy, &sparam);

    if (priority < min) {

      sparam.sched_priority = min;
    }
    if (priority > max) {

      sparam.sched_priority = max;
    }
    else {

      sparam.sched_priority = priority;
    }

    if (pthread_setschedparam (pthread_self(), SCHED_FIFO, &sparam) < 0) {

      throw std::system_error (errno, std::system_category(), __FUNCTION__);
    }
  }

// -----------------------------------------------------------------------------
  int Scheduler::rtPriority() {
    struct sched_param sparam;
    int policy;

    pthread_getschedparam (pthread_self(), &policy, &sparam);
    return sparam.sched_priority;
  }

// -----------------------------------------------------------------------------
  int Scheduler::rtPriorityMin() {

    return sched_get_priority_min (SCHED_FIFO);
  }

// -----------------------------------------------------------------------------
  int Scheduler::rtPriorityMax() {

    return sched_get_priority_max (SCHED_FIFO);
  }

// -----------------------------------------------------------------------------
  void Scheduler::yield() {
    sched_yield();
  }

}
/* ========================================================================== */
