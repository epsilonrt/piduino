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

#include <string>
#include <map>
#include <piduino/converter.h>
#include <piduino/gpio2.h>


namespace Piduino {

  class Gpio;
  class GpioDevice;
  class Connector;

  /**
    @enum AccessLayer
    @brief Enumerates the possible hardware access layers for GPIO operations.

    The AccessLayer enum defines the different mechanisms by which a GPIO pin can be accessed
    and controlled at the system level. These layers represent various interfaces provided by
    the operating system or hardware abstraction, allowing flexible and platform-specific
    access to GPIO functionality.

    - AccessLayerAuto:      Automatically select the most appropriate access layer.
    - AccessLayerIoMap:     Use memory-mapped I/O for direct hardware access.
    - AccessLayerSysFs:     Use the sysfs interface provided by the Linux kernel.
    - AccessLayerGpioDev:   Use the /dev/gpiochip character device interface.
    - AccessLayerAll:       Enable all supported access layers.
    - AccessLayerUnk:       Unknown or uninitialized access layer.
  */
  enum AccessLayer {
    AccessLayerAuto  = 0,
    AccessLayerIoMap = 0x0001,
    AccessLayerSysFs = 0x0002,
    AccessLayerGpioDev = 0x0004,
    AccessLayerAll = AccessLayerIoMap + AccessLayerSysFs,
    AccessLayerUnk = -1
  };

  /**
     @class Pin
     @brief Represents a general-purpose input/output (GPIO) pin and its configuration.

     The Pin class provides an abstraction for a GPIO pin, allowing configuration of its mode,
     pull-up/down resistors, edge detection, and other properties. It supports various numbering
     schemes and pin types, and provides methods for digital and analog I/O, interrupt handling,
     and device management.

     @note This class is intended to be used with the Connector and Gpio classes for hardware access.
  */
  class Pin {
    public:
      friend class Connector; ///< Allows Connector to access protected members of Pin.

      /**
         @enum Mode
         @brief Enumerates the possible modes for a pin.
      */
      enum Mode {
        ModeInput,      ///< Pin is configured as input.
        ModeOutput,     ///< Pin is configured as output.
        ModeDisabled,   ///< Pin is disabled.
        ModePwm,        ///< Pin is configured for PWM output.
        ModeAlt0,       ///< Alternate function 0.
        ModeAlt1,       ///< Alternate function 1.
        ModeAlt2,       ///< Alternate function 2.
        ModeAlt3,       ///< Alternate function 3.
        ModeAlt4,       ///< Alternate function 4.
        ModeAlt5,       ///< Alternate function 5.
        ModeAlt6,       ///< Alternate function 6.
        ModeAlt7,       ///< Alternate function 7.
        ModeAlt8,       ///< Alternate function 8.
        ModeAlt9,       ///< Alternate function 9.
        ModeUnknown = -1///< Unknown or uninitialized mode.
      };

      /**
         @enum Pull
         @brief Enumerates the possible pull resistor configurations.
      */
      enum Pull {
        PullOff,        ///< No pull resistor.
        PullDown,       ///< Pull-down resistor enabled.
        PullUp,         ///< Pull-up resistor enabled.
        PullUnknown = -1///< Unknown or uninitialized pull configuration.
      };

      /**
         @enum Edge
         @brief Enumerates the possible edge detection types for interrupts.
      */
      enum Edge {
        EdgeNone,       ///< No edge detection.
        EdgeRising,     ///< Detect rising edge.
        EdgeFalling,    ///< Detect falling edge.
        EdgeBoth,       ///< Detect both rising and falling edges.
        EdgeUnknown = -1///< Unknown or uninitialized edge type.
      };

      /**
         @enum Numbering
         @brief Enumerates the possible pin numbering schemes.
      */
      enum Numbering {
        NumberingLogical,   ///< Logical numbering.
        NumberingMcu,       ///< MCU-specific numbering.
        NumberingSystem,    ///< System numbering.
        NumberingUnknown = -1///< Unknown or uninitialized numbering.
      };

