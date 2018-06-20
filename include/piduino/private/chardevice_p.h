/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * CharDevice is a modified and simplified version of QIODevice, 
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

#ifndef PIDUINO_CHARDEVICE_PRIVATE_H
#define PIDUINO_CHARDEVICE_PRIVATE_H

#include <algorithm>
#include <cstring>
#include <piduino/chardevice.h>
#include "iodevice_p.h"

#ifndef CHARDEVICE_BUFFERSIZE
#define CHARDEVICE_BUFFERSIZE 16384
#endif

namespace Piduino {

  class CharDevice::Private : public IoDevice::Private {

    public:

      class LinearBuffer {

        public:
          //--------------------------------------------------------------------
          LinearBuffer() : _capacity (0), _pwrite (0), _pread (0), _buf (0) {}
          //--------------------------------------------------------------------
          ~LinearBuffer() {
            delete [] _buf;
          }
          //--------------------------------------------------------------------
          void clear() {
            _pread = _pwrite = 0;
          }
          //--------------------------------------------------------------------
          int size() const {
            return (_pwrite - _pread);
          }
          //--------------------------------------------------------------------
          bool empty() const {
            return size() == 0;
          }
          //--------------------------------------------------------------------
          int getChar() {
            if (size() == 0) {
              return -1;
            }
            int i = static_cast<unsigned char> (*_pread);
            _pread++;
            return i;
          }
          //--------------------------------------------------------------------
          int peek (char * target, int maxlen) {
            int r = std::min (maxlen, size());
            memcpy (target, _pread, r);
            return r;
          }
          //--------------------------------------------------------------------
          int read (char * target, int maxlen) {
            int r = peek (target, maxlen);
            _pread += r;
            return r;
          }
          //--------------------------------------------------------------------
          std::vector<char> readAll() {
            std::vector<char> result;
            
            result.resize (size());
            result.assign (_pread, _pread + size());
            clear();
            return result;
          }
          //--------------------------------------------------------------------
          bool canReadLine() const {
            return memchr (_pread, '\n', size()) != nullptr;
          }
          //--------------------------------------------------------------------
          int readLine (char * target, int maxlen) {
            int r = std::min (maxlen, size());
            char * eol = static_cast<char *> (memchr (_pread, '\n', r));
            if (eol) {
              r = (eol - _pread) + 1;
            }
            return read (target, r);
          }
          //--------------------------------------------------------------------
          void skip (int n) {
            if (n >= size()) {
              clear();
            }
            else {
              _pread += n;
            }
          }
          //--------------------------------------------------------------------
          void chop (int n) {
            if (n >= size()) {
              clear();
            }
            else {
              _pwrite -= n;
            }
          }
          //--------------------------------------------------------------------
          void ungetChar (char c) {
            if (_pread == _buf) {
              resize (size() + 1);
            }
            _pread--;
            *_pread = c;
          }
          //--------------------------------------------------------------------
          void ungetBlock (const char * block, int len) {
            if ( (_pread - _buf) < len) {
              resize (size() + len);
            }
            _pread -= len;
            memcpy (_pread, block, len);
          }
          //--------------------------------------------------------------------
          /* Ajoute un bloc de len bytes à la fin du buffer
           * déplace le pointeur d'écriture après ce bloc
           * retourne le pointeur d'écriture avant ajout
           */
          char * reserve (int len) {
            char * p;

            resize (len + size(), true);
            p = _pwrite;
            _pwrite += len;
            return p;
          }

        protected:
          //--------------------------------------------------------------------
          void resize (size_t required, bool atEnd = false) {
            size_t newsize = std::max (_capacity, static_cast<size_t> (CHARDEVICE_BUFFERSIZE));

            while (newsize < required) {
              newsize *= 2;
            }

            int offset = atEnd ? 0 : newsize - size();
            if (newsize > _capacity) {
              char * newbuf = new char[newsize];
              memmove (newbuf + offset, _pread, size());
              delete [] _buf;
              _buf = newbuf;
              _capacity = newsize;
            }
            else {
              memmove (_buf + offset, _pread, size());
            }
            _pread = _buf + offset;
          }
        private:
          size_t _capacity;
          char * _pread;
          char * _pwrite;
          char * _buf;
      };

      explicit Private (bool isSequential);
      virtual ~Private();

      inline void incrementPos (long offset) {
        pos += isSequential ? 0 : offset;
      }

      inline void incrementDevicePos (long offset) {
        devicePos += isSequential ? 0 : offset;
      }

      LinearBuffer buffer;
      bool firstRead;
      long pos;
      long devicePos;
      bool baseReadLineDataCalled;
  };
}
/* ========================================================================== */
#endif /* PIDUINO_CHARDEVICE_PRIVATE_H defined */
