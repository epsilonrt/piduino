/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#pragma once

#include <ext/stdio_filebuf.h>
#include <piduino/filestream.h>
#include "filedevice_p.h"

namespace Piduino {

  /**
   * @class FileStream::Private
   * @brief
   */
  class FileStream::Private  : public FileDevice::Private {

    public:
      Private (FileStream * q);

      virtual bool open (OpenMode mode, int additionalPosixFlags = 0);
      virtual void close();

      __gnu_cxx::stdio_filebuf<char> iosbuf;

      PIMP_DECLARE_PUBLIC (FileStream)
  };
}

/* ========================================================================== */
