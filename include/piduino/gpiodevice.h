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

#ifndef _PIDUINO_GPIO_DEVICE_H_
#define _PIDUINO_GPIO_DEVICE_H_

#include <piduino/gpio.h>

#ifndef __DOXYGEN__

namespace Piduino {

  /*
   * @class Device
   * @author Pascal JEAN
   * @date 02/22/18
   * @brief Accès à la couche matérielle du GPIO
   *
   * Classe abstraite devant être implémentée sur la plateforme cible.
   */
  class Device {

  public:
      enum {
        hasToggle   = 0x0001,
        hasPullRead = 0x0002,
        hasAltRead  = 0x0004
      };
      
      Device();
      virtual ~Device();
      virtual bool isOpen() const;
      virtual bool isDebug() const;
      virtual void setDebug (bool enable);
      virtual unsigned int flags() const;

      // Méthodes purement virtuelles
      virtual const Gpio::Descriptor * descriptor() const = 0;

      virtual bool open() = 0;
      virtual void close() = 0;
      virtual AccessLayer preferedAccessLayer() const = 0;

      virtual void setMode (const Pin * pin, Pin::Mode m) = 0;
      virtual void setPull (const Pin * pin, Pin::Pull p) = 0;
      virtual void write (const Pin * pin, bool v) = 0;
      virtual void toggle (const Pin * pin);

      virtual bool read (const Pin * pin) const = 0;
      virtual Pin::Mode mode (const Pin * pin) const = 0;
      virtual Pin::Pull pull (const Pin * pin) const;

      virtual const std::map<Pin::Mode, std::string> & modes() const = 0;

    protected:
      virtual void setOpen (bool open);

    private:
      bool _isopen;
      bool _isdebug;
  };
}
#endif /* DOXYGEN not defined */
/* ========================================================================== */
#endif /*_PIDUINO_GPIO_DEVICE_H_ defined */
