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

#include <piduino/gpio.h>

namespace Piduino {

  class Gpio::Private {

    public:
      Private (Gpio *q, long long gpioDatabaseId, SoC::Family::Id socFamilyId, AccessLayer layer);
      virtual ~Private();

      Gpio *const q_ptr;
      bool roc; // Release On Close
      bool isopen;
      AccessLayer accesslayer;
      GpioDevice *device;  // Accès à la couche matérielle
      Pin::Numbering numbering; // Numérotation en cours
      std::shared_ptr<Gpio::Descriptor> descriptor;
      std::map<int, std::shared_ptr<Pin>> pin; // Broches uniquement GPIO
      std::map<int, std::shared_ptr<Connector>> connector; // Connecteurs avec toutes les broches physiques

      PIMP_DECLARE_PUBLIC (Gpio)
  };
}

/* ========================================================================== */
