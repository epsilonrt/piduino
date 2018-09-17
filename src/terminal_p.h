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
#ifndef PIDUINO_TERMINAL_PRIVATE_H
#define PIDUINO_TERMINAL_PRIVATE_H

#include <piduino/terminal.h>
#include "filestream_p.h"

namespace Piduino {

  /**
   * @class Terminal::Private
   * @brief
   */
  class Terminal::Private  : public FileStream::Private {

    public:
      Private (Terminal * q);

      virtual bool open (OpenMode mode, int additionalPosixFlags = 0);
      virtual void close();

      Piduino::TerminalNotifier notifier;

      PIMP_DECLARE_PUBLIC (Terminal)
  };
}

/* ========================================================================== */
#endif /* PIDUINO_TERMINAL_PRIVATE_H defined */
