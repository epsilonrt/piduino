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

#include <piduino/converter.h>
#include <piduino/i2cdev.h>

namespace Piduino {

  /**
     @class Max1161x
     @brief Class for the MAX11612 to MAX11617 series of analog-to-digital converters.

    |          | Addr. | Chan SE | Chan Dif | DefaultReference | VccReference | InternalReference | ExternalReference |
    |:--------:|:-----:|:-------:|:--------:|:----------------:|:------------:|:-----------------:|:-----------------:|
    | Max11612 |  0x34 |    4    |     2    |   VccReference   |       5      |       4,096       |       5 max       |
    | Max11613 |  0x34 |    4    |     2    |   VccReference   |      3,3     |       2,048       |      3,3 max      |
    | Max11614 |  0x33 |    8    |     4    |   VccReference   |       5      |       4,096       |       5 max       |
    | Max11615 |  0x33 |    8    |     4    |   VccReference   |      3,3     |       2,048       |      3,3 max      |
    | Max11616 |  0x35 |    12   |     6    |   VccReference   |       5      |       4,096       |       5 max       |
    | Max11617 |  0x35 |    12   |     6    |   VccReference   |      3,3     |       2,048       |      3,3 max      |

    The parameters for the constructor registered are:
    - `bus=id` : The I2C bus ID (default is I2cDev::defaultBus().id(), use pinfo to get the default bus ID).
    - `max={12,13,14,15,16,17}` : The Max1161x model to use, which can be 12, 13, 14, 15, 16, or 17 (default is 15 for Max11615).
    - `ref={ext,vdd,int1,int2,int3,int4}` : The reference voltage to use (internal, external, or VDD) (default is `vdd`).
    - `fsr=value` : The full-scale range value. It must be specified for external reference.
                    This value is automatically set based on the reference voltage for internal and VDD, but may be changed.
    - `bipolar={true|1,false|0}` : If true, enables bipolar mode (default is false).
  */
  class Max1161x : public Converter {

    public:

      /**
         @enum MaxIndex
         @brief Enumeration of the Max1161x models.
         This enum represents the different models of the Max1161x series.
      */
      enum MaxIndex {
        Max11612 = 12,
        Max11613 = 13,
        Max11614 = 14,
        Max11615 = 15,
        Max11616 = 16,
        Max11617 = 17
      };

      /**
         @brief Constructs a Max1161x object with the specified I2C device and parameters.
         @param busId The I2C bus ID (default is I2cDev::Info::defaultBus().id()).
         @param maxId The Max1161x model to use, which can be 12, 13, 14, 15, 16, or 17 (default is Max11615).
         @param referenceId The reference voltage ID (default is DefaultReference).
         @param fsr The full-scale range value (default is 0, which means it will be set based on the Max1161x model).
         @param bipolar If true, enables bipolar mode (default is false).
      */
      Max1161x (int busId = I2cDev::Info::defaultBus().id(), MaxIndex maxId = Max11615, int referenceId = DefaultReference, double fsr = 0, bool bipolar = false);

      /**
         @brief Destructor for the Max1161x class.
      */
      virtual ~Max1161x();

      /**
         @brief Constructs a Max1161x object from a string of parameters.
         @param parameters A string containing the parameters for the PWM configuration, formatted as
                "bus=id:max={12,13,14,15,16,17}:ref={ext,vdd,int1,int2,int3,int4}:fsr=value:bipolar={true|1,false|0}". \n
                The parameters for the constructor registered are:
                - `bus=id` : The I2C bus ID (default is I2cDev::Info::defaultBus().id(), use pinfo to check the default bus ID).
                - `max={12,13,14,15,16,17}` : The Max1161x model to use, which can be 12, 13, 14, 15, 16, or 17 (default is 15 for Max11615).
                - `ref={ext,vdd,int1,int2,int3,int4}` : The reference voltage to use (internal, external, or VDD) (default is `vdd`).
                - `fsr=value` : The full-scale range value. It must be specified for external reference.
                                This value is automatically set based on the reference voltage for internal and VDD, but may be changed.
                - `bipolar={true|1,false|0}` : If true, enables bipolar mode (default is false).
         @note This constructor is used for factory registration and must be implemented by subclasses.
      */
      explicit Max1161x (const std::string &parameters);

      /**
         @brief Returns the name registered for this converter.
         @return The name of the converter as a string.
         @note This method is used for factory registration and must be implemented by subclasses.
      */
      static std::string registeredName() {
        return "max1161x";
      }

    protected:
      /**
         @brief Protected constructor for internal use with a Private implementation.
         @param dd Reference to the Private implementation object.
      */
      class Private;
      Max1161x (Private &dd);

    private:
      /**
         @brief Macro to declare the private implementation pointer for Max1161x.
      */
      PIMP_DECLARE_PRIVATE (Max1161x)
  };
}


/* ========================================================================== */
