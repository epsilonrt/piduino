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
#include <exception>
#include "board.h"

using namespace std;

// ---------------------------------------------------------------------------
//
//                         BoardFamily Class
//
// ---------------------------------------------------------------------------
void BoardFamily::setId (long long id) {
  cppdb::result res =
    _db << "SELECT name "
    "FROM board_family "
    "WHERE id=?"
    << id << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("board_family not found");
  }
  res >> _name;
  _id = id;
}

/* ========================================================================== */
