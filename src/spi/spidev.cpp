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
#include <cstdio>
#include <piduino/spidev.h>
#include <piduino/database.h>
#include <piduino/system.h>

namespace Piduino {
  // ---------------------------------------------------------------------------
  std::vector<SpiDev::Info>
  SpiDev::availableBuses () {
    std::vector<SpiDev::Info> buses;

    for (int id = 0; id < db.board().soc().spiCount(); id++) {
      char path[256];

      for (int cs = 0; cs < db.board().soc().spiCsCount(); cs++) {

        ::snprintf (path, sizeof (path), db.board().soc().spiSysPath().c_str(), id, cs);
        if (System::fileExist (std::string (path))) {
          Info bus;

          bus.id = id;
          bus.cs = cs;
          bus.path = path;
          buses.push_back (bus);
        }
      }
    }
    return buses;
  }
}

/* ========================================================================== */
