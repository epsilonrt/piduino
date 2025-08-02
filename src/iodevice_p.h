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

#include <cerrno>
#include <piduino/iodevice.h>

namespace Piduino {

  /**
   * @class IoDevice::Private
   * @brief Private implementation class for IoDevice, encapsulating internal state and operations.
   *
   * This class manages the internal state and error handling for IoDevice objects.
   * It provides methods for opening, closing, and querying the state of the device,
   * as well as managing error codes and messages. This class is intended for internal
   * use only and follows the PIMPL (Pointer to IMPLementation) idiom.
   */
  class IoDevice::Private {

    public:
      /**
       * @brief Constructs a Private implementation for the given IoDevice.
       * @param q Pointer to the associated IoDevice instance.
       */
      Private (IoDevice * q);

      /**
       * @brief Virtual destructor for proper cleanup in derived classes.
       */
      virtual ~Private();
      
      /**
       * @brief Opens the device with the specified mode.
       * @param mode The mode in which to open the device.
       * @return True if the device was successfully opened, false otherwise.
       */
      virtual bool open (OpenMode mode);

      /**
       * @brief Closes the device.
       */
      virtual void close();

      /**
       * @brief Checks if the device is currently open.
       * @return True if the device is open, false otherwise.
       */
      virtual bool isOpen() const;
      
      /**
       * @brief Clears the current error state.
       */
      virtual void clearError() const;

      /**
       * @brief Sets a generic error state.
       */
      virtual void setError () const;

      /**
       * @brief Sets a specific error code.
       * @param error The error code to set.
       */
      virtual void setError (int error) const;

      /**
       * @brief Sets a specific error code and error message.
       * @param error The error code to set.
       * @param errorString The error message to associate with the error.
       */
      virtual void setError (int error, const std::string & errorString) const;
      
      /**
       * @brief Converts an OpenMode value to the corresponding POSIX flags.
       * @param mode The OpenMode to convert.
       * @return The corresponding POSIX flags as an integer.
       */
      static int modeToPosixFlags (OpenMode mode);

      /**
       * @brief Pointer to the associated IoDevice instance (const).
       */
      IoDevice * const q_ptr;

      /**
       * @brief The current open mode of the device.
       */
      OpenMode openMode;

      /**
       * @brief Indicates if the device operates in sequential mode.
       */
      bool isSequential;

      /**
       * @brief The current error code (mutable).
       */
      mutable int error;

      /**
       * @brief The current error message (mutable).
       */
      mutable std::string errorString;

      /**
       * @brief Indicates if debug mode is enabled.
       */
      bool isDebug;

      /**
       * @brief Macro for declaring public interface access in the PIMPL idiom.
       */
      PIMP_DECLARE_PUBLIC (IoDevice)
  };

}

/* ========================================================================== */
