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
#include <vector>
#include <functional>
#include <piduino/iodevice.h>
#include <climits>

namespace Piduino {
  /**
     @brief The Converter class provides an interface for analog-to-digital and digital-to-analog converters.

     This abstract class defines the basic operations and properties for converter devices,
     such as ADCs (Analog-to-Digital Converters) and DACs (Digital-to-Analog Converters).
     It inherits from IoDevice and provides methods for reading, writing, enabling, and configuring the converter.
  */
  class Converter : public IoDevice {
    public:
      /**
         @brief Enumeration of converter types.
      */
      enum Type {
        AnalogToDigital,  ///< Analog-to-Digital Converter (ADC)
        DigitalToAnalog,  ///< Digital-to-Analog Converter (DAC)
        Sensor,          ///< Sensor (not a converter, but can be used with converters)
        GpioExtender,  ///< GPIO extender (not a converter, but can be used with converters)
        UnknownType = -1         ///< No converter type specified
      };

      enum {
        hasFrequency      = 0x00000001, ///< Indicates that the converter has a clock
        hasReference      = 0x00000002, ///< Indicates that the converter has a reference voltage
        hasResolution     = 0x00000004, ///< Indicates that the converter has a resolution
        hasBipolar        = 0x00000008, ///< Indicates that the converter supports bipolar operation
        hasRange          = 0x00000010, ///< Indicates that the converter has a range
        hasSetFrequency   = 0x00000020, ///< Indicates that the converter supports setting the frequency
        hasSetRange       = 0x00000040, ///< Indicates that the converter supports setting the range
        hasSetReference   = 0x00000080, ///< Indicates that the converter supports setting the reference voltage
        hasSetResolution  = 0x00000100, ///< Indicates that the converter supports setting the resolution
        hasSetBipolar     = 0x00000200, ///< Indicates that the converter supports setting the bipolar mode
        requiresWaitLoop  = 0x00000400, ///< Indicates that the converter requires a wait loop to function correctly
        hasClockSelection = 0x00000800, ///< Indicates that the converter supports clock selection
      };

      enum {
        DefaultReference = 0, ///< Default reference voltage
        InternalReference,    ///< Internal reference voltage
        VddReference,         ///< Vdd reference voltage (analog or digital power supply depending on the converter)
        ExternalReference,    ///< External reference voltage
        // The following references are for converters that have multiple internal reference configurations
        Internal1Reference,   ///< 1st internal reference voltage
        Internal2Reference,   ///< 2nd internal reference voltage
        Internal3Reference,   ///< 3rd internal reference voltage
        Internal4Reference,   ///< 4th internal reference voltage
        UnknownReference = -1 ///< Unknown reference voltage
      };

      enum {
        DefaultClock = 0, ///< Default clock setting
        InternalClock,    ///< Internal clock setting
        ExternalClock,    ///< External clock setting
        UnknownClock = -1 ///< Unknown clock setting
      };

      static constexpr long InvalidValue = LONG_MIN; ///< Invalid value for the converter

      /**
         @brief Constructs a Converter object.
         @param parameters The parameters for the converter. This is a list of values separated by colon
         @note This constructor must be implemented by subclasses that want to register in the factory.
         @note This class is abstract and cannot be instantiated directly.
      */
      explicit Converter (const std::string &parameters = "");

      /**
         @brief Creates a converter instance from device name.
         @param deviceName The name of the converter class to instantiate, this name may be followed by parameters in the form of a colon-separated list.
         @param parameters The parameters for the converter, a colon-separated list of values if not specified in deviceName.
         @return Pointer to new Converter instance, or nullptr if not found.
      */
      static Converter *factory (const std::string &deviceName, const std::string &parameters = "");

      /**
         @brief Registers a converter class in the factory.
         @param deviceName The name to register the class under.
         @param creator Function that creates an instance of the class.
      */
      static void registerConverter (const std::string &deviceName,
                                     std::function<Converter* (const std::string &) > creator,
                                     const std::string &type = "dac",
                                     const std::string &parameters = "");
      /**
         @brief Contains information about a registered converter.
      */
      struct Info {
        std::string name; ///< The name of the converter class.
        std::string type; ///< The type of the converter (e.g., "dac", "adc").
        std::string parameters; ///< Parameters for the converter, a colon-separated list of values.
        Info() = default;
        Info (const std::string &name, const std::string &type, const std::string &parameters)
          : name (name), type (type), parameters (parameters) {}
      };

