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

#include <climits>
#include <cmath>
#include <piduino/converter.h>
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
         @brief Constructs the private implementation for Converter.
         @param q Pointer to the associated Converter object.
         @param type The type of the converter (default is None).
         @param flags The flags indicating the capabilities of the converter (default is 0).
      */
      Private (Converter *q, Type type = None, unsigned int flags = 0);

      /**
         @brief Destructor for the Private class.
      */
      virtual ~Private();

      /**
         @brief Returns the name of the device.
         @return The device name as a string.
         @note must be implemented by subclasses to return the actual device name.
      */
      virtual const std::string &deviceName() const {
        static const std::string name = "Unknown Converter";
        return name;
      }

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
         @brief Returns the maximum value supported by the converter.
         @return The maximum value.
         @note must be implemented by subclasses to return the actual maximum value.
      */
      virtual long max() const {
        return LONG_MAX; // Default implementation returns LONG_MAX, indicating no maximum set
      }

      /**
         @brief Returns the minimum value supported by the converter.
         @return The minimum value.
         @note must be implemented by subclasses to return the actual minimum value.
      */
      virtual long min() const {
        return 0; // Default implementation returns 0, indicating no minimum set
      }

      /**
         @brief Reads a value from the converter device.
         @return The value read from the device, defaut implementation returns -LONG_MAX.
         @note may be overridden by subclasses to implement specific reading logic.
      */
      virtual long read() {

        return LONG_MIN;
      }

      /**
         @brief Writes a value to the converter device.
         @param value The value to write.
         @return True if the value was successfully written, false otherwise.
         @note may be overridden by subclasses to implement specific writing logic.
      */
      virtual bool write (long value) {

        return false;
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

      // ------------------------------------ Optional API ------------------------------------
      // Depending on the flags and type, these methods may not be overridden by subclasses.

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
      virtual bool bipolar() const {
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
        @brief Gets the current PWM range.
        @return The range value, typically max() - min()
      */
      virtual long range() const {
        return max() - min();
      }

      /**
         @brief Sets the PWM range.
         @param range The desired range value.
         @return The set range value, or -1 if not supported.
         @note Default implementation returns -1. Should be overridden by subclasses.
      */
      virtual long setRange (long range) {
        return -1;
      }

      /**
        @brief Sets the reference voltage of the ADC.
        @param referenceId The ID of the reference voltage to set, which can be a predefined constant or a custom value depending on the ADC model.
        @return true if the reference voltage was set successfully, false otherwise.
      */
      virtual bool setReference (int referenceId) {
        return false; // Default implementation returns false, should be overridden by subclasses
      }

      /**
         @brief Gets the reference voltage of the ADC.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the ADC model.
      */
      virtual int reference() const {
        return UnknownReference; // Default implementation returns UnknownReference
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
         @brief Macro to declare the public interface for the Converter.
      */
      PIMP_DECLARE_PUBLIC (Converter)
  };
}

/* ========================================================================== */
