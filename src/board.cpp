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
#include <piduino/board.h>
#include <piduino/database.h>

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

    cppdb::result res = Piduino::db << "SELECT tag,revision,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id FROM board WHERE id=?" << i << cppdb::row;
    if (!res.empty()) {
      int bmid;
      int mid;

      _id = i;
      res >> _tag >> _revision >> _ram >> _pcb_revision >> bmid >> _gpio_id >> mid;
      _model.setId (static_cast<Board::Model::Id> (bmid));
      _manufacturer.setId (static_cast<Manufacturer::Id> (mid));
    }
  }
//static_cast<>()

// -----------------------------------------------------------------------------
  Board::Board() : _id (-1), _gpio_id (-1), _revision (-1), _pcb_revision (-1), _ram (-1) {
    bool boardFound;

    db.is_open();

    _ram = ramDetect();
    boardFound = armbianDetect();
    if (!boardFound) {

      boardFound = raspbianDetect();
    }
  }

// -----------------------------------------------------------------------------
  Board::~Board() {
  }

// -----------------------------------------------------------------------------
#if 0
  static int
  iStartWith (const char * text, const char * with) {
    while (*text == ' ') {
      text++;
    }
    return strstr (text, with) == text;
  }
#endif

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
  bool Board::raspbianDetect() {
//#ifndef __ARM_ARCH
#if 0
    int revision = 0;
    /* Read the entire contents of /proc/cpuinfo into the buffer.  */
    FILE * fp = fopen ("/proc/cpuinfo", "r");

    if (fp) {
      char *p;
      char buffer[1024];
      bool bisBcm = false;

      while ( (p = fgets (buffer, sizeof (buffer), fp))) {

        /* Locate Hardware for check if is it a Raspberry  */
        if (iStartWith (p, "Hardware")) {

          if (strstr (p, "BCM2710") && strstr (p, "BCM2709") &&
              strstr (p, "BCM2708") && strstr (p, "BCM2837") &&
              strstr (p, "BCM2836") && strstr (p, "BCM2837")) {

            return 0;
          }
          bisBcm = true;
        }
        else if ( (iStartWith (p, "Revision")) && (bisBcm)) {

          p = strstr (p, "Revision");
          /* Parse the line to extract the revision.  */
          if (sscanf (p, "Revision  : %x", &revision) > 0) {

            iRev = revision;
          }
        }
      }
      if (fclose (fp) < 0) {

        perror ("fclose");
      }
      return revision;
    }

    perror ("fopen");
    return -1;
#endif
    return false;
  }
// -----------------------------------------------------------------------------
  bool Board::armbianDetect() {
#ifndef __ARM_ARCH
#endif
    return false;
  }

}

/* ========================================================================== */
