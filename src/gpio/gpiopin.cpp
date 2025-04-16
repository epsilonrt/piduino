/* Copyright © 2018 Pascal JEAN, All rights reserved.
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

#include <piduino/gpio.h>
#include <piduino/gpiodevice.h>
#include <exception>
#include "gpiopin_p.h"
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                              Pin Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Pin::Pin (Pin::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  Pin::Pin (Connector *parent, const Descriptor *desc) :
    d_ptr (new Private (this, parent, desc))  {

  }

  // ---------------------------------------------------------------------------
  Pin::~Pin() {

    close();
  }

  // ---------------------------------------------------------------------------
  void
  Pin::analogWrite (long value) {
    PIMP_D (Pin);

    if (d->dac != nullptr) {

      if (!d->dac->isOpen()) {

        d->dac->open();
      }
      d->dac->write (value);
    }
  }

  // ---------------------------------------------------------------------------
  bool Pin::setDac (Converter *dac) {

    if (dac) {

      if (dac->type() == Converter::DigitalToAnalog) {
        PIMP_D (Pin);

        d->dac.reset (dac);
        return true;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void Pin::resetDac () {
    PIMP_D (Pin);

    d->dac.reset ();
  }

  // ---------------------------------------------------------------------------
  Converter *
  Pin::dac() {
    PIMP_D (Pin);

    return d->dac.get();
  }


  // ---------------------------------------------------------------------------
  bool Pin::isOpen() const {
    PIMP_D (const Pin);

    return d->isopen;
  }

  // ---------------------------------------------------------------------------
  Pin::Pull
  Pin::pull () {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      if (isOpen()) {

        if (device()->flags() & GpioDevice::hasPullRead) {

          d->readPull();
        }
        else {

          throw std::invalid_argument ("Unable to read pull resistor");
        }
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
    return d->pull;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::setPull (Pin::Pull p) {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      d->pull = p;
      if (isOpen()) {

        d->writePull();
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
  }

  // ---------------------------------------------------------------------------
  Pin::Mode
  Pin::mode () {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      if (isOpen()) {

        d->readMode();
      }
    }
    return d->mode;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::setMode (Pin::Mode m) {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      d->mode = m;
      if (isOpen()) {

        d->writeMode();
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::setEdge (Pin::Edge e) {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      d->edge = e;
      if (isOpen() && d->useSysFs) {

        d->sysFsSetEdge();
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
  }


  // ---------------------------------------------------------------------------
  Pin::Edge
  Pin::edge() {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      if (isOpen() && d->useSysFs) {

        d->sysFsGetEdge();
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
    return d->edge;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::drive () {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      if (isOpen()) {

        d->readDrive();;
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
    return d->drive;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::setDrive (int drive) {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      d->drive = drive;
      if (isOpen()) {

        d->writeDrive();
      }
    }
    else {

      throw std::domain_error ("This pin is not a GPIO pin");
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::useSysFs() const {
    PIMP_D (const Pin);

    return d->useSysFs;
  }

  // ---------------------------------------------------------------------------
  AccessLayer
  Pin::accessLayer() const {

    return gpio()->accessLayer();
  }

  // ---------------------------------------------------------------------------
  Pin::Type
  Pin::type() const {
    PIMP_D (const Pin);

    return d->descriptor->type;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::logicalNumber () const {
    PIMP_D (const Pin);

    return d->descriptor->num.logical;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::mcuNumber () const {
    PIMP_D (const Pin);

    return d->descriptor->num.mcu;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::systemNumber () const {
    PIMP_D (const Pin);

    if (device()) {

      return d->descriptor->num.system + device()->systemNumberOffset();
    }
    return d->descriptor->num.system;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::number (Pin::Numbering n) const {
    PIMP_D (const Pin);

    switch (n) {
      case NumberingLogical:
        return logicalNumber();
      case NumberingMcu:
        return mcuNumber();
      case NumberingSystem:
        return systemNumber();
      default:
        break;
    }

    throw std::invalid_argument ("Bad pin numbering requested");
  }

  // ---------------------------------------------------------------------------
  int
  Pin::row() const {
    PIMP_D (const Pin);

    return d->descriptor->num.row;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::column() const {
    PIMP_D (const Pin);

    return d->descriptor->num.column;
  }

  // ---------------------------------------------------------------------------
  int
  Pin::physicalNumber () const {
    PIMP_D (const Pin);

    return connector()->pinNumber (row(), column());
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::name (Pin::Mode m) const {
    PIMP_D (const Pin);

    return d->descriptor->name.at (m);
  }

  // ---------------------------------------------------------------------------
  long long
  Pin::id() const {
    PIMP_D (const Pin);

    return d->descriptor->id;
  }

  // ---------------------------------------------------------------------------
  Connector *
  Pin::connector() const {
    PIMP_D (const Pin);

    return d->parent;
  }

  // ---------------------------------------------------------------------------
  Gpio *
  Pin::gpio() const {
    PIMP_D (const Pin);

    return connector()->gpio();
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::typeName() const {
    PIMP_D (const Pin);

    return typeName (type());
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::modeName (Pin::Mode m) const {
    PIMP_D (const Pin);

    return modes().at (m);
  }

  // ---------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  Pin::modes () const {
    PIMP_D (const Pin);

    return device() ? device()->modes() : Private::sysfsmodes;
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::isDebug() const {
    PIMP_D (const Pin);

    return device() ? device()->isDebug() : false;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::setDebug (bool enable) {
    PIMP_D (Pin);

    if (device()) {

      device()->setDebug (enable);
    }
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::modeName() {

    return modeName (mode ());
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::pullName ()  {

    return pullName (pull());
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::name() {

    if (isOpen() && (type() == TypeGpio)) {

      try {

        return name (mode());
      }
      catch (...) {
        // Pas de nom pour le mode concerné
      }
    }
    return name (ModeInput);
  }

  // ---------------------------------------------------------------------------
  void
  Pin::write (bool value) {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (Pin);

      if (d->useSysFs) {

        if (d->sysFsWrite (d->valueFd, value) < 0) {

          throw std::system_error (errno, std::system_category(), __FUNCTION__);
        }
      }
      else {

        device()->write (this, value);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::toggle () {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (Pin);

      if (!d->useSysFs) {

        if (device()->flags() & GpioDevice::hasToggle) {

          device()->toggle (this);
          return;
        }
      }

      write (!read());
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::read () const {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (const Pin);

      if (d->useSysFs) {
        int ret = d->sysFsRead (d->valueFd);

        if (ret < 0) {

          throw std::system_error (errno, std::system_category(), __FUNCTION__);
        }
        return ret > 0;
      }

      return device()->read (this);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::release () {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (Pin);

      if (d->holdMode != ModeUnknown) {

        setMode (d->holdMode);
        if (d->holdMode == ModeOutput) {

          write (d->holdState);
        }
        d->holdMode = ModeUnknown;
      }

      if (d->holdPull != PullUnknown) {

        setPull (d->holdPull);
        d->holdPull = PullUnknown;
      }
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::forceUseSysFs (bool enable) {
    PIMP_D (Pin);

    if (isOpen() && (d->useSysFs != enable)) {

      d->useSysFs = d->sysFsEnable (enable);
    }
    else {

      d->useSysFs = enable;
    }
    return useSysFs();
  }

  // ---------------------------------------------------------------------------
  void
  Pin::waitForInterrupt (Pin::Edge e, int timeout_ms) {

    if (isOpen()) {
      PIMP_D (Pin);
      int ret;

      if (d->firstPolling) {

        setEdge (EdgeNone);
        forceUseSysFs (true);
        setEdge (e);
        // clear pending irq
        if (d->sysFsPoll (d->valueFd, 1) > 0) {

          d->sysFsRead (d->valueFd);
        }
        d->firstPolling = false;
      }

      if (e != d->edge) {

        setEdge (e);
      }

      ret = d->sysFsPoll (d->valueFd, timeout_ms);
      if (ret < 0) {

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }
      else if (ret == 0) {

        throw std::system_error (ETIME, std::generic_category());
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::attachInterrupt (Isr isr, Edge e, void *userData) {
    PIMP_D (Pin);

    if (!d->thread.joinable()) {

      setEdge (EdgeNone);
      forceUseSysFs (true);
      setEdge (e);
      // clear pending irq
      if (d->sysFsPoll (d->valueFd, 1) > 0) {

        d->sysFsRead (d->valueFd);
      }

      // Fetch std::future object associated with promise
      std::future<void> running = d->stopRead.get_future();
      d->thread = std::thread (Private::irqThread, std::move (running), d->valueFd, isr, userData);
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::detachInterrupt() {
    PIMP_D (Pin);

    if (d->thread.joinable()) {

      // Set the value in promise
      d->stopRead.set_value();
      d->thread.join();
    }
  }

  // -----------------------------------------------------------------------------
  //                                   Protected
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  GpioDevice *
  Pin::device() const {

    return connector()->device();
  }

  // ---------------------------------------------------------------------------
  bool Pin::open() {

    if (!isOpen()) {
      PIMP_D (Pin);

      if (type() == TypeGpio) {

        if (d->useSysFs) {

          d->isopen = d->sysFsEnable (true);
        }
        else {

          d->isopen = true;
        }

        if (d->isopen) {

          if (d->mode != ModeUnknown) {

            d->writeMode();
          }
          else {

            d->readMode();
          }

          if (device()) {

            if (d->pull != PullUnknown) {

              d->writePull();
            }
            else {

              d->readPull();
            }
            if (d->drive != -1) {

              d->writeDrive();
            }
            else {

              if (device()->flags() & GpioDevice::hasDrive) {
                d->readDrive();
              }
            }

          }

          if (d->edge != EdgeUnknown) {

            d->writeEdge();
          }
          else {

            d->readEdge();
          }
        }
      }
      else {

        d->isopen = true;
      }
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  Pin::close() {

    if (isOpen()) {
      PIMP_D (Pin);

      if (type() == TypeGpio) {

        detachInterrupt();
        forceUseSysFs (false); // close & unexport
        if (gpio()->releaseOnClose()) {

          release();
        }
      }
      d->isopen = false;
    }
  }

  // -----------------------------------------------------------------------------
  //                                   Static
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::numberingName (Pin::Numbering n)  {

    return Private::numberings.at (n);
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::typeName (Pin::Type t) {

    return Private::types.at (t);
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::edgeName (Pin::Edge e) {

    return Private::edges.at (e);
  }

  // ---------------------------------------------------------------------------
  const std::string &
  Pin::pullName (Pin::Pull p) {

    return Private::pulls.at (p);
  }

  // ---------------------------------------------------------------------------
  const std::map<Pin::Type, std::string> &
  Pin::types () {

    return Private::types;
  }

  // ---------------------------------------------------------------------------
  const std::map<Pin::Numbering, std::string> &
  Pin::numberings () {

    return Private::numberings;
  }

  // ---------------------------------------------------------------------------
  const std::map<Pin::Pull, std::string> &
  Pin::pulls () {

    return Private::pulls;
  }

  // ---------------------------------------------------------------------------
  const std::map<Pin::Edge, std::string> &
  Pin::edges () {

    return Private::edges;
  }

}
/* ========================================================================== */
