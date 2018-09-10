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

#ifndef PIDUINO_SCHEDULER_H
#define PIDUINO_SCHEDULER_H

/**
 *  @defgroup piduino_sceduler Scheduler
 */

/**
 *  @addtogroup piduino_sceduler
 *  @{
 */

namespace Piduino {

  /**
   * @class Scheduler
   * @author Pascal JEAN
   * @date 10/03/18
   * @brief
   */
  class Scheduler {

    public:
      /**
       * @brief Définie la priorité en temps réel du thread appelant
       *
       * L'algorithme choisie est le round-robin. Sur un système Linux, la
       * valeur normale de priorité est de 20, la valeur minimale est de 1 et
       * la valeur maximale est de 99. Donner une valeur de 99 est une très
       * mauvaise idée qui peut bloquer le système...
       *
       * @param priority valeur de la priorité
       */
      static void setRtPriority (int priority);
  };
}

/**
* @}
*/

/* ========================================================================== */
#endif /*PIDUINO_SCHEDULER_H defined */
