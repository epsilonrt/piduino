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
#ifndef PIDUINO_SOCPWM_H
#define PIDUINO_SOCPWM_H

#include <climits>
#include <piduino/pwm.h>
#include <piduino/gpiopin.h>

/**
 *  @defgroup piduino_gpiopwm SoC PWM generator
 *  @{
 */
namespace Piduino {
  /**
   * @class SocPwm
   * @brief
   */
  class SocPwm : public Pwm {

    public:
      SocPwm (Pin * pin = nullptr);
      virtual ~SocPwm();

      // Converter
      virtual int resolution() const;
      virtual int setResolution (int resolution);
      virtual void setEnable (bool enable);
      virtual bool isEnabled () const;
      virtual const std::string & deviceName() const;

      // Pwm
      virtual long frequency() const;
      virtual long setFrequency (long freq);
      virtual long range() const;
      virtual long setRange (long range);

      // SocPwm
      bool hasEngine() const; // no engine !
      bool hasPin() const;
      const Pin * pin() const;

      class Private;

      /**
       * @class Engine
       * @brief
       */
      class Engine {

        public:
          Engine (Private * d, Pin * p) : parent (d), pin (p) {}

          virtual bool open (OpenMode mode) = 0;
          virtual void close() = 0;
          virtual const std::string & deviceName() const = 0;

          // isOpen () checked before calling this functions
          virtual long frequency() const = 0;
          virtual int  resolution() const = 0;
          virtual long range() const { return (1L << resolution()); }
          virtual long max() const;
          virtual long min() const;
          virtual bool setFrequency (long freq) { return false; }
          virtual bool setResolution (int resolution) { return false; }
          virtual bool setRange (long range) { return false; }

          // hasPin () checked before calling this functions
          virtual long read() = 0;
          virtual bool write (long value) = 0;
          virtual void setEnable (bool enable) = 0;
          virtual bool isEnabled () const = 0;
          //---

          inline void run() { setEnable (true); }
          inline void stop() { setEnable (false); }
          inline bool hasPin() const { return pin != nullptr; }

          Private * parent;
          Pin * pin;
      };

    protected:
      SocPwm (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (SocPwm)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_SOCPWM_H defined */
