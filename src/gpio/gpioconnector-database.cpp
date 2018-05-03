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

#include <piduino/gpioconnector.h>
#include <piduino/database.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                       Connector::Descriptor Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  bool
  Connector::Descriptor::insertToDb () {
    /*
        std::string name;
        int number;
        int rows;
        int columns;
        PinNumberFunc pinNumber;
        std::vector<Pin::Descriptor> pin;
        long long id;
     */
#if 0
    if (id < 0) {
      // new record
      cppdb::statement stat;

      stat = Piduino::db << "INSERT INTO gpio_connector(name,rows,gpio_connector_family_id) "
             "VALUES(?,?,?)" << name << rows << family.id();
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
#endif
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  Connector::Descriptor::findInDb() const {
    return false;
  }

#if 0
// -----------------------------------------------------------------------------
  Connector::Descriptor::Descriptor (long long i) :
    id (i), number (-1), rows (-1) {
    // Chargement depuis database
  }
#endif

// -----------------------------------------------------------------------------
//
//                            Connector::Family Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  int hex1NumFunc (int row, int column, int columns) {

    return row;
  }

  // ---------------------------------------------------------------------------
  int hex2NumFunc (int row, int column, int columns) {

    return (row - 1) * columns + column;
  }

  // ---------------------------------------------------------------------------
  std::map<Connector::Family::Id, Connector::Family::PinNumberFunc>
  Connector::Family::_fnum_map = {
    { Connector::Family::Hex1, hex1NumFunc},
    { Connector::Family::Hex2, hex2NumFunc},
  };

  // ---------------------------------------------------------------------------
  int
  Connector::Family::pinNumber (int row, int column) const {

    return _fnum (row, column, _columns);
  }

  // ---------------------------------------------------------------------------
  void
  Connector::Family::setId (Connector::Family::Id i)  {

    if (Piduino::db.is_open()) {
      cppdb::result res =
        Piduino::db << "SELECT name,columns FROM gpio_connector_family WHERE id=?"
        << i << cppdb::row;

      if (!res.empty()) {

        _id = i;
        _fnum = _fnum_map.at (i);
        res >> _name >> _columns;
      }
    }
  }

}
/* ========================================================================== */
