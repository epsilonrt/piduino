/* Copyright © 2018 Pascal JEAN, All rights reserved.
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

#ifndef PIDUINO_CONFIGFILE_H
#define PIDUINO_CONFIGFILE_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>

/**
    @defgroup piduino_configfile Fichier configuration

    Ce module permet d'accéder à des fichiers de configuration
    @{
*/

namespace Piduino {

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

        return line.find_first_not_of (" \t\r\n") == std::string::npos;
      }

      // -----------------------------------------------------------------------
      bool
      validLine (const std::string &line) const {
        if (line.empty()) return false;
        
        std::string temp = line;
        
        // Supprimer les espaces au début
        size_t start = temp.find_first_not_of ("\t ");
        if (start == std::string::npos) return false;
        
        temp = temp.substr(start);
        
        // Vérifier que la ligne ne commence pas par le séparateur
        if (temp[0] == _keysep) {
            return false;
        }
        
        // Vérifier qu'il y a du contenu après le séparateur
        size_t sepPos = temp.find(_keysep);
        if (sepPos == std::string::npos) return false;
        
        return sepPos + 1 < temp.length();
      }

      // -----------------------------------------------------------------------
      static void trim (std::string &str) {
        // Supprimer les espaces au début
        size_t start = str.find_first_not_of ("\t \"");
        if (start == std::string::npos) {
            str.clear();
            return;
        }
        
        // Supprimer les espaces à la fin
        size_t end = str.find_last_not_of ("\t \"");
        if (end != std::string::npos) {
            str = str.substr(start, end - start + 1);
        }
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
    
        if (sepPos != std::string::npos && sepPos + 1 < line.length()) {
            value = line.substr (sepPos + 1);
            trim (value);
        }
        else {
            value.clear();
        }
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

          if (validLine (line)) {

            extractContents (line);
          }
        }
      }

      // -----------------------------------------------------------------------
      void
      extractKeys (const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::invalid_argument ("File " + filename + " couldn't be found!");
        }

        std::string line;
        size_t lineNo = 0;
        
        while (std::getline (file, line)) {
            lineNo++;
            
            if (line.empty()) {
                continue;
            }

            // Copie une seule fois
            std::string processedLine = line;
            removeComment (processedLine);
            
            if (onlyWhitespace (processedLine)) {
                continue;
            }

            // Traitement direct sans passer par parseLine
            if (processedLine.find(_keysep) != std::string::npos) {
                if (validLine(processedLine)) {
                    extractContents(processedLine);
                }
            }
        }
        // file.close() automatique avec RAII
      }

  };
  #if 0
  // -----------------------------------------------------------------------
  std::string ConfigFile::value (const std::string &key, std::string const &defaultValue) const {

    if (!keyExists (key)) {

      return std::string();
    }
    return _contents.find (key)->second;
  }
  #endif
}
/**
    @}
*/

/* ========================================================================== */
#endif /*PIDUINO_CONFIGFILE_H defined */
