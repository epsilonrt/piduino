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

#include <iostream>
#include <map>
#include <climits>
#include <cmath>
#include <piduino/converter.h>
#include <piduino/gpio.h>
#include "iodevice_p.h"

namespace Piduino {

  /**
     @brief Private implementation class for Converter.

     This class encapsulates the private data and implementation details for the Converter class,
     inheriting from IoDevice::Private. It manages the configuration and state of a Converter,
     including its type, resolution, and whether it operates in bipolar mode.
  */
  class Converter::Private  : public IoDevice::Private {

    public:

      /**
         @brief Constructor for the Private class.
         @param q Pointer to the Converter instance.
         @param type Type of the converter (default is None).
         @param flags Flags for the converter (default is 0).
         @param parameters Parameters for the converter, a colon-separated list of values (default is empty).
      */
      Private (Converter *q, Type type = UnknownType, unsigned int flags = 0, const std::string &parameters = "");

      /**
         @brief Destructor for the Private class.
      */
      virtual ~Private();

      /**
         @brief Returns the name of the device.
         @return The device name as a string.
         @note must be implemented by subclasses to return the actual device name.
      */
      virtual const std::string &deviceName() const = 0;

      /**
         @brief Opens the converter device with the specified mode.
         @param mode The mode in which to open the device.
         @return True if the device was successfully opened, false otherwise.
         @note must be overridden by subclasses to implement specific opening logic.
      */
      virtual bool open (OpenMode mode) {

        return IoDevice::Private::open (mode);
      }

      /**
         @brief Closes the converter device.
      */
      virtual void close() {

        IoDevice::Private::close();
      }

