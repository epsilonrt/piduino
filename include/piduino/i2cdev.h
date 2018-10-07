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

#ifndef PIDUINO_I2CDEV_H
#define PIDUINO_I2CDEV_H

#include <deque>
#include <string>
#include <piduino/system.h>
#include <piduino/iodevice.h>

/**
 *  @defgroup piduino_i2cdev I2c Bus
 *
 *  Ce module permet l'utilisation des bus I2c par le driver i2cdev.
 *  @{
 */

namespace Piduino {

  class I2cDev : public IoDevice {
    public:

      class Info {
        public:
          static const int MaxBuses = 32;
          Info (int id = 0) {
            setId (id);
          }
          inline int id() const {
            return _id;
          }
          inline void setId (int id) {
            _id = id;
            _path = busPath (id);
          }
          inline const std::string & path() const {
            return _path;
          }
          bool setPath (const std::string & path);
          inline bool exists() const {
            return System::charFileExists(_path);
          }
          bool operator== (const Info & other) {
            return (_path == other._path) ;
          }
          bool operator!= (const Info & other) {
            return (_path != other._path) ;
          }

          static std::string busPath (int id);
          static Info defaultBus ();
          /**
            Returns a list of available serial ports on the system.
           */
          static std::deque<Info> availableBuses ();


        private:
          int _id;
          std::string _path;
      };

      I2cDev ();
      I2cDev (const Info & bus);
      explicit I2cDev (const char * path);
      explicit I2cDev (const std::string & path);
      explicit I2cDev (int idBus);
      virtual ~I2cDev();

      virtual bool open (OpenMode mode = IoDevice::ReadWrite);
      virtual void close();

      void setBus (const Info & bus);
      void setBus (int idBus);
      const Info & bus() const;
      void setBusPath (const char * path);
      void setBusPath (const std::string & path);

      void beginTransmission (uint16_t slave);
      bool endTransmission (bool stop = true);
      virtual int write (uint8_t data);
      virtual int write (const uint8_t * buffer, uint16_t len);

      inline int write (std::string str) {
        return write (reinterpret_cast<const uint8_t *> (str.c_str()), str.length());
      }
      inline int write (const char * str) {
        return write (std::string (str));
      }
      inline int write (int data) {
        return write (static_cast<uint8_t> (data));
      }
      inline int write (unsigned int data) {
        return write (static_cast<uint8_t> (data));
      }
      inline int write (long data) {
        return write (static_cast<uint8_t> (data));
      }
      inline int write (unsigned long data) {
        return write (static_cast<uint8_t> (data));
      }

      int requestFrom (uint16_t slave, uint16_t max, bool stop = true);
      inline int requestFrom (int slave, int max, int stop = 1) {
        return requestFrom (static_cast<uint16_t> (slave), static_cast<uint16_t> (max), stop != 0);
      }

      uint16_t available() const;
      virtual int read (uint8_t * buffer, uint16_t max);
      virtual int read();
      virtual int peek() const;

      virtual void flush ();

    protected:
      class Private;
      I2cDev (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (I2cDev)
  };
  // ---------------------------------------------------------------------------
  //
  //                      Piduino I2cDev Global Object
  //
  // ---------------------------------------------------------------------------
  extern I2cDev Wire; ///< Piduino I2cDev Global Object
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_I2CDEV_H defined */
