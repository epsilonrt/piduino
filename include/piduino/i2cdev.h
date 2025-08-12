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

#include <deque>
#include <string>
#include <map>
#include <memory>
#include <piduino/system.h>
#include <piduino/iodevice.h>

namespace Piduino {

  /**
     @brief I2C device interface for communicating with I2C peripheral devices.

     This class provides a high-level interface for I2C communication on Linux systems.
     It supports master mode operations including reading from and writing to I2C slave devices.
     The class handles I2C bus management, transaction control, and data buffering.
  */
  class I2cDev : public IoDevice {
    public:

      /**
         @brief Information class for I2C bus configuration and management.

         This nested class encapsulates I2C bus information including bus ID, device path,
         and provides utility methods for bus discovery and validation.
      */
      class Info {
        public:
          /// @brief Maximum number of I2C buses supported by the system
          static const int MaxBuses = 32;

          /**
             @brief Constructs an Info object with the specified bus ID.
             @param id The I2C bus ID (default: 0)
          */
          Info (int id = 0);

          /**
             @brief Gets the current I2C bus ID.
             @return The bus ID as an integer
          */
          inline int id() const;

          /**
             @brief Sets the I2C bus ID and updates the corresponding device path.
             @param id The new bus ID to set
          */
          inline void setId (int id);

          /**
             @brief Gets the device path for this I2C bus.
             @return A reference to the device path string (e.g., "/dev/i2c-0")
          */
          inline const std::string &path() const;

          /**
             @brief Sets the device path and extracts the corresponding bus ID.
             @param path The device path to set
             @return true if the path is valid and the ID was successfully extracted, false otherwise
          */
          bool setPath (const std::string &path);

          /**
             @brief Checks if the I2C bus device file exists on the system.
             @return true if the device file exists and is accessible, false otherwise
          */
          inline bool exists() const;

          /**
             @brief Equality comparison operator.
             @param other The Info object to compare with
             @return true if both objects represent the same I2C bus path, false otherwise
          */
          bool operator== (const Info &other);

          /**
             @brief Inequality comparison operator.
             @param other The Info object to compare with
             @return true if the objects represent different I2C bus paths, false otherwise
          */
          bool operator!= (const Info &other);

          /**
             @brief Generates the device path for a given I2C bus ID.
             @param id The bus ID
             @return The corresponding device path string (e.g., "/dev/i2c-0" for ID 0)
          */
          static std::string busPath (int id);

          /**
             @brief Gets information about the default I2C bus on the system.
             @return An Info object representing the default I2C bus
          */
          static Info defaultBus ();

          /**
             @brief Returns a list of available I2C buses on the system.
             @return A deque containing Info objects for all detected I2C buses
          */
          static std::deque<Info> availableBuses ();

        private:
          /// @brief The I2C bus ID
          int _id;
          /// @brief The device path (e.g., "/dev/i2c-0")
          std::string _path;
      };

      /**
         @brief Default constructor that creates an I2cDev object without specifying a bus.
         The bus must be set later using setBus() before opening the device.
      */
      I2cDev ();

      /**
         @brief Constructs an I2cDev object for the specified I2C bus.
         @param bus The I2C bus information
      */
      I2cDev (const Info &bus);

      /**
         @brief Constructs an I2cDev object using a device path.
         @param path The device path (e.g., "/dev/i2c-0")
      */
      explicit I2cDev (const char *path);

      /**
         @brief Constructs an I2cDev object using a device path.
         @param path The device path as a string
      */
      explicit I2cDev (const std::string &path);

      /**
         @brief Constructs an I2cDev object for the specified bus ID.
         @param idBus The I2C bus ID
      */
      explicit I2cDev (int idBus);

      /**
         @brief Virtual destructor that properly cleans up the I2C device.
      */
      virtual ~I2cDev();

      /**
         @brief Factory method that creates a shared pointer to an I2cDev instance.

         This method is used to create an I2cDev object for a specific I2C bus ID and shares the instance
         across the application. It ensures that only one instance per bus ID exists.

         @param busId The I2C bus ID
         @return A shared pointer to the created I2cDev object
      */
      static std::shared_ptr<I2cDev> factory (int busId);

      /**
         @brief Opens the I2C device for communication.
         @param mode The opening mode (default: ReadWrite)
         @return true if the device was successfully opened, false otherwise
      */
      virtual bool open (OpenMode mode = IoDevice::ReadWrite);

      /**
         @brief Closes the I2C device and releases system resources.
      */
      virtual void close();

      /**
         @brief Sets the I2C bus using an Info object.
         @param bus The I2C bus information
      */
      void setBus (const Info &bus);

      /**
         @brief Sets the I2C bus using a bus ID.
         @param idBus The I2C bus ID
      */
      void setBus (int idBus);

      /**
         @brief Gets the current I2C bus information.
         @return A reference to the current bus Info object
      */
      const Info &bus() const;

      /**
         @brief Sets the I2C bus using a device path.
         @param path The device path (e.g., "/dev/i2c-0")
      */
      void setBusPath (const char *path);

      /**
         @brief Sets the I2C bus using a device path.
         @param path The device path as a string
      */
      void setBusPath (const std::string &path);

      /**
         @brief Begins an I2C transmission to the specified slave device.
         This must be called before writing data to a slave device.
         @param slave The 7-bit I2C slave address
      */
      void beginTransmission (uint16_t slave);

      /**
         @brief Ends the current I2C transmission and sends the data.
         @param stop Whether to send a stop condition (default: true)
         @return true if the transmission was successful, false otherwise
      */
      bool endTransmission (bool stop = true);

