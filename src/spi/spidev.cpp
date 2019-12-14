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
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <libudev.h>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <piduino/spidev.h>
#include <piduino/gpio.h>
#include <piduino/database.h>
#include <piduino/system.h>
#include "spidev_p.h"
#include "config.h"

using namespace std;

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                         SpiDev::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SpiDev::Private::Private (SpiDev * q) :
    IoDevice::Private (q), fd (-1) {

    isSequential = true;
  }

  // ---------------------------------------------------------------------------
  SpiDev::Private::~Private()  {}

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setMode () {

    if (isOpen()) {

      if (::ioctl (fd, SPI_IOC_WR_MODE, &settings.mode) < 0) {

        setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setSpeedHz () {

    if (isOpen()) {

      if (::ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &settings.speedHz) < 0) {

        setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setBitsPerWord () {

    if (isOpen()) {

      if (::ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &settings.bitsPerWord) < 0) {

        setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setBitOrder () {

    if (isOpen()) {
      uint8_t b = settings.bitOrder ? 0 : 1;

      if (::ioctl (fd, SPI_IOC_WR_LSB_FIRST, &b) < 0) {

        setError();
      }

    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getMode () {

    if (isOpen()) {

      if (::ioctl (fd, SPI_IOC_RD_MODE, &settings.mode) < 0) {

        setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getSpeedHz () {

    if (isOpen()) {

      if (::ioctl (fd, SPI_IOC_RD_MAX_SPEED_HZ, &settings.speedHz) < 0) {

        setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getBitsPerWord () {

    if (isOpen()) {

      if (::ioctl (fd, SPI_IOC_RD_BITS_PER_WORD, &settings.bitsPerWord) < 0) {

        setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getBitOrder () {

    if (isOpen()) {
      uint8_t b;

      if (::ioctl (fd, SPI_IOC_RD_LSB_FIRST, &b) < 0) {

        setError();
      }
      else {

        settings.bitOrder = (b == 0);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getSettings() {

    getMode();
    getSpeedHz();
    getBitOrder();
    getBitsPerWord();
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setSettings() {

    setMode();
    setSpeedHz();
    setBitOrder();
    setBitsPerWord();
  }

// -----------------------------------------------------------------------------
//
//                           SpiDev::Cs Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  void
  SpiDev::Cs::set (bool value) {

    if (! _driverControl) {

      _pin->write ( (value ^ _activeLevel) != 0);
    }
  }

  // ---------------------------------------------------------------------------
  bool
  SpiDev::Cs::get () const {

    if (! _driverControl) {

      return (_pin->read() ^ _activeLevel) != 0;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  SpiDev::Cs::setDriverControl (bool enable, bool al) {

    if (enable != _driverControl) {

      if (enable) {

        _pin->setMode (_mode);
        _driverControl = true;
      }
      else {

        if (_pin) {

          _activeLevel = al;
          _driverControl = false;
          _pin->setMode (Pin::ModeOutput);
          set (false);
        }
      }
    }
    return _driverControl;
  }

// -----------------------------------------------------------------------------
//
//                           SpiDev::Info Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  bool
  SpiDev::Info::setPath (const std::string & p) {

    for (int c = 0; c < MaxCs; c++) {

      for (int b = 0; b < MaxBuses; b++) {
        std::string bp = busPath (b, c);

        if (bp == p) {

          setId (b, c);
          return true;
        }
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::Info::setId (int idBus, int idCs) {
    const vector<Pin::SpiCs> & socCsList = db.board().soc().spiCs();

    _bus = idBus;
    _cs = idCs;
    _path = busPath (idBus, idCs);
    _csList.clear();

    for (int i = 0; i < socCsList.size(); i++) {
      const Pin::SpiCs & socCs = socCsList[i];

      if (socCs.bus == idBus) {
        Cs cs;

        cs.setId (idCs);
        cs.setMode (socCs.mode);
        cs.setPin (gpio.pin (socCs.pin));
        _csList[socCs.cs] = cs;
      }
    }
  }

  // ---------------------------------------------------------------------------
  string
  SpiDev::Info::busPath (int bus, int cs) {
    char path[256];

    ::snprintf (path, sizeof (path), db.board().family().spiSysPath().c_str(), bus, cs);
    return string (path);
  }

  // ---------------------------------------------------------------------------
  SpiDev::Info
  SpiDev::Info::defaultBus () {

    return Info (db.board().defaultSpiBus());
  }

  // ---------------------------------------------------------------------------
  std::deque<SpiDev::Info>
  SpiDev::Info::availableBuses () {
    std::deque<SpiDev::Info> buses;
    struct udev *udev;

    udev = udev_new();
    if (udev) {
      struct udev_enumerate * enumerate;
      struct udev_list_entry * devices, * dev_list_entry;
      struct udev_device * dev;

      enumerate = udev_enumerate_new (udev);
      udev_enumerate_add_match_subsystem (enumerate, "spidev");
      udev_enumerate_scan_devices (enumerate);
      devices = udev_enumerate_get_list_entry (enumerate);

      udev_list_entry_foreach (dev_list_entry, devices) {

        dev = udev_device_new_from_syspath (
                udev, udev_list_entry_get_name (dev_list_entry));

        if (!dev) {
          break;
        }

        const char * path = udev_device_get_devnode (dev);
        if (path) {
          Info bus;
          if (bus.setPath (path)) {
            buses.push_back (bus);
          }
        }
      }
      udev_unref (udev);
    }

    return buses;
  }

// -----------------------------------------------------------------------------
//
//                             SpiDev Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (SpiDev::Private & dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev () :
    IoDevice (*new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (const char * path) : SpiDev() {

    setBusPath (path);
  }

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (const std::string & path) : SpiDev() {

    setBusPath (path);
  }

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (const Info & bus) : SpiDev() {

    setBus (bus);
  }

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (int idBus, int idCs) : SpiDev() {

    setBus (idBus, idCs);
  }

  // ---------------------------------------------------------------------------
  SpiDev::~SpiDev() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool
  SpiDev::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (SpiDev);

      d->tstack.clear();
      d->fd = ::open (d->bus.path().c_str(), d->modeToPosixFlags (mode));
      if (d->fd < 0) {

        d->setError();
        return false;
      }
      d->setSettings();

      IoDevice::open (mode);
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::close() {

    if (isOpen()) {
      PIMP_D (SpiDev);

      if (::close (d->fd)) {

        d->setError();
      }
      d->fd = -1;
      IoDevice::close();
    }
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::setBus (const Info & bus) {
    PIMP_D (SpiDev);

    if (d->bus != bus) {

      d->bus = bus;

      if (isOpen()) {
        OpenMode m = openMode();

        close();
        open (m);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::setBus (int idBus, int idCs) {
    PIMP_D (SpiDev);

    if ( (d->bus.busId() != idBus) || (d->bus.csId() != idCs)) {

      d->bus.setId (idBus, idCs);
      if (isOpen()) {
        OpenMode m = openMode();

        close();
        open (m);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::setBusPath (const std::string & path) {
    PIMP_D (SpiDev);

    if (d->bus.path() != path) {

      d->bus.setPath (path);
      if (isOpen()) {
        OpenMode m = openMode();

        close();
        open (m);
      }
    }
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::setBusPath (const char * path) {
    PIMP_D (SpiDev);

    setBusPath (std::string (path));
  }

  // ---------------------------------------------------------------------------
  const SpiDev::Settings & SpiDev::settings() const {
    PIMP_D (const SpiDev);

    return d->settings;
  }

  // ---------------------------------------------------------------------------
  uint8_t SpiDev::mode() const {
    PIMP_D (const SpiDev);

    return d->settings.mode;
  }

  // ---------------------------------------------------------------------------
  const SpiDev::Info &
  SpiDev::bus() const {
    PIMP_D (const SpiDev);

    return d->bus;
  }

  // ---------------------------------------------------------------------------
  uint32_t SpiDev::speedHz () const {
    PIMP_D (const SpiDev);

    return d->settings.speedHz;
  }

  // ---------------------------------------------------------------------------
  uint8_t SpiDev::bitsPerWord () const {
    PIMP_D (const SpiDev);

    return d->settings.bitsPerWord;
  }

  // ---------------------------------------------------------------------------
  bool SpiDev::bitOrder() const {
    PIMP_D (const SpiDev);

    return d->settings.bitOrder;
  }

  // ---------------------------------------------------------------------------
  void SpiDev::setSettings (const SpiDev::Settings & settings) {
    PIMP_D (SpiDev);

    if (d->settings != settings) {

      d->settings = settings;
      d->setSettings();
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::setMode (uint8_t mode) {
    PIMP_D (SpiDev);

    if (d->settings.mode != mode) {

      d->settings.mode = mode;
      d->setMode();
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::setSpeedHz (uint32_t speedHz) {
    PIMP_D (SpiDev);

    if (d->settings.speedHz != speedHz) {

      d->settings.speedHz = speedHz;
      d->setSpeedHz();
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::setBitsPerWord (uint8_t bitsPerWord) {
    PIMP_D (SpiDev);

    if (d->settings.bitsPerWord != bitsPerWord) {

      d->settings.bitsPerWord = bitsPerWord;
      d->setBitsPerWord();
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::setBitOrder (bool bitOrder) {
    PIMP_D (SpiDev);

    if (d->settings.bitOrder != bitOrder) {

      d->settings.bitOrder = bitOrder;
      d->setBitOrder();
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::pushTransfer (Transfer & t) {
    PIMP_D (SpiDev);

    d->tstack.push_back (&t);
  }

  // ---------------------------------------------------------------------------
  void SpiDev::clear() {
    PIMP_D (SpiDev);

    d->tstack.clear();
  }

  // ---------------------------------------------------------------------------
  int SpiDev::transfer () {
    PIMP_D (SpiDev);
    int ret = -1;

    d->clearError();
    if (isOpen()) {

      unsigned int nofmsg = d->tstack.size();
      ret = 0;

      if (nofmsg) {
        struct spi_ioc_transfer * spi_message = new spi_ioc_transfer[nofmsg];
        memset (spi_message, 0, sizeof (struct spi_ioc_transfer) * nofmsg);

        for (unsigned int i = 0; i < nofmsg; i++)   {

          spi_message[i].rx_buf = reinterpret_cast<uint64_t> (d->tstack[i]->rxBuf);
          spi_message[i].tx_buf = reinterpret_cast<const uint64_t> (d->tstack[i]->txBuf);
          spi_message[i].len = d->tstack[i]->len;
          spi_message[i].speed_hz = d->tstack[i]->speedHz;
          spi_message[i].delay_usecs = d->tstack[i]->delayBeforeReleaseCs;
          spi_message[i].bits_per_word = d->tstack[i]->bitsPerWord;
          spi_message[i].cs_change = d->tstack[i]->releaseCsAfter;
        }

        ret = ::ioctl (d->fd, SPI_IOC_MESSAGE (nofmsg), spi_message);
        if (ret < 0) {
          d->setError();
        }
        d->tstack.clear();
        delete[] spi_message;
      }
    }
    else {

      d->setError (ENOTCONN);
    }
    return ret;
  }

  // ---------------------------------------------------------------------------
  int SpiDev::transfer (const uint8_t * txbuf, uint8_t * rxbuf, uint32_t len) {
    Transfer t (txbuf, rxbuf, len);

    pushTransfer (t);
    return transfer();
  }

  // ---------------------------------------------------------------------------
  int SpiDev::read (uint8_t * buffer, uint32_t len) {

    return transfer (0, buffer, len);
  }

  // ---------------------------------------------------------------------------
  int SpiDev::write (const uint8_t * buffer, uint32_t len) {

    return transfer (buffer, 0, len);
  }

#if 0
  // ---------------------------------------------------------------------------
  // TODO: Utilisation du flag SPI_NO_CS ?
  int
  SpiDev::setCsDriverControl (bool enable, bool al) {
    int ret = -1;

    clearError();
    if (isOpen()) {
      PIMP_D (SpiDev);
      vector<int> list = d->bus.csList();

      ret = 0;
      for (int i = 0; i < list.size(); i++) {

        d->bus.cs (i).setDriverControl (enable, al);
        ret++;
      }
    }
    else {

      setError (ENOTCONN);
    }
    return ret;
  }
#endif

}

/* ========================================================================== */
