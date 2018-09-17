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

#ifndef PIDUINO_STRING_H
#define PIDUINO_STRING_H

#include <cstring>

#ifdef __cplusplus
#include <string>
#include <algorithm>

/**
 *  @defgroup piduino_string String
 *
 *  Ce module permet la gestion des chaînes de caractères.
 *  @{
 */
namespace Piduino {

  class String : public std::string {

    public:
      // Constructors
      String() : std::string() {}
      String (const std::string &s) : std::string (s) {}
      explicit String (unsigned char n, unsigned char base = 10) : String (toString (n, base)) {}
      explicit String (int n, unsigned char base = 10) : String (toString (n, base)) {}
      explicit String (unsigned int n, unsigned char base = 10) : String (toString (n, base)) {}
      explicit String (long n, unsigned char base = 10) : String (toString (n, base)) {}
      explicit String (unsigned long n, unsigned char base = 10) : String (toString (n, base)) {}
      //------------------------------------------------------------------------
      explicit String (char c) : String() {

        std::string::push_back (c);
      }

      // Capacity
      inline void reserve (unsigned int size);

      // Modifiers

      void reverse();
      void replace (const String& str1, const String& str2);
      inline void replace (char c1, char c2);
      void toLowerCase ();
      void toUpperCase ();
      
      inline String & operator += (const String& str);
      inline String & operator += (const char * str);
      inline String & operator += (char c);
      inline String & operator += (unsigned char num);
      inline String & operator += (int num);
      inline String & operator += (unsigned int num);
      inline String & operator += (long num);
      inline String & operator += (unsigned long num);

      inline void concat (const String &str);
      inline void concat (const char *cstr);
      inline void concat (char c);
      inline void concat (unsigned char num);
      inline void concat (int num);
      inline void concat (unsigned int num);
      inline void concat (long num);
      inline void concat (unsigned long num);
      inline void ltrim ();
      inline void rtrim ();
      inline void trim ();

      // Relationnals/Conditionnals

      bool equalsIgnoreCase (const String &s) const;
      inline int compareTo (const String &s) const;
      inline bool equals (const String &s) const;
      inline bool equals (const char *cstr) const;
      inline bool startsWith (const String &prefix, unsigned int offset = 0) const;
      inline bool startsWith (const std::string &prefix, unsigned int offset = 0) const;
      inline bool startsWith (char prefix, unsigned int offset = 0) const;
      inline bool startsWith (const char * prefix, unsigned int offset = 0) const;
      inline bool endsWith (const String &suffix) const;
      inline bool endsWith (const std::string &suffix) const;
      inline bool endsWith (char suffix) const;
      inline bool endsWith (const char * suffix) const;

      // Element access

      void getBytes (unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;
      inline char charAt (unsigned int index) const;
      inline void setCharAt (unsigned int index, char c);
      inline void toCharArray (char *buf, unsigned int bufsize, unsigned int index = 0) const;
      inline int indexOf (char ch, unsigned int fromIndex = 0) const;
      inline int indexOf (const String &str, unsigned int fromIndex = 0) const;
      inline int lastIndexOf (char ch, unsigned int fromIndex = 0) const;
      inline int lastIndexOf (const String &str, unsigned int fromIndex = 0) const;
      inline String substring (unsigned int beginIndex) const;
      inline String substring (unsigned int beginIndex, unsigned int endIndex) const;
      inline long toInt () const;

      static String toString (long num, unsigned char base = 10);
      static String toString (unsigned long num, unsigned char base = 10);
      static inline String toString (int num, unsigned char base = 10);
      static inline String toString (short num, unsigned char base = 10);
      static inline String toString (unsigned int num, unsigned char base = 10);
      static inline String toString (unsigned short num, unsigned char base = 10);
      static inline String toString (unsigned char num, unsigned char base = 10);
  };
  
