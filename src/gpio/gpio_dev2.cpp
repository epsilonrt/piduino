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
#include <iostream>
#include <iomanip>
#include <exception>
#include <piduino/clock.h>
#include <piduino/database.h>
#include "gpio_dev2_p.h"
#include "config.h"
#include "gpio_dev2.h"

namespace Piduino {


  // -------------------------------------------------------------------------
  //
  //                        GpioDev2 Class
  //
  // -------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  GpioDev2::GpioDev2 (GpioDev2::Private &dd) : IoDevice (dd) {}

  // ---------------------------------------------------------------------------
  GpioDev2::GpioDev2 (Pin &pin) :
    IoDevice (*new Private (this, &pin)) {}

  // ---------------------------------------------------------------------------
  GpioDev2::~GpioDev2() {

    detachInterrupt();
    close();
  }

  // ---------------------------------------------------------------------------
  bool GpioDev2::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (GpioDev2);

      return d->open (mode);
    }
    return isOpen();
  }

  // -------------------------------------------------------------------------
  Pin::Mode
  GpioDev2::mode () const {
    PIMP_D (const GpioDev2);

    if (isOpen()) {
      Gpio2::LineInfo info;

      if (d->line->getInfo (&info)) {

        if (info.flags & GPIO_V2_LINE_FLAG_OUTPUT) {

          d->mode = Pin::ModeOutput;
        }
        else if (info.flags & GPIO_V2_LINE_FLAG_INPUT) {

          d->mode = Pin::ModeInput;
        }
        else {

          d->mode = Pin::ModeDisabled;
        }
      }
      else {

        d->setError (d->line->errorCode(), d->line->errorMessage());
      }
    }

    return d->mode;
  }

  // -----------------------------------------------------------------------------
  Pin::Pull
  GpioDev2::pull () const {
    PIMP_D (const GpioDev2);

    if (isOpen()) {
      Gpio2::LineInfo info;

      if (d->line->getInfo (&info)) {

        if (info.flags & GPIO_V2_LINE_FLAG_BIAS_PULL_UP) {

          d->pull = Pin::PullUp;
        }
        else if (info.flags & GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN) {

          d->pull = Pin::PullDown;
        }
        else if (info.flags & GPIO_V2_LINE_FLAG_BIAS_DISABLED) {

          d->pull = Pin::PullOff;
        }
      }
      else {

        d->setError (d->line->errorCode(), d->line->errorMessage());
      }
    }

    return d->pull;
  }

  // -------------------------------------------------------------------------
  void
  GpioDev2::setMode (Pin::Mode m) {
    PIMP_D (GpioDev2);

    if (d->modes.find (m) == d->modes.end()) {

      throw std::invalid_argument (EXCEPTION_MSG ("Invalid mode for pin " + std::to_string (d->pin->id())));
    }

    d->mode = m; // Store the mode for later use
    if (isOpen()) {
      Gpio2::LineConfig config = d->line->config();

      switch (m) {
        case Pin::ModeInput:
          config.flags |= GPIO_V2_LINE_FLAG_INPUT;
          config.flags &= ~GPIO_V2_LINE_FLAG_OUTPUT;
          break;
        case Pin::ModeOutput:
          config.flags |= GPIO_V2_LINE_FLAG_OUTPUT;
          config.flags &= ~GPIO_V2_LINE_FLAG_INPUT;
          break;
        case Pin::ModeDisabled:
          config.flags &= ~ (GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_OUTPUT);
          break;
        default:
          break;
      }
      if (!d->line->setConfig (config)) {

        d->setError (d->line->errorCode(), d->line->errorMessage());
      }
    }
  }

  // -------------------------------------------------------------------------
  void
  GpioDev2::setPull (Pin::Pull p) {
    PIMP_D (GpioDev2);

    if (p == Pin::PullUnknown) {
      throw std::invalid_argument (EXCEPTION_MSG ("Invalid pull for pin " + std::to_string (d->pin->id())));
    }

    d->pull = p; // Store the pull configuration for later use
    if (isOpen()) {

      Gpio2::LineConfig config = d->line->config();
      switch (p) {
        case Pin::PullUp:
          config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
          config.flags &= ~ (GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN | GPIO_V2_LINE_FLAG_BIAS_DISABLED);
          break;
        case Pin::PullDown:
          config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
          config.flags &= ~ (GPIO_V2_LINE_FLAG_BIAS_PULL_UP | GPIO_V2_LINE_FLAG_BIAS_DISABLED);
          break;
        case Pin::PullOff:
          config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
          config.flags &= ~ (GPIO_V2_LINE_FLAG_BIAS_PULL_UP | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN);
          break;
        default:
          throw std::invalid_argument (EXCEPTION_MSG ("Invalid pull for pin " + std::to_string (d->pin->id())));
      }
      if (!d->line->setConfig (config)) {
        d->setError (d->line->errorCode(), d->line->errorMessage());
      }
    }
  }

  // -------------------------------------------------------------------------
  void
  GpioDev2::write (bool v) {
    PIMP_D (GpioDev2);

    if (isOpen()) {

      if (d->line->setValue (v)) {

        d->clearError();
      }
      else {

        d->setError (d->line->errorCode(), d->line->errorMessage());
      }
    }
    else {

      // If the device is not open, store the value to be written later
      d->outputValue = v ? 1 : 0; // Store as integer for consistency
    }
  }

  // -------------------------------------------------------------------------
  bool
  GpioDev2::read () const {
    PIMP_D (const GpioDev2);

    return isOpen() ? d->line->getValue() : false;
  }

  // -----------------------------------------------------------------------------
  void GpioDev2::setDebounce (uint32_t debounce_ms) {
    PIMP_D (GpioDev2);

    d->debounce = debounce_ms;
  }

  // -----------------------------------------------------------------------------
  uint32_t GpioDev2::debounce () const {
    PIMP_D (const GpioDev2);

    return d->debounce;
  }

  // -----------------------------------------------------------------------------
  bool GpioDev2::waitForInterrupt (Pin::Edge edge, Pin::Event &event, int timeout_ms) {
    PIMP_D (GpioDev2);

    if (isOpen()) {
      // place here the code to wait for an interrupt on the pin
      if (d->setPinEdge (edge) && d->setDebounce()) {

        d->clearError();
        return d->waitForInterrupt (event, timeout_ms);
      }
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  bool GpioDev2::attachInterrupt (Pin::Isr isr, Pin::Edge edge, void *userData) {
    PIMP_D (GpioDev2);
    if (isOpen()) {
      if (d->setPinEdge (edge) && d->setDebounce()) {

        d->clearError();
        return d->attachInterrupt (isr, userData);
      }
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  void GpioDev2::detachInterrupt () {
    PIMP_D (GpioDev2);

    d->detachInterrupt ();
  }

  // -----------------------------------------------------------------------------
  Gpio2::Chip &GpioDev2::chip() const {
    PIMP_D (const GpioDev2);
    return *d->chip;
  }

  // -----------------------------------------------------------------------------
  Gpio2::Line &GpioDev2::line() const {
    PIMP_D (const GpioDev2);
    return *d->line;
  }

  // -----------------------------------------------------------------------------
  Pin &GpioDev2::pin() const {
    PIMP_D (const GpioDev2);
    return *d->pin;
  }

  // -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &GpioDev2::modes () {

    return Private::modes;
  }

  // -----------------------------------------------------------------------------
  //
  //                         GpioDev2::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // static
  std::map<int, std::shared_ptr<Gpio2::Chip>> GpioDev2::Private::chips;

  // ---------------------------------------------------------------------------
  // static
  const std::map<Pin::Mode, std::string> GpioDev2::Private::modes = {
    { Pin::ModeInput, "in" },
    { Pin::ModeOutput, "out" }
  };

  // ---------------------------------------------------------------------------
  GpioDev2::Private::Private (GpioDev2 *q, Pin *pin) :
    IoDevice::Private (q), pin (pin), chip (nullptr), line (nullptr), debounce (0),
    mode (Pin::ModeUnknown), pull (Pin::PullUnknown), outputValue (-1) {

    // If the chip is not already in the map, create a new instance
    if (chips.find (pin->chipNumber()) == chips.end()) {

      chips[pin->chipNumber()] = std::make_shared<Gpio2::Chip> (System::progName());
      // Note: make_shared call std::bad_alloc if allocation fails
    }

    chip = chips[pin->chipNumber()]; // Get the chip instance from the map
    if (!chip->open (pin->chipNumber())) {

      setError (chip->errorCode(), chip->errorMessage());
    }

    line = std::make_unique<Gpio2::Line> (chip, pin->chipOffset());
    // Note: make_unique call std::bad_alloc if allocation fails
  }

  // ---------------------------------------------------------------------------
  GpioDev2::Private::~Private() {
  }

  // ---------------------------------------------------------------------------
  bool GpioDev2::Private::open (OpenMode mode) {

    if (chip->isOpen ()) {

      if (line->open (pinConfig())) {

        return IoDevice::Private::open (mode);
      }
      else {

        setError (line->errorCode(), line->errorMessage());
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void GpioDev2::Private::close() {

    detachInterrupt(); // Detach any attached interrupt
    if (line->close()) {

      clearError();
    }
    else {

      setError (line->errorCode(), line->errorMessage());
    }
    outputValue = -1; // Reset output value
    IoDevice::Private::close();
  }

  // ---------------------------------------------------------------------------
  Gpio2::LineConfig GpioDev2::Private::pinConfig() const {
    Gpio2::LineConfig config;

    Pin::Mode m = (mode == Pin::ModeUnknown ? pin->mode() : mode);
    Pin::Pull p = (pull == Pin::PullUnknown ? pin->pull() : pull);

    // Initialize the configuration with default flags
    switch (m) {

      case Pin::ModeInput: // Set the pin as input
        config.flags |= GPIO_V2_LINE_FLAG_INPUT;
        break;

      case Pin::ModeOutput: { // Set the pin as output and set the initial value
        bool initialValue = (outputValue >= 0 ? outputValue : pin->read());
        config.flags |= GPIO_V2_LINE_FLAG_OUTPUT;
        config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
        config.attrs[0].attr.values = initialValue;
        config.attrs[0].mask = 1 << 0; // Apply to the first line
        config.num_attrs = 1;
        break;
      }

      default: // current mode not supported, configure as input with pull-down
        config.flags |= GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
        return config;
        break;
    }

    switch (p) {

      case Pin::PullUp:
        config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
        break;
      case Pin::PullDown:
        config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
        break;
      default:
        config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
        break;
    }

    return config;
  }

  // -----------------------------------------------------------------------------
  bool GpioDev2::Private::attachInterrupt (Pin::Isr isr, void *userData) {

    if (!isrThread.joinable()) { // if the ISR thread is not already running

      std::future<void> running = killIsrThread.get_future(); // create a future to control the thread
      isrThread = std::thread (Private::isrFunc, std::move (running), this, isr, userData); // start the ISR thread
    }
    return isrThread.joinable();
  }

  // -----------------------------------------------------------------------------
  void GpioDev2::Private::detachInterrupt () {

    if (isrThread.joinable()) { // if the ISR thread is running

      killIsrThread.set_value(); // signal the thread to stop
      isrThread.join(); // wait for the thread to finish
      isrThread = std::thread(); // reset the thread object
      killIsrThread = std::promise<void>(); // reset the promise for the next call
    }
  }

  // ---------------------------------------------------------------------------
  // static
  void *GpioDev2::Private::isrFunc (std::future<void> run, GpioDev2::Private *d, Pin::Isr userIsr, void *userData) {
    int timeout = 10; // 10 ms

    try {

      while (run.wait_for (std::chrono::milliseconds (1)) == std::future_status::timeout) { // wait for the signal to stop
        Pin::Event event;

        if (d->waitForInterrupt (event, timeout)) { // wait for an interrupt on the pin

          // a new event occurred, call the user ISR
          userIsr (event, userData);
        }
        else if (d->error != 0) {
          // error occurred
          throw std::system_error (d->error, std::system_category(), EXCEPTION_MSG ("waitForInterrupt() failed"));
        }
      }
    }
    catch (std::system_error &e) {

      // handle system errors
      std::cerr << e.what() << "(code " << e.code() << ")" << std::endl;
      std::terminate();
    }
    catch (...) {

    }

    #ifndef NDEBUG
    std::cout << std::endl << __FUNCTION__ << " terminated" << std::endl;
    #endif
    return 0;
  }
}
/* ========================================================================== */
