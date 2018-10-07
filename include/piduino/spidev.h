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

#ifndef PIDUINO_SPIDEV_H
#define PIDUINO_SPIDEV_H

#include <deque>
#include <map>
#include <string>
#include <piduino/iodevice.h>
#include <piduino/gpiopin.h>
#include <piduino/system.h>
#include <linux/spi/spidev.h>

/**
 * @defgroup piduino_spidev SPI Bus
 *
 * Ce module permet l'utilisation des bus SPI en mode maître à l'aide du driver spidev.
 *
 * @warning
 * Dans le modèle Arduino, qui est monotâche, c'est l'utilisateur qui gère le
 * signal de sélection d'esclave (SS,CS ou CE en fonction des circuits) à l'aide
 * de la fonction digitalWrite(). Dans les nouvelles version d'Arduino, il est
 * apparu les fonctions beginTransaction() et endTransaction() qui préfigure
 * une évolution de l'utilisation du bus SPI qui se rapproche de ce qu'on trouve
 * sur un système comme GNU Linux (structure spi_ioc_transfer).
 *
 * Dans un système multitâche, l'accès au bus SPI (et plus généralement à toutes
 * les ressources non partageables) est géré par le système d'exploitation par
 * le driver. C'est le driver spidev qui gère tous les signaux du bus :
 * MOSI, MISO, SCK et CS.
 *
 * La gestion du CS de l'espace utilisateur a été envisagée car on constate
 * la présence d'un flag SPI_NO_CS dans les sources du noyau
 * https://github.com/torvalds/linux/blob/master/drivers/spi, néanmoins, on
 * constate que seuls des drivers spi-bcm2835.c (Raspberry Pi !), spi-davinci.c
 * et spi-imx.c gère cette possibilité.
 *
 * Le choix qui a été fait, pour l'instant dans le noyau Arduino de PiDuino
 * (src/arduino/Core.cpp) est de laisser le driver spi gérer le signal CS sur
 * les autres cartes  que les RaspberryPi.
 * Dans ce cas, Les appels à pinMode() et donc à digitalWrite() n'ont aucun effet
 * lorsqu'il s'agit de la broche correspondant au CS du bus SPI utilisé.
 * Cela permet d'assurer une compatibilité avec le code Arduino, tout en restant
 * compatible avec toutes les cartes Pi.Néanmoins,cela n'autorise pas
 * l'utilisation d'une autre broche GPIO que celle du CS SPI sur les autres
 * cartes que Raspberry Pi (pour l'instant...).
 *
 *  @{
 */

namespace Piduino {

  /**
   * @class SpiDev
   * @brief
   */
  class SpiDev : public IoDevice {
    public:

      /**
       * @class Cs
       * @brief Broche de chip select d'un bus SPI
       */
      class Cs {
        public:
          friend class SpiDev;

          Cs() : _id (-1), _pin (0), _mode (Pin::ModeUnknown),
            _driverControl (true), _activeLevel (false) {}

          inline int id() const {
            return _id;
          }

          inline Pin * pin() const {
            return _pin;
          }

          inline Pin::Mode mode() const {
            return _mode;
          }

          inline bool driverControl() const {
            return _driverControl;
          }

          inline bool activeLevel() const {
            return _activeLevel;
          }

          bool setDriverControl (bool enable = false, bool activeLevel = false);
          bool get() const;
          void set (bool value);

        protected:
          inline void setId (int value) {
            _id = value;
          }
          inline void setPin (Pin * value) {
            _pin = value;
          }
          inline void setMode (Pin::Mode value) {
            _mode = value;
          }

        private:
          int _id;
          Pin * _pin;
          Pin::Mode _mode;
          bool _driverControl;
          bool _activeLevel;
      };

      /**
       * @class Info
       * @brief Informations sur un bus SPI
       */
      class Info {
        public:
          static const int MaxBuses = 32;
          static const int MaxCs = 32;

          Info (int bus = 0, int cs = 0) {
            setId (bus, cs);
          }

          void setId (int bus, int cs = 0);
          bool setPath (const std::string & path);

          inline int busId() const {
            return _bus;
          }

          inline int csId() const {
            return _cs;
          }

          inline const std::string & path() const {
            return _path;
          }

          inline const std::map<int, Cs> & csList() const {
            return _csList;
          }

          inline const Cs & cs() const   {
            return _csList.at (_cs);
          }
          
          inline bool exists() const {
            return System::charFileExists(_path);
          }

          bool operator== (const Info & other) {
            return (_path == other._path) ;
          }

          bool operator!= (const Info & other) {
            return (_path != other._path) ;
          }

          /**
           * @brief Liste des bus disponibles sur le systèmes
           */
          static std::deque<SpiDev::Info> availableBuses ();

          /**
           * @brief Information sur le bus SPI par défaut disponible sur la carte Pi
           * Dépend du modèle de carte (informations stockées dans la base de données)
           */
          static Info defaultBus ();

          /**
           * @brief Chemin système correspondant à un bus
           * @param bus identifiant du bus
           * @param cs identifiant du chip select
           * @return Chemin du fichier dans /dev
           */
          static std::string busPath (int bus, int cs = 0);