      /**
         @brief Reads a value from the converter device.
         @return The value read from the device, default implementation returns InvalidValue.
         @note may be overridden by subclasses to implement specific reading logic.
         This function is not callable if the open mode is not ReadOnly or ReadWrite (or closed).
      */
      virtual long read () {

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
      virtual long readChannel (int channel = 0, bool differential = false) {

        return read();
      }

      /**
         @brief Writes a value to the converter device.
         @param value The value to write, this value will be clamped to the valid range.
         @return True if the value was successfully written, false otherwise.
         @note may be overridden by subclasses to implement specific writing logic.
          This function is not callable if the open mode is not WriteOnly or ReadWrite (or closed).
      */
      virtual bool write (long value) {

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
      virtual bool writeChannel (long value, int channel = 0, bool differential = false) {

        return write (value);
      }

      /**
         @brief Returns the number of channels supported by the converter.
         @return The number of channels, a channel is numbering from 0 to numberOfChannels() - 1.
         @note Default implementation returns 1, indicating a single channel.
      */
      virtual int numberOfChannels() const {

        return 1; // Default implementation returns 1, indicating a single channel
      }

      /**
         @brief Enables or disables the converter.
         @param enable true to enable, false to disable.
      */
      virtual void setEnable (bool enable) {
        // Default implementation does nothing
      }

      /**
         @brief Checks if the converter is enabled.
         @return true if enabled, false otherwise.
      */
      virtual bool isEnabled () const {
        return false; // Default implementation returns false, indicating not enabled
      }

      /**
        @brief Gets the digital range of the converter

        This function is used by default by \c valueToDigital() and \c digitalToValue() to calculate the appropriate scaling factors.

        @return The range value in LSB,  this is the number of discrete values the converter can produce, 2^n for n bits resolution.
        @note must be implemented by subclasses to return the actual range value.
      */
      virtual long range() const = 0;

      /**
         @brief Returns the maximum value supported by the converter.
         @return The maximum value.
         @note may be implemented by subclasses to return the actual maximum value.
      */
      virtual long max (bool differential = false) const {

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
      virtual long min (bool differential = false) const {

        if (isBipolar() && differential) {
          return -range() / 2; // For bipolar, min is negative half the range
        }
        return 0; // For unipolar, min is always 0
      }

      // ------------------------------------ Optional API ------------------------------------
      // Depending on the flags and type, these methods may not be overridden by subclasses.

      /**
         @brief Toggles the state of a specific channel on the converter.
         @param channel The channel to toggle (default is -1, which toggles all channels).
         @return true if successful, false otherwise.
         @note This function is disabled if the open mode is not WriteOnly or ReadWrite or if type() is not GpioExpander.
      */
      virtual bool toggle (int channel = -1) {

        return false;
      }

      /**
         @brief Sets the digital range of the converter.
         @param range The desired range value.
         @return The set range value, 0 if not set, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setRange (long range) {
        return -1;
      }

      /**
        @brief Returns the resolution of the converter in bits.
        @return The resolution in bits.
      */
      virtual int resolution() const {
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
      virtual int setResolution (int resolution) {
        return setRange (1L << resolution);
      }

      /**
         @brief Indicates if the converter supports bipolar operation.
         @return true if bipolar, false otherwise.
      */
      virtual bool isBipolar() const {
        return false; // Default implementation returns false, indicating no bipolar support
      }

      /**
         @brief Sets the bipolar mode of the converter.
         @param bipolar true to enable bipolar mode, false to disable.
         @return true if the operation was successful, false otherwise.
      */
      virtual bool setBipolar (bool bipolar) {
        return false; // Default implementation returns false, indicating no bipolar support
      }

      /**
         @brief Sets the reference value of the converter.
         @param referenceId The ID of the reference value to set, which can be a predefined constant or a custom value depending on the converter model.
         @param fsr The full-scale range value, used for external reference (default is 0.0).
         @return true if the reference value was set successfully, false otherwise.
         @note Default implementation returns false, should be overridden by subclasses.
      */
      virtual bool setReference (int referenceId, double fsr = 0.0, int channel = -1) {
        return false; // Default implementation returns false, should be overridden by subclasses
      }

      /**
         @brief Gets the current reference ID of the converter.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the converter model.
      */
      virtual int reference (int channel = -1) const {
        return UnknownReference; // Default implementation returns UnknownReference
      }

      /**
         @brief Gets the current full-scale range of the converter.

         This function is used by default by \c valueToDigital() and \c digitalToValue() to calculate the appropriate scaling factors.

         @return The full-scale range value, typically in volts but may vary depending on the converter model.
         @note Default implementation returns 3.3V, should be overridden by subclasses.
      */
      virtual double fullScaleRange (int channel = -1) const {
        return 3.3; // Default implementation returns 3.3V, should be overridden by subclasses
      }

      /**
        @brief Sets the full-scale range of the converter.
        @param fsr The desired full-scale range value.
        @return True if the full-scale range was successfully set, false otherwise.
        @note Default implementation returns false. Should be overridden by subclasses.
      */
      virtual bool setFullScaleRange (double fsr, int channel = -1) {
        return false;
      }

      /**
         @brief Converts a digital value to an analog value.
         @param digitalValue The digital value to convert.
         @param differential If true, converts in differential mode (default is false).
         @return The corresponding analog value.
      */
      virtual double digitalToValue (long digitalValue, bool differential = false, int channel = -1) const {
        double result = static_cast<double> (digitalValue) * fullScaleRange(channel) / range();

        return result;
      }

      /**
        @brief Converts a analog value to a digital value.
        @param value The value to convert.
        @param differential If true, converts in differential mode (default is false).
        @return The corresponding digital value.
      */
      virtual long valueToDigital (double value, bool differential = false, int channel = -1) const {
        long result = static_cast<long> ( (value * range()) / fullScaleRange(channel));

        return clampValue (result, differential);
      }

      /**
        @brief Gets the current clock frequency.
        @return The frequency in Hertz.
      */
      virtual long frequency() const {
        return -1; // Default implementation returns -1, indicating no frequency set
      }

      /**
         @brief Sets the clock frequency.
         @param freq The desired frequency in Hertz.
         @return The actual frequency set, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setFrequency (long freq) {
        return -1;
      }

      /**
         @brief Gets the current clock setting.
         @return The current clock setting. This is typically used to choose a specific clock source or frequency for the converter.
         @note Default implementation returns UnknownClock, indicating no clock set.
      */
      virtual int clock() const {
        return UnknownClock; // Default implementation returns UnknownClock, indicating no clock set
      }

      /**
         @brief Sets the current clock setting.
         @param clock The desired clock setting.
      */
      virtual bool setClock (int clock) {
        return false; // Default implementation returns false, indicating no clock support
      }

      /**
        Return the current mode of the converter.
        @param channel The channel number (default is 0).
        @return The current mode of the converter as a Mode object, the default channel implementation returns NoMode.
      */
      virtual Mode mode (int channel = 0) const {
        return NoMode; // Default implementation returns NoMode, indicating no specific mode set
      }

      /**
         Set the mode of the converter.
         @param m The mode to set.
         @param channel The channel number (default is -1, indicating all channels).
         @return True if the mode was successfully set, false otherwise. The default implementation returns false.
         @note This function may be overridden by subclasses to implement specific mode setting logic.
      */
      virtual bool setMode (Mode m, int channel = -1) {
        return false; // Default implementation returns false, indicating no mode support
      }

      // ------------------------- internal methods -------------------------

      /**
         @brief Clamps a value to the valid range of the converter.
         @param value The value to clamp.
         @param differential If true, clamps in differential mode (default is false).
         @return The clamped value within the range [min(), max()].
         @note This method ensures that the value does not exceed the converter's limits.
      */
      long clampValue (long value, bool differential = false) const {

        if (value < min (differential)) {

          value = min (differential);
          if (isDebug) {
            std::cerr << "Converter::write(" << value << ") below minimum, setting to " << min (differential) << std::endl;
          }
        }
        if (value > max (differential)) {

          value = max (differential);
          if (isDebug) {
            std::cerr << "Converter::write(" << value << ") above maximum, setting to " << max (differential) << std::endl;
          }
        }
        return value;
      }

      /**
         @brief Splits a string into a vector of strings based on a delimiter.
         @param str The string to split.
         @param delimiter The character used to split the string.
         @param skipEmpty If true, empty strings are skipped in the result (default is false).
         @return A vector of strings resulting from the split operation.
      */
      static std::vector<std::string> split (const std::string &str, char delimiter, bool skipEmpty = false);

      /**
         @brief Parses a list of parameters into a map.
         @param parameters The list of parameters as a vector of strings. Each parameter must be in the format "key=value" if not, it will be ignored.
         @return A map where keys are parameter names and values are parameter values.
      */
      static std::map<std::string, std::string> parseParameters (std::vector<std::string> &parameters);

      /**
         @brief Gets the pin associated with a given string identifier.
         @param s The string identifier for the pin.
         @return Pointer to the Pin object, or nullptr if not found.
         @note This method is used to retrieve a Pin object based on its string identifier.
      */
      Pin *getPin (const std::string &s);

      /**
         @brief struct for storing converter registration information.
      */
      struct registryKey {
        std::string type; ///< The type of the converter ("dac" or "adc").
        std::string parameters; ///< Parameters for the converter, a colon-separated list of values.
        std::function<Converter* (const std::string &parameters) > creator; ///< Function to create an instance of the converter.
        registryKey() = default; ///< Default constructor.
        registryKey (std::function<Converter* (const std::string &parameters) > creator, const std::string &type = "dac",
                     const std::string &parameters = "")
          : creator (creator), type (type), parameters (parameters) {} ///< Constructor with parameters.
      };

      /**
          @brief Gets the registry of converter creators.
          @return A reference to the registry map.
      */
      static std::map<std::string, registryKey>   &getRegistry();

      //-- Private data members ------------------------------------------------------

      /**
         @brief The type of the converter.
      */
      Type type;

      /**
         @brief The flags indicating the capabilities of the converter.
      */
      unsigned int flags;

      /**
         @brief The parameters for the converter.
      */
      std::vector<std::string> parameters; ///< Parameters for the converter, e.g., "param1:param2:param3"

      /**
         @brief Macro to declare the public interface for the Converter.
      */
      PIMP_DECLARE_PUBLIC (Converter)
  };

  /**
    @brief Macro to automatically register a converter class.

    This macro simplifies the registration process by creating a static registrar
    class that calls the Converter::registerConverter() function with the appropriate parameters.
    For the registration to work, the class must implement the registeredName() static method and a constructor that takes
    a std::string parameter corresponding to the colon-separated list of parameters.

    @param ClassName The name of the class to register.
  */
#define REGISTER_CONVERTER(ClassName, Type, Parameters) \
  namespace { \
    struct ClassName##Registrar { \
      ClassName##Registrar() { \
        Converter::registerConverter(ClassName::registeredName(), \
                                     [](const std::string& parameters) -> Converter* { \
                                                                                       return new ClassName(parameters); \
                                                                                     }, \
                                     Type, Parameters); \
      } \
    }; \
    static ClassName##Registrar ClassName##_registrar; \
  }
}

/* ========================================================================== */
