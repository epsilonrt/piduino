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
#include <sys/stat.h>
#include <algorithm>
#include <piduino/system.h>
#include <piduino/configfile.h>
#include "config.h"

#ifndef __ARM_ARCH
#error PiDuino library currently only supports the ARM architecture !
#endif

namespace Piduino {

  // ---------------------------------------------------------------------------
  //
  //                        System Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  bool System::fileExists (const char * path) {
    struct stat fs;
    
    return (stat (path, &fs) == 0);
  }

  // ---------------------------------------------------------------------------
  bool System::charFileExists (const char * path) {
    struct stat fs;

    if (stat (path, &fs) == 0) {
      
      return S_ISCHR (fs.st_mode) != 0;
    }
    return false;
  }
  
  // ---------------------------------------------------------------------------
  bool System::directoryExists (const char * path) {
    struct stat fs;

    if (stat (path, &fs) == 0) {
      
      return S_ISDIR (fs.st_mode) != 0;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  System::System () : _revision (0), _sn (0), _ram (-1), _ncore (1) {

    readTotalRam();
    readCpuInfo();
  }


  // ---------------------------------------------------------------------------
  void
  System::readTotalRam() {
    struct sysinfo info;
    long roundram = -1;

    if (sysinfo (&info) == 0) {
      unsigned long totalram = (info.totalram * info.mem_unit) / (1024 * 1024);

      roundram = 1;
      while (roundram < totalram) {

        roundram <<= 1;
      }
    }
    _ram = roundram;
  }

  /* ---------------------------------------------------------------------------
    Détection /proc/cpuinfo
    Lecture des paramètres Hardware et Revision dans /proc/cpuinfo
    $ cat /proc/cpuinfo

    The other variables are:

    Hardware    : HiKey Development Board is self explanatory

    ------------ Raspberry Pi B
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

    ------------ Raspberry Pi 2
    processor : 0
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 38.40
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    processor : 1
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 38.40
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    processor : 2
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 38.40
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    processor : 3
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 38.40
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    Hardware  : BCM2835
    Revision  : a01041
    Serial    : 0000000043c0c334

    ------------ NanoPi Neo
    processor : 0
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 91.20
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    processor : 1
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 91.20
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    processor : 2
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 91.20
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    processor : 3
    model name  : ARMv7 Processor rev 5 (v7l)
    BogoMIPS  : 91.20
    Features  : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
    CPU implementer : 0x41
    CPU architecture: 7
    CPU variant : 0x0
    CPU part  : 0xc07
    CPU revision  : 5

    Hardware  : Allwinner sun8i Family
    Revision  : 0000
    Serial    : 02c00081aa289408
   */
  void
  System::readCpuInfo() {

    if (fileExists ("/proc/cpuinfo")) {
      std::string str;
      ConfigFile cfg ("/proc/cpuinfo", ':');

      _hardware = cfg.value ("Hardware");

      if (cfg.keyExists ("Revision")) {

        str = cfg.value ("Revision");
        _revision = std::stol (str, nullptr, 16);
      }

      if (cfg.keyExists ("Serial")) {

        str = cfg.value ("Serial");
        _sn = std::stoll (str, nullptr, 16);
      }

      if (cfg.keyExists ("Features")) {
        std::istringstream sts (cfg.value ("Features"));

        while (std::getline (sts, str, ' ')) {

          _core.features.push_back (str);
        }
      }
      _ncore = cfg.value<int> ("processor", 0) + 1;
      _core.model = cfg.value ("model name");
      _core.bogomips = cfg.value<double> ("BogoMIPS", 0);
      _core.implementer = cfg.value<int> ("CPU implementer", 0);
      _core.archno = cfg.value<int> ("CPU architecture", 0);
      _core.variant = cfg.value<int> ("CPU variant", 0);
      _core.partno = cfg.value<int> ("CPU part", 0);
      _core.revision = cfg.value<int> ("CPU revision", 0);
    }
  }

  // ---------------------------------------------------------------------------
  //
  //                         System::ArmbianInfo Class
  //
  // ---------------------------------------------------------------------------
  /*
    BOARD=nanopineo
    BOARD_NAME="NanoPi Neo"
    VERSION=5.23
    LINUXFAMILY=sun8i
    BRANCH=default
    ARCH=arm

    BOARD=nanopineo
    BOARD_NAME="NanoPi Neo"
    BOARDFAMILY=sun8i
    VERSION=5.41
    LINUXFAMILY=sunxi
    BRANCH=next
    ARCH=arm
    IMAGE_TYPE=user-built
    BOARD_TYPE=conf
    INITRD_ARCH=arm
    KERNEL_IMAGE_TYPE=zImage

    /etc/friendlyelec-release
    BOARD="NanoPi-NEO-Core2"
    BOARD_NAME="NanoPi-NEO-Core2"
    LINUXFAMILY=Allwinnersun50iw2Family
    BRANCH=dev
    BOARD_VENDOR=FriendlyELEC
    ARCH=arm

   */

  // ---------------------------------------------------------------------------
  System::ArmbianInfo::ArmbianInfo() : _valid (false) {
    std::vector<std::string> filename = { "/etc/friendlyelec-release",
                                          "/etc/armbian-release"
                                        };
    int findex = -1;

    for (int i = 0; i < filename.size(); i++) {

      if (fileExists (filename[i])) {
        findex = i;
        break;
      }
    }

    if (findex >= 0) {

      ConfigFile cfg (filename[findex]);
      _valid = true;
      _board = cfg.value ("BOARD");
      transform (_board.begin(), _board.end(), _board.begin(), ::tolower);
      _board.erase (std::remove (_board.begin(), _board.end(), '-'), _board.end());
      _boardName = cfg.value ("BOARD_NAME");

      if (cfg.keyExists ("VERSION")) {

        _version = cfg.value ("VERSION");
      }

      if (cfg.keyExists ("BOARDFAMILY")) {

        _family = cfg.value ("BOARDFAMILY");
      }
      else {

        _family = cfg.value ("LINUXFAMILY");
      }
      _branch = cfg.value ("BRANCH");
      _arch = cfg.value ("ARCH");
    }
  }
}

/* ========================================================================== */
