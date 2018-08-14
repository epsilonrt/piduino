/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * I2cDev is a modified and simplified version of QIODevice,
 * from Qt according to the LGPL and Copyright (C) 2015 The Qt Company Ltd.
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

#ifndef PIDUINO_FIFO_H
#define PIDUINO_FIFO_H

#include <cstring>
#include <algorithm>

namespace Piduino {

  class Fifo {
    public:

      Fifo (uint16_t max) : _data (new uint8_t[max]), _size (max), _in (_data), _out (_data) {}
      ~Fifo() {

        delete[] _data;
      }
      void clear() {

        _in = _out = _data;
      }
      uint16_t length() const {

        return (_in - _out) % _size;
      }
      uint16_t size() const {

        return _size;
      }
      uint8_t * data() const {

        return _data;
      }
      uint8_t * in() const {

        return _in;
      }
      uint8_t * out() const {

        return _out;
      }
      uint16_t pull (uint8_t * buf, uint16_t max) {

        max = std::min (max, length());
        ::memcpy (buf, _out, max);
        _out += max;
        return max;
      }
      uint16_t push (const uint8_t * buf, uint16_t len) {

        len = std::min (len, _size);
        ::memcpy (_in, buf, len);
        _in += len;
        return len;
      }
      void seek (uint16_t len) {
        
        len = std::min (len, static_cast<uint16_t>(_size - static_cast<uint16_t>(_out - _data)));
        _in += len;
      }
    
    protected:
      uint8_t * _data;
      uint16_t _size;
      uint8_t * _in;
      uint8_t * _out;
  };

}
/* ========================================================================== */
#endif /* PIDUINO_FIFO_H defined */
