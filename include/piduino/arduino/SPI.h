/*
   Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
   Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
   Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
   Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
   SPI Master library for arduino.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either the GNU General Public License version 2
   or the GNU Lesser General Public License version 2.1, both as
   published by the Free Software Foundation.

   Modified 2018 by Pascal JEAN for Piduino Library
*/

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <piduino/spidev.h>
#include <Arduino.h>
#include <mutex>

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1

// SPI_ATOMIC_VERSION means that SPI has atomicity fixes and what version.
// This way when there is a bug fix you can check this define to alert users
// of your code if it uses better version of this library.
// This also implies everything that SPI_HAS_TRANSACTION as documented above is
// available too.
#define SPI_ATOMIC_VERSION 1

#ifndef LSBFIRST
#define LSBFIRST false
#endif
#ifndef MSBFIRST
#define MSBFIRST true
#endif

#define SPI_MODE0 SPI_MODE_0
#define SPI_MODE1 SPI_MODE_1
#define SPI_MODE2 SPI_MODE_2
#define SPI_MODE3 SPI_MODE_3

#define SPI_CLOCK_DIV4 4
#define SPI_CLOCK_DIV16 16
#define SPI_CLOCK_DIV64 64
#define SPI_CLOCK_DIV128 128
#define SPI_CLOCK_DIV2 2
#define SPI_CLOCK_DIV8 8
#define SPI_CLOCK_DIV32 32

class SPISettings : public Piduino::SpiDev::Settings {
  public:
    SPISettings (uint32_t clock = 4000000, uint8_t bitOrder = MSBFIRST, uint8_t dataMode = SPI_MODE0) :
      Piduino::SpiDev::Settings (clock, bitOrder, dataMode | SPI_NO_CS) {}
};


class SPIClass : protected Piduino::SpiDev {
public:
  
    SPIClass() : _defaultBus(Info::defaultBus()) {}

    // Initialize the SPI library with the default bus
    // calling end() before reusing SPI
    void begin();

    // Piduino Only: Initialize the SPI library with a specific bus
    void begin (int idBus, int idCs = 0);

    // Disable the SPI bus
    void end();

    // Piduino Only: Set the SPI bus to use
    // This function is used to set the default bus for SPI
    // It may call before begin() to set the bus
    void setDefaultBus (int bus, int cs = 0);

    // Piduino Only: Set the default bus path
    // This function is used to set the default bus path for SPI
    // It may call before begin() to set the bus
    void setDefaultBusPath (const String &path);

    // Piduino Only: Get the default bus path
    // This function returns the path of the default SPI bus
    String defaultBusPath() const {
      return _defaultBus.path();
    }

    // Piduino Only: Get the default bus identifier
    int defaultBusId() const {
      return _defaultBus.busId();
    }

    // Piduino Only: Get the default chip select identifier
    int defaultCsId() const {
      return _defaultBus.csId();
    }

    // -------------------------------------------------------------------------
    // Before using SPI.transfer() or asserting chip select pins,
    // this function is used to gain exclusive access to the SPI bus
    // and configure the correct settings.
    void beginTransaction (const SPISettings &s);

    // -------------------------------------------------------------------------
    // After performing a group of transfers and releasing the chip select
    // signal, this function allows others to access the SPI bus
    void endTransaction (void);

    // -------------------------------------------------------------------------
    // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
    uint8_t transfer (uint8_t data);

    // -------------------------------------------------------------------------
    uint16_t transfer16 (uint16_t data);

    // -------------------------------------------------------------------------
    void transfer (void *buf, size_t count);

    // -------------------------------------------------------------------------
    // This function is deprecated.  New applications should use
    // beginTransaction() to configure SPI settings.
    void setBitOrder (uint8_t b);

    // -------------------------------------------------------------------------
    // This function is deprecated.  New applications should use
    // beginTransaction() to configure SPI settings.
    void setDataMode (uint8_t dataMode);

    // -------------------------------------------------------------------------
    // This function is deprecated.  New applications should use
    // beginTransaction() to configure SPI settings.
    void setClockDivider (uint8_t clockDiv);

    // These undocumented functions should not be used.  SPI.transfer()
    // polls the hardware flag which is automatically cleared as the
    // AVR responds to SPI's interrupt
    inline void attachInterrupt() {}
    inline void detachInterrupt() {}
    // If SPI is used from within an interrupt, this function registers
    // that interrupt with the SPI library, so beginTransaction() can
    // prevent conflicts.  The input interruptNumber is the number used
    // with attachInterrupt.  If SPI is used from a different interrupt
    // (eg, a timer), interruptNumber should be 255.
    inline void usingInterrupt (uint8_t interruptNumber) {}
    // And this does the opposite.
    inline void notUsingInterrupt (uint8_t interruptNumber) {}

  private:
    std::mutex _pendingTransaction;
    Piduino::SpiDev::Info _defaultBus;
};

extern SPIClass SPI;

#endif
