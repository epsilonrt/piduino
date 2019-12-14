/* Copyright © 2019 Pascal JEAN, All rights reserved.
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
#define SYSLOG_NAMES 1
#include <cstdio>
#include <iostream>
#include <piduino/syslog.h>

namespace Piduino {

  // ---------------------------------------------------------------------------
  std::string syslogCodeName (int c, const CODE * n) {

    while ( (n->c_name) && (n->c_val != c)) {

      n++;
    }
    if (n->c_name == NULL) {
      return std::string ("Unknown");
    }
    return std::string (n->c_name);
  }

  // ---------------------------------------------------------------------------
  //
  //                        SysLog Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  SysLog::SysLog (Priority  priority) :
    SysLogBuffer (priority),
    std::ostream (static_cast<std::streambuf*> (this)),
    m_option (NoOption),
    m_facility (NoFacility),
    m_isopen (false)
  {}

  // ---------------------------------------------------------------------------
  SysLog::~SysLog () {
    close();
  }

  // ---------------------------------------------------------------------------
  void SysLog::open (const char * ident, const SysLog::Facility & facility, const SysLog::Option & option) {

    if (!is_open()) {

      m_option = option;
      m_facility = facility;
      ::openlog (ident, option.value(), static_cast<int> (facility));
      m_isopen = true;
    }
  }

  // ---------------------------------------------------------------------------
  void SysLog::open (const std::string & ident, const SysLog::Facility & facility, const SysLog::Option & option) {

    open (ident.c_str(), facility, option);
  }

  // ---------------------------------------------------------------------------
  void SysLog::open (const SysLog::Facility & facility, const SysLog::Option & option) {

    open (NULL, facility, option);
  }

  // ---------------------------------------------------------------------------
  void SysLog::close() {

    if (is_open()) {

      pubsync();
      ::closelog();
      m_option = NoOption;
      m_facility = NoFacility;
      m_isopen = false;
    }
  }

  // ---------------------------------------------------------------------------
  void SysLog::log (Priority  p, const char *format, ...) {

    if (is_open()) {
      va_list va;

      va_start (va, format);
      ::vsyslog (p, format, va);
      va_end (va);
    }
  }

  // ---------------------------------------------------------------------------
  void SysLog::log (const char *format, ...) {

    if (is_open()) {
      va_list va;

      va_start (va, format);
      log (priority(), format, va);
      va_end (va);
    }
  }

  // ---------------------------------------------------------------------------
  // static
  std::string
  SysLog::priorityName (Priority  p) {
    return syslogCodeName (p, prioritynames);
  }

  // ---------------------------------------------------------------------------
  // static
  std::string
  SysLog::facilityName (const Facility & facility) {
    return syslogCodeName (static_cast<int> (facility), facilitynames);
  }

  // ---------------------------------------------------------------------------
  // static
  void SysLog::cerrToSyslog () {

    std::cerr << std::nounitbuf;
    std::cerr.rdbuf (Syslog.rdbuf());
  }
  
  // ---------------------------------------------------------------------------
  // static
  void SysLog::coutToSyslog () {

    std::cout.rdbuf (Syslog.rdbuf());
  }

  // ---------------------------------------------------------------------------
  //
  //                        SysLogBuffer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  int SysLogBuffer::sync() {

    if (in_avail()) {

      sputc (0);
      ::syslog (m_priority, "%s", gptr());
      seekoff (0, std::ios_base::beg, std::ios_base::in | std::ios_base::out);
      setg (pbase(), pbase(), pbase());
    }
    return 0;
  }

  // ---------------------------------------------------------------------------
  void SysLogBuffer::setPriority (const int priority) {

    pubsync();
    m_priority = priority;
  }

}
