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

#include <mutex>

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
       * @brief Constructeur
       * @param interruptPriority priorité utilisée par noInterrupts(), -1 pour la priorité maximale
       */
      Scheduler (int interruptPriority = -1) : _interruptPriority (interruptPriority) {
        if (interruptPriority < 0) {
          _interruptPriority = rtPriorityMax();
        }
        _previousPriority = rtPriority();
      }

      /**
       * @brief Passage en mode "sans interruptions"
       *
       * Le thread appelant est passé avec une priorité interrupPriority(),
       * l'appel à cette fonction est bloqué jusqu'à libération par interrupts().
       * Cette fonction ne devrait être appellée que pour des portions de code
       * extrémement critiques. \n
       * Elle nécessite de disposer des droits root.
       */
      inline void noInterrupts() {

        _intMutex.lock();
        _previousPriority = rtPriority();
        setRtPriority (_interruptPriority);
      }

      /**
       * @brief Passage en mode "avec interruptions"
       *
       * C'est le mode normal d'un thread qui peut donc être interrompu.
       * Cette fonction doit être appelé pour désactiver le mode "sans interruptions"
       * activé par noInterrupts().
       */
      inline void interrupts() {

        setRtPriority (_previousPriority);
        _intMutex.unlock();
      }

      /**
       * @brief Priorité affectée lors d'un appel à noInterrupts()
       */
      int interrupPriority () const {
        return _interruptPriority;
      }

      /**
       * @brief Modifie la priorité affectée lors d'un appel à noInterrupts()
       *
       * @warning l'appel à cette fonction, alors que le mode noInterrupts() est
       * activé, bloque le thread appelant.
       */
      void setInterrupPriority (int value) {
        std::unique_lock<std::mutex> lock (_intMutex);
        _interruptPriority = value;
      }

      /**
       * @brief Définis la priorité en temps réel du thread appelant
       *
       * L'algorithme choisie est le round-robin. Sur un système Linux, la
       * valeur normale de priorité est de 20, la valeur minimale est de 1 et
       * la valeur maximale est de 99. Donner une valeur de 99 est une très
       * mauvaise idée qui peut bloquer le système...
       *
       * @param priority valeur de la priorité
       */
      static void setRtPriority (int priority);

      /**
       * @brief Priorité du thread appelant
       */
      static int rtPriority();

      /**
       * @brief Priorité minimale du thread appelant
       */
      static int rtPriorityMin();

      /**
       * @brief Priorité maximale du thread appelant
       */
      static int rtPriorityMax();

    protected:
      int _interruptPriority;
      int _previousPriority;
      std::mutex _intMutex;
  };
}

/**
* @}
*/

/* ========================================================================== */
#endif /*PIDUINO_SCHEDULER_H defined */
