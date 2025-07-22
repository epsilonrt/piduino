/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#include <dirent.h>
#include <fstream>
#include <sstream>
#include "rp1.h"
#include "config.h"

namespace Piduino {

  namespace Rp1 {

    // -------------------------------------------------------------------------
    bool isPCIeFileContain (const char *dirname, const char *filename, const char *content) {
      std::ostringstream fpath;

      fpath << PCIeDevPath << "/" << dirname << "/" << filename;
      std::ifstream f (fpath.str());

      if (f.is_open()) {
        std::string line;

        while (std::getline (f, line)) {

          if (line.find (content) != std::string::npos) {

            return true;
          }
        }
        f.close();
      }
      return false;
    }

    // -------------------------------------------------------------------------
    std::string findPCIeDevice() {
      DIR *dir;
      std::ostringstream res;

      dir = opendir (PCIeDevPath);

      if (dir != NULL) {
        struct dirent *entry;

        while ( (entry = readdir (dir)) != NULL) {

          if (entry->d_type == DT_LNK) {

            if (isPCIeFileContain (entry->d_name, "device", PCIeRp1DeviceId) &&
                isPCIeFileContain (entry->d_name, "vendor", PCIeRp1VendorId)) {

              res.clear();
              res << PCIeDevPath << "/" << entry->d_name << "/" << PCIeRp1File;
              break;
            }
          }
        }

        closedir (dir);
      }

      return res.str();
    }
  }
}