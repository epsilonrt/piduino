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

#include <array>
#include <piduino/converter.h>
#include <piduino/i2cdev.h>

namespace Piduino {

  /**
     @class Mcp4728
     @brief Class for the MCP4728 series of digital-to-analog converters.

     The MCP4728 is a 12-bit digital-to-analog converter (DAC) with an I2C interface.
     It allows for easy integration into microcontroller projects and provides a simple way to generate analog voltages.
     The reference voltage is equal to the supply voltage (VDD), which is typically 3.3V or 5V (3.3V on Pi boards).
     An EEPROM can be enabled to store the DAC's value.

     The parameters for the constructor registered are:
     - `bus=id` : The I2C bus ID (default is I2cDev::defaultBus().id(), use pinfo to get the default bus ID).
     - `addr={0x60...0x67}` : The I2C address of the converter (default is 0x60).
     - `ref={vdd,int}` : The reference voltage setting (default is vdd).
     - `fsr=value` : The full-scale range value. (default is 3.3V)
     - `gain={1,2}` : The gain setting (default is 1).
     - `mode={norm,fast,eeprom,pd1k,pd100k,pd500k}` : The mode setting (default is norm). See the MCP4728 datasheet for more information.
  */
  class Mcp4728 : public Converter {

    public:

      static constexpr int DefaultAddress = 0x60; ///< Default I2C address for the MCP4728
      static constexpr int DefaultReferenceId = VddReference; ///< Default reference ID for the MCP4728
      static constexpr double DefaultFullScaleRange = 3.3; ///< Default full-scale range for the MCP4728
      static constexpr double InternalFullScaleRange = 2.048; ///< Internal full-scale range for the MCP4728

      /**
         @brief Constructs a Mcp4728 object with the specified I2C device and parameters.

         Creates a new MCP4728 DAC controller instance connected to the specified I2C bus
         with the given device address. The device must be properly connected and powered
         before attempting communication.

         @param busId The I2C bus ID (default is I2cDev::Info::defaultBus().id()).
         @param address The I2C address of the converter (default is 0x60).
      */
      Mcp4728 (int busId = I2cDev::Info::defaultBus().id(), int address = DefaultAddress);

      /**
         @brief Virtual destructor for the Mcp4728 class.

         Ensures proper cleanup of I2C resources and resets the device state
         when the object is destroyed.
      */
      virtual ~Mcp4728();

      /**
         @brief Constructs a Mcp4728 object from a string of parameters.

         Creates a MCP4728 DAC controller instance using configuration parameters
         provided as a formatted string. This constructor is primarily used
         by the converter factory system for dynamic object creation.

         @param parameters A string containing the parameters for the Mcp4728 configuration, formatted as
                           "bus=id:addr={0x60..0x67}:ref={vdd,int}:fsr=value:gain={1,2}:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}". \n
                           The parameters for the constructor registered are:
                           - `bus=id` : The I2C bus ID (default is I2cDev::Info::defaultBus().id(), use pinfo to check the default bus ID).
                           - `addr={0x60..0x67}` : The I2C address of the converter (default is 0x60).
                           - `fsr=value` : The full-scale range value. (default is 3.3V).
                           - `mode={norm,eeprom,pd1k,pd100k,pd500k}` : The mode setting (default is norm). See the MCP4728 datasheet for more information.

         @note This constructor is used for factory registration and must be implemented by subclasses.
      */
      explicit Mcp4728 (const std::string &parameters);

      /**
         @brief Returns the name registered for this converter.

         Provides the string identifier used by the converter factory system
         to create instances of this converter type. This name is used in
         configuration files and command-line interfaces.

         @return The name of the converter as a string ("mcp4728").

         @note This method is used for factory registration and must be implemented by subclasses.
      */
      static std::string registeredName() {
        return "mcp4728";
      }

      /**
         @fn virtual bool writeChannel (long value, int channel = 0,  bool noUpdate = false)
         @brief Writes a digital value to a specific channel of the converter.
         @param value The sample value to write. This value will be clamped to the valid range defined by \c min() and \c max().
         @param channel The channel to write to (default is 0).
         @param noUpdate If true, disables the DAC output (default is false). This is useful for storing intermediate values without affecting the output when using LDAC Pin.
         @return true if successful, false otherwise.
         @note This function is disabled if the open mode is not WriteOnly or ReadWrite.
      */

