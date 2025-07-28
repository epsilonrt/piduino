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

#include <piduino/pwm.h>
#include <piduino/gpiopin.h>


namespace Piduino {

  /**
    @class SoftPwm
    @brief Software-based PWM implementation for Piduino.

    The SoftPwm class provides a software-driven Pulse Width Modulation (PWM) interface
    for controlling the duty cycle and frequency of a digital output pin. It inherits from
    the Pwm base class and allows flexible configuration of PWM parameters such as range
    and frequency. This class is useful for platforms where hardware PWM is not available
    or when additional PWM channels are required.

    @note The minimum tick value is set to 10 microseconds, which determines the smallest
    time unit for PWM period calculation (range * tick = period in microseconds).

    @see Pwm
  */
  class SoftPwm : public Pwm {

    public:
      /**
         @brief Constructs a SoftPwm instance with the specified pin, range, and frequency.
         @param pin Pointer to the Pin object to be used for PWM output.
         @param range The range of the PWM signal (default is 100).
         @param frequency The frequency of the PWM signal in Hz (default is 200).
      */
      SoftPwm (Pin *pin, long range = 100, long frequency = 200);

      /**
         @brief Destructor for SoftPwm.
         Cleans up resources and stops the PWM generation thread if running.
      */
      virtual ~SoftPwm();

      /**
         @brief Sets the PWM signal to be enabled or disabled.
         @param enable True to enable the PWM signal, false to disable it.
      */
      virtual void setEnable (bool enable);

      /**
         @brief Checks if the PWM signal is enabled.
         @return True if the PWM signal is enabled, false otherwise.
      */
      virtual bool isEnabled () const;

      /**
         @brief Returns the device name for the SoftPwm instance.
         @return A string representing the device name.
      */
      virtual const std::string &deviceName() const;

      // not supported, return -1
      // virtual int resolution() const;
      // virtual int setResolution (int resolution);

      /**
         @brief Returns the current frequency of the PWM signal.
         @return The frequency in Hz.
      */
      virtual long frequency() const;

      /**
         @brief Sets the frequency of the PWM signal.
         @param freq The desired frequency in Hz.
         @return The actual frequency set, or -1 if not supported.
      */
      virtual long setFrequency (long freq);

      /**
         @brief Returns the range of the PWM signal.
         @return The range value.
      */
      virtual long range() const;

      /**
         @brief Sets the range of the PWM signal.
         @param range The desired range value.
         @return The actual range set, or -1 if not supported.
      */
      virtual long setRange (long range);
      
      /**
         @brief Reads the current tick value of the PWM signal.

         The tick value represents the time duration for one unit of the PWM signal,
         which is calculated as (range * tick = period in microseconds).
         @note The tick value is determined by the frequency and range of the PWM signal.
         @note The minimum tick value is set to 10 microseconds.
         @note This function is useful for debugging and understanding the timing of the PWM signal.
         @return The current tick value of the PWM signal.
      */
      long tick() const;
      
      /**
         @brief Returns the minimum tick value for PWM calculations.
         @return The minimum tick value in microseconds.
      */
      static constexpr unsigned int minTick = 10; ///< Minimum tick value in microseconds.

    protected:
      class Private;
      SoftPwm (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (SoftPwm)
  };
}


