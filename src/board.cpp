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

#ifndef __ARM_ARCH
#error PiDuino library currently only supports the ARM architecture !
#endif

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
  void Board::setId (int i)  {

    cppdb::result res = Piduino::db << "SELECT tag,revision,ram,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE id=?" << i << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      _id = i;
      res >> _tag >> _revision >> _ram >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
  }

// -----------------------------------------------------------------------------
  void Board::setRevision (int rev) {

    cppdb::result res = Piduino::db << "SELECT id,tag,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE revision=?" << rev << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      _revision = rev;
      res >> _id >> _tag >> _pcb_revision >> bmid >> _gpio_id >> mid;
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

    cppdb::result res = Piduino::db << "SELECT id,revision,pcb_revision,"
                        "board_model_id,gpio_id,manufacturer_id FROM board "
                        "WHERE tag=?" << t << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      _tag = t;
      res >> _id >> _revision >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
    else {

      throw std::invalid_argument ("Couldn't find a board with " +
                                   t + " tag !");
    }
  }

// -----------------------------------------------------------------------------
  Board::Board (int rev) : _revision (rev), _id (-1), _gpio_id (-1), _pcb_revision (-1) {

    _ram = totalRam();
    if (rev == -1) {
      // if the revision is not provided, try to detect the host system model...
      std::string tag = findTag();

      if (!tag.empty()) {
        // BOARD Tag from /etc/armbian-release found...
        setTag (tag);
      }
      else {
        
        rev = findRevision();
        if (rev > 0) {
          
          // Revision from /proc/cpuinfo found...
          setRevision (rev);
        }
        else {

          throw std::system_error (ENOTSUP, std::system_category(),
                                   "Unable to identify your host system");
        }

      }
    }
    else {

      setRevision (rev);
    }
  }

// -----------------------------------------------------------------------------
  Board::Board (const std::string & t) : Board() {

    setTag (t);
  }

// -----------------------------------------------------------------------------
  Board::~Board() {

  }

// -----------------------------------------------------------------------------
  int
  Board::totalRam() {
    struct sysinfo info;
    int roundram = -1;

    if (sysinfo (&info) == 0) {
      unsigned long totalram = (info.totalram * info.mem_unit) / (1024 * 1024);

      roundram = 1;
      while (roundram < totalram) {

        roundram <<= 1;
      }
    }
    return roundram;
  }

  /* ---------------------------------------------------------------------------
    Détection Raspbian
    Lecture des paramètres Hardware et Revision dans /proc/cpuinfo
    $ cat /proc/cpuinfo
    processor : 0
    model name  : ARMv6-compatible processor rev 7 (v6l)
    BogoMIPS  : 2.00
    Features  : half thumb fastmult vfp edsp java tls
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xb76
    CPU revision  : 7

    Hardware  : BCM2708
    Revision  : 000e
    Serial    : 0000000059ad5ead
   */
  int
  Board::findRevision() {
    int rev = -1;

    if (ConfigFile::fileExist ("/proc/cpuinfo")) {
      ConfigFile cfg ("/proc/cpuinfo", ':');

      if (cfg.keyExists ("Hardware")) {
        std::string hardware = cfg.value<std::string> ("Hardware");

        if ( (hardware == "BCM2710") || (hardware == "BCM2709") ||
             (hardware == "BCM2708") || (hardware == "BCM2837") ||
             (hardware == "BCM2836") || (hardware == "BCM2837")) {

          if (cfg.keyExists ("Revision")) {

            rev = cfg.value<int> ("Revision");
          }
        }
      }
    }
    return rev;
  }

  /* -----------------------------------------------------------------------------
    $ cat /etc/armbian-release
    # PLEASE DO NOT EDIT THIS FILE
    BOARD=nanopineo
    BOARD_NAME="NanoPi Neo"
    VERSION=5.23
    LINUXFAMILY=sun8i
    BRANCH=default
    ARCH=arm
   */
  std::string
  Board::findTag() {

    if (ConfigFile::fileExist ("/etc/armbian-release")) {

      ConfigFile cfg ("/etc/armbian-release");
      return cfg.value<std::string> ("BOARD");
    }
    return std::string ();
  }

}

/* ========================================================================== */
