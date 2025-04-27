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
  //                        GpioTemplate Class
  //
  // -------------------------------------------------------------------------
  class GpioTemplate : public GpioDevice {

    public:
      GpioTemplate();
      virtual ~GpioTemplate();

      // Mandatory API
      // ----------------------------------------------------------------
      bool open();
      void close();
      AccessLayer preferedAccessLayer() const;
      unsigned int flags() const;

      void setMode (const Pin *pin, Pin::Mode m);
      Pin::Mode mode (const Pin *pin) const;

      void write (const Pin *pin, bool v);
      bool read (const Pin *pin) const;

      void setPull (const Pin *pin, Pin::Pull p);

      const std::map<Pin::Mode, std::string> &modes() const;

      // Optional API
      // ----------------------------------------------------------------

      // may be redefined, in this case set the flag hasToggle
      void toggle (const Pin *pin);

      // may be redefined, in this case set the flag hasPullRead
      Pin::Pull pull (const Pin *pin) const;

      // may be redefined, in this case set the flag hasDrive
      void setDrive (const Pin *pin, int d);
      int drive (const Pin *pin) const;

      // may be redefined, in this case set the flag hasWfi
      // set edge for the pin and wait for an interrupt
      // timeout_ms = -1 for infinite wait
      int waitForInterrupt (const Pin *pin, Pin::Edge edge, int timeout_ms);

      // may be redefined, in this case set the flag hasIrq
      bool attachInterrupt (const Pin *pin, Pin::Isr isr, Pin::Edge edge, void *userData);
      void detachInterrupt (const Pin *pin);

      // may be redefined, in this case set the flag hasDebounce
      void setDebounce (const Pin *pin, uint32_t debounce_ms);
      uint32_t debounce (const Pin *pin) const;

      // may be redefined, in this case set the flag hasActiveLow
      void setActiveLow (const Pin *pin, bool activeLow);
      bool isActiveLow (const Pin *pin) const;

    protected:
      // do not remove the following lines
      // they are used for the private implementation idiom
      // all private data and methods are declared in the Private class in the _p.h file
      // see https://github.com/epsilonrt/pImpl
      class Private;
      GpioTemplate (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (GpioTemplate)
  };
}
/* ========================================================================== */
