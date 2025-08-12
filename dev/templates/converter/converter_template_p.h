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

#include <piduino/converter_template.h>
#include "converter_p.h"

namespace Piduino {

  /**
     @class ConverterTemplate::Private
     @brief Private implementation class for ConverterTemplate.
  */
  class ConverterTemplate::Private  : public Converter::Private {

    public:
      Private (ConverterTemplate *q /* TODO: complete constructor */);
      Private (ConverterTemplate *q, const std::string &parameters);
      virtual ~Private();

      /**
        @brief Returns the name of the device.
        @return The device name as a string, the same as ConverterTemplate::registeredName().
      */
      virtual const std::string &deviceName() const override {
        static const std::string name = ConverterTemplate::registeredName();
        return name;
      }

      /**
         @brief Opens the converter device with the specified mode.
         @param mode The mode in which to open the device.
         @return True if the device was successfully opened, false otherwise.
         @note must be overridden by subclasses to implement specific opening logic.
      */
      virtual bool open (OpenMode mode) override {

        return Converter::Private::open (mode);
      }

      /**
         @brief Closes the converter device.
      */
      virtual void close() override {

        Converter::Private::close();
      }

      /**
         @brief Reads a value from the converter device.
         @return The value read from the device, default implementation returns InvalidValue.
         @note may be overridden by subclasses to implement specific reading logic.
         This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long read () override {

        return InvalidValue;
      }

      /**
         @brief Reads a value from the converter device.
         @param channel The channel to read from.
         @param differential If true, reads in differential mode (default is false).
         @return The value read from the device, default implementation returns InvalidValue.
         @note may be overridden by subclasses to implement specific reading logic.
          This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long readChannel (int channel = 0, bool differential = false) override {

        return read();
      }

      /**
         @brief Writes a value to the converter device.
         @param value The value to write, this value will be clamped to the valid range.
         @return True if the value was successfully written, false otherwise.
         @note may be overridden by subclasses to implement specific writing logic.
          This function is not callable if the open mode is not WriteOnly or ReadWrite (or closed).
      */
      virtual bool write (long value) override {

        return false;
      }

      /**
         @brief Writes a value to the converter device.
         @param value The value to write, this value will be clamped to the valid range.
         @param channel The channel to write to.
         @param differential If true, writes in differential mode (default is false).
         @return True if the value was successfully written, false otherwise.
         @note may be overridden by subclasses to implement specific writing logic.
          This function is not callable if the open mode is not WriteOnly or ReadWrite (or closed).
      */
      virtual bool writeChannel (long value, int channel = 0, bool differential = false) override {

        return write (value);
      }

      /**
        @brief Returns the number of channels supported by the converter.
        @return The number of channels, a channel is numbering from 0 to numberOfChannels() - 1.
        @note Default implementation returns 1, indicating a single channel.
      */
      virtual int numberOfChannels() const override {

        return 1; // Default implementation returns 1, indicating a single channel
      }

      /**
         @brief Enables or disables the converter.
         @param enable true to enable, false to disable.
      */
      virtual void setEnable (bool enable) override {
        // Default implementation does nothing
      }

      /**
         @brief Checks if the converter is enabled.
         @return true if enabled, false otherwise.
      */
      virtual bool isEnabled () const override {
        return false; // Default implementation returns false, indicating not enabled
      }

      /**
        @brief Gets the digital range of the converter
        @return The range value in LSB, e.g. max() - min() + 1.
        @note must be implemented by subclasses to return the actual range value.
      */
      virtual long range() const = 0;

      /**
         @brief Returns the maximum value supported by the converter.
         @return The maximum value.
         @note may be implemented by subclasses to return the actual maximum value.
      */
      virtual long max (bool differential = false) const override {

        if (isBipolar() && differential) {
          return range() / 2 - 1; // For bipolar, max is half the range minus one
        }
        return range() - 1; // For unipolar, max is the full range minus one
      }

      /**
         @brief Returns the minimum value supported by the converter.
         @return The minimum value.
         @note must be implemented by subclasses to return the actual minimum value.
      */
      virtual long min (bool differential = false) const override {

        if (isBipolar() && differential) {
          return -range() / 2; // For bipolar, min is negative half the range
        }
        return 0; // For unipolar, min is always 0
      }

