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

#ifndef PIDUINO_SYSTEM_H
#define PIDUINO_SYSTEM_H

#include <string>
#include <vector>
#include <fstream>

/**
    @defgroup piduino_system System

    Ce module fournit informations concernant le système hôte.
    @{
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
          ArmCore() : bogomips (0), implementer (0), archno (0), variant (0), revision (0), partno (0) {}
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
          inline const std::string &board() const {
            return _board;
          }
          inline const std::string &boardName() const {
            return _boardName;
          }
          inline const std::string &version() const {
            return _version;
          }
          inline const std::string &family() const {
            return _family;
          }
          inline const std::string &branch() const {
            return _branch;
          }
          inline const std::string &arch() const {
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
      //                         System::RaspianInfo Class
      //
      // -----------------------------------------------------------------------
      // This class is used to read the new revision codes found in /proc/cpuinfo
      // and /proc/device-tree/system/linux,revision
      // https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#raspberry-pi-revision-codes
      class RaspianInfo {
        public:
          RaspianInfo();

          // Describe the board revision codes found in /proc/cpuinfo or /proc/device-tree/system/linux,revision
          // RRRR (bits 0-3): Revision
          enum BoardType {
            // TTTTTTTT (bits 4-11)
            RpiA = 0x00,
            RpiB = 0x01,
            RpiAPlus = 0x02,
            RpiBPlus = 0x03,
            Rpi2B = 0x04,
            RpiAlpha = 0x05,
            RpiCM1 = 0x06,
            Rpi3B = 0x08,
            RpiZero = 0x09,
            RpiCM3 = 0x0a,
            RpiZeroW = 0x0c,
            Rpi3BPlus = 0x0d,
            Rpi3APlus = 0x0e,
            RpiInternal = 0x0f,
            RpiCM3Plus = 0x10,
            Rpi4B = 0x11,
            RpiZero2W = 0x12,
            Rpi400 = 0x13,
            RpiCM4 = 0x14,
            RpiCM4S = 0x15,
            RpiInternal2 = 0x16,
            Rpi5 = 0x17,
            RpiCM5 = 0x18,
            Rpi500 = 0x19,
            RpiCM5Lite = 0x1a,
            RpiUnknown = -1
          };

          enum Processor {
            // PPPP (bits 12-15)
            BCM2835 = 0x00,
            BCM2836 = 0x01,
            BCM2837 = 0x02,
            BCM2711 = 0x03,
            BCM2712 = 0x04
          };

          enum Manufacturer {
            // CCCC (bits 16-19)
            SonyUK = 0x00,
            Egoman = 0x01,
            Embest = 0x02,
            SonyJapan = 0x03,
            Embest2 = 0x04,
            Stadium = 0x05
          };

          enum MemorySize {
            // MMM (bits 20-22)
            Memory256MB = 0x00,
            Memory512MB = 0x01,
            Memory1GB = 0x02,
            Memory2GB = 0x03,
            Memory4GB = 0x04,
            Memory8GB = 0x05,
            Memory16GB = 0x06
          };

          inline bool newFlag() const {
            return _info.field.newrev;
          }
          inline bool found() const {
            return newFlag();
          }
          inline uint8_t revision() const {
            return _info.field.revision;
          }
          inline BoardType board() const {
            return _info.field.board;
          }
          inline Processor processor() const {
            return _info.field.processor;
          }
          inline Manufacturer manufacturer() const {
            return _info.field.manufacturer;
          }
          inline MemorySize memory() const {
            return _info.field.memory;
          }
          inline bool warranty() const {
            return _info.field.warranty;
          }
          inline bool otpReady() const {
            return _info.field.otpready;
          }
          inline bool otpProgram() const {
            return _info.field.otpprogram;
          }
          inline bool overVoltage() const {
            return _info.field.overvoltage;
          }
          inline uint32_t value() const {
            return _info.value;
          }

        private:
          struct BoardInfoField {
            uint8_t revision: 4;
            BoardType board: 8;
            Processor processor: 4;
            Manufacturer manufacturer: 4;
            MemorySize memory: 3;
            bool newrev: 1;
            bool unused: 1;
            bool warranty: 1;
            uint8_t unused2: 3;
            bool otpready: 1;
            bool otpprogram: 1;
            bool overvoltage: 1;
          };

          union BoardInfo {
            uint32_t value;
            BoardInfoField field;
            BoardInfo() : value (0) {}
            BoardInfo (uint32_t v) : value (v) {}
            BoardInfo (const BoardInfo &b) : value (b.value) {}
            BoardInfo &operator= (const BoardInfo &b) {
              value = b.value;
              return *this;
            }
            BoardInfo &operator= (long v) {
              value = static_cast<uint32_t> (v);
              return *this;
            }
            BoardInfo &operator= (unsigned long v) {
              value = static_cast<uint32_t> (v);
              return *this;
            }
          };
          BoardInfo _info;
      };

      // -----------------------------------------------------------------------
      //
      //                         System Class
      //
      // -----------------------------------------------------------------------
      System();
      ~System();

      // -----------------------------------------------------------------------
      static bool fileExists (const char *path);
      static inline bool fileExists (const std::string &p) {
        return fileExists (p.c_str());
      }
      static bool charFileExists (const char *path);
      static inline bool charFileExists (const std::string &p) {
        return charFileExists (p.c_str());
      }
      static bool directoryExists (const char *path);
      static inline bool directoryExists (const std::string &p) {
        return fileExists (p.c_str());
      }

      static unsigned long readLinuxRevision();

      static std::string progName();

      void createPidFile (const char *path = nullptr);
      inline void createPidFile (const std::string &path) {
        createPidFile (path.c_str());
      }
      void deletePidFile ();

      void close();

      inline const std::string &hardware() const {
        return _hardware;
      }

      inline unsigned long long serialNumber() const {
        return _sn;
      }

      inline unsigned long revision() const {
        return _revision;
      }

      inline unsigned long totalRam() const {
        return _ram;
      }

      inline const ArmCore &core() const {
        return _core;
      }

      inline const ArmbianInfo &armbianInfo() const {
        return _armbian;
      }

      inline const RaspianInfo &raspianInfo() const {
        return _raspian;
      }
    protected:
      void readTotalRam();
      void readCpuInfo();

    private:
      std::string _hardware;
      unsigned long _revision;
      unsigned long long _sn;
      long _ram; // RAM en Mo
      int _ncore;
      ArmCore _core;
      ArmbianInfo _armbian;
      std::string _pidfn;
      RaspianInfo _raspian;
  };

  // ---------------------------------------------------------------------------
  //
  //                      Piduino System Global Object
  //
  // ---------------------------------------------------------------------------
  extern System system; ///< Piduino System Global Object
  /**
     @}
  */
}
/**
    @}
*/

/* ========================================================================== */
#endif /*PIDUINO_SYSTEM_H defined */
