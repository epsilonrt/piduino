/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#pragma once

#include <piduino/memory.h>
#include <piduino/flags.h>
#include <piduino/global.h>
#include <string>
#include <ios>

/**
 *  @defgroup piduino_iodevice IO Device
 *  @{
 */

namespace Piduino {

  class IoDevice {

    public:

      enum OpenModeFlag {
        NotOpen = 0x0000,
        Append = std::ios_base::app, ///< Set the stream's position indicator to the end of the stream before each output operation.
        AtEnd = std::ios_base::ate, ///< Set the stream's position indicator to the end of the stream on opening.
        Binary = std::ios_base::binary, ///< Consider stream as binary rather than text. 
        /*
         * Text mode
         * When reading, the end-of-line terminators are translated to '\n'. 
         * When writing, the end-of-line terminators are translated to the 
         * local encoding, for example '\r\n' for Win32.
         */
        ReadOnly = std::ios_base::in, ///< Allow input operations
        WriteOnly = std::ios_base::out, ///< Allow output operations 
        ReadWrite = ReadOnly | WriteOnly, ///< Allow input and output operations
        Truncate = std::ios_base::trunc, ///< Any current content is discarded, assuming a length of zero on opening.
        IosModes = Append | AtEnd | Binary | ReadOnly | WriteOnly | Truncate,
        Unbuffered = (Truncate << 1)
      };
      typedef Flags<OpenModeFlag> OpenMode;

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
      void setDebug (bool enabled);
      bool isDebug() const;

      virtual bool open (OpenMode mode);
      virtual void close();
      virtual std::string errorString() const;
      virtual int error() const;

    protected:
      class Private;
      IoDevice (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (IoDevice)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
