/* Copyright © 2015-2018 Pascal JEAN, All rights reserved.
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

#ifndef _PIDUINO_GPIO_H_
#define _PIDUINO_GPIO_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <exception>
#include <piduino/gpioconnector.h>

/**
 *  @defgroup piduino_gpio Gpio
 *
 *  Ce module fournit les classes permettant de contrôler les broches
 *  d'entrées-sorties à usage général.
 *  @{
 *  @defgroup piduino_gpio_class Gpio
 *  @defgroup piduino_gpio_pin Broches
 *  @defgroup piduino_gpio_connector Connecteurs
 *  @}
 */

namespace Piduino {

  class Device;

  /**
   *  @addtogroup piduino_gpio_class
   *  @{
   */

  /**
   * @class Gpio
   * @author Pascal JEAN
   * @date 02/22/18
   * @brief Classe Gpio
   */
  class Gpio {
    public:
      friend class Connector;

      /**
       * @class Descriptor
       * @author Pascal JEAN
       * @date 02/23/18
       * @brief Descripteur d'un GPIO
       */
      class Descriptor {
        public:
          std::string name; ///< Nom de la carte
          long long id; ///< Database Id
          std::vector<Connector::Descriptor> connector; ///< Descripteurs des connecteurs
          // TODO
          //Descriptor (long long id = -1);
          bool insertToDb (); ///< Insertion dans la base de données
          bool findInDb() const;
      };


      /**
       * @brief Constructeur par défaut
       * @param layer choix de la couche d'accès, AccessLayerAuto par défaut, dans
       * ce cas, le choix est laissé à la couche GpioDevice (conseillé).
       */
      Gpio (AccessLayer layer = AccessLayerAuto);

      /**
       * @brief Destructeur
       */
      virtual ~Gpio();

      /**
       * @brief Ouverture
       * @return true si ouvert
       */
      bool open();

      /**
       * @brief Fermeture du GPIO
       */
      void close();

      /**
       * @brief Indique si ouvert
       */
      bool isOpen() const;

      /**
       * @brief Nom de la carte
       */
      const std::string & name() const;

      /**
       * @brief Modifie la libération des broches à la fermeture
       *
       * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
       * close(). Cette fonction permet de modifier ce comportement.
       *
       * @param enable true active la libération, false la désactive.
       */
      void setReleaseOnClose (bool enable);

      /**
       * @brief Lecture la libération des broches à la fermeture
       *
       * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
       * close(). Cette fonction permet de lire l'état de cette directive
       *
       * @return true si validé, false sinon
       */
      bool releaseOnClose() const;

      /**
       * @brief Numérotation en cours
       *
       * La numérotation par défaut est \c NumberingLogical.
       */
      Pin::Numbering numbering() const;

      /**
       * @brief Modifie la numérotation
       *
       */
      void setNumbering (Pin::Numbering numbering);

      /**
       * @brief Nom de la numérotation en cours
       */
      const std::string & numberingName () const;

      /**
       * @brief Lecture de la couche d'accès utilisée
       */
      AccessLayer accessLayer() const;

      /**
       * @brief Indique si le mode mise au point est actif
       */
      bool isDebug() const;

      /**
       * @brief Active le mode mise au point
       *
       * Cela active l'affichage d'informations de mise au point de la couche
       * matérielle (GpioDevice).
       */
      void setDebug (bool enable);

      //------------------------------------------------------------------------
      //                          Accès aux connecteurs
      //------------------------------------------------------------------------

      /**
       * @brief Nombre de connecteurs
       */
      int connectors() const;

      /**
       * @brief Connecteur
       *
       * @param conn numéro du connecteur, les numéros ne sont pas forcément
       * contiguës, pour parcourir les connecteurs on utilisera les itérateurs
       * de la STL associés à \c connector(). Déclenche une exception
       * std::out_of_range si en dehors des clous.
       * @return pointeur sur le connecteur
       */
      Connector * connector (int conn) const;

      /**
       * @brief Liste des connecteurs de la carte
       *
       * Permet de parcourir les connecteurs à l'aide des itérateurs de la STL,
       * par exemple pour imprimer tous les connecteurs
       * @code
       *    Gpio * g = new Gpio;
       *    // ....
       *    for (auto p = g->connector().cbegin(); p != g->connector().cend(); ++p) {
       *      // p est une std::pair: first = numéro et second = connecteur
       *      cout << p->second;
       *    }
       * @endcode
       *
       * @return Container std::map des connecteurs indexé sur leur numéro
       */
      const std::map<int, std::shared_ptr<Connector>> & connector();


      //------------------------------------------------------------------------
      //                          Accès aux broches
      //------------------------------------------------------------------------

      /**
       * @brief Nombre de broches de type GPIO sur la carte(\c TypeGpio)
       */
      int size() const;

      /**
       * @brief Broche GPIO
       *
       * @param num numéro de broche dans la numérotation \c numbering(). Déclenche
       * une exception std::out_of_range si la broche n'existe pas
       * @return référence sur la broche
       */
      Pin & pin (int num) const;

      /**
       * @brief Liste des broches de type GPIO
       *
       * Permet de parcourir les broches GPIO à l'aide des itérateurs de la STL,
       * par exemple pour mettre en entrée toutes les broches:
       * @code
       *    Piduino::Gpio * g = new Piduino::Gpio;
       *    // ....
       *    for (auto p = g->pin().cbegin(); p != g->pin().cend(); ++p) {
       *      // p est une std::pair: first = numéro et second = broche
       *      p->second->setMode(ModeInput);
       *    }
       * @endcode
       *
       * @return Container std::map des broches indexé avec la numérotation
       * \c numbering().
       */
      const std::map<int, std::shared_ptr<Pin>> & pin();

    protected:
      /**
       * @brief Accès à la couche matérielle
       */
      Device * device() const;

      /**
       * @brief Constructeur sous jacent
       *
       * Doit être appelé par le constucteur par défaut défini dans la partie
       * spécifique à la plateforme.
       * @param device pointeur sur le driver de la plateforme
       * @param layer choix de la couche d'accès
       */
      explicit Gpio (Device * device, AccessLayer layer);

    private:
      bool _roc; // Release On Close
      bool _isopen;
      AccessLayer _accesslayer;
      Device * _device; // Accès à la couche matérielle
      Pin::Numbering _numbering; // Numérotation en cours
      Descriptor _descriptor;
      std::map<int, std::shared_ptr<Pin>> _pin; // Broches uniquement GPIO
      std::map<int, std::shared_ptr<Connector>> _connector; // Connecteurs avec toutes les broches physiques
  };
  // ---------------------------------------------------------------------------
  //
  //                      Piduino Gpio Global Object
  //
   // ---------------------------------------------------------------------------
  //extern Gpio gpio; ///< Piduino Gpio Global Object, must be open before using !
  /**
   * @}
   */
}

/* ========================================================================== */
#endif /*_PIDUINO_GPIO_H_ defined */
