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

#include <piduino/board.h>
#include <piduino/gpio.h>

namespace Piduino {


  /**
     @class GpioDevice
     @brief Abstract base class for GPIO devices.

     This class provides an interface for GPIO device operations. It uses the
     PIMPL (Pointer to Implementation) pattern to hide implementation details.
     Derived classes should implement the pure virtual methods to provide
     specific functionality.
  */
  class GpioDevice {

    public:


      /**
         @brief Public Constructor

         Initializes a GpioDevice instance. This constructor is used by derived
         classes to initialize the base class.

         @note This class uses the PIMPL pattern to hide implementation details.
      */
      GpioDevice();

      /**
         @brief Destructor

         Cleans up resources used by the GpioDevice instance.
      */
      virtual ~GpioDevice();

      // -----------------------------------------------------------------
      //
      //                         Mandatory API
      //
      // -----------------------------------------------------------------
      /**
        @brief Opens the GPIO device.

        @return true if the device was successfully opened, false otherwise.
      */
      virtual bool open() = 0;

      /**
         @brief Closes the GPIO device.
      */
      virtual void close() = 0;

      /**
         @brief Writes a boolean value to a GPIO pin.

         @param pin Pointer to the Pin object.
         @param v Boolean value to write to the pin (true for high, false for low).
      */
      virtual void write (const Pin *pin, bool v) = 0;

      /**
         @brief Reads the value of a GPIO pin.

         @param pin Pointer to the Pin object.
         @return Boolean value representing the pin state (true for high, false for low).
      */
      virtual bool read (const Pin *pin) const = 0;

      /**
         @brief Sets the mode of a GPIO pin.

         @param pin Pointer to the Pin object.
         @param m Mode to set for the pin.
      */
      virtual void setMode (const Pin *pin, Pin::Mode m) = 0;

      /**
         @brief Gets the mode of a GPIO pin.

         @param pin Pointer to the Pin object.
         @return The current mode of the pin.
      */
      virtual Pin::Mode mode (const Pin *pin) const = 0;

      /**
         @brief Sets the pull-up/pull-down configuration of a GPIO pin.

         @param pin Pointer to the Pin object.
         @param p Pull configuration to set for the pin.
      */
      virtual void setPull (const Pin *pin, Pin::Pull p) = 0;

      /**
         @brief Gets the preferred access layer for the GPIO device.

         @return The preferred access layer.
      */
      virtual AccessLayer preferedAccessLayer() const = 0;

      /**
         @brief Gets the supported modes for the GPIO device.

         @return A map of supported modes and their corresponding string representations.
      */
      virtual const std::map<Pin::Mode, std::string> &modes() const = 0;

      // -----------------------------------------------------------------
      //
      //                           Optional API
      //
      // -----------------------------------------------------------------

      /**
         @brief Toggles the state of a GPIO pin.

         The default implementation writes the inverse of the current pin value.

         @param pin Pointer to the Pin object.
      */
      virtual void toggle (const Pin *pin);

      /**
         @brief Gets the pull-up/pull-down configuration of a GPIO pin.

         The default implementation returns PullUnknown.

         @note If reimplemented, the hasPullRead flag must be set.
         @param pin Pointer to the Pin object.
         @return The current pull configuration of the pin.
      */
      virtual Pin::Pull pull (const Pin *pin) const;

      /**
         @brief Sets the drive strength of a GPIO pin.

         The default implementation does nothing.

         @note If reimplemented, the hasDrive flag must be set.
         @param pin Pointer to the Pin object.
         @param d Drive strength to set for the pin.
      */
      virtual void setDrive (const Pin *pin, int d);

      /**
         @brief Gets the drive strength of a GPIO pin.

         The default implementation returns 0.

         @note If reimplemented, the hasDrive flag must be set.
         @param pin Pointer to the Pin object.
         @return The current drive strength of the pin.
      */
      virtual int drive (const Pin *pin) const;

      /**
         @brief Waits for an interrupt on a GPIO pin.

         The default implementation return -1.

         @note If reimplemented, the hasWfi flag must be set.
         @param pin Pointer to the Pin object.
         @param edge The edge to wait for (rising, falling, or both).
         @param timeout_ms The maximum wait time in milliseconds. -1 for infinite wait.
         @return 1 if the interrupt occurred, 0 if the timeout occurred and -1 on error.
         @note The function should block until the interrupt occurs or the timeout
      */
      virtual int waitForInterrupt (const Pin *pin, Pin::Edge edge, int timeout_ms);

