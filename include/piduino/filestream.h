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

#ifndef PIDUINO_FILESTREAM_H
#define PIDUINO_FILESTREAM_H

#include <string>
#include <iostream>
#include <piduino/filedevice.h>

/**
 *  @defgroup piduino_filestream File Stream
 *  @{
 */

namespace Piduino {

  /**
   * @class FileStream
   * @brief Provides an interface for reading from and writing to files with stream
   */
  class FileStream : public FileDevice, public std::iostream {

    public:

      /**
        Constructs a FileStream object.
      */
      FileStream ();

      /**
        Constructs a new stream object to represent the file with the given \a path.
      */
      FileStream (const std::string & path);

      /**
        @overload
        Writes at most \a maxSize bytes of data from \a data to the
        stream. Returns the number of bytes that were actually written, or
        -1 if an error occurred.

        \sa read()
      */
      virtual ssize_t write (const char * data, size_t maxSize);

      /**
        @overload

        Writes data from a zero-terminated string of 8-bit characters to the
        stream. Returns the number of bytes that were actually written, or
        -1 if an error occurred.

        \sa read()
      */
      ssize_t write (const char * str);

      /**
          @overload

          Writes the content of \a string to the stream. Returns the number of
          bytes that were actually written, or -1 if an error occurred.

          \sa read()
      */
      ssize_t write (const std::string & str);

      /**
          @overload

          Writes the character \a c to the stream. Returns the number of
          bytes that were actually written, or -1 if an error occurred.

          \sa read()
      */
      ssize_t write (char c);

      /**
        @overload
        Reads at most \a maxSize bytes from the stream into \a data, and
        returns the number of bytes read. If an error occurs, such as when
        attempting to read from a stream opened in WriteOnly mode, this
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
      FileStream (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (FileStream)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_FILESTREAM_H defined */
