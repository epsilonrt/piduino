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
  FileDevice::FileDevice (int fd) : FileDevice() {
    setFd (fd);
  }

  // ---------------------------------------------------------------------------
  FileDevice::~FileDevice() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool FileDevice::open (OpenMode mode) {

    if (!isOpen() && isOurFile()) {
      PIMP_D (FileDevice);

      if (d->open (mode)) {

        return IoDevice::open (mode);
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void FileDevice::close() {

    if (isOpen() && isOurFile()) {
      PIMP_D (FileDevice);

      d->close();
      IoDevice::close();
    }
  }

  // ---------------------------------------------------------------------------
  void FileDevice::setPath (const std::string & path) {
    PIMP_D (FileDevice);

    close();
    d->path = path;
    d->ourFile = true;
    d->fd = -1;
  }

  // ---------------------------------------------------------------------------
  std::string FileDevice::path() const {
    PIMP_D (const FileDevice);

    return d->path;
  }

  // ---------------------------------------------------------------------------
  bool FileDevice::setFd (int fdesc) {

    if (fdesc != fd()) {

      int flags = fcntl (fdesc, F_GETFL, 0);

      if (flags >= 0) {
        PIMP_D (FileDevice);
        OpenMode m = NotOpen;

        close();
        switch (flags & O_ACCMODE) {
          case O_RDONLY:
            m = ReadOnly;
            break;
          case O_WRONLY:
            m = WriteOnly;
            break;
          case O_RDWR:
            m = ReadWrite;
            break;
        }

        if (m != NotOpen) {

          if (flags & O_APPEND) {

            m |= Append;
          }
          d->fd = fdesc;
          d->ourFile = false;
          d->path.clear();
          return IoDevice::open (m);
        }
      }
    }
    return isOpen();
  }


  // ---------------------------------------------------------------------------
  int FileDevice::fd() const {
    PIMP_D (const FileDevice);

    return d->fd;
  }

  // ---------------------------------------------------------------------------
  bool FileDevice::isOurFile() const {
    PIMP_D (const FileDevice);

    return d->ourFile;
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t FileDevice::read (char * buf, size_t n) {

    if (openMode() & ReadOnly) {
      PIMP_D (FileDevice);
      ssize_t len = ::read (d->fd, buf, n);

      if (len < 0) {
        d->setError();
      }
      else if (! (openMode() & IoDevice::Binary)) {
        /*
         * Text mode
         * When reading, the end-of-line terminators are translated to '\n'.
         */
        for (size_t i = 0; i < len; i++) {

          if (buf[i] == '\r') {

            buf[i] = '\n'; // change \r to \n
            if (i < (len - 1)) {
              if (buf[i + 1] == '\n') { // shift left all chars to erase \n

                for (size_t j = i + 1; j < (len - 1); j++) {

                  buf[j] = buf[j + 1];
                }
                buf[len - 1] = 0;
                len --;
              }
            }
          }
        }
      }
      return len;
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t FileDevice::write (const char * buf, size_t n) {

    if (openMode() & WriteOnly) {
      PIMP_D (FileDevice);
      ssize_t len = ::write (d->fd, buf, n);
      if (len < 0) {

        d->setError();
      }
      return len;
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
    IoDevice::Private (q), fd (-1), ourFile (true) {}

  // ---------------------------------------------------------------------------
  FileDevice::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  bool FileDevice::Private::open (OpenMode m, int additionalPosixFlags) {
    PIMP_Q (FileDevice);

    additionalPosixFlags &= ~static_cast<int> (O_ACCMODE | O_APPEND | O_TRUNC);
    additionalPosixFlags |= modeToPosixFlags (m);

    fd = ::open (path.c_str(), additionalPosixFlags);
    if (fd == -1) {
      setError ();
      return false;
    }

    return IoDevice::Private::open(m);
  }

  // ---------------------------------------------------------------------------
  void FileDevice::Private::close() {

    if (::close (fd) != 0) {

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
