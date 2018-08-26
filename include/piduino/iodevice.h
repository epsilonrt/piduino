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

#ifndef PIDUINO_IODEVICE_H
#define PIDUINO_IODEVICE_H

#include <piduino/memory.h>
#include <piduino/flags.h>
#include <piduino/global.h>
#include <string>

/**
 *  @defgroup piduino_iodevice IO Device
 *  @{
 */

namespace Piduino {

  class IoDevice {

    public:

      enum class OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Unbuffered = 0x0020
      };

      IoDevice ();
      virtual ~IoDevice();

      OpenMode openMode() const;
      virtual bool isOpen() const;
      bool isReadable() const;
      bool isWritable() const;
      bool isBuffered() const;
      /**
       * Returns true if this device is sequential; otherwise returns false.
       *
       * Sequential devices, as opposed to a random-access devices, have no
       * concept of a start, an end, a size, or a current position, and they
       * do not support seeking. You can only read from the device when it
       * reports that data is available. The most common example of a sequential
       * device is a network socket. On Unix, special files such as /dev/zero
       * and fifo pipes are sequential.
       *
       * Regular files, on the other hand, do support random access. They have
       * both a size and a current position, and they also support seeking
       * backwards and forwards in the data stream. Regular files are non-sequential.
       *
       * The IoDevice implementation returns false.
       */
      virtual bool isSequential() const;

      void setTextModeEnabled (bool enabled);
      bool isTextModeEnabled() const;

      virtual bool open (OpenMode mode);
      virtual void close();
      std::string errorString() const;
      int error() const;

    protected:
      class Private;
      IoDevice (Private &dd);
      Private * d_ptr;

      void setOpenMode (OpenMode openMode);
      int systemMode (OpenMode openMode);
      void setSequential (bool enable);
      void setError ();
      void setError (int error);
      void setError (int error, const std::string &errorString);
      void clearError();

    private:
      PIMP_DECLARE_PRIVATE (IoDevice)
  };
  ENABLE_FLAGS_OPERATORS (IoDevice::OpenMode);
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_IODEVICE_H defined */
