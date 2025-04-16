/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <piduino/gpiodevice.h>

namespace Piduino {

  // -------------------------------------------------------------------------
  //
  //                        Bcm2835Gpio Class
  //
  // -------------------------------------------------------------------------
  class Bcm2835Gpio : public GpioDevice {

    public:
      Bcm2835Gpio();
      virtual ~Bcm2835Gpio();

      bool open();
      void close();
      AccessLayer preferedAccessLayer() const;
      unsigned int flags() const;

      void setMode (const Pin *pin, Pin::Mode m);
      void setPull (const Pin *pin, Pin::Pull p);
      void write (const Pin *pin, bool v);

      bool read (const Pin *pin) const;
      Pin::Mode mode (const Pin *pin) const;
      Pin::Pull pull (const Pin *pin) const;
      int systemNumberOffset() const;

      const std::map<Pin::Mode, std::string> &modes() const;

    protected:
      class Private;
      Bcm2835Gpio (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Bcm2835Gpio)
  };
}
/* ========================================================================== */
