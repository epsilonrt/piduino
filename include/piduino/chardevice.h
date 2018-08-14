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

#ifndef PIDUINO_CHARDEVICE_H
#define PIDUINO_CHARDEVICE_H

#include <vector>
#include <piduino/iodevice.h>
#include <piduino/ringbuffer.h>

/**
 *  @defgroup piduino_chardevice Characters Device
 *  @{
 */
namespace Piduino {

  class CharDevice : public IoDevice {

    public:

      /**
       * Constructs a CharDevice object.
       */
      CharDevice ();
      
      /**
       * The destructor is virtual, and CharDevice is an abstract base class. 
       * This destructor does not call close(), but the subclass destructor might. 
       * If you are in doubt, call close() before destroying the CharDevice.
       */
      virtual ~CharDevice();

      /**
       * Opens the device and sets its OpenMode to \a mode. Returns true if 
       * successful; otherwise returns false. 
       * This function should be called from any reimplementations of open() or 
       * other functions that open the device.
       */
      virtual bool open (OpenMode mode);
      
      /**
       * Closes the device and sets its OpenMode to NotOpen. 
       * The error string is also reset.
       */
      virtual void close();

      /**
        For random-access devices, this function returns the position that
        data is written to or read from. For sequential devices or closed
        devices, where there is no concept of a "current position", 0 is
        returned.

        The current read/write position of the device is maintained internally by
        CharDevice, so reimplementing this function is not necessary. When
        subclassing CharDevice, use CharDevice::seek() to notify CharDevice about
        changes in the device position.
       */
      virtual long pos() const;
      
      /**
        For open random-access devices, this function returns the size of the
        device. For open sequential devices, bytesAvailable() is returned.

        If the device is closed, the size returned will not reflect the actual
        size of the device.
       */
      virtual long size() const;
      
      /**
        For random-access devices, this function sets the current position
        to \a pos, returning true on success, or false if an error occurred.
        For sequential devices, the default behavior is to do nothing and
        return false.

        When subclassing CharDevice, you must call CharDevice::seek() at the
        start of your function to ensure integrity with CharDevice's
        built-in buffer. The base implementation always returns true.
       */
      virtual bool seek (long pos);
      
      /**
        Returns true if the current read and write position is at the end
        of the device (i.e. there is no more data available for reading on
        the device); otherwise returns false.

        For some devices, atEnd() can return true even though there is more data
        to read. This special case only applies to devices that generate data in
        direct response to you calling read().
       */
      virtual bool atEnd() const;
      
      /**
        Seeks to the start of input for random-access devices. Returns
        true on success; otherwise returns false (for example, if the
        device is not open).
       */
      virtual bool reset();

      /**
        Returns the number of bytes that are available for reading. This
        function is commonly used with sequential devices to determine the
        number of bytes to allocate in a buffer before reading.

        Subclasses that reimplement this function must call the base
        implementation in order to include the size of CharDevices' buffer. Example:
       */
      virtual long bytesAvailable() const;
      
      /**
        For buffered devices, this function returns the number of bytes
        waiting to be written. For devices with no buffer, this function
        returns 0.
       */
      virtual long bytesToWrite() const;

      /**
        Reads at most \a maxlen bytes from the device into \a data, and
        returns the number of bytes read. If an error occurs, such as when
        attempting to read from a device opened in WriteOnly mode, this
        function returns -1.

        0 is returned when no more data is available for reading. However,
        reading past the end of the stream is considered an error, so this
        function returns -1 in those cases (that is, reading on a closed
        socket or after a process has died).
       */
      long read (char *data, long maxlen);
      
      /**
        Reads at most \a maxlen bytes from the device, and returns the
        data read as a vector<char>.

        This function has no way of reporting errors; returning an empty
        vector<char>() can mean either that no data was currently available
        for reading, or that an error occurred.
       */
      std::vector<char> read (long maxlen);
      
      /**
        Reads all available data from the device, and returns it as a
        vector<char>.

        This function has no way of reporting errors; returning an empty
        vector<char>() can mean either that no data was currently available
        for reading, or that an error occurred.
       */
      std::vector<char> readAll();
      
      /**
        This function reads a line of ASCII characters from the device, up
        to a maximum of \a maxlen - 1 bytes, stores the characters in \a
        data, and returns the number of bytes read. If a line could not be
        read but no error ocurred, this function returns 0. If an error
        occurs, this function returns the length of what could be read, or
        -1 if nothing was read.

        A terminating '\0' byte is always appended to \a data, so \a
        maxlen must be larger than 1.

        Data is read until either of the following conditions are met:

        \list
        \o The first '\n' character is read.
        \o \a maxlen - 1 bytes are read.
        \o The end of the device data is detected.
        \endlist

        The newline character ('\n') is included in the buffer. If a
        newline is not encountered before maxlen - 1 bytes are read, a
        newline will not be inserted into the buffer. On windows newline
        characters are replaced with '\n'.

        This function calls readLineData(), which is implemented using
        repeated calls to getChar(). You can provide a more efficient
        implementation by reimplementing readLineData() in your own
        subclass.
       */
      long readLine (char *data, long maxlen);
      
      /**
        Reads a line from the device, but no more than \a maxlen characters,
        and returns the result as a vector<char>.

        This function has no way of reporting errors; returning an empty
        vector<char>() can mean either that no data was currently available
        for reading, or that an error occurred.
       */
      std::vector<char> readLine (long maxlen);
      
