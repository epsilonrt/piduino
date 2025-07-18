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
#include <dirent.h>
#include <iostream>
#include <iomanip>
#include <sstream>
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
    GpioDevice (*new Private (this)) {
    PIMP_D (Rp1Gpio);

    // place the code to initialize the device
  }

  // ---------------------------------------------------------------------------
  Rp1Gpio::~Rp1Gpio() = default;

  // -------------------------------------------------------------------------
  AccessLayer
  Rp1Gpio::preferedAccessLayer() const {

    // replace this with the prefered access layer for your platform
    return AccessLayerAll;
  }

  // -------------------------------------------------------------------------
  unsigned int
  Rp1Gpio::flags() const {

    return  hasAltRead | hasPullRead;
    // return  hasAltRead;
  }

  // -------------------------------------------------------------------------
  bool
  Rp1Gpio::open() {

    if (!isOpen()) {
      PIMP_D (Rp1Gpio);

      std::string pcieDevice = d->findPCIeDevice();
      if (pcieDevice.empty()) {

        // No PCIe device found, use /dev/gpiomem
        d->iomap.openGpioMem (0, GpioMemBlockSize); // throw std::system_error if error
        d->gpio = d->iomap.io(); // pointer to the GPIO registers
      }
      else {

        // PCIe access
        d->iomap.open (pcieDevice.c_str(), PCIeBlockSize); // throw std::system_error if error
        d->gpio = d->iomap[PCIeGpioOffset]; // pointer to the GPIO registers
      }
      d->pad = &d->gpio[PadsOffset]; // RP1 start adress of map memory for pad
      d->rio  = &d->gpio[RioOffset]; // RP1 start adress of map memory for rio
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

    fsel = d->ctrlReg (pin->mcuNumber()) & RP1_GPIO_CTRL_FUNCSEL_MASK;
    m = Private::fsel2mode.at (fsel);

    if (fsel == FselGpio) {

      uint32_t oe = d->statusReg (pin->mcuNumber()) & RP1_GPIO_STATUS_OE;
      m = (oe == RP1_GPIO_STATUS_OE) ? Pin::ModeOutput : Pin::ModeInput;
    }
    return m;
  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::setMode (const Pin *pin, Pin::Mode m) {
    PIMP_D (Rp1Gpio);

    int p = pin->mcuNumber();
    uint32_t fsel = Private::mode2fsel.at (m);
  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::setPull (const Pin *pin, Pin::Pull p) {
    PIMP_D (Rp1Gpio);

    // int p = pin->mcuNumber();
    // unsigned int pullbits = pads[1 + pin] & RP1_INV_PUD_MASK; // remove bits
    // switch (pud) {
    //   case PUD_OFF:
    //     pads[1 + pin] = pullbits;
    //     break;
    //   case PUD_UP:
    //     pads[1 + pin] = pullbits | RP1_PUD_UP;
    //     break;
    //   case PUD_DOWN:
    //     pads[1 + pin] = pullbits | RP1_PUD_DOWN;
    //     break;
    //   default:
    //     return ; /* An illegal value */
    // }

  }

  // -------------------------------------------------------------------------
  void
  Rp1Gpio::write (const Pin *pin, bool v) {
    PIMP_D (Rp1Gpio);

    /* Assume the pin is already an output */
    d->rio[RP1_RIO_OUT + (v ? RP1_SET_OFFSET : RP1_CLR_OFFSET)] = (1 << pin->mcuNumber());
  }

  // -------------------------------------------------------------------------
  bool
  Rp1Gpio::read (const Pin *pin) const {
    PIMP_D (const Rp1Gpio);

    return !! (d->rio[RP1_RIO_IN] & (1 << pin->mcuNumber()));
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
    switch (pull & RP1_PAD_PULL_MASK) {
      case RP1_PAD_PULL_OFF:
        return Pin::PullOff;
      case RP1_PAD_PULL_DOWN:
        return Pin::PullDown;
      case RP1_PAD_PULL_UP:
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

    pad &= ~RP1_PAD_DRIVE_MASK; // Clear the drive bits
    pad |= (drive << RP1_PAD_DRIVE_LSB) & RP1_PAD_DRIVE_MASK; // Set the new drive value
    d->setPadReg (pin->mcuNumber(), pad); // Write back the modified pad register
  }

  // -----------------------------------------------------------------------------
  int
  Rp1Gpio::drive (const Pin *pin) const {
    PIMP_D (const Rp1Gpio);

    uint32_t drive = d->padReg (pin->mcuNumber());
    drive = (drive & RP1_PAD_DRIVE_MASK) >> RP1_PAD_DRIVE_LSB;
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
    {Pin::ModeAlt5, "alt5"},
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
    GpioDevice::Private (q) {

    // place here the code to initialize the private data
  }

  // ---------------------------------------------------------------------------
  Rp1Gpio::Private::~Private() = default;

  // -------------------------------------------------------------------------
  bool Rp1Gpio::Private::isPCIeFileContain (const char *dirname, const char *filename, const char *content) {
    std::ostringstream fpath;

    fpath << PCIeDevPath << "/" << dirname << "/" << filename;
    std::ifstream f (fpath.str());

    if (f.is_open()) {
      std::string line;

      while (std::getline (f, line)) {

        if (line.find (content) != std::string::npos) {

          return true;
        }
      }
      f.close();
    }
    return false;
  }

  // -------------------------------------------------------------------------
  std::string Rp1Gpio::Private::findPCIeDevice() {
    DIR *dir;
    std::ostringstream res;

    dir = opendir (PCIeDevPath);

    if (dir != NULL) {
      struct dirent *entry;

      while ( (entry = readdir (dir)) != NULL) {

        if (entry->d_type == DT_LNK) {

          if (isPCIeFileContain (entry->d_name, "device", PCIeRp1DeviceId) &&
              isPCIeFileContain (entry->d_name, "vendor", PCIeRp1VendorId)) {

            res.clear();
            res << PCIeDevPath << "/" << entry->d_name << "/" << PCIeRp1File;
            break;
          }
        }
      }

      closedir (dir);
    }

    return res.str();
  }

}
/* ========================================================================== */
