/* Copyright © 2018 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/
#include <sstream>
#include <cstdlib>
#include <piduino/database.h>
#include <piduino/system.h>
#include <piduino/configfile.h>
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                            Database Class
  //
  // -----------------------------------------------------------------------------

  // -----------------------------------------------------------------------------
  Database::Database (const std::string &cinfo) :
    cppdb::session (), _cinfo (findConnectionInfo (cinfo)) {

    if (!_cinfo.empty()) {

      cppdb::session::open (_cinfo);
    }
    _board = std::make_shared<Board>();
  }

  // -----------------------------------------------------------------------------
  Database::Database (int cpuinfoBoardRevision, const std::string &cinfo) :
    cppdb::session (), _cinfo (findConnectionInfo (cinfo)) {

    if (!_cinfo.empty()) {

      cppdb::session::open (_cinfo);
    }
    _board = std::make_shared<Board> (cpuinfoBoardRevision);
  }

  // -----------------------------------------------------------------------------
  Database::Database (const std::string &armbianBoardTag, const std::string &cinfo) :
    cppdb::session (), _cinfo (findConnectionInfo (cinfo)) {

    if (!_cinfo.empty()) {

      cppdb::session::open (_cinfo);
    }
    _board = std::make_shared<Board> (armbianBoardTag);
  }

  // -----------------------------------------------------------------------------
  Database::~Database() {

    cppdb::session::close();
  }

  // -----------------------------------------------------------------------------
  void
  Database::setConnectionInfo (const std::string &cinfo) {

    if (cppdb::session::is_open()) {
      cppdb::session::close();
    }
    _cinfo = cinfo;
    cppdb::session::open (_cinfo);
  }

  // -----------------------------------------------------------------------------
  std::string
  Database::findConnectionInfo (const std::string &cinfo) {
    std::string ret (cinfo), str;

    if (ret.empty()) {
      const char *env;

      env = std::getenv ("PIDUINO_CONN_INFO");
      if (env) {

        ret = std::string (env);
      }
      else {
        std::string fn (PIDUINO_INSTALL_ETC_DIR);
        fn += "/piduino.conf";

        if (System::fileExists (fn)) {
          ConfigFile cfg (fn);

          ret = cfg.value ("connection_info");
        }
      }
    }
    return ret;
  }

  // -----------------------------------------------------------------------------
  //
  //                            Database::Board Class
  //
  // -----------------------------------------------------------------------------

  // -----------------------------------------------------------------------------
  Database::Board::Board (int cpuinfoBoardRevision) :
    _id (-1), _gpio_id (-1), _revision (-1), _found (false), _default_i2c_id (-1),
    _default_spi_id (-1), _default_uart_id (-1) {

    _ram = system.totalRam();
    if (cpuinfoBoardRevision == -1) {
      // if the revision is not provided, try to detect the host system model...

      probingSystem();
    }
    else {

      setRevision (cpuinfoBoardRevision);
    }
  }

  // -----------------------------------------------------------------------------
  Database::Board::Board (const std::string &t) : Board() {

    setTag (t);
  }

  // -----------------------------------------------------------------------------
  Database::Board::~Board() {

  }

  // -----------------------------------------------------------------------------
  bool
  Database::Board::probingSystem() {
    std::string fn (PIDUINO_INSTALL_ETC_DIR);
    fn += "/piduino.conf";

    if (System::fileExists (fn)) {
      ConfigFile cfg (fn);

      if (cfg.keyExists ("tag")) {
        std::string t = cfg.value ("tag");

        // BOARD Tag from /etc/piduino.conf found...
        if (setTag (t)) {

          return true;
        }
      }

      if (cfg.keyExists ("revision")) {
        std::string s = cfg.value ("revision");

        if (!s.empty()) {
          // Revision from /etc/piduino.conf found...
          try {
            unsigned long r = std::stoul (s, nullptr, 0);

            // BOARD Revision from /etc/piduino.conf found...
            if (setRevision (r)) {

              return true;
            }
          }
          catch(std::invalid_argument &e) {
            
            throw std::runtime_error ("Invalid revision value in piduino.conf: " + s);
          }
        }
      }
    }

    if (system.armbianInfo().found()) {

      // BOARD Tag from /etc/armbian-release found...
      return setTag (system.armbianInfo().board());
    }
    else if (system.raspianInfo().found()) {

      return setRevision (system.raspianInfo().value());
    }
    else {

      // Raspberry Pi ?
      if ( (system.revision() > 0) &&
           ( (system.hardware() == "BCM2708") || (system.hardware() == "BCM2709") ||
             (system.hardware() == "BCM2710") || (system.hardware() == "BCM2711") ||
             (system.hardware() == "BCM2835") || (system.hardware() == "BCM2836") ||
             (system.hardware() == "BCM2837") || (system.hardware() == "BCM2838"))) {

        // Revision from /proc/cpuinfo found...
        return setRevision (system.revision());
      }
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  bool
  Database::Board::setRevision (int rev) {

    if (Piduino::db.is_open()) {

      cppdb::result res = Piduino::db <<
                          "SELECT id,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id"
                          " FROM board"
                          " INNER JOIN revision ON board.id = revision.board_id"
                          " WHERE revision.revision=?" << rev << cppdb::row;
      if (!res.empty()) {
        int bmid;
        int mid;

        res >> _id >> _pcb_revision >> bmid >> _gpio_id >> mid >> _default_i2c_id >> _default_spi_id >> _default_uart_id;
        _model.setId (static_cast<Database::Board::Model::Id> (bmid));
        _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
        _revision = rev;
        _found = true;
        return true;
      }
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  bool
  Database::Board::setTag (const std::string &t) {

    if (Piduino::db.is_open()) {
      cppdb::result res = Piduino::db <<
                          "SELECT id,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id"
                          " FROM board"
                          " INNER JOIN tag ON board.id = tag.board_id"
                          " WHERE tag.tag=?" << t << cppdb::row;
      if (!res.empty()) {
        int bmid;
        int mid;

        res >> _id >> _pcb_revision >> bmid >> _gpio_id >> mid >> _default_i2c_id >> _default_spi_id >> _default_uart_id;
        _model.setId (static_cast<Database::Board::Model::Id> (bmid));
        _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
        _tag = t;
        _found = true;
        return true;
      }
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  std::ostream &operator<< (std::ostream &os, const Database::Board &b)  {
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
    if (system.armbianInfo().found()) {

      os << "Armbian Board   : " << system.armbianInfo().board() << std::endl;
    }
    os << "SoC             : " << b.soc().name() << " (" << b.soc().manufacturer().name() << ")" << std::endl;
    os << "Memory          : " << system.totalRam() << "MB" << std::endl;
    os << "GPIO Id         : " << b.gpioId() << std::endl;
    if (!b.pcbRevision().empty()) {

      os << "PCB Revision    : " << b.pcbRevision() << std::endl;
    }
    return os;
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
      Piduino::db << "SELECT name,i2c_syspath,spi_syspath,uart_syspath FROM board_family WHERE id=?"
      << i << cppdb::row;

    if (!res.empty()) {

      _id = i;
      res >> _name >> _i2c_syspath >>  _spi_syspath >> _uart_syspath;
    }
  }
}

/* ========================================================================== */
