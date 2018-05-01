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

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                           Gpio Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Gpio::Gpio (Device * dev, AccessLayer layer) :
    _roc (true), _isopen (false), _accesslayer (layer), _device (dev),
    _numbering (Pin::NumberingUnknown) {

    const std::vector<Connector::Descriptor> & v = device()->descriptor()->connector;

    if (layer == AccessLayerAuto) {

      _accesslayer = dev->preferedAccessLayer();
    }

    // Création des connecteurs à partir des descripteurs
    for (int i = 0; i < v.size(); i++) {

      _connector[v[i].number] = std::make_shared<Connector> (this, & v[i]);
    }
    setNumbering (Pin::NumberingLogical);
  }

// -----------------------------------------------------------------------------
  Gpio::~Gpio() {

    close();
    delete _device;
  }

// -----------------------------------------------------------------------------
  AccessLayer
  Gpio::accessLayer() const {

    return _accesslayer;
  }

// -----------------------------------------------------------------------------
  bool
  Gpio::open () {

    if (!isOpen()) {

      if (_accesslayer & AccessLayerIoMap) {

        if (! device()->open()) {

          return false;
        }
      }

      for (auto c = _connector.cbegin(); c != _connector.cend(); ++c) {

        if (!c->second->open()) {

          return false;
        }
      }

      _isopen = true;
    }

    return isOpen();
  }

// -----------------------------------------------------------------------------
  void
  Gpio::close() {

    if (isOpen()) {

      for (auto c = _connector.cbegin(); c != _connector.cend(); ++c) {

        c->second->close();
      }
      device()->close();
      _isopen = false;
    }
  }

// -----------------------------------------------------------------------------
  bool
  Gpio::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  Pin::Numbering
  Gpio::numbering() const {

    return _numbering;
  }

// -----------------------------------------------------------------------------
  void
  Gpio::setNumbering (Pin::Numbering nb) {

    if (nb != numbering()) {

      _pin.clear();

      for (auto cpair = _connector.cbegin(); cpair != _connector.cend(); ++cpair) {
        const std::shared_ptr<Connector> & conn = cpair->second;
        const std::map<int, std::shared_ptr<Pin>> & pinmap = conn->pin();

        for (auto pair = pinmap.cbegin(); pair != pinmap.cend(); ++pair) {

          std::shared_ptr<Pin> p = pair->second;
          if (p->type() == Pin::TypeGpio) {

            int num = p->number (nb);
            _pin[num] = p;
          }
        }
      }
      _numbering = nb;
    }
  }

// -----------------------------------------------------------------------------
  const std::string &
  Gpio::numberingName () const {

    return Pin::numberingName (numbering());
  }

// -----------------------------------------------------------------------------
  bool
  Gpio::releaseOnClose() const {

    return _roc;
  }

// -----------------------------------------------------------------------------
  void
  Gpio::setReleaseOnClose (bool enable) {

    _roc = enable;
  }

// -----------------------------------------------------------------------------
  const std::string &
  Gpio::name() const {

    return device()->descriptor()->name;
  }

// -----------------------------------------------------------------------------
  int
  Gpio::size() const {

    return _pin.size();
  }

// -----------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Pin>> &
  Gpio::pin() {

    return _pin;
  }

// -----------------------------------------------------------------------------
  Pin &
  Gpio::pin (int number) const {

    return *_pin.at (number).get();
  };

// -----------------------------------------------------------------------------
  int
  Gpio::connectors() const {

    return _connector.size();
  }

// -----------------------------------------------------------------------------
  Connector *
  Gpio::connector (int num) const {

    return _connector.at (num).get();
  }

// -----------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Connector>> &
  Gpio::connector() {
    return _connector;
  }

// -----------------------------------------------------------------------------
  Device *
  Gpio::device() const {

    return _device;
  }

// -----------------------------------------------------------------------------
  bool
  Gpio::isDebug() const {

    return device()->isDebug();
  }

// -----------------------------------------------------------------------------
  void
  Gpio::setDebug (bool enable) {

    device()->setDebug (enable);
  }
}
/* ========================================================================== */