        private:
          int _bus; ///< numéro du bus
          int _cs; ///< index du CS affecté par le driver spidev
          std::string _path; ///< chemin du bus dans /dev
          std::map<int, Cs> _csList; ///< liste des broches de CS du bus
      };

      /**
       * @class Settings
       * @brief
       */
      class Settings {
        public:
          enum {
            MsbFirst = true, ///< MSB en premier
            LsbFirst = false ///< LSB en premier
          };

          Settings (uint32_t clock = 10000000, bool bitOrder = MsbFirst, uint8_t dataMode = SPI_MODE_0, uint8_t dataBits = 8) :
            mode (dataMode), bitOrder (bitOrder), bitsPerWord (dataBits), speedHz (clock) {}

          bool operator== (const Settings & other) {
            return (mode == other.mode) && (bitOrder == other.bitOrder) &&
                   (bitsPerWord == other.bitsPerWord) && (speedHz == other.speedHz);
          }

          bool operator!= (const Settings & other) {
            return ! (*this == other);
          }

          /*
            SPI_CPHA    0x01
            SPI_CPOL    0x02

            SPI_MODE_0    (0|0)
            SPI_MODE_1    (0|SPI_CPHA)
            SPI_MODE_2    (SPI_CPOL|0)
            SPI_MODE_3    (SPI_CPOL|SPI_CPHA)

            SPI_CS_HIGH   0x04
            SPI_LSB_FIRST   0x08
            SPI_3WIRE   0x10
            SPI_LOOP    0x20
            SPI_NO_CS   0x40
            SPI_READY   0x80
          */
          uint8_t mode;
          bool bitOrder;
          uint8_t bitsPerWord;
          uint32_t speedHz;
      };

      /**
       * @class Transfer
       * @brief
       */
      class Transfer {
        public:
          const uint8_t * txBuf;
          uint8_t * rxBuf;
          uint32_t len;
          uint32_t speedHz;
          uint8_t bitsPerWord;
          bool releaseCsAfter;
          uint16_t delayBeforeReleaseCs;
          Transfer (const uint8_t * txbuf = 0, uint8_t * rxbuf = 0, uint32_t count = 0) :
            txBuf (txbuf), rxBuf (rxbuf), len (count), speedHz (0),
            bitsPerWord (0), releaseCsAfter (false), delayBeforeReleaseCs (0) {}
          bool operator== (const Transfer & other) {
            return (txBuf == other.txBuf) && (rxBuf == other.rxBuf) &&
                   (len == other.len) && (speedHz == other.speedHz) &&
                   (delayBeforeReleaseCs == other.delayBeforeReleaseCs) &&
                   (bitsPerWord == other.bitsPerWord) &&
                   (releaseCsAfter == other.releaseCsAfter);
          }
      };

      /**
       * @brief Constructeur par défaut
       */
      SpiDev ();

      /**
       * @brief Constructeur à partir d'un bus
       */
      SpiDev (const Info & bus);

      /**
       * @brief Constructeur à partir identifiant de bus
       */
      explicit SpiDev (int idBus, int idCs = 0);

      /**
       * @brief Constructeur à partir chemin de bus
       */
      explicit SpiDev (const std::string & path);

      /**
       * @brief Constructeur à partir chemin de bus const char *
       */
      explicit SpiDev (const char * path);

      /**
       * @brief Destructeur
       */
      virtual ~SpiDev();

      /**
       * @brief Ouverture du bus SPI
       * La configuration settings() est appliquée.
       * @param mode mode d'ouverture
       * @return true si ouvert, false sinon, dans ce cas error() et
       * errorString() peuvent être utilisé pour connaître la raison.
       */
      virtual bool open (OpenMode mode = IoDevice::ReadWrite);

      /**
       * @brief Fermeture du bus
       * Cette fonction est appelée par le destructeur
       */
      virtual void close();

      /**
       * @brief Modification du bus SPI
       * Si la liaison ouverte, celle-ci est fermée, puis réouverte sur le
       * nouveau bus.
       * @param idBus identifiant du bus 0..N
       * @param idCs identifiant du chip select 0..C
       * @return true si ouvert, false sinon, dans ce cas error() et
       * errorString() peuvent être utilisé pour connaître la raison.
       */
      void setBus (int idBus, int idCs = 0);

      /**
       * @brief Surcharge de setBus() à partir d'une variable Info
       */
      void setBus (const Info & bus);

      /**
       * @brief Modification du chemin du bus
       * Si la liaison ouverte, celle-ci est fermée, puis réouverte sur le
       * nouveau bus.
       * @param path chemin dans /dev
       */
      void setBusPath (const std::string & path);

      /**
       * @brief Surcharge de setBusPath() à partir d'une variable const char *
       */
      void setBusPath (const char * path);

      /**
       * @brief Informations sur le bus
       */
      const Info & bus() const;