      /**
         @enum Type
         @brief Enumerates the possible types of pins.
      */
      enum Type {
        TypeGpio,           ///< General-purpose I/O pin.
        TypePower,          ///< Power pin.
        TypeUsb,            ///< USB pin.
        TypeSound,          ///< Sound pin.
        TypeVideo,          ///< Video pin.
        TypeNotConnected,   ///< Not connected.
        TypeNet,            ///< Network pin.
        TypeUnknown = -1    ///< Unknown or uninitialized type.
      };

      /**
         @class Number
         @brief Holds various numbering representations for a pin.
      */
      class Number {
        public:
          int logical;    ///< Logical pin number.
          int mcu;        ///< MCU-specific pin number.
          int system;     ///< System pin number.
          int row;        ///< Row position (if applicable).
          int column;     ///< Column position (if applicable).
          int chip;       ///< Chip number (for multi-chip systems).
          int offset;     ///< Offset within the chip.
          /**
             @brief Constructs a Number object with optional parameters.
          */
          Number (int logical = -1, int mcu = -1, int system = -1,
                  int row = -1, int column = -1, int chip = -1, int offset = -1)
            : logical (logical), mcu (mcu), system (system), row (row),
              column (column), chip (chip), offset (offset) {}

      };

      /**
         @class Descriptor
         @brief Describes a pin's type, numbering, and associated names.
      */
      class Descriptor {
        public:
          Type type;                          ///< Pin type.
          Number num;                         ///< Pin numbering information.
          long long id;                       ///< Unique identifier for the pin.
          std::map<Mode, std::string> name;   ///< Map of mode to pin name.
          /**
             @brief Constructs a Descriptor with optional parameters.
          */
          Descriptor (long long pinId = -1, int pinRow = -1, int pinColumn = -1);
          /**
             @brief Inserts the descriptor into the system.
             @return True on success, false otherwise.
          */
          bool insert();
          /**
             @brief Checks if a mode name exists for a given mode and name ID.
             @param m The mode to check.
             @param nameId The name identifier.
             @return True if the mode name exists, false otherwise.
          */
          bool hasModeName (Mode m, long long nameId) const;
          /**
             @brief Finds the unique identifier for the pin.
             @return The pin's unique identifier.
          */
          long long findId() const;
          /**
             @brief Finds the identifier for a given name.
             @param name The name to search for.
             @return The identifier associated with the name.
          */
          long long findName (const std::string &name) const;
        private:
          /**
             @brief Inserts a mode-name pair into the descriptor.
             @param m The mode.
             @param n The name.
          */
          void insertModeName (Mode m, const std::string &n);
      };

      /**
         @class SpiCs
         @brief Represents a SPI chip select configuration for a pin.
      */
      class SpiCs {
        public:
          long long pin;   ///< Pin identifier.
          int bus;         ///< SPI bus number.
          int cs;          ///< Chip select number.
          Pin::Mode mode;  ///< Pin mode for SPI.
      };

      /**
         @typedef Event
         @brief Alias for Gpio2::LineEvent, representing a GPIO event.
      */
      using Event = Gpio2::LineEvent;

      /**
         @brief Returns the access layer used by the pin.
         @return The access layer.
      */
      AccessLayer accessLayer() const;

      /**
         @brief Checks if the pin is open.
         @return True if open, false otherwise.
      */
      bool isOpen() const;

      /**
         @brief Returns the pin number according to the specified numbering scheme.
         @param numbering The numbering scheme.
         @return The pin number.
      */
      int number (Numbering numbering) const;

      /**
         @brief Returns the map of available numbering schemes and their names.
         @return Reference to the numbering map.
      */
      static const std::map<Numbering, std::string> &numberings();

      /**
         @brief Returns the name of a numbering scheme.
         @param n The numbering scheme.
         @return Reference to the numbering name.
      */
      static const std::string &numberingName (Numbering n);

      /**
         @brief Returns the logical pin number.
         @return The logical number.
      */
      int logicalNumber() const;

