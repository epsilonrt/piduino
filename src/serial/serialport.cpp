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
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <piduino/serialport.h>
#include <piduino/database.h>

namespace Piduino {
  
  // ---------------------------------------------------------------------------
  std::vector<SerialPort::Info>
  SerialPort::availablePorts () {
    std::vector<SerialPort::Info> ports;

    for (int id = 0; id < db.board().soc().serialCount(); id++) {
      char path[256];
      int fd;

      ::snprintf (path, sizeof (path), db.board().soc().serialSysPath().c_str(), id);
      fd = ::open (path, O_RDWR);
      
      if (fd >= 0) {
        
        if (::read (fd, path, 0) >= 0) {
          
          Info port;
          port.id = id;
          port.path = path;
          ports.push_back (port);
        }
        ::close (fd);
      }
    }
    
    return ports;
  }
}

/* ========================================================================== */
