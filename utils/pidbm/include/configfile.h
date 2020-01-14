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
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>


class Convert {

  public:
    // -----------------------------------------------------------------------
    template <typename T>
    static std::string T_to_string (T const &val) {
      std::ostringstream ostr;

      ostr << val;
      return ostr.str();
    }

    // -----------------------------------------------------------------------
    template <typename T>
    static T string_to_T (std::string const &val) {
      std::istringstream istr (val);
      T returnVal;

      if (! (istr >> returnVal)) {

        throw std::invalid_argument ("Not a valid " +
                                     (std::string) typeid (T).name() + " received !");
      }
      return returnVal;
    }
};

class ConfigFile {
  public:

    // -----------------------------------------------------------------------
    ConfigFile (const std::string &filename, char keysep = '=', char remsep = '#') :
      _keysep (keysep), _remsep (remsep) {

      extractKeys (filename);
    }

    // -----------------------------------------------------------------------
    bool
    keyExists (const std::string &key) const {

      return _contents.find (key) != _contents.end();
    }

    // -----------------------------------------------------------------------
    template<typename ValueType> ValueType value (const std::string &key,
        ValueType const &defaultValue = ValueType()) const {

      if (!keyExists (key)) {

        return defaultValue;
      }

      return Convert::string_to_T<ValueType> (_contents.find (key)->second);
    }

    // -----------------------------------------------------------------------
    std::string value (const std::string &key,
                       std::string const &defaultValue = std::string()) const {
      if (!keyExists (key)) {

        return defaultValue;
      }

      return _contents.find (key)->second;
    }

  private:
    std::map<std::string, std::string> _contents;
    char _keysep;
    char _remsep;

    // -----------------------------------------------------------------------
    void
    removeComment (std::string &line) const {
      size_t pos = line.find (_remsep);
      if (pos != line.npos) {
        line.erase (pos, line.npos);
      }
    }

    // -----------------------------------------------------------------------
    bool
    onlyWhitespace (const std::string &line) const {

      return (line.find_first_not_of (' ') == line.npos);
    }

    // -----------------------------------------------------------------------
    bool
    validLine (const std::string &line) const {
      std::string temp = line;

      temp.erase (0, temp.find_first_not_of ("\t "));

      if (temp[0] == _keysep) {

        return false;
      }

      for (size_t i = temp.find (_keysep) + 1; i < temp.length(); i++)

        if (temp[i] != ' ') {

          return true;
        }

      return false;
    }

    // -----------------------------------------------------------------------
    static void trim (std::string &str) {

      str.erase (0, str.find_first_not_of ("\t \""));
      str.erase (str.find_last_not_of ("\t \"") + 1);
    }

    // -----------------------------------------------------------------------
    void
    extractKey (std::string &key, size_t const &sepPos,
                const std::string &line) const {

      key = line.substr (0, sepPos);
      trim (key);
    }

    // -----------------------------------------------------------------------
    void
    extractValue (std::string &value, size_t const &sepPos,
                  const std::string &line) const {

      value = line.substr (sepPos + 1);
      trim (value);
    }

    // -----------------------------------------------------------------------
    void
    extractContents (const std::string &line) {
      std::string temp = line;

      temp.erase (0, temp.find_first_not_of ("\t "));
      size_t sepPos = temp.find (_keysep);

      std::string key, value;
      extractKey (key, sepPos, temp);
      extractValue (value, sepPos, temp);

      if (keyExists (key)) {  // last replace previous key

        _contents.erase (key);
      }
      _contents.insert (std::pair<std::string, std::string> (key, value));
    }

    // -----------------------------------------------------------------------
    void
    parseLine (const std::string &line, size_t const lineNo) {

      if (line.find (_keysep) != line.npos) {

        if (!validLine (line)) {

          throw std::invalid_argument ("Bad format for line: " +
                                       Convert::T_to_string (lineNo));
        }

        extractContents (line);
      }
    }

    // -----------------------------------------------------------------------
    void
    extractKeys (const std::string &filename) {
      std::ifstream file;
      file.open (filename.c_str());
      if (!file) {

        throw std::invalid_argument ("File " + filename + " couldn't be found!");
      }

      std::string line;
      size_t lineNo = 0;
      while (std::getline (file, line)) {
        lineNo++;
        std::string temp = line;

        if (temp.empty()) {
          continue;
        }

        removeComment (temp);
        if (onlyWhitespace (temp)) {
          continue;
        }

        parseLine (temp, lineNo);
      }

      file.close();
    }
};

/* ========================================================================== */
