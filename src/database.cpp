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
#include <sstream>
#include <cstdlib>
#include <piduino/database.h>

namespace Piduino {

// -----------------------------------------------------------------------------
  Database::Database() : cppdb::session() {
    std::ostringstream ci;
    std::string dbpath(PIDUINO_DBPATH);
    const char * dbpath_env = std::getenv ("PIDUINO_DBPATH");
    if (dbpath_env) {
      
      dbpath = std::string(dbpath_env);
    }
    ci << "sqlite3:db=" << dbpath;
    cppdb::session::open(ci.str());
  }

// -----------------------------------------------------------------------------
  Database::~Database() {
    
    cppdb::session::close();
  }

// -----------------------------------------------------------------------------
  Database db;
}
/* ========================================================================== */
