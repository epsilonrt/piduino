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
 */
#include <piduino/private/chardevice_p.h>
#include <algorithm>

namespace Piduino {

  // ---------------------------------------------------------------------------
  CharDevice::Private::Private (bool isSequential) :
    IoDevice::Private (isSequential), firstRead (true),
    pos (0), devicePos (0), baseReadLineDataCalled (false) {}

  // ---------------------------------------------------------------------------
  CharDevice::Private::~Private()  {}

  // ---------------------------------------------------------------------------
  CharDevice::CharDevice (CharDevice::Private &dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  CharDevice::CharDevice (bool isSequential) :
    IoDevice (*std::make_unique<Private> (isSequential))  {

  }

  // ---------------------------------------------------------------------------
  CharDevice::~CharDevice() {
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::open (OpenMode mode) {

    if (!isOpen()) {
      if (IoDevice::open (mode) == false) {

        return false;
      }
      d->pos = ( (mode & OpenMode::Append) == OpenMode::Append) ? size() : 0;
      d->buffer.clear();
      d->firstRead = true;
    }
    return true;
  }

  // ---------------------------------------------------------------------------
  void
  CharDevice::close() {

    if (isOpen()) {
      d->pos = 0;
      d->buffer.clear();
      d->firstRead = true;
      IoDevice::close();
    }
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::pos() const {
    return d->pos;
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::size() const {
    return isSequential() ?  bytesAvailable() : 0;
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::seek (long pos) {

    if (!isOpen() || (pos < 0) || isSequential()) {
      return false;
    }

    long offset = pos - d->pos;
    d->pos = pos;

    if (offset < 0 || offset >= long (d->buffer.size())) {
      // When seeking backwards, an operation that is only allowed for
      // random-access devices, the buffer is cleared. The next read
      // operation will then refill the buffer. We can optimize this, if we
      // find that seeking backwards becomes a significant performance hit.

      d->buffer.clear();
    }
    else if (!d->buffer.empty()) {

      d->buffer.skip (int (offset));
    }

    return true;
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::atEnd() const {

    return !isOpen() || (d->buffer.empty() && bytesAvailable() == 0);
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::reset() {

    return seek (0);
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::bytesAvailable() const {

    if (!isSequential()) {
      return std::max (size() - d->pos, static_cast<long> (0));
    }
    // isSequential
    return d->buffer.size();
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::bytesToWrite() const {
    return 0;
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::read (char *data, long maxlen) {

    if (maxlen < 0) {
      return -1;
    }

    // Short circuit for getChar()
    if (maxlen == 1) {
      int chint;
      while ( (chint = d->buffer.getChar()) != -1) {
        d->incrementPos (1);

        char c = static_cast<unsigned char> (chint);
        if (c == '\r' && isTextModeEnabled()) {
          continue;
        }
        *data = c;
        return long (1);
      }
    }

    long readSoFar = 0;
    bool moreToRead = true;
    do {
      // Try reading from the buffer.
      int lastReadChunkSize = d->buffer.read (data, maxlen);

      if (lastReadChunkSize > 0) {

        d->incrementPos (lastReadChunkSize);
        readSoFar += lastReadChunkSize;
        // fast exit when satisfied by buffer
        if (lastReadChunkSize == maxlen && ! isTextModeEnabled()) {
          return readSoFar;
        }

        data += lastReadChunkSize;
        maxlen -= lastReadChunkSize;
      }
      else {
        if (d->firstRead) {
          // this is the first time the file has been read, check it's valid and set up pos pointers
          // for fast pos updates.
          if (!isReadable() || !isOpen()) {
            return -1;
          }
          d->firstRead = false;
        }

        if (maxlen == 0) {
          return readSoFar;
        }

        if (isBuffered() && (maxlen < CHARDEVICE_BUFFERSIZE)) {
          // In buffered mode, we try to fill up the QIODevice buffer before
          // we do anything else.
          // buffer is empty at this point, try to fill it

          int bytesToBuffer = CHARDEVICE_BUFFERSIZE;
          char * writePointer = d->buffer.reserve (bytesToBuffer);

          // Make sure the device is positioned correctly.
          if (d->pos != d->devicePos && !isSequential() && !seek (d->pos)) {

            return readSoFar ? readSoFar : long (-1);
          }

          long readFromDevice = readData (writePointer, bytesToBuffer);
          d->buffer.chop (bytesToBuffer - (readFromDevice < 0 ? 0 : int (readFromDevice)));

          if (readFromDevice > 0) {

            d->incrementDevicePos (readFromDevice);

            if (!d->buffer.empty()) {

              lastReadChunkSize = d->buffer.read (data, maxlen);
              readSoFar += lastReadChunkSize;
              data += lastReadChunkSize;
              maxlen -= lastReadChunkSize;
              d->incrementPos (lastReadChunkSize);
            }
          }
        }
      }

      // If we need more, try reading from the device.
      if (maxlen > 0) {

        // Make sure the device is positioned correctly.
        if (d->pos != d->devicePos && !isSequential() && !seek (d->pos)) {

          return readSoFar ? readSoFar : long (-1);
        }

        long readFromDevice = readData (data, maxlen);
        if (readFromDevice == -1 && readSoFar == 0) {
          // error and we haven't read anything: return immediately
          return -1;
        }

        if (readFromDevice > 0) {
          lastReadChunkSize += int (readFromDevice);
          readSoFar += readFromDevice;
          data += readFromDevice;
          maxlen -= readFromDevice;
          d->incrementPos (readFromDevice);
          d->incrementDevicePos (readFromDevice);
        }
      }
      // Best attempt has been made to read data, don't try again except for text mode adjustment below
      moreToRead = false;

      if (readSoFar && isTextModeEnabled()) {
        char *readPtr = data - lastReadChunkSize;
        const char *endPtr = data;

        if (readPtr < endPtr) {
          // optimization to avoid initial self-assignment
          while (*readPtr != '\r') {
            if (++readPtr == endPtr) {
              return readSoFar;
            }
          }

          char *writePtr = readPtr;

          while (readPtr < endPtr) {
            char ch = *readPtr++;
            if (ch != '\r') {
              *writePtr++ = ch;
            }
            else {
              --readSoFar;
              --data;
              ++maxlen;
            }
          }

          // Make sure we get more data if there is room for more. This
          // is very important for when someone seeks to the start of a
          // '\r\n' and reads one character - they should get the '\n'.
          moreToRead = (readPtr != writePtr);
        }
      }
    }
    while (moreToRead);

    return readSoFar;
  }

  // ---------------------------------------------------------------------------
  std::vector<char>
  CharDevice::read (long maxlen) {
    std::vector<char> result;

    if (maxlen < 0) {
      return result;
    }

    if (maxlen > result.max_size()) {
      maxlen = result.max_size();
    }

    long readBytes = 0;

    if (maxlen) {

      result.resize (static_cast<std::size_t> (maxlen));
      if (result.size() == 0) {
        // If resize fails, read incrementally.
        long readResult;
        do {
          result.resize (std::min (static_cast<std::size_t> (maxlen), result.size() + CHARDEVICE_BUFFERSIZE));
          readResult = read (result.data() + readBytes, result.size() - readBytes);
          if (readResult > 0 || readBytes == 0) {
            readBytes += readResult;
          }
        }
        while (readResult == CHARDEVICE_BUFFERSIZE);
      }
      else {
        readBytes = read (result.data(), result.size());
      }
    }

    if (readBytes <= 0) {

      result.clear();
    }
    else {

      result.resize (int (readBytes));
    }

    return result;
  }

  // ---------------------------------------------------------------------------
  std::vector<char>
  CharDevice::readAll() {
    std::vector<char> result;
    long readBytes = 0;

    // flush internal read buffer
    if (!isTextModeEnabled() && !d->buffer.empty()) {

      result = d->buffer.readAll();
      readBytes = result.size();
      d->incrementPos (readBytes);
    }

    long theSize;
    if (isSequential() || (theSize = size()) == 0) {
      // Size is unknown, read incrementally.
      long readResult;
      do {
        result.resize (result.size() + CHARDEVICE_BUFFERSIZE);
        readResult = read (result.data() + readBytes, result.size() - readBytes);
        if (readResult > 0 || readBytes == 0) {
          readBytes += readResult;
        }
      }
      while (readResult > 0);
    }
    else {
      // Read it all in one go.
      // If resize fails, don't read anything.
      result.resize (static_cast<std::size_t> (readBytes + theSize - d->pos));
      readBytes += read (result.data() + readBytes, result.size() - readBytes);
    }

    if (readBytes <= 0) {
      result.clear();
    }
    else {
      result.resize (static_cast<std::size_t> (readBytes));
    }

    return result;
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::readLine (char *data, long maxlen) {

    if (maxlen < 2) {
      return -1;
    }

    // Leave room for a '\0'
    --maxlen;

    long readSoFar = 0;

    if (!d->buffer.empty()) {

      readSoFar = d->buffer.readLine (data, maxlen);
      d->incrementPos (readSoFar);

      if (readSoFar && data[readSoFar - 1] == '\n') {

        if (isTextModeEnabled()) {

          if (readSoFar > 1 && data[readSoFar - 2] == '\r') {
            --readSoFar;
            data[readSoFar - 1] = '\n';
          }
        }
        data[readSoFar] = '\0';
        return readSoFar;
      }
    }

    if (d->pos != d->devicePos && !isSequential() && !seek (d->pos)) {
      return long (-1);
    }

    d->baseReadLineDataCalled = false;
    long readBytes = readLineData (data + readSoFar, maxlen - readSoFar);

    if (readBytes < 0) {
      data[readSoFar] = '\0';
      return readSoFar ? readSoFar : -1;
    }

    readSoFar += readBytes;
    if (!d->baseReadLineDataCalled && !isSequential()) {
      d->pos += readBytes;
      // If the base implementation was not called, then we must
      // assume the device position is invalid and force a seek.
      d->devicePos = long (-1);
    }
    data[readSoFar] = '\0';

    if (isTextModeEnabled()) {
      if (readSoFar > 1 && data[readSoFar - 1] == '\n' && data[readSoFar - 2] == '\r') {
        data[readSoFar - 2] = '\n';
        data[readSoFar - 1] = '\0';
        --readSoFar;
      }
    }

    return readSoFar;
  }

  // ---------------------------------------------------------------------------
  std::vector<char>
  CharDevice::readLine (long maxlen) {
    std::vector<char> result;

    if (maxlen < 0) {
      return result;
    }

    if (maxlen > result.max_size()) {
      maxlen = result.max_size();
    }

    result.resize (static_cast<std::size_t> (maxlen));

    long readBytes = 0;

    if (result.size() == 0) {

      // If resize fails or maxlen == 0, read incrementally
      if (maxlen == 0) {

        maxlen = result.max_size();
      }

      // The first iteration needs to leave an extra byte for the terminating null
      result.resize (1);

      long readResult;
      do {

        result.resize (std::min (static_cast<std::size_t> (maxlen), result.size() + CHARDEVICE_BUFFERSIZE));
        readResult = readLine (result.data() + readBytes, result.size() - readBytes);
        if (readResult > 0 || readBytes == 0) {
          readBytes += readResult;
        }
      }
      while (readResult == CHARDEVICE_BUFFERSIZE
             && result[int (readBytes - 1)] != '\n');
    }
    else {

      readBytes = readLine (result.data(), result.size());
    }

    if (readBytes <= 0) {

      result.clear();
    }
    else {

      result.resize (readBytes);
    }

    return result;
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::canReadLine() const {
    return d->buffer.canReadLine();
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::write (const char *data, long len) {

    if ( (len < 0) || !isWritable() || !isOpen()) {

      return -1;
    }

    // Make sure the device is positioned correctly.
    if (d->pos != d->devicePos && !isSequential() && !seek (d->pos)) {
      return -1;
    }

    long written = writeData (data, len);

    if (written > 0) {

      d->incrementPos (written);
      d->incrementDevicePos (written);

      if (!d->buffer.empty() && !isSequential()) {
        d->buffer.skip (written);
      }
    }
    return written;
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::write (const char *data) {
    return write (data, std::strlen (data));
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::peek (char *data, long maxlen) {
    long readBytes = read (data, maxlen);

    if (readBytes <= 0) {
      return readBytes;
    }

    d->buffer.ungetBlock (data, readBytes);
    d->incrementPos (-readBytes);
    return readBytes;
  }

  // ---------------------------------------------------------------------------
  std::vector<char>
  CharDevice::peek (long maxlen) {
    std::vector<char> result = read (maxlen);

    if (result.empty()) {
      return result;
    }

    d->buffer.ungetBlock (result.data(), result.size());
    d->incrementPos (-result.size());
    return result;
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::waitForReadyRead (int msecs) {
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::waitForBytesWritten (int msecs) {
    return false;
  }

  // ---------------------------------------------------------------------------
  void
  CharDevice::ungetChar (char c) {

    if (!isReadable() || !isOpen()) {
      return;
    }
    d->buffer.ungetChar (c);
    d->incrementPos (-1);
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::putChar (char c) {
    return (1 == write (&c, 1));
  }

  // ---------------------------------------------------------------------------
  bool
  CharDevice::getChar (char *c) {
    char ch;
    return (1 == read (c ? c : &ch, 1));
  }

  // ---------------------------------------------------------------------------
  long
  CharDevice::readLineData (char *data, long maxlen) {
    long readSoFar = 0;
    char c;
    int lastReadReturn = 0;
    d->baseReadLineDataCalled = true;

    while (readSoFar < maxlen && (lastReadReturn = read (&c, 1)) == 1) {
      *data++ = c;
      ++readSoFar;
      if (c == '\n') {
        break;
      }
    }

    if (lastReadReturn != 1 && readSoFar == 0) {
      return isSequential() ? lastReadReturn : -1;
    }
    return readSoFar;

  }
}

/* ========================================================================== */
