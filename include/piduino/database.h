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

#ifndef _PIDUINO_DATABASE_H_
#define _PIDUINO_DATABASE_H_

#include <cppdb/frontend.h>

/**
 *  @defgroup piduino_database Database
 *
 *  Les informations concernant la carte cible sont stockées dans une base de 
 *  données.
 *  @{
 */

namespace Piduino {

  class Database : public cppdb::session {
    
    public:
      Database();
      virtual ~Database();
  };
  extern Database db;
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_DATABASE_H_ defined */
