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

#ifndef PIDUINO_SYSTEM_H
#define PIDUINO_SYSTEM_H

#include <string>
#include <vector>
#include <fstream>

/**
 *  @defgroup piduino_system System
 *
 *  Ce module fournit informations concernant le système hôte.
 *  @{
 */

namespace Piduino {

  class System {
    public:
      /*
        processor : 0
        model name  : ARMv6-compatible processor rev 7 (v6l)
        BogoMIPS  : 2.00
        Features  : half thumb fastmult vfp edsp java tls
        CPU implementer : 0x41
        CPU architecture: 7
        CPU variant : 0x0
        CPU part  : 0xb76
        CPU revision  : 7

        CPU implementer: Your code means ARM;
        CPU architecture: AArch64 means 64 bit ARM board:
        CPU variant : Indicates the variant number of the processor, or "major revision". Yours is zero.
        CPU part: Part number. 0xd03 indicates Cortex-A53 processor.
        CPU revision: Indicates patch release or "minor revision". 3, in your case
       */
      // -----------------------------------------------------------------------
      //
      //                         System::ArmCore Class
      //
      // -----------------------------------------------------------------------
      class ArmCore {
        public:
          ArmCore() : bogomips (0) , implementer (0), archno (0), variant (0), revision (0), partno (0) {}
          std::string model;
          std::vector<std::string> features;
          double bogomips;
          int implementer;
          int archno;
          int variant;
          int revision;
          int partno;
      };

      // -----------------------------------------------------------------------
      //
      //                         System::ArmbianInfo Class
      //
      // -----------------------------------------------------------------------
      class ArmbianInfo {
        public:
          ArmbianInfo();

          inline bool found() const {
            return _valid;
          }
          inline const std::string & board() const {
            return _board;
          }
          inline const std::string & boardName() const {
            return _boardName;
          }
          inline const std::string & version() const {
            return _version;
          }
          inline const std::string & family() const {
            return _family;
          }
          inline const std::string & branch() const {
            return _branch;
          }
          inline const std::string & arch() const {
            return _arch;
          }

        private:
          bool _valid;
          std::string _board;
          std::string _boardName;
          std::string _version;
          std::string _family;
          std::string _branch;
          std::string _arch;
      };

      // -----------------------------------------------------------------------
      //
      //                         System Class
      //
      // -----------------------------------------------------------------------
      System();

      // -----------------------------------------------------------------------
      static bool
      fileExist (const std::string &filename) {

        return std::ifstream (filename).good();
      }

      inline const std::string & hardware() const {
        return _hardware;
      }

      inline unsigned long long serialNumber() const {
        return _sn;
      }

      inline unsigned short revision() const {
        return _revision;
      }

      inline unsigned long totalRam() const {
        return _ram;
      }

      inline const ArmCore & core() const {
        return _core;
      }

      inline const ArmbianInfo & armbianInfo() const {
        return _armbian;
      }

    protected:
      void readTotalRam();
      void readCpuInfo();

    private:
      std::string _hardware;
      unsigned short _revision;
      unsigned long long _sn;
      long _ram; // RAM en Mo
      int _ncore;
      ArmCore _core;
      ArmbianInfo _armbian;
  };
  
  // ---------------------------------------------------------------------------
  //
  //                      Piduino System Global Object
  //
  // ---------------------------------------------------------------------------
  extern System system; ///< Piduino System Global Object
  /**
   * @}
   */
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_SYSTEM_H defined */