      /**
         @brief Gets the current full-scale range of the converter.
         @return The full-scale range value, typically in volts but may vary depending on the converter model.
         @note Default implementation returns 3.3V, should be overridden by subclasses.
      */
      virtual double fullScaleRange() const override {
        return 3.3; // Default implementation returns 3.3V, should be overridden by subclasses
      }

      /**
         @brief Converts a digital value to an analog value.
         @param digitalValue The digital value to convert.
         @param differential If true, converts in differential mode (default is false).
         @return The corresponding analog value.
      */
      virtual double digitalToValue (long digitalValue, bool differential = false) const override {
        double result = static_cast<double> (digitalValue) * fullScaleRange() / range();

        return result;
      }

      /**
        @brief Converts a analog value to a digital value.
        @param value The value to convert.
        @param differential If true, converts in differential mode (default is false).
        @return The corresponding digital value.
      */
      virtual long valueToDigital (double value, bool differential = false) const override {
        long result = static_cast<long> ( (value * range()) / fullScaleRange());

        return clampValue (result, differential);
      }

      // ------------------------------------ Optional API ------------------------------------
      // Depending on the flags and type, these methods may not be overridden by subclasses.

      /**
         @brief Sets the digital range of the converter.
         @param range The desired range value.
         @return The set range value, 0 if not set, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setRange (long range) override {

        return -1;
      }

      /**
        @brief Returns the resolution of the converter in bits.
        @return The resolution in bits.
      */
      virtual int resolution() const override {
        long r = range();

        if (r > 0) {
          return log2 (r);
        }
        return -1; // Default implementation returns -1, indicating no resolution set
      }

      /**
         @brief Sets the resolution of the converter.
         @param resolution The desired resolution in bits.
         @return The actual resolution set, or -1 if not supported.
      */
      virtual int setResolution (int resolution) override {
        return setRange (1L << resolution);
      }

      /**
         @brief Indicates if the converter supports bipolar operation.
         @return true if bipolar, false otherwise.
      */
      virtual bool isBipolar() const override {
        return false; // Default implementation returns false, indicating no bipolar support
      }

      /**
         @brief Sets the bipolar mode of the converter.
         @param bipolar true to enable bipolar mode, false to disable.
         @return true if the operation was successful, false otherwise.
      */
      virtual bool setBipolar (bool bipolar) override {
        return false; // Default implementation returns false, indicating no bipolar support
      }

      /**
         @brief Sets the reference value of the converter.
         @param referenceId The ID of the reference value to set, which can be a predefined constant or a custom value depending on the converter model.
         @param fsr The full-scale range value, used for external reference (default is 0.0).
         @return true if the reference value was set successfully, false otherwise.
         @note Default implementation returns false, should be overridden by subclasses.
      */
      virtual bool setReference (int referenceId, double fsr = 0.0) override {
        return false; // Default implementation returns false, should be overridden by subclasses
      }

      /**
         @brief Gets the current reference ID of the converter.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the converter model.
      */
      virtual int reference() const override {
        return UnknownReference; // Default implementation returns UnknownReference
      }

      /**
        @brief Gets the current clock frequency.
        @return The frequency in Hertz.
      */
      virtual long frequency() const override {
        return -1; // Default implementation returns -1, indicating no frequency set
      }

      /**
         @brief Sets the clock frequency.
         @param freq The desired frequency in Hertz.
         @return The actual frequency set, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setFrequency (long freq) override {
        return -1;
      }

      /**
        @brief Gets the current clock setting.
        @return The current clock setting. This is typically used to choose a specific clock source or frequency for the converter.
        @note Default implementation returns UnknownClock, indicating no clock set.
      */
      virtual int clock() const override {
        return UnknownClock; // Default implementation returns UnknownClock, indicating no clock set
      }

      /**
         @brief Sets the current clock setting.
         @param clock The desired clock setting.
      */
      virtual bool setClock (int clock) override {
        return false; // Default implementation returns false, indicating no clock support
      }
      
      // ------------------------- internal methods -------------------------



      // --------------------------- data members ---------------------------


      PIMP_DECLARE_PUBLIC (ConverterTemplate)
  };
} // namespace Piduino
/* ========================================================================== */
