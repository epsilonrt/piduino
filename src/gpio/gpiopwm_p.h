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

#ifndef PIDUINO_GPIOPWM_PRIVATE_H
#define PIDUINO_GPIOPWM_PRIVATE_H

#include <atomic>
#include <thread>
#include <mutex>
#include <piduino/gpiopwm.h>
#include "../pwm_p.h"

namespace Piduino {

  /**
   * @class GpioPwm::Private
   * @brief
   */
  class GpioPwm::Private  : public Pwm::Private {

    public:
      Private (GpioPwm * q, Pin * pin, unsigned int resolution, unsigned long freq);
      virtual ~Private();

      virtual bool open (OpenMode mode);
      virtual bool isOpen() const;
      virtual void close();
      virtual long read();
      virtual bool write (long value);

      Pin * pin;
      long value;
      long freq;
      std::atomic<int> flag;  // communication avec le thread
      std::thread thread;

      static const int FlagRun = 1;
      static const int FlagValueUpdated = 2;
      static void * generator (std::atomic<int> & flag, GpioPwm::Private * d);

      PIMP_DECLARE_PUBLIC (GpioPwm)
  };
}

/* ========================================================================== */
#endif /* PIDUINO_GPIOPWM_PRIVATE_H defined */
