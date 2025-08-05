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

namespace Piduino {

  /**
     @class ConverterTemplate
     @brief Provides a template for creating converter classes in the Piduino library.

  */
  class ConverterTemplate : public Converter {

    public:
      /**
         @brief Constructs a ConverterTemplate object for the specified pin, with optional range and frequency.
      */
      ConverterTemplate (/* TODO: complete constructor */);

      /**
         @brief Destructor for the ConverterTemplate class.
      */
      virtual ~ConverterTemplate();

      /**
         @brief Constructs a ConverterTemplate object from a string of parameters.
         @param parameters A string containing the parameters for the PWM configuration, formatted as "pin:range:freq".
         @note This constructor is used for factory registration and must be implemented by subclasses.
      */
      ConverterTemplate (const std::string &parameters);

      /**
         @brief Returns the name registered for this converter.
         @return The name of the converter as a string.
         @note This method is used for factory registration and must be implemented by subclasses.
      */
      static std::string registeredName() {
        return "converter_template"; // TODO: replace with actual name
      }

      // TODO: Add other methods as needed for the converter functionality
      // Note that these methods will be specific and not usable from a Converter pointer

    protected:
      /**
         @brief Protected constructor for internal use with a Private implementation.
         @param dd Reference to the Private implementation object.
      */
      class Private;
      ConverterTemplate (Private &dd);

    private:
      /**
         @brief Macro to declare the private implementation pointer for ConverterTemplate.
      */
      PIMP_DECLARE_PRIVATE (ConverterTemplate)
  };
}

/* ========================================================================== */
