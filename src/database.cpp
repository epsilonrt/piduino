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
#include <sstream>
#include <cstdlib>
#include <piduino/database.h>
#include <piduino/system.h>
#include <piduino/configfile.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                            Database Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Database::Database (const std::string & sqliteDbPath) :
    cppdb::session (connectionInfo (sqliteDbPath)) {

    _board = std::make_shared<Board>();
  }

// -----------------------------------------------------------------------------
  Database::Database (int cpuinfoBoardRevision, const std::string & sqliteDbPath) :
    cppdb::session (connectionInfo (sqliteDbPath)) {

    _board = std::make_shared<Board> (cpuinfoBoardRevision);
  }

// -----------------------------------------------------------------------------
  Database::Database (const std::string & armbianBoardTag, const std::string & sqliteDbPath) :
    cppdb::session (connectionInfo (sqliteDbPath)) {

    _board = std::make_shared<Board> (armbianBoardTag);
  }

// -----------------------------------------------------------------------------
  Database::~Database() {

    cppdb::session::close();
  }

// -----------------------------------------------------------------------------
  std::string
  Database::connectionInfo (const std::string & sqliteDbPath) {
    std::ostringstream ci;
    std::string dbPath (sqliteDbPath);

    if (dbPath.empty()) {
      const char * path;

      path = std::getenv ("PWD");
      if (path) {
        std::ostringstream filepath;

        filepath << path << "/piduino.db";
        if (System::fileExist (filepath.str())) {

          dbPath = filepath.str();
        }
      }

      if (dbPath.empty()) {

        path = std::getenv ("PIDUINO_DBPATH");
        if (path) {

          if (System::fileExist (path)) {

            dbPath = std::string (path);
          }
        }
      }

      if (dbPath.empty()) {
        std::ostringstream filepath;

        filepath << PIDUINO_INSTALL_DATA_DIR << "/piduino.db";
        dbPath = filepath.str();
      }
    }

    ci << "sqlite3:db=" << dbPath;
    return ci.str();
  }

// -----------------------------------------------------------------------------
//
//                            Database::Board Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Database::Board::Board (int raspberryPiRevision) : _id (-1), _gpio_id (-1), _pcb_revision (-1) {

    if (raspberryPiRevision == -1) {
      // if the revision is not provided, try to detect the host system model...

      if (system.armbianInfo().isValid()) {

        // BOARD Tag from /etc/armbian-release found...
        setTag (system.armbianInfo().board());
      }
      else {

        if ( (system.hardware() == "BCM2710") || (system.hardware() == "BCM2709") ||
             (system.hardware() == "BCM2708") || (system.hardware() == "BCM2835") ||
             (system.hardware() == "BCM2836") || (system.hardware() == "BCM2837")) {

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
  Database::Board::Board (const std::string & t) : Board() {

    setTag (t);
  }

// -----------------------------------------------------------------------------
  Database::Board::~Board() {

  }

// -----------------------------------------------------------------------------
  void Database::Board::setRevision (int rev) {

    cppdb::result res = Piduino::db << "SELECT id,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE revision=?" << rev << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      res >> _id >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Database::Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
    else {

      throw std::invalid_argument ("Couldn't find a board with " +
                                   Convert::T_to_string (rev) + " revision number !");
    }
  }

// -----------------------------------------------------------------------------
  void Database::Board::setTag (const std::string & t) {

    cppdb::result res = Piduino::db << "SELECT id,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE tag=?" << t << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      res >> _id >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Database::Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
    else {

      throw std::invalid_argument ("Couldn't find a board with " +
                                   t + " tag !");
    }
  }

// -----------------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& os, const Database::Board& b)  {
    /*
        Name        : Pi2 Model B
        Mcu         : bcm2709
        Revision    : 0xa01041
        GPIO Rev    : 3
        PCB Rev     : 1.1
        Memory      : 1024MB
        Manufacturer: Sony
     */
    os << "Name            : " << b.name() << std::endl;
    os << "Family          : " << b.family().name() << std::endl;
    os << "Database Id     : " << b.id() << std::endl;
    os << "Manufacturer    : " << b.manufacturer().name() << std::endl;
    if (system.revision() > 0) {

      os << "Board Revision  : " << std::hex << std::showbase << system.revision() << std::endl;
    }
    if (system.armbianInfo().isValid()) {

      os << "Armbian Board   : " << system.armbianInfo().board() << std::endl;
    }
    os << "SoC             : " << b.soc().name() << "(" << b.soc().manufacturer().name() << ")" << std::endl;
    os << "Memory          : " << system.totalRam() << "MB" << std::endl;
    os << "GPIO Id         : " << b.gpioId() << std::endl;
    if (b.pcbRevision() > 0) {

      os << "PCB Revision    : " << b.pcbRevision() << std::endl;
    }
  }

// -----------------------------------------------------------------------------
//
//                            Database::Board::Model Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void
  Database::Board::Model::setId (Database::Board::Model::Id i)  {

    cppdb::result res =
      Piduino::db << "SELECT name,board_family_id,soc_id FROM board_model WHERE id=?"
      << i << cppdb::row;

    if (!res.empty()) {
      int bfid;
      int sid;

      _id = i;
      res >> _name >> bfid >> sid;
      _family.setId (static_cast<Database::Board::Family::Id> (bfid));
      _soc.setId (static_cast<SoC::Id> (sid));
    }
  }

// -----------------------------------------------------------------------------
//
//                            Database::Board::Family Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void
  Database::Board::Family::setId (Database::Board::Family::Id i)  {

    cppdb::result res =
      Piduino::db << "SELECT name FROM board_family WHERE id=?"
      << i << cppdb::row;

    if (!res.empty()) {

      _id = i;
      res >> _name;
    }
  }
}

/* ========================================================================== */
