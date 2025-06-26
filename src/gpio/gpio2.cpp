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
#include <array>
#include <iostream>
#include <piduino/gpio2.h>

namespace Gpio2 {

  struct GpioFlag {
    char *name;
    unsigned long long mask;
    GpioFlag (const char *name, unsigned long long mask)
      : name (const_cast<char *> (name)), mask (mask) {}
  };

  std::array <GpioFlag, 10> flagnames = {
    GpioFlag ("used", GPIO_V2_LINE_FLAG_USED),
    GpioFlag ("input", GPIO_V2_LINE_FLAG_INPUT),
    GpioFlag ("output", GPIO_V2_LINE_FLAG_OUTPUT),
    GpioFlag ("active-low", GPIO_V2_LINE_FLAG_ACTIVE_LOW),
    GpioFlag ("open-drain", GPIO_V2_LINE_FLAG_OPEN_DRAIN),
    GpioFlag ("open-source", GPIO_V2_LINE_FLAG_OPEN_SOURCE),
    GpioFlag ("pull-up", GPIO_V2_LINE_FLAG_BIAS_PULL_UP),
    GpioFlag ("pull-down", GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN),
    GpioFlag ("bias-disabled", GPIO_V2_LINE_FLAG_BIAS_DISABLED),
    GpioFlag ("clock-realtime", GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME),
  };

  // --------------------------------------------------------------------------
  void LineInfo::printAttributes (std::ostream &os) const {
    int k = 0; // Counter for printed items
    std::string sep = "";

    for (int i = 0; i < flagnames.size(); i++) {

      if (this->flags & flagnames[i].mask) {


        if (k > 0)  {
          sep = ", ";
        }
        os << sep << flagnames[i].name;
        k++;
      }
    }

    if ( (this->flags & GPIO_V2_LINE_FLAG_EDGE_RISING) &&
         (this->flags & GPIO_V2_LINE_FLAG_EDGE_FALLING)) {
      os << sep << "both-edges";
      k++;
    }
    else if (this->flags & GPIO_V2_LINE_FLAG_EDGE_RISING) {
      os << sep << "rising-edge";
      k++;
    }
    else if (this->flags & GPIO_V2_LINE_FLAG_EDGE_FALLING) {
      os << sep << "falling-edge";
      k++;
    }

    if (k > 0) {
      sep = ", ";
    }

    for (int i = 0; i < this->num_attrs; i++) {
      if (this->attrs[i].id == GPIO_V2_LINE_ATTR_ID_DEBOUNCE) {
        os << sep << "debounce_period=" << this->attrs[i].debounce_period_us << "usec";
      }
    }

    if (k == 0) {
      os << "none";
    }
  }

  // --------------------------------------------------------------------------
  std::ostream &operator<< (std::ostream &os, const LineInfo &info) {

    info.printAttributes (os);
    return os;
  }
}
