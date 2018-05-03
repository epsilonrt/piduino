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

#include <piduino/gpio.h>
#include <piduino/gpiodevice.h>
#include <exception>
#include <iomanip>
#include <algorithm>
#include <sstream>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                          Connector Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Connector::Connector (Gpio * parent, Descriptor * desc) :
    _isopen (false), _parent (parent), _descriptor (desc) {
    int num;
    std::vector<Pin::Descriptor> & v = _descriptor->pin;

    for (int i = 0; i < v.size(); i++) {

      num = pinNumber (v[i].num.row, v[i].num.column);
      _pin[num] = std::make_shared<Pin> (this, & v[i]);
    }
  }

// -----------------------------------------------------------------------------
  Connector::~Connector() {

    close();
  }

// -----------------------------------------------------------------------------
  bool Connector::open() {

    if (!isOpen()) {

      for (auto p = _pin.begin(); p != _pin.end(); ++p) {

        if (!p->second->open()) {

          return false;
        }
      }
      _isopen = true;
    }
    return isOpen();
  }

// -----------------------------------------------------------------------------
  void Connector::close() {

    if (isOpen()) {

      for (auto p = _pin.begin(); p != _pin.end(); ++p) {

        p->second->close();
      }
      _isopen = false;
    }
  }

// -----------------------------------------------------------------------------
  bool Connector::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  const std::string &
  Connector::name() const {

    return _descriptor->name;
  }
// -----------------------------------------------------------------------------
  int
  Connector::number() const {

    return _descriptor->number;
  }

// -----------------------------------------------------------------------------
  int
  Connector::rows() const {

    return _descriptor->rows;
  }

// -----------------------------------------------------------------------------
  int
  Connector::columns() const {

    return _descriptor->family.columns();
  }

// -----------------------------------------------------------------------------
  const Connector::Family & Connector::family() const {

    return _descriptor->family;
  }

// -----------------------------------------------------------------------------
  int
  Connector::id() const {

    return _descriptor->id;
  }

// -----------------------------------------------------------------------------
  int
  Connector::size() const {

    return _pin.size();
  }

// -----------------------------------------------------------------------------
  int
  Connector::pinNumber (int row, int column) const {

    return _descriptor->family.pinNumber (row, column);
  }

// -----------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Pin>> &
  Connector::pin()  {

    return _pin;
  }

// -----------------------------------------------------------------------------
  Pin &
  Connector::pin (int number) const {

    return *_pin.at (number).get(); // throw out_of_range if not exist
  }

// -----------------------------------------------------------------------------
  Gpio *
  Connector::gpio() const {

    return _parent;
  }

// -----------------------------------------------------------------------------
  Device *
  Connector::device() const {

    return gpio()->device();
  }

// -----------------------------------------------------------------------------
  bool
  Connector::isDebug() const {

    return device()->isDebug();
  }

// -----------------------------------------------------------------------------
  void
  Connector::setDebug (bool enable) {

    device()->setDebug (enable);
  }