      /**
         @brief Gets list of registered converter names.
         @return Vector of available converter names.
      */
      static std::vector<Info> availableConverters();

      /**
         @brief Destroys the Converter object.
      */
      virtual ~Converter();

      /**
        @brief Returns the device name associated with the converter.
        @return The device name as a string.
      */
      virtual const std::string &deviceName() const;

      /**
         @brief Returns the type of the converter.
         @return The converter type (ADC, DAC, or None).
      */
      Type type() const;

      /**
         @brief returns the flags associated with the converter.
         @return The flags indicating the capabilities of the converter.
      */
      virtual unsigned int flags() const;

      /**
         @brief Opens the converter device with the specified mode.
         @param mode The open mode (default is ReadWrite).
         @return true if successful, false otherwise.
      */
      virtual bool open (IoDevice::OpenMode mode = IoDevice::ReadWrite);

      /**
         @brief Closes the converter device.
      */
      virtual void close();

      /**
         @brief Reads a value from the converter.
         @return The value read.
         @note This function is disabled if the open mode is not ReadOnly or ReadWrite.
      */
      virtual long read();

      /**
         @brief Writes a value to the converter.
         @param value The value to write.
         @return true if successful, false otherwise.
         @note This function is disabled if the open mode is not WriteOnly or ReadWrite.
      */
      virtual bool write (long value);

      /**
         @brief Reads a value from the converter (ADC)
         @param channel The channel to read from.
         @param differential If true, reads in differential mode (default is false).
      */
      virtual long readSample (int channel = 0, bool differential = false);

      /**
         @brief Reads a value from the converter (ADC)
         @param channel The channel to read from (default is 0).
         @param differential If true, reads in differential mode (default is false).
         @return The value read from the converter, which is converted to a value with digitalToValue().
      */
      virtual double readValue (int channel = 0, bool differential = false);

      /**
         @brief Reads an average value of count samples from the converter.
         @param channel The channel to read from (default is 0).
         @param differential If true, reads in differential mode (default is false).
         @param count The number of samples to average (default is 8).
         @return The average digital value read from the converter.
      */
      virtual double readAverage (int channel = 0, bool differential = false, int count = 8);

      /**
         @brief Reads an average value from the converter.
         @param channel The channel to read from (default is 0).
         @param differential If true, reads in differential mode (default is false).
         @param count The number of samples to average (default is 8).
         @return The average value read from the converter, which is converted to a value with digitalToValue().
      */
      virtual double readAverageValue (int channel = 0, bool differential = false, int count = 8);

      /**
         @brief Writes a sample value to the converter (DAC).
         @param value The sample value to write.
         @param channel The channel to write to (default is 0).
         @param differential If true, writes in differential mode (default is false).
         @return true if successful, false otherwise.
         @note This function is disabled if the open mode is not WriteOnly or ReadWrite.
      */
      virtual bool writeSample (long value, int channel = 0, bool differential = false);

      /**
         @brief Writes a value to the converter (DAC).
         @param value The value to write, which is converted to a digital value with valueToDigital().
         @param channel The channel to write to (default is 0).
         @param differential If true, writes in differential mode (default is false).
         @return true if successful, false otherwise.
         @note This function is disabled if the open mode is not WriteOnly or ReadWrite.
      */
      virtual bool writeValue (double value, int channel = 0, bool differential = false);

      /**
         @brief Returns the number of channels supported by the converter.
         @return The number of channels, a channel is numbering from 0 to numberOfChannels() - 1.
         @note Default implementation returns 1, indicating a single channel.
      */
      virtual int numberOfChannels() const;

      /**
         @brief Converts a digital value to an analog value.
         @param digitalValue The digital value to convert.
         @param differential If true, converts in differential mode (default is false).
         @return The corresponding analog value.
      */
      virtual double digitalToValue (long digitalValue, bool differential = false) const;