      /**
       * @brief Transfert d'un message en entrée-sortie
       * @param txbuf buffer sur les octets à transmettre, 0 si pas de données
       * à transmettre, dans ce cas, des 0 sont transmis sur MOSI.
       * @param rxbuf buffer qui contiendra les octets lues sur MISO, 0 si les
       * données ne doivent pas être récupérées.
       * @param len nombre d'octets dans le ou les buffers. Si les 2 buffers
       * \c txbuf et \c rxbuf sont fournis, ils doivent avoir une taille au moins
       * égale à \c len.
       * @return 0, valeur négative si erreur, dans ce cas error() et
       * errorString() peuvent être utilisé pour connaître la raison.
       */
      int transfer (const uint8_t * txbuf, uint8_t * rxbuf, uint32_t len);

      /**
       * @brief Lecture d'un message
       * @param rxbuf buffer qui contiendra les octets lues sur MISO
       * @param len nombre d'octets à lire.
       * @return 0, valeur négative si erreur, dans ce cas error() et
       * errorString() peuvent être utilisé pour connaître la raison.
       */
      int read (uint8_t * buffer, uint32_t len);

      /**
       * @brief Ecriture d'un message
       * @param txbuf buffer sur les octets à transmettre
       * à transmettre, dans ce cas, des 0 sont transmis sur MOSI.
       * @param len nombre d'octets dans le buffer
       * @return 0, valeur négative si erreur, dans ce cas error() et
       * errorString() peuvent être utilisé pour connaître la raison.
       */
      int write (const uint8_t *buffer, uint32_t len);

      /**
       * @brief Empilement d'un message dans la pile de transmission
       * Permet de gérer l'envoi de plusieurs message d'un seul tenant. Le ou
       * les messages seront transmis lors du prochain appel à \c transfer(),
       * \c read() ou \c write()
       * @param t référence sur le message
       */
      void pushTransfer (Transfer & t);

      /**
       * @brief Transmission des messages de la pile de transmission
       * La pile est vidée avant retour.
       * @return 0, valeur négative si erreur, dans ce cas error() et
       * errorString() peuvent être utilisé pour connaître la raison.
       */
      int transfer ();

      /**
       * @brief Vidage de la pile de transmission
       * Pas nécessaire après transfer()
       */
      void clear();

      /**
       * @brief Modification des réglages de la transmission
       * Si la liaison est ouverte les réglages sont appliqués immédiatement,
       * sinon ils le seront à l'ouverture par \c open()
       */
      void setSettings (const Settings & settings);

      /**
       * @brief Modification du mode de fonctionnement
       * Les valeurs généralement utilisées sont SPI_MODE_0, SPI_MODE_1,
       * SPI_MODE_2 ou SPI_MODE_3 qui correspondent aux valeurs de
       * polarité et de phase d'horloge telle que définies dans les standards
       * https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Clock_polarity_and_phase.
       * On pourra y ajouter des flags en effectuant un "OU" (|) avec les
       * valeurs suivantes (prise en compte dépendant de la plateforme !):
       * - SPI_CS_HIGH pour une action du chip select à l'état haut
       * - SPI_3WIRE mode 3 fils (MISO/MOSI sur le même fil)
       * - SPI_NO_CS chip select géré de l'espace utilisateur
       * - .
       * Si la liaison est ouverte les réglages sont appliqués immédiatement,
       * sinon ils le seront à l'ouverture par \c open()
       */
      void setMode (uint8_t mode);

      /**
       * @brief Modification de la vitesse maximale de transmission en Hz
       * Si la liaison est ouverte les réglages sont appliqués immédiatement,
       * sinon ils le seront à l'ouverture par \c open()
       */
      void setSpeedHz (uint32_t speedHz);

      /**
       * @brief Modification du nombre de bits par mot
       * Si la liaison est ouverte les réglages sont appliqués immédiatement,
       * sinon ils le seront à l'ouverture par \c open()
       */
      void setBitsPerWord (uint8_t bit);

      /**
       * @brief Modification de l'ordre de transmission des bits \c MsbFirst ou \c LsbFirst
       * Si la liaison est ouverte les réglages sont appliqués immédiatement,
       * sinon ils le seront à l'ouverture par \c open()
       */
      void setBitOrder (bool bitOrder);

      /**
       * @brief Configuration actuelle
       */
      const Settings & settings() const;

      /**
       * @brief Mode de fonctionnement
       * cf \c setMode()
       */
      uint8_t mode() const;

      /**
       * @brief Vitesse maximale de transmission en Hz
       */
      uint32_t speedHz () const;

      /**
       * @brief Nombre de bits par mot
       */
      uint8_t bitsPerWord () const;

      /**
       * @brief Ordre de transmission des bits \c MsbFirst ou \c LsbFirst
       */
      bool bitOrder() const;

    protected:
      class Private;
      SpiDev (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (SpiDev)
  };
  // ---------------------------------------------------------------------------
  //
  //                      Piduino SpiDev Global Object
  //
  // ---------------------------------------------------------------------------
  extern SpiDev SPI; ///< Piduino SpiDev Global Object
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_SPIDEV_H defined */
