/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/

#include <piduino/gpiodevice.h>
#include <piduino/database.h>
#ifdef __ARM_ARCH
#include  "arch/arm/allwinner/gpio_hx.h"
#include  "arch/arm/broadcom/gpio_bcm2835.h"
#endif /* __ARM_ARCH */
#include "gpio_p.h"
#include "config.h"

namespace Piduino {
  // -----------------------------------------------------------------------------
  //
  //                        Gpio::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Gpio::Private::Private (Gpio *q, long long gpioDatabaseId, SoC::Family::Id socFamilyId, AccessLayer layer) :
    q_ptr (q), roc (true), isopen (false), accesslayer (layer), device (nullptr),
    numbering (Pin::NumberingUnknown) {

    descriptor = std::make_shared<Descriptor> (gpioDatabaseId);

    switch (socFamilyId) {
        #if PIDUINO_DRIVER_BCM2835 != 0
      case SoC::Family::BroadcomBcm2835 :
        device = new Bcm2835Gpio();
        break;
        #endif /* PIDUINO_DRIVER_BCM2835 */
        #if PIDUINO_DRIVER_ALLWINNERH != 0
      case SoC::Family::AllwinnerH :
        device = new AllWinnerHxGpio();
        break;
        #endif /* PIDUINO_DRIVER_ALLWINNERH */
      default:
        throw std::system_error (ENOTSUP, std::system_category(),
                                 "It seems that this system is not supported !");
        break;
    }

    if (layer == AccessLayerAuto) {

      accesslayer = device->preferedAccessLayer();
    }
  }

  // ---------------------------------------------------------------------------
  Gpio::Private::~Private()  {
    delete device;
  }

  // -----------------------------------------------------------------------------
  //
  //                           Gpio Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Gpio::Gpio (Gpio::Private &dd) : d_ptr (&dd) {}

  // ---------------------------------------------------------------------------
  Gpio::Gpio (long long gpioDatabaseId, SoC::Family::Id socFamilyId, AccessLayer layer) :
    d_ptr (new Private (this, gpioDatabaseId, socFamilyId, layer)) {
    PIMP_D (Gpio);

    // Création des connecteurs à partir des descripteurs
    std::vector<Connector::Descriptor> &v = d->descriptor->connector;
    for (int i = 0; i < v.size(); i++) {

      d->connector[v[i].number] = std::make_shared<Connector> (this, & v[i]);
    }
    // code below throws a segfault, why ?
    // for (auto c: d->descriptor->connector) {

    //   d->connector[c.number] = std::make_shared<Connector> (this, &c);
    // }
    setNumbering (Pin::NumberingLogical);
  }

  // ---------------------------------------------------------------------------
  Gpio::Gpio (AccessLayer layer) :
    Gpio (db.board().gpioId(), db.board().soc().family().id(), layer) {}

  // ---------------------------------------------------------------------------
  Gpio::~Gpio() {

    close();
  }

  // ---------------------------------------------------------------------------
  AccessLayer
  Gpio::accessLayer() const {
    PIMP_D (const Gpio);

    return d->accesslayer;
  }

  // ---------------------------------------------------------------------------
  bool
  Gpio::open () {

    if (!isOpen()) {
      PIMP_D (Gpio);

      if (d->accesslayer & AccessLayerIoMap) {

        if (! device()->open()) {

          return false;
        }
      }

      for (auto c = d->connector.cbegin(); c != d->connector.cend(); ++c) {

        if (!c->second->open()) {

          return false;
        }
      }

      d->isopen = true;
    }

    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  Gpio::close() {

    if (isOpen()) {
      PIMP_D (Gpio);

      for (auto c = d->connector.cbegin(); c != d->connector.cend(); ++c) {

        c->second->close();
      }
      device()->close();
      d->isopen = false;
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Gpio::isOpen() const {
    PIMP_D (const Gpio);

    return d->isopen;
  }

  // ---------------------------------------------------------------------------
  Pin::Numbering
  Gpio::numbering() const {
    PIMP_D (const Gpio);

    return d->numbering;
  }

  // ---------------------------------------------------------------------------
  void
  Gpio::setNumbering (Pin::Numbering nb) {

    if (nb != numbering()) {
      PIMP_D (Gpio);

      d->pin.clear();

      for (auto cpair = d->connector.cbegin(); cpair != d->connector.cend(); ++cpair) {
        const std::shared_ptr<Connector> &conn = cpair->second;
        const std::map<int, std::shared_ptr<Pin>> &pinmap = conn->pin();

        for (auto pair = pinmap.cbegin(); pair != pinmap.cend(); ++pair) {

          std::shared_ptr<Pin> p = pair->second;
          if (p->type() == Pin::TypeGpio) {

            int num = p->number (nb);
            d->pin[num] = p;
          }
        }
      }
      d->numbering = nb;
    }
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Gpio::numberingName () const {
    PIMP_D (const Gpio);

    return Pin::numberingName (numbering());
  }

  // ---------------------------------------------------------------------------
  bool
  Gpio::releaseOnClose() const {
    PIMP_D (const Gpio);

    return d->roc;
  }

  // ---------------------------------------------------------------------------
  void
  Gpio::setReleaseOnClose (bool enable) {
    PIMP_D (Gpio);

    d->roc = enable;
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Gpio::name() const {
    PIMP_D (const Gpio);

    return d->descriptor->name;
  }

  // ---------------------------------------------------------------------------
  long long
  Gpio::id() const {
    PIMP_D (const Gpio);

    return d->descriptor->id;
  }

  // ---------------------------------------------------------------------------
  int
  Gpio::size() const {
    PIMP_D (const Gpio);

    return d->pin.size();
  }

  // ---------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Pin>> &
  Gpio::pin() {
    PIMP_D (Gpio);

    return d->pin;
  }

  // ---------------------------------------------------------------------------
  Pin &
  Gpio::pin (int number) const {
    PIMP_D (const Gpio);

    return *d->pin.at (number).get();
  };

  // ---------------------------------------------------------------------------
  Pin *
  Gpio::pin (long long id) const {
    PIMP_D (const Gpio);

    for (auto pair = d->pin.cbegin(); pair != d->pin.cend(); ++pair) {
      std::shared_ptr<Pin> p = pair->second;

      if (p->id() == id) {

        return p.get();
      }
    }

    return 0;
  }

  // ---------------------------------------------------------------------------
  int
  Gpio::connectors() const {
    PIMP_D (const Gpio);

    return d->connector.size();
  }

  // ---------------------------------------------------------------------------
  Connector *
  Gpio::connector (int num) const {
    PIMP_D (const Gpio);

    return d->connector.at (num).get();
  }

  // ---------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Connector>> &
  Gpio::connector() {
    PIMP_D (Gpio);

    return d->connector;
  }

  // ---------------------------------------------------------------------------
  GpioDevice *
  Gpio::device() const {
    PIMP_D (const Gpio);

    return d->device;
  }

  // ---------------------------------------------------------------------------
  bool
  Gpio::isDebug() const {

    return device()->isDebug();
  }

  // ---------------------------------------------------------------------------
  void
  Gpio::setDebug (bool enable) {

    device()->setDebug (enable);
  }
}
/* ========================================================================== */
