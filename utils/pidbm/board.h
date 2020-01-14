/* Copyright © 2020 Pascal JEAN, All rights reserved.
 *
 * Piduino pidbm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Piduino pidbm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Piduino pidbm.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <string>
#include <cppdb/frontend.h>

class BoardFamily {
  public:
    BoardFamily (cppdb::session & db, long long id = -1)  : _db (db), _id (id) {}
    inline long long id() const {
      return _id;
    }
    inline const std::string & name() const {
      return _name;
    }
    void setId(long long id);

  private:
    cppdb::session & _db;
    long long _id;
    std::string _name;
};

/* ========================================================================== */
