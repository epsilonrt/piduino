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
#pragma once

#include <stdarg.h>
#include <syslog.h>
#include <sstream>
#include <string>
#include <piduino/flags.h>
#include <piduino/global.h>

/**
 * @defgroup piduino_syslog Gestion des logs
 *
 * Ce module fournit les fonctions permettant d'afficher des messages sur
 * la sortie d'erreur ou dans syslog. \n
 * Les logs sont organisés en fonction de priorités, dans l'ordre décroissant
 * d'importance nous avons :
 * - LOG_EMERG      Le système est inutilisable.
 * - LOG_ALERT      Des actions doivent être entreprises immédiatement.
 * - LOG_CRIT       Les conditions sont critiques.
 * - LOG_ERR        Des erreurs se produisent.
 * - LOG_WARNING    Des avertissement se présentent.
 * - LOG_NOTICE     Condition normale, mais message significatif.
 * - LOG_INFO       Message d'information simple.
 * - LOG_DEBUG      Message de débogage.
 * .
 *
 *  @{
 */

namespace Piduino {

#ifndef __DOXYGEN__
  class SysLogBuffer :  public std::stringbuf {
    protected:
      SysLogBuffer (int priority) : m_priority (priority) {}
      int sync();
      void setPriority (const int priority);
      inline int priority() const {
        return m_priority;
      }
    private:
      int m_priority;
  };
#endif

  /**
   * @class SysLog
   * @brief System logger
   */
  class SysLog : protected SysLogBuffer, public std::ostream  {
    public:

      /**
       * @enum Priority
       * @brief importance of the message
       *
       * This determines the importance of the message. The levels are, in order
       * of decreasing importance
       */
      enum  Priority {
        Emergency = LOG_EMERG, ///< system is unusable
        Alert = LOG_ALERT, ///< action must be taken immediately
        Critical = LOG_CRIT, ///< critical conditions
        Error = LOG_ERR, ///< error conditions
        Warning = LOG_WARNING, ///< warning conditions
        Notice = LOG_NOTICE, ///< normal, but significant, condition
        Info = LOG_INFO, ///< informational message
        Debug = LOG_DEBUG, ///< debug-level message
        NoPriority = -1
      };

      /**
       * @enum Facility
       * @brief
       *
       * The facility argument is used to specify what type of program is
       * logging the message. This lets the configuration file specify that
       * messages from different facilities will be handled differently.
       */
      enum Facility {
        Auth = LOG_AUTH, ///< security/authorization messages
        AuthPriv = LOG_AUTHPRIV, ///<  security/authorization messages (private)
        Cron = LOG_CRON, ///<  clock daemon (cron and at)
        Daemon = LOG_DAEMON, ///<  system daemons without separate facility value
        Ftp = LOG_FTP, ///<  ftp daemon
        Kernel = LOG_KERN, ///<  kernel messages (these can't be generated from user processes)
        Local0 = LOG_LOCAL0, ///< reserved for local use
        Local1 = LOG_LOCAL1, ///< reserved for local use
        Local2 = LOG_LOCAL2, ///< reserved for local use
        Local3 = LOG_LOCAL3, ///< reserved for local use
        Local4 = LOG_LOCAL4, ///< reserved for local use
        Local5 = LOG_LOCAL5, ///< reserved for local use
        Local6 = LOG_LOCAL6, ///< reserved for local use
        Local7 = LOG_LOCAL7, ///< reserved for local use
        Lpr = LOG_LPR, ///<  line printer subsystem
        Mail = LOG_MAIL, ///<  mail subsystem
        News = LOG_NEWS, ///<  USENET news subsystem
        Syslogd = LOG_SYSLOG, ///<  messages generated internally by syslogd(8)
        User = LOG_USER, ///<  (default) generic user-level messages
        Uucp = LOG_UUCP, ///<  UUCP subsystem
        NoFacility = -1
      };

      /**
       * @enum OptionFlag
       * @brief The option argument specifies flags which control the operation 
       * of \c open() and subsequent calls to \c log()
       */
      enum OptionFlag {

