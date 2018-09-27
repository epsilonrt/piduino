/* Copyright © 2015 Pascal JEAN, All rights reserved.
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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <piduino/iomap.h>
#include <exception>
#include <system_error>
#include "config.h"

namespace Piduino {

  const char IomapShm[] = "/dev/mem";

  // ---------------------------------------------------------------------------
  IoMap::IoMap () : _base (0), _size (0), _fd (-1), _map (nullptr) {

  }

  // ---------------------------------------------------------------------------
  IoMap::~IoMap() {

    this->close();
  }

  // ---------------------------------------------------------------------------
  bool
  IoMap::open (off_t b, size_t s) {

    if ( (b != base()) && (s != size())) {

      this->close();

      if ( (_fd = ::open (IomapShm, O_RDWR | O_SYNC | O_CLOEXEC)) < 0) {  // TODO: NO POSIX COMPLIANT

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }

      _map = mmap (
               NULL,
               s,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _fd,
               b
             );

      if (_map == MAP_FAILED) {

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }

      _base = b;
      _size = s;
    }

    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  IoMap::close () {

    if (isOpen()) {
      
      if (munmap (_map, _size) != 0) {

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }

      if (::close (_fd) != 0) {  // TODO: NO POSIX COMPLIANT

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }

      _fd = -1;
      _base = 0;
      _size = 0;
      _map = nullptr;
    }
  }

  // ---------------------------------------------------------------------------
  volatile uint32_t *
  IoMap::io (size_t offset) const {

    return static_cast<volatile uint32_t *> (_map) + offset;
  }
}
/* ========================================================================== */
