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
  TerminalNotifier::TerminalNotifier () :
    d_ptr (new Private (this))  {

  }

  // ---------------------------------------------------------------------------
  TerminalNotifier::~TerminalNotifier() {

    terminate();
  }

  // ---------------------------------------------------------------------------
  bool TerminalNotifier::start (int fd) {

    if (!isRunning()) {
      struct termios term;

      // Get the current terminal settings so that only
      // the settings we want to change are changed
      if (tcgetattr (fd, &term) == -1) {
        return false;
      }

      // Save the original terminal settings
      d_ptr->pterm = term;

      // turn buffering off
      term.c_lflag &= ~ICANON;

      // Make sure that read() will wait for a single character
      term.c_cc[VMIN] = 1;
      term.c_cc[VTIME] = 0;

      // Apply the changed settings
      if (tcsetattr (fd, TCSANOW, &term) == -1) {
        return false;
      }

      d_ptr->fd = fd;

      // Fetch std::future object associated with promise
      std::future<void> running = d_ptr->stop.get_future();

      // Starting Thread & move the future object in lambda function by reference
      d_ptr->thread = std::thread (&Private::notifyThread, std::move (running), d_ptr.get());

    }
    return isRunning();
  }

  // ---------------------------------------------------------------------------
  void TerminalNotifier::terminate () {

    if (isRunning()) {

      // Set the value in promise
      d_ptr->stop.set_value();
      // Wait for thread to join
      d_ptr->thread.join();

      d_ptr->fd = -1;
    }
  }

  // ---------------------------------------------------------------------------
  bool TerminalNotifier::isRunning() const {

    return d_ptr->thread.joinable();
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
  TerminalNotifier::Private::Private (TerminalNotifier * q) :
    q_ptr (q), fd (-1) {}

  // ---------------------------------------------------------------------------
  TerminalNotifier::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  void * TerminalNotifier::Private::notifyThread (std::future<void> run, TerminalNotifier::Private * d) {
    int len = 0;

    while ( (run.wait_for (std::chrono::milliseconds (1)) == std::future_status::timeout) && (len >= 0)) {

      len = poll (d->fd, 50);
      if (len > 0) {
        long ret;
        std::vector<char> v (len);

        ret = ::read (d->fd, v.data(), len);
        if (ret > 0) {
          if (ret != len) {
            v.resize (ret);
          }
          d->buf.write (v);
        }
      }
    }
    tcsetattr (d->fd, TCSANOW, &d->pterm);
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
