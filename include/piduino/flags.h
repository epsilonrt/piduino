/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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
#pragma once


namespace Piduino {

  /**
     @brief A type-safe flags class for bitwise operations on enum values.

     This template class provides a type-safe way to handle bitwise operations
     on enumeration values. It allows combining multiple enum flags using
     bitwise operators while maintaining type safety and preventing accidental
     operations between incompatible types.

     @tparam EnumType The enumeration type that defines the individual flags
     @tparam Underlying The underlying integral type used to store the flags (default: int)
  */
  template <typename EnumType, typename Underlying = int>
  class Flags {
      /// @brief Restricted bool type for safe boolean conversion
      typedef Underlying Flags::*RestrictedBool;

    public:
      /**
         @brief Default constructor that initializes flags with a given value.
         @param original The initial value for the flags (default: 0)
      */
      Flags (Underlying original = 0) : m_flags (original) {}

      /**
         @brief Copy constructor that creates a new Flags object from an existing one.
         @param original The Flags object to copy from
      */
      Flags (const Flags &original) :
        m_flags (original.m_flags)
      {}

      /**
         @brief Bitwise OR assignment operator.
         Combines this flags object with another using bitwise OR operation.
         @param f The flags object to combine with
         @return Reference to this object after the operation
      */
      Flags &operator |= (const Flags &f) {
        m_flags |= f.m_flags;
        return *this;
      }

      /**
         @brief Bitwise XOR assignment operator.
         Combines this flags object with another using bitwise XOR operation.
         @param f The flags object to combine with
         @return Reference to this object after the operation
      */
      Flags &operator ^= (const Flags &f) {
        m_flags ^= f.m_flags;
        return *this;
      }

      /**
         @brief Bitwise AND assignment operator.
         Combines this flags object with another using bitwise AND operation.
         @param f The flags object to combine with
         @return Reference to this object after the operation
      */
      Flags &operator &= (const Flags &f) {
        m_flags &= f.m_flags;
        return *this;
      }

      /**
         @brief Bitwise OR operator for combining two Flags objects.
         @param f1 The first flags object
         @param f2 The second flags object
         @return A new Flags object containing the result of the OR operation
      */
      friend Flags operator | (const Flags &f1, const Flags &f2) {
        return Flags (f1) |= f2;
      }

      /**
         @brief Bitwise XOR operator for combining two Flags objects.
         @param f1 The first flags object
         @param f2 The second flags object
         @return A new Flags object containing the result of the XOR operation
      */
      friend Flags operator ^ (const Flags &f1, const Flags &f2) {
        return Flags (f1) ^= f2;
      }

      /**
         @brief Bitwise AND operator for combining two Flags objects.
         @param f1 The first flags object
         @param f2 The second flags object
         @return A new Flags object containing the result of the AND operation
      */
      friend Flags operator & (const Flags &f1, const Flags &f2) {
        return Flags (f1) &= f2;
      }

      /**
         @brief Bitwise NOT operator that inverts all bits in the flags.
         @return A new Flags object with all bits inverted
      */
      Flags operator ~() const {
        Flags result (*this);
        result.m_flags = ~result.m_flags;
        return result;
      }

      /**
         @brief Safe boolean conversion operator.
         Allows the Flags object to be used in boolean contexts without
         allowing implicit conversion to other types.
         @return A pointer-to-member if flags are non-zero, nullptr otherwise
      */
      operator RestrictedBool() const {
        return m_flags ? &Flags::m_flags : 0;
      }

      /**
         @brief Equality comparison operator.
         @param other The Flags object to compare with
         @return true if both objects have the same flag values, false otherwise
      */
      bool operator== (const Flags &other) {
        return m_flags == other.m_flags;
      }

      /**
         @brief Inequality comparison operator.
         @param other The Flags object to compare with
         @return true if the objects have different flag values, false otherwise
      */
      bool operator!= (const Flags &other) {
        return m_flags != other.m_flags;
      }

      /**
         @brief Sets multiple flags using bitwise OR operation.
         @param flags The flags to set (as underlying type value)
      */
      void setFlags (Underlying flags) {
        m_flags |= flags;
      }

      /**
         @brief Sets a single flag using bitwise OR operation.
         @param singleFlag The individual flag to set (as enum value)
      */
      void setFlag (EnumType singleFlag) {
        setFlags (static_cast<Underlying> (singleFlag));
      }

      /**
         @brief Clears multiple flags using bitwise AND with complement.
         @param flags The flags to clear (as underlying type value)
      */
      void clearFlags (Underlying flags) {
        m_flags &= ~flags;
      }

      /**
         @brief Clears a single flag using bitwise AND with complement.
         @param singleFlag The individual flag to clear (as enum value)
      */
      void clearFlag (EnumType singleFlag) {
        clearFlags (static_cast<Underlying> (singleFlag));
      }

      /**
         @brief Gets the raw underlying value of the flags.
         @return The underlying integral value representing all set flags
      */
      Underlying value() const {
        return m_flags;
      }

    protected:
      /// @brief The underlying storage for the flag bits
      Underlying  m_flags;
  };
}


/* ========================================================================== */
