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
#include <vector>
#include <iostream>
#include <cppdb/frontend.h>
#include "board.h"

class Connector;
class Gpio {
  public:
    Gpio (cppdb::session & db, long long id);
    void print (std::ostream& os) const;

    inline long long id() const {
      return _id;
    }
    inline int size() const {
      return _connector.size();
    }
    inline const std::string & name() const {
      return _name;
    }
    Connector & connector (int index) {
      return *_connector.at (index).get();
    }
    const Connector & connector (int index) const {
      return *_connector.at (index).get();
    }

    friend std::ostream& operator<< (std::ostream& os, const Gpio & c);

  private:
    cppdb::session & _db;
    long long _id;
    BoardFamily _board_family;
    std::string _name;
    std::vector<std::shared_ptr<Connector>> _connector;
};
/* ========================================================================== */
