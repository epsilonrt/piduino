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
#include <iostream>
#include "mcp4725_p.h"
#include "config.h"

using namespace std::chrono;

namespace Piduino {

  namespace Mcp4725Defs {
    const uint8_t C2 = BIT (7);
    const uint8_t C1 = BIT (6);
    const uint8_t C0 = BIT (5);
    const uint8_t PD1 = BIT (2);
    const uint8_t PD0 = BIT (1);
    const uint8_t WRDAC = C1;
    const uint8_t WRDACEE = C1 | C0;
    const uint8_t PD_1K = PD0;
    const uint8_t PD_100K = PD1;
    const uint8_t PD_500K = PD1 | PD0;
    const uint8_t PD_MASK = PD1 | PD0;
    const uint8_t PD_FAST_1K = PD_1K << 3;
    const uint8_t PD_FAST_100K = PD_100K << 3;
    const uint8_t PD_FAST_500K = PD_500K << 3;
    const uint8_t PD_FAST_MASK = PD_MASK << 3;
    const uint8_t RDY = BIT (7);
    const uint8_t POR = BIT (6);
    const uint8_t READY = RDY | POR;

  } // namespace Mcp4725Defs
  using namespace Mcp4725Defs;

  // -----------------------------------------------------------------------------
  //
  //                             Mcp4725 Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  Mcp4725::Mcp4725 (Mcp4725::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  Mcp4725::Mcp4725 (int busId, int address) :
    Converter (*new Private (this, std::make_shared<I2cDev> (busId), address)) {}

  // ---------------------------------------------------------------------------
  Mcp4725::Mcp4725 (const std::string &parameters) :
    Converter (*new Private (this, parameters)) {}

  // ---------------------------------------------------------------------------
  Mcp4725::~Mcp4725() = default;

  // ---------------------------------------------------------------------------
  // Register the Mcp4725 converter with the factory
  REGISTER_CONVERTER (Mcp4725, "dac", "bus=id:addr={0x60..0x67}:fsr=value:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}");

  // -----------------------------------------------------------------------------
  //
  //                         Mcp4725::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Mcp4725::Private::Private (Mcp4725 *q, std::shared_ptr<I2cDev> dev, int address) :
    Converter::Private (q, DigitalToAnalog, hasResolution | hasRange | hasModeSetting),
    i2c (dev),
    addr (address), fsr (DefaultFullScaleRange), isConnected (false), value (0), currentMode (NormalMode | AnalogOutput), modeSetByUser (false) {}

