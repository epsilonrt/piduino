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
#include <piduino/scheduler.h>
#include <piduino/system.h>
#include <exception>
#include <sstream>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "gpiopin_p.h"
#include "config.h"

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                         Pin::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Pin::Private::Private (Pin *q, Connector *parent, const Pin::Descriptor *desc) :
    q_ptr (q), isopen (false), parent (parent), descriptor (desc), holdMode (ModeUnknown),
    holdPull (PullUnknown), holdState (false), mode (ModeUnknown),
    pull (PullUnknown), drive (-1) {}

  // ---------------------------------------------------------------------------
  // Sets the holdPull member to the current pull state if unknown
  void Pin::Private::setHoldPull() {

    if ( (holdPull == PullUnknown) && parent->device()) {
      PIMP_Q (Pin);

      holdPull = parent->device()->pull (q);
    }
  }

  // ---------------------------------------------------------------------------
  // Sets the holdMode member to the current mode if unknown, and saves output state
  void
  Pin::Private:: setHoldMode() {

    if ( (holdMode == ModeUnknown) && parent->device()) {
      PIMP_Q (Pin);

      holdMode = parent->device()->mode (q);
      if (holdMode == ModeOutput) {

        holdState = parent->device()->read (q);
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Writes the current pull configuration to the hardware
  void
  Pin::Private::writePull () {

    if (isGpioDevOpen()) {

      gpiodev->setPull (pull);
    }
    else    if (parent->device()) {
      PIMP_Q (Pin);

      setHoldPull();
      parent->device()->setPull (q, pull);
    }
  }

  // ---------------------------------------------------------------------------
  // Reads the current pull configuration from the hardware
  void
  Pin::Private::readPull ()  {

    if (isGpioDevOpen()) {

      pull = gpiodev->pull ();
    }
    else  if (parent->device()) {

      if (parent->device()->flags() & GpioDevice::hasPullRead) {
        PIMP_Q (Pin);

        pull = parent->device()->pull (q);
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Reads the current mode from the hardware
  void
  Pin::Private::readMode ()  {

    if (isGpioDevOpen()) {

      mode = gpiodev->mode ();
    }
    else if (parent->device()) {
      PIMP_Q (Pin);

      mode = parent->device()->mode (q);
    }
  }

  // ---------------------------------------------------------------------------
  // Writes the current mode to the hardware
  void
  Pin::Private::writeMode () {

    setHoldMode();
    if (isGpioDevOpen()) {

      gpiodev->setMode (mode);
    }
    else {
      PIMP_Q (Pin);

      parent->device()->setMode (q, mode);
    }
  }

  // ---------------------------------------------------------------------------
  // Writes the drive strength to the hardware, if supported
  void
  Pin::Private::writeDrive () {

    if (parent->device()) {

      if (parent->device()->flags() & GpioDevice::hasDrive) {
        PIMP_Q (Pin);

        parent->device()->setDrive (q, drive);
      }
      else {

        throw std::domain_error (EXCEPTION_MSG ("Unable to set the drive strength for this pin, not supported on this board!"));
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Reads the drive strength from the hardware, if supported
  void
  Pin::Private::readDrive ()  {

    if (parent->device()) {

      if (parent->device()->flags() & GpioDevice::hasDrive) {
        PIMP_Q (Pin);

        drive = parent->device()->drive (q);
      }
      else {

        throw std::domain_error (EXCEPTION_MSG ("Unable to get the drive strength for this pin, not supported on this board!"));
      }
    }
  }

  // ---------------------------------------------------------------------------
  // static
  // Map of Pin types to their string representations
  const std::map<Pin::Type, std::string> Pin::Private::types = {
    {TypeGpio, "Gpio"},
    {TypePower, "Power"},
    {TypeUsb, "Usb"},
    {TypeSound, "Sound"},
    {TypeVideo, "Video"},
    {TypeNotConnected, "Not Connected"},
    {TypeUnknown, "Unknown"}
  };

  // ---------------------------------------------------------------------------
  // static
  // Map of Pin numbering schemes to their string representations
  const std::map<Pin::Numbering, std::string> Pin::Private::numberings = {
    {NumberingLogical, "Logical"},
    {NumberingMcu, "Mcu"},
    {NumberingSystem, "System"},
    {NumberingUnknown, "Unknown"}
  };

  // ---------------------------------------------------------------------------
  // static
  // Map of Pin pull configurations to their string representations
  const std::map<Pin::Pull, std::string> Pin::Private::pulls = {
    {PullOff, "off"},
    {PullDown, "down"},
    {PullUp, "up"},
    {PullUnknown, "unk"}
  };

}
/* ========================================================================== */
