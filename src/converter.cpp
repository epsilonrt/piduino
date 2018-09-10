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
#include <piduino/converter.h>

namespace Piduino {

  // ---------------------------------------------------------------------------
  Converter::Converter (Converter::Type t, unsigned int r, bool b) :
    IoDevice(), _type (t), _resolution (r), _bipolar (b)  {

  }

  // ---------------------------------------------------------------------------
  Converter::~Converter() {

  }
  
  // ---------------------------------------------------------------------------
  bool
  Converter::open (OpenMode mode) {
    
    return IoDevice::open (mode);
  }

  // ---------------------------------------------------------------------------
  Converter::Type
  Converter::type() const {

    return _type;
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::bipolar() const {

    return _bipolar;
  }

  // ---------------------------------------------------------------------------
  unsigned int
  Converter::resolution() const {

    return _resolution;
  }

  // ---------------------------------------------------------------------------
  void 
  Converter::setResolution (unsigned int r) {
    
    _resolution = r;

  }
  // ---------------------------------------------------------------------------
  void 
  Converter::setBipolar (bool enable) {
    
    _bipolar = enable;
  }

  // ---------------------------------------------------------------------------
  long
  Converter::max() const {

    return (1L << (resolution() - (bipolar() ? 1 : 0))) - 1;
  }

  // ---------------------------------------------------------------------------
  long
  Converter::min() const {

    return bipolar() ? - (1L << (resolution() - 1)) : 0;
  }
}

/* ========================================================================== */
