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

#include <piduino/gpiopin.h>
#include <piduino/database.h>
#include "config.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                       Pin::Descriptor Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  bool
  Pin::Descriptor::insert () {
    cppdb::statement stat;
    long long pin_id;

    pin_id = findId();
    if (pin_id < 0) {
      // new pin

      if (id < 0) {
        // auto-index
        stat = Piduino::db << "INSERT INTO pin(pin_type_id) "
               "VALUES(?)" << type;
      }
      else {

        stat = Piduino::db << "INSERT INTO pin(id,pin_type_id) "
               "VALUES(?,?)" << id << type;
      }
      stat.exec();
      id = stat.last_insert_id();
      if (type == TypeGpio) {
        stat = Piduino::db << "INSERT INTO pin_number(pin_id,logical_num,mcu_num,system_num) "
               "VALUES(?,?,?,?)" << id << num.logical << num.mcu << num.system;
      }
      for (auto n = name.begin(); n != name.end(); ++n) {

        insertModeName (n->first, n->second);
      }
      return true;
    }
    else {
      // already existing pin

      id = pin_id;
    }
    return false;
  }

// ---------------------------------------------------------------------------
  long long
  Pin::Descriptor::findId() const {
    cppdb::result res;

    for (auto it = name.begin(); it != name.end(); ++it) {

      // Recherche tous les pin_id correspondants au nom et au mode courant
      res = Piduino::db <<
            "SELECT pin_has_name.pin_id "
            " FROM pin_has_name "
            " INNER JOIN pin_name ON pin_has_name.pin_name_id = pin_name.id "
            " WHERE "
            "   pin_name.name=? AND "
            "   pin_has_name.pin_mode_id=?"
            << it->second << it->first;

      while (res.next()) {
        int match_count = 0;
        cppdb::result res2;
        long long pin_id;

        res >> pin_id;
        // Pour chaque pin_id, vérifier qu'il correspond aux éléments du map
        res2 = Piduino::db <<
               "SELECT pin_name.name,pin_has_name.pin_mode_id "
               "  FROM pin_has_name "
               "  INNER JOIN pin_name ON pin_has_name.pin_name_id=pin_name.id "
               "  WHERE pin_has_name.pin_id=?"
               << pin_id;
        while (res2.next()) {
          std::string n;
          Pin::Mode m;
          int i;

          res2 >> n >> i;
          m = static_cast<Pin::Mode> (i);
          if (name.count (m)) {
            if (name.at (m) == n) {
              match_count++;
            }
          }
        }
        if (match_count == name.size()) {
          return pin_id;
        }
      }
      break;
    }
    return -1;
  }

// ---------------------------------------------------------------------------
  long long
  Pin::Descriptor::findName (const std::string & n) const {
    cppdb::result res =
      Piduino::db << "SELECT id FROM pin_name WHERE name=?"
      << n << cppdb::row;
    if (!res.empty()) {
      long long i;
      res >> i;
      return i;
    }
    return -1;
  }

// ---------------------------------------------------------------------------
  bool
  Pin::Descriptor::hasModeName (Mode modeId, long long nameId) const {
    cppdb::result res =
      Piduino::db <<
      "SELECT pin_name_id "
      " FROM pin_has_name "
      " WHERE  pin_id=? AND "
      "   pin_name_id=? AND "
      "   pin_mode_id=?"
      << id << nameId << modeId << cppdb::row;
    return !res.empty();
  }

// ---------------------------------------------------------------------------
  void
  Pin::Descriptor::insertModeName (Mode m, const std::string & n) {
    cppdb::statement stat;
    long long name_id;

    name_id = findName (n);
    if (name_id < 0) {
      // new name

      stat = Piduino::db << "INSERT INTO pin_name(name) "
             "VALUES(?)" << n;
      stat.exec();
      name_id = stat.last_insert_id();
      stat.reset();
    }
    if (!hasModeName (m, name_id)) {

      stat = Piduino::db << "INSERT INTO pin_has_name(pin_id,pin_name_id,pin_mode_id) "
             "VALUES(?,?,?)" << id << name_id << m;
      stat.exec();
    }
  }

// -----------------------------------------------------------------------------
  Pin::Descriptor::Descriptor (long long pinId, int pinRow, int pinColumn) :
    type (Pin::TypeUnknown), id (pinId) {

    num.row = pinRow;
    num.column = pinColumn;

    if (id > 0) {
      // Chargement depuis database
      cppdb::result res;

      res = Piduino::db <<
            "SELECT pin_type_id "
            " FROM pin "
            " WHERE "
            "   id=?"
            << id << cppdb::row;

      if (!res.empty()) {
        int tid;

        res >> tid;
        type = static_cast<Pin::Type> (tid);

        res = Piduino::db <<
              "SELECT pin_name.name,pin_has_name.pin_mode_id "
              " FROM pin_has_name "
              " INNER JOIN pin_name ON pin_name.id = pin_has_name.pin_name_id "
              " WHERE "
              "   pin_has_name.pin_id=?"
              << id;

        while (res.next()) {
          int pin_mode;
          std::string pin_name;

          res >> pin_name >> pin_mode;
          name[static_cast<Pin::Mode> (pin_mode)] = pin_name;
        }
        res = Piduino::db <<
              "SELECT logical_num,mcu_num,system_num "
              " FROM pin_number "
              " WHERE "
              "   pin_id=?"
              << id << cppdb::row;
        if (!res.empty()) {
          res >> num.logical >> num.mcu >> num.system;
        }
      }
    }
  }
}
/* ========================================================================== */
