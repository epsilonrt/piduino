/* Copyright © 2018 Pascal JEAN, All rights reserved.
 *
 * Piduino pinfo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Piduino pinfo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Piduino pinfo.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include <piduino/database.h>
#include <piduino/serialport.h>
#ifndef PIDUINO_PROJECT_BUILD
#include <piduino/config.h>
#else
#include <config.h>
#endif
#if PIDUINO_WITH_I2C
#include <piduino/i2cdev.h>
#endif
#if PIDUINO_WITH_SPI
#include <piduino/spidev.h>
#endif

#include "version.h"

/* constants ================================================================ */
#define OPT_SOC     0x0001
#define OPT_REV     0x0002
#define OPT_GPIOID  0x0004
#define OPT_MEM     0x0008
#define OPT_PCB     0x0010
#define OPT_MAN     0x0020
#define OPT_NAME    0x0040
#define OPT_ID      0x0080
#define OPT_TAG     0x0100
#define OPT_FAM     0x0200
#define OPT_I2C     0x0400
#define OPT_SPI     0x0800
#define OPT_SER     0x1000
#define OPT_ALL     0xFFFF

namespace Pinfo {

  const std::string authors = "Pascal JEAN";
  const std::string website = "https://github.com/epsilonrt/piduino";

  /* private functions ======================================================== */
  void warranty ();
  void usage ();
  void version ();
  std::string i2cBuses();
  std::string spiBuses();
  std::string serialPorts();
  void printWithLabels (uint16_t flags);
}

using namespace std;
using namespace Piduino;
using namespace Pinfo;

/* main ===================================================================== */
int
main (int argc, char **argv) {
  const char *short_options = "srgmpbnitfahvw"
#if PIDUINO_WITH_I2C
                              "I"
#endif
#if PIDUINO_WITH_SPI
                              "P"
#endif
                              "S"
                              ;
  static const struct option long_options[] = {
    {"soc",  no_argument, NULL, 's'},       // OPT_SOC
    {"revision",  no_argument, NULL, 'r'},  // OPT_REV
    {"gpio",  no_argument, NULL, 'g'},      // OPT_GPIOID
    {"mem",  no_argument, NULL, 'm'},       // OPT_MEM
    {"pcb",  no_argument, NULL, 'p'},       // OPT_PCB
    {"builder",  no_argument, NULL, 'b'},   // OPT_MAN
    {"name",  no_argument, NULL, 'n'},      // OPT_NAME
    {"id",  no_argument, NULL, 'i'},        // OPT_ID
    {"tag",  no_argument, NULL, 't'},       // OPT_TAG
    {"family",  no_argument, NULL, 'f'},    // OPT_FAM
#if PIDUINO_WITH_I2C
    {"i2c",  no_argument, NULL, 'I'},       // OPT_I2C
#endif
#if PIDUINO_WITH_SPI
    {"spi",  no_argument, NULL, 'P'},       // OPT_SPI
#endif
    {"serial",  no_argument, NULL, 'S'},    // OPT_SER
    {"all",  no_argument, NULL, 'a'},       // OPT_ALL
    {"warranty",  no_argument, NULL, 'w'},
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };

  int i;
  uint16_t flags = 0, count = 0;
  std::string str;

  do {
    i = getopt_long (argc, argv, short_options, long_options, NULL);

    switch (i) {

      case 's':
        count++;
        flags |= OPT_SOC;
        break;

      case 'r':
        count++;
        flags |= OPT_REV;
        break;

      case 'g':
        count++;
        flags |= OPT_GPIOID;
        break;

      case 'm':
        count++;
        flags |= OPT_MEM;
        break;

      case 'p':
        count++;
        flags |= OPT_PCB;
        break;

      case 'b':
        count++;
        flags |= OPT_MAN;
        break;

      case 'n':
        count++;
        flags |= OPT_NAME;
        break;

      case 'i':
        count++;
        flags |= OPT_ID;
        break;

      case 't':
        count++;
        flags |= OPT_TAG;
        break;

      case 'f':
        count++;
        flags |= OPT_FAM;
        break;

#if PIDUINO_WITH_I2C
      case 'I':
        count++;
        flags |= OPT_I2C;
        break;
#endif

#if PIDUINO_WITH_SPI
      case 'P':
        count++;
        flags |= OPT_SPI;
        break;
#endif

      case 'S':
        count++;
        flags |= OPT_SER;
        break;

      case 'a':
        count = -1;
        flags = 0xFFFF;
        break;

      case 'w':
        warranty();
        exit (EXIT_SUCCESS);
        break;

      case 'h':
        usage();
        exit (EXIT_SUCCESS);
        break;

      case 'v':
        version();
        exit (EXIT_SUCCESS);
        break;

      case -1:
        break;

      default:
        /* An invalid option has been used, exit with code EXIT_FAILURE */
        exit (EXIT_FAILURE);
        break;
    }
  }
  while (i != -1);

  if (db.board().found()) {

    if (count == 0) {
      // Pas d'option fournie, all par défaut
      flags = OPT_ALL;
      count = OPT_ALL;
    }

    if (count > 1) {

      printWithLabels (flags);
    }
    else {

      // Une seule info demandée, affichage brut
      switch (flags) {
        case OPT_SOC:
          cout  << db.board().soc().name() << endl;
          break;
        case OPT_REV:
          if (db.board().revision() > 0) {
            cout  << hex << showbase << db.board().revision() << noshowbase << dec << endl;
          }
          else {
            cout << "none" << endl;
          }
          break;
        case OPT_GPIOID:
          cout  << db.board().gpioId() << endl;
          break;
        case OPT_MEM:
          cout  << db.board().totalRam() << endl;
          break;
        case OPT_PCB:
          if (!db.board().pcbRevision().empty()) {
            cout  << db.board().pcbRevision() << endl;
          }
          else {
            cout << "none" << endl;
          }
          break;
        case OPT_MAN:
          cout  << db.board().manufacturer().name() << endl;
          break;
        case OPT_NAME:
          cout  << db.board().name() << endl;
          break;
        case OPT_ID:
          cout  << db.board().id() << endl;
          break;
        case OPT_TAG:
          cout  << db.board().tag() << endl;
          break;
        case OPT_FAM:
          cout  << db.board().family().name() << endl;
          break;
#if PIDUINO_WITH_I2C
        case OPT_I2C:
          str = i2cBuses();
          if (!str.empty()) {
            cout  << str << endl;
          }
          break;
#endif
#if PIDUINO_WITH_SPI
        case OPT_SPI:
          str = spiBuses();
          if (!str.empty()) {
            cout  << str << endl;
          }
          break;
#endif
        case OPT_SER:
          str = serialPorts();
          if (!str.empty()) {
            cout  << str << endl;
          }
          break;
        default:
          exit (EXIT_FAILURE);
          break;
      }
    }
    return 0;
  }

  cerr << "Unable to identify your host system !" << endl;
  return 1;
}

