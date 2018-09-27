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
#include "config.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                       Gpio::Descriptor Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  bool
  Gpio::Descriptor::insert () {
    cppdb::statement stat;
    long long gpio_id ;

    gpio_id = findId();
    if (gpio_id < 0) {
      // new gpio

      if (id < 0) {

        stat = Piduino::db << "INSERT INTO gpio(name) "
               "VALUES(?)" << name;

      }
      else {

        stat = Piduino::db << "INSERT INTO gpio(id,name) "
               "VALUES(?,?)" << id << name;
      }
      stat.exec();
      id = stat.last_insert_id();
      for (int i = 0; i < connector.size(); i++) {

        connector[i].insert ();
        if (!hasConnector (connector[i])) {

          stat.reset();
          stat = Piduino::db << "INSERT INTO gpio_has_connector(num,gpio_id,gpio_connector_id) "
                 "VALUES(?,?,?)" << connector[i].number << id << connector[i].id;
          stat.exec();
        }
      }
      return true;
    }
    else {
      // already existing connector

      id = gpio_id;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  Gpio::Descriptor:: hasConnector (const Connector::Descriptor & c) const {
    cppdb::result res =
      Piduino::db << "SELECT gpio_connector_id FROM gpio_has_connector "
      "WHERE gpio_id=? AND "
      "gpio_connector_id=?"
      << id << c.id << cppdb::row;
    return !res.empty();
  }

  // ---------------------------------------------------------------------------
  long long
  Gpio::Descriptor::findId() const {
    cppdb::result res =
      Piduino::db << "SELECT id FROM gpio WHERE name=?"
      << name << cppdb::row;
    if (!res.empty()) {
      long long i;
      res >> i;
      return i;
    }
    return -1;
  }

// -----------------------------------------------------------------------------
  Gpio::Descriptor::Descriptor (long long gpioId) :
    id (gpioId) {

    if (id > 0) {
      // Chargement depuis database
      cppdb::result res;

      res = Piduino::db <<
            "SELECT name "
            " FROM gpio "
            " WHERE "
            "   id=?"
            << id << cppdb::row;
      if (!res.empty()) {
        res >> name;
        res = Piduino::db <<
              "SELECT num,gpio_connector_id "
              " FROM gpio_has_connector "
              " WHERE "
              "   gpio_id=?"
              << id;

        while (res.next()) {
          long long connector_id;
          int connector_num;

          res >> connector_num >> connector_id;
          connector.push_back (Connector::Descriptor (connector_id, connector_num));
        }
      }
    }
  }

}
/* ========================================================================== */