      /**
        Returns true if a complete line of data can be read from the device;
        otherwise returns false.

        Note that unbuffered devices, which have no way of determining what
        can be read, always return false.

        This function is often called in conjunction with the readyRead()
        signal.

        Subclasses that reimplement this function must call the base
        implementation in order to include the contents of the CharDevice's buffer. Example:
       */
      virtual bool canReadLine() const;
      

      /**
        Writes at most \a maxlen bytes of data from \a data to the
        device. Returns the number of bytes that were actually written, or
        -1 if an error occurred.
       */
      long write (const char *data, long len);
      
      /**
        Writes data from a zero-terminated string of 8-bit characters to the
        device. Returns the number of bytes that were actually written, or
        -1 if an error occurred. This is equivalent to
        \code
        ...
        CharDevice::write(data, std::strlen(data));
        ...
        \endcode
       */
      long write (const char *data);
      
      /**
        Writes the content of \a byteArray to the device. Returns the number of
        bytes that were actually written, or -1 if an error occurred.
       */
      inline long write (const std::vector<char> &data) {
        return write (data.data(), data.size());
      }

      /**
        Reads at most \a maxlen bytes from the device into \a data, without side
        effects (i.e., if you call read() after peek(), you will get the same
        data).  Returns the number of bytes read. If an error occurs, such as
        when attempting to peek a device opened in WriteOnly mode, this function
        returns -1.

        0 is returned when no more data is available for reading.
       */
      long peek (char *data, long maxlen);
      
      /**
        Peeks at most \a maxlen bytes from the device, returning the data peeked
        as a vector<char>.

        This function has no way of reporting errors; returning an empty
        vector<char>() can mean either that no data was currently available
        for peeking, or that an error occurred.
       */
      std::vector<char> peek (long maxlen);

      /**
        Blocks until new data is available for reading and the readyRead()
        signal has been emitted, or until \a msecs milliseconds have
        passed. If msecs is -1, this function will not time out.

        Returns true if new data is available for reading; otherwise returns
        false (if the operation timed out or if an error occurred).

        This function can operate without an event loop. It is
        useful when writing non-GUI applications and when performing
        I/O operations in a non-GUI thread.

        If called from within a slot connected to the readyRead() signal,
        readyRead() will not be reemitted.

        Reimplement this function to provide a blocking API for a custom
        device. The default implementation does nothing, and returns false.

        \warning Calling this function from the main (GUI) thread
        might cause your user interface to freeze.
       */
      virtual bool waitForReadyRead (int msecs);
      
      /**
        For buffered devices, this function waits until a payload of
        buffered written data has been written to the device and the
        bytesWritten() signal has been emitted, or until \a msecs
        milliseconds have passed. If msecs is -1, this function will
        not time out. For unbuffered devices, it returns immediately.

        Returns true if a payload of data was written to the device;
        otherwise returns false (i.e. if the operation timed out, or if an
        error occurred).

        This function can operate without an event loop. It is
        useful when writing non-GUI applications and when performing
        I/O operations in a non-GUI thread.

        If called from within a slot connected to the bytesWritten() signal,
        bytesWritten() will not be reemitted.

        Reimplement this function to provide a blocking API for a custom
        device. The default implementation does nothing, and returns false.

        \warning Calling this function from the main (GUI) thread
        might cause your user interface to freeze.
       */
      virtual bool waitForBytesWritten (int msecs);

      /**
        Puts the character \a c back into the device, and decrements the
        current position unless the position is 0. This function is
        usually called to "undo" a getChar() operation, such as when
        writing a backtracking parser.

        If \a c was not previously read from the device, the behavior is
    undefined.
       */
      void ungetChar (char c);
      
      /**
        Writes the character \a c to the device. Returns true on success;
        otherwise returns false.
       */
      bool putChar (char c);
      
      /**
        Reads one character from the device and stores it in \a c. If \a c
        is 0, the character is discarded. Returns true on success;
        otherwise returns false.
       */
      bool getChar (char *c);

    protected:
      class Private;
      CharDevice (Private &dd);
      
      /**
        Reads up to \a maxlen bytes from the device into \a data, and
        returns the number of bytes read or -1 if an error occurred.

        If there are no bytes to be read and there can never be more bytes
        available (examples include socket closed, pipe closed, sub-process
        finished), this function returns -1.

        This function is called by CharDevice. Reimplement this function
        when creating a subclass of CharDevice.

        When reimplementing this function it is important that this function
        reads all the required data before returning. 
       */
      virtual long readData (char * data, long maxlen) = 0;
      
      /**
        Writes up to \a maxlen bytes from \a data to the device. Returns
        the number of bytes written, or -1 if an error occurred.

        This function is called by CharDevice. Reimplement this function
        when creating a subclass of CharDevice.

        When reimplementing this function it is important that this function
        writes all the data available before returning. 
       */
      virtual long writeData (const char * data, long len) = 0;
      
      /**
        Reads up to \a maxlen characters into \a data and returns the
        number of characters read.

        This function is called by readLine(), and provides its base
        implementation, using getChar(). Buffered devices can improve the
        performance of readLine() by reimplementing this function.

        readLine() appends a '\0' byte to \a data; readLineData() does not
        need to do this.

        If you reimplement this function, be careful to return the correct
        value: it should return the number of bytes read in this line,
        including the terminating newline, or 0 if there is no line to be
        read at this point. If an error occurs, it should return -1 if and
        only if no bytes were read. Reading past EOF is considered an error.
       */
      virtual long readLineData (char *data, long maxlen);

    private:
      PIMP_DECLARE_PRIVATE (CharDevice)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /* PIDUINO_CHARDEVICE_H defined */
