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

#include <piduino/converter.h>
#include <piduino/i2cdev.h>

namespace Piduino {

  /**
     @brief I2C GPIO expander controller for the MAX7311 device.

     The MAX7311 is a 16-port I2C GPIO expander that provides additional digital I/O pins
     for microcontroller systems. This class provides a high-level interface for configuring
     and controlling the MAX7311 device, including pin mode setting, digital read/write
     operations, and bus timeout management.

     The device supports addresses from 0x20 to 0xDE and operates on standard I2C buses.
     Each of the 16 ports can be individually configured as input or output with optional
     polarity inversion and interrupt generation capabilities.
  */
  class Max7311 : public Converter {

    public:
      /**
         @brief Constructs a Max7311 object for the specified I2C bus and device address.

         Creates a new MAX7311 controller instance connected to the specified I2C bus
         with the given device address. The device must be properly connected and powered
         before attempting communication.

         @param busId The I2C bus identifier (default: system default bus)
         @param address The 7-bit I2C device address (default: 0x20, range: 0x20-0xDE)
      */
      Max7311 (int busId = I2cDev::Info::defaultBus().id(), int address = 0x20);

      /**
         @brief Virtual destructor that properly cleans up the Max7311 object.

         Ensures proper cleanup of I2C resources and resets the device state
         when the object is destroyed.
      */
      virtual ~Max7311();

      /**
         @brief Constructs a Max7311 object from a parameter string.

         Creates a MAX7311 controller instance using configuration parameters
         provided as a formatted string. This constructor is primarily used
         by the converter factory system for dynamic object creation.

         @param parameters Configuration string formatted as "bus=id:addr=0x20:bustimeout=1"
                           - bus: I2C bus number (optional, default: system default)
                           - addr: Device I2C address in hex (optional, default: 0x20)
                           - bustimeout: Enable/disable bus timeout (optional, default: true)

         @note This constructor is used for factory registration and dynamic instantiation.
      */
      Max7311 (const std::string &parameters);

      /**
         @brief Returns the registered name for this converter type.

         Provides the string identifier used by the converter factory system
         to create instances of this converter type. This name is used in
         configuration files and command-line interfaces.

         @return The converter type name "max7311"

         @note This method is required for factory registration and must return a unique name.
      */
      static std::string registeredName() {
        return "max7311";
      }


      /**
         @brief Gets the current bus timeout setting.

         Returns whether the I2C bus timeout feature is currently enabled.
         When enabled, I2C operations will timeout after a predetermined period
         to prevent system lockup in case of bus communication issues.

         @return true if bus timeout is enabled, false otherwise
      */
      bool busTimeout() const;

      /**
         @brief Sets the bus timeout enable/disable state.

         Configures whether I2C bus operations should use timeout protection.
         Enabling timeout prevents the system from hanging indefinitely if
         the I2C bus becomes unresponsive or if there are hardware issues.

         @param enable true to enable bus timeout, false to disable
         @return true if the setting was successfully applied, false on error
      */
      bool setBusTimeout (bool enable);

    protected:
      /**
         @brief Forward declaration of the private implementation class.

         The Private class contains the actual implementation details and
         internal state management for the MAX7311 controller, following
         the PIMPL (Pointer to Implementation) design pattern.
      */
      class Private;

      /**
         @brief Protected constructor for internal use with PIMPL pattern.

         This constructor is used internally by the class hierarchy to create
         instances with a specific private implementation object. It supports
         the PIMPL design pattern used throughout the Piduino library.

         @param dd Reference to the private implementation object
      */
      Max7311 (Private &dd);

    private:
      /**
         @brief PIMPL pattern declaration macro for private implementation.

         This macro declares the necessary infrastructure for the Pointer to
         Implementation design pattern, providing clean separation between
         public interface and private implementation details.
      */
      PIMP_DECLARE_PRIVATE (Max7311)
  };
} // namespace Piduino
/* ========================================================================== */