      /**
         @brief Writes a single byte to the current I2C transaction buffer.
         @param data The byte to write
         @return 1 if successful, 0 if the buffer is full
      */
      virtual int write (uint8_t data);

      /**
         @brief Writes multiple bytes to the current I2C transaction buffer.
         @param buffer Pointer to the data buffer
         @param len Number of bytes to write
         @return The number of bytes successfully written
      */
      virtual int write (const uint8_t *buffer, uint16_t len);

      /**
         @brief Writes a string to the current I2C transaction buffer.
         @param str The string to write
         @return The number of bytes successfully written
      */
      inline int write (std::string str);

      /**
         @brief Writes a C-style string to the current I2C transaction buffer.
         @param str The null-terminated string to write
         @return The number of bytes successfully written
      */
      inline int write (const char *str);

      /**
         @brief Writes an integer value (cast to uint8_t) to the current I2C transaction buffer.
         @param data The integer value to write
         @return 1 if successful, 0 if the buffer is full
      */
      inline int write (int data);

      /**
         @brief Writes an unsigned integer value (cast to uint8_t) to the current I2C transaction buffer.
         @param data The unsigned integer value to write
         @return 1 if successful, 0 if the buffer is full
      */
      inline int write (unsigned int data);

      /**
         @brief Writes a long value (cast to uint8_t) to the current I2C transaction buffer.
         @param data The long value to write
         @return 1 if successful, 0 if the buffer is full
      */
      inline int write (long data);

      /**
         @brief Writes an unsigned long value (cast to uint8_t) to the current I2C transaction buffer.
         @param data The unsigned long value to write
         @return 1 if successful, 0 if the buffer is full
      */
      inline int write (unsigned long data);

      /**
         @brief Requests data from an I2C slave device and stores it in the read buffer.
         @param slave The 7-bit I2C slave address
         @param max Maximum number of bytes to request
         @param stop Whether to send a stop condition after the transaction (default: true)
         @return The number of bytes actually received, or -1 on error
      */
      int requestFrom (uint16_t slave, uint16_t max, bool stop = true);

      /**
         @brief Requests data from an I2C slave device (convenience overload with int parameters).
         @param slave The I2C slave address
         @param max Maximum number of bytes to request
         @param stop Whether to send a stop condition (1 = true, 0 = false, default: 1)
         @return The number of bytes actually received, or -1 on error
      */
      inline int requestFrom (int slave, int max, int stop = 1);

      /**
         @brief Gets the number of bytes available for reading from the internal buffer.
         @return The number of bytes available
      */
      uint16_t available() const;

      /**
         @brief Reads data from the internal buffer into the provided buffer.
         @param buffer Pointer to the destination buffer
         @param max Maximum number of bytes to read
         @return The number of bytes actually read
      */
      virtual int read (uint8_t *buffer, uint16_t max);

      /**
         @brief Reads a single byte from the internal buffer.
         @return The byte read, or -1 if no data is available
      */
      virtual int read();

      /**
         @brief Peeks at the next byte in the buffer without removing it.
         @return The next byte, or -1 if no data is available
      */
      virtual int peek() const;

      /**
         @brief Flushes any pending data in the internal buffers.
         This ensures all data is sent before returning.
      */
      virtual void flush ();

    protected:
      /// @brief Forward declaration of the private implementation class
      class Private;

      /**
         @brief Protected constructor for derived classes using the PIMPL pattern.
         @param dd Reference to the private implementation
      */
      I2cDev (Private &dd);

    private:
      /// @brief PIMPL pattern macro for private implementation
      PIMP_DECLARE_PRIVATE (I2cDev)
  };

  // ---------------------------------------------------------------------------
  //
  //                      Piduino I2cDev Global Object
  //
  // ---------------------------------------------------------------------------

  /// @brief Global I2cDev object for convenient access to the default I2C bus
  extern I2cDev Wire;

  #ifndef DOXYGEN
  // ===========================================================================
  //
  //                          Inline Function Definitions
  //
  // ===========================================================================

  // ---------------------------------------------------------------------------
  //                             I2cDev::Info
  // ---------------------------------------------------------------------------

  inline I2cDev::Info::Info (int id) {
    setId (id);
  }

  inline int I2cDev::Info::id() const {
    return _id;
  }

  inline void I2cDev::Info::setId (int id) {
    _id = id;
    _path = busPath (id);
  }

  inline const std::string &I2cDev::Info::path() const {
    return _path;
  }

  inline bool I2cDev::Info::exists() const {
    return System::charFileExists (_path);
  }

  inline bool I2cDev::Info::operator== (const Info &other) {
    return (_path == other._path) ;
  }

  inline bool I2cDev::Info::operator!= (const Info &other) {
    return (_path != other._path) ;
  }

  // ---------------------------------------------------------------------------
  //                               I2cDev
  // ---------------------------------------------------------------------------

  inline int I2cDev::write (std::string str) {
    return write (reinterpret_cast<const uint8_t *> (str.c_str()), str.length());
  }

  inline int I2cDev::write (const char *str) {
    return write (std::string (str));
  }

  inline int I2cDev::write (int data) {
    return write (static_cast<uint8_t> (data));
  }

  inline int I2cDev::write (unsigned int data) {
    return write (static_cast<uint8_t> (data));
  }

  inline int I2cDev::write (long data) {
    return write (static_cast<uint8_t> (data));
  }

  inline int I2cDev::write (unsigned long data) {
    return write (static_cast<uint8_t> (data));
  }

  inline int I2cDev::requestFrom (int slave, int max, int stop) {
    return requestFrom (static_cast<uint16_t> (slave), static_cast<uint16_t> (max), stop != 0);
  }

  #endif // DOXYGEN

}

/* ========================================================================== */
