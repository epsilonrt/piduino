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
#include <piduino/scheduler.h>
#include <exception>
#include <fstream>
#include <sstream>
//
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                              Pin Class
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
  std::string Pin::_syspath = "/sys/class/gpio";

// -----------------------------------------------------------------------------
  const std::map<Pin::Type, std::string> Pin::_types = {
    {TypeGpio, "Gpio"},
    {TypePower, "Power"},
    {TypeUsb, "Usb"},
    {TypeSound, "Sound"},
    {TypeVideo, "Video"},
    {TypeNotConnected, "Not Connected"},
    {TypeUnknown, "Unknown"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Numbering, std::string> Pin::_numberings = {
    {NumberingLogical, "Logical"},
    {NumberingMcu, "Mcu"},
    {NumberingSystem, "System"},
    {NumberingUnknown, "Unknown"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Pull, std::string> Pin::_pulls = {
    {PullOff, "off"},
    {PullDown, "down"},
    {PullUp, "up"},
    {PullUnknown, "unk"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Edge, std::string> Pin::_edges = {
    {EdgeNone, "none"},
    {EdgeRising, "rising"},
    {EdgeFalling, "falling"},
    {EdgeBoth, "both"},
    {EdgeUnknown, "unk"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> Pin::_sysfsmodes = {
    { ModeInput, "in" },
    { ModeOutput, "out" },
    { ModeUnknown, "unk" }
  };

// -----------------------------------------------------------------------------
  const std::map<std::string, Pin::Edge> Pin::_str2edge = {
    { "none", EdgeNone },
    { "rising", EdgeRising },
    { "falling", EdgeFalling },
    { "both", EdgeBoth }
  };

// -----------------------------------------------------------------------------
  const std::map<std::string, Pin::Mode> Pin::_str2mode = {
    { "in", ModeInput },
    { "out", ModeOutput }
  };

// -----------------------------------------------------------------------------
  Pin::Pin (Connector * parent, const Descriptor * desc) :
    _isopen (false), _parent (parent), _descriptor (desc), _holdMode (ModeUnknown),
    _holdPull (PullUnknown), _holdState (false), _useSysFs (false),
    _valueFd (-1), _firstPolling (true), _edge (EdgeUnknown), _mode (ModeUnknown),
    _pull (PullUnknown), _run (false) {

    if ( (parent->gpio()->accessLayer() & AccessLayerIoMap) != AccessLayerIoMap) {

      _useSysFs = true;
    }
  }

// -----------------------------------------------------------------------------
  Pin::~Pin() {

    detachInterrupt();
    close();
  }

// -----------------------------------------------------------------------------
  bool Pin::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  Pin::Pull
  Pin::pull () {

    if (isOpen() && (type() == TypeGpio)) {

      readPull();;
    }
    return _pull;
  }

// -----------------------------------------------------------------------------
  void
  Pin::setPull (Pin::Pull p) {

    if (type() == TypeGpio) {
      _pull = p;

      if (isOpen()) {

        writePull();
      }
    }
  }

// -----------------------------------------------------------------------------
  Pin::Mode
  Pin::mode () {

    if (isOpen() && (type() == TypeGpio)) {

      readMode();
    }
    return _mode;
  }

// -----------------------------------------------------------------------------
  void
  Pin::setMode (Pin::Mode m) {

    if (type() == TypeGpio) {

      _mode = m;
      if (isOpen()) {

        writeMode();
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::setEdge (Pin::Edge e) {

    if (type() == TypeGpio) {
      _edge = e;
      if (isOpen() && _useSysFs) {

        sysFsSetEdge();
      }
    }
  }


// -----------------------------------------------------------------------------
  Pin::Edge
  Pin::edge() {

    if (isOpen() && (type() == TypeGpio) && _useSysFs) {

      sysFsGetEdge();
    }
    return _edge;
  }

// -----------------------------------------------------------------------------
  bool
  Pin::useSysFs() const {

    return _useSysFs;
  }

// -----------------------------------------------------------------------------
  AccessLayer
  Pin::accessLayer() const {

    gpio()->accessLayer();
  }

// -----------------------------------------------------------------------------
  Pin::Type
  Pin::type() const {

    return _descriptor->type;
  }

// -----------------------------------------------------------------------------
  int
  Pin::logicalNumber () const {

    return _descriptor->num.logical;
  }

// -----------------------------------------------------------------------------
  int
  Pin::mcuNumber () const {

    return _descriptor->num.mcu;
  }

// -----------------------------------------------------------------------------
  int
  Pin::systemNumber () const {

    return _descriptor->num.system;
  }

// -----------------------------------------------------------------------------
  int
  Pin::number (Pin::Numbering n) const {

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

// -----------------------------------------------------------------------------
  int
  Pin::row() const {

    return _descriptor->num.row;
  }

// -----------------------------------------------------------------------------
  int
  Pin::column() const {

    return _descriptor->num.column;
  }

// -----------------------------------------------------------------------------
  int
  Pin::physicalNumber () const {

    return connector()->pinNumber (row(), column());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::name (Pin::Mode m) const {

    return _descriptor->name.at (m);
  }

// -----------------------------------------------------------------------------
  Connector *
  Pin::connector() const {

    return _parent;
  }

// -----------------------------------------------------------------------------
  Gpio *
  Pin::gpio() const {

    return connector()->gpio();
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::typeName() const {

    return typeName (type());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::numberingName (Pin::Numbering n)  {

    return _numberings.at (n);
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::typeName (Pin::Type t) {

    return _types.at (t);
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::edgeName (Pin::Edge e) {

    return _edges.at (e);
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::pullName (Pin::Pull p) {

    return _pulls.at (p);
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Type, std::string> &
  Pin::types () {

    return _types;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Numbering, std::string> &
  Pin::numberings () {

    return _numberings;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Pull, std::string> &
  Pin::pulls () {

    return _pulls;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Edge, std::string> &
  Pin::edges () {

    return _edges;
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::modeName (Pin::Mode m) const {

    return modes().at (m);
  }

// -----------------------------------------------------------------------------
  Device *
  Pin::device() const {

    return connector()->device();
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  Pin::modes () const {

    return device() ? device()->modes() : _sysfsmodes;
  }

// -----------------------------------------------------------------------------
  bool
  Pin::isDebug() const {

    return device() ? device()->isDebug() : false;
  }

// -----------------------------------------------------------------------------
  void
  Pin::setDebug (bool enable) {

    if (device()) {

      device()->setDebug (enable);
    }
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::modeName() {

    return modeName (mode ());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::pullName ()  {

    return pullName (pull());
  }

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
  void
  Pin::write (bool value) {

    if (isOpen() && (type() == TypeGpio)) {

      if (_useSysFs) {

        if (sysFsWrite (_valueFd, value) < 0) {

          throw std::system_error (errno, std::system_category(), __FUNCTION__);
        }
      }
      else {

        device()->write (this, value);
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::toggle () {

    if (isOpen() && (type() == TypeGpio)) {

      if (!_useSysFs) {

        if (device()->flags() & Device::hasToggle) {

          device()->toggle (this);
          return;
        }
      }

      write (!read());
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::read () const {

    if (isOpen() && (type() == TypeGpio)) {

      if (_useSysFs) {
        int ret = sysFsRead (_valueFd);

        if (ret < 0) {

          throw std::system_error (errno, std::system_category(), __FUNCTION__);
        }
        return ret > 0;
      }

      return device()->read (this);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::release () {

    if (isOpen() && (type() == TypeGpio)) {

      if (_holdMode != ModeUnknown) {

        setMode (_holdMode);
        if (_holdMode == ModeOutput) {

          write (_holdState);
        }
        _holdMode = ModeUnknown;
      }

      if (_holdPull != PullUnknown) {

        setPull (_holdPull);
        _holdPull = PullUnknown;
      }
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::forceUseSysFs (bool enable) {

    if (isOpen() && (_useSysFs != enable)) {

      _useSysFs = sysFsEnable (enable);
    }
    else {

      _useSysFs = enable;
    }
    return useSysFs();
  }

// -----------------------------------------------------------------------------
  void
  Pin::waitForInterrupt (Pin::Edge e, int timeout_ms) {

    if (isOpen()) {
      int ret;

      if (_firstPolling) {

        setEdge (EdgeNone);
        forceUseSysFs (true);
        setEdge (e);
        // clear pending irq
        if (sysFsPoll (_valueFd, 1) > 0) {

          sysFsRead (_valueFd);
        }
        _firstPolling = false;
      }

      if (e != _edge) {

        setEdge (e);
      }

      ret = sysFsPoll (_valueFd, timeout_ms);
      if (ret < 0) {

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }
      else if (ret == 0) {

        throw std::system_error (ETIME, std::generic_category());
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::attachInterrupt (Isr isr, Edge e) {

    if (!_thread.joinable()) {

      setEdge (EdgeNone);
      forceUseSysFs (true);
      setEdge (e);
      // clear pending irq
      if (sysFsPoll (_valueFd, 1) > 0) {

        sysFsRead (_valueFd);
      }
      _run = true;
      _thread = std::thread (irqThread, std::ref (_run), _valueFd, isr);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::detachInterrupt() {

    if (_thread.joinable()) {

      _run = false;
      _thread.join();
    }
  }

// -----------------------------------------------------------------------------
//                                   Protected
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
  bool Pin::open() {

    if (!isOpen()) {

      if (type() == TypeGpio) {

        if (_useSysFs) {

          _isopen = sysFsEnable (true);
        }
        else {

          _isopen = true;
        }

        if (_isopen) {

          if (_mode != ModeUnknown) {

            writeMode();
          }
          else {

            readMode();
          }

          if (device()) {

            if (_pull != PullUnknown) {

              writePull();
            }
            else {

              readPull();
            }
          }

          if (_edge != EdgeUnknown) {

            writeEdge();
          }
          else {

            readEdge();
          }
        }
      }
      else {

        _isopen = true;
      }
    }
    return isOpen();
  }

// -----------------------------------------------------------------------------
  void
  Pin::close() {

    if (isOpen()) {

      if (type() == TypeGpio) {

        forceUseSysFs (false); // close & unexport
        if (gpio()->releaseOnClose()) {

          release();
        }
      }
      _isopen = false;
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::writePull () {

    if (device()) {

      holdPull();
      device()->setPull (this, _pull);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::readPull ()  {

    if (device()) {
      if (device()->flags() & Device::hasPullRead) {

        _pull = device()->pull (this);
        return;
      }
    }
    _pull = PullUnknown;
  }

// -----------------------------------------------------------------------------
  void
  Pin::readMode ()  {

    if (_useSysFs) {

      sysFsGetMode();
    }
    else {

      _mode = device()->mode (this);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::writeMode () {

    holdMode();
    if (_useSysFs) {

      sysFsSetMode();
    }
    else {

      device()->setMode (this, _mode);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::readEdge()  {

    if (_useSysFs) {

      sysFsGetEdge();
    }
    else {

      _edge = EdgeUnknown;
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::writeEdge () {

    if (_useSysFs) {

      sysFsSetEdge();
    }
  }

// -----------------------------------------------------------------------------
//                                   Private
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Thread de surveillance des entrées du port
  void *
  Pin::irqThread (std::atomic<int> & run, int fd, Isr isr) {
    int ret;

    Scheduler::setRtPriority (50);

    try {
      while (run) {

        ret = sysFsPoll (fd, 100);
        if (ret > 0) {

          isr();
        }
        else if (ret < 0) {

          throw std::system_error (errno, std::system_category(), __FUNCTION__);
        }
      }
    }
    catch (std::system_error & e) {

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

// -----------------------------------------------------------------------------
  void Pin::holdPull() {

    if ( (_holdPull == PullUnknown) && device()) {

      _holdPull = device()->pull (this);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::holdMode() {

    if ( (_holdMode == ModeUnknown) && device()) {

      _holdMode = device()->mode (this);
      if (_holdMode == ModeOutput) {

        _holdState = device()->read (this);
      }
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsEnable (bool enable) {

    if (enable) {

      holdMode();
      holdPull();
      sysFsExport (true);
      return sysFsOpen();
    }

    sysFsClose();
    sysFsExport (false);
    return false;
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsExport (bool enable) {

    if ( (type() == TypeGpio) && (enable != sysFsIsExport())) {
      std::ostringstream fn;
      std::ofstream f;

      if (systemNumber() < 0) {
        std::ostringstream msg;

        msg << "Unknown pin number in SysFs for " << name (ModeInput) << " !";
        throw std::invalid_argument (msg.str());
      }

      if (enable) {

        // Export
        fn << _syspath << "/export";

      }
      else {

        // Unexport
        if (sysFsFileExist ("edge")) {

          setEdge (EdgeNone);
        }
        fn << _syspath << "/unexport";
      }

      f.open (fn.str());
      f.exceptions (f.failbit | f.badbit);
      f << systemNumber() << std::endl;
      f.exceptions (f.failbit | f.badbit);
      f.close();
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsOpen() {

    if (_valueFd < 0) {
      std::ostringstream fn;
      char buffer[2];

      // Construction du nom du fichier
      fn << _syspath << "/gpio" << systemNumber() << "/value";
      // std::cout << "Open: " << fn.str() << std::endl;

      // Ouvrir le fichier
      if ( (_valueFd = ::open (fn.str().c_str(), O_RDWR)) < 0) { //

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }

      if (_mode != ModeUnknown) {

        sysFsSetMode();
      }
      else {

        sysFsGetMode();
      }

      if (_edge != EdgeUnknown) {

        sysFsSetEdge();
      }
      else {

        sysFsGetEdge();
      }

    }
    return _valueFd >= 0;
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsClose() {

    if (_valueFd >= 0) {

      // fermeture fichier value sysFs
      if (::close (_valueFd) < 0) {

        throw std::system_error (errno, std::system_category(), __FUNCTION__);
      }
      _valueFd = -1;
    }
  }

// -----------------------------------------------------------------------------
  int
  Pin::sysFsPoll (int fd, int timeout_ms) {
    struct pollfd  fds;

    fds.fd = fd;
    fds.events = POLLPRI | POLLERR;
    return poll (& fds, 1, timeout_ms);
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsGetEdge() {

    if (sysFsFileExist ("edge")) {
      std::string value;

      value = sysFsReadFile ("edge");
      _edge = _str2edge.at (value);
    }
    else {

      _edge = EdgeUnknown;
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsSetEdge () {

    if (sysFsFileExist ("edge")) {

      sysFsWriteFile ("edge", edgeName (_edge));
    }
    else {
      std::ostringstream msg;

      msg << "Pin " << name (ModeInput) << " does not have interrupt";
      throw std::system_error (ENOTSUP, std::system_category(), msg.str());
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsGetMode() {

    if (sysFsFileExist ("direction")) {

      std::string value = sysFsReadFile ("direction");
      _mode = _str2mode.at (value);
    }
    else {

      _mode = ModeUnknown;
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsSetMode () {

    if (sysFsFileExist ("direction") && ( (_mode == ModeInput) || (_mode == ModeOutput))) {

      sysFsWriteFile ("direction", modeName (_mode));
    }
  }


// -----------------------------------------------------------------------------
  int
  Pin::sysFsRead (int fd) {
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

// -----------------------------------------------------------------------------
  int
  Pin::sysFsWrite (int fd, bool value) {
    int ret;

    if (value) {

      ret = ::write (fd, "1\n", 2);
    }
    else {

      ret = ::write (fd, "0\n", 2);
    }
    return ret;
  }

// -----------------------------------------------------------------------------
  std::string
  Pin::sysFsReadFile (const char * n) const {
    std::ostringstream fn;
    std::ifstream f;
    std::string value;
    std::string::size_type pos;

    fn << _syspath << "/gpio" << systemNumber() << "/" << n;
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

// -----------------------------------------------------------------------------
  void
  Pin::sysFsWriteFile (const char * n, const std::string & v) {
    std::ostringstream fn;
    std::ofstream f;

    fn << _syspath << "/gpio" << systemNumber() << "/" << n;
    f.open (fn.str());
    f.exceptions (f.failbit | f.badbit);

    f << v << std::endl;
    f.exceptions (f.failbit | f.badbit);
    f.close();
  }

// -----------------------------------------------------------------------------
  bool
  Pin::directoryExist (const std::string & dname) {
    struct stat sb;

    return (stat (dname.c_str(), &sb) == 0 && S_ISDIR (sb.st_mode));
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsIsExport () const {
    std::ostringstream fn;

    fn << _syspath << "/gpio" << systemNumber();
    return directoryExist (fn.str());
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsFileExist (const char * n) const {
    struct stat sb;
    std::ostringstream fn;

    fn << _syspath << "/gpio" << systemNumber() << "/" << n;
    return (stat (fn.str().c_str(), &sb) == 0 && S_ISREG (sb.st_mode));
  }

}
/* ========================================================================== */