  // ---------------------------------------------------------------------------
  // "bus=id:addr={0x60..0x67}:fsr=value:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}"
  Mcp4725::Private::Private (Mcp4725 *q, const std::string &params) :
    Converter::Private (q, DigitalToAnalog, hasResolution | hasRange | hasModeSetting, params),
    i2c (std::make_shared<I2cDev> (I2cDev::Info::defaultBus().id())),
    addr (DefaultAddress), fsr (DefaultFullScaleRange), isConnected (false), value (0), currentMode (NormalMode | AnalogOutput), modeSetByUser (false) {

    std::map<std::string, std::string> paramsMap = parseParameters (parameters);

    auto it = paramsMap.find ("bus");
    if (it != paramsMap.end()) {
      int busId = it->second.empty() ? 0 : std::stoi (it->second);
      i2c->setBus (busId);
    }

    it = paramsMap.find ("addr");
    if (it != paramsMap.end()) {
      int id = it->second.empty() ? 0x60 : std::stoi (it->second);
      addr = id;
    }

    it = paramsMap.find ("fsr");
    if (it != paramsMap.end()) {
      double fsrValue = std::stod (it->second);
      if (fsrValue > 0.0) {

        fsr = fsrValue; // Set the full-scale range value
      }
      else {
        // Log a warning if the full-scale range is invalid
        std::cerr << "Mcp4725: Invalid full-scale range value: " << it->second << ", using default value of " << fsr << std::endl;
      }
    }

    it = paramsMap.find ("mode");
    if (it != paramsMap.end()) {
      std::string str = it->second;

      modeSetByUser = true;
      if (str == "fast") {
        currentMode |= FastMode; // Set fast mode
      }
      else if (str == "eeprom") {
        currentMode |= SaveToEEPROM; // Set save to EEPROM mode
      }
      else if (str == "pd1k") {
        currentMode |= PwrDwnR1; // Set power down mode 1k
      }
      else if (str == "pd100k") {
        currentMode |= PwrDwnR2; // Set power down mode 100k
      }
      else if (str == "pd500k") {
        currentMode |= PwrDwnR3; // Set power down mode 500k
      }
      else if (str != "norm") {
        std::cerr << "Mcp4725: Invalid mode value: " << str << ", using default mode." << std::endl;
        modeSetByUser = false;
      }
    }
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Mcp4725::Private::open (OpenMode mode) {

    if (i2c->open (mode)) {

      if (isDebug) {

        std::cout << "Mcp4725: I2c Device opened successfully." << std::endl;
      }

      if (updateSetup ()) {

        if (isDebug) {

          std::cout << "Mcp4725: Configuration updated successfully." << std::endl;
        }
        return Converter::Private::open (mode);
      }
    }
    else {

      if (i2c->error()) {

        setError (); // Get errno and set error message
      }
      else {

        setError (ENODEV); // Set device not found error if no error code is available
      }
      if (isDebug) {

        std::cerr << "Mcp4725: Failed to open device. Error(" << i2c->error() << ") :" << i2c->errorString() << std::endl;
      }
      isConnected = false; // Set the connection status to false
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  void
  Mcp4725::Private::close() {
    Converter::Private::close();
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Mcp4725::Private::read() {

    return readChannel();
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Mcp4725::Private::readChannel (int channel, bool differential) {

    return value;
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Mcp4725::Private::write (long value) {

    return writeChannel (value);
  }

  // ---------------------------------------------------------------------------
  // override
  // value is clamped to 12 bits by the caller
  bool
  Mcp4725::Private::writeChannel (long value, int channel, bool differential) {

    if (i2c->isOpen()) {
      uint16_t bytesToWrite;
      
      if (currentMode & FastMode) {

        bytesToWrite = RegFastWriteLength; // Use fast write length
        regs.config = value >> 8;
        regs.dac[0] = value & 0xFF;
      }
      else { // Normal and SaveToEEPROM modes
        bytesToWrite = RegWriteLength;
        regs.dac[0] = (value >> 4) & 0xFF; // High byte (bits 11-4)
        regs.dac[1] = (value << 4) & 0xF0;

        if (currentMode == SaveToEEPROM) {
          regs.config = WRDACEE;
        }
        else {
          regs.config = WRDAC;
        }

        if (currentMode & PwrDwnEn) {
          switch (currentMode.value() & PwrDwnMask) {
            case PwrDwnR1:
              regs.config |= PD_1K;
              break;
            case PwrDwnR2:
              regs.config |= PD_100K;
              break;
            case PwrDwnR3:
              regs.config |= PD_500K;
              break;
            default:
              break;
          }
        }
      }
      if (writeRegisters (regs, bytesToWrite)) {

        this->value = value; // Update the current value
        return true; // Return true if the write operation was successful
      }
    }

    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  Mcp4725::Mode
  Mcp4725::Private::mode (int channel) const {

    return currentMode;
  }

  // ---------------------------------------------------------------------------
  // override
  // | **Flag**      | **Str**     |
  // |---------------|-------------|
  // | DigitalInput  | in          |
  // | DigitalOutput | out         |
  // | AnalogInput   | ain         |
  // | AnalogOutput  | aout        |
  // | ActiveLow     | activelow   |
  // | PullUp        | up          |
  // | PullDown      | down        |
  // | NoMode        | off         |
  // | EdgeRising    | rising      |
  // | EdgeFalling   | falling     |
  // | Interrupt     | int         |
  // | Continuous    | continuous  |
  // | SingleShot    | single      |
  // | FastMode      | fast        |
  // | SaveToEEPROM  | eeprom      |
  // | PwrDwnEn      | pden        |
  // | PwrDwn0       | pd0         |
  // | PwrDwn1       | pd1         |
  bool
  Mcp4725::Private::setMode (Mode m, int channel) {
    // Only allow specific mode flags
    Mode mask = AnalogOutput | FastMode | SaveToEEPROM | PwrDwnEn | PwrDwn0 | PwrDwn1;

    if (m & ~mask) {
      if (isDebug) {
        std::cerr << "Mcp4725: Invalid mode flags, you can only set AnalogOutput, FastMode, SaveToEEPROM, PwrDwnEn, PwrDwn0, and PwrDwn1." << std::endl;
      }
      return false; // Return false if the mode contains invalid flags
    }

    if (m & FastMode && m & SaveToEEPROM) {
      if (isDebug) {
        std::cerr << "Mcp4725: Fast mode and SaveToEEPROM cannot be set together." << std::endl;
      }
      return false; // Return false if both FastMode and SaveToEEPROM are set
    }

    if (! (m & PwrDwnEn) && (m & (PwrDwn0 | PwrDwn1))) {
      if (isDebug) {
        std::cerr <<  "Mcp4725: PwrDwnEn must be set if PwrDwn0 or PwrDwn1 is set." << std::endl;
      }
      return false; // Return false if PwrDwnEn is set without PwrDwn0 or PwrDwn1
    }

    modeSetByUser = true;
    currentMode = m | AnalogOutput;
    return true;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Mcp4725::Private::updateSetup() {

    if (i2c->isOpen()) {
      if (readRegisters (regs)) {

        isConnected = true; // Set the connection status to true
        if ( (regs.config & READY) == READY) {
          uint16_t rv;

          if (!modeSetByUser) {
            switch (regs.config & PD_MASK) {
              case PD_1K:
                currentMode |= PwrDwnR1;
                break;
              case PD_100K:
                currentMode |= PwrDwnR2;
                break;
              case PD_500K:
                currentMode |= PwrDwnR3;
                break;
              default:
                currentMode |= NormalMode;
                break;
            }
          }

          if (currentMode & SaveToEEPROM) {
            rv = (regs.eeprom[0] & 0x0F) << 8 | regs.eeprom[1];
          }
          else {

            rv = static_cast<uint16_t> (regs.dac[0]) << 4 | (static_cast<uint16_t> (regs.dac[1]) >> 4) & 0x000F;
          }
          value = rv;
          return true; // Return true if the registers were read successfully
        }
        else {
          if (isDebug) {
            std::cerr << "Mcp4725: Device not ready. Config: " << std::hex << static_cast<int> (regs.config) << std::dec << std::endl;
          }
        }
      }
      else {
        if (isDebug) {
          std::cerr << "Mcp4725: Failed to read registers." << std::endl;
        }
      }
    }

    return false;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Mcp4725::Private::readRegisters (RegBuffers &buffer, uint16_t max) const {

    if (i2c->isOpen()) {
      // Implement I2C read operation here

      if (i2c->requestFrom (addr, max) >= 0) { // The frame is send now ! > START + ADDR + W + REG + RESTART + ADDR + R + DATA READ + STOP

        uint16_t bytesRead = i2c->available();
        i2c->read (reinterpret_cast<uint8_t *> (&buffer), std::min (bytesRead, max));
        isConnected = true;
        return bytesRead == max; // Return true if all requested bytes were read
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool Mcp4725::Private::writeRegisters (RegBuffers &buffer, uint16_t size) {

    if (i2c->isOpen()) {

      i2c->beginTransmission (addr);
      i2c->write (reinterpret_cast<uint8_t *> (&buffer), size);
      if (i2c->endTransmission ()) { // The frame is send now ! > START + ADDR + W + REG + DATA WRITE + STOP
        isConnected = true;
        return true;
      }
    }
    return false;
  }

} // namespace Piduino
/* ========================================================================== */
