/* Copyright © 2018 Pascal JEAN, All rights reserved.
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
#include <memory>
#include <piduino/gpiopin.h>
#include "gpio_dev2.h"

namespace Piduino {

  class Pin::Private {
    public:
      Private (Pin *q, Connector *parent, const Descriptor *desc);
      virtual ~Private () = default;

      void setHoldMode();
      void setHoldPull();

      void readPull ();
      void writePull();
      void readMode();
      void writeMode ();
      void readDrive ();
      void writeDrive();

      // ----------------------------------------------------------------------
      bool isGpioDevEnabled() const {

        return gpiodev != nullptr;
      }

      // ----------------------------------------------------------------------
      bool isGpioDevOpen() const {

        if (isGpioDevEnabled()) {

          return gpiodev->isOpen();
        }
        return false;
      }

      // ----------------------------------------------------------------------
      bool enableGpioDev (bool enable = true) {

        if (enable && !gpiodev && (descriptor->type == TypeGpio)) {

          gpiodev = std::make_unique<GpioDev2> (*q_ptr);
          if (isopen) {

            setHoldMode();
            setHoldPull();
            if (!gpiodev->open()) {

              gpiodev.reset(); // failed to open, reset the pointer
            }
          }
        }
        else if (!enable && gpiodev) {

          gpiodev.reset(); // call the destructor that closes the device
        }
        return gpiodev != nullptr;
      }

    public:
      Pin *const q_ptr;
      bool isopen;
      Connector *parent;
      const Descriptor *descriptor;
      Mode holdMode;
      Pull holdPull;
      bool holdState;

      Mode mode;
      Pull pull;

      std::shared_ptr<Converter> dac;
      int drive;

      std::unique_ptr<GpioDev2> gpiodev;


      static const std::map<Pull, std::string> pulls;
      static const std::map<Type, std::string> types;
      static const std::map<Numbering, std::string> numberings;

      PIMP_DECLARE_PUBLIC (Pin)
  };
}