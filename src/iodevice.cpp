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
#include <fcntl.h>
#include <cstring>
#include "iodevice_p.h"

namespace Piduino {

  // ---------------------------------------------------------------------------
  IoDevice::Private::Private (IoDevice * q) :
    q_ptr (q), openMode (NotOpen), isSequential (false), error (0) {}

  // ---------------------------------------------------------------------------
  IoDevice::Private::~Private()  {}


  // ---------------------------------------------------------------------------
  void
  IoDevice::Private::setError (int error) {

    error = error;
    errorString.assign (strerror (error));
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::Private::setError (int error, const std::string & str) {

    error = error;
    errorString = str;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::Private::setError () {

    if (errno) {
      error = errno;
      errorString.assign (strerror (errno));
    }
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::Private::clearError () {

    error = 0;
    errorString.clear();
  }

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice (IoDevice::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice () :
    d_ptr (new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  IoDevice::~IoDevice() = default;

  // ---------------------------------------------------------------------------
  void
  IoDevice::setOpenMode (OpenMode m) {

    d_ptr->openMode = m;
  }

  // ---------------------------------------------------------------------------
  IoDevice::OpenMode
  IoDevice::openMode() const {

    return d_ptr->openMode;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::open (OpenMode m) {

    if (!isOpen()) {

      setOpenMode (m);
      clearError();
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::close() {

    if (isOpen()) {

      setOpenMode (NotOpen);
    }
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isOpen() const {

    return openMode() != NotOpen;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isReadable() const {

    return (openMode() & ReadOnly);
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isBuffered() const {

    return ! (openMode() & Unbuffered);
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isWritable() const {

    return (openMode() & WriteOnly);
  }

  // ---------------------------------------------------------------------------
  std::string
  IoDevice::errorString() const {

    return d_ptr->errorString;
  }

  // ---------------------------------------------------------------------------
  int
  IoDevice::error() const {

    return d_ptr->error;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setError (int error, const std::string & str) {

    d_ptr->setError (error, str);
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setError (int error) {

    d_ptr->setError (error);
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setError () {

    d_ptr->setError();
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::clearError () {

    d_ptr->clearError();
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isSequential() const {

    return d_ptr->isSequential;
  }

  // ---------------------------------------------------------------------------
  void IoDevice::setTextModeEnabled (bool enabled) {

    if (enabled) {
      d_ptr->openMode |= Text;
    }
    else {
      d_ptr->openMode &= ~Text;
    }
  }

  // ---------------------------------------------------------------------------
  bool IoDevice::isTextModeEnabled() const {

    return (openMode() & Text);
  }

  // ---------------------------------------------------------------------------
  int IoDevice::systemMode (OpenMode openMode) {
    int m = O_RDONLY;

    openMode &= ReadWrite;
    if (openMode == WriteOnly) {
      m = O_WRONLY;
    }
    else if (openMode == ReadWrite) {
      m = O_RDWR;
    }

    return m;
  }

}

/* ========================================================================== */
