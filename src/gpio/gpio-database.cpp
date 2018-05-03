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

#include <piduino/gpio.h>
#include <piduino/database.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                       Gpio::Descriptor Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  bool
  Gpio::Descriptor::insertToDb () {
    /*
        std::string name;
        int number;
        int rows;
        int columns;
        PinNumberFunc pinNumber;
        std::vector<Pin::Descriptor> pin;
        long long id;
     */
    if (!findInDb()) {
      // new record
      cppdb::statement stat;

      if (id < 0) {

        stat = Piduino::db << "INSERT INTO gpio(name) "
               "VALUES(?)" << name;

      }
      else {

        stat = Piduino::db << "INSERT INTO gpio(id,name) "
               "VALUES(?,?)" << id << name;
      }
      stat.exec();

      if (stat.affected() == 1) {
        
        id = stat.last_insert_id();
        for (int i = 0; i < connector.size(); i++) {

          if (connector[i].insertToDb ()) {

            stat.reset();
            stat = Piduino::db << "INSERT INTO gpio_has_connector(num,gpio_id,gpio_connector_id) "
                   "VALUES(?,?,?)" << connector[i].number << id << connector[i].id;
            stat.exec();
          }
        }
        return true;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  Gpio::Descriptor::findInDb() const {
    return false;
  }

#if 0
// -----------------------------------------------------------------------------
  Gpio::Descriptor::Descriptor (long long i) :
    id (i), number (-1), rows (-1) {
    // Chargement depuis database
  }
#endif

}
/* ========================================================================== */
