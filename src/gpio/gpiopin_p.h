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

  /**
     @class Pin::Private
     @brief Internal implementation class for Pin, encapsulating GPIO pin state and operations.

     This class manages the internal state and operations of a GPIO pin, including its mode,
     pull configuration, drive strength, and interaction with the underlying GPIO device.
     It is intended for use only within the Pin class (PIMPL idiom).
  */
  class Pin::Private {
    public:
      /**
         @brief Constructs a Private implementation for a Pin.
         @param q Pointer to the owning Pin object.
         @param parent Pointer to the parent Connector.
         @param desc Pointer to the pin Descriptor.
      */
      Private (Pin *q, Connector *parent, const Descriptor *desc);

      /**
         @brief Virtual destructor.
      */
      virtual ~Private() = default;

      /**
         @brief Sets the pin mode to the previously held mode.
      */
      void setHoldMode();

      /**
         @brief Sets the pin pull configuration to the previously held pull.
      */
      void setHoldPull();

      /**
         @brief Reads the current pull configuration from the hardware.
      */
      void readPull();

      /**
         @brief Writes the current pull configuration to the hardware.
      */
      void writePull();

      /**
         @brief Reads the current mode from the hardware.
      */
      void readMode();

      /**
         @brief Writes the current mode to the hardware.
      */
      void writeMode();

      /**
         @brief Reads the current drive strength from the hardware.
      */
      void readDrive();

      /**
         @brief Writes the current drive strength to the hardware.
      */
      void writeDrive();

      /**
         @brief Checks if the GPIO device is enabled.
         @return True if the GPIO device is enabled, false otherwise.
      */
      bool isGpioDevEnabled() const{

        return gpiodev != nullptr;
      }

      /**
         @brief Checks if the GPIO device is open.
         @return True if the GPIO device is open, false otherwise.
      */
      bool isGpioDevOpen() const{

        if (isGpioDevEnabled()) {

          return gpiodev->isOpen();
        }
        return false;
      }

      /**
         @brief Enables or disables the GPIO device.
         @param enable True to enable, false to disable.
         @return True if the GPIO device is enabled after the operation, false otherwise.
      */
      bool enableGpioDev (bool enable = true){

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
      Pin *const q_ptr;                  ///< Pointer to the owning Pin object.
      bool isopen;                       ///< Indicates if the pin is open.
      Connector *parent;                 ///< Pointer to the parent Connector.
      const Descriptor *descriptor;      ///< Pointer to the pin Descriptor.
      Mode holdMode;                     ///< Previously held mode for restoration.
      Pull holdPull;                     ///< Previously held pull configuration for restoration.
      bool holdState;                    ///< Previously held state.
      Mode mode;                         ///< Current mode of the pin.
      Pull pull;                         ///< Current pull configuration of the pin.
      std::shared_ptr<Converter> dac;    ///< Shared pointer to DAC converter, if applicable.
      int drive;                         ///< Current drive strength.
      std::unique_ptr<GpioDev2> gpiodev; ///< Unique pointer to the GPIO device implementation.

      static const std::map<Pull, std::string> pulls;           ///< Mapping of Pull enum to string.
      static const std::map<Type, std::string> types;           ///< Mapping of Type enum to string.
      static const std::map<Numbering, std::string> numberings; ///< Mapping of Numbering enum to string.

      PIMP_DECLARE_PUBLIC (Pin)
  };
  // End of Pin::Private class
}