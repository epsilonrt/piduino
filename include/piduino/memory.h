/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
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

#include <memory>

#ifndef __cpp_lib_make_unique
// defined make_unique if < c++14
#include <cstddef>
#include <type_traits>
#include <utility>

namespace std {
  template<class T> struct _Unique_if {
    typedef unique_ptr<T> _Single_object;
  };

  template<class T> struct _Unique_if<T[]> {
    typedef unique_ptr<T[]> _Unknown_bound;
  };

  template<class T, size_t N> struct _Unique_if<T[N]> {
    typedef void _Known_bound;
  };

  template<class T, class... Args>
  typename _Unique_if<T>::_Single_object
  make_unique (Args && ... args) {
    return unique_ptr<T> (new T (std::forward<Args> (args)...));
  }

  template<class T>
  typename _Unique_if<T>::_Unknown_bound
  make_unique (size_t n) {
    typedef typename remove_extent<T>::type U;
    return unique_ptr<T> (new U[n]());
  }

  template<class T, class... Args>
  typename _Unique_if<T>::_Known_bound
  make_unique (Args && ...) = delete;
}
#endif /* __cpp_lib_make_unique not defined */

/* ========================================================================== */
