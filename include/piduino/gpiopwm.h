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

#ifndef _PIDUINO_GPIOPWM_H_
#define _PIDUINO_GPIOPWM_H_

#include <atomic>
#include <thread>
#include <mutex>
#include <piduino/pwm.h>
#include <piduino/gpiopin.h>

/**
 *  @defgroup piduino_gpiopwm GPIO PWM generator
 *  @{
 */

namespace Piduino {
  class GpioPwm;

  class GpioPwm : public Pwm {

    public:
      GpioPwm (Pin * pin, unsigned int resolution = 16, unsigned long clock = 1000);
      virtual ~GpioPwm();

      virtual bool open ();
      virtual void close();
      virtual bool isOpen() const;
      Pin * pin() const;

      virtual long clock() const;
      virtual long read();
      virtual void write (long value);
      virtual const std::string & name() const;

    protected:
      virtual void setClock (long clk);

    private:
      Pin * _pin;
      long _value;
      long _clk;
      static std::string _name;

      std::atomic<int> _flag;  // communication avec le thread
      static const int FlagRun = 1;
      static const int FlagValueUpdated = 2;
      std::thread _thread;
      static void * generator (std::atomic<int> & flag, GpioPwm * pwm);
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_GPIOPWM_H_ defined */
