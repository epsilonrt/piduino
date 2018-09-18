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

#ifndef PIDUINO_FILEDEVICE_H
#define PIDUINO_FILEDEVICE_H

#include <string>
#include <piduino/iodevice.h>

/**
 *  @defgroup piduino_filedevice File Device
 *  @{
 */

namespace Piduino {

  /**
   * @class FileDevice
   * @brief Provides an interface for reading from and writing to files

    FileDevice is an I/O device for reading and writing binary and text
    files (with std::iostream features \sa FileDevice::ios()).

    The file name is usually passed in the constructor, but it can be
    set at any time using setFileName(). FileDevice expects the file
    separator to be '/' regardless of operating system.

    The file is opened with open(), closed with close().

    Data is usually read and written using std::iostream with ios(), but you can
    also call the functions read() and write().
   */
  class FileDevice : public IoDevice {

    public:

      /**
        Constructs a FileDevice object.
      */
      FileDevice ();

      /**
        Constructs a FileDevice object from an open file descriptor.
      */
      FileDevice (int fd);

      /**
        Constructs a new file object to represent the file with the given \a path.
      */
      FileDevice (const std::string & path);

      /**
        Destroys the file object, closing it if necessary.
      */
      virtual ~FileDevice();

      /**
        Opens the file device using OpenMode \a mode, and then returns \c true if
        successful; otherwise returns \c false and sets an error code which can be
        obtained by calling the error() method.  The error string is also reset.
       */
      virtual bool open (OpenMode mode = ReadWrite);

      /**
        Closes the device and sets its OpenMode to NotOpen
       */
      virtual void close();

      /**
        Sets the \a path of the file. The path can have no prefix, a
        relative path, or an absolute path.

        Do not call this function if the file has already been opened, if so,
        the file is closed before the path is modified.

        If the file name has no path or a relative path, the path used
        will be the application's current directory path
        at the time of the open() call.

        Note that the directory separator is "/".

          \sa path()
      */
      virtual void setPath (const std::string & path);

      /**
        Returns the path set by setPath() or to the FileDevice
        constructors.

        \sa setPath()
      */
      virtual std::string path() const;
      
      bool setFd (int fd);

      /**
        Returns the file descriptor of the file, if it is open, else returns -1.

        Do not use the ::close() function with this descriptor !
      */
      int fd() const;
      
      bool isOurFile() const;

      /**
          Writes at most \a maxSize bytes of data from \a data to the
          device. Returns the number of bytes that were actually written, or
          -1 if an error occurred.

          \sa read()
      */
      virtual ssize_t write (const char * data, size_t maxSize);

      /**
        @overload

        Writes data from a zero-terminated string of 8-bit characters to the
        device. Returns the number of bytes that were actually written, or
        -1 if an error occurred.

        \sa read()
      */
      ssize_t write (const char * str);

      /** 
          @overload

          Writes the content of \a string to the device. Returns the number of
          bytes that were actually written, or -1 if an error occurred.

          \sa read()
      */
      ssize_t write (const std::string & str);

      /**
        Reads at most \a maxSize bytes from the device into \a data, and
        returns the number of bytes read. If an error occurs, such as when
        attempting to read from a device opened in WriteOnly mode, this
        function returns -1.

        0 is returned when no more data is available for reading. However,
        reading past the end of the stream is considered an error, so this
        function returns -1 in those cases (that is, reading on a closed
        socket or after a process has died).

        \sa write()
      */
      virtual ssize_t read (char * data, size_t maxSize);

    protected:
      class Private;
      FileDevice (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (FileDevice)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_FILEDEVICE_H defined */
