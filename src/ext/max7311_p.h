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

#include <piduino/max7311.h>
#include "converter_p.h"

namespace Piduino {

  /**
     @class Max7311::Private
     @brief Private implementation class for Max7311.

     This class provides the private implementation for the Max7311 device, inheriting from Converter::Private.
     It manages the communication with the MAX7311 I2C I/O expander, providing methods for opening, closing,
     reading, writing, and configuring the device. The class supports multiple channels and exposes device-specific
     properties such as resolution, range, and operational modes.

     @note This class is intended for internal use within the Max7311 implementation and should not be used directly.
  */
  class Max7311::Private  : public Converter::Private {

    public:
      /**
         @brief Constructs a Private implementation object for Max7311.
         @param q Pointer to the parent Max7311 object.
         @param dev Shared pointer to the I2C device used for communication.
         @param address I2C address of the MAX7311 device.
      */
      Private (Max7311 *q, std::shared_ptr<I2cDev> dev, int address);

      /**
         @brief Constructs a Private implementation object for Max7311 using parameter string.
         @param q Pointer to the parent Max7311 object.
         @param parameters String containing initialization parameters.
      */
      Private (Max7311 *q, const std::string &parameters);

      /**
         @brief Destructor for the Private implementation object.
      */
      ~Private() = default;

      /**
          @brief Returns the name of the device.
          @return The device name as a string, the same as Max7311::registeredName().
      */
      virtual const std::string &deviceName() const override {
        static const std::string name = Max7311::registeredName();
        return name;
      }

      /**
         @brief Opens the converter device with the specified mode.
         @param mode The mode in which to open the device.
         @return True if the device was successfully opened, false otherwise.
         @note must be overridden by subclasses to implement specific opening logic.
      */
      virtual bool open (OpenMode mode) override;

      /**
         @brief Closes the converter device.
      */
      virtual void close() override;

      /**
         @brief Reads a value from the converter device.
         @return The value read from the device, default implementation returns InvalidValue.
         @note may be overridden by subclasses to implement specific reading logic.
         This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long read () override;

      /**
         @brief Reads a value from the converter device for a specific channel.
         @param channel The channel to read from.
         @param differential If true, reads in differential mode (default is false).
         @return The value read from the device, default implementation returns InvalidValue.
         @note may be overridden by subclasses to implement specific reading logic.
         This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long readChannel (int channel = 0, bool differential = false) override;

      /**
         @brief Writes a value to the converter device.
         @param value The value to write, this value will be clamped to the valid range.
         @return True if the value was successfully written, false otherwise.
         @note may be overridden by subclasses to implement specific writing logic.
         This function is not callable if the open mode is not WriteOnly or ReadWrite (or closed).
      */
      virtual bool write (long value) override;

      /**
         @brief Writes a value to the converter device for a specific channel.
         @param value The value to write, this value will be clamped to the valid range.
         @param channel The channel to write to.
         @param differential If true, writes in differential mode (default is false).
         @return True if the value was successfully written, false otherwise.
         @note may be overridden by subclasses to implement specific writing logic.
         This function is not callable if the open mode is not WriteOnly or ReadWrite (or closed).
      */
      virtual bool writeChannel (long value, int channel = 0, bool differential = false) override;

      /**
         @brief Returns the current mode of the converter.
         @param channel The channel number (default is 0).
         @return The current mode of the converter as a Mode object, the default channel implementation returns NoMode.
      */
      virtual Mode mode (int channel = 0) const override;

      /**
         @brief Sets the mode of the converter.
         @param m The mode to set.
         @param channel The channel number (default is -1, indicating all channels).
         @return True if the mode was successfully set, false otherwise. The default implementation returns false.
         @note This function may be overridden by subclasses to implement specific mode setting logic.
      */
      virtual bool setMode (Mode m, int channel = -1) override;

      /**
         @brief Returns the number of channels supported by the converter.
         @return The number of channels, a channel is numbering from 0 to numberOfChannels() - 1.
         @note Default implementation returns 16, indicating 16 channels.
      */
      virtual int numberOfChannels() const override {
        return 16; // Default implementation returns 16, indicating 16 channels
      }

      /**
         @brief Checks if the converter is enabled.
         @return true if enabled, false otherwise.
      */
      virtual bool isEnabled () const override {
        return isConnected;
      }

      /**
         @brief Gets the digital range of the converter.
         @return The range value in LSB, e.g. max() - min() + 1.
         @note must be implemented by subclasses to return the actual range value.
      */
      virtual long range() const {
        return 1 << 16; // Default implementation returns 65536, indicating a 16-bit range
      }

      /**
         @brief Returns the maximum value supported by the converter.
         @param differential If true, returns the maximum value for differential mode.
         @return The maximum value.
         @note may be implemented by subclasses to return the actual maximum value.
      */
      virtual long max (bool differential = false) const override {
        return range() - 1; // For unipolar, max is the full range minus one
      }