namespace Pinfo {

// -----------------------------------------------------------------------------
  /*
      Name            : NanoPi Neo
      Family          : NanoPi
      Database Id     : 31
      Manufacturer    : Friendly ARM
      Armbian Board   : nanopineo
      SoC             : H3(Allwinner)
      Memory          : 512MB
      GPIO Id         : 4
   */
  void
  printWithLabels (uint16_t flags) {
    if (flags & OPT_NAME) {
      cout << "Name            : " << db.board().name() << endl;
    }
    if (flags & OPT_FAM) {
      cout << "Family          : " << db.board().family().name() << endl;
    }
    if (flags & OPT_ID) {
      cout << "Database Id     : " << db.board().id() << endl;
    }
    if (flags & OPT_MAN) {
      cout << "Manufacturer    : " << db.board().manufacturer().name() << endl;
    }
    if ( (flags & OPT_REV) && (db.board().revision() >= 0)) {
      cout << "Board Revision  : " << hex << showbase << db.board().revision() << noshowbase << dec << endl;
    }
    if ( (flags & OPT_TAG) && (db.board().tag().empty() == false))  {

      cout << "Board Tag       : " << db.board().tag() << endl;
    }
    if (flags & OPT_SOC) {
      cout << "SoC             : " << db.board().soc().name() << " (" << db.board().soc().manufacturer().name() << ")" << endl;
    }
    if (flags & OPT_MEM) {
      cout << "Memory          : " << db.board().totalRam() << "MB" << endl;
    }
    if (flags & OPT_GPIOID) {
      cout << "GPIO Id         : " << db.board().gpioId() << endl;
    }
    if ( (flags & OPT_PCB) && (!db.board().pcbRevision().empty())) {

      cout << "PCB Revision    : " << db.board().pcbRevision() << endl;
    }
#if PIDUINO_WITH_I2C
    if (flags & OPT_I2C) {
      std::string buses = i2cBuses();

      if (!buses.empty()) {

        cout << "I2C Buses       : " << buses << endl;
      }
    }
#endif
#if PIDUINO_WITH_SPI
    if (flags & OPT_SPI) {
      std::string buses = spiBuses();

      if (!buses.empty()) {

        cout << "SPI Buses       : " << buses << endl;
      }
    }
#endif
    if (flags & OPT_SER) {
      std::string ports = serialPorts();

      if (!ports.empty()) {

        cout << "Serial Ports    : " << ports << endl;
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  version() {

    cout << "Piduino pinfo version " << VERSION_SHORT << endl;
    cout << "Copyright © 2018-2019 " << authors << ", " << website << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
    cout << "This is free software, and you are welcome to redistribute it" << endl;
    cout << "under certain conditions; type 'pinfo -w' for details." << endl << endl;
  }

// -----------------------------------------------------------------------------
  void
  warranty () {
    cout <<
         "Copyright © 2018-2019 " << authors << ", All rights reserved.\n\n"

         " Piduino pinfo is free software: you can redistribute it and/or modify\n"
         " it under the terms of the GNU General Public License as published by\n"
         " the Free Software Foundation, either version 3 of the License, or\n"
         " (at your option) any later version.\n\n"

         " Piduino pinfo is distributed in the hope that it will be useful,\n"
         " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
         " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
         " GNU General Public License for more details.\n\n"

         " You should have received a copy of the GNU General Public License\n"
         " along with Piduino pinfo. If not, see <http://www.gnu.org/licenses/>.\n";
  }

// -----------------------------------------------------------------------------
  void
  usage () {
    cout << "usage : " << System::progName() << " [ options ]" << endl;
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789
    cout << "Retrieves information from the pi board." << endl;
    cout << "If only one information is requested, it is displayed without formatting," << endl;
    cout << "otherwise each information is displayed on a line preceded by its name." << endl;
    cout << "Return value: 0 if the board is a pi board, 1 otherwise" << endl;
    cout << "valid options are :" << endl;
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789
    cout << "  -a  --all       \tPrints all informations (default)" << endl;
    cout << "  -n  --name      \tPrints the \"human-readable\" name of the board." << endl;
    cout << "  -f  --family    \tPrints the board family (raspberrypi, nanopi, ...)" << endl;
    cout << "  -i  --id        \tPrints the piduino database id (for debug purpose)" << endl;
    cout << "  -s  --soc       \tPrints the SoC model (bcm2708 ...)" << endl;
    cout << "  -r  --revision  \tPrints the revision number of the board in hexadecimal (prefixed by 0x)." << endl;
    cout << "  -t  --tag       \tPrints the identifier tag of the board." << endl;
    cout << "  -g  --gpio      \tPrints the revision number of the GPIO in decimal" << endl;
    cout << "  -m  --mem       \tPrints the RAM size in megabytes" << endl;
    cout << "  -p  --pcb       \tPrints the revision number of the PCB in the form M.m" << endl;
    cout << "  -b  --builder   \tPrints the name of the manufacturer." << endl;
#if PIDUINO_WITH_I2C
    cout << "  -I  --i2c       \tPrints I2c buses available on the SoC." << endl;
#endif
#if PIDUINO_WITH_SPI
    cout << "  -P  --spi       \tPrints SPI buses available on the SoC." << endl;
#endif
    cout << "  -S  --serial    \tPrints serial ports available on the SoC." << endl;
    cout << "  -h  --help      \tPrints this message" << endl;
    cout << "  -v  --version   \tPrints version and exit" << endl;
    cout << "  -w  --warranty  \tOutput the warranty and exit." << endl;
  }

// -----------------------------------------------------------------------------
  std::string i2cBuses() {
    std::string ret;
    const auto buses = I2cDev::Info::availableBuses();

    for (const I2cDev::Info & bus : buses) {

      if (ret.size()) {
        ret.push_back (',');
      }
      ret += bus.path();
    }
    return ret;
  }

// -----------------------------------------------------------------------------
  std::string spiBuses() {
    std::string ret;
    const auto buses = SpiDev::Info::availableBuses();

    for (const SpiDev::Info & bus : buses) {

      if (ret.size()) {
        ret.push_back (',');
      }
      ret += bus.path();
    }
    return ret;
  }

// -----------------------------------------------------------------------------
  std::string serialPorts() {
    const auto ports = SerialPort::Info::availablePorts();
    std::string ret;

    for (const SerialPort::Info & port : ports) {
      if (ret.size()) {
        ret.push_back (',');
      }
      ret += port.systemLocation();
    }

    return ret;
  }
}
/* ========================================================================== */
