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

#include <string>
#include <piduino/iodevice.h>

/**
 *  @defgroup piduino_converter A/D or D/A Converter
 *  @{
 */

namespace Piduino {

  class Converter : public IoDevice {

    public:
      enum Type {
        AnalogToDigital,
        DigitalToAnalog,
        None = -1
      };

      Converter();
      virtual ~Converter();

      Type type() const;
      virtual bool bipolar() const;
      virtual int resolution() const;
      virtual int setResolution (int resolution) { return -1; }
      virtual long max() const;
      virtual long min() const;
      
      virtual bool open (IoDevice::OpenMode mode = IoDevice::ReadWrite);
      virtual void close();
      virtual long read();
      virtual bool write (long value);
      virtual void setEnable (bool enable) { /* do nothing */ }
      virtual bool isEnabled () const { return true; }
      inline void run() { setEnable (true); }
      inline void stop() { setEnable (false); }

      virtual const std::string & deviceName() const;

    protected:
      class Private;
      Converter (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Converter)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