      /**
         @brief Returns the minimum value supported by the converter.
         @param differential If true, returns the minimum value for differential mode.
         @return The minimum value.
         @note must be implemented by subclasses to return the actual minimum value.
      */
      virtual long min (bool differential = false) const override {
        return 0; // For unipolar, min is always 0
      }

      /**
         @brief Returns the resolution of the converter in bits.
         @return The resolution in bits.
      */
      virtual int resolution() const override {
        return 16;
      }

      // ------------------------- internal methods -------------------------
      /**
         @brief Reads a register from the MAX7311 device.
         @param reg The register address to read from.
         @param buffer Pointer to the buffer where the read data will be stored.
         @param max Maximum number of bytes to read, reg + max must be less than 8 (Timeout is the last register).
         @return The number of bytes read, or -1 on error.
         @note This function reads data from the specified register and stores it in the provided buffer.
               It is used internally to access the device's registers.
      */
      int readRegister (uint8_t reg, uint8_t *buffer, uint16_t max) const;

      /**
         @brief Reads the input register from the MAX7311 device.
         @param buffer Pointer to the buffer where the read data will be stored.
         @param max Maximum number of bytes to read (max: 2)
         @return The number of bytes read, or -1 on error.
         @note This function reads data from the input register and stores it in the provided buffer.
      */
      int readInputRegister (uint8_t *buffer, uint16_t max) const;

      /**
         @brief Writes data that starts with the specified register address to the MAX7311 device.
         @param reg The register address where the data will be started to be written.
         @param data Pointer to the data to write.
         @param size The number of bytes to write, reg + size must be less than 8 (Timeout is the last register).
         @return  The number of bytes written, or -1 on error.
         @note This function writes data to the specified register, starting with the register address.
               It is used internally to update the device's registers.
               The data is written sequentially starting from the specified register address.
      */
      int writeRegister (uint8_t reg, const uint8_t *data, uint16_t size);

      // ---------------------------------------- I/O methods ---------------------------------------
      bool readInputPorts (uint8_t index = 0, uint8_t nb = 2) const;
      bool readOutputPorts (uint8_t index = 0, uint8_t nb = 2) const;
      bool writeOutputPorts (uint8_t index = 0, uint8_t nb = 2);
      bool readPolarityInversion (uint8_t index = 0, uint8_t nb = 2) const;
      bool writePolarityInversion (uint8_t index = 0, uint8_t nb = 2);
      bool readConfiguration (uint8_t index = 0, uint8_t nb = 2) const;
      bool writeConfiguration (uint8_t index = 0, uint8_t nb = 2);
      bool readTimeout () const;
      bool writeTimeout ();

      // ----------------------------------------------------------------------
      // internal
      inline bool readAllRegisters () const {
        return readInputPorts() &&
               readOutputPorts() &&
               readPolarityInversion() &&
               readConfiguration() &&
               readTimeout();
      }

      // ----------------------------------------------------------------------
      // internal
      inline bool writeAllRegisters () {
        return writeOutputPorts() &&
               writePolarityInversion() &&
               writeConfiguration() &&
               writeTimeout();
      }

      // ----------------------------------------------------------------------
      // internal
      inline bool writeSetup (uint8_t index = 0, uint8_t nb = 2) {
        return  writePolarityInversion (index, nb) &&
                writeConfiguration (index, nb);
      }

      // ----------------------------------------------------------------------
      // internal
      inline bool readSetup (uint8_t index = 0, uint8_t nb = 2) const {
        return  readPolarityInversion (index, nb) &&
                readConfiguration (index, nb);
      }

      // --------------------------- data members ---------------------------
      static constexpr uint8_t NofRegisters = 9; // Total number of registers

      /**
         @brief Pointer to the I2C device used for communication.
      */
      std::shared_ptr<I2cDev> i2c;

      /**
         @brief I2C address of the MAX7311 device.
      */
      uint16_t addr;

      /**
         @brief Indicates if the converter is successfully connected to the I2C bus.
      */
      mutable bool isConnected;

      /**
         @brief Indicates if the bus timeout is enabled.
      */
      bool busTimeout;

      struct RegBuffers {
        uint8_t input[2]; ///< Input register buffer
        uint8_t output[2]; ///< Output register buffer
        uint8_t polarity[2]; ///< Polarity inversion register buffer
        uint8_t config[2]; ///< Configuration register buffer
        uint8_t timeout; ///< Timeout register buffer
        RegBuffers() : input{0}, output{0}, polarity{0}, config{0}, timeout (0) {}
      };

      mutable RegBuffers regs; ///< Buffer for register values

      PIMP_DECLARE_PUBLIC (Max7311)
  };
} // namespace Piduino
/* ========================================================================== */