      /**
         @fn virtual bool writeValue (double value, int channel = 0, bool noUpdate = false)
         @brief Writes a value to the converter (DAC).
         @param value The value to write, which is converted to a digital value with valueToDigital().
         @param channel The channel to write to (default is 0).
         @param noUpdate If true, disables the DAC output (default is false). This is useful for storing intermediate values without affecting the output when using LDAC Pin.
         @return true if successful, false otherwise.
      */

      /**
         @brief Sets the mode of the converter.
         @fn virtual bool setMode (Converter::Mode m, int channel = -1)

         Configures the operational mode of the MCP4728 DAC. The mode determines
         how the DAC operates, including speed settings, power management, and
         EEPROM storage behavior.

         @param m The mode to set. This can be a combination of ModeFlag values:
                  - `ModeFlag::NormalMode`: Normal mode (default).
                  - `ModeFlag::FastMode`: Fast mode.
                  - `ModeFlag::SaveToEEPROM`: Save to EEPROM mode, the value written to the DAC will be stored in EEPROM.
                  - `ModeFlag::PwrDwnR1`: Power down mode (for DACs) with 1K resistor.
                  - `ModeFlag::PwrDwnR2`: Power down mode (for DACs) with 100K resistor.
                  - `ModeFlag::PwrDwnR3`: Power down mode (for DACs) with 500K resistor.
                  When PwrDwn mode is set, the DAC will enter a low-power state to save energy.
         @param channel The channel number, the MCP4728 supports only one channel (0). This parameter is ignored.
         @return True if the mode was successfully set, false otherwise.
      */

      /**
         @fn virtual Converter::Mode mode (int channel = 0) const
         @brief Returns the current mode of the converter.

         Retrieves the current operational mode of the MCP4728 DAC, including
         speed settings, power state, and EEPROM configuration.

         @param channel The channel number (default is 0).
         @return The current mode of the converter as a combination of ModeFlag values:
                 - `ModeFlag::NormalMode`: Normal mode (default).
                 - `ModeFlag::FastMode`: Fast mode.
                 - `ModeFlag::SaveToEEPROM`: Save to EEPROM mode, the value written to the DAC will be stored in EEPROM.
                 - `ModeFlag::PwrDwnR1`: Power down mode (for DACs) with 1K resistor.
                 - `ModeFlag::PwrDwnR2`: Power down mode (for DACs) with 100K resistor.
                 - `ModeFlag::PwrDwnR3`: Power down mode (for DACs) with 500K resistor.
                 When PwrDwn mode is set, the DAC will enter a low-power state to save energy.
      */

      /**
         @brief Writes multiple values to the MCP4728 DAC channels in a single operation.

         This function allows writing values to all four channels of the MCP4728
         in one go, which can be more efficient than writing each channel individually.
         The values are provided as an array of 16-bit integers, where each value
         corresponds to a channel.

         @param values An array of 16-bit integers containing the values to write to each channel.
         @return true if the write operation was successful, false otherwise.
         @note The values are written in the order of channels 0 to 3, and the function will block until the operation is complete.
      */
      bool fastWrite (const uint16_t values[4]);

      /**
         @brief Writes multiple values to the MCP4728 DAC channels in a single operation.

         This function allows writing values to all four channels of the MCP4728
         in one go, which can be more efficient than writing each channel individually.
         The values are provided as an array of 16-bit integers, where each value
         corresponds to a channel.

         @param values An array of 16-bit integers containing the values to write to each channel.
         @return true if the write operation was successful, false otherwise.
         @note The values are written in the order of channels 0 to 3, and the function will block until the operation is complete.
      */
      bool fastWrite (const std::array<uint16_t, 4> &values) {
        return fastWrite (values.data());
      }


    protected:
      /**
         @brief Forward declaration of the private implementation class.

         The Private class contains the actual implementation details and
         internal state management for the MCP4728 controller, following
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
      Mcp4728 (Private &dd);

    private:
      /**
         @brief PIMPL pattern declaration macro for private implementation.

         This macro declares the necessary infrastructure for the Pointer to
         Implementation design pattern, providing clean separation between
         public interface and private implementation details.
      */
      PIMP_DECLARE_PRIVATE (Mcp4728)
  };
}

/* ========================================================================== */
