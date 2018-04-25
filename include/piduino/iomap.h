/* Copyright © 2015 Pascal JEAN, All rights reserved.
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

#ifndef _PIDUINO_IOMAP_H_
#define _PIDUINO_IOMAP_H_

#include <stdbool.h>

/**
 *  @defgroup piduino_iomap Accès mémoire IO
 *
 *  Ce module fournit les fonctions pour accéder à la mémoire où sont implantés
 *  les coupleurs d'entrée-sortie
 *  @{
 */

#ifdef __cplusplus
  extern "C" {
#endif

/* structures =============================================================== */

/**
 * @brief Projection mémoire
 *
 * Cet objet permet d'accéder à des registres d'entrée-sortie
 * (gpio, port parallèle...) \n
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xIoMap xIoMap;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'une projection mémoire
 *
 * @param base adresse de base de la zone à projeter
 * @param size taille de la zone à projeter en octets
 * @return pointeur sur la projection, NULL si erreur
 */
xIoMap * xIoMapOpen (unsigned long base, unsigned int size);

/**
 * @brief Fermeture d'une projection mémoire
 *
 * @param p pointeur sur la projection
 */
int iIoMapClose (xIoMap *p);


/**
 * @brief Indique si une projection mémoire est ouverte
 *
 * @param p pointeur sur la projection
 * @return true si ouverte
 */
bool bIoMapIsOpen (const xIoMap *p);

/**
 * @brief Adresse de base de la zone projetée
 *
 * @param p pointeur sur la projection
 * @return la taille en octets
 */
unsigned long ulIoMapBase (const xIoMap *p);

/**
 * @brief Taille de la zone projetée
 *
 * @param p pointeur sur la projection
 * @return l'adresse de base
 */
unsigned int uIoMapSize (const xIoMap *p);

/**
 * @brief Pointeur d'accès aux registres
 *
 * @param p pointeur sur la projection
 * @param offset offset à l'intérieur de la zone en sizeof(int)
 * @return le pointeur sur le registre, NULL si erreur
 */
volatile unsigned int * pIo(const xIoMap *p, unsigned int offset);

#ifdef __cplusplus
  }
#endif

/**
 * @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_IOMAP_H_ defined */
