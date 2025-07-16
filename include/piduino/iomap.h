/* Copyright © 2015-2018 Pascal JEAN, All rights reserved.
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

#ifndef PIDUINO_IOMAP_H
#define PIDUINO_IOMAP_H

#include <piduino/global.h>

/**
    @defgroup piduino_iomap Accès mémoire IO

    Ce module fournit les fonctions pour accéder à la mémoire où sont implantés
    les coupleurs d'entrée-sortie
    @{
*/

namespace Piduino {

  /**
     @class IoMap
     @brief Projection mémoire
  */
  class IoMap {
    public:
      /**
         @brief Constructeur
      */
      IoMap ();

      /**
         @brief Destructeur
      */
      virtual ~IoMap();

      /**
         @brief Ouverture d'une projection mémoire

         @param base adresse de base de la zone à projeter, doit être un multiple
         de pageSize().
         @param size taille de la zone à projeter en octets
         @return true si ouverte, false si erreur
      */
      bool open (off_t base, size_t size);

      /**
         @brief Fermeture d'une projection mémoire
      */
      void close();

      /**
         @brief Pointeur d'accès aux registres
         @param offset offset à l'intérieur de la zone en sizeof(int)
         @return le pointeur sur le registre, NULL si erreur
      */
      volatile uint32_t *io (size_t offset = 0) const;

      /**
         @brief Pointeur d'accès aux registres
         @param offset offset à l'intérieur de la zone en sizeof(int)
         @return le pointeur sur le registre, NULL si erreur
      */
      volatile uint32_t *operator [] (size_t offset) const {

        return io (offset);
      }

      /**
         @brief Lecture atomique d'un registre
         @param offset offset à l'intérieur de la zone en sizeof(int)
         @return la valeur lue

         This function reads a register atomically, ensuring that the read operation
         is not interrupted by other threads or processes.
      */
      inline uint32_t atomicRead (size_t offset) const {
        volatile uint32_t *reg = io (offset);
        return __atomic_load_n (reg, __ATOMIC_SEQ_CST);
      }

      /**
         @brief Écriture atomique d'un registre
         @param offset offset à l'intérieur de la zone en sizeof(int)
         @param value valeur à écrire

         This function writes a value to a register atomically, ensuring that the write operation
         is not interrupted by other threads or processes.
         It uses the C11 atomic operations to ensure memory consistency.
         @note This function is useful for ensuring that hardware registers are updated safely in a multi-threaded environment.
      */
      inline void atomicWrite (size_t offset, uint32_t value) {
        volatile uint32_t *reg = io (offset);
        __atomic_store_n (reg, value, __ATOMIC_SEQ_CST);
      }

      /**
         @brief Indique si une projection mémoire est ouverte
      */
      bool isOpen() const {

        return _fd >= 0;
      }

      /**
         @brief Adresse de base de la zone projetée
      */
      inline off_t base() const {
        return _base;
      }

      /**
         @brief Taille de la zone projetée
      */
      inline size_t size() const {
        return _size;
      }

      /**
         @brief Taille d'une page de projection
      */
      static size_t pageSize();

    private:
      off_t _base;  /*< adresse de base de la zone */
      size_t _size; /*< Taille de la zone */
      int _fd;      /*< descripteur d'accès à la zone mémoire */
      void *_map;   /*< pointeur de la zone */
  };
}
/**
   @}
*/

/* ========================================================================== */
#endif /* PIDUINO_IOMAP_H defined */
