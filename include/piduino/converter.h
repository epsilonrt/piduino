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

#ifndef _PIDUINO_CONVERTER_H_
#define _PIDUINO_CONVERTER_H_

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

      virtual ~Converter();

      Type type() const;
      bool bipolar() const;
      unsigned int resolution() const;
      virtual long max() const;
      virtual long min() const;
      
      virtual void setResolution (unsigned int resolution);
      virtual void setBipolar (bool enable);
      
      virtual bool open () = 0;
      virtual const std::string & name() const = 0;
      virtual long read() = 0;
      virtual void write (long value) = 0;

    protected:
      Converter (Type type, unsigned int resolution, bool bipolar = false);
      virtual bool open (OpenMode mode);

    private:
      Type _type;
      bool _bipolar;
      unsigned int _resolution;
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_CONVERTER_H_ defined */
