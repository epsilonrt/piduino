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
    holdPull (PullUnknown), holdState (false), useSysFs (false),
    valueFd (-1), firstPolling (true), edge (EdgeUnknown), mode (ModeUnknown),
    pull (PullUnknown), dac (0), drive (-1) {

    if ( (parent->gpio()->accessLayer() & AccessLayerIoMap) != AccessLayerIoMap) {

      useSysFs = true;
    }

  }

  // ---------------------------------------------------------------------------
  void Pin::Private::setHoldPull() {

    if ( (holdPull == PullUnknown) && parent->device()) {
      PIMP_Q (Pin);

      holdPull = parent->device()->pull (q);
    }
  }

  // ---------------------------------------------------------------------------
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
  bool
  Pin::Private::sysFsEnable (bool enable) {

    if (enable) {

      setHoldMode();
      setHoldPull();
      sysFsExport (true);
      return sysFsOpen();
    }

    sysFsClose();
    sysFsExport (false);
    return false;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsExport (bool enable) {

    if ( (descriptor->type == TypeGpio) && (enable != sysFsIsExport())) {
      PIMP_Q (Pin);
      std::ostringstream fn;
      std::ofstream f;

      if (q->systemNumber() < 0) {
        std::ostringstream msg;

        msg << "Unknown pin number in SysFs for " << q->name (ModeInput) << " !";
        throw std::invalid_argument (msg.str());
      }

      if (enable) {

        // Export
        fn << syspath << "/export";

      }
      else {

        // Unexport
        if (sysFsFileExist ("edge")) {

          q->setEdge (EdgeNone);
        }
        fn << syspath << "/unexport";
      }

      f.open (fn.str());
      f.exceptions (f.failbit | f.badbit);
      f << q->systemNumber() << std::endl;
      f.exceptions (f.failbit | f.badbit);
      f.close();
    }
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::Private::sysFsOpen() {

    if (valueFd < 0) {
      PIMP_Q (const Pin);
      std::ostringstream fn;
      char buffer[2];

      // Construction du nom du fichier
      fn << syspath << "/gpio" << q->systemNumber() << "/value";
      // std::cout << "Open: " << fn.str() << std::endl;

      // Ouvrir le fichier
      if ( (valueFd = ::open (fn.str().c_str(), O_RDWR)) < 0) { //

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }

      if (mode != ModeUnknown) {

        sysFsSetMode();
      }
      else {

        sysFsGetMode();
      }

      if (edge != EdgeUnknown) {

        sysFsSetEdge();
      }
      else {

        sysFsGetEdge();
      }

    }
    return valueFd >= 0;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsClose() {

    if (valueFd >= 0) {

      // fermeture fichier value SysFs
      if (::close (valueFd) < 0) {

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }
      valueFd = -1;
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsGetEdge() {

    if (sysFsFileExist ("edge")) {
      std::string value;

      value = sysFsReadFile ("edge");
      edge = str2edge.at (value);
    }
    else {

      edge = EdgeUnknown;
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsSetEdge () {

    if (sysFsFileExist ("edge")) {

      sysFsWriteFile ("edge", edgeName (edge));
    }
    else {
      std::ostringstream msg;

      msg << "Pin " << descriptor->name.at (ModeInput) << " does not have interrupt";
      throw std::system_error (ENOTSUP, std::system_category(), msg.str());
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsGetMode() {

    if (sysFsFileExist ("direction")) {

      std::string value = sysFsReadFile ("direction");
      mode = str2mode.at (value);
    }
    else {

      mode = ModeUnknown;
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsSetMode () {

    if (sysFsFileExist ("direction") && ( (mode == ModeInput) || (mode == ModeOutput))) {
      PIMP_Q (Pin);
      sysFsWriteFile ("direction", q->modeName (mode));
    }
  }


  // ---------------------------------------------------------------------------
  std::string
  Pin::Private::sysFsReadFile (const char *n) const {
    PIMP_Q (const Pin);
    std::ostringstream fn;
    std::ifstream f;
    std::string value;
    std::string::size_type pos;

    fn << syspath << "/gpio" << q->systemNumber() << "/" << n;
    f.open (fn.str());
    f.exceptions (f.failbit | f.badbit);

    std::getline (f, value);
    f.exceptions (f.failbit | f.badbit);

    pos = value.rfind ('\n');
    if (pos != std::string::npos) {
      value.resize (pos + 1);
    }

    f.close();
    return value;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::sysFsWriteFile (const char *n, const std::string &v) {
    PIMP_Q (const Pin);
    std::ostringstream fn;
    std::ofstream f;

    fn << syspath << "/gpio" << q->systemNumber() << "/" << n;
    f.open (fn.str());
    f.exceptions (f.failbit | f.badbit);

    f << v << std::endl;
    f.exceptions (f.failbit | f.badbit);
    f.close();
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::Private::sysFsIsExport () const {
    PIMP_Q (const Pin);
    std::ostringstream fn;

    fn << syspath << "/gpio" << q->systemNumber();
    return System::directoryExists (fn.str());
  }

  // ---------------------------------------------------------------------------
  bool
  Pin::Private::sysFsFileExist (const char *n) const {
    PIMP_Q (const Pin);
    struct stat sb;
    std::ostringstream fn;

    fn << syspath << "/gpio" << q->systemNumber() << "/" << n;
    return (stat (fn.str().c_str(), &sb) == 0 && S_ISREG (sb.st_mode));
  }


  // ---------------------------------------------------------------------------
  void
  Pin::Private::writePull () {

    if (parent->device()) {
      PIMP_Q (Pin);

      setHoldPull();
      parent->device()->setPull (q, pull);
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::readPull ()  {

    if (parent->device()) {

      if (parent->device()->flags() & GpioDevice::hasPullRead) {
        PIMP_Q (Pin);

        pull = parent->device()->pull (q);
        return;
      }
    }
    pull = PullUnknown;
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::readMode ()  {

    if (useSysFs) {

      sysFsGetMode();
    }
    else {
      PIMP_Q (Pin);

      mode = parent->device()->mode (q);
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::writeMode () {

    setHoldMode();
    if (useSysFs) {

      sysFsSetMode();
    }
    else {
      PIMP_Q (Pin);

      parent->device()->setMode (q, mode);
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::readEdge()  {

    if (useSysFs) {

      sysFsGetEdge();
    }
    else {

      edge = EdgeUnknown;
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::writeEdge () {

    if (useSysFs) {

      sysFsSetEdge();
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::writeDrive () {

    if (parent->device()) {

      if (parent->device()->flags() & GpioDevice::hasDrive) {
        PIMP_Q (Pin);

        parent->device()->setDrive (q, drive);
      }
      else {

        throw std::domain_error ("Unable to set the drive strength for this pin, not supported on this board!");
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  Pin::Private::readDrive ()  {

    if (parent->device()) {

      if (parent->device()->flags() & GpioDevice::hasDrive) {
        PIMP_Q (Pin);

        drive = parent->device()->drive (q);
      }
      else {

        throw std::domain_error ("Unable to get the drive strength for this pin, not supported on this board!");
      }
    }
  }

  // ---------------------------------------------------------------------------
  // static
  int
  Pin::Private::sysFsPoll (int fd, int timeout_ms) {
    struct pollfd  fds;

    fds.fd = fd;
    fds.events = POLLPRI | POLLERR;
    return poll (& fds, 1, timeout_ms);
  }

  // ---------------------------------------------------------------------------
  // static
  int
  Pin::Private::sysFsRead (int fd) {
    int ret;

    ret = lseek (fd, 0, SEEK_SET);
    if (ret >= 0) {
      char buffer[2];

      // Lire la valeur actuelle du GPIO.
      ret = ::read (fd, buffer, sizeof (buffer));
      if (ret > 0) {

        ret = (buffer[0] != '0');
      }
      else {

        ret = -1;
      }
    }
    return ret;
  }

  // ---------------------------------------------------------------------------
  // static
  int
  Pin::Private::sysFsWrite (int fd, bool value) {
    int ret;

    if (value) {

      ret = ::write (fd, "1\n", 2);
    }
    else {

      ret = ::write (fd, "0\n", 2);
    }
    return ret;
  }


  // ---------------------------------------------------------------------------
  // static
  // Thread de surveillance des entrées du port
  void *
  Pin::Private::irqThread (std::future<void> run, int fd, Pin::Isr isr, void *userData) {
    int ret;

    Scheduler::setRtPriority (50);

    try {
      while (run.wait_for (std::chrono::milliseconds (1)) == std::future_status::timeout) {

        ret = sysFsPoll (fd, 10);
        if (ret > 0) {

          isr (userData);
          sysFsRead (fd);
        }
        else if (ret < 0) {

          throw std::system_error (errno, std::system_category(), __FUNCTION__);
        }
      }
    }
    catch (std::system_error &e) {

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

  // ---------------------------------------------------------------------------
  // static
  std::string Pin::Private::syspath = "/sys/class/gpio";

  // ---------------------------------------------------------------------------
  // static
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
  const std::map<Pin::Numbering, std::string> Pin::Private::numberings = {
    {NumberingLogical, "Logical"},
    {NumberingMcu, "Mcu"},
    {NumberingSystem, "System"},
    {NumberingUnknown, "Unknown"}
  };

  // ---------------------------------------------------------------------------
  // static
  const std::map<Pin::Pull, std::string> Pin::Private::pulls = {
    {PullOff, "off"},
    {PullDown, "down"},
    {PullUp, "up"},
    {PullUnknown, "unk"}
  };

  // ---------------------------------------------------------------------------
  // static
  const std::map<Pin::Edge, std::string> Pin::Private::edges = {
    {EdgeNone, "none"},
    {EdgeRising, "rising"},
    {EdgeFalling, "falling"},
    {EdgeBoth, "both"},
    {EdgeUnknown, "unk"}
  };

  // ---------------------------------------------------------------------------
  // static
  const std::map<Pin::Mode, std::string> Pin::Private::sysfsmodes = {
    { ModeInput, "in" },
    { ModeOutput, "out" },
    { ModeUnknown, "unk" }
  };

  // ---------------------------------------------------------------------------
  // static
  const std::map<std::string, Pin::Edge> Pin::Private::str2edge = {
    { "none", EdgeNone },
    { "rising", EdgeRising },
    { "falling", EdgeFalling },
    { "both", EdgeBoth }
  };

  // ---------------------------------------------------------------------------
  // static
  const std::map<std::string, Pin::Mode> Pin::Private::str2mode = {
    { "in", ModeInput },
    { "out", ModeOutput }
  };

}
/* ========================================================================== */