  //------------------------------------------------------------------------
  inline String & String::operator += (const String& str) {

    concat(str);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (const char * str) {

    concat(str);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (char c) {

    concat(c);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (unsigned char num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (int num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (unsigned int num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (long num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline String & String::operator += (unsigned long num)  {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  inline void String::reserve (unsigned int size) {

    std::string::reserve (size);
  }
  //------------------------------------------------------------------------
  inline void String::replace (char c1, char c2) {

    std::replace (begin(), end(), c1, c2);
  }
  //------------------------------------------------------------------------
  inline void String::concat (const String &str) {

    std::string::append (str);
  }
  //------------------------------------------------------------------------
  inline void String::concat (const char *cstr) {

    std::string::append (cstr);
  }
  //------------------------------------------------------------------------
  inline void String::concat (char c) {

    std::string::push_back (c);
  }
  //------------------------------------------------------------------------
  inline void String::concat (unsigned char num) {

    std::string::append (toString (num));
  }
  //------------------------------------------------------------------------
  inline void String::concat (int num) {

    std::string::append (toString (num));
  }
  //------------------------------------------------------------------------
  inline void String::concat (unsigned int num) {

    std::string::append (toString (num));
  }
  //------------------------------------------------------------------------
  inline void String::concat (long num) {

    std::string::append (toString (num));
  }
  //------------------------------------------------------------------------
  inline void String::concat (unsigned long num) {

    std::string::append (toString (num));
  }
  //------------------------------------------------------------------------
  inline void String::ltrim () {

    std::string::erase (0, find_first_not_of (std::string ("\t\n\v\f\r ")));
  }
  //------------------------------------------------------------------------
  inline void String::rtrim () {

    std::string::erase (find_last_not_of (std::string ("\t\n\v\f\r ")) + 1);
  }
  //------------------------------------------------------------------------
  inline void String::trim () {

    ltrim ();
    rtrim();
  }
  //------------------------------------------------------------------------
  inline int String::compareTo (const String &s) const {

    return std::string::compare (s);
  }
  //------------------------------------------------------------------------
  inline bool String::equals (const String &s) const {

    return reinterpret_cast<const std::string &> (s) == reinterpret_cast<const std::string &> (*this);
  }
  //------------------------------------------------------------------------
  inline bool String::equals (const char *cstr) const {

    return cstr == (*this);
  }
  //------------------------------------------------------------------------
  inline bool String::startsWith (const String &prefix, unsigned int offset) const {

    return std::string::find (prefix, offset) == 0;
  }
  //------------------------------------------------------------------------
  inline bool String::startsWith (const std::string &prefix, unsigned int offset) const {

    return std::string::find (prefix, offset) == 0;
  }

  //------------------------------------------------------------------------
  inline bool String::startsWith (char prefix, unsigned int offset) const {

    return std::string::find (prefix, offset) == 0;
  }

  //------------------------------------------------------------------------
  inline bool String::startsWith (const char * prefix, unsigned int offset) const  {

    return std::string::find (prefix, offset) == 0;
  }

  //------------------------------------------------------------------------
  inline bool String::endsWith (const String &suffix) const {

    return std::string::rfind (suffix) == (length() - suffix.length());
  }

  //------------------------------------------------------------------------
  inline bool String::endsWith (const std::string &suffix) const {

    return std::string::rfind (suffix) == (length() - suffix.length());
  }

  //------------------------------------------------------------------------
  inline bool String::endsWith (char suffix) const {

    return std::string::rfind (suffix) == (length() - 1);
  }

  //------------------------------------------------------------------------
  inline bool String::endsWith (const char * suffix) const {

    return std::string::rfind (suffix) == (length() - strlen (suffix));
  }


  //------------------------------------------------------------------------
  inline char String::charAt (unsigned int index) const {

    return at (index);
  }

  //------------------------------------------------------------------------
  inline void String::setCharAt (unsigned int index, char c) {

    std::string::replace (index, 1, &c);
  }

  //------------------------------------------------------------------------
  inline void String::toCharArray (char *buf, unsigned int bufsize, unsigned int index) const {

    getBytes ( (unsigned char *) buf, bufsize, index);
  }

  //------------------------------------------------------------------------
  inline int String::indexOf (char ch, unsigned int fromIndex) const {

    return std::string::find (ch, fromIndex);
  }

  //------------------------------------------------------------------------
  inline int String::indexOf (const String &str, unsigned int fromIndex) const {

    return std::string::find (str, fromIndex);
  }

  //------------------------------------------------------------------------
  inline int String::lastIndexOf (char ch, unsigned int fromIndex) const {

    return std::string::rfind (ch, fromIndex);
  }

  //------------------------------------------------------------------------
  inline int String::lastIndexOf (const String &str, unsigned int fromIndex) const {

    return std::string::rfind (str, fromIndex);
  }

  //------------------------------------------------------------------------
  inline String String::substring (unsigned int beginIndex) const {

    return substr (beginIndex);
  }

  //------------------------------------------------------------------------
  inline String String::substring (unsigned int beginIndex, unsigned int endIndex) const {

    return substr (beginIndex, endIndex);
  }

  //------------------------------------------------------------------------
  inline long String::toInt () const {

    return stoi (*this);
  }

  //------------------------------------------------------------------------
  inline String String::toString (int num, unsigned char base) {

    return toString (static_cast<long> (num), base);
  }

  //------------------------------------------------------------------------
  inline String String::toString (short num, unsigned char base) {

    return toString (static_cast<long> (num), base);
  }

  //------------------------------------------------------------------------
  inline String String::toString (unsigned int num, unsigned char base) {

    return toString (static_cast<unsigned long> (num), base);
  }

  //------------------------------------------------------------------------
  inline String String::toString (unsigned short num, unsigned char base) {

    return toString (static_cast<unsigned long> (num), base);
  }

  //------------------------------------------------------------------------
  inline String String::toString (unsigned char num, unsigned char base) {

    return toString (static_cast<unsigned long> (num), base);
  }
}
#endif /* __cplusplus defined */

/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_STRING_H defined */