      /**
         @brief Sets the debounce period for the GPIO line.
          The default implementation does nothing.

         @param pin Pointer to the Pin object.
         @param debounce_ms The debounce period in milliseconds.
      */
      virtual void setDebounce (const Pin *pin, uint32_t debounce_ms);

      /**
         @brief Gets the debounce period for the GPIO line.

         The default implementation returns 0.

         @param pin Pointer to the Pin object.
         @return The debounce period in milliseconds.
      */
      virtual uint32_t debounce (const Pin *pin) const;

      /**
         @brief Sets the active low configuration for a GPIO pin.

          This function is used to configure the pin's active low state.
          The active low state means that the pin is considered "active" when
          it is driven low (0V). This is useful for devices that are
          activated by a low signal, such as certain types of sensors or
          switches.

          The default implementation does nothing.

         @param pin Pointer to the Pin object.
         @param activeLow true to set the pin as active low, false otherwise.
         @note If reimplemented, the hasActiveLow flag must be set.
      */
      virtual void setActiveLow (const Pin *pin, bool activeLow);

      /**
         @brief Checks if a GPIO pin is configured as active low.

         This function checks the active low configuration of the specified
         GPIO pin. If the pin is configured as active low, it means that the
         pin is considered "active" when it is driven low (0V).

         The default implementation returns false.

         @param pin Pointer to the Pin object.
         @return true if the pin is active low, false otherwise.
      */
      virtual bool isActiveLow (const Pin *pin) const;

      /**
         @brief Gets the system number offset for the GPIO device.

         The default implementation returns 0.

         @return The system number offset.
      */
      virtual int systemNumberOffset() const;

      /**
         @enum Flags
         @brief Flags indicating the capabilities of the GPIO device.

         - hasToggle: The device supports toggling pin states.
         - hasPullRead: The device supports reading pull-up/pull-down configuration.
         - hasAltRead: The device supports reading alternate functions.
         - hasDrive: The device supports setting drive strength.
         - hasWfi: The device supports waiting for interrupts.
         - hasActiveLow: The device supports active low configuration.
         - hasDebounce: The device supports debounce configuration.
      */
      enum {
        hasToggle     = 0x0001,
        hasPullRead   = 0x0002,
        hasAltRead    = 0x0004,
        hasDrive      = 0x0008,
        hasWfi        = 0x0010,
        hasActiveLow  = 0x0020,
        hasDebounce   = 0x0040
      };

      /**
         @brief Gets the capability flags of the GPIO device.

         @return The capability flags as a bitwise OR of the Flags enum values.
         @note The default implementation returns 0.
      */
      virtual unsigned int flags() const;

      /**
         @brief Checks if the GPIO device is open.

         @return true if the device is open, false otherwise.
      */
      virtual bool isOpen() const;

      /**
         @brief Checks if the GPIO device is in debug mode.

         @return true if the device is in debug mode, false otherwise.
      */
      virtual bool isDebug() const;

      /**
         @brief Sets the debug mode of the GPIO device.

         @param enable true to enable debug mode, false to disable it.
      */
      virtual void setDebug (bool enable);

    protected:
      /**
         @class Private
         @brief Internal implementation class for GpioDevice.

         This class is used to hide implementation details using the PIMPL pattern.
      */
      class Private;

      /**
         @brief Protected Constructor

         Initializes a GpioDevice instance with a private implementation.

         @param dd Reference to the private implementation object.
      */
      GpioDevice (Private &dd);

      /**
         @brief A unique pointer to the private implementation (PIMPL idiom).

         This member variable is used to encapsulate the private details of the
         class implementation, ensuring a clean separation between the interface
         and implementation. It provides automatic memory management for the
         private data, ensuring proper cleanup when the owning object is destroyed.
      */
      std::unique_ptr<Private> d_ptr;

    private:
      /**
         @brief Macro for declaring private implementation.
      */
      PIMP_DECLARE_PRIVATE (GpioDevice)
  };
}
/* ========================================================================== */
