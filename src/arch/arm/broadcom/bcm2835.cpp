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
#include <exception>
#include <piduino/database.h>
#include "bcm2835.h"
#include "config.h"

namespace Piduino {

  namespace Bcm2835 {

    // -------------------------------------------------------------------------
    off_t iobase () {
      static const std::map<SoC::Id, off_t> list  {
        {SoC::Bcm2708, Bcm2708Base},
        {SoC::Bcm2709, Bcm2709Base},
        {SoC::Bcm2710, Bcm2710Base},
        {SoC::Bcm2711, Bcm2711Base},
      };
      
      if (db.board().soc().family().id() != SoC::Family::BroadcomBcm2835) {

        throw std::system_error (ENOTSUP, std::system_category(),
                                 "It seems that this system is not a Broadcom BCM2835 board !");
      }
      return list.at  (db.board().soc().id());
    }
  }
}
/* ========================================================================== */
