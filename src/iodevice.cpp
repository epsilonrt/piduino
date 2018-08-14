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
#include <cstring>
#include <piduino/private/iodevice_p.h>

namespace Piduino {

  // ---------------------------------------------------------------------------
  IoDevice::Private::Private (IoDevice * q) :
    q_ptr (q), openMode (OpenMode::NotOpen), isSequential (false), error (0) {}

  // ---------------------------------------------------------------------------
  IoDevice::Private::~Private()  {}

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice (IoDevice::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice () :
    d_ptr (new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  IoDevice::~IoDevice() {

    delete d_ptr;
  }

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

      setOpenMode (OpenMode::NotOpen);
    }
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isOpen() const {

    return openMode() != OpenMode::NotOpen;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isReadable() const {

    return (openMode() & OpenMode::ReadOnly) == OpenMode::ReadOnly;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isBuffered() const {

    return (openMode() & OpenMode::Unbuffered) != OpenMode::Unbuffered;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isWritable() const {

    return (openMode() & OpenMode::WriteOnly) == OpenMode::WriteOnly;
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
  IoDevice::setError (int error) {

    d_ptr->error = error;
    d_ptr->errorString.assign (strerror (error));
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setError () {

    if (errno) {
      d_ptr->error = errno;
      d_ptr->errorString.assign (strerror (errno));
    }
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::clearError () {

    d_ptr->error = 0;
    d_ptr->errorString.clear();
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isSequential() const {

    return d_ptr->isSequential;
  }

  // ---------------------------------------------------------------------------
  void IoDevice::setTextModeEnabled (bool enabled) {

    if (enabled) {
      d_ptr->openMode |= OpenMode::Text;
    }
    else {
      d_ptr->openMode &= ~OpenMode::Text;
    }
  }

  // ---------------------------------------------------------------------------
  bool IoDevice::isTextModeEnabled() const {

    return (openMode() & OpenMode::Text) == OpenMode::Text;
  }
}

/* ========================================================================== */
