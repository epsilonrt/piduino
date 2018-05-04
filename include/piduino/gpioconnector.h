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

#ifndef _PIDUINO_GPIO_CONNECTOR_H_
#define _PIDUINO_GPIO_CONNECTOR_H_

#include <iostream>
#include <piduino/gpiopin.h>

namespace Piduino {

  class Gpio;
  class Device;

  /**
   *  @addtogroup piduino_gpio_connector
   *  @{
   */

  /**
   * @class Connector
   * @author Pascal JEAN
   * @date 02/23/18
   * @brief Connecteur
   */
  class Connector {

    public:
      friend class Pin;
      friend class Gpio;

      class Family {
        public:
          /**
           * @enum Id
           * @brief Identifiant
           */
          enum  Id {
            Header1X = 0, ///< Connecteur Header à 1 rangée (SIL)
            Header2X,     ///< Connecteur Header à 2 rangées: 1 impaire, 1 paire (ie HE10)
            Unknown = -1
          };

          Family (Id i = Unknown) : _id (i), _columns (-1), _fnum (nullptr) {
            setId (i);
          }
          virtual ~Family() {}

          inline Id id() const {
            return _id;
          }

          inline int columns() const {
            return _columns;
          }

          inline const std::string &name() const {
            return _name;
          }

          /**
           * @brief Calcul du numéro de broche
           *
           * Permet à une broche de récupérer son numéro dans le connecteur
           *
           * @param row ligne de la broche
           * @param column colonne de la broche
           * @return Numéro de la broche dans le connecteur
           */
          int pinNumber (int row, int column) const;

          void setId (Id i);

        private:
          typedef int (* PinNumberFunc) (int row, int column, int columns);
          Id _id;
          int _columns;
          PinNumberFunc _fnum;
          std::string _name;
      };

      /**
       * @class Descriptor
       * @author Pascal JEAN
       * @date 02/23/18
       * @brief Descripteur d'un connecteur
       */
      class Descriptor {
        public:
          std::string name;
          int number;
          int rows;
          Family family;
          long long id; ///< Database Id
          std::vector<Pin::Descriptor> pin;
          // -- functions
          Descriptor (long long connectorId = -1, int connectorNumber = -1);
          bool insert (); ///< Insertion dans la base de données
          bool hasPin (const Pin::Descriptor & p) const;
          long long findId() const;
      };

      /**
       * @brief Indique si ouvert
       */
      bool isOpen() const;

      /**
       * @brief Nom du connecteur
       */
      const std::string & name() const;

      /**
       * @brief Numéro du connecteur sur la carte (commence à 1)
       */
      int number() const;

      /**
       * @brief Nombre de broches
       */
      int size() const;

      /**
       * @brief Nombre de lignes
       */
      int rows() const;

      /**
       * @brief Nombre de colonnes
       */
      int columns() const;

      /**
       * @brief Modèle du connecteur
       */
      const Family & family() const;

      /**
       * @brief Identifiant en base de données
       */
      long long id() const;

      /**
       * @brief Active le mode mise au point
       *
       * Cela active l'affichage d'informations de mise au point de la couche
       * matérielle (GpioDevice).
       */
      void setDebug (bool enable);

      /**
       * @brief Indique si le mode mise au point est actif
       */
      bool isDebug() const;

