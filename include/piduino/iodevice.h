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
#pragma once

#include <piduino/memory.h>
#include <piduino/flags.h>
#include <piduino/global.h>
#include <string>
#include <ios>

namespace Piduino {

  /**
     @class IoDevice
     @brief Abstract base class for input/output devices.

     The IoDevice class provides a generic interface for devices that support input and/or output operations.
     It defines common functionality for file-like objects, such as files, sockets, or pipes, and provides
     methods to query and control the device's state and mode of operation.

     Derived classes should implement the pure virtual methods to provide device-specific behavior.
  */
  class IoDevice {

    public:

      /**
         @enum OpenModeFlag
         @brief Flags that specify how the device is to be opened.

         These flags control the behavior of the device when it is opened.
         They can be combined using bitwise OR.
      */
      enum OpenModeFlag {
        NotOpen = 0x0000, ///< Device is not open.
        Append = std::ios_base::app, ///< Set the stream's position indicator to the end of the stream before each output operation.
        AtEnd = std::ios_base::ate, ///< Set the stream's position indicator to the end of the stream on opening.
        Binary = std::ios_base::binary, ///< Open the device in binary mode (no text translation).
        /**
           Text mode:
           When reading, end-of-line terminators are translated to '\n'.
           When writing, end-of-line terminators are translated to the local encoding (e.g., '\r\n' for Win32).
        */
        ReadOnly = std::ios_base::in, ///< Open the device for input operations only.
        WriteOnly = std::ios_base::out, ///< Open the device for output operations only.
        ReadWrite = ReadOnly | WriteOnly, ///< Open the device for both input and output operations.
        Truncate = std::ios_base::trunc, ///< Discard any existing content when opening the device.
        IosModes = Append | AtEnd | Binary | ReadOnly | WriteOnly | Truncate, ///< Combination of all standard I/O modes.
        Unbuffered = (Truncate << 1) ///< Open the device in unbuffered mode (implementation-defined).
      };

      /**
         @typedef OpenMode
         @brief Type representing a combination of OpenModeFlag values.
      */
      typedef Flags<OpenModeFlag> OpenMode;
      
      /**
         @brief Constructs a new IoDevice object.
      */
      IoDevice ();

      /**
         @brief Destroys the IoDevice object.
      */
      virtual ~IoDevice();

      /**
         @brief Returns the current open mode of the device.
         @return The open mode flags.
      */
      OpenMode openMode() const;

      /**
         @brief Returns true if the device is currently open.
         @return True if open, false otherwise.
      */
      virtual bool isOpen() const;

      /**
         @brief Returns true if the device is readable.
         @return True if readable, false otherwise.
      */
      bool isReadable() const;

      /**
         @brief Returns true if the device is writable.
         @return True if writable, false otherwise.
      */
      bool isWritable() const;

      /**
         @brief Returns true if the device is buffered.
         @return True if buffered, false otherwise.
      */
      bool isBuffered() const;

      /**
         @brief Returns true if this device is sequential; otherwise returns false.

         Sequential devices, as opposed to random-access devices, have no concept of a start, an end, a size, or a current position,
         and do not support seeking. You can only read from the device when it reports that data is available.
         The most common example of a sequential device is a network socket. On Unix, special files such as /dev/zero and fifo pipes are sequential.
         Regular files, on the other hand, do support random access. They have both a size and a current position, and they also support seeking
         backwards and forwards in the data stream. Regular files are non-sequential.

         The IoDevice implementation returns false.
         @return True if the device is sequential, false otherwise.
      */
      virtual bool isSequential() const;

      /**
         @brief Enables or disables text mode for the device.
         @param enabled True to enable text mode, false to disable.
      */
      void setTextModeEnabled (bool enabled);

      /**
         @brief Returns true if text mode is enabled.
         @return True if text mode is enabled, false otherwise.
      */
      bool isTextModeEnabled() const;

