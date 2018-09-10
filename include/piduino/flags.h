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

  template <typename EnumType, typename Underlying = int>
  class Flags {
      typedef Underlying Flags::* RestrictedBool;

    public:
      Flags(Underlying original = 0) : m_flags (original) {}
      
      Flags (const Flags& original) :
        m_flags (original.m_flags)
      {}

      Flags& operator |= (const Flags& f) {
        m_flags |= f.m_flags;
        return *this;
      }

      Flags& operator ^= (const Flags& f) {
        m_flags ^= f.m_flags;
        return *this;
      }

      Flags& operator &= (const Flags& f) {
        m_flags &= f.m_flags;
        return *this;
      }

      friend Flags operator | (const Flags& f1, const Flags& f2) {
        return Flags (f1) |= f2;
      }
      
      friend Flags operator ^ (const Flags& f1, const Flags& f2) {
        return Flags (f1) ^= f2;
      }

      friend Flags operator & (const Flags& f1, const Flags& f2) {
        return Flags (f1) &= f2;
      }

      Flags operator ~() const {
        Flags result (*this);
        result.m_flags = ~result.m_flags;
        return result;
      }

      operator RestrictedBool() const {
        return m_flags ? &Flags::m_flags : 0;
      }
      
      bool operator== (const Flags & other) {
        return m_flags == other.m_flags;
      }
      
      bool operator!= (const Flags & other) {
        return m_flags != other.m_flags;
      }
      
      void setFlags (Underlying flags) {
        m_flags |= flags;
      }
      
      void setFlag (EnumType singleFlag) {
        setFlags(static_cast<Underlying> (singleFlag));
      }
      
      void clearFlags (Underlying flags) {
        m_flags &= ~flags;
      }
      
      void clearFlag (EnumType singleFlag) {
        clearFlags(static_cast<Underlying> (singleFlag));
      }

      Underlying value() const {
        return m_flags;
      }

    protected:
      Underlying  m_flags;
  };
}

/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_FLAGS_H defined */
