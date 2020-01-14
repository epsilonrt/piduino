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
#include <algorithm>
#include <sstream>
#include "connector.h"
#include "pin.h"

using namespace std;

// -----------------------------------------------------------------------------
//
//                     Connector::Family Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Connector::Family::Family (cppdb::session & d, long long i) : _db (d), _id (i) {

  if (i >= 0) {
    setId (i);
  }
}

// -----------------------------------------------------------------------------
void Connector::Family::setId (long long i) {

  cppdb::result res =
    _db << "SELECT name,columns "
    "FROM connector_family "
    "WHERE id=?"
    << i << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Connector Family not found");
  }
  _id = i;
  res >> _name >> _columns;
}

// -----------------------------------------------------------------------------
//
//                         Connector Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
const std::array<Connector::Column, 7> Connector::Columns = {{
    {5, "sOc"},
    {5, "iNo"},
    {5, "sYs"},
    {10, "Name"},
    {6, "Type"},
    {6, "Id"},
    {4, "Ph"}
  }
};

// ---------------------------------------------------------------------------
std::string Connector::toUpper (const std::string & s) {
  std::string out (s);

  std::transform (out.begin(), out.end(), out.begin(),
                  std::ptr_fun<int, int> (std::toupper));
  return out;
}

// ---------------------------------------------------------------------------
std::string Connector::formatColumn (const std::string & s,
                                     std::string::size_type w, Alignment a) {
  std::string in (s);
  std::string out (w, ' ');
  std::string::size_type pos;

  if (w < 3) {
    w = 3;
  }
  if (in.size() > (w - 2)) {

    in.resize (w - 2);
  }

  switch (a) {
    case Left:
      pos = 1;
      break;
    case Right:
      pos = out.size() - in.size() - 1;
      break;
    case Center:
      pos = (out.size() - in.size()) / 2;
      break;
  }

  out.replace (pos, in.size(), s);
  return out;
}

// -----------------------------------------------------------------------------
Connector::Connector (cppdb::session & d, long long i, int n) : _db (d), _id (i),
  _family (d) {

  _number = n < 0 ? _id : n;
  if (i >= 0) {
    setId (i);
  }
}

// -----------------------------------------------------------------------------
Connector::Connector (const Connector & src, const std::string & n) :
  _db (src.db()), _family (src.db(), src.family().id()), _number (src.number()),
  _name (n), _rows (src.rows()) {
  cppdb::statement st;

  st = _db << "INSERT INTO connector(name,rows,connector_family_id) VALUES(?,?,?)"
       << _name << _rows << _family.id();
  st.exec();
  _id = st.last_insert_id();

  for (size_t i = 1; i <= src.size(); i++) {
    std::shared_ptr<Pin> p;

    p = std::make_shared<Pin> (*this, src.pin (i));
    _pin[p->number()] = p;
    st = _db << "INSERT INTO connector_has_pin(connector_id,pin_id,row,column) VALUES(?,?,?,?)"
         << _id << p->id() << p->row() << p->column();
    st.exec();
  }
}

// -----------------------------------------------------------------------------
void Connector::setId (long long i) {
  long long fid;
  cppdb::result res =
    _db << "SELECT name,rows,connector_family_id "
    "FROM connector "
    "WHERE id=?"
    << i << cppdb::row;

  if (res.empty()) {

    throw std::invalid_argument ("Connector not found");
  }

  res >> _name >> _rows >> fid;
  _family.setId (fid);

  _pin.clear();
  res =
    _db << "SELECT pin_id,row,column "
    "FROM connector_has_pin "
    "WHERE connector_id=?"
    << _id;
  while (res.next()) {
    long long i;
    int r, c;
    std::shared_ptr<Pin> p;

    res >> i >> r >> c;
    p = std::make_shared<Pin> (*this, i, r, c);
    _pin[p->number()] = p;
  }
}

// ---------------------------------------------------------------------------
bool Connector::insertPin (size_t r, size_t c, long long pin_id) {
  size_t n = pinNumber (r, c);

  if (_pin.count (n) == 0) {
    cppdb::statement st;
    std::shared_ptr<Pin> p;

    st = _db << "INSERT INTO connector_has_pin(connector_id,pin_id,row,column) VALUES(?,?,?,?)" << _id << pin_id << r << c;
    st.exec();

    p = std::make_shared<Pin> (*this, pin_id, r, c);
    _pin[p->number()] = p;
  }
  return false;
}

