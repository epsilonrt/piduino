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
#include <map>
#include <iostream>
#include <cppdb/frontend.h>

class Connector;
class Pin {
  public:
    class Type {
      public:
        enum {
          Gpio = 0,
          Power,
          Usb,
          Sound,
          Video,
          NotConnected,
          Net,
          Unknown = -1
        };
        Type (int i = Unknown) : _id (i) {}
        inline const std::string & name() const {
          return _names.at (_id);
        }
        inline int id() const {
          return _id;
        }
      private:
        int _id;
        static const std::map<int, std::string> _names;
    };

    Pin (Connector & parent, long long id, size_t row, size_t column);
    Pin (Connector & parent, const Pin & src);
    void setId (long long i);
    std::string name (int mode = 0) const;
    int number() const;

    inline long long id() const {
      return _id;
    }
    inline const Type & type() const {
      return _type;
    }
    inline int mcuNumber() const {
      return _mcuNumber;
    }
    inline int logicalNumber() const {
      return _logicalNumber;
    }
    inline int systemNumber() const {
      return _systemNumber;
    }
    inline size_t row() const {
      return _row;
    }
    inline size_t column() const {
      return _column;
    }

  private:
    Connector & _parent;
    long long _id;
    size_t _row;
    size_t _column;
    Type _type;
    int _logicalNumber;
    int _mcuNumber;
    int _systemNumber;
};
/* ========================================================================== */
