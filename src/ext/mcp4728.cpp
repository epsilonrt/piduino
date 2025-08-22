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
#include <piduino/clock.h>
#include "mcp4728_p.h"
#include "config.h"

using namespace std::chrono;

namespace Piduino {

  namespace Mcp4728Defs {
    // Command byte (2nd byte)
    const uint8_t C2 = BIT (7);
    const uint8_t C1 = BIT (6);
    const uint8_t C0 = BIT (5);
    const uint8_t W1 = BIT (4);


    // Specific for write DAC
    const uint8_t W0 = BIT (3);
    const uint8_t DAC1 = BIT (2);
    const uint8_t DAC0 = BIT (1);
    const uint8_t WRCHIDX = 1;
    const uint8_t UDAC = BIT (0);

    const uint8_t MTWRDAC = C1; // Multi Write DAC
    const uint8_t SGWRDACEE = C1 | W1 | W0; // Single Write DAC and EEPROM
    const uint8_t SQWRDACEE = C1 | W1; // Sequential Write DAC and EEPROM

    const uint8_t VREF = BIT (7);
    const uint8_t PD1 = BIT (6);
    const uint8_t PD0 = BIT (5);
    const uint8_t GX = BIT (4);

    const uint8_t PD_1K = PD0;
    const uint8_t PD_100K = PD1;
    const uint8_t PD_500K = PD1 | PD0;
    const uint8_t PD_MASK = PD1 | PD0;

    // Reading
    const uint8_t RDY = BIT (7);
    const uint8_t POR = BIT (6);
    const uint8_t RDAC1 = BIT (5);
    const uint8_t RDAC0 = BIT (4);
    const uint8_t RWCHIDX = 4;
    const uint8_t DAC_MASK = RDAC1 | RDAC0;
    const uint8_t EEPROM = BIT (3);
    const uint8_t A2 = BIT (2);
    const uint8_t A1 = BIT (1);
    const uint8_t A0 = BIT (0);
    const uint8_t READY = RDY | POR;

    // Specific for set voltage reference
    const uint8_t VREFA = BIT (3);
    const uint8_t VREFB = BIT (2);
    const uint8_t VREFC = BIT (1);
    const uint8_t VREFD = BIT (0);

  } // namespace Mcp4728Defs
  using namespace Mcp4728Defs;

  // -----------------------------------------------------------------------------
  //
  //                             Mcp4728 Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  Mcp4728::Mcp4728 (Mcp4728::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  bool
  Mcp4728::fastWrite (const uint16_t values[4]) {
    PIMP_D (Mcp4728);

    return d->fastWrite (values);
  }

  // ---------------------------------------------------------------------------
  Mcp4728::Mcp4728 (int busId, int address) :
    Converter (*new Private (this, std::make_shared<I2cDev> (busId), address)) {}

  // ---------------------------------------------------------------------------
  Mcp4728::Mcp4728 (const std::string &parameters) :
    Converter (*new Private (this, parameters)) {}

  // ---------------------------------------------------------------------------
  Mcp4728::~Mcp4728() = default;

  // ---------------------------------------------------------------------------
  // Register the Mcp4728 converter with the factory
  REGISTER_CONVERTER (Mcp4728, "dac", "bus=id:addr={0x60..0x67}:ref={vdd,int}:fsr=value:gain={1,2}:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}");

  // -----------------------------------------------------------------------------
  //
  //                         Mcp4728::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Mcp4728::Private::Private (Mcp4728 *q, std::shared_ptr<I2cDev> dev, int address) :
    Converter::Private (q, DigitalToAnalog, hasResolution | hasRange | hasModeSetting | hasReference | hasSetReference | hasReferencePerChannel),
    i2c (dev),
    addr (address), isConnected (false) {}