// ---------------------------------------------------------------------------
bool Connector::updatePin (size_t r, size_t c, long long pin_id) {
  size_t n = pinNumber (r, c);

  if (_pin.count (n) == 0) {

    return insertPin (r, c, pin_id);
  }
  else {
    cppdb::statement st;

    st = _db << "UPDATE connector_has_pin SET pin_id=? WHERE "
         "connector_id=? AND row=? AND column=?"
         << pin_id << _id  << r << c;
    st.exec();
    _pin[n]->setId (pin_id);
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
void Connector::print (std::ostream& os) const {
  std::string::size_type width = 0;
  std::ostringstream buf;

  for (auto c : Connector::Columns) {

    width += c.size;
  }
  width = (width + Columns.size() + 1) * columns();

  // header
  os.width (width);

  buf << _name << " (#" << _number << ")";
  os << std::setw ( (width + buf.str().size()) / 2 + 1)  << Connector::toUpper (buf.str()) << std::endl;

  if (size() >= rows()) {
    printTitle (os);
    // pins
    for (size_t i = 1; i <= size(); i += columns()) {

      printRow (os, i);
    }
    // footer
    if (_rows > 6) {

      printTitle (os);
    }
    else {

      printHline (os);
    }
  }
  else {
    cout << "insufficient number of pins to display the connector pinout." << endl;
  }
}

// ---------------------------------------------------------------------------
void
Connector::printHline (std::ostream & os) const {
  os << '+';
  for (auto c : Columns) {
    os << std::string (c.size, '-') << '+';
  }
  if (columns() > 1) {

    os << '+';
    for (int i = Columns.size() - 1; i >= 0 ; --i) {
      os << std::string (Columns[i].size, '-') << '+';
    }
  }
  os << std::endl;
}

// ---------------------------------------------------------------------------
void
Connector::printTitle (std::ostream & os) const {

  printHline (os);
  os << '|';
  for (auto c : Columns) {

    os << formatColumn (c.name, c.size, Center) << '|';
  }
  if (columns() > 1) {

    os << '|';
    for (int i = Columns.size() - 1; i >= 0 ; --i) {

      os << formatColumn (Columns[i].name, Columns[i].size, Center) << '|';
    }
  }
  os << std::endl;
  printHline (os);
}


// ---------------------------------------------------------------------------
void
Connector::printRow (std::ostream & os, int number) const {
  unsigned int i = 0;

  for (size_t c = 0; c < columns(); c++) {
    string sOc, iNo, sYs;

    const Pin & p = pin (number++);
    if (p.type().id() == Pin::Type::Gpio) {
      sOc = to_string (p.mcuNumber());
      iNo = to_string (p.logicalNumber());
      sYs = to_string (p.systemNumber());
    }

    if ( (c % 2) == 0) {

      os << '|';
      os << formatColumn (sOc, Columns[i++].size, Right) << '|';
      os << formatColumn (iNo, Columns[i++].size, Right) << '|';
      os << formatColumn (sYs, Columns[i++].size, Right) << '|';
      os << formatColumn (p.name(), Columns[i++].size, Right) << '|';
      os << formatColumn (p.type().name(), Columns[i++].size, Right) << '|';
      os << formatColumn (to_string (p.id()), Columns[i++].size, Right) << '|';
      os << formatColumn (to_string (p.number()), Columns[i++].size, Right) << '|';
    }
    else {
      os << '|';
      os << formatColumn (to_string (p.number()), Columns[--i].size, Left) << '|';
      os << formatColumn (to_string (p.id()), Columns[--i].size, Left) << '|';
      os << formatColumn (p.type().name(), Columns[--i].size, Left) << '|';
      os << formatColumn (p.name(), Columns[--i].size, Left) << '|';
      os << formatColumn (sYs, Columns[--i].size, Left) << '|';
      os << formatColumn (iNo, Columns[--i].size, Left) << '|';
      os << formatColumn (sOc, Columns[--i].size, Left) << '|';
    }
  }
  os << endl;
}

// ---------------------------------------------------------------------------
size_t Connector::pinNumber (size_t row, size_t column) const {

  return (row - 1) * columns() + column;
}

// ---------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, const Connector & c)  {
  c.print (os);
  return os;
}
/* ========================================================================== */
