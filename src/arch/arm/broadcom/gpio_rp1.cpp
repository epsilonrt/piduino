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
#include <exception>
#include <piduino/clock.h>
#include <piduino/database.h>

#include "rp1.h"
#include "gpio_rp1_p.h"
#include "config.h"

namespace Piduino {

  using namespace Rp1;

  // -------------------------------------------------------------------------
  //
  //                        Rp1Gpio Class
  //
  // -------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Rp1Gpio::Rp1Gpio (Rp1Gpio::Private &dd) : GpioDevice (dd) {}

  // -------------------------------------------------------------------------
  //
  //                         Mandatory API
  //
  // -------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Rp1Gpio::Rp1Gpio () :
    GpioDevice (*new Private (this)) {}

  // ---------------------------------------------------------------------------
  Rp1Gpio::~Rp1Gpio() = default;

  // -------------------------------------------------------------------------
  AccessLayer
  Rp1Gpio::preferedAccessLayer() const {

    return AccessLayerAll;
  }

  // -------------------------------------------------------------------------
  unsigned int
  Rp1Gpio::flags() const {
    PIMP_D (const Rp1Gpio);

    return  d->flags; // return the flags for the GPIO device
  }

  // -------------------------------------------------------------------------
  bool
  Rp1Gpio::open() {

    if (!isOpen()) {
      PIMP_D (Rp1Gpio);

      std::string pcieDevice = findPCIeDevice();
      if (pcieDevice.empty() || (d->flags & useGpioMem)) {

        // No PCIe device found, use /dev/gpiomem
        d->iomap.openGpioMem (0, GpioBlockSize); // throw std::system_error if error
        d->gpio = d->iomap.io(); // pointer to the GPIO registers
        d->flags |= useGpioMem; // set the flag to use /dev/gpiomem
      }
      else {

        // PCIe access
        d->iomap.open (pcieDevice.c_str(), PCIeBlockSize); // throw std::system_error if error
        d->gpio = d->iomap[GpioOffset]; // pointer to the GPIO registers
        d->flags &= ~useGpioMem; // clear the flag to use /dev/gpiomem
      }
      d->pad = &d->gpio[PadsOffset]; // RP1 start address of map memory for pad
      d->rio  = &d->gpio[RioOffset]; // RP1 start address of map memory for rio
      d->isopen = true;
    }
    return isOpen();
  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::close() {

    if (isOpen()) {
      PIMP_D (Rp1Gpio);

      // place here the code to close the device
      d->iomap.close();
      d->isopen = false;
      d->gpio = nullptr; // reset the pointer to the GPIO registers
      d->pad = nullptr; // reset the pointer to the pad registers
      d->rio  = nullptr; // reset the pointer to the rio registers
    }
  }

  // -------------------------------------------------------------------------
  Pin::Mode
  Rp1Gpio::mode (const Pin *pin) const {
    PIMP_D (const Rp1Gpio);
    Pin::Mode m;
    uint32_t fsel;

    int p = pin->mcuNumber(); // Get the GPIO number from the Pin object
    fsel = d->ctrlReg (p) & GPIO_CTRL_FUNCSEL_MASK;
    m = Private::fsel2mode.at (fsel);

    if (fsel == FselGpio) {

      uint32_t oe = d->statusReg (pin->mcuNumber()) & GPIO_STATUS_OE;
      m = (oe == GPIO_STATUS_OE) ? Pin::ModeOutput : Pin::ModeInput;
    }
    else if ( ( (m == Pin::ModeAlt0) && ( (p == 12) || (p == 13))) ||
              ( (m == Pin::ModeAlt3) && ( (p == 18) || (p == 19)))) {
      m = Pin::ModePwm;
    }
    return m;
  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::setMode (const Pin *pin, Pin::Mode m) {
    PIMP_D (Rp1Gpio);

    int p = pin->mcuNumber();
    bool isPwm = (m == Pin::ModePwm);

    if (m == Pin::ModePwm) { // TODO: replace with a database pin_has_pwm table lookup

      if ( (p == 12) || (p == 13)) { // Ino26 and Ino23

        m = Pin::ModeAlt0;
      }
      else if ( (p == 18) || (p == 19)) { // Ino1 and Ino24

        m = Pin::ModeAlt3;
      }
      else {

        throw std::invalid_argument (EXCEPTION_MSG ("ModePwm can only be set for GPIO12, GPIO13, GPIO18 or GPIO19"));
      }
    }

    uint32_t fsel = Private::mode2fsel.at (m); // Get the function select value for the mode

    switch (m) {
      case Pin::ModeInput:
      // case Pin::ModeAlt5:
        d->setPadReg (p, (p <= 8) ? GPIO_PAD_DEFAULT_0TO8 : GPIO_PAD_DEFAULT_FROM9);
        d->setCtrlReg (p, FselGpio | GPIO_CTRL_DEBOUNCE_DEFAULT);
        d->rio[GPIO_RIO_OE + GPIO_RIO_CLR_OFFSET] = 1 << p;
        break;
      case Pin::ModeOutput:
        d->setPadReg (p, (p <= 8) ? GPIO_PAD_DEFAULT_0TO8 : GPIO_PAD_DEFAULT_FROM9);
        d->setCtrlReg (p, FselGpio | GPIO_CTRL_DEBOUNCE_DEFAULT);
        d->rio[GPIO_RIO_OE + GPIO_RIO_SET_OFFSET] = 1 << p;
        break;
      case Pin::ModeDisabled:
        d->setPadReg (p, (p <= 8) ? GPIO_PAD_HW_0TO8 : GPIO_PAD_HW_FROM9);
        d->setCtrlReg (p, GPIO_CTRL_IRQRESET | FselNoneHw | GPIO_CTRL_DEBOUNCE_DEFAULT);
        d->rio[GPIO_RIO_OE + GPIO_RIO_CLR_OFFSET] = 1 << p;
        break;
      case Pin::ModeAlt0:
      case Pin::ModeAlt3:
        if (isPwm) {
          // For PWM mode, set the function select bits and set the pad register to hardware mode
          d->setPadReg (p, (p <= 8) ? GPIO_PAD_DEFAULT_0TO8 : GPIO_PAD_DEFAULT_FROM9); // enable output
          // Set the control register to the function select value and enable debounce
          d->setCtrlReg (p, fsel | GPIO_CTRL_DEBOUNCE_DEFAULT);
          break;
        }
      case Pin::ModeAlt1:
      case Pin::ModeAlt2:
      case Pin::ModeAlt4:
      case Pin::ModeAlt6:
      case Pin::ModeAlt7:
      case Pin::ModeAlt8: {
        // Alternate functions, set the function select bits
        // and set the pad register to hardware mode
        d->setPadReg (p, (p <= 8) ? GPIO_PAD_HW_0TO8 : GPIO_PAD_HW_FROM9);
        uint32_t ctrl = d->ctrlReg (p) & ~GPIO_CTRL_FUNCSEL_MASK; // Clear the function select bits
        ctrl |= fsel; // Set the new function select value
        d->setPadReg (p, (p <= 8) ? GPIO_PAD_HW_0TO8 : GPIO_PAD_HW_FROM9); // Set the pad register to hardware mode
        d->setCtrlReg (p, ctrl); // Write back the modified control register
        d->rio[GPIO_RIO_OE + GPIO_RIO_CLR_OFFSET] = 1 << p; // Clear the output enable bit for the pin
      }
      break;
      default:
        break;
    }
  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::setPull (const Pin *pin, Pin::Pull p) {
    PIMP_D (Rp1Gpio);

    uint32_t pull = d->padReg (pin->mcuNumber()) & ~GPIO_PAD_PULL_MASK; // remove bits
    switch (p) {
      case Pin::PullOff:
        pull |= GPIO_PAD_PULL_OFF;
        break;
      case Pin::PullDown:
        pull |= GPIO_PAD_PULL_DOWN;
        break;
      case Pin::PullUp:
        pull |= GPIO_PAD_PULL_UP;
        break;
      default:
        return; // An illegal value or unknown pull configuration
    }
    d->setPadReg (pin->mcuNumber(), pull); // write back the modified pad register
  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::write (const Pin *pin, bool v) {
    PIMP_D (Rp1Gpio);

    /* Assume the pin is already an output */
    d->rio[GPIO_RIO_OUT + (v ? GPIO_RIO_SET_OFFSET : GPIO_RIO_CLR_OFFSET)] = (1 << pin->mcuNumber());
  }

    // -------------------------------------------------------------------------
  void
  Rp1Gpio::toggle (const Pin *pin) {
    PIMP_D (Rp1Gpio);

    /* Assume the pin is already an output */
    d->rio[GPIO_RIO_OUT + GPIO_RIO_XOR_OFFSET] = (1 << pin->mcuNumber());
  }
  // -------------------------------------------------------------------------
  bool
  Rp1Gpio::read (const Pin *pin) const {
    PIMP_D (const Rp1Gpio);

    return !! (d->rio[GPIO_RIO_IN] & (1 << pin->mcuNumber()));
  }

  // -------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  Rp1Gpio::modes() const {

    return Private::modes;
  }

  // -------------------------------------------------------------------------
  //
  //                             Optional API
  //
  // -------------------------------------------------------------------------

  // -----------------------------------------------------------------------------
  Pin::Pull
  Rp1Gpio::pull (const Pin *pin) const {
    PIMP_D (const Rp1Gpio);

    uint32_t pull = d->padReg (pin->mcuNumber());
    switch (pull & GPIO_PAD_PULL_MASK) {
      case GPIO_PAD_PULL_OFF:
        return Pin::PullOff;
      case GPIO_PAD_PULL_DOWN:
        return Pin::PullDown;
      case GPIO_PAD_PULL_UP:
        return Pin::PullUp;
      default:
        break;
    }
    return Pin::PullUnknown; // Unknown or uninitialized pull configuration
  }

  // -----------------------------------------------------------------------------
  void
  Rp1Gpio::setDrive (const Pin *pin, int drive) {
    PIMP_D (Rp1Gpio);
    uint32_t pad = d->padReg (pin->mcuNumber());

    pad &= ~GPIO_PAD_DRIVE_MASK; // Clear the drive bits
    pad |= (drive << GPIO_PAD_DRIVE_LSB) & GPIO_PAD_DRIVE_MASK; // Set the new drive value
    d->setPadReg (pin->mcuNumber(), pad); // Write back the modified pad register
  }

  // -----------------------------------------------------------------------------
  int
  Rp1Gpio::drive (const Pin *pin) const {
    PIMP_D (const Rp1Gpio);

    uint32_t drive = d->padReg (pin->mcuNumber());
    drive = (drive & GPIO_PAD_DRIVE_MASK) >> GPIO_PAD_DRIVE_LSB;
    return drive;
  }

  // -----------------------------------------------------------------------------
  //
  //                         Rp1Gpio::Private Class
  //
  // -----------------------------------------------------------------------------

  // -------------------------------------------------------------------------
  // static
  const std::map<Pin::Mode, std::string> Rp1Gpio::Private::modes = {

    {Pin::ModeInput, "in"},
    {Pin::ModeOutput, "out"},
    {Pin::ModeAlt0, "alt0"},
    {Pin::ModeAlt1, "alt1"},
    {Pin::ModeAlt2, "alt2"},
    {Pin::ModeAlt3, "alt3"},
    {Pin::ModeAlt4, "alt4"},
    // {Pin::ModeAlt5, "alt5"},
    {Pin::ModeAlt6, "alt6"},
    {Pin::ModeAlt7, "alt7"},
    {Pin::ModeAlt8, "alt8"},
    {Pin::ModeDisabled, "off"},
    {Pin::ModePwm, "pwm"}
  };

  // -------------------------------------------------------------------------
  // static
  const std::map<uint32_t, Pin::Mode> Rp1Gpio::Private::fsel2mode = {
    {FselGpio, Pin::ModeInput},
    {FselGpio, Pin::ModeOutput},
    {FselAlt0, Pin::ModeAlt0},
    {FselAlt1, Pin::ModeAlt1},
    {FselAlt2, Pin::ModeAlt2},
    {FselAlt3, Pin::ModeAlt3},
    {FselAlt4, Pin::ModeAlt4},
    {FselAlt5, Pin::ModeAlt5},
    {FselAlt6, Pin::ModeAlt6},
    {FselAlt7, Pin::ModeAlt7},
    {FselAlt8, Pin::ModeAlt8},
    {FselNoneHw, Pin::ModeDisabled}, // Hardware function (default, mask)
    {FselNone, Pin::ModeUnknown}
  };

  // -------------------------------------------------------------------------
  // static
  const std::map<Pin::Mode, uint32_t> Rp1Gpio::Private::mode2fsel = {
    {Pin::ModeInput, FselGpio},
    {Pin::ModeOutput, FselGpio},
    {Pin::ModeAlt0, FselAlt0},
    {Pin::ModeAlt1, FselAlt1},
    {Pin::ModeAlt2, FselAlt2},
    {Pin::ModeAlt3, FselAlt3},
    {Pin::ModeAlt4, FselAlt4},
    {Pin::ModeAlt5, FselAlt5},
    {Pin::ModeAlt6, FselAlt6},
    {Pin::ModeAlt7, FselAlt7},
    {Pin::ModeAlt8, FselAlt8},
    {Pin::ModeDisabled, FselNoneHw},
    {Pin::ModeUnknown, FselNone}
  };

  // ---------------------------------------------------------------------------
  Rp1Gpio::Private::Private (Rp1Gpio *q) :
    GpioDevice::Private (q), flags (hasAltRead | hasPullRead | hasDrive | hasToggle) {

  }

  // ---------------------------------------------------------------------------
  Rp1Gpio::Private::~Private() = default;

}
/* ========================================================================== */
