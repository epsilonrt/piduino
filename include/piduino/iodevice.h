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

#ifndef _PIDUINO_IODEVICE_H_
#define _PIDUINO_IODEVICE_H_


/**
 *  @defgroup piduino_iodevice IO Device
 *  @{
 */

namespace Piduino {

  class IoDevice {

    public:

      enum OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Unbuffered = 0x0020
      };

      IoDevice();
      virtual ~IoDevice();

      OpenMode openMode() const;
      virtual bool isOpen() const;
      bool isReadable() const;
      bool isWritable() const;

      virtual bool open (OpenMode mode);
      virtual void close();

    protected:
      void setOpenMode (OpenMode openMode);

    private:
      OpenMode _openMode;
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_IODEVICE_H_ defined */
