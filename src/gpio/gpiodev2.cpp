/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <piduino/global.h>
#include "gpiodev2.h"

namespace GpioDev2 {
  // ---------------------------------------------------------------------------
  //                        Line Class
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Constructor with multiple lines
  Line::Line (Chip *chip, uint32_t num_lines, const uint32_t *offsets) : m_chip (chip) {

    memset (&m_req, 0, sizeof (m_req));
    m_req.num_lines = num_lines;
    m_req.fd = -1;
    memcpy (m_req.offsets, offsets, num_lines * sizeof (uint32_t));

    // m_req.config.flags |= GPIO_V2_LINE_FLAG_INPUT;
    strncpy (m_req.consumer, chip->consumer().c_str(), sizeof (m_req.consumer));
  }

  // ---------------------------------------------------------------------------
  // Constructor with single line
  Line::Line (Chip *chip, uint32_t offset) : Line (chip, 1, &offset) {}


  // ---------------------------------------------------------------------------
  // Constructor with LineRequest
  Line::Line (Chip *chip, const LineRequest &request) : m_chip (chip), m_req (request) {

    // m_req.config.flags |= GPIO_V2_LINE_FLAG_INPUT;
    strncpy (m_req.consumer, chip->consumer().c_str(), sizeof (m_req.consumer));
  }

  // ---------------------------------------------------------------------------
  bool Line::open (uint64_t flags) {

    if (m_chip->isOpen() && !isOpen()) {

      m_req.config.flags = flags;
      return m_chip->ioCtl (GPIO_V2_GET_LINE_IOCTL, &m_req);
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  // Open the line with a specific configuration
  bool Line::open (const LineConfig &config) {

    if (m_chip->isOpen() && !isOpen()) {

      m_req.config = config;
      return m_chip->ioCtl (GPIO_V2_GET_LINE_IOCTL, &m_req);
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  // Set the configuration of the line
  // The configuration is set only if the line is already open
  // if not, config is set in the request and will be used when the line is opened
  // return if the configuration was effectively set
  bool Line::setConfig (const LineConfig &config) {

    m_req.config = config;
    if (isOpen()) {

      int ret = ::ioctl (m_req.fd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &config);
      return (ret >= 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Line::isOpen() const {

    return m_req.fd >= 0;
  }

  // ---------------------------------------------------------------------------
  bool Line::close() {

    if (isOpen()) {
      int ret = ::close (m_req.fd);
      m_req.fd = -1;
      return ret == 0;
    }
    return false;
  }

  // ---------------------------------------------------------------------------

  bool Line::setDebounce (uint32_t debounce_ms) {

    if (debounce_ms != debounce()) {
      LineConfig &cfg = m_req.config;
      auto &debounce = cfg.attrs[0];

      if (debounce_ms > 0) {

        debounce.attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
        debounce.attr.debounce_period_us = debounce_ms * 1000;
        debounce.mask |= 1 << 0;
        cfg.num_attrs = 1;
      }
      else {

        debounce.attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
        debounce.attr.debounce_period_us = 0;
        debounce.mask = 0;
        cfg.num_attrs = 0;
      }
      if (isOpen()) {

        return setConfig (m_req.config);
      }
    }
    return false;
  }


  // ---------------------------------------------------------------------------
  bool Line::waitForEvent (LineEvent &event, int timeout_ms) {

    if (isOpen()) {

      pollfd pfd[1];
      pfd[0].fd = m_req.fd;
      pfd[0].events = POLLIN | POLLPRI;

      int ret = ::poll (pfd, 1, timeout_ms);
      if (ret > 0) {
        return ::read (m_req.fd, &event, sizeof (event)) == sizeof (event);
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool Line::getValues (LineValues &values) const {

    if (isOpen()) {

      int ret = ::ioctl (m_req.fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values);
      return (ret >= 0);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // Get the value of a specific line

  bool Line::getValue (int line_no) const {

    if (isOpen()) {
      LineValues values;
      values.mask = 1 << line_no;
      values.bits = 0;

      if (getValues (values)) {

        return ( (values.bits & values.mask) != 0);
      }
      else {

        throw std::system_error (errno, std::system_category(), EXCEPTION_MSG ("Failed to get line values"));
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // Set the value of a specific line
  bool Line::setValue (int line_no, bool value) {

    if (isOpen()) {
      LineValues values;
      values.mask = 1 << line_no;
      values.bits = value ? values.mask : 0;

      return setValues (values);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // Set the values of the lines
  // The values are set in the same order as the offsets in the request
  // The mask is used to select the lines to set
  // The bits are used to set the values of the lines
  bool Line::setValues (const LineValues &values) {

    if (isOpen()) {

      int ret = ::ioctl (m_req.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
      return (ret >= 0);
    }
    return false;
  }



}