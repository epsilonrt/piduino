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
#include <sys/sysinfo.h>
#include <string>
#include <piduino/board.h>
#include <piduino/database.h>
#include <piduino/configfile.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                            Board::Family Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void
  Board::Family::setId (Board::Family::Id i)  {

    cppdb::result res = Piduino::db << "SELECT name FROM board_family WHERE id=?" << i << cppdb::row;
    if (!res.empty()) {

      _id = i;
      res >> _name;
    }
  }

// -----------------------------------------------------------------------------
//
//                            Board::Model Class
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
  void Board::Model::setId (Board::Model::Id i)  {

    cppdb::result res = Piduino::db << "SELECT name,board_family_id,soc_id FROM board_model WHERE id=?" << i << cppdb::row;
    if (!res.empty()) {
      int bfid;
      int sid;

      _id = i;
      res >> _name >> bfid >> sid;
      _family.setId (static_cast<Board::Family::Id> (bfid));
      _soc.setId (static_cast<SoC::Id> (sid));
    }
  }
// -----------------------------------------------------------------------------
//
//                        Board Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void Board::setRevision (int rev) {

    cppdb::result res = Piduino::db << "SELECT id,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE revision=?" << rev << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      res >> _id >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
    else {

      throw std::invalid_argument ("Couldn't find a board with " +
                                   Convert::T_to_string (rev) + " revision number !");
    }
  }

// -----------------------------------------------------------------------------
  void Board::setTag (const std::string & t) {

    cppdb::result res = Piduino::db << "SELECT id,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE tag=?" << t << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      res >> _id >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
    else {

      throw std::invalid_argument ("Couldn't find a board with " +
                                   t + " tag !");
    }
  }

// -----------------------------------------------------------------------------
  Board::Board (int raspberryPiRevision) : _id (-1), _gpio_id (-1), _pcb_revision (-1) {

    if (raspberryPiRevision == -1) {
      // if the revision is not provided, try to detect the host system model...

      if (system().armbianInfo().isValid()) {

        // BOARD Tag from /etc/armbian-release found...
        setTag (system().armbianInfo().board());
      }
      else {

        if ( (_system.hardware() == "BCM2710") || (_system.hardware() == "BCM2709") ||
             (_system.hardware() == "BCM2708") || (_system.hardware() == "BCM2835") ||
             (_system.hardware() == "BCM2836") || (_system.hardware() == "BCM2837")) {

          // Revision from /proc/cpuinfo found...
          setRevision (raspberryPiRevision);
        }
        else {

          throw std::system_error (ENOTSUP, std::system_category(),
                                   "Unable to identify your host system");
        }

      }
    }
    else {

      setRevision (raspberryPiRevision);
    }
  }

// -----------------------------------------------------------------------------
  Board::Board (const std::string & t) : Board() {

    setTag (t);
  }

// -----------------------------------------------------------------------------
  Board::~Board() {

  }
}

/* ========================================================================== */
