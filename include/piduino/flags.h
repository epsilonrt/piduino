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

#ifndef PIDUINO_FLAGS_H
#define PIDUINO_FLAGS_H

/**
 *  @defgroup piduino_flags Flags
 *
 *  Ce module permet la gestion des flags avec des énumérations.
 *  @{
 */

namespace Piduino {

  template<typename Flags>
  struct EnableFlagsOperators {
    static const bool enable = false;
  };

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator | (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    return static_cast<Flags> (
             static_cast<underlying> (lhs) |
             static_cast<underlying> (rhs)
           );
  }

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator & (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    return static_cast<Flags> (
             static_cast<underlying> (lhs) &
             static_cast<underlying> (rhs)
           );
  }

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator ^ (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    return static_cast<Flags> (
             static_cast<underlying> (lhs) ^
             static_cast<underlying> (rhs)
           );
  }

  template<typename Flags>
  bool
  operator == (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    return (
             static_cast<underlying> (lhs) ==
             static_cast<underlying> (rhs)
           );
  }

  template<typename Flags>
  bool
  operator != (Flags lhs, Flags rhs) {

    return ! (lhs == rhs);
  }

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator |= (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    lhs = static_cast<Flags> (
            static_cast<underlying> (lhs) |
            static_cast<underlying> (rhs)
          );
    return lhs;
  }

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator &= (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    lhs = static_cast<Flags> (
            static_cast<underlying> (lhs) &
            static_cast<underlying> (rhs)
          );
    return lhs;
  }

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator ^= (Flags lhs, Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    lhs = static_cast<Flags> (
            static_cast<underlying> (lhs) ^
            static_cast<underlying> (rhs)
          );
    return lhs;
  }

  template<typename Flags>
  typename std::enable_if<EnableFlagsOperators<Flags>::enable, Flags>::type
  operator ~ (Flags rhs) {
    using underlying = typename std::underlying_type<Flags>::type;
    return static_cast<Flags> (
             ~static_cast<underlying> (rhs)
           );
  }
  
#define ENABLE_FLAGS_OPERATORS(T)     \
  template<>                          \
  struct EnableFlagsOperators<T>      \
  {                                   \
    static const bool enable = true;  \
  }
}


/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_FLAGS_H defined */
