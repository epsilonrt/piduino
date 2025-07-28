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
#include <cstring>
#include "filestream_p.h"
#include "config.h"

using namespace std;

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                             FileStream Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  FileStream::FileStream (FileStream::Private &dd) :
    FileDevice (dd), std::iostream (&dd.iosbuf) {

  }

  // ---------------------------------------------------------------------------
  FileStream::FileStream () :
    FileDevice (*new Private (this)), std::iostream (& (d_func()->iosbuf))  {

  }

  // ---------------------------------------------------------------------------
  FileStream::FileStream (const std::string & path) : FileStream() {
    setPath (path);
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t FileStream::write (const char * data, size_t max) {

    if (openMode() & WriteOnly) {

      iostream::write (data, max);
      if (iostream::good()) {

        return max;
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t FileStream::write (const char * str) {

    return write (str, strlen (str));
  }

  // ---------------------------------------------------------------------------
  ssize_t FileStream::write (const std::string & str) {

    return write (str.c_str(), str.size());
  }

  // ---------------------------------------------------------------------------
  ssize_t FileStream::write (char c) {

    return write (&c, 1);
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t FileStream::read (char * data, size_t max) {

    if (openMode() & ReadOnly) {

      iostream::read (data, max);
      if (iostream::good() || iostream::eof()) {
        return iostream::gcount();
      }
    }
    return -1;
  }

// -----------------------------------------------------------------------------
//
//                         FileStream::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  FileStream::Private::Private (FileStream * q) :
    FileDevice::Private (q) {

  }

  // ---------------------------------------------------------------------------
  bool FileStream::Private::open (OpenMode mode, int additionalPosixFlags) {

    if (FileDevice::Private::open (mode, additionalPosixFlags)) {

      ios::openmode iosmode = static_cast<ios::openmode> (mode.value() & IosModes);
      __gnu_cxx::stdio_filebuf<char> buf (fd, iosmode);
      iosbuf.swap (buf);
      return true;
    }

    return false;
  }

  // ---------------------------------------------------------------------------
  void FileStream::Private::close() {

    iosbuf.close(); // also closes the file descriptor
    fd = -1;
  }
}

/* ========================================================================== */
