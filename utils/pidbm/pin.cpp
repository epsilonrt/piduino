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
#include "pin.h"
#include "connector.h"

using namespace std;

// ---------------------------------------------------------------------------
//
//                             Pin Class
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
void Pin::setId (long long i) {
  int t;
  cppdb::result res =
    _parent.db() << "SELECT pin_type_id "
    "FROM pin "
    "INNER JOIN pin_type ON pin_type_id=pin_type.id "
    "WHERE pin.id=?"
    << i << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Pin not found");
  }
  res >> t;
  _type = Type (t);
  _id = i;

  if (_type.id() == Type::Gpio) {

    res =
      _parent.db() << "SELECT logical_num,mcu_num,system_num "
      "FROM pin_number "
      "WHERE pin_id=?"
      << _id << cppdb::row;

    if (res.empty()) {

      throw std::invalid_argument ("Pin numbers not found");
    }
    res >> _logicalNumber >> _mcuNumber >> _systemNumber;
  }
}

// ---------------------------------------------------------------------------
Pin::Pin (Connector & p, long long i, size_t r, size_t c) : _parent (p),
  _row (r), _column (c), _logicalNumber (-1), _mcuNumber (-1), _systemNumber (-1) {

  setId (i);
}

// ---------------------------------------------------------------------------
Pin::Pin (Connector & p, const Pin & src) :
  Pin (p, src.id(), src.row(), src.column()) {
}

// ---------------------------------------------------------------------------
std::string Pin::name (int mode) const {
  string n;

  cppdb::result res =
    _parent.db() << "SELECT name "
    "FROM pin_name "
    "INNER JOIN pin_has_name ON pin_name_id=pin_name.id "
    "WHERE pin_id=? AND pin_mode_id=?"
    << _id << mode << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Pin name not found");
  }
  res >> n;
  return n;
}

// ---------------------------------------------------------------------------
int Pin::number() const {

  return _parent.pinNumber (_row, _column);
}

// ---------------------------------------------------------------------------
const std::map<int, std::string> Pin::Type::_names = {
  {Gpio , "Gpio"},
  {Power , "Pwr"},
  {Usb , "Usb"},
  {Sound , "Snd"},
  {Video , "Vdo"},
  {NotConnected , "NC"},
  {Net , "Net"},
  {Unknown , "Unk"},
};

/* ========================================================================== */
