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
#ifndef PIDUINO_ARCH_ARM_BROADCOM_GPIO_BCM2835_H
#define PIDUINO_ARCH_ARM_BROADCOM_GPIO_BCM2835_H

#include <piduino/gpiodevice.h>
#include <piduino/iomap.h>
#include "bcm2835.h"

namespace Piduino {

  namespace Bcm2835 {
    // -------------------------------------------------------------------------
    //
    //                        Bcm2835::GpioDevice Class
    //
    // -------------------------------------------------------------------------
    class GpioDevice : public Piduino::GpioDevice {

      public:
        GpioDevice();
        virtual ~GpioDevice();

        bool open();
        void close();
        AccessLayer preferedAccessLayer() const;
        unsigned int flags() const;

        void setMode (const Pin * pin, Pin::Mode m);
        void setPull (const Pin * pin, Pin::Pull p);
        void write (const Pin * pin, bool v);

        bool read (const Pin * pin) const;
        Pin::Mode mode (const Pin * pin) const;
        Pin::Pull pull (const Pin * pin) const;

        const std::map<Pin::Mode, std::string> & modes() const;

      private:
        off_t _piobase;
        IoMap  _iomap;

        inline uint32_t readReg (size_t offset) const {
          return *_iomap.io (offset);
        }

        inline void writeReg (size_t offset, uint32_t value) {
          *_iomap.io (offset) = value;
        }

        static const std::map<unsigned int, Pin::Mode> _int2mode;
        static const std::map<Pin::Mode, unsigned int> _mode2int;
        static const std::map<Pin::Mode, std::string> _modes;
        static bool _is2711;
    };
  }
}
/* ========================================================================== */
#endif /*PIDUINO_ARCH_ARM_BROADCOM_GPIO_BCM2835_H defined */
