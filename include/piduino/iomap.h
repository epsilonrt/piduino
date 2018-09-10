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

#ifndef PIDUINO_IOMAP_H
#define PIDUINO_IOMAP_H

/**
 *  @defgroup piduino_iomap Accès mémoire IO
 *
 *  Ce module fournit les fonctions pour accéder à la mémoire où sont implantés
 *  les coupleurs d'entrée-sortie
 *  @{
 */

namespace Piduino {

  /**
   * @class IoMap
   * @brief Projection mémoire
   */
  class IoMap {
    public:
      /**
       * @brief Constructeur
       */
      IoMap ();

      /**
       * @brief Destructeur
       */
      virtual ~IoMap();

      /**
       * @brief Ouverture d'une projection mémoire
       *
       * @param base adresse de base de la zone à projeter
       * @param size taille de la zone à projeter en octets
       * @return true si ouverte, false si erreur
       */
      bool open (unsigned long base, unsigned int size);

      /**
       * @brief Fermeture d'une projection mémoire
       */
      void close();

      /**
       * @brief Pointeur d'accès aux registres
       * @param offset offset à l'intérieur de la zone en sizeof(int)
       * @return le pointeur sur le registre, NULL si erreur
       */
      volatile unsigned int * io (unsigned int offset = 0) const;

      /**
       * @brief Pointeur d'accès aux registres
       * @param offset offset à l'intérieur de la zone en sizeof(int)
       * @return le pointeur sur le registre, NULL si erreur
       */
      volatile unsigned int * operator [] (unsigned int offset) const {

        return io (offset);
      }

      /**
       * @brief Indique si une projection mémoire est ouverte
       */
      bool isOpen() const {

        return _fd >= 0;
      }

      /**
       * @brief Adresse de base de la zone projetée
       */
      inline unsigned long base() const {
        return _base;
      }

      /**
       * @brief Taille de la zone projetée
       */
      inline unsigned int size() const {
        return _size;
      }

    private:
      unsigned long _base; /*< adresse de base de la zone */
      unsigned int _size; /*< Taille de la zone */
      int _fd;           /*< descripteur d'accès à la zone mémoire */
      void * _map;         /*< pointeur de la zone */
  };
}
/**
 * @}
 */

/* ========================================================================== */
#endif /*PIDUINO_IOMAP_H defined */
