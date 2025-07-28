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
#include <piduino/soc.h>
#include <piduino/database.h>
#include "config.h"

namespace Piduino {
// -----------------------------------------------------------------------------
//
//                            SoC::Family Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void SoC::Family::setId (SoC::Family::Id i)  {

    cppdb::result res = Piduino::db << "SELECT name FROM soc_family WHERE id=?" << i << cppdb::row;
    if (!res.empty()) {

      _id = i;
      res >> _name;
    }
  }

// -----------------------------------------------------------------------------
//
//                              SoC Class
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
  void SoC::setId (SoC::Id i)  {

    cppdb::result res = Piduino::db << "SELECT name,soc_family_id,manufacturer_id,i2c_count,spi_count,uart_count FROM soc WHERE id=?" << i << cppdb::row;
    if (!res.empty()) { 
      int sfid;
      int mid;

      _id = i;
      res >> _name >> sfid >> mid >> _i2c_count >> _spi_count >> _uart_count;
      _family.setId (static_cast<SoC::Family::Id> (sfid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));

      _spi_cs.clear();
      res = Piduino::db <<
            "SELECT soc_has_pin.pin_id,pin_spics.bus_id,pin_spics.cs_id,pin_spics.pin_mode_id "
            "FROM soc_has_pin "
            "INNER JOIN pin_spics ON pin_spics.pin_id = soc_has_pin.pin_id "
            "WHERE soc_has_pin.soc_id=?" << _id;

      while (res.next()) {
        int pin_mode;
        Pin::SpiCs cs;

        res >> cs.pin >> cs.bus >> cs.cs >> pin_mode;
        cs.mode = static_cast<Pin::Mode> (pin_mode);
        _spi_cs.push_back (cs);
      }
    }
  }
}
/* ========================================================================== */
