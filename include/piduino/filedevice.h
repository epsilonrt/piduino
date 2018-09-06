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
 *  @defgroup piduino_iodevice IO Device
 *  @{
 */

namespace Piduino {

  class FileDevice : public IoDevice {

    public:

      FileDevice ();
      virtual ~FileDevice();

      virtual bool open (OpenMode mode = ReadWrite | Binary);
      virtual void close();

      int fd() const;
      virtual void setPath (const std::string & path);
      virtual std::string path() const;

      virtual long write (const char * buf, long n);
      long write (const char * str);
      long write (const std::string & str);

      virtual long read (char * buf, long n);
      std::string read (long n = -1);

      virtual int bytesAvailable();
      virtual bool waitForReadyRead (int msecs);
      virtual bool waitForBytesWritten (int msecs);

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
