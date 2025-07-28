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
#ifndef FUJITSU_MB85RS_LIBRARY_H
#define FUJITSU_MB85RS_LIBRARY_H

#include <SPI.h>
#include <Print.h>

// -----------------------------------------------------------------------------
//
//                           FUJITSU MB85RS Class
//
// -----------------------------------------------------------------------------
class Mb85rs {
public:
    /// Status register bits
    enum {
      WPEN  = 7,
      QPI   = 6,
      LC1   = 5,
      LC0   = 4,
      BP1   = 3,
      BP0   = 2,
      WEL   = 1
    };
    
    enum BlockProtect {
      None = 0,
      UpperQuarter = (1 << BP0),
      UpperHalf = (1 << BP1),
      All = UpperQuarter | UpperHalf
    };
    
    class DeviceId {
      public:
        uint8_t manufacturerId;
        uint8_t continuationCode;
        uint16_t productId;
        DeviceId() : manufacturerId(0), continuationCode(0), productId(0) {}
    };

    Mb85rs (int csPin, size_t size);
    void begin (uint32_t fclk = 10000000);
    void write (size_t address, uint8_t * buffer, size_t len);
    void read (size_t address, uint8_t * buffer, size_t len);
    
    void setWriteEnable (bool enable);
    bool writeEnabled();
    BlockProtect blockProtected();
    void protectBlock(BlockProtect bp);
    void setStatus (uint8_t status);
    uint8_t status();
    DeviceId deviceId();
    
    inline size_t size() const {
      return _size;
    }
    inline int csPin() const {
      return _cspin;
    }
    
    static void printBlock (Print & console, size_t address, const uint8_t * buffer, size_t len);
    static void printHex (Print & console, uint32_t n, int width = 0);
  
private:
    void transfer (uint8_t * buffer, size_t len);
    void transfer (uint8_t  b);
    int _cspin;
    size_t _size;
    SPISettings _settings;
};

/* ========================================================================== */
#endif /* FUJITSU_MB85RS_LIBRARY_H defined */
