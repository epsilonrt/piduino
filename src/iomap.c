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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <piduino/iomap.h>

/* constants ================================================================ */
#define IOMAP_SHM "/dev/mem"

/* structures =============================================================== */
typedef struct xIoMap {
  unsigned long base; /*< adresse de base de la zone */
  unsigned int size; /*< Taille de la zone */
  int fd;           /*< descripteur d'accès à la zone mémoire */
  void * map;         /*< pointeur de la zone */
  volatile unsigned int * io; /*< pointeur d'accès aux registres */
} xIoMap;

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xIoMap *
xIoMapOpen (unsigned long base, unsigned int size) {

  xIoMap * p = malloc (sizeof(xIoMap));
  if (!p) {

    return 0;
  }
  memset (p, 0, sizeof (xIoMap));


  if ( (p->fd = open (IOMAP_SHM, O_RDWR | O_SYNC | O_CLOEXEC)) < 0) {  // TODO: NO POSIX COMPLIANT

    perror ("open");
    goto iomap_open_error;
  }

  p->map = mmap (
               NULL,
               size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               p->fd,
               base
             );

  if (p->map == MAP_FAILED) {

    perror ("mmap");
    goto iomap_open_error;
  }

  p->io = (volatile unsigned int *) p->map;
  p->base = base;
  p->size = size;
  return p;

iomap_open_error:
  fprintf (stderr, "Failed to open %s, try checking permissions.\n", IOMAP_SHM);
  free(p);
  return NULL;
}

// -----------------------------------------------------------------------------
int
iIoMapClose (xIoMap *p) {
  assert (p);
  int i = 0;

  if ((i = munmap (p->map, p->size)) != 0) {

    perror ("munmap");
  }
  if (close (p->fd) != 0) {  // TODO: NO POSIX COMPLIANT

    i = -1;
    perror ("close");
  }
  return i;
}

// -----------------------------------------------------------------------------
bool
bIoMapIsOpen (const xIoMap *p) {

  if (p) {

    return (p->io != 0);
  }
  return false;
}

// -----------------------------------------------------------------------------
volatile unsigned int *
pIo(const xIoMap *p, unsigned int offset) {
  assert (p);

  return p->io + offset;
}

// -----------------------------------------------------------------------------
unsigned long
ulIoMapBase (const xIoMap *p) {
  assert (p);

  return p->base;
}

// -----------------------------------------------------------------------------
unsigned int
uIoMapSize (const xIoMap *p) {
  assert (p);

  return p->size;
}

/* ========================================================================== */
