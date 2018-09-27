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
#include "terminal_p.h"
#include "config.h"

using namespace std;

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                             Terminal Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Terminal::Terminal (Terminal::Private &dd) : FileStream (dd) {

  }

  // ---------------------------------------------------------------------------
  Terminal::Terminal () :
    FileStream (*new Private (this)) {

  }

  // ---------------------------------------------------------------------------
  Terminal::Terminal (const std::string & path) : Terminal() {
    setPath (path);
  }

  // ---------------------------------------------------------------------------
  Piduino::TerminalNotifier & Terminal::notifier() {
    PIMP_D (Terminal);

    return d->notifier;
  }

  // ---------------------------------------------------------------------------
  const Piduino::TerminalNotifier & Terminal::notifier() const {
    PIMP_D (const Terminal);

    return d->notifier;
  }

  // ---------------------------------------------------------------------------
  ssize_t Terminal::available() const {
    if (openMode() & ReadOnly) {

      return notifier().available();
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  // virtual
  ssize_t Terminal::read (char * buf, size_t len) {
    if (openMode() & ReadOnly) {

      return notifier().read (buf, len);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t Terminal::read (char * buf, size_t len, long msTimeout) {
    if (openMode() & ReadOnly) {

      return notifier().read (buf, len, msTimeout);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t Terminal::read (std::string & str, long msTimeout) {
    if (openMode() & ReadOnly) {

      return notifier().read (str, msTimeout);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t Terminal::read (char & c, long msTimeout) {
    if (openMode() & ReadOnly) {

      return (notifier().read (c, msTimeout) ? 1 : 0);
    }
    return -1;
  }


  // ---------------------------------------------------------------------------
  ssize_t Terminal::peek (char * buf, size_t len, long msTimeout) {
    if (openMode() & ReadOnly) {

      return notifier().peek (buf, len, msTimeout);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t Terminal::peek (std::string & str, long msTimeout) {
    if (openMode() & ReadOnly) {

      return notifier().peek (str, msTimeout);
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  ssize_t Terminal::peek (char & c, long msTimeout) {
    if (openMode() & ReadOnly) {

      return (notifier().peek (c, msTimeout) ? 1 : 0);
    }
    return -1;
  }

// -----------------------------------------------------------------------------
//
//                         Terminal::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Terminal::Private::Private (Terminal * q) :
    FileStream::Private (q), notifier(q) {
      
    isSequential = true;
  }

  // ---------------------------------------------------------------------------
  bool Terminal::Private::open (OpenMode mode, int additionalPosixFlags) {

    if (FileStream::Private::open (mode, additionalPosixFlags)) {

      if (notifier.start ()) {

        return true;
      }
      FileStream::Private::close();
    }

    return false;
  }

  // ---------------------------------------------------------------------------
  void Terminal::Private::close() {

    notifier.terminate();
    FileStream::Private::close();
  }
}

/* ========================================================================== */
