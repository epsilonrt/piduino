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
#include <climits>
#include <piduino/converter.h>
#include "converter_p.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                             Converter Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Converter::Converter (Converter::Private &dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  Converter::Converter () :
    IoDevice (*new Private (this)) {

  }

  // ---------------------------------------------------------------------------
  Converter::~Converter() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool Converter::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (Converter);

      return d->open (mode);
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void Converter::close() {

    if (isOpen()) {
      PIMP_D (Converter);

      d->close();
    }
  }

  // ---------------------------------------------------------------------------
  // virtual
  long Converter::read () {

    if (openMode() & ReadOnly) {
      PIMP_D (Converter);

      return d->read();
    }
    return LONG_MIN;
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool Converter::write (long value) {

    if (openMode() & WriteOnly) {
      PIMP_D (Converter);

      if (value < min()) {

        value = min();
      }
      if (value > max()) {

        value = max();
      }
      return d->write (value);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  Converter::Type
  Converter::type() const {
    PIMP_D (const Converter);

    return d->type;
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::bipolar() const {
    PIMP_D (const Converter);

    return d->bipolar;
  }

  // ---------------------------------------------------------------------------
  unsigned int
  Converter::resolution() const {
    PIMP_D (const Converter);

    return d->resolution;
  }

  // ---------------------------------------------------------------------------
  long
  Converter::max() const {
    PIMP_D (const Converter);

    return d->max();
  }

  // ---------------------------------------------------------------------------
  long
  Converter::min() const {
    PIMP_D (const Converter);

    return d->min();
  }

  // ---------------------------------------------------------------------------
  // static
  const std::string & Converter::deviceName() {
    static std::string dn;

    return dn;
  }

// -----------------------------------------------------------------------------
//
//                         Converter::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Converter::Private::Private (Converter * q, Type t, unsigned int r, bool b) :
    IoDevice::Private (q), type (t), resolution (r), bipolar (b) {}

  // ---------------------------------------------------------------------------
  Converter::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // virtual
  bool Converter::Private::open (OpenMode m) {

    return IoDevice::Private::open (m);
  }

  // ---------------------------------------------------------------------------
  // virtual
  void Converter::Private::close() {

    IoDevice::Private::close();
  }

  // ---------------------------------------------------------------------------
  // virtual
  long Converter::Private::read() {

    return LONG_MIN;
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool Converter::Private::write (long value) {

    return false;
  }

  // ---------------------------------------------------------------------------
  // virtual
  long
  Converter::Private::max() const {

    return (1L << (resolution - (bipolar ? 1 : 0))) - 1;
  }

  // ---------------------------------------------------------------------------
  // virtual
  long
  Converter::Private::min() const {

    return bipolar ? - (1L << (resolution - 1)) : 0;
  }

}

/* ========================================================================== */
