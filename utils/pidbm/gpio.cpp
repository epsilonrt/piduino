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
#include <iomanip>
#include "gpio.h"
#include "connector.h"

using namespace std;

// ---------------------------------------------------------------------------
//
//                         Gpio Class
//
// ---------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Gpio::Gpio (cppdb::session & db, long long id) : _db (db), _id (id),
  _board_family (db) {
  long long f;

  cppdb::result res =
    _db << "SELECT name,board_family_id "
    "FROM gpio "
    "WHERE id=?"
    << _id << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("gpio not found");
  }
  res >> _name >> f;
  _board_family.setId (f);

  res =
    _db << "SELECT num,connector_id "
    "FROM gpio_has_connector "
    "WHERE gpio_id=?"
    << _id;

  while (res.next()) {
    long long i;
    int num;
    std::shared_ptr<Connector> c;

    res >> num >> i;
    c = std::make_shared<Connector> (_db, i, num);
    _connector.push_back (c);
  }
}

// -----------------------------------------------------------------------------
void Gpio::print (std::ostream& os) const {
  std::string::size_type width = 100;
  std::ostringstream buf;

  // entête
  os.width (width);

  buf << _name << " (#" << _id << ")";
  os << std::setw ( (width + buf.str().size()) / 2 + 1)  << Connector::toUpper (buf.str()) << std::endl;
  for (int i = 0; i < size(); i++) {
    os << endl << connector(i);
  }
}

// ---------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, const Gpio & g)  {
  g.print (os);
  return os;
}

/* ========================================================================== */
