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
#include <piduino/iodevice.h>

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
        None = -1         ///< No converter type specified
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
        hasSetBipolar     = 0x00000200  ///< Indicates that the converter supports setting the bipolar mode
      };

      enum {
        DefaultReference = 0, ///< Default reference voltage
        InternalReference,    ///< Internal reference voltage
        VccReference,        ///< Vcc reference voltage
        ExternalReference,     ///< External reference voltage
        Internal2Reference,   ///< 2nd internal reference voltage
        UnknownReference = -1 ///< Unknown reference voltage
      };

      /**
         @brief Constructs a Converter object.
      */
      Converter();

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
      virtual long max() const;

      /**
         @brief Returns the minimum value the converter can produce.
         @return The minimum value.
      */
      virtual long min() const;

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
      virtual bool bipolar() const;

      /**
         @brief Sets the bipolar mode of the converter.
         @param bipolar true to enable bipolar mode, false to disable.
         @return true if the operation was successful, false otherwise.
      */
      virtual bool setBipolar (bool bipolar);

      /**
        @brief Sets the reference voltage of the ADC.
        @param referenceId The ID of the reference voltage to set, which can be a predefined constant or a custom value depending on the ADC model.
        @return true if the reference voltage was set successfully, false otherwise.
      */
      virtual bool setReference (int referenceId);

      /**
         @brief Gets the reference voltage of the ADC.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the ADC model.
      */
      virtual int reference() const;

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
        @brief Gets the current range.
        @return The range value, typically max() - min()
      */
      virtual long range() const;

      /**
         @brief Sets the range.
         @param range The desired range value.
         @return The set range value, 0 if not set, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setRange (long range);

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