      /**
         @brief Returns the MCU-specific pin number.
         @return The MCU number.
      */
      int mcuNumber() const;

      /**
         @brief Returns the system pin number.
         @return The system number.
      */
      int systemNumber() const;

      /**
         @brief Returns the physical pin number.
         @return The physical number.
      */
      int physicalNumber() const;

      /**
         @brief Returns the chip number for the pin.
         @return The chip number.
      */
      int chipNumber() const;

      /**
         @brief Returns the offset within the chip.
         @return The chip offset.
      */
      int chipOffset() const;

      /**
         @brief Returns the row position of the pin.
         @return The row.
      */
      int row() const;

      /**
         @brief Returns the column position of the pin.
         @return The column.
      */
      int column() const;

      /**
         @brief Returns the type of the pin.
         @return The pin type.
      */
      Type type() const;

      /**
         @brief Returns the name of the pin type.
         @return Reference to the type name.
      */
      const std::string &typeName() const;

      /**
         @brief Returns the name of a given pin type.
         @param t The pin type.
         @return Reference to the type name.
      */
      static const std::string &typeName (Type t);

      /**
         @brief Returns the map of available pin types and their names.
         @return Reference to the type map.
      */
      static const std::map<Type, std::string> &types();

      /**
         @brief Returns the default name of the pin.
         @return Reference to the pin name.
      */
      const std::string &name() const;

      /**
         @brief Returns the name of the pin for a specific mode.
         @param mode The mode.
         @return Reference to the mode-specific name.
      */
      const std::string &name (Mode mode) const;

      /**
         @brief Sets the mode of the pin.
         @param mode The mode to set.
      */
      void setMode (Mode mode);

      /**
         @brief Returns the current mode of the pin.
         @return The pin mode.
      */
      Mode mode() const;

      /**
         @brief Returns the map of available modes and their names.
         @return Reference to the mode map.
      */
      const std::map<Mode, std::string> &modes() const;

      /**
         @brief Returns the name of the current mode.
         @return Reference to the mode name.
      */
      const std::string &modeName();

      /**
         @brief Returns the name of a specific mode.
         @param mode The mode.
         @return Reference to the mode name.
      */
      const std::string &modeName (Mode mode) const;

      /**
         @brief Sets the pull resistor configuration.
         @param pull The pull configuration.
      */
      void setPull (Pull pull);

      /**
         @brief Returns the current pull resistor configuration.
         @return The pull configuration.
      */
      Pull pull() const;

      /**
         @brief Returns the map of available pull configurations and their names.
         @return Reference to the pull map.
      */
      static const std::map<Pull, std::string> &pulls();

      /**
         @brief Returns the name of the current pull configuration.
         @return Reference to the pull name.
      */
      const std::string &pullName();

      /**
         @brief Returns the name of a specific pull configuration.
         @param n The pull configuration.
         @return Reference to the pull name.
      */
      static const std::string &pullName (Pull n);

      /**
         @brief Returns the drive strength of the pin.
         @return The drive strength.
      */
      int drive();

      /**
         @brief Sets the drive strength of the pin.
         @param drive The drive strength.
      */
      void setDrive (int drive);

      /**
         @brief Writes a digital value to the pin.
         @param value The value to write (true for high, false for low).
      */
      void write (bool value);

      /**
         @brief Reads the digital value from the pin.
         @return The value read (true for high, false for low).
      */
      bool read() const;

      /**
         @brief Toggles the digital output value of the pin.
      */
      void toggle();

      /**
         @brief Releases the pin, closing any open resources.
      */
      void release();

      /**
         @brief Waits for an interrupt event on the pin.
         @param edge The edge to detect.
         @param timeout_ms Timeout in milliseconds (-1 for infinite).
      */
      void waitForInterrupt (Edge edge, int timeout_ms = -1);

      /**
         @brief Waits for an interrupt event and returns the event data.
         @param edge The edge to detect.
         @param event Reference to store the event data.
         @param timeout_ms Timeout in milliseconds (-1 for infinite).
      */
      void waitForInterrupt (Edge edge, Event &event, int timeout_ms = -1);

      /**
         @brief Waits for an interrupt event with debounce and returns the event data.
         @param edge The edge to detect.
         @param debounce_ms Debounce time in milliseconds.
         @param event Reference to store the event data.
         @param timeout_ms Timeout in milliseconds (-1 for infinite).
      */
      void waitForInterrupt (Edge edge, int debounce_ms, Event &event, int timeout_ms = -1);

      /**
         @typedef Isr
         @brief Interrupt service routine callback type.
         @param event The GPIO event.
         @param userData User data pointer.
      */
      typedef void (*Isr) (Event event, void *userData);

      /**
         @brief Attaches an interrupt handler to the pin.
         @param isr The interrupt service routine.
         @param edge The edge to detect.
         @param userData Optional user data pointer.
      */
      void attachInterrupt (Isr isr, Edge edge, void *userData = nullptr);

      /**
         @brief Attaches an interrupt handler with debounce.
         @param isr The interrupt service routine.
         @param edge The edge to detect.
         @param debounce_ms Debounce time in milliseconds.
         @param userData Optional user data pointer.
      */
      void attachInterrupt (Isr isr, Edge edge, int debounce_ms, void *userData = nullptr);

      /**
         @brief Detaches the interrupt handler from the pin.
      */
      void detachInterrupt();

      /**
         @brief Checks if the GPIO device is enabled.
         @return True if enabled, false otherwise.
      */
      bool isGpioDevEnabled() const;

      /**
         @brief Enables or disables the GPIO device.
         @param enable True to enable, false to disable.
         @return True on success, false otherwise.
      */
      bool enableGpioDev (bool enable = true);

      /**
         @brief Writes an analog value to the pin (PWM or DAC).
         @param value The analog value.
      */
      void analogWrite (long value);

      /**
         @brief Returns the DAC converter associated with the pin.
         @return Pointer to the DAC converter.
      */
      Converter *dac();

      /**
         @brief Sets the DAC converter for the pin.
         @param dac Pointer to the DAC converter.
         @return True on success, false otherwise.
      */
      bool setDac (Converter *dac);

      /**
         @brief Resets the DAC converter for the pin.
      */
      void resetDac();

      /**
         @brief Returns the unique identifier of the pin.
         @return The pin ID.
      */
      long long id() const;

      /**
         @brief Returns the parent connector of the pin.
         @return Pointer to the Connector.
      */
      Connector *connector() const;

      /**
         @brief Returns the GPIO object associated with the pin.
         @return Pointer to the Gpio object.
      */
      Gpio *gpio() const;

      /**
         @brief Checks if debug mode is enabled for the pin.
         @return True if debug is enabled, false otherwise.
      */
      bool isDebug() const;

      /**
         @brief Enables or disables debug mode for the pin.
         @param enable True to enable, false to disable.
      */
      void setDebug (bool enable);

      /**
         @brief Constructs a Pin object.
         @param parent Pointer to the parent Connector.
         @param desc Pointer to the pin Descriptor.
      */
      Pin (Connector *parent, const Descriptor *desc);

      /**
         @brief Destructor for the Pin object.
      */
      virtual ~Pin();

    protected:
      /**
         @brief Returns the underlying GPIO device.
         @return Pointer to the GpioDevice.
      */
      GpioDevice *device() const;

      /**
         @brief Opens the pin for use.
         @return True on success, false otherwise.
      */
      bool open();

      /**
         @brief Closes the pin and releases resources.
      */
      void close();

    protected:
      /**
         @class Private
         @brief Opaque private data class for Pin implementation.
      */
      class Private;

      /**
         @brief Constructs a Pin object from a Private data reference.
         @param dd Reference to the Private data.
      */
      Pin (Private &dd);

      /**
         @brief Unique pointer to the private implementation.
      */
      std::unique_ptr<Private> d_ptr;

    private:
      /**
         @brief Macro for private implementation declaration.
      */
      PIMP_DECLARE_PRIVATE (Pin)
  };
}

/* ========================================================================== */
