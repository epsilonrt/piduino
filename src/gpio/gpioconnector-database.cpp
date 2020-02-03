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
#include <sstream>
#include "config.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                       ConnectorDescriptor Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  bool
  ConnectorDescriptor::insert () {
    cppdb::statement stat;
    long long connector_id;

    connector_id = findId();
    if (connector_id < 0) {
      // new connector

      if (id < 0) {

        stat = Piduino::db << "INSERT INTO connector(name,rows,connector_family_id) "
               "VALUES(?,?,?)" << name << rows << family.id();
      }
      else {

        stat = Piduino::db << "INSERT INTO connector(id,name,rows,connector_family_id) "
               "VALUES(?,?,?,?)" << id << name << rows << family.id();
      }
      stat.exec();
      id = stat.last_insert_id();

      for (int i = 0; i < pin.size(); i++) {

        pin[i].insert ();
        if (!hasPin (pin[i])) {

          stat.reset();
          stat = Piduino::db << "INSERT INTO connector_has_pin(connector_id,pin_id,row,column) "
                 "VALUES(?,?,?,?)" << id << pin[i].id << pin[i].num.row << pin[i].num.column;
          stat.exec();
        }
      }
      return true;
    }
    else {
      // already existing connector

      id = connector_id;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  ConnectorDescriptor:: hasPin (const Pin::Descriptor & p) const {
    cppdb::result res =
      Piduino::db << "SELECT pin_id "
      "FROM connector_has_pin "
      "WHERE connector_id=? AND "
      "pin_id=? AND "
      "row=? AND "
      "column=?"
      << id << p.id << p.num.row << p.num.column << cppdb::row;

    return !res.empty();
  }

// ---------------------------------------------------------------------------
  long long
  ConnectorDescriptor::findId() const {
    cppdb::result res =
      Piduino::db <<
      "SELECT id "
      " FROM connector "
      " WHERE name=? AND "
      "   rows=? AND "
      "   connector_family_id=?"
      << name << rows << family.id();

    while (res.next()) {
      int match_count = 0;
      cppdb::result res2;
      long long connector_id;

      res >> connector_id;
      res2 = Piduino::db <<
             "SELECT pin_id,row,column "
             " FROM connector_has_pin "
             " WHERE "
             "   connector_id=?"
             << connector_id;

      while (res2.next()) {
        long long pin_id;
        int r, c;

        res2 >> pin_id >> r >> c;

        for (int i = 0; i < pin.size(); i++) {
          const Pin::Descriptor & p = pin[i];

          if ( (p.num.row == r) && (p.num.column == c)) {
            long long pin_id = p.id;

            if (pin_id < 0) {
              pin_id = p.findId();
            }

            if (pin_id == pin_id) {
              match_count++;
            }
            else {
              return -1;
            }
            break;
          }
        }
      }
      if (match_count == pin.size()) {

        return connector_id;
      }
    }
    return -1;
  }

// -----------------------------------------------------------------------------
  ConnectorDescriptor::ConnectorDescriptor (const GpioDescriptor * p,
                                     long long i, 
                                     int n) :
    number (n), rows (-1), id (i), parent(p) {

    if (id > 0) {
      // Chargement depuis database
      cppdb::result res;

      res = Piduino::db <<
            "SELECT name,rows,connector_family_id "
            " FROM connector "
            " WHERE "
            "   id=?"
            << id << cppdb::row;

      if (!res.empty()) {
        int fid;
        res >> name >> rows >> fid;
        family.setId (static_cast<ConnectorFamily::Id> (fid));
        res = Piduino::db <<
              "SELECT pin_id,row,column"
              " FROM connector_has_pin "
              " WHERE "
              "   connector_id=?"
              << id;

        while (res.next()) {
          long long pin_id;
          int row, column;

          res >> pin_id >> row >> column;
          pin.push_back (Pin::Descriptor (this, pin_id, row, column));
        }
      }
    }
  }

// -----------------------------------------------------------------------------
//
//                        ConnectorFamily Class
//
// -----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
  int header1XNumFunc (int row, int column, int columns) {

    return row;
  }

// ---------------------------------------------------------------------------
  int header2XNumFunc (int row, int column, int columns) {

    return (row - 1) * columns + column;
  }

// ---------------------------------------------------------------------------
  int
  ConnectorFamily::pinNumber (int row, int column) const {

    return _fnum (row, column, _columns);
  }

// ---------------------------------------------------------------------------
  void
  ConnectorFamily::setId (ConnectorFamily::Id familyId)  {

    if ( (Piduino::db.is_open()) && (familyId != Id::Unknown)) {
      cppdb::result res =
        Piduino::db << "SELECT name,columns FROM connector_family WHERE id=?"
        << familyId << cppdb::row;

      if (!res.empty()) {

        _id = familyId;

        switch (familyId) {
          case ConnectorFamily::Header1X:
            _fnum = header1XNumFunc;
            break;
          case ConnectorFamily::Header2X:
            _fnum = header2XNumFunc;
            break;
          default: {
            std::ostringstream msg;

            _fnum = nullptr;
            msg << __FILE__ << "(" << __LINE__ << "): Invalid ConnectorFamily::Id";
            throw std::invalid_argument (msg.str());
          }
          break;
        }
        res >> _name >> _columns;
      }
    }
  }
}
/* ========================================================================== */