// -----------------------------------------------------------------------------
//                      Opérateur << vers ostream
// -----------------------------------------------------------------------------
  namespace ConnectorOstream {

// -----------------------------------------------------------------------------
    enum Alignment {
      Left,
      Right,
      Center
    };

// -----------------------------------------------------------------------------
    class Field {
      public:
        std::string::size_type size;
        std::string name;
    };

// -----------------------------------------------------------------------------
    const std::array<Field, 7> _field = {{
        {5, "sOc"},
        {5, "sIo"},
        {10, "Name"},
        {6, "Mode"},
        {6, "Pull"},
        {3, "V"},
        {4, "Ph"}
      }
    };

// -----------------------------------------------------------------------------
    std::string
    toUpper (const std::string & s) {
      std::string out (s);

      std::transform (out.begin(), out.end(), out.begin(),
                      std::ptr_fun<int, int> (std::toupper));
      return out;
    }

// -----------------------------------------------------------------------------
    std::string
    format (const std::string & s, std::string::size_type w, Alignment a) {
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
  }

  using namespace ConnectorOstream;
// -----------------------------------------------------------------------------
  void
  Connector::printHline (std::ostream & os) const {

    os << '+';
    for (unsigned int i = 0; i < _field.size(); i++) {
      if ( (_field[i].name != "Pull") || (device()->flags() & Device::hasPullRead)) {
        os << std::string (_field[i].size, '-') << '+';
      }
    }
    if (columns() > 1) {

      os << '+';
      for (int i = _field.size() - 1; i >= 0 ; --i) {
        if ( (_field[i].name != "Pull") || (device()->flags() & Device::hasPullRead)) {
          os << std::string (_field[i].size, '-') << '+';
        }
      }
    }
    os << std::endl;
  }

// -----------------------------------------------------------------------------
  void
  Connector::printTitle (std::ostream & os) const {
    printHline (os);

    os << '|';
    for (unsigned int i = 0; i < _field.size(); i++) {

      if ( (_field[i].name != "Pull") || (device()->flags() & Device::hasPullRead)) {
        os << format (_field[i].name, _field[i].size, Center) << '|';
      }
    }
    if (columns() > 1) {

      os << '|';
      for (int i = _field.size() - 1; i >= 0 ; --i) {

        if ( (_field[i].name != "Pull") || (device()->flags() & Device::hasPullRead)) {
          os << format (_field[i].name, _field[i].size, Center) << '|';
        }
      }
    }
    os << std::endl;

    printHline (os);
  }


// -----------------------------------------------------------------------------
  void
  Connector::printRow (std::ostream & os, int number) const {
    std::array<std::string, 5> s;
    unsigned int i = 0;

    Pin * p = &pin (number++);
    os << '|';
    if (p->type() == Pin::TypeGpio) {
      s[0] = std::to_string (p->mcuNumber());
      s[1] = std::to_string (p->logicalNumber());
      s[2] = toUpper (p->modeName());
      if (device()->flags() & Device::hasPullRead) {
        s[3] = toUpper (p->pullName());
      }
      if (p->mode() != Pin::ModeDisabled)  {
        if ( (p->mode() == Pin::ModeInput) || (p->mode() == Pin::ModeOutput) ||
             (device()->flags() & Device::hasAltRead)) {
          s[4] = std::to_string (p->read());
        }
      }
    }
    os << format (s[0], _field[i++].size, Right) << '|';
    os << format (s[1], _field[i++].size, Right) << '|';
    os << format (p->name(), _field[i++].size, Right) << '|';
    os << format (s[2], _field[i++].size, Right) << '|';
    if (device()->flags() & Device::hasPullRead) {
      os << format (s[3], _field[i].size, Right) << '|';
    }
    i++;
    os << format (s[4], _field[i++].size, Right) << '|';
    os << format (std::to_string (p->physicalNumber()), _field[i++].size, Right) << '|';

    if (columns() > 1) {

      p = &pin (number);

      os << '|';
      if (p->type() == Pin::TypeGpio) {
        s[0] = std::to_string (p->mcuNumber());
        s[1] = std::to_string (p->logicalNumber());
        s[2] = toUpper (p->modeName());
        if (device()->flags() & Device::hasPullRead) {
          s[3] = toUpper (p->pullName());
        }
        s[4].clear();
        if (p->mode() != Pin::ModeDisabled)  {
          if ( (p->mode() == Pin::ModeInput) || (p->mode() == Pin::ModeOutput) ||
               (device()->flags() & Device::hasAltRead)) {

            s[4] = std::to_string (p->read());
          }
        }
      }
      else {

        for (auto & n : s) {
          n.clear();
        }
      }
      os << format (std::to_string (p->physicalNumber()), _field[--i].size, Left) << '|';
      os << format (s[4], _field[--i].size, Left) << '|';
      --i;
      if (device()->flags() & Device::hasPullRead) {
        os << format (s[3], _field[i].size, Left) << '|';
      }
      os << format (s[2], _field[--i].size, Left) << '|';
      os << format (p->name(), _field[--i].size, Left) << '|';
      os << format (s[1], _field[--i].size, Left) << '|';
      os << format (s[0], _field[--i].size, Left) << '|';
    }
    os << std::endl;
  }

// -----------------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& os, const Connector * c)  {
    std::string::size_type width = 0;
    std::ostringstream buf;

    if (c->columns() > 2) {

      throw std::out_of_range ("the number of columns must not exceed 2.");
    }

    for (unsigned int i = 0; i < _field.size(); i++) {

      if ( (_field[i].name != "Pull") || (c->device()->flags() & Device::hasPullRead)) {
        width += _field[i].size;
      }
    }
    width = (width + _field.size() - (c->device()->flags() & Device::hasPullRead ? 0 : 1)) * c->columns();

    // entête
    os.width (width);

    buf << c->name() << " (#" << c->number() << ")";
    os << std::setw ( (width + buf.str().size()) / 2 + 1)  << toUpper (buf.str()) << std::endl;
    c->printTitle (os);
    // broches
    for (int i = 1; i <= c->size(); i += c->columns()) {

      c->printRow (os, i);
    }
    // pied de page
    if (c->rows() > 6) {

      c->printTitle (os);
    }
    else {

      c->printHline (os);
    }
    return os;
  }
}
/* ========================================================================== */
