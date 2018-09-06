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
#include <poll.h>
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
  FileDevice::~FileDevice() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool FileDevice::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (FileDevice);

      d->clearError();
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
  long FileDevice::read (char * buf, long n) {

    if (isOpen()) {
      PIMP_D (FileDevice);
      long ret = ::read (d->fd, buf, n);
      if (ret < 0) {
        d->setError();
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  long FileDevice::write (const char * buf, long n) {

    if (isOpen()) {
      PIMP_D (FileDevice);
      long ret = ::write (d->fd, buf, n);
      if (ret < 0) {

        d->setError();
      }
      return ret;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  long FileDevice::write (const char * str) {

    return write (str, strlen (str));
  }

  // ---------------------------------------------------------------------------
  long FileDevice::write (const std::string & str) {

    return write (str.c_str(), str.size());
  }

  // ---------------------------------------------------------------------------
  std::string FileDevice::read (long n) {
    std::string str;

    if (n < 0) {

      n = bytesAvailable();
    }

    if (n > 0) {
      long len;

      str.resize (n, 0);
      len = read (&str[0], n);

      if (len <= 0)  {

        str.resize (0);
      }
      else if (len != n) {

        str.resize (len);
      }
    }
    return str;
  }

  // ---------------------------------------------------------------------------
  int FileDevice::bytesAvailable() {
    PIMP_D (FileDevice);
    int ret;
    int bytesQueued = 0;

    ret = d->ioctl (FIONREAD, &bytesQueued);

    if (ret < 0) {

      bytesQueued = ret;
    }
    return bytesQueued;
  }

  // ---------------------------------------------------------------------------
  bool
  FileDevice::waitForReadyRead (int msecs) {

    if (bytesAvailable() == 0) {
      PIMP_D (FileDevice);
      bool readyToRead = false;
      bool readyToWrite = false;

      if (d->poll (&readyToRead, &readyToWrite, true, false, msecs) > 0) {

        return readyToRead;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  FileDevice::waitForBytesWritten (int msecs) {

    if (isOpen()) {
      PIMP_D (FileDevice);
      bool readyToRead = false;
      bool readyToWrite = false;

      if (d->poll (&readyToRead, &readyToWrite, false, true, msecs) > 0) {

        return readyToWrite;
      }
    }

    return false;
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

    lock();
    fd = ::open (path.c_str(), additionalPosixFlags);
    if (fd == -1) {
      setError ();
      success = false;
    }
    unlock();
    return success;
  }

  // ---------------------------------------------------------------------------
  void FileDevice::Private::close() {

    unlock();
    if (::close (fd) < 0) {

      setError();
    }
    fd = -1;
  }

  // ---------------------------------------------------------------------------
  int FileDevice::Private::ioctl (int req) {
    int ret;

    lock();
    ret = ::ioctl (fd, req);
    unlock();
    if (ret < 0) {
      setError();
    }
    return ret;
  }

  // ---------------------------------------------------------------------------
  int FileDevice::Private::ioctl (int req, void * arg) {
    int ret;

    lock();
    ret = ::ioctl (fd, req, arg);
    unlock();
    if (ret < 0) {
      setError();
    }
    return ret;
  }

  // ---------------------------------------------------------------------------
  int
  FileDevice::Private::poll (bool *selectForRead, bool *selectForWrite,
                             bool checkRead, bool checkWrite,
                             int msecs) {
    int ret;
    struct pollfd pfd = { fd, 0, 0 };

    if (checkRead) {
      pfd.events |= POLLIN;
    }

    if (checkWrite) {
      pfd.events |= POLLOUT;
    }

    lock();
    ret = ::poll (&pfd, 1, msecs);
    unlock();

    if (ret < 0) {
      setError ();
      return ret;
    }

    if (ret == 0) {
      setError (ETIMEDOUT);
      return ret;
    }

    if (pfd.revents & POLLNVAL) {
      setError (EBADF);
      return -1;
    }

    *selectForWrite = ( (pfd.revents & POLLOUT) != 0);
    *selectForRead  = ( (pfd.revents & POLLIN) != 0);
    return ret;
  }

}

/* ========================================================================== */