      /**
        @brief Converts a analog value to a digital value.
        @param value The value to convert.
        @param differential If true, converts in differential mode (default is false).
        @return The corresponding digital value.
      */
      virtual long valueToDigital (double value, bool differential = false) const;

      /**
         @brief Enables or disables the converter.
         @param enable true to enable, false to disable.
      */
      virtual void setEnable (bool enable);

      /**
         @brief Checks if the converter is enabled.
         @return true if enabled, false otherwise.
      */
      virtual bool isEnabled () const;

      /**
         @brief Starts the converter (enables it).
      */
      inline void run();

      /**
         @brief Stops the converter (disables it).
      */
      inline void stop();

      /**
         @brief Returns the maximum value the converter can produce.
         @return The maximum value.
      */
      virtual long max (bool differential = false) const;

      /**
         @brief Returns the minimum value the converter can produce.
         @return The minimum value.
      */
      virtual long min (bool differential = false) const;

      /**
        @brief Returns the resolution of the converter in bits.
        @return The resolution in bits.
      */
      virtual int resolution() const;

      /**
         @brief Sets the resolution of the converter.
         @param resolution The desired resolution in bits.
         @return The actual resolution set, or -1 if not supported.
      */
      virtual int setResolution (int resolution);

      /**
         @brief Indicates if the converter supports bipolar operation.
         @return true if bipolar, false otherwise.
      */
      virtual bool isBipolar() const;

      /**
         @brief Sets the bipolar mode of the converter.
         @param bipolar true to enable bipolar mode, false to disable.
         @return true if the operation was successful, false otherwise.
      */
      virtual bool setBipolar (bool bipolar);

      /**
        @brief Sets the reference value of the converter.
        @param referenceId The ID of the reference value to set, which can be a predefined constant or a custom value depending on the converter model.
        @param fsr The full-scale range value, used for external reference (default is 0.0 that indicates internal or VDD reference).
        @return true if the reference value was set successfully, false otherwise.
        @note Default implementation returns false, should be overridden by subclasses.
      */
      virtual bool setReference (int referenceId, double fsr = 0.0);

      /**
         @brief Gets the current reference ID of the converter.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the converter model.
      */
      virtual int reference() const;

      /**
         @brief Gets the current full-scale range of the converter.
         @return The full-scale range value, typically in volts but may vary depending on the converter model.
         @note Default implementation returns 3.3V, should be overridden by subclasses.
      */
      virtual double fullScaleRange() const;

      /**
        @brief Gets the current clock frequency.
        @return The frequency in Hertz.
      */
      virtual long frequency() const;

      /**
         @brief Sets the clock frequency.
         @param freq The desired frequency in Hertz.
         @return The actual frequency set, 0 if not set, -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setFrequency (long freq);

      /**
        @brief Gets the digital range of the converter
        @return The range value in LSB, e.g. max() - min() + 1.
      */
      virtual long range() const;

      /**
         @brief Sets the digital range of the converter.
         @param range The desired range value.
         @return The set range value, 0 if not set, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setRange (long range);

      /**
         @brief Gets the current clock setting.
         @return The current clock setting. This is typically used to choose a specific clock source or frequency for the converter.
         @note Default implementation returns UnknownClock, indicating no clock set.
      */
      virtual int clock() const;

      /**
         @brief Sets the current clock setting.
         @param clock The desired clock setting.
         @return true if the clock was set successfully, false otherwise.
      */
      virtual bool setClock (int clock);

    protected:
      /**
         @brief Forward declaration of the Private implementation class.
      */
      class Private;

      /**
         @brief Protected constructor for use by derived classes with private data.
         @param dd Reference to the private data.
      */
      Converter (Private &dd);

    private:
      /**
         @brief Macro to declare private implementation pointer.
      */
      PIMP_DECLARE_PRIVATE (Converter)
  };

  #ifndef DOXYGEN
  inline void Converter::run() {
    setEnable (true);
  }
  inline void Converter::stop() {
    setEnable (false);
  }
  #endif // DOXYGEN
} // namespace Piduino
/* ========================================================================== */
