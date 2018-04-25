/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * The Piduino Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The Piduino Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PIDUINO_MANUFACTURER_H_
#define _PIDUINO_MANUFACTURER_H_

#include <string>

/**
 *  @defgroup piduino_manufacturer Fabricants
 *
 *  Ce module fournit informations concernant les fabricants
 *  @{
 */

namespace Piduino {

  class Manufacturer {
    public:

      enum Id {

        Broadcom = 0,
        Sony,
        Qisda,
        Egoman,
        Embest,
        Allwinner,
        FriendlyARM,
        // Add New item here and update manufactuer table in database !

        //---
        Unknown = -1
      };

      Manufacturer (Id i = Unknown) {
        setId (i);
      }
      virtual ~Manufacturer() {}

      inline Id id() const {
        return _id;
      }

      inline const std::string &name() const {
        return _name;
      }
      
      void setId (Id i);

    private:
      Id _id;
      std::string _name;
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_MANUFACTURER_H_ defined */
