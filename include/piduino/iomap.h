/* Copyright © 2015-2018 Pascal JEAN, All rights reserved.
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

#include <piduino/global.h>

/**
  @defgroup piduino_iomap IO Memory Access

  This module provides functions to access the memory where the input-output couplers are implemented
  @{
*/

namespace Piduino {

  /**
    @class IoMap
    @brief Memory mapping
  */
  class IoMap {
    public:
      /**
        @brief Constructor
      */
      IoMap ();

      /**
        @brief Destructor
      */
      virtual ~IoMap();

      bool open (const char *device, size_t size, off_t base = 0);

      /**
        @brief Open a memory mapping on /dev/mem

        @param base base address of the area to be mapped, must be a multiple of pageSize().
        @param size size of the area to be mapped in bytes
        @return true if opened, false if error
      */
      bool open (off_t base, size_t size);

      /**
        @brief Open a GPIO memory mapping for RP1

        @param gpio_bank GPIO bank (0-4 for /dev/gpiomem0 to /dev/gpiomem4)
        @param size size of the area to be mapped in bytes
        @return true if opened, false if error

        @exception std::system_error if the open the device or memory mapping fails.
      */
      bool openGpioMem (int gpio_bank, size_t size);

      /**
        @brief Close a memory mapping
      */
      void close();

      /**
        @brief Pointer to access registers
        @param offset offset inside the area in sizeof(int)
        @return pointer to the register, NULL if error
      */
      volatile uint32_t *io (size_t offset = 0) const {

        return static_cast<volatile uint32_t *> (_map) + offset;
      }

      /**
        @brief Pointer to access registers
        @param offset offset inside the area in sizeof(int)
        @return pointer to the register, NULL if error
      */
      volatile uint32_t *operator [] (size_t offset) const {

        return io (offset);
      }

      /**
       * @brief Read a register
       * @param offset offset inside the area in sizeof(int)
       * @return the value read
       *
       * This function reads a register at the specified offset and returns its value.
       * It is a convenience method that allows direct access to the memory-mapped registers.
       */
      inline uint32_t read (size_t offset) const {
        return *io (offset);
      }

      /**
        @brief Write a value to a register
        @param offset offset inside the area in sizeof(int)
        @param value value to write
      */
      inline void write (size_t offset, uint32_t value) {
        *io (offset) = value;
      }

      /**
        @brief Atomic read of a register
        @param offset offset inside the area in sizeof(int)
        @return the value read

        This function reads a register atomically, ensuring that the read operation
        is not interrupted by other threads or processes.
      */
      inline uint32_t atomicRead (size_t offset) const {
        volatile uint32_t *reg = io (offset);
        return __atomic_load_n (reg, __ATOMIC_SEQ_CST);
      }

      /**
        @brief Atomic write to a register
        @param offset offset inside the area in sizeof(int)
        @param value value to write

        This function writes a value to a register atomically, ensuring that the write operation
        is not interrupted by other threads or processes.
        It uses C11 atomic operations to ensure memory consistency.
        @note This function is useful for ensuring that hardware registers are updated safely in a multi-threaded environment.
      */
      inline void atomicWrite (size_t offset, uint32_t value) {
        volatile uint32_t *reg = io (offset);
        __atomic_store_n (reg, value, __ATOMIC_SEQ_CST);
      }

      /**
        @brief Indicates if a memory mapping is open
      */
      inline bool isOpen() const {

        return _fd >= 0;
      }

      /**
        @brief Base address of the mapped area
      */
      inline off_t base() const {
        return _base;
      }

      /**
        @brief Size of the mapped area
      */
      inline size_t size() const {
        return _size;
      }

      /**
        @brief Size of a mapping page
      */
      static size_t pageSize();

    private:
      off_t _base;  /*< base address of the area */
      size_t _size; /*< size of the area */
      int _fd;      /*< file descriptor for memory area */
      void *_map;   /*< pointer to the area */
  };
}
/**
   @}
*/

/* ========================================================================== */
