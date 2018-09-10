/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * Copyright © 2010-2016 Stephen Kell, https://github.com/stephenrkell
 * Copyright © 2005 Richard Kreckel, https://www.ginac.de/~kreckel
 * This file is part of the Piduino Library.
 *
 * Stephen Kell's distribution of Richard Kreckel's fileno() function for
 * popular implementations of the C++ standard library.
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
#ifndef PIDUINO_FILENO_H
#define PIDUINO_FILENO_H

#include <iosfwd>

namespace Piduino {

  /**
   * @brief Map a stream pointer to a file descriptor 
   * 
   * Returns the integer file descriptor associated with the C++ stream pointed to by stream. 
   */
  template <typename charT, typename traits>
  int fileno (const std::basic_ios<charT, traits>& stream);
}

/* ========================================================================== */
#endif /*PIDUINO_FILENO_H defined */
