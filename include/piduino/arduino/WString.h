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

#ifndef __WSTRING_H__
#define __WSTRING_H__

#include <piduino/arduino.h>

#ifdef __cplusplus
#include <string>
#include <algorithm>

typedef char __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(string_literal))

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
    //--------------------------------------------------------------------------
    explicit String (char c) : String() {

      std::string::push_back (c);
    }

    // Capacity

    //--------------------------------------------------------------------------
    unsigned char reserve (unsigned int size) {

      std::string::reserve (size);
      return true;
    }

    // Modifiers

    void reverse();
    void replace (const String& str1, const String& str2);

    //--------------------------------------------------------------------------
    void replace (char c1, char c2) {

      std::replace (begin(), end(), c1, c2);
    }
    //--------------------------------------------------------------------------
    unsigned char concat (const String &str) {

      std::string::append (str);
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (const char *cstr) {

      std::string::append (cstr);
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (char c) {

      std::string::push_back (c);
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (unsigned char num) {

      std::string::append (toString (num));
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (int num) {

      std::string::append (toString (num));
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (unsigned int num) {

      std::string::append (toString (num));
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (long num) {

      std::string::append (toString (num));
      return true;
    }
    //--------------------------------------------------------------------------
    unsigned char concat (unsigned long num) {

      std::string::append (toString (num));
      return true;
    }
    //--------------------------------------------------------------------------
    String & operator += (unsigned char num) {

      concat (num);
      return (*this);
    }
    //--------------------------------------------------------------------------
    String & operator += (int num) {

      concat (num);
      return (*this);
    }
    //--------------------------------------------------------------------------
    String & operator += (unsigned int num) {

      concat (num);
      return (*this);
    }
    //--------------------------------------------------------------------------
    String & operator += (long num) {

      concat (num);
      return (*this);
    }
    //--------------------------------------------------------------------------
    String & operator += (unsigned long num)  {

      concat (num);
      return (*this);
    }
    //--------------------------------------------------------------------------
    void toLowerCase () {
      std::transform (cbegin(), cend(), begin(),
      [] (unsigned char c) {
        return std::tolower (c);
      }
                     );
    }
    //--------------------------------------------------------------------------
    void toUpperCase () {

      std::transform (cbegin(), cend(), begin(),
      [] (unsigned char c) {
        return std::toupper (c);
      }
                     );
    }
    //--------------------------------------------------------------------------
    void ltrim () {

      std::string::erase (0, find_first_not_of (std::string ("\t\n\v\f\r ")));
    }
    //--------------------------------------------------------------------------
    void rtrim () {

      std::string::erase (find_last_not_of (std::string ("\t\n\v\f\r ")) + 1);
    }
    //--------------------------------------------------------------------------
    void trim () {

      ltrim ();
      rtrim();
    }

    // Relationnals/Conditionnals

    unsigned char equalsIgnoreCase (const String &s) const;

    //--------------------------------------------------------------------------
    int compareTo (const String &s) const {

      return std::string::compare (s);
    }
    //--------------------------------------------------------------------------
    unsigned char equals (const String &s) const {

      return s == (*this);
    }
    //--------------------------------------------------------------------------
    unsigned char equals (const char *cstr) const {

      return cstr == (*this);
    }
    //--------------------------------------------------------------------------
    unsigned char startsWith (const String &prefix, unsigned int offset = 0) const {

      return std::string::find (prefix, offset) == 0;
    }
    //--------------------------------------------------------------------------
    unsigned char endsWith (const String &suffix) const {

      return std::string::rfind (suffix) == (length() - suffix.length());
    }

    // Element access

    void getBytes (unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;

    //--------------------------------------------------------------------------
    char charAt (unsigned int index) const {

      return at (index);
    }
    //--------------------------------------------------------------------------
    void setCharAt (unsigned int index, char c) {

      std::string::replace (index, 1, &c);
    }
    //--------------------------------------------------------------------------
    void toCharArray (char *buf, unsigned int bufsize, unsigned int index = 0) const {

      getBytes ( (unsigned char *) buf, bufsize, index);
    }
    //--------------------------------------------------------------------------
    int indexOf (char ch, unsigned int fromIndex = 0) const {

      return std::string::find (ch, fromIndex);
    }
    //--------------------------------------------------------------------------
    int indexOf (const String &str, unsigned int fromIndex = 0) const {

      return std::string::find (str, fromIndex);
    }
    //--------------------------------------------------------------------------
    int lastIndexOf (char ch, unsigned int fromIndex = 0) const {

      return std::string::rfind (ch, fromIndex);
    }
    //--------------------------------------------------------------------------
    int lastIndexOf (const String &str, unsigned int fromIndex = 0) const {

      return std::string::rfind (str, fromIndex);
    }
    //--------------------------------------------------------------------------
    String substring (unsigned int beginIndex) const {

      return substr (beginIndex);
    }
    //--------------------------------------------------------------------------
    String substring (unsigned int beginIndex, unsigned int endIndex) const {

      return substr (beginIndex, endIndex);
    }

    //--------------------------------------------------------------------------
    long toInt () const {

      return stoi (*this);
    }

    static String toString (long num, unsigned char base = 10);
    static String toString (unsigned long num, unsigned char base = 10);

    //--------------------------------------------------------------------------
    static String toString (int num, unsigned char base = 10) {

      return toString (static_cast<long> (num), base);
    }
    //--------------------------------------------------------------------------
    static String toString (short num, unsigned char base = 10) {

      return toString (static_cast<long> (num), base);
    }
    //--------------------------------------------------------------------------
    static String toString (unsigned int num, unsigned char base = 10) {

      return toString (static_cast<unsigned long> (num), base);
    }
    //--------------------------------------------------------------------------
    static String toString (unsigned short num, unsigned char base = 10) {

      return toString (static_cast<unsigned long> (num), base);
    }
    //--------------------------------------------------------------------------
    static String toString (unsigned char num, unsigned char base = 10) {

      return toString (static_cast<unsigned long> (num), base);
    }
};


#endif /* __cplusplus defined */
/* ========================================================================== */
#endif /*__WSTRING_H__ defined */
