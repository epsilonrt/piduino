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
#include <piduino/string.h>

namespace Piduino {

// -----------------------------------------------------------------------------
  struct iequal {
    bool operator() (int c1, int c2) const {
      return std::toupper (c1) == std::toupper (c2);
    }
  };

// -----------------------------------------------------------------------------
  bool
  String::equalsIgnoreCase (const String &s) const {

    return std::equal (cbegin(), cend(), s.begin(), iequal());
  }

// -----------------------------------------------------------------------------
  void
  String::reverse() {

    std::reverse (this->begin(), this->end());
  }

// -----------------------------------------------------------------------------
// from https://www.geeksforgeeks.org/implement-itoa/
  String
  String::toString (unsigned long num, unsigned char base) {
    String str;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) {

      str.push_back ('0');
      return str;
    }

    // Process individual digits
    while (num != 0) {

      unsigned char rem = num % base;
      str.push_back ( (rem > 9) ? (rem - 10) + 'a' : rem + '0');
      num = num / base;
    }

    // Reverse the string
    str.reverse ();

    return str;
  }

// -----------------------------------------------------------------------------
  String String::toString (long num, unsigned char base) {
    bool isneg = false;
    String str;

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10) {

      isneg = true;
      num = -num;
    }

    str = toString (static_cast<unsigned long> (num), base);

    // If number is negative, append '-'
    if (isneg) {

      str.insert (0, 1, '-');
    }

    return str;
  }

// -----------------------------------------------------------------------------
  void
  String::getBytes (unsigned char *buf, unsigned int bufsize, unsigned int index) const {

    if (!bufsize || !buf) {
      return;
    }
    if (index >= length()) {
      buf[0] = 0;
      return;
    }
    unsigned int n = bufsize - 1;
    if (n > length() - index) {
      n = length() - index;
    }
    strncpy ( (char *) buf, c_str() + index, n);
    buf[n] = 0;
  }

// -----------------------------------------------------------------------------
  void
  String::replace (const String& str1, const String& str2) {
    size_t found;

    while ( (found = find (str1)) != std::string::npos) {

      std::string::replace (found, str1.length(), str2);
    }
  }

  //------------------------------------------------------------------------
  String & String::operator += (unsigned char num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  String & String::operator += (int num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  String & String::operator += (unsigned int num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  String & String::operator += (long num) {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  String & String::operator += (unsigned long num)  {

    concat (num);
    return (*this);
  }
  //------------------------------------------------------------------------
  void String::toLowerCase () {
    std::transform (cbegin(), cend(), begin(),
    [] (unsigned char c) {
      return std::tolower (c);
    }
                   );
  }
  //------------------------------------------------------------------------
  void String::toUpperCase () {

    std::transform (cbegin(), cend(), begin(),
    [] (unsigned char c) {
      return std::toupper (c);
    }
                   );
  }

}
/* ========================================================================== */
