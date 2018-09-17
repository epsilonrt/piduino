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

#ifndef PIDUINO_TERMINAL_H
#define PIDUINO_TERMINAL_H

#include <piduino/filestream.h>
#include <piduino/terminalnotifier.h>

/**
 *  @defgroup piduino_terminal Terminal
 *  @{
 */

namespace Piduino {

  /**
   * @class Terminal
   * @brief Provides an interface for reading from and writing to terminals (tty)
   */
  class Terminal : public FileStream {

    public:

      /**
        Constructs a Terminal object.
      */
      Terminal ();

      /**
        Constructs a new stream object to represent the file with the given \a path.
      */
      Terminal (const std::string & path);

      /**
        Returns the number of incoming bytes that are waiting to be read.
       */
      ssize_t available() const;

      /**
        @overload
        Reads at most \a maxSize bytes from the terminal into \a data, and
        returns the number of bytes read. If an error occurs, such as when
        attempting to read from a terminal opened in WriteOnly mode, this
        function returns -1.

        0 is returned when no more data is available for reading. However,
        reading past the end of the stream is considered an error, so this
        function returns -1 in those cases.
      */
      virtual ssize_t read (char * data, size_t maxSize);
      
      /**
        @overload
        Reads at most \a maxSize bytes from the terminal into \a data, and
        returns the number of bytes read. If an error occurs, such as when
        attempting to read from a terminal opened in WriteOnly mode, this
        function returns -1.

        0 is returned when no more data is available for reading. However,
        reading past the end of the stream is considered an error, so this
        function returns -1 in those cases.
      */
      ssize_t read (char * buf, size_t maxSize, long msTimeout);
      
      ssize_t read (std::string & str, long msTimeout = 0);
      ssize_t read (char & c, long msTimeout = 0);

      ssize_t peek (char * buf, size_t len, long msTimeout = 0);
      ssize_t peek (std::string & str, long msTimeout = 0);
      ssize_t peek (char & c, long msTimeout = 0);

      Piduino::TerminalNotifier & notifier();
      const Piduino::TerminalNotifier & notifier() const;

    protected:
      class Private;
      Terminal (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Terminal)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_TERMINAL_H defined */