  // ---------------------------------------------------------------------------
  // "bus=id:addr={0x60..0x67}:ref={vdd,int}:fsr=value:gain={1,2}:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}"
  Mcp4728::Private::Private (Mcp4728 *q, const std::string &params) :
    Converter::Private (q, DigitalToAnalog, hasResolution | hasRange | hasModeSetting | hasReference | hasSetReference | hasReferencePerChannel, params),
    i2c (std::make_shared<I2cDev> (I2cDev::Info::defaultBus().id())),
    addr (DefaultAddress), isConnected (false) {
    int refId = DefaultReference;
    double fsr = 0.0;
    Mode m = DefaultMode; // AnalogOutput, Gain1

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

    bool modeSetByUser = false;
    bool refSetByUser = false;

    it = paramsMap.find ("gain");
    if (it != paramsMap.end()) {
      int gain = it->second.empty() ? 1 : std::stoi (it->second);
      if (gain == 2) {
        m = (m & ~GainMask) | Gain2; // Set gain mode
      }
      modeSetByUser = true;
    }

    it = paramsMap.find ("mode");
    if (it != paramsMap.end()) {
      std::string str = it->second;

      if (str == "eeprom") {
        m |= SaveToEEPROM; // Set save to EEPROM mode
      }
      else if (str == "fast") {
        m |= FastMode; // Set fast mode
      }
      else if (str == "pd1k") {
        m |= PwrDwnR1; // Set power down mode 1k
        modeSetByUser = true;
      }
      else if (str == "pd100k") {
        m |= PwrDwnR2; // Set power down mode 100k
        modeSetByUser = true;
      }
      else if (str == "pd500k") {
        m |= PwrDwnR3; // Set power down mode 500k
        modeSetByUser = true;
      }
      else if (str != "norm") {
        std::cerr << "Mcp4728: Invalid mode value: " << str << ", using default mode." << std::endl;
      }
    }

    it = paramsMap.find ("fsr");
    if (it != paramsMap.end()) {
      double fsrValue = std::stod (it->second);

      fsr = fsrValue;
    }

    it = paramsMap.find ("ref");
    if (it != paramsMap.end()) {
      std::string ref = it->second;
      static const std::map<std::string, int> referenceMap = {
        {"int", InternalReference},
        {"vdd", VddReference},
      };

      auto refIt = referenceMap.find (ref);
      if (refIt != referenceMap.end()) {

        refId = refIt->second;
        refSetByUser = true; // Reference is set by user
      }
      else {
        // Log a warning if the reference is invalid
        std::cerr << "Mcp4728: Invalid reference value: " << ref << ", using default reference." << std::endl;
      }
    }

    for (auto &ch : chan) {

      ch.mode = m;
      ch.modeSetByUser = modeSetByUser;
      ch.refSetByUser = refSetByUser;
      ch.setReference (refId, fsr);
    }
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Mcp4728::Private::open (OpenMode mode) {

    if (i2c->open (mode)) {

      if (isDebug) {

        std::cout << "Mcp4728: I2c Device opened successfully." << std::endl;
      }

      if (updateSetup ()) {

        if (isDebug) {

          std::cout << "Mcp4728: Configuration updated successfully." << std::endl;
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

        std::cerr << "Mcp4728: Failed to open device. Error(" << i2c->error() << ") :" << i2c->errorString() << std::endl;
      }
      isConnected = false; // Set the connection status to false
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  void
  Mcp4728::Private::close() {

    Converter::Private::close();
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Mcp4728::Private::read() {

    return readChannel();
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Mcp4728::Private::readChannel (int channel, bool differential) {

    if (mode() & SaveToEEPROM) {

      return chan.at (channel).eeprom; // Return the EEPROM value if in EEPROM mode
    }
    return chan.at (channel).value; // ✅ this will throw if out of bounds
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Mcp4728::Private::write (long value) {

    return writeChannel (value);
  }

  // ---------------------------------------------------------------------------
  // override
  // value is clamped to 12 bits by the caller
  bool
  Mcp4728::Private::writeChannel (long value, int channel, bool noUpdate) {

    if (channel == -1) { // Write All channels

      for (auto &ch : chan) {

        ch.value = static_cast<uint16_t> (value); // Update the current value
      }

      if (chan[0].mode & FastMode || chan[1].mode & FastMode || chan[2].mode & FastMode || chan[3].mode & FastMode) {

        return fastWrite (value);
      }
      else { // No Fast Mode

        ChannelBuffer buffers[NofChannels] = {
          {chan[0], 0, noUpdate},
          {chan[1], 1, noUpdate},
          {chan[2], 2, noUpdate},
          {chan[3], 3, noUpdate}
        };

        if (chan[0].mode & SaveToEEPROM || chan[1].mode & SaveToEEPROM || chan[2].mode & SaveToEEPROM || chan[3].mode & SaveToEEPROM) {

          for (int i = 0; i < NofChannels; ++i) {
            if (chan[i].mode & SaveToEEPROM) {

              clk.delay (55); // Wait 55ms between writes to EEPROM (TWRITE)
            }
            if (!writeRegisters (buffers[i])) {
              std::cerr << "Mcp4728: Write failed for channel: " << buffers[i].channel() << std::endl;
              return false; // Return false if any write fails
            }
          }
          return true;
        }
        else {
          // Write all channels in a single operation
          if (writeRegisters (reinterpret_cast<const uint8_t *> (&buffers), sizeof (buffers))) {
            return true;
          }
        }
      }
    }
    else if (channel >= 0 && channel < NofChannels) { // Write Single channel
      chan[channel].value = static_cast<uint16_t> (value); // Update the current value
      ChannelBuffer buffer (chan.at (channel), channel, noUpdate);

      if (!writeRegisters (buffer)) {

        chan.at (channel).value = 0; // Reset the value on error
        std::cerr << "Mcp4728: Write failed for channel: " << channel << std::endl;
        return false;
      }
      return true;
    }
    else {
      if (isDebug) {

        std::cerr << "Mcp4728: Invalid channel number: " << channel << ". Must be between 0 and " << (NofChannels - 1) << ", or -1 for all channels." << std::endl;
      }
    }

    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Mcp4728::Private::setFullScaleRange (double fsr, int channel)  {

    if (fsr != 0.0) {
      if (channel == -1) {
        // Set for all channels
        for (auto &ch : chan) {
          ch.fsr = fsr;
        }
      }
      else {

        chan.at (channel).fsr = fsr; // ✅ this will throw if out of bounds
      }
      return true;
    }
    else {
      if (isDebug) {
        std::cerr << "Mcp4728::Private::setFullScaleRange: Invalid full-scale range: " << fsr << std::endl;
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Mcp4728::Private::setReference (int referenceId, double fsr, int channel) {

    if (referenceId == VddReference || referenceId == InternalReference || referenceId == DefaultReference) {

      if (channel == -1) { // All channels
        // Set for all channels
        for (auto &ch : chan) {

          if (referenceId != ch.refId) {
            ch.refSetByUser = true; // Mark the channel as set by user
          }
          ch.setReference (referenceId,  fsr);
        }
      }
      else { // Single channel
        Channel &ch  = chan.at (channel);

        if (referenceId != ch.refId) {
          ch.refSetByUser = true; // Mark the channel as set by user
        }
        ch.setReference (referenceId, fsr);
      }
      return true;
    }
    else {
      if (isDebug) {
        std::cerr << "Mcp4728::Private::setReference: Unsupported reference ID: " << referenceId << std::endl;
      }
    }
    return false; // Default implementation returns false, should be overridden by subclasses
  }


  // ---------------------------------------------------------------------------
  // override
  Mcp4728::Mode
  Mcp4728::Private::mode (int channel) const {

    return chan.at (channel).mode; // ✅ this will throw if out of bounds
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
  Mcp4728::Private::setMode (Mode m, int channel) {
    // Only allow specific mode flags
    Mode mask = AnalogOutput | GainMask | SaveToEEPROM | PwrDwnMask | FastMode;

    if (m & ~mask) {
      if (isDebug) {
        std::cerr << "Mcp4728: Invalid mode flags, you can only set AnalogOutput, GainMask, SaveToEEPROM, FastMode, PwrDwnMask." << std::endl;
      }
      return false; // Return false if the mode contains invalid flags
    }

    if (! (m & PwrDwnEn) && (m & (PwrDwn0 | PwrDwn1))) {
      if (isDebug) {
        std::cerr <<  "Mcp4728: PwrDwnEn must be set if PwrDwn0 or PwrDwn1 is set." << std::endl;
      }
      return false; // Return false if PwrDwnEn is set without PwrDwn0 or PwrDwn1
    }

    if (channel == -1) {
      for (auto &ch : chan) {
        ch.mode = m | AnalogOutput; // ✅ this will throw if out of bounds
        ch.modeSetByUser = true; // Mark the channel as set by user
      }
      return true; // Return true if the mode was set for all channels
    }
    else if (channel >= 0 && channel < NofChannels) {
      chan.at (channel).mode = m | AnalogOutput; // ✅ this will throw if out of bounds
      chan.at (channel).modeSetByUser = true; // Mark the channel as set by user
      return true;
    }
    return false;

  }

  // ---------------------------------------------------------------------------
  // specific to Mcp4728
  bool
  Mcp4728::Private::fastWrite (const uint16_t values[4]) {

    uint8_t buffer[RegFastWriteLength];

    for (int i = 0; i < NofChannels; ++i) {

      buffer[i * 2] = static_cast<uint8_t> ( (values[i] >> 8) & 0x0F); // High byte
      buffer[i * 2 + 1] = static_cast<uint8_t> (values[i] & 0xFF); // Low byte
    }

    return writeRegisters (buffer, sizeof (buffer)); // Write the buffer to the device
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Mcp4728::Private::fastWrite (uint16_t value) {
    uint8_t buffer[RegFastWriteLength];
    uint8_t msb = static_cast<uint8_t> ( (value >> 8) & 0x0F);
    uint8_t lsb = static_cast<uint8_t> (value & 0xFF);

    for (int i = 0; i < 4; ++i) {
      buffer[i * 2] = msb;
      buffer[i * 2 + 1] = lsb;
    }
    return writeRegisters (buffer, sizeof (buffer)); // Write the buffer to the device
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Mcp4728::Private::updateSetup() {
    ChannelBuffer buffers[NofChannels * 2];

    if (readRegisters (buffers)) {

      for (auto &buffer : buffers) {
        uint8_t idx = buffer.channel (true);

        buffer.setChannel (chan[idx]); // Set the channel data from the buffer
      }
      return true;
    }
    else {

      if (isDebug) {

        std::cerr << "Mcp4728: Failed to read registers." << std::endl;
      }
    }

    return false;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Mcp4728::Private::readRegisters (ChannelBuffer *buffer) const {

    if (i2c->isOpen()) {
      // Implement I2C read operation here
      uint16_t max = sizeof (ChannelBuffer) * NofChannels * 2;

      if (i2c->requestFrom (addr, max) >= 0) { // The frame is send now ! > START + ADDR + W + REG + RESTART + ADDR + R + DATA READ + STOP

        uint16_t bytesRead = i2c->available();
        i2c->read (reinterpret_cast<uint8_t *> (buffer), std::min (bytesRead, max));
        isConnected = true;
        return bytesRead == max; // Return true if all requested bytes were read
      }
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Mcp4728::Private::writeRegisters (const uint8_t *buffer, uint16_t length) {

    if (i2c->isOpen()) {

      i2c->beginTransmission (addr);
      i2c->write (buffer, length);
      if (i2c->endTransmission ()) { // The frame is send now ! > START + ADDR + W + REG + DATA WRITE + STOP
        isConnected = true;
        return true;
      }
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  //
  //                   Mcp4728::Private::ChannelBuffer Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // set buffer from channel
  void Mcp4728::Private::ChannelBuffer::setBuffer (Channel &ch, uint8_t index, bool noUpdate, uint8_t wrCmd) {
    // [             cmd            ]
    // B7 B6 B5 B4 B3 B2   B1   B0
    // C2 C1 C0 W1 W0 DAC1 DAC0 UDAC
    cmd = wrCmd ? wrCmd : (ch.mode & SaveToEEPROM ? SGWRDACEE : MTWRDAC);
    cmd |= (index << WRCHIDX);
    cmd |= noUpdate ? UDAC : 0;

    // [             data[0]               ][          data[1]            ]
    // B15  B14  B13  B12  B11  B10  B9  B8  B7  B6  B5  B4  B3  B2  B1  B0
    // VREF PD1  PD0  GX   D11  D10  D9  D8  D7  D6  D5  D4  D3  D2  D1  D0
    data[0] = ch.value >> 8; // MSB
    data[1] = ch.value & 0xFF; // LSB

    data[0] |= (ch.refId == InternalReference) ? VREF : 0;
    data[0] |= ( (ch.mode & GainMask) == Gain2) ? GX : 0;

    if (ch.mode & PwrDwnEn) {
      switch (ch.mode.value() & PwrDwnMask) {
        case PwrDwnR1:
          data[0] |= PD_1K;
          break;
        case PwrDwnR2:
          data[0] |= PD_100K;
          break;
        case PwrDwnR3:
          data[0] |= PD_500K;
          break;
        default:
          break;
      }
    }

    if (ch.mode & SaveToEEPROM) {

      ch.eeprom = ch.value; // Update the EEPROM value if in EEPROM mode
    }
  }

  // ---------------------------------------------------------------------------
  // set channel from buffer
  void
  Mcp4728::Private::ChannelBuffer::setChannel (Channel &ch) {

    if (cmd & EEPROM) {

      ch.eeprom = value();
      ch.isReady = (cmd & READY) == READY;
    }
    else {

      ch.value = value();

      if (!ch.modeSetByUser) { // If the mode was not set by user

        ch.mode &= ~ (PwrDwnMask | GainMask); // Keep only the power down and gain bits
        ch.mode |= mode() & (PwrDwnMask | GainMask);
      }

      if (!ch.refSetByUser) { // If the reference was not set by user

        ch.setReference (reference());
      }
    }
  }

  // [             cmd            ]
  // B7  B6  B5   B4   B3 B2 B1 B0
  // RDY POR DAC1 DAC0 0  A2 A1 A0

  // [             data[0]               ][          data[1]            ]
  // B15  B14  B13  B12  B11  B10  B9  B8  B7  B6  B5  B4  B3  B2  B1  B0
  // VREF PD1  PD0  GX   D11  D10  D9  D8  D7  D6  D5  D4  D3  D2  D1  D0
  // ---------------------------------------------------------------------------
  Converter::Mode
  Mcp4728::Private::ChannelBuffer::mode() const {
    Mode m = AnalogOutput; // Default mode is AnalogOutput

    if (data[0] & GX) {
      m |= Gain2; // Add gain mode if GX bit is set
    }
    else {
      m |= Gain1; // Default to gain 1 if GX bit is not set
    }
    switch (data[0] & PD_MASK) {
      case PD_1K:
        m |= PwrDwnR1; // Power down mode 1k
        break;
      case PD_100K:
        m |= PwrDwnR2; // Power down mode 100k
        break;
      case PD_500K:
        m |= PwrDwnR3; // Power down mode 500k
        break;
      default:
        break;
    }
    return m;
  }

  // ---------------------------------------------------------------------------
  int
  Mcp4728::Private::ChannelBuffer::reference() const {
    return (data[0] & VREF) ? InternalReference : VddReference;
  }

  // ---------------------------------------------------------------------------
  uint16_t
  Mcp4728::Private::ChannelBuffer::value() const {

    return (data[0] & 0x0F) << 8 | data[1]; // Combine the two bytes to get the value
  }

  // ---------------------------------------------------------------------------
  uint8_t
  Mcp4728::Private::ChannelBuffer::channel (bool isRxFrame) const {

    return (cmd >> (isRxFrame ? RWCHIDX : WRCHIDX)) & 0x03;
  }


  // ---------------------------------------------------------------------------
  void
  Mcp4728::Private::Channel::setReference (int referenceId, double fsr) {

    if (fsr != 0.0) {

      this->fsr = fsr;
    }
    else if (refId != referenceId) {

      if (referenceId == InternalReference) {

        this->fsr = InternalFullScaleRange;
        if ( (mode & GainMask) == Gain2) {

          this->fsr *= 2.0; // Apply gain if set
        }
      }
      else {

        this->fsr = DefaultFullScaleRange;
      }
    }
    refId = referenceId;
  }

} // namespace Piduino
/* ========================================================================== */