      /**
       * @brief Affiche toutes les informations sur le connecteur et ses broches
       *
       * Voici un exemple d'affichage pour le connecteur DBG_UART d'un NanoPi à
       * partir du code suivant:
       *
       * @code
          Piduino::Gpio * board = new Piduino::Gpio;
          ...
          cout << board->connector(4);
          ....
       * @endcode
       *
       * Cela donne à l'affichage :
       *
       * @verbatim
                             CON2 (#4)
          +-----+-----+----------+------+------+---+----+
          | sOc | sIo |   Name   | Mode | Pull | V | Ph |
          +-----+-----+----------+------+------+---+----+
          |     |     |       5V |      |      |   |  1 |
          |     |     |  USB-DP1 |      |      |   |  2 |
          |     |     |  USB-DM1 |      |      |   |  3 |
          |     |     |  USB-DP2 |      |      |   |  4 |
          |     |     |  USB-DM2 |      |      |   |  5 |
          | 105 |  20 |  GPIOL11 |  OUT |  OFF | 0 |  6 |
          |  17 |  11 |  GPIOA17 |  OUT |  OFF | 0 |  7 |
          |  18 |  31 |  GPIOA18 |  OUT |  OFF | 0 |  8 |
          |  19 |  30 |  GPIOA19 |   IN |   UP | 1 |  9 |
          |  20 |  21 |  GPIOA20 |   IN |   UP | 1 | 10 |
          |  21 |  22 |  GPIOA21 |   IN |   UP | 1 | 11 |
          |     |     |      GND |      |      |   | 12 |
          +-----+-----+----------+------+------+---+----+
          | sOc | sIo |   Name   | Mode | Pull | V | Ph |
          +-----+-----+----------+------+------+---+----+
       * @endverbatim
       *
        * @param os os flux de sortie
        * @param c pointeur sur un connecteur
        */
      friend std::ostream& operator<< (std::ostream& os, const Connector * c);

      /**
       * @brief Accès au GPIO parent
       */
      Gpio * gpio() const;

      //------------------------------------------------------------------------
      //                          Accès aux broches
      //------------------------------------------------------------------------

      /**
       * @brief Broche du connecteur
       *
       * @param num numéro de broche dans la numérotation du connecteur. Déclenche
       * une exception std::out_of_range si la broche n'existe pas
       * @return référence sur la broche
       */
      Pin & pin (int num) const;

      /**
       * @brief Liste des broches du connecteur
       *
       * Permet de parcourir les broches à l'aide des itérateurs de la STL
       * par exemple pour mettre en entrée toutes les broches:
       * @code
       *    Gpio * g = new Gpio;
       *    // ....
       *    Connector * c = g->connector(1);
       *    // ....
       *    for (auto p = c->pin().begin(); p != c->pin().end(); ++p) {
       *      // p est une std::pair: first = numéro et second = broche
       *      p->second->setMode(ModeInput);
       *    }
       * @endcode
       *
       *
       * @return Container std::map indexé par numéro de broche connecteur
       */
      const std::map<int, std::shared_ptr<Pin>> & pin ();

      /**
       * @brief Constructeur
       *
       * Un connecteur ne peut être instancié que par un objet Gpio.
       *
       * @param parent pointeur sur le Gpio parent
       * @param desc pointeur sur la description du connecteur
       */
      Connector (Gpio * parent, Descriptor * desc);

      /**
       * @brief Destructeur
       */
      virtual ~Connector();

    protected:
      /**
       * @brief Accès à la couche matérielle
       */
      Device * device() const;

      /**
       * @brief Ouverture
       */
      bool open();

      /**
       * @brief Fermeture du GPIO
       */
      void close();

      /**
       * @brief Calcul du numéro de broche
       *
       * Permet à une broche de récupérer son numéro dans le connecteur
       *
       * @param row ligne de la broche
       * @param column colonne de la broche
       * @return Numéro de la broche dans le connecteur
       */
      int pinNumber (int row, int column) const;

      /**
       * @brief Affiche une ligne horizontale d'un tableau de broches
       * @param os flux d'affichage
       */
      void printHline (std::ostream & os) const;

      /**
       * @brief Affiche l'entête d'un tableau de broches
       * @param os flux d'affichage
       */
      void printTitle (std::ostream & os) const;

      /**
       * @brief Affiche une ligne d'un tableau de broches
       * Si le connecteur à 2 colonnes, les broches num et num+1 sont affichées
       * @param os flux d'affichage
       * @param num numéro de broche dans la numérotation du connecteur. Déclenche
       * une exception std::out_of_range si la broche n'existe pas
       */
      void printRow (std::ostream & os, int num) const;

      /**
       * @brief Modification identifiant en base de données
       */
      inline void setId (int i) {
        _descriptor->id = i;
      }

    private:
      bool _isopen;
      Gpio * _parent;
      Descriptor * _descriptor; // descripteur
      std::map<int, std::shared_ptr<Pin>> _pin; // toutes les broches
  };
}
/**
 * @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_GPIO_CONNECTOR_H_ defined */
