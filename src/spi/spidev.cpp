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
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <piduino/spidev.h>
#include <piduino/gpio.h>
#include <piduino/database.h>
#include <piduino/system.h>
#include <piduino/private/spidev_p.h>

using namespace std;

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                         SpiDev::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SpiDev::Private::Private (SpiDev * q, const Settings & s) :
    IoDevice::Private (q), fd (-1), settings (s) {

    isSequential = true;
  }

  // ---------------------------------------------------------------------------
  SpiDev::Private::~Private()  {}

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setMode () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {

      if (ioctl (fd, SPI_IOC_WR_MODE, &settings.mode) < 0) {

        q->setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setSpeedHz () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {

      if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &settings.speedHz) < 0) {

        q->setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setBitsPerWord () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {

      if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &settings.bitsPerWord) < 0) {

        q->setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::setBitOrder () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {
      uint8_t b = settings.bitOrder ? 0 : 1;

      if (ioctl (fd, SPI_IOC_WR_LSB_FIRST, &b) < 0) {

        q->setError();
      }

    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getMode () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {

      if (ioctl (fd, SPI_IOC_RD_MODE, &settings.mode) < 0) {

        q->setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getSpeedHz () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {

      if (ioctl (fd, SPI_IOC_RD_MAX_SPEED_HZ, &settings.speedHz) < 0) {

        q->setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getBitsPerWord () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {

      if (ioctl (fd, SPI_IOC_RD_BITS_PER_WORD, &settings.bitsPerWord) < 0) {

        q->setError();
      }
    }
  }

  // ---------------------------------------------------------------------------
  void SpiDev::Private::getBitOrder () {
    PIMP_Q (SpiDev);

    if (q->isOpen()) {
      uint8_t b;

      if (ioctl (fd, SPI_IOC_RD_LSB_FIRST, &b) < 0) {

        q->setError();
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
  vector<int>
  SpiDev::Info::csList() const {
    vector<int> list;

    for (auto pair = _csList.cbegin(); pair != _csList.cend(); ++pair) {

      list.push_back (pair->first);
    }
    return list;
  }

// -----------------------------------------------------------------------------
//
//                             SpiDev Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (SpiDev::Private &dd) : IoDevice (dd) {

  }

  // ---------------------------------------------------------------------------
  SpiDev::SpiDev (const Settings & s) :
    IoDevice (*new Private (this, s))  {

  }

  // ---------------------------------------------------------------------------
  SpiDev::~SpiDev() {
    close();
  }

  // ---------------------------------------------------------------------------
  bool SpiDev::open (OpenMode mode) {

    return IoDevice::open (mode);
  }

  // ---------------------------------------------------------------------------
  bool
  SpiDev::open (int idBus, int idCs) {

    if (!isOpen()) {
      PIMP_D (SpiDev);

      if (findBus (d->info, idBus, idCs)) {

        d->fd = ::open (d->info.path().c_str(), O_RDWR);
        if (d->fd < 0) {

          setError();
          return false;
        }

        open (OpenMode::ReadWrite);
        d->setSettings();
      }
      else {

        setError (ENOENT);
      }
    }
    return isOpen();
  }


  // ---------------------------------------------------------------------------
  bool
  SpiDev::open (const Info & info) {

    return open (info.busId(), info.csId());
  }

  // ---------------------------------------------------------------------------
  void
  SpiDev::close() {

    if (isOpen()) {
      PIMP_D (SpiDev);

      if (::close (d->fd)) {

        setError();
      }
      d->fd = -1;
      IoDevice::close();
    }
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
  SpiDev::info() const {
    PIMP_D (const SpiDev);

    return d->info;
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
    int ret = -1;

    clearError();
    if (isOpen()) {
      PIMP_D (SpiDev);

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

        ret = ioctl (d->fd, SPI_IOC_MESSAGE (nofmsg), spi_message);
        if (ret < 0) {
          setError();
        }
        d->tstack.clear();
        delete[] spi_message;
      }
    }
    else {

      setError (ENOTCONN);
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
  int SpiDev::write (const uint8_t *buffer, uint32_t len) {

    return transfer (buffer, 0, len);
  }

  // ---------------------------------------------------------------------------
  string
  SpiDev::busPath (int idBus, int idCs) {
    char path[256];

    ::snprintf (path, sizeof (path), db.board().soc().spiSysPath().c_str(), idBus, idCs);
    return string (path);
  }

  // ---------------------------------------------------------------------------
  bool
  SpiDev::findBus (SpiDev::Info & info, int idBus, int idCs)  {
    const vector<Pin::SpiCs> & csList = db.board().soc().spiCs();
    bool found = false;

    for (int i = 0; i < csList.size(); i++) {
      const Pin::SpiCs & cs = csList[i];

      if ( (cs.bus == idBus) && (cs.cs == idCs)) {

        string path = busPath (idBus, idCs);
        if (System::fileExist (path)) {
          Cs csPin;

          info.setBusId (idBus);
          info.setCsId (idCs);
          info.setPath (path);
          csPin.setId (idCs);
          csPin.setMode (cs.mode);
          csPin.setPin (gpio.pin (cs.pin));
          info.insertCs (cs.cs, csPin);
          found = true;
        }
      }
    }

    if (found) {

      for (int i = 0; i < csList.size(); i++) {
        const Pin::SpiCs & cs = csList[i];

        if ( (cs.bus == idBus) && (cs.cs != idCs)) {
          Cs csPin;

          csPin.setId (cs.cs);
          csPin.setMode (cs.mode);
          csPin.setPin (gpio.pin (cs.pin));
          info.insertCs (cs.cs, csPin);
        }
      }
    }
    return found;
  }

  // ---------------------------------------------------------------------------
  SpiDev::Info
  SpiDev::defaultBus () {
    Info info;

    if (! findBus (info, gpio.defaultSpiBus())) {

      throw system_error (ENOENT, system_category(),
                          "Default SPI Bus not found !");
    }
    return info;
  }

  // ---------------------------------------------------------------------------
  vector<SpiDev::Info>
  SpiDev::availableBuses () {
    vector<SpiDev::Info> buses;
    const vector<Pin::SpiCs> & csList = db.board().soc().spiCs();

    for (int i = 0; i < csList.size(); i++) {
      Info info;
      const Pin::SpiCs & cs = csList[i];

      if (findBus (info, cs.bus, cs.cs)) {

        buses.push_back (info);
      }
    }

    return buses;
  }

  // ---------------------------------------------------------------------------
  // TODO: Utilisation du flag SPI_NO_CS ?
  int
  SpiDev::setCsDriverControl (bool enable, bool al) {
    int ret = -1;

    clearError();
    if (isOpen()) {
      PIMP_D (SpiDev);
      vector<int> list = d->info.csList();

      ret = 0;
      for (int i = 0; i < list.size(); i++) {

        d->info.cs (i).setDriverControl (enable, al);
        ret++;
      }
    }
    else {

      setError (ENOTCONN);
    }
    return ret;
  }

}

/* ========================================================================== */
