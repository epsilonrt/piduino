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
         @return The value read from the device, default implementation returns LONG_MIN.
         @note may be overridden by subclasses to implement specific reading logic.
         This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long read () override {

        return LONG_MIN;
      }

      /**
         @brief Reads a value from the converter device.
         @param channel The channel to read from.
         @param differential If true, reads in differential mode (default is false).
         @return The value read from the device, default implementation returns LONG_MIN.
         @note may be overridden by subclasses to implement specific reading logic.
          This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long readSample (int channel = 0, bool differential = false) override {

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
      virtual bool writeSample (long value, int channel = 0, bool differential = false) override {

        return write (value);
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
         @brief Returns the maximum value supported by the converter.
         @return The maximum value.
         @note must be implemented by subclasses to return the actual maximum value.
      */
      virtual long max() const {
        // Default implementation returns 0, indicating no maximum set
        return 0;
      }

      /**
         @brief Returns the minimum value supported by the converter.
         @return The minimum value.
         @note must be implemented by subclasses to return the actual minimum value.
      */
      virtual long min() const override {
        return 0; // Default implementation returns 0, indicating no minimum set
      }

      // ------------------------------------ Optional API ------------------------------------
      // Depending on the flags and type, these methods may not be overridden by subclasses.

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
      virtual bool bipolar() const override {
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
        @brief Gets the current PWM range.
        @return The range value, typically max() - min()
      */
      virtual long range() const override {
        return max() - min();
      }

      /**
         @brief Sets the PWM range.
         @param range The desired range value.
         @return The set range value, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setRange (long range) override {
        return -1;
      }

      /**
        @brief Sets the reference voltage of the ADC.
        @param referenceId The ID of the reference voltage to set, which can be a predefined constant or a custom value depending on the ADC model.
        @return true if the reference voltage was set successfully, false otherwise.
      */
      virtual bool setReference (int referenceId) override {
        return false; // Default implementation returns false, should be overridden by subclasses
      }

      /**
         @brief Gets the reference voltage of the ADC.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the ADC model.
      */
      virtual int reference() const override {
        return UnknownReference; // Default implementation returns UnknownReference
      }

      /**
        @brief Gets the current reference voltage.
        @return The reference voltage value, typically 3.3V or 5V depending on the ADC model.
      */
      virtual double referenceValue() const override {
        return 3.3; // Default implementation returns 3.3V, should be overridden by subclasses
      }

      /**
         @brief Converts digital value to analog value.
         @param digitalValue The digital value to convert.
         @return The corresponding analog value.
         @note Default implementation converts digital value to analog value based on reference value.
      */
      virtual double digitalToValue (long digitalValue) const override {
        return (static_cast<double> (digitalValue) * referenceValue()) / (max() - min() + 1);
      }

      /**
         @brief Converts analog value to digital value.
         @param value The analog value to convert.
         @return The corresponding digital value.
         @note Default implementation converts analog to digital value based on reference value.
      */
      virtual long valueToDigital (double value) const override {
        return static_cast<long> ( (value * (max() - min() + 1)) / referenceValue());
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

      // --------------------------------------------------------------------------
      virtual const std::string &deviceName() const override {
        static const std::string name = ConverterTemplate::registeredName();
        return name;
      }

      // ------------------------- internal methods -------------------------



      // --------------------------- data members ---------------------------


      PIMP_DECLARE_PUBLIC (ConverterTemplate)
  };
}

/* ========================================================================== */
