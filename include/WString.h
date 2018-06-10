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

#ifdef __cplusplus
#include <string>
#include <algorithm>

class String : public std::string {
  public:
    static String toString (long num, unsigned char base = 10);
    static String toString (unsigned long num, unsigned char base = 10);
    static String toString (int num, unsigned char base = 10) {
      return toString (static_cast<long> (num), base);
    }
    static String toString (short num, unsigned char base = 10) {
      return toString (static_cast<long> (num), base);
    }
    static String toString (unsigned int num, unsigned char base = 10) {
      return toString (static_cast<unsigned long> (num), base);
    }
    static String toString (unsigned short num, unsigned char base = 10) {
      return toString (static_cast<unsigned long> (num), base);
    }
    static String toString (unsigned char num, unsigned char base = 10) {
      return toString (static_cast<unsigned long> (num), base);
    }

    String() : std::string() {}
    String (const std::string &s) : std::string (s) {}
    explicit String (char c) : String() {
      push_back (c);
    }
    explicit String (unsigned char n, unsigned char base = 10) : String (toString (n, base)) {}
    explicit String (int n, unsigned char base = 10) : String (toString (n, base)) {}
    explicit String (unsigned int n, unsigned char base = 10) : String (toString (n, base)) {}
    explicit String (long n, unsigned char base = 10) : String (toString (n, base)) {}
    explicit String (unsigned long n, unsigned char base = 10) : String (toString (n, base)) {}

    unsigned char concat (const String &str) {
      append (str);
      return true;
    }
    unsigned char concat (const char *cstr) {

      append (cstr);
      return true;
    }
    unsigned char concat (char c) {
      push_back (c);
    }
    unsigned char concat (unsigned char num) {

      append (toString (num));
      return true;
    }
    unsigned char concat (int num) {

      append (toString (num));
      return true;
    }
    unsigned char concat (unsigned int num) {

      append (toString (num));
      return true;
    }
    unsigned char concat (long num) {

      append (toString (num));
      return true;
    }
    unsigned char concat (unsigned long num) {

      append (toString (num));
      return true;
    }

    unsigned char equalsIgnoreCase (const String &s) const;
    int compareTo (const String &s) const {
      return compare (s);
    }

    unsigned char equals (const String &s) const {
      return compare (s) == 0;
    }
    unsigned char equals (const char *cstr) const {
      return compare (cstr) == 0;
    }
    unsigned char startsWith (const String &prefix, unsigned int offset = 0) const {
      return find (prefix, offset) == 0;
    }
    unsigned char endsWith (const String &suffix) const {
      return rfind (suffix) == (length() - suffix.length());
    }
    char charAt (unsigned int index) const {
      return at (index);
    }
    void setCharAt (unsigned int index, char c) {
      std::string::replace (index, 1, &c);
    }

    void getBytes (unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;

    void toCharArray (char *buf, unsigned int bufsize, unsigned int index = 0) const {
      getBytes ( (unsigned char *) buf, bufsize, index);
    }
    int indexOf (char ch, unsigned int fromIndex = 0) const {
      return find (ch, fromIndex);
    }
    int indexOf (const String &str, unsigned int fromIndex = 0) const {
      return find (str, fromIndex);
    }
    int lastIndexOf (char ch, unsigned int fromIndex = 0) const {
      return rfind (ch, fromIndex);
    }
    int lastIndexOf (const String &str, unsigned int fromIndex = 0) const {
      return rfind (str, fromIndex);
    }

    String substring (unsigned int beginIndex) const {

      return substr (beginIndex);
    }
    String substring (unsigned int beginIndex, unsigned int endIndex) const {

      return substr (beginIndex, endIndex);
    }

    void replace (char find, char by) {

      std::replace (begin(), end(), find, by);
    }
    void replace (const String& find, const String& replace) {

    }
    void toLowerCase () {
      std::transform (cbegin(), cend(), begin(),
      [] (unsigned char c) {
        return std::tolower (c);
      }
                     );
    }
    void toUpperCase () {

      std::transform (cbegin(), cend(), begin(),
      [] (unsigned char c) {
        return std::toupper (c);
      }
                     );
    }
    void ltrim () {
      erase (0, find_first_not_of (std::string("\t\n\v\f\r ")));
    }

    void rtrim () {
      erase (find_last_not_of (std::string("\t\n\v\f\r ")) + 1);
    }

    void trim () {
      ltrim ();
      rtrim();
    }
    
    void reverse();

    // parsing/conversion
    long toInt () const {
      return stoi (*this);
    }


    unsigned char reserve (unsigned int size) {
      std::string::reserve (size);
      return true;
    }
    String & operator += (unsigned char num)    {
      concat (num);
      return (*this);
    }
    String & operator += (int num)      {
      concat (num);
      return (*this);
    }
    String & operator += (unsigned int num)   {
      concat (num);
      return (*this);
    }
    String & operator += (long num)     {
      concat (num);
      return (*this);
    }
    String & operator += (unsigned long num)  {
      concat (num);
      return (*this);
    }

};

#endif /* __cplusplus defined */
/* ========================================================================== */
#endif /*__WSTRING_H__ defined */
