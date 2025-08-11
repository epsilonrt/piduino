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

#include <piduino/max1161x.h>
#include "converter_p.h"

namespace Piduino {

  /**
     @class Max1161x::Private
     @brief Private implementation class for Max1161x.
  */
  class Max1161x::Private  : public Converter::Private {

    public:
      Private (Max1161x *q, I2cDev *dev, MaxIndex maxId, int referenceId, double fsr, bool bipolar);
      Private (Max1161x *q, const std::string &parameters);
      virtual ~Private();

      /**
         @brief Returns the name of the device.
         @return The device name as a string, the same as Max1161x::registeredName().
      */
      virtual const std::string &deviceName() const override {
        static const std::string name = Max1161x::registeredName();
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
      virtual long read () override {

        return readSample();
      }

      /**
         @brief Reads a value from the converter device.
         @param channel The channel to read from.
         @param differential If true, reads in differential mode (default is false).
         @return The value read from the device, default implementation returns InvalidValue.
         @note may be overridden by subclasses to implement specific reading logic.
          This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long readSample (int channel = 0, bool differential = false) override;

      /**
        @brief Returns the number of channels supported by the converter.
        @return The number of channels, a channel is numbering from 0 to numberOfChannels() - 1.
      */
      virtual int numberOfChannels() const override {

        return max.nchan;
      }

      /**
         @brief Checks if the converter is enabled.
         @return true if enabled, false otherwise.
      */
      virtual bool isEnabled () const override {
        return isConnected;
      }

      /**
        @brief Gets the digital range of the converter
        @return The range value in LSB
      */
      virtual long range() const override {
        return 1L << 12; // Default implementation returns 4096, indicating a 12-bit range
      }

      /**
        @brief Returns the resolution of the converter in bits.
        @return The resolution in bits.
      */
      virtual int resolution() const override {

        return 12; // Default implementation returns 12, indicating a 12-bit resolution
      }

      /**
         @brief Indicates if the converter supports bipolar operation.
         @return true if bipolar, false otherwise.
      */
      virtual bool isBipolar() const override {

        return bipolar; // Default implementation returns false, indicating no bipolar support
      }

      /**
         @brief Sets the bipolar mode of the converter.
         @param bipolar true to enable bipolar mode, false to disable.
         @return true if the operation was successful, false otherwise.
      */
      virtual bool setBipolar (bool bipolar) override {

        if (this->bipolar != bipolar) {

          this->bipolar = bipolar;
          if (isConnected) {
            return updateSetup();  // Update the setup after changing bipolar mode
          }
        }
        return true; // Return true if the bipolar mode is unchanged
      }

      /**
         @brief Sets the reference value of the converter.
         @param referenceId The ID of the reference value to set, which can be a predefined constant or a custom value depending on the converter model.
         @param fsr The full-scale range value, used for external reference (default is 0.0).
         @return true if the reference value was set successfully, false otherwise.
         @note Default implementation returns false, should be overridden by subclasses.
      */
      virtual bool setReference (int referenceId, double fsr = 0.0) override;

      /**
         @brief Gets the current reference ID of the converter.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the converter model.
      */
      virtual int reference() const override {

        return referenceId;
      }

      /**
         @brief Gets the current full-scale range of the converter.
         @return The full-scale range value, typically in volts but may vary depending on the converter model.
         @note Default implementation returns 3.3V, should be overridden by subclasses.
      */
      virtual double fullScaleRange() const override {

        return fsr;
      }

      /**
         @brief Gets the current clock setting.
         @return The current clock setting. 2 selections are available:
          - `InternalClock` : The internal clock is used (default).
          - `ExternalClock` : The external clock is used.
      */
      virtual int clock() const override {

        return clkSetting;
      }

      /**
         @brief Sets the current clock setting.
         @param clock The desired clock setting.
      */
      virtual bool setClock (int clock) override {

        if (clock != clkSetting) {
          clkSetting = clock;
          if (isConnected) {
            return updateSetup(); // Update the setup after changing the clock setting
          }
        }
        return true; // Return true if the clock setting is unchanged
      }

      // ------------------------- internal methods -------------------------
      bool sendByte (uint8_t data);
      bool getLastConversion (long &conversion);
      bool updateSetup ();

      // ----------------- internal typedef and structures ------------------



      // --------------------------- data members ---------------------------
      std::shared_ptr<I2cDev> i2c; ///< Pointer to the I2C device used for communication.
      Info max; ///< Information about the Max1161x converter, including model ID, I2C address, number of channels, and internal reference voltage.
      int referenceId; ///< ID of the reference voltage used by the converter.
      double fsr; ///< Full-scale range value, typically in volts but may vary depending on the converter model.
      bool bipolar; ///< Indicates if the converter is in bipolar mode.
      bool isConnected; ///< Indicates if the converter is successfully connected to the I2C bus.
      int clkSetting; ///< The external clock setting.

      PIMP_DECLARE_PUBLIC (Max1161x)
  };
}

/* ========================================================================== */
