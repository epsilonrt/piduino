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
  // Constructor: Initializes Pin with a given Private implementation
  Pin::Pin (Pin::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  // Constructor: Initializes Pin with parent Connector and Descriptor
  Pin::Pin (Connector *parent, const Descriptor *desc) :
    d_ptr (new Private (this, parent, desc))  {

  }

  // ---------------------------------------------------------------------------
  // Destructor: Closes the pin on destruction
  Pin::~Pin() {

    close();
  }

  // ---------------------------------------------------------------------------
  // Writes an analog value to the pin using DAC if available
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
  // Sets the DAC converter for the pin
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
  // Resets the DAC converter
  void Pin::resetDac () {
    PIMP_D (Pin);

    d->dac.reset ();
  }

  // ---------------------------------------------------------------------------
  // Returns the DAC converter
  Converter *
  Pin::dac() {
    PIMP_D (Pin);

    return d->dac.get();
  }

  // ---------------------------------------------------------------------------
  // Returns true if the pin is open
  bool Pin::isOpen() const {
    PIMP_D (const Pin);

    return d->isopen;
  }

  // ---------------------------------------------------------------------------
  // Returns the current pull configuration of the pin
  Pin::Pull
  Pin::pull () const {
    PIMP_D (const Pin);

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
  // Sets the pull configuration of the pin
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
  // Returns the current mode of the pin
  Pin::Mode
  Pin::mode () const {
    PIMP_D (const Pin);

    if (type() == TypeGpio) {

      if (isOpen()) {

        d->readMode();
      }
    }
    return d->mode;
  }

  // ---------------------------------------------------------------------------
  // Sets the mode of the pin
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
  // Returns the drive strength of the pin
  int
  Pin::drive () {
    PIMP_D (const Pin);

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
  // Sets the drive strength of the pin
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
  // Returns the access layer of the pin
  AccessLayer
  Pin::accessLayer() const {

    return gpio()->accessLayer();
  }

  // ---------------------------------------------------------------------------
  // Returns the type of the pin
  Pin::Type
  Pin::type() const {
    PIMP_D (const Pin);

    return d->descriptor->type;
  }
  // ---------------------------------------------------------------------------
  // Returns the flags of the pin
  // These flags indicate the capabilities of the pin's device
  unsigned int
  Pin::flags() const {

    if (type() == TypeGpio && device()) {

      return device()->flags();
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  // Returns the logical number of the pin
  int
  Pin::logicalNumber () const {
    PIMP_D (const Pin);

    return d->descriptor->num.logical;
  }

  // ---------------------------------------------------------------------------
  // Returns the MCU number of the pin
  int
  Pin::mcuNumber () const {
    PIMP_D (const Pin);

    return d->descriptor->num.mcu;
  }

  // ---------------------------------------------------------------------------
  // Returns the system number of the pin
  int
  Pin::systemNumber () const {
    PIMP_D (const Pin);

    return d->descriptor->num.system;
  }

  // ---------------------------------------------------------------------------
  // Returns the chip number of the pin
  int Pin::chipNumber() const {
    PIMP_D (const Pin);

    return d->descriptor->num.chip;
  }

  // ---------------------------------------------------------------------------
  // Returns the chip offset of the pin
  int Pin::chipOffset() const {
    PIMP_D (const Pin);

    return d->descriptor->num.offset;
  }

  // ---------------------------------------------------------------------------
  // Returns the pin number according to the specified numbering scheme
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
  // Returns the row of the pin on the connector
  int
  Pin::row() const {
    PIMP_D (const Pin);

    return d->descriptor->num.row;
  }

  // ---------------------------------------------------------------------------
  // Returns the column of the pin on the connector
  int
  Pin::column() const {
    PIMP_D (const Pin);

    return d->descriptor->num.column;
  }

  // ---------------------------------------------------------------------------
  // Returns the physical number of the pin on the connector
  int
  Pin::physicalNumber () const {
    PIMP_D (const Pin);

    return connector()->pinNumber (row(), column());
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the pin for the specified mode
  const std::string &
  Pin::name (Pin::Mode m) const {
    PIMP_D (const Pin);

    return d->descriptor->name.at (m);
  }

  // ---------------------------------------------------------------------------
  // Returns the unique identifier of the pin
  long long
  Pin::id() const {
    PIMP_D (const Pin);

    return d->descriptor->id;
  }

  // ---------------------------------------------------------------------------
  // Returns the parent connector of the pin
  Connector *
  Pin::connector() const {
    PIMP_D (const Pin);

    return d->parent;
  }

  // ---------------------------------------------------------------------------
  // Returns the GPIO controller associated with the pin
  Gpio *
  Pin::gpio() const {
    PIMP_D (const Pin);

    return connector()->gpio();
  }

  // ---------------------------------------------------------------------------
  // Returns the type name of the pin
  const std::string &
  Pin::typeName() const {
    PIMP_D (const Pin);

    return typeName (type());
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the specified mode
  const std::string &
  Pin::modeName (Pin::Mode m) const {
    PIMP_D (const Pin);

    return modes().at (m);
  }

  // ---------------------------------------------------------------------------
  // Returns true if debug is enabled for the pin's device
  bool
  Pin::isDebug() const {
    PIMP_D (const Pin);

    return device() ? device()->isDebug() : false;
  }

  // ---------------------------------------------------------------------------
  // Enables or disables debug for the pin's device
  void
  Pin::setDebug (bool enable) {
    PIMP_D (Pin);

    if (device()) {

      device()->setDebug (enable);
    }
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the current mode
  const std::string &
  Pin::modeName() {

    return modeName (mode ());
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the current pull configuration
  const std::string &
  Pin::pullName ()  {

    return pullName (pull());
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the pin for the current mode, or default if not open
  const std::string &
  Pin::name() const {

    if (isOpen() && (type() == TypeGpio)) {

      try {

        return name (mode());
      }
      catch (...) {
        // No name for the current mode
      }
    }
    return name (ModeInput);
  }

  // ---------------------------------------------------------------------------
  // Returns the map of available modes for the pin
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
  // Writes a digital value to the pin
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
  // Toggles the digital value of the pin
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
  // Reads the digital value from the pin
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
  // Releases the pin, restoring previous mode and pull state if held
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
  // Waits for an interrupt event on the pin with debounce and timeout
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
  // Waits for an interrupt event on the pin with timeout
  void
  Pin::waitForInterrupt (Pin::Edge edge, Event &event, int timeout_ms) {

    waitForInterrupt (edge, -1, event, timeout_ms);
  }

  // ---------------------------------------------------------------------------
  // Waits for an interrupt event on the pin with timeout, no event object
  void
  Pin::waitForInterrupt (Pin::Edge edge, int timeout_ms) {
    Event event;

    waitForInterrupt (edge, -1, event, timeout_ms);
  }

  // ---------------------------------------------------------------------------
  // Attaches an interrupt service routine to the pin with debounce
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
  // Attaches an interrupt service routine to the pin without debounce
  void
  Pin::attachInterrupt (Isr isr, Edge edge, void *userData) {

    attachInterrupt (isr, edge, -1, userData);
  }

  // ---------------------------------------------------------------------------
  // Detaches the interrupt service routine from the pin
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
  // Returns the GpioDevice associated with the pin
  GpioDevice *
  Pin::device() const {

    return connector()->device();
  }

  // ---------------------------------------------------------------------------
  // Opens the pin for use, initializing mode, pull, and drive as needed
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
  // Closes the pin, detaching interrupts and releasing resources
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
  // Returns true if the GPIO device is enabled for this pin
  bool
  Pin::isGpioDevEnabled() const {
    PIMP_D (const Pin);
    return d->isGpioDevEnabled();
  }

  // ---------------------------------------------------------------------------
  // Enables or disables the GPIO device for this pin
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
  // Returns the name of the specified numbering scheme
  const std::string &
  Pin::numberingName (Pin::Numbering n)  {

    return Private::numberings.at (n);
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the specified pin type
  const std::string &
  Pin::typeName (Pin::Type t) {

    return Private::types.at (t);
  }

  // ---------------------------------------------------------------------------
  // Returns the name of the specified pull configuration
  const std::string &
  Pin::pullName (Pin::Pull p) {

    return Private::pulls.at (p);
  }

  // ---------------------------------------------------------------------------
  // Returns the map of pin types
  const std::map<Pin::Type, std::string> &
  Pin::types () {

    return Private::types;
  }

  // ---------------------------------------------------------------------------
  // Returns the map of pin numbering schemes
  const std::map<Pin::Numbering, std::string> &
  Pin::numberings () {

    return Private::numberings;
  }

  // ---------------------------------------------------------------------------
  // Returns the map of pull configurations
  const std::map<Pin::Pull, std::string> &
  Pin::pulls () {

    return Private::pulls;
  }

  // ---------------------------------------------------------------------------
  // Overloaded operator to print Pin information
  std::ostream &operator<< (std::ostream &os, const Pin &p) {

    if (p.type() == Pin::TypeGpio) {
      os << "Pin iNo#" << p.logicalNumber() << " (" << p.name() << ") -----" << std::endl;
    }
    else {
      os << "Pin " <<  p.name() << " -----" << std::endl;
    }
    os   << "   Database Id   : " << p.id() << std::endl
         << "   Row           : " << p.row() << std::endl
         << "   Column        : " << p.column();
    if (p.type() == Pin::TypeGpio) {
      os   << std::endl
           << "   Logical Number: " << p.logicalNumber() << std::endl
           << "   Mcu Number    : " << p.mcuNumber() << std::endl
           << "   System Number : " << p.systemNumber() << std::endl
           << "   Chip Number   : " << p.chipNumber() << std::endl
           << "   Chip Offset   : " << p.chipOffset();
    }
    return os;
  }

} // namespace Piduino
/* ========================================================================== */
