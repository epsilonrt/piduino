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

#include <memory>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstdint>
#include <cstddef>
#include <sys/types.h>

/**
    @defgroup piduino_global Global definitions

    @{
*/

#ifdef DOXYGEN
/**
   @brief PIMP_D

   Provides a d pointer to the private class, use d_func() to access it which is defined in PIMP_DECLARE_PRIVATE.
   If you want to access the private class from a const method, use const_cast as in the following example:
   @code
   void MyClass::foo() const
   {
     PIMP_D(const MyClass);
     d->bar();
   }
   @endcode

   @param Class the API class name
*/
#define PIMP_D(Class)

/**
   @brief PIMP_Q

   Provides a q pointer to the API class, use q_func() to access it which is defined in PIMP_DECLARE_PUBLIC.
   If you want to access the API class from a const method, use const_cast as in the following example:
   @code
   void MyClass::Private::foo() const
   {
     PIMP_Q(const MyClass);
     q->bar();
   }
   @endcode

   @param Class the API class name
*/
#define PIMP_Q(Class)

/**
   @brief PIMP_DECLARE_PRIVATE

   Provides d_func() to access the private class, must be added in the private section of the API class, as in the following example:
   @code
   class MyClass : public PimplClass {
    PIMP_DECLARE_PRIVATE(MyClass)
   public:
    void foo();
   };
   @endcode

   @param Class the API class name
*/
#define PIMP_DECLARE_PRIVATE(Class)

/**
   @brief PIMP_DECLARE_PUBLIC

   Provides q_func() to access the API class, must be added in the public section of the private class, as in the following example:
   @code
   class MyClass::Private : public PimplClass::Private {
    PIMP_DECLARE_PUBLIC(MyClass)
   public:
    void foo();
   };
   @endcode

   @param Class the API class name
*/
#define PIMP_DECLARE_PUBLIC(Class)
#else

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif

#ifdef __FILE_NAME__
#define EXCEPTION_MSG(m) std::string{} + __FILE_NAME__ + "(" + std::to_string(__LINE__) + "): [" + __func__ +"] " + m
#else
#define EXCEPTION_MSG(m) std::string{} + __FILE__ + "(" + std::to_string(__LINE__) + "): [" + __func__ +"] " + m
#endif

namespace Piduino {
  static inline void Assert (const char *expr_str, bool expr, const char *file, int line, const std::string msg) {
    if (!expr) {
      std::cerr << std::endl
                << "---------->>>" << std::endl
                << "Assert failed:\t" << msg << std::endl
                << "Expected:\t" << expr_str << std::endl
                << "Source:\t\t" << file << ":" << line << std::endl
                << "----------<<<" << std::endl;
      abort();
    }
  }
} // namespace Piduino

#ifndef NDEBUG
#   define M_Assert(Expr, Msg) \
  Piduino::Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif

// ---------------------------------------------------------
// PIMP_D
#define PIMP_D(Class) Class::Private * const d = d_func()
#define PIMP_Q(Class) Class * const q = q_func()
#define PIMP_DECLARE_PRIVATE(Class)\
  inline Class::Private* d_func() {\
    return reinterpret_cast<Class::Private*>(d_ptr.get());\
  }\
  inline const Class::Private* d_func() const {\
    return reinterpret_cast<const Class::Private *>(d_ptr.get());\
  }\
  friend class Class::Private;
#define PIMP_DECLARE_PUBLIC(Class) \
  inline Class* q_func() { return reinterpret_cast<Class *>(q_ptr); } \
  inline const Class* q_func() const { return reinterpret_cast<const Class *>(q_ptr); } \
  friend class Class;
// ---------------------------------------------------------
#endif

/**
   @brief Global namespace for Piduino
*/
namespace Piduino {
}

/**
    @}
*/

/* ========================================================================== */
