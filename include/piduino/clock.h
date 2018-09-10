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

#ifndef PIDUINO_CLOCK_H
#define PIDUINO_CLOCK_H

#include <cstdint>

/**
 *  @defgroup piduino_clock Clock
 *  Provides the functionality of a clock for time measurement and delay.
 */

namespace Piduino {

  /**
  *  @addtogroup piduino_clock
  *  @{
  */

  /**
   * @class Clock
   * @author Pascal JEAN
   * @date 03/05/18
   * @brief Horloge
   */
  class Clock {

    public:
      /**
       * @brief Constructor
       *
       * His call is the origin of the times provided by the millis() and micros()
       * functions.
       */
      Clock();

      /**
       * @brief Destructor
       */
      virtual ~Clock();

      /**
       * @brief Pauses for the amount of seconds
       *
       * Pauses the thread for the amount of time (in seconds) specified
       * as parameter.
       *
       * @param ms the number of seconds to pause, -1 deaden the thread
       * until it is woken up by a signal
       */
#ifdef __DOXYGEN__
      static inline void sleep (unsigned long s);
#else
      static inline void sleep (unsigned long s) {
        delay (s * 1000UL);
      }
#endif

      /**
       * @brief Pauses for the amount of milliseconds
       *
       * Pauses the thread for the amount of time (in milliseconds) specified
       * as parameter.
       *
       * @param ms the number of milliseconds to pause, -1 deaden the thread
       * until it is woken up by a signal
       */
      static void delay (unsigned long ms);

      /**
       * @brief Pauses for the amount of microseconds
       *
       * Pauses the thread for the amount of time (in microseconds) specified
       * as parameter. \n
       * Depending on the value provided, this function can wait for a loop
       * or alarm (for short values) or put the thread into the corresponding
       * number of us.
       *
       * @param us the number of milliseconds to pause, -1 deaden the thread
       * until it is woken up by a signal
       */
      static void delayMicroseconds (unsigned long us);

      /**
       * @brief Number of milliseconds
       *
       * Returns the number of milliseconds since began running the current
       * program.
       */
      unsigned long millis();

      /**
       * @brief Number of microseconds
       *
       * Returns the number of microseconds since began running the current
       * program.
       */
      unsigned long micros();

    private:
      uint64_t _us;
      uint64_t _ms;
  };
  // ---------------------------------------------------------------------------
  //
  //                      Piduino Clock Global Object
  //
  // ---------------------------------------------------------------------------
  extern Clock clk; ///< Piduino Clock Global Object
  /**
   * @}
   */
}

/* ========================================================================== */
#endif /*PIDUINO_CLOCK_H defined */
