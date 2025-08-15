/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino pido tool.
 * Piduino pido tool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Piduino pido tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Piduino pido tool.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _PIDUINO_UTILS_PIDO_EXCEPTION_H_
#define _PIDUINO_UTILS_PIDO_EXCEPTION_H_
#include <string>
#include <exception>

// -----------------------------------------------------------------------------
class Exception: public std::exception {
  public:
    enum Code {
      CommandExpected = 1,
      UnknownCommand,
      ArgumentExpected,
      BadArguments,
      IllegalMode,
      PinNumberExpected,
      BadPinNumber,
      NotBinaryValue,
      NotPwmValue,
      NotOutputPin,
      NotPwmPin,
      PwmOpenError,
      PwmWriteError,
      ConverterUnknown,
      ConverterOpenError,
      ConverterWriteError,
      ConverterReadError,
      ConverterModeError,
      ConverterPullError,
    };

    explicit Exception (Exception::Code code, int value = 0);
    explicit Exception (Exception::Code code, const std::string& param);
    explicit Exception (Exception::Code code, const char* param) : Exception (code, std::string (param)) {}
    virtual ~Exception() throw () {}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char * what() const throw () {

      return _msg.c_str();
    }


    virtual Exception::Code code() const throw () {
      return _code;
    }

  protected:
    Exception::Code _code;
    std::string _param;
    int _value;
    std::string _msg;
};
/* ========================================================================== */
#endif /*_PIDUINO_UTILS_PIDO_EXCEPTION_H_ defined */
