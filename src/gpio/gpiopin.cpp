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

        d->readPull();
      }
    }
    else {

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
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

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
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

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
    }
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

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
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

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
    }
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

    return d->descriptor->num.system;
  }

  // ---------------------------------------------------------------------------
  int Pin::chipNumber() const {
    PIMP_D (const Pin);

    return d->descriptor->num.chip;
  }

  // ---------------------------------------------------------------------------
  int Pin::chipOffset() const {
    PIMP_D (const Pin);

    return d->descriptor->num.offset;
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

    throw std::invalid_argument (EXCEPTION_MSG ("Bad pin numbering requested"));
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
  const std::map<Pin::Mode, std::string> &
  Pin::modes () const {
    PIMP_D (const Pin);

    if (type() != TypeGpio) {

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
    }
    if (d->isGpioDevEnabled() || device() == nullptr) {

      return d->gpiodev->modes();
    }

    return device()->modes();
  }

  // ---------------------------------------------------------------------------
  void
  Pin::write (bool value) {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (Pin);

      if (d->isGpioDevOpen()) {

        d->gpiodev->write (value);
        if (d->gpiodev->error()) {

          throw std::system_error (d->gpiodev->error(), std::system_category(), EXCEPTION_MSG ("Failed to write value to GPIO pin"));
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

      if (!d->isGpioDevOpen()) {

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

      if (d->isGpioDevOpen()) {

        return d->gpiodev->read();
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
  void Pin::waitForInterrupt (Edge edge, int debounce_ms, Event &event, int timeout_ms) {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (Pin);

      if (!d->enableGpioDev()) {

        throw std::domain_error (EXCEPTION_MSG ("Failed to enable GPIO device for waiting interrupt"));
      }

      if (debounce_ms >= 0) {

        d->gpiodev->setDebounce (debounce_ms);
      }
      d->gpiodev->read(); // clear pending irq

      if (!d->gpiodev->waitForInterrupt (edge, event, timeout_ms)) {

        if (d->gpiodev->error()) {

          throw std::system_error (d->gpiodev->error(), std::system_category(), EXCEPTION_MSG ("Failed to wait for GPIO interrupt"));
        }
      }

    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::waitForInterrupt (Pin::Edge edge, Event &event, int timeout_ms) {

    waitForInterrupt (edge, -1, event, timeout_ms);
  }

  // ---------------------------------------------------------------------------
  void
  Pin::waitForInterrupt (Pin::Edge edge, int timeout_ms) {
    Event event;

    waitForInterrupt (edge, -1, event, timeout_ms);
  }

  // ---------------------------------------------------------------------------
  void
  Pin::attachInterrupt (Isr isr, Edge edge, int debounce_ms, void *userData) {

    if (isOpen() && (type() == TypeGpio)) {
      PIMP_D (Pin);

      if (!d->enableGpioDev()) {

        throw std::domain_error (EXCEPTION_MSG ("Failed to enable GPIO device for waiting interrupt"));
      }
      if (debounce_ms >= 0) {

        d->gpiodev->setDebounce (debounce_ms);
      }
      d->gpiodev->read(); // clear pending irq
      if (!d->gpiodev->attachInterrupt (isr, edge, userData)) {

        throw std::system_error (d->gpiodev->error(), std::system_category(), EXCEPTION_MSG ("Failed to attach GPIO interrupt"));
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::attachInterrupt (Isr isr, Edge edge, void *userData) {

    attachInterrupt (isr, edge, -1, userData);
  }
  
  // ---------------------------------------------------------------------------
  void
  Pin::detachInterrupt() {
    PIMP_D (Pin);

    if (d->isGpioDevEnabled()) {

      d->gpiodev->detachInterrupt();
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

        if (d->isGpioDevEnabled()) {

          d->isopen = d->gpiodev->open();
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
        d->enableGpioDev (false);
        if (gpio()->releaseOnClose()) {

          release();
        }
      }
      d->isopen = false;
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::isGpioDevEnabled() const {
    PIMP_D (const Pin);
    return d->isGpioDevEnabled();
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::enableGpioDev (bool enable) {
    PIMP_D (Pin);

    if (type() == TypeGpio) {

      return d->enableGpioDev (enable);
    }
    else {

      throw std::domain_error (EXCEPTION_MSG ("This pin is not a GPIO pin"));
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

}
/* ========================================================================== */
