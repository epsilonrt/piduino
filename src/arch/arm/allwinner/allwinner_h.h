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

#ifndef _PIDUINO_ARCH_ARM_NANOPI_GPIODEVICE_H_
#define _PIDUINO_ARCH_ARM_NANOPI_GPIODEVICE_H_

#include <piduino/gpiodevice.h>
#include <piduino/iomap.h>
namespace Piduino {

  struct PioBank;
// -----------------------------------------------------------------------------
//
//                          DeviceAllwinnerH Class
//
// -----------------------------------------------------------------------------
  class DeviceAllwinnerH : public Device {

    public:
      DeviceAllwinnerH();
      virtual ~DeviceAllwinnerH();

      const Gpio::Descriptor * descriptor() const;

      bool open();
      void close();
      AccessLayer preferedAccessLayer() const;
      unsigned int flags() const;

      void setMode (const Pin * pin, Pin::Mode m);
      void setPull (const Pin * pin, Pin::Pull p);
      void write (const Pin * pin, bool v);
      void toggle (const Pin * pin);

      bool read (const Pin * pin) const;
      Pin::Mode mode (const Pin * pin) const;
      Pin::Pull pull (const Pin * pin) const;

      const std::map<Pin::Mode, std::string> & modes() const;

    private:
      xIoMap * iomap[2];
      const Gpio::Descriptor * _gpioDescriptor;

      static const int _portSize[];
      static const std::map<int, Pin::Mode> _int2mode;
      static const std::map<Pin::Mode, int> _mode2int;
      static const std::map<int, Gpio::Descriptor> _gpioDescriptors;
      static const std::map<Pin::Mode, std::string> _modes;

      const unsigned long Io1Base = 0x01C20000;
      const unsigned int Pio1Offset = 0x800;
      const unsigned long Io2Base = 0x01F02000;
      const unsigned int Pio2Offset = 0xC00;
      const unsigned int GpioSize = 106;
      const unsigned int MapBlockSize = 4*1024;

      void debugPrintBank (const PioBank * b) const;
      void debugPrintAllBanks () const;
      struct PioBank * pinBank (int * mcupin) const;
      struct PioBank * bank (unsigned int bkindex) const;
  };
}
/* ========================================================================== */
#endif /*_PIDUINO_ARCH_ARM_NANOPI_GPIODEVICE_H_ defined */
