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
#include <iostream>
#include <sstream>
#include "exception.h"

// -----------------------------------------------------------------------------
Exception::Exception (Exception::Code code, int value) :
  _code (code), _value (value) {
  std::ostringstream s;

  switch (_code) {
    case CommandExpected:
      _msg.assign ("Command expected");
      break;
    case ArgumentExpected:
      _msg.assign ("Arguments expected");
      break;
    case BadArguments:
      _msg.assign ("Bad arguments");
      break;
    case PinNumberExpected:
      _msg.assign ("Pin number expected");
      break;
    case NotBinaryValue:
      s << _value << " is not a binary value";
      _msg = s.str();
      break;
    case NotPwmValue:
      s << _value << " is not a pwm value";
      _msg = s.str();
      break;
    case NotOutputPin:
      s << "Pin #" << value << " is not an output";
      _msg = s.str();
    case NotPwmPin:
      s << "Pin #" << value << " is not an pwm output";
      _msg = s.str();
      break;
    case PwmOpenError:
      s << "Unable to open pwm Pin #" << value;
      _msg = s.str();
      break;
    case PwmWriteError:
      s << "Unable to write on pwm Pin #" << value;
      _msg = s.str();
      break;

    default:
      break;
  }
}


Exception::Exception (Exception::Code code, const std::string& param) :
  _code (code), _param (param) {

  switch (_code) {
    case UnknownCommand:
      _msg.assign ("Invalid operation: ");
      break;
    case IllegalMode:
      _msg.assign ("Illegal mode: ");
      break;
    case BadPinNumber:
      _msg.assign ("Invalid pin number: ");
      break;
    case ConverterUnknown:
      _msg.assign ("Unknown converter: ");
      break;
    case ConverterOpenError:
      _msg.assign ("Unable to open converter: ");
      break;
    case ConverterWriteError:
      _msg.assign ("Unable to write on converter: ");
      break;
    case ConverterReadError:
      _msg.assign ("Unable to read from converter: ");
      break;
    default:
      break;
  }
  _msg.append (_param);
}
/* ========================================================================== */
