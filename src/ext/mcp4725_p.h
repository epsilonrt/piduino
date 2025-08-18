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

#include <piduino/mcp4725.h>
#include "converter_p.h"

namespace Piduino {

  /**
     @class Mcp4725::Private
     @brief Private implementation class for Mcp4725.

     This class provides the private implementation for the Mcp4725 device, inheriting from Converter::Private.
     It manages the communication with the MAX7311 I2C I/O expander, providing methods for opening, closing,
     reading, writing, and configuring the device. The class supports multiple channels and exposes device-specific
     properties such as resolution, range, and operational modes.

     @note This class is intended for internal use within the Mcp4725 implementation and should not be used directly.
  */
  class Mcp4725::Private  : public Converter::Private {

    public:
      /**
         @brief Constructs a Private implementation object for Mcp4725.
         @param q Pointer to the parent Mcp4725 object.
         @param dev Shared pointer to the I2C device used for communication.
         @param address I2C address of the MAX7311 device.
      */
      Private (Mcp4725 *q, std::shared_ptr<I2cDev> dev, int address);

      /**
         @brief Constructs a Private implementation object for Mcp4725 using parameter string.
         @param q Pointer to the parent Mcp4725 object.
         @param parameters String containing initialization parameters.
      */
      Private (Mcp4725 *q, const std::string &parameters);

      /**
         @brief Destructor for the Private implementation object.
      */
      ~Private() = default;

      /**
          @brief Returns the name of the device.
          @return The device name as a string, the same as Mcp4725::registeredName().
      */
      virtual const std::string &deviceName() const override {
        static const std::string name = Mcp4725::registeredName();
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
        return 1; // Default implementation returns 1, indicating 1 channel
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
        return 1 << 12; // Default implementation returns 4096, indicating a 12-bit range
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
        return 12;
      }

      /**
        @brief Gets the current full-scale range of the converter.

        This function is used by default by \c valueToDigital() and \c digitalToValue() to calculate the appropriate scaling factors.

        @return The full-scale range value, typically in volts but may vary depending on the converter model.
      */
      virtual double fullScaleRange() const override {

        return fsr;
      }

      /**
        @brief Sets the full-scale range of the converter.
        @param fsr The desired full-scale range value.
        @return True if the full-scale range was successfully set, false otherwise.
      */
      virtual bool setFullScaleRange (double fsr) override {
        this->fsr = fsr;
        return true;
      }

      // ----------------------- types and constants -----------------------
      struct RegBuffers {
        uint8_t config; ///< Config register buffer
        uint8_t dac[2]; ///< DAC register buffer
        uint8_t eeprom[2]; ///< EEPROM register buffer
        RegBuffers() : config{0}, dac{0}, eeprom{0} {}
      };
      static constexpr uint8_t RegReadLength = sizeof (RegBuffers) / sizeof (uint8_t); ///<  Length of the register read buffer
      static constexpr uint8_t RegWriteLength = 3; ///< Length of the register write buffer
      static constexpr uint8_t RegFastWriteLength = 2; ///< Length of the register fast write buffer

      // ------------------------- internal methods -------------------------
      bool readRegisters (RegBuffers &regs, uint16_t max = RegReadLength) const;
      bool writeRegisters (RegBuffers &regs, uint16_t size = RegWriteLength);
      bool updateSetup();

      // --------------------------- data members ---------------------------

      std::shared_ptr<I2cDev> i2c; ///< I2C device pointer
      uint16_t addr; ///< I2C slave address
      double fsr; ///< Full-scale range value, typically in volts but may vary depending on the converter model.
      mutable bool isConnected; ///< Connection status flag, updated after each I2C transaction.
      uint16_t value; ///< Current value of the DAC output.
      Converter::Mode currentMode; ///< Current mode of the converter
      bool modeSetByUser; ///< Flag to indicate if the mode was set by the user
      RegBuffers regs; ///< Buffer for register values

      PIMP_DECLARE_PUBLIC (Mcp4725)
  };
} // namespace Piduino
/* ========================================================================== */
