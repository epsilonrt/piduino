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

#ifndef PIDUINO_ARCH_ARM_ALLWINNER_GPIO_HX_H
#define PIDUINO_ARCH_ARM_ALLWINNER_GPIO_HX_H

#include <piduino/gpiodevice.h>
#include <piduino/iomap.h>
#include "hx.h"

namespace Piduino {

  namespace AllWinnerHx {

    struct PioBank;
    // -------------------------------------------------------------------------
    //
    //                          GpioDevice Class
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
        void toggle (const Pin * pin);

        bool read (const Pin * pin) const;
        Pin::Mode mode (const Pin * pin) const;
        Pin::Pull pull (const Pin * pin) const;
        void setDrive (const Pin * pin, int d);
        int drive (const Pin * pin) const;

        const std::map<Pin::Mode, std::string> & modes() const;

      private:
        IoMap _iomap[2];

        static const int _portSize[];
        static const std::map<int, Pin::Mode> _int2mode;
        static const std::map<Pin::Mode, int> _mode2int;
        static const std::map<Pin::Mode, std::string> _modes;

        void debugPrintBank (const PioBank * b) const;
        void debugPrintAllBanks () const;
        struct PioBank * pinBank (int * mcupin) const;
        struct PioBank * bank (unsigned int bkindex) const;
    };
  }
}
/* ========================================================================== */
#endif /*PIDUINO_ARCH_ARM_ALLWINNER_GPIO_HX_H defined */