        Console = LOG_CONS, ///< Write directly to system console if there is an error while sending to system logger.
        NoDelay = LOG_NDELAY, ///< Open the connection immediately (normally, the connection is opened when the first message is logged).
        NoWait = LOG_NOWAIT, ///< Don't wait for child processes that may have been created while logging the message. (The GNU C library does not create a child process, so this option has no effect on Linux.)
        Delay = LOG_ODELAY, ///< The converse of LOG_NDELAY; opening of the connection is delayed until syslog() is called. (This is the default, and need not be specified.)
        Perror = LOG_PERROR, ///< (Not in POSIX.1-2001 or POSIX.1-2008.) Print to stderr as well.
        Pid = LOG_PID, ///< Include PID with each message.
        NoOption = 0
      };

      /**
       * @typedef Option
       * @brief
       */
      typedef Flags<OptionFlag> Option;
      
      /**
       * @brief Constructor
       * @param initialPrio initial priority
       */
      SysLog (Priority  initialPrio = Info);
      
      /**
       * @brief Destructor
       */
      ~SysLog();

      /**
       * @brief Opens a connection to the system logger
       * 
       * The string pointed to by ident is prepended to every message, and is 
       * typically set to the program name.
       */
      void open (const std::string & ident, const SysLog::Facility & facility = User, const SysLog::Option & option = Pid);
      /**
       * @brief Opens a connection to the system logger
       * @overload 
       */
      void open (const SysLog::Facility & facility = User, const SysLog::Option & option = Pid);

      /**
       * @brief Returns whether the object is currently associated to the system logger.
       */
      inline bool is_open() const {
        return m_isopen;
      }
      /**
       * @brief closes the descriptor being used to write to the system logger. 
       * 
       * The use of close() is optional. 
       */
      void close();


      /**
       * @brief Generates a log message
       *
       * The priority argument is formed by ORing the facility and the level values (explained below).
       * The remaining arguments are a format, as in printf(3) and any arguments
       * required by the format, except that the two character sequence %m will
       * be replaced by the error message string strerror(errno).
       * A trailing newline may be added if needed.
       */
      void log (Priority  p, const char *format, ...);

      /**
       * @brief Generates a log message
       * @overload
       */
      void log (const char *format, ...);

      inline void setMask (Priority  p) {
        (void) setlogmask (LOG_MASK (p));
      }

      inline void setMaskUpTo (Priority  p) {
        (void) setlogmask (LOG_UPTO (p));
      }

      inline int mask() const {
        return setlogmask (0);
      }

      inline void setPriority (Priority  p = Info) {
        SysLogBuffer::setPriority (static_cast<int> (p));
      }

      inline Priority priority() const {
        return static_cast<Priority> (SysLogBuffer::priority());
      }

      inline std::string priorityName() const {
        return priorityName (priority());
      }

      /**
       * @brief Reads the current facility
       *
       * The facility argument is used to specify what type of program is
       * logging the message. This lets the configuration file specify that
       * messages from different facilities will be handled differently.
       */
      inline Facility facility() const {
        return m_facility;
      }

      /**
       * @brief Name of the current facility
       * @return the facilty name, "Unknown" if is not opened
       */
      inline std::string facilityName() const {
        return facilityName (facility());
      }

      /**
       * @brief Reads the current option flags
       */
      inline Option option() const {
        return m_option;
      }

      /**
       * @brief Name of a priority
       */
      static std::string priorityName (Priority  p);

      /**
       * @brief Name of a facility
       */
      static std::string facilityName (const Facility & facility);
      
      /**
       * @brief redirect std::cerr to the system logger
       */
      static void cerrToSyslog ();
      
      /**
       * @brief redirect std::cout to the system logger
       */
      static void coutToSyslog ();

    private:
      Option m_option;
      Facility m_facility;
      bool m_isopen;
    private:
      void open (const char * ident, const SysLog::Facility & facility, const SysLog::Option & option);
  };

#ifndef __DOXYGEN__
  struct _Setpriority {
    SysLog::Priority m_value;
  };

  inline _Setpriority
  setpriority (SysLog::Priority __priority) {
    return { __priority };
  }

  inline std::ostream &
  operator<< (std::ostream & __os, _Setpriority __p) {
    SysLog * log = dynamic_cast<SysLog *> (&__os);

    log->setPriority (__p.m_value);
    // __os << "setpriority(" << log->priorityName() << ") ";
    return __os;
  }
#endif

  // ---------------------------------------------------------------------------
  //
  //                      Piduino SysLog Global Object
  //
  // ---------------------------------------------------------------------------
  extern SysLog Syslog; ///< Piduino SysLog Global Object
}
/**
 * @}
 */
