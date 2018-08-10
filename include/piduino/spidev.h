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

#ifndef PIDUINO_SPIDEV_H
#define PIDUINO_SPIDEV_H

#include <vector>
#include <string>

/**
 *  @defgroup piduino_spidev SPI Bus
 *
 *  Ce module permet l'utilisation des bus SPI par le driver spidev.
 *  @{
 */

namespace Piduino {
  
  class SpiDev {
    public:

      class Info {
        public:
          int id;
          std::string path;
          int cs;
      };
      static std::vector<Info> availableBuses ();
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_SPIDEV_H defined */
