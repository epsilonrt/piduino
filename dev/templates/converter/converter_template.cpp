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
#include <iostream>
#include "converter_template_p.h"
#include "config.h"

using namespace std::chrono;

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                             ConverterTemplate Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  ConverterTemplate::ConverterTemplate (ConverterTemplate::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  ConverterTemplate::ConverterTemplate (/* TODO: complete constructor */) :
    Converter (*new Private (this /* TODO: complete constructor */)) {}

  // ---------------------------------------------------------------------------
  ConverterTemplate::ConverterTemplate (const std::string &parameters) :
    Converter (*new Private (this, parameters)) {}

  // ---------------------------------------------------------------------------
  ConverterTemplate::~ConverterTemplate() = default;



  // ---------------------------------------------------------------------------
  // Register the ConverterTemplate converter with the factory
  REGISTER_CONVERTER (ConverterTemplate, "todo: dac or adc", "todo: parameters");

  // -----------------------------------------------------------------------------
  //
  //                         ConverterTemplate::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  ConverterTemplate::Private::Private (ConverterTemplate *q /* TODO: complete constructor */) :
    Converter::Private (q, /* TODO: complete type */, /* TODO: complete flags */) { /* TODO: initialize members */
  }

  // ---------------------------------------------------------------------------
  ConverterTemplate::Private::Private (ConverterTemplate *q, const std::string &params) :
    Converter::Private (q, /* TODO: complete type */, /* TODO: complete flags */, params) { /* TODO: initialize members */

    if (parameters.empty()) {
      throw std::invalid_argument (EXCEPTION_MSG ("parameters cannot be empty, you must specify a pin number"));
    }

    // TODO: mandatory parameters
    // pin = getPin (parameters[0]); // throw an exception if not found
    if (parameters.size() > 1) {
      // TODO: first optionnal parameter
    }
  }

  // ---------------------------------------------------------------------------
  ConverterTemplate::Private::~Private() = default;
} // namespace Piduino
/* ========================================================================== */
