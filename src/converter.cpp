/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <piduino/converter.h>
#include "converter_p.h"
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                             Converter Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Converter::Converter (Converter::Private &dd) : IoDevice (dd) {}

  // ---------------------------------------------------------------------------
  Converter::Converter () :
    IoDevice (*new Private (this, None, 0)) {}

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
        if (d->isDebug) {
          std::cerr << "Converter::write(" << value << ") below minimum, setting to " << min() << std::endl;
        }
      }
      if (value > max()) {

        value = max();
        if (d->isDebug) {
          std::cerr << "Converter::write(" << value << ") above maximum, setting to " << max() << std::endl;
        }
      }
      return d->write (value);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  const std::string &Converter::deviceName() const {
    PIMP_D (const Converter);

    return d->deviceName();
  }

  // ---------------------------------------------------------------------------
  Converter::Type
  Converter::type() const {
    PIMP_D (const Converter);

    return d->type;
  }

  // ---------------------------------------------------------------------------
  unsigned int
  Converter::flags() const {
    PIMP_D (const Converter);

    return d->flags;
  }

  // ---------------------------------------------------------------------------
  void Converter::setEnable (bool enable) {
    PIMP_D (Converter);

    d->setEnable (enable);
  }

  // ---------------------------------------------------------------------------
  bool Converter::isEnabled () const {
    PIMP_D (const Converter);

    return d->isEnabled();
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

  // ------------------------------- Optional API ------------------------------

  // ---------------------------------------------------------------------------
  int
  Converter::resolution() const {
    PIMP_D (const Converter);

    return d->resolution();
  }

  // ---------------------------------------------------------------------------
  int Converter::setResolution (int resolution) {
    PIMP_D (Converter);

    return d->setResolution (resolution);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::bipolar() const {
    PIMP_D (const Converter);

    return d->bipolar();
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::setBipolar (bool bipolar) {
    PIMP_D (Converter);

    return d->setBipolar (bipolar);
  }

  // ---------------------------------------------------------------------------
  long
  Converter::range() const {
    PIMP_D (const Converter);

    return d->range();
  }

  // ---------------------------------------------------------------------------
  long
  Converter::setRange (long range) {
    PIMP_D (Converter);

    return d->setRange (range);
  }

  // ---------------------------------------------------------------------------
  bool Converter::setReference (int reference) {
    PIMP_D (Converter);

    return d->setReference (reference);
  }

  // ---------------------------------------------------------------------------
  int Converter::reference() const {
    PIMP_D (const Converter);

    return d->reference();
  }

  // ---------------------------------------------------------------------------
  long Converter::frequency() const {
    PIMP_D (const Converter);

    return d->frequency();
  }

  // ---------------------------------------------------------------------------
  long Converter::setFrequency (long freq) {
    PIMP_D (Converter);

    return d->setFrequency (freq);
  }

  // ---------------------------------------------------------------------------
  //
  //                     Converter::Private Class
  //
  // ---------------------------------------------------------------------------
  
  // ---------------------------------------------------------------------------
  Converter::Private::Private (Converter *q, Type type, unsigned int flags) :
    IoDevice::Private (q), type (type), flags (flags) {}

  // ---------------------------------------------------------------------------
  Converter::Private::~Private() = default;
} // namespace Piduino
/* ========================================================================== */