      /**
         @brief Enables or disables debug mode for the device.
         @param enabled True to enable debug mode, false to disable.
      */
      void setDebug (bool enabled);

      /**
         @brief Returns true if debug mode is enabled.
         @return True if debug mode is enabled, false otherwise.
      */
      bool isDebug() const;

      /**
         @brief Opens the device with the specified open mode.
         @param mode The open mode flags.
         @return True if the device was successfully opened, false otherwise.
      */
      virtual bool open (OpenMode mode);

      /**
         @brief Closes the device.
      */
      virtual void close();

      /**
         @brief Returns a human-readable description of the last error.
         @return The error string.
      */
      virtual std::string errorString() const;

      /**
         @brief Returns the code of the last error.
         @return The error code.
      */
      virtual int error() const;

    protected:
      /**
         @class Private
         @brief Private implementation details for IoDevice.
      */
      class Private;

      /**
         @brief Constructs an IoDevice with a given private implementation.
         @param dd Reference to the private implementation.
      */
      IoDevice (Private &dd);

      /**
         @brief Pointer to the private implementation.
      */
      std::unique_ptr<Private> d_ptr;

    private:
      /**
         @brief Macro for declaring private implementation accessor.
      */
      PIMP_DECLARE_PRIVATE (IoDevice)
  };

  // public:

  //   enum OpenModeFlag {
  //     NotOpen = 0x0000,
  //     Append = std::ios_base::app, ///< Set the stream's position indicator to the end of the stream before each output operation.
  //     AtEnd = std::ios_base::ate, ///< Set the stream's position indicator to the end of the stream on opening.
  //     Binary = std::ios_base::binary, ///< Consider stream as binary rather than text.
  //     /*
  //        Text mode
  //        When reading, the end-of-line terminators are translated to '\n'.
  //        When writing, the end-of-line terminators are translated to the
  //        local encoding, for example '\r\n' for Win32.
  //     */
  //     ReadOnly = std::ios_base::in, ///< Allow input operations
  //     WriteOnly = std::ios_base::out, ///< Allow output operations
  //     ReadWrite = ReadOnly | WriteOnly, ///< Allow input and output operations
  //     Truncate = std::ios_base::trunc, ///< Any current content is discarded, assuming a length of zero on opening.
  //     IosModes = Append | AtEnd | Binary | ReadOnly | WriteOnly | Truncate,
  //     Unbuffered = (Truncate << 1)
  //   };
  //   typedef Flags<OpenModeFlag> OpenMode;

  //   IoDevice ();
  //   virtual ~IoDevice();

  //   OpenMode openMode() const;
  //   virtual bool isOpen() const;
  //   bool isReadable() const;
  //   bool isWritable() const;
  //   bool isBuffered() const;
  //   /**
  //      Returns true if this device is sequential; otherwise returns false.

  //      Sequential devices, as opposed to a random-access devices, have no
  //      concept of a start, an end, a size, or a current position, and they
  //      do not support seeking. You can only read from the device when it
  //      reports that data is available. The most common example of a sequential
  //      device is a network socket. On Unix, special files such as /dev/zero
  //      and fifo pipes are sequential.

  //      Regular files, on the other hand, do support random access. They have
  //      both a size and a current position, and they also support seeking
  //      backwards and forwards in the data stream. Regular files are non-sequential.

  //      The IoDevice implementation returns false.
  //   */
  //   virtual bool isSequential() const;

  //   void setTextModeEnabled (bool enabled);
  //   bool isTextModeEnabled() const;
  //   void setDebug (bool enabled);
  //   bool isDebug() const;

  //   virtual bool open (OpenMode mode);
  //   virtual void close();
  //   virtual std::string errorString() const;
  //   virtual int error() const;

  // protected:
  //   class Private;
  //   IoDevice (Private &dd);
  //   std::unique_ptr<Private> d_ptr;

  // private:
  //   PIMP_DECLARE_PRIVATE (IoDevice)
  // };

} // namespace Piduino

/* ========================================================================== */
