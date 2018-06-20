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
#include <piduino/private/iodevice_p.h>

namespace Piduino {

  // ---------------------------------------------------------------------------
  IoDevice::Private::Private (bool _isSequential) :
    openMode (OpenMode::NotOpen), isSequential (_isSequential) {}

  // ---------------------------------------------------------------------------
  IoDevice::Private::~Private()  {}

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice (IoDevice::Private &dd) : d (&dd) {

  }

  // ---------------------------------------------------------------------------
  IoDevice::IoDevice (bool isSequential) :
    d (std::make_unique<Private> (isSequential))  {

  }

  // ---------------------------------------------------------------------------
  IoDevice::~IoDevice() {

  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setOpenMode (OpenMode m) {

    d->openMode = m;
  }

  // ---------------------------------------------------------------------------
  IoDevice::OpenMode
  IoDevice::openMode() const {

    return d->openMode;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::open (OpenMode m) {

    if (!isOpen()) {
      setOpenMode (m);
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::close() {

    if (isOpen()) {
      d->errorString.clear();
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

    return d->errorString;
  }

  // ---------------------------------------------------------------------------
  void
  IoDevice::setErrorString (const std::string &errorString) {

    d->errorString = errorString;
  }

  // ---------------------------------------------------------------------------
  bool
  IoDevice::isSequential() const {

    return d->isSequential;
  }

  // ---------------------------------------------------------------------------
  void IoDevice::setTextModeEnabled (bool enabled) {

    if (enabled) {
      d->openMode |= OpenMode::Text;
    }
    else {
      d->openMode &= ~OpenMode::Text;
    }
  }

  // ---------------------------------------------------------------------------
  bool IoDevice::isTextModeEnabled() const {

    return (openMode() & OpenMode::Text) == OpenMode::Text;
  }
}

/* ========================================================================== */
