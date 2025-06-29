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

#include <piduino/iodevice.h>
#include <piduino/gpiopin.h>


namespace Piduino {

  /**
     @class GpioDev2
     @brief Represents a GPIO device abstraction for pin control and event handling.

     The GpioDev2 class provides an interface for configuring and controlling a GPIO pin,
     including setting its mode, reading and writing values, configuring pull-up/down resistors,
     handling interrupts, and debouncing. It is designed to work with the Piduino framework.
  */
  class GpioDev2 : public IoDevice {
    public:
      /**
         @brief Constructs a GpioDev2 object for the specified pin.
         @param pin Reference to the Pin object to be managed.
      */
      GpioDev2 (Pin &pin);

      /**
         @brief Destructor for GpioDev2.
      */
      virtual ~GpioDev2();

      /**
         @brief Opens the GPIO device with the specified mode.
         @param mode The open mode (default is IoDevice::ReadWrite).
         @return True if the device was successfully opened, false otherwise.
      */
      bool open (IoDevice::OpenMode mode = IoDevice::ReadWrite);

      /**
         @brief Sets the mode of the GPIO pin (input, output, etc.).
         @param m The desired pin mode.
      */
      void setMode (Pin::Mode m);

      /**
         @brief Gets the current mode of the GPIO pin.
         @return The current pin mode.
      */
      Pin::Mode mode() const;

      /**
         @brief Writes a boolean value to the GPIO pin.
         @param v The value to write (true for high, false for low).
      */
      void write (bool v);

      /**
         @brief Reads the current value of the GPIO pin.
         @return The value read (true for high, false for low).
      */
      bool read() const;

      /**
         @brief Sets the pull-up or pull-down resistor configuration for the pin.
         @param p The desired pull configuration.
      */
      void setPull (Pin::Pull p);

      /**
         @brief Gets the current pull-up or pull-down configuration.
         @return The current pull configuration.
      */
      Pin::Pull pull() const;

      /**
         @brief Waits for an interrupt event on the pin.
         @param edge The edge type to wait for (rising, falling, etc.).
         @param event Reference to a Pin::Event object to receive event details.
         @param timeout_ms Timeout in milliseconds (-1 for infinite).
         @return True if an event occurred, false if timed out or an error occurred (use error() to check).
      */
      bool waitForInterrupt (Pin::Edge edge, Pin::Event &event, int timeout_ms = -1);

      /**
         @brief Attaches an interrupt service routine to the pin.
         @param isr The ISR callback function.
         @param edge The edge type to trigger the interrupt.
         @param userData Optional user data to pass to the ISR.
         @return True if the ISR was successfully attached, false otherwise.
      */
      bool attachInterrupt (Pin::Isr isr, Pin::Edge edge, void *userData = nullptr);

      /**
         @brief Detaches the interrupt service routine from the pin.
      */
      void detachInterrupt();

      /**
         @brief Sets the debounce time for the pin in milliseconds.
         @param debounce_ms The debounce duration in milliseconds.
      */
      void setDebounce (uint32_t debounce_ms);

      /**
         @brief Gets the current debounce time in milliseconds.
         @return The debounce duration in milliseconds.
      */
      uint32_t debounce() const;

      /**
         @brief Gets a reference to the underlying GPIO chip.
         @return Reference to the Gpio2::Chip object.
      */
      Gpio2::Chip &chip() const;

      /**
         @brief Gets a reference to the underlying GPIO line.
         @return Reference to the Gpio2::Line object.
      */
      Gpio2::Line &line() const;

      /**
         @brief Gets a reference to the managed Pin object.
         @return Reference to the Pin object.
      */
      Pin &pin() const;

    protected:
      /**
         @brief Private implementation class for GpioDev2.
         This class encapsulates the private data and methods for GpioDev2.
      */
      class Private;
      /**
         @brief Protected constructor for internal use with a Private implementation.
         @param dd Reference to the Private implementation object.
      */
      GpioDev2 (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (GpioDev2)
  };
}
/* ========================================================================== */
