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

#ifndef _PIDUINO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_
#define _PIDUINO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_

#include <piduino/gpiodevice.h>
#include <piduino/iomap.h>
namespace Piduino {

// -----------------------------------------------------------------------------
//
//                        DeviceBcm2835 Class
//
// -----------------------------------------------------------------------------
  class DeviceBcm2835 : public Device {

    public:
      DeviceBcm2835();
      virtual ~DeviceBcm2835();

      const Gpio::Descriptor * descriptor() const;

      bool open();
      void close();
      AccessLayer preferedAccessLayer() const;
      unsigned int flags() const;

      void setMode (const Pin * pin, Pin::Mode m);
      void setPull (const Pin * pin, Pin::Pull p);
      void write (const Pin * pin, bool v);

      bool read (const Pin * pin) const;
      Pin::Mode mode (const Pin * pin) const;

      const std::map<Pin::Mode, std::string> & modes() const;

    private:
      unsigned long _piobase;
      xIoMap * _iomap;
      const Gpio::Descriptor * _gpioDescriptor;

      inline unsigned int readReg (unsigned int offset) const {
        return *pIo (_iomap, offset);
      }
      
      inline void writeReg (unsigned int offset, unsigned int value) {
        *pIo (_iomap, offset) = value;
      }

      static const std::map<SoC::Id, unsigned long> _iobase;
      static const std::map<unsigned int, Pin::Mode> _int2mode;
      static const std::map<Pin::Mode, unsigned int> _mode2int;
      static const std::map<int, Gpio::Descriptor> _gpioDescriptors;
      static const std::map<Pin::Mode, std::string> _modes;

      static const unsigned int  GpioSize     = 54;
      static const unsigned long Bcm2708Base  = 0x20000000;
      static const unsigned long Bcm2709Base  = 0x3F000000;
      static const unsigned long Bcm2710Base  = 0x3F000000;
      static const unsigned int  MapBlockSize = 4*1024;
      static const unsigned int  PioOffset    = 0x200000;

// Register offsets
      static const unsigned int GFPSEL0 = 0;
      static const unsigned int GFPSEL1 = 1;
      static const unsigned int GFPSEL2 = 2;
      static const unsigned int GFPSEL3 = 3;
      static const unsigned int GFPSEL4 = 4;
      static const unsigned int GFPSEL5 = 5;
// Reserved: 6
      static const unsigned int GPSET0  = 7;
      static const unsigned int GPSET1  = 8;
// Reserved: 9
      static const unsigned int GPCLR0  = 10;
      static const unsigned int GPCLR1  = 11;
// Reserved: 12
      static const unsigned int GPLEV0  = 13;
      static const unsigned int GPLEV1  = 14;
      static const unsigned int GPPUD   =  37;
      static const unsigned int GPPUDCLK0 =   38;
  };
}
/* ========================================================================== */
#endif /*_PIDUINO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_ defined */
