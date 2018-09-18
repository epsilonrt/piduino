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
#include <chrono>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include "terminalnotifier_p.h"

namespace Piduino {

// -----------------------------------------------------------------------------
//
//                             TerminalNotifier Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  TerminalNotifier::TerminalNotifier (TerminalNotifier::Private &dd) : d_ptr (&dd) {

  }

  // ---------------------------------------------------------------------------
  TerminalNotifier::TerminalNotifier (FileDevice * io) :
    d_ptr (new Private (this, io))  {

  }

  // ---------------------------------------------------------------------------
  TerminalNotifier::~TerminalNotifier() {

    terminate();
  }

  // ---------------------------------------------------------------------------
  bool TerminalNotifier::start () {
    PIMP_D (TerminalNotifier);

    if (!isRunning() && (d->io->openMode() & IoDevice::ReadOnly)) {
      struct termios term;

      // Get the current terminal settings so that only
      // the settings we want to change are changed
      if (tcgetattr (d->io->fd(), &term) == -1) {
        return false;
      }

      // Save the original terminal settings
      d->pterm = term;

      // turn buffering off
      term.c_lflag &= ~ICANON;

      // Make sure that read() will wait for a single character
      term.c_cc[VMIN] = 1;
      term.c_cc[VTIME] = 0;

      // Apply the changed settings
      if (tcsetattr (d->io->fd(), TCSANOW, &term) == -1) {
        return false;
      }

      // Fetch std::future object associated with promise
      std::future<void> running = d->stopRead.get_future();

      // Starting Thread & move the future object in lambda function by reference
      d->readThread = std::thread (&Private::readNotifier, std::move (running), d);

    }
    return isRunning();
  }

  // ---------------------------------------------------------------------------
  void TerminalNotifier::terminate () {

    if (isRunning()) {

      // Set the value in promise
      d_ptr->stopRead.set_value();
      // Wait for thread to join
      d_ptr->readThread.join();
    }
  }

  // ---------------------------------------------------------------------------
  bool TerminalNotifier::isRunning() const {

    return d_ptr->readThread.joinable();
  }

  // ---------------------------------------------------------------------------
  size_t TerminalNotifier::available() const {

    return d_ptr->buf.size();
  }

  // ---------------------------------------------------------------------------
  bool TerminalNotifier::read (char & c, long msTimeout) {

    return d_ptr->buf.read (c, msTimeout);
  }

  // ---------------------------------------------------------------------------
  size_t TerminalNotifier::read (char * buf, size_t max, long msTimeout) {

    return d_ptr->buf.read (buf, max, msTimeout);
  }

  // ---------------------------------------------------------------------------
  size_t TerminalNotifier::read (std::string & str, long msTimeout) {
    std::vector<char> v;

    d_ptr->buf.read (v, msTimeout);
    str.assign (v.data(), v.size());
    return str.size();
  }

  // ---------------------------------------------------------------------------
  bool TerminalNotifier::peek (char & c, long msTimeout) {

    return d_ptr->buf.peek (c, msTimeout);
  }

  // ---------------------------------------------------------------------------
  size_t TerminalNotifier::peek (char * buf, size_t max, long msTimeout) {

    return d_ptr->buf.peek (buf, max, msTimeout);
  }

  // ---------------------------------------------------------------------------
  size_t TerminalNotifier::peek (std::string & str, long msTimeout) {
    std::vector<char> v;

    d_ptr->buf.peek (v, msTimeout);
    str.assign (v.data(), v.size());
    return str.size();
  }

// -----------------------------------------------------------------------------
//
//                         TerminalNotifier::Private Class
//
// -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  TerminalNotifier::Private::Private (TerminalNotifier * q, FileDevice * iofile) :
    q_ptr (q), io (iofile) {}

  // ---------------------------------------------------------------------------
  TerminalNotifier::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  void * TerminalNotifier::Private::readNotifier (std::future<void> run, TerminalNotifier::Private * d) {
    int len = 0;

    while ( (run.wait_for (std::chrono::milliseconds (1)) == std::future_status::timeout) &&
            (len >= 0) && (d->io->openMode() & IoDevice::ReadOnly)) {

      len = poll (d->io->fd(), 50);
      if (len > 0) {
        long ret;
        std::vector<char> v (len);

        ret = d->io->FileDevice::read (v.data(), len);
        if (ret > 0) {
          if (ret != len) {
            v.resize (ret);
          }
          d->buf.write (v);
        }
      }
    }
    tcsetattr (d->io->fd(), TCSANOW, &d->pterm);
    return 0;
  }

  // ---------------------------------------------------------------------------
  int
  TerminalNotifier::Private::poll (int fd, unsigned long timeout_ms) {
    int ret;
    fd_set set;

    // Initialize the file descriptor set
    FD_ZERO (&set);
    FD_SET (fd, &set);

    if (timeout_ms < 0) {

      // select returns 0 if timeout, 1 if input available, -1 if error
      ret = ::select (FD_SETSIZE, &set, NULL, NULL, NULL);
    }
    else {
      struct timeval timeout;
      unsigned long timeout_us = timeout_ms * 1000UL;

      // Initialize the timeout data structure
      timeout.tv_sec  = timeout_us / 1000000UL;
      timeout.tv_usec = timeout_us % 1000000UL;

      // select returns 0 if timeout, 1 if input available, -1 if error
      ret = ::select (FD_SETSIZE, &set, NULL, NULL, &timeout);
    }

    if (ret == -1) {
      if (errno == EINTR) {

        ret = 0;
      }
    }
    else if ( (ret > 0) && (FD_ISSET (fd, &set))) {
      int available_data;

      ret = ::ioctl (fd, FIONREAD, &available_data);
      if (ret == 0) {

        ret = available_data;
      }
    }

    return ret;
  }
}
/* ========================================================================== */
