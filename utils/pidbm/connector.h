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
#include <array>
#include <map>
#include <iostream>
#include <cppdb/frontend.h>

class Pin;
class Connector {
  public:
    enum Alignment {
      Left,
      Right,
      Center
    };

    class Family {
      public:
        Family (cppdb::session & db, long long id = -1);
        void setId (long long id);

        inline long long id() const {
          return _id;
        }
        inline size_t columns() const {
          return _columns;
        }
        inline const std::string & name() const {
          return _name;
        }
        inline cppdb::session & db()  {
          return _db;
        }
      private:
        cppdb::session & _db;
        long long _id;
        std::string _name;
        size_t _columns;
    };

    Connector (cppdb::session & db, long long id = -1, int number = -1);
    Connector (const Connector & src, const std::string & name);

    bool insertPin (size_t row, size_t column, long long pin_id);
    bool updatePin (size_t row, size_t column, long long pin_id);
    void print (std::ostream& os) const;
    size_t pinNumber (size_t row, size_t column) const;
    void setId (long long id);

    inline long long id() const {
      return _id;
    }
    inline long long number() const {
      return _number;
    }
    inline size_t rows() const {
      return _rows;
    }
    inline size_t columns() const {
      return _family.columns();
    }
    inline const Family & family() const {
      return _family;
    }
    inline size_t size() const {
      return _rows * columns();
    }
    inline const std::string & name() const {
      return _name;
    }
    inline cppdb::session & db() const  {
      return _db;
    }
    inline const Pin & pin (size_t number) const {
      return *_pin.at (number).get();
    }

    friend std::ostream& operator<< (std::ostream& os, const Connector & c);

    static std::string toUpper (const std::string & s);

    static std::string formatColumn (const std::string & s,
                                     std::string::size_type w, Alignment a);
  private:

    class Column {
      public:
        std::string::size_type size;
        std::string name;
    };
    void printHline (std::ostream & os) const;
    void printTitle (std::ostream & os) const;
    void printRow (std::ostream & os, int num) const;

    cppdb::session & _db;
    long long _id;
    Family _family;
    long long _number;
    std::string _name;
    size_t _rows;
    std::map<size_t, std::shared_ptr<Pin>> _pin;
    static const std::array<Column, 7> Columns;
};
/* ========================================================================== */
