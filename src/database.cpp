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
    _board = std::make_shared<Board> (true);
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
  // long long _id;
  // long long _gpio_id; // Révision du GPIO
  // int _revision;
  // bool _found;
  // long _ram; // RAM en Mo
  // int _default_i2c_id;
  // int _default_spi_id;
  // int _default_uart_id;

  Database::Board::Board (bool isSelectFromSystem) :
    _id (-1), _gpio_id (-1), _revision (-1), _found (false), _ram (-1), _default_i2c_id (-1),
    _default_spi_id (-1), _default_uart_id (-1) {

    if (isSelectFromSystem) {

      // Select board from system information : revision or tag
      selectFromSystem();
    }
  }

  // -----------------------------------------------------------------------------
  Database::Board::Board (int rpiBoardRevision) : Board() {

    if (!selectWhereRevision (rpiBoardRevision)) {

      throw std::runtime_error ("Board not found in database for revision: " + std::to_string (rpiBoardRevision));
    }
  }

  // -----------------------------------------------------------------------------
  Database::Board::Board (const std::string &armbianBoardTag) : Board() {

    if (! selectWhereTag (armbianBoardTag)) {

      throw std::runtime_error ("Board not found in database for tag: " + armbianBoardTag);
    }
  }

  // -----------------------------------------------------------------------------
  Database::Board::~Board() {

  }

  // -----------------------------------------------------------------------------
  // private
  bool
  Database::Board::selectWhereRevision (int rev) {

    if (Piduino::db.is_open()) {

      cppdb::result res = Piduino::db <<
                          "SELECT id,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id"
                          " FROM board"
                          " INNER JOIN revision ON board.id = revision.board_id"
                          " WHERE revision.revision=?" << rev << cppdb::row;
      if (!res.empty()) {
        int bmid;
        int mid;

        res >> _id  >> _ram >> _pcb_revision >> bmid >> _gpio_id >> mid >> _default_i2c_id >> _default_spi_id >> _default_uart_id;
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
  // private
  bool
  Database::Board::selectWhereTag (const std::string &t) {

    if (Piduino::db.is_open()) {
      cppdb::result res = Piduino::db <<
                          "SELECT id,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id"
                          " FROM board"
                          " INNER JOIN tag ON board.id = tag.board_id"
                          " WHERE tag.tag=?" << t << cppdb::row;
      if (!res.empty()) {
        int bmid;
        int mid;

        res >> _id  >> _ram >> _pcb_revision >> bmid >> _gpio_id >> mid >> _default_i2c_id >> _default_spi_id >> _default_uart_id;
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
  // private
  // Select board from model id
  // if found, set the _id, _gpio_id, _default_i2c_id, _default_spi_id, _default_uart_id and _model
  // and return _found = true
  // if not found, return false and not modify the board informations
  bool Database::Board::selectWhereModel (Model::Id modelId) {
    long long previousGpioId = -1;
    long long boardId;
    int defaultI2cId;
    int defaultSpiId;
    int defaultUartId;

    // select boards with the same model id
    cppdb::result res = Piduino::db <<
                        "SELECT id,gpio_id,default_i2c_id,default_spi_id,default_uart_id FROM board"
                        " WHERE board_model_id=?" << static_cast<long long> (modelId);
    while (res.next()) {
      long long gpioId;

      res >> boardId >> gpioId >> defaultI2cId >> defaultSpiId >> defaultUartId;

      // for each check if the gpioId is the same
      if ( (previousGpioId != -1) && (previousGpioId != gpioId)) {

        // this model have multiple GPIO id
        return false;
      }
      previousGpioId = gpioId;
    }

    if (previousGpioId != -1) {

      _id = boardId;
      _gpio_id = previousGpioId;
      _default_i2c_id = defaultI2cId;
      _default_spi_id = defaultSpiId;
      _default_uart_id = defaultUartId;
      _found = true;
      _model.setId (modelId);
    }

    return _found;
  }

  // -----------------------------------------------------------------------------
  // private
  // Select board from system information : revision or tag
  bool
  Database::Board::selectFromSystem() {
    std::string fn (PIDUINO_INSTALL_ETC_DIR);
    fn += "/piduino.conf";

    if (System::fileExists (fn)) {

      // The configuration file exists, it's the first place to look for
      // the board tag or revision.
      ConfigFile cfg (fn);

      if (cfg.keyExists ("tag")) {
        std::string t = cfg.value ("tag");

        // BOARD Tag from /etc/piduino.conf found...
        if (selectWhereTag (t)) {

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
            if (selectWhereRevision (r)) {

              return true;
            }
          }
          catch (...) {

            throw std::runtime_error ("Invalid revision value in piduino.conf: " + s);
          }
        }
      }
    }

    if (system.armbianInfo().found()) {

      // BOARD Tag from /etc/armbian-release found...
      return selectWhereTag (system.armbianInfo().board());
    }
    else if (system.raspianInfo().found()) {

      if (selectWhereRevision (system.raspianInfo().value())) {

        return true;
      }
      else {
        Model::Id bid = Model::Id::Unknown;

        // revision not found in database
        // try to find the board information with raspbianInfo
        switch (system.raspianInfo().board()) {
          case System::RaspianInfo::BoardType::RpiA:
            bid = Model::RaspberryPiA;
            break;
          case System::RaspianInfo::BoardType::RpiB:
            bid = Model::RaspberryPiB;
            break;
          case System::RaspianInfo::BoardType::RpiAPlus:
            bid = Model::RaspberryPiAPlus;
            break;
          case System::RaspianInfo::BoardType::RpiBPlus:
            bid = Model::RaspberryPiBPlus;
            break;
          case System::RaspianInfo::BoardType::Rpi2B:
            bid = Model::RaspberryPi2B;
            break;
          case System::RaspianInfo::BoardType::Rpi3B:
            bid = Model::RaspberryPi3B;
            break;
          case System::RaspianInfo::BoardType::RpiZero:
            bid = Model::RaspberryPiZero;
            break;
          case System::RaspianInfo::BoardType::RpiCM1:
            bid = Model::RaspberryPiComputeModule;
            break;
          case System::RaspianInfo::BoardType::RpiZeroW:
            bid = Model::RaspberryPiZeroW;
            break;
          case System::RaspianInfo::BoardType::Rpi3BPlus:
            bid = Model::RaspberryPi3BPlus;
            break;
          case System::RaspianInfo::BoardType::Rpi3APlus:
            bid = Model::RaspberryPi3APlus;
            break;
          case System::RaspianInfo::BoardType::Rpi4B:
            bid = Model::RaspberryPi4B;
            break;
          case System::RaspianInfo::BoardType::RpiZero2W:
            bid = Model::RaspberryPiZero2W;
            break;
          case System::RaspianInfo::BoardType::Rpi400:
            bid = Model::RaspberryPi400;
            break;
          case System::RaspianInfo::BoardType::RpiCM3:
            bid = Model::RaspberryPiComputeModule3;
            break;
          case System::RaspianInfo::BoardType::RpiCM3Plus:
            bid = Model::RaspberryPiComputeModule3Plus;
            break;
          case System::RaspianInfo::BoardType::RpiCM4:
            bid = Model::RaspberryPiComputeModule4;
            break;
          case System::RaspianInfo::BoardType::Rpi5:
            bid = Model::RaspberryPi5;
            break;
          case System::RaspianInfo::BoardType::RpiCM5:
            bid = Model::RaspberryPiComputeModule5;
            break;
          case System::RaspianInfo::BoardType::Rpi500:
            bid = Model::RaspberryPi500;
            break;
          case System::RaspianInfo::BoardType::RpiCM5Lite:
            bid = Model::RaspberryPiComputeModule5Lite;
            break;
          default:
            // RpiAlpha = 0x05,
            // RpiInternal = 0x0f,
            // RpiCM4S = 0x15, // TODO
            // RpiInternal2 = 0x16,
            return false;
        }

        if (selectWhereModel (bid)) {
          Manufacturer::Id mid = Manufacturer::Unknown;

          // All boards with this model are the same gpio, we can decode the revision

          _revision = system.raspianInfo().value();
          _pcb_revision = system.raspianInfo().revision();

          switch (system.raspianInfo().manufacturer()) {
            case System::RaspianInfo::Manufacturer::SonyUK:
              mid = Manufacturer::SonyUK;
              break;
            case System::RaspianInfo::Manufacturer::Egoman:
              mid = Manufacturer::Egoman;
              break;
            case System::RaspianInfo::Manufacturer::Embest:
            case System::RaspianInfo::Manufacturer::Embest2:
              mid = Manufacturer::Embest;
              break;
            case System::RaspianInfo::Manufacturer::SonyJapan:
              mid = Manufacturer::SonyJapan;
              break;
            case System::RaspianInfo::Manufacturer::Stadium:
              mid = Manufacturer::Stadium;
              break;
            default:
              // Unknown manufacturer
              break;
          }
          _manufacturer.setId (mid);
          // long _ram; // RAM en Mo
          // Memory256MB = 0x00,
          // Memory512MB = 0x01,
          // Memory1GB = 0x02,
          // Memory2GB = 0x03,
          // Memory4GB = 0x04,
          // Memory8GB = 0x05,
          // Memory16GB = 0x06
          switch (system.raspianInfo().memory()) {
            case System::RaspianInfo::MemorySize::Memory256MB:
              _ram = 256;
              break;
            case System::RaspianInfo::MemorySize::Memory512MB:
              _ram = 512;
              break;
            case System::RaspianInfo::MemorySize::Memory1GB:
              _ram = 1024;
              break;
            case System::RaspianInfo::MemorySize::Memory2GB:
              _ram = 2048;
              break;
            case System::RaspianInfo::MemorySize::Memory4GB:
              _ram = 4096;
              break;
            case System::RaspianInfo::MemorySize::Memory8GB:
              _ram = 8192;
              break;
            case System::RaspianInfo::MemorySize::Memory16GB:
              _ram = 16384;
              break;
            default:
              _ram = system.totalRam();
              break;
          }
        }
      }
    }
    else {

      // Raspberry Pi Legacy
      if ( (system.revision() > 0) &&
           ( (system.hardware() == "BCM2708") || (system.hardware() == "BCM2709") ||
             (system.hardware() == "BCM2710") || (system.hardware() == "BCM2711") ||
             (system.hardware() == "BCM2835") || (system.hardware() == "BCM2836") ||
             (system.hardware() == "BCM2837") || (system.hardware() == "BCM2838"))) {

        // Revision from /proc/cpuinfo found...
        return selectWhereRevision (system.revision());
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
    if (b.tag().empty()) {

      os << "Board Revision  : " << std::hex << std::showbase << b.revision() << std::endl;
    }
    else {

      os << "Armbian Board   : " << b.tag() << std::endl;
    }
    os << "SoC             : " << b.soc().name() << " (" << b.soc().manufacturer().name() << ")" << std::endl;
    os << "Memory          : " << b.totalRam() << "MB" << std::endl;
    os << "GPIO Id         : " << b.gpioId() << std::endl;
    if (!b.pcbRevision().empty()) {

      os << "PCB Revision    : " << b.pcbRevision() << std::endl;
    }
    return os;
  }

  //-----------------------------------------------------------------------------
  bool  Database::Board::boardList (std::map<long long, Board> &boardList) {
    boardList.clear();

    if (Piduino::db.is_open()) {

      cppdb::result res = Piduino::db <<
                          "SELECT id,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id,revision"
                          " FROM board"
                          " INNER JOIN revision ON board.id = revision.board_id";

      while (res.next()) {
        Board b;
        int bmid;
        int mid;

        res >> b._id  >> b._ram >> b._pcb_revision >> bmid >> b._gpio_id >> mid >> b._default_i2c_id >> b._default_spi_id >> b._default_uart_id >> b._revision;
        b._model.setId (static_cast<Database::Board::Model::Id> (bmid));
        b._manufacturer.setId (static_cast<Manufacturer::Id> (mid));
        boardList[b.id()] = b;
      }
      res.clear();

      res = Piduino::db <<
            "SELECT id,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id,tag"
            " FROM board"
            " INNER JOIN tag ON board.id = tag.board_id";
      while (res.next()) {
        Board b;
        int bmid;
        int mid;

        res >> b._id  >> b._ram >> b._pcb_revision >> bmid >> b._gpio_id >> mid >> b._default_i2c_id >> b._default_spi_id >> b._default_uart_id >> b._tag;
        b._model.setId (static_cast<Database::Board::Model::Id> (bmid));
        b._manufacturer.setId (static_cast<Manufacturer::Id> (mid));
        boardList[b.id()] = b;
      }
    }
    return ! boardList.empty();
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
