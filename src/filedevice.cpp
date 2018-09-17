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
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/ioctl.h>
#include "filedevice_p.h"

using namespace std;

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                             FileDevice Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  FileDevice::FileDevice (FileDevice::Private &dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  FileDevice::FileDevice () :
    IoDevice (*new Private (this)) {

  }
  
  // ---------------------------------------------------------------------------
  FileDevice::FileDevice (const std::string & path) : FileDevice() {
    setPath (path);
  }

  // ---------------------------------------------------------------------------
  FileDevice::~FileDevice() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool FileDevice::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (FileDevice);

      if (d->open (mode)) {

        return IoDevice::open (mode);
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void FileDevice::close() {

    if (isOpen()) {
      PIMP_D (FileDevice);

      d->close();
      IoDevice::close();
    }
  }

  // ---------------------------------------------------------------------------
  void FileDevice::setPath (const std::string & path) {
    PIMP_D (FileDevice);
    
    if (isOpen()) {
      
      close();
    }

    d->path = path;
  }

  // ---------------------------------------------------------------------------
  std::string FileDevice::path() const {
    PIMP_D (const FileDevice);

    return d->path;
  }

  // ---------------------------------------------------------------------------
  int FileDevice::fd() const {
    PIMP_D (const FileDevice);

    return d->fd;
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t FileDevice::read (char * buf, size_t n) {

    if (openMode() & ReadOnly) {
      PIMP_D (FileDevice);
      ssize_t ret = ::read (d->fd, buf, n);
      if (ret < 0) {
        d->setError();
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t FileDevice::write (const char * buf, size_t n) {

    if (openMode() & WriteOnly) {
      PIMP_D (FileDevice);
      ssize_t ret = ::write (d->fd, buf, n);
      if (ret < 0) {

        d->setError();
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t FileDevice::write (const char * str) {

    return write (str, strlen (str));
  }

  // ---------------------------------------------------------------------------
  ssize_t FileDevice::write (const std::string & str) {

    return write (str.c_str(), str.size());
  }

// -----------------------------------------------------------------------------
//
//                         FileDevice::Private Class
//
// -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  FileDevice::Private::Private (FileDevice * q) :
    IoDevice::Private (q), fd (-1) {}

  // ---------------------------------------------------------------------------
  FileDevice::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  bool FileDevice::Private::open (OpenMode mode, int additionalPosixFlags) {
    PIMP_Q (FileDevice);
    bool success = true;

    additionalPosixFlags &= ~static_cast<int> (O_ACCMODE | O_APPEND | O_TRUNC);
    additionalPosixFlags |= modeToPosixFlags (mode);

    fd = ::open (path.c_str(), additionalPosixFlags);
    if (fd == -1) {
      setError ();
      success = false;
    }

    return success;
  }

  // ---------------------------------------------------------------------------
  void FileDevice::Private::close() {

    if (::close(fd) != 0) {

      setError();
    }
    fd = -1;
  }

  // ---------------------------------------------------------------------------
  int FileDevice::Private::ioctl (int req) {
    int ret;

    ret = ::ioctl (fd, req);
    if (ret < 0) {
      setError();
    }
    return ret;
  }

  // ---------------------------------------------------------------------------
  int FileDevice::Private::ioctl (int req, void * arg) {
    int ret;

    ret = ::ioctl (fd, req, arg);
    if (ret < 0) {
      setError();
    }
    return ret;
  }
}

/* ========================================================================== */
