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
#include "max7311_p.h"
#include "config.h"

using namespace std::chrono;

namespace Piduino {

  namespace Max7311Regs {

    // Register addresses
    const uint8_t InputPort1Reg = 0x00; // I7-I0
    const uint8_t InputPort2Reg = 0x01; // I15-I8
    const uint8_t OutputPort1Reg = 0x02; // O7-O0  default 0xFF
    const uint8_t OutputPort2Reg = 0x03; // O15-O8 default 0xFF

    const uint8_t Polarity1Reg = 0x04;  // Polarity Inversion I7-I0   -> 0 normal, 1 inverted, default 0x00
    const uint8_t Polarity2Reg = 0x05;  // Polarity Inversion I15-I8  -> 0 normal, 1 inverted, default 0x00
    const uint8_t Config1Reg = 0x06;    // Configuration B7-B0        -> 0 output, 1 input, default 0xFF
    const uint8_t Config2Reg = 0x07;    // Configuration B15-B8       -> 0 output, 1 input, default 0xFF
    const uint8_t TimeoutReg = 0x08;    // Timeout register           -> 0 no timeout, 1 timeout enabled, default 0x01


  } // namespace Max7311Regs
  using namespace Max7311Regs;

  // -----------------------------------------------------------------------------
  //
  //                             Max7311 Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  Max7311::Max7311 (Max7311::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  Max7311::Max7311 (int busId, int address) :
    Converter (*new Private (this, std::make_shared<I2cDev> (busId), address)) {}

  // ---------------------------------------------------------------------------
  Max7311::Max7311 (const std::string &parameters) :
    Converter (*new Private (this, parameters)) {}

  // ---------------------------------------------------------------------------
  Max7311::~Max7311() = default;

  // ---------------------------------------------------------------------------
  // Register the Max7311 converter with the factory
  REGISTER_CONVERTER (Max7311, "gpioexp", "bus=id:addr={0x20...0xDE}:bustimeout={0,1}");

  // -----------------------------------------------------------------------------
  //
  //                         Max7311::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Max7311::Private::Private (Max7311 *q, std::shared_ptr<I2cDev> dev, int address) :
    Converter::Private (q, GpioExpander, hasResolution | hasRange | hasModeSetting),
    i2c (dev),
    addr (address), isConnected (false), busTimeout (true) {}

  // ---------------------------------------------------------------------------
  Max7311::Private::Private (Max7311 *q, const std::string &params) :
    Converter::Private (q, GpioExpander, hasResolution | hasRange | hasModeSetting, params),
    i2c (std::make_shared<I2cDev> (I2cDev::Info::defaultBus().id())),
    addr (0x20), isConnected (false), busTimeout (true) {

    std::map<std::string, std::string> paramsMap = parseParameters (parameters);

    auto it = paramsMap.find ("bus");
    if (it != paramsMap.end()) {
      int busId = it->second.empty() ? 0 : std::stoi (it->second);
      i2c->setBus (busId);
    }

    it = paramsMap.find ("addr");
    if (it != paramsMap.end()) {
      int id = it->second.empty() ? 0x20 : std::stoi (it->second);
      addr = id;
    }

    it = paramsMap.find ("bustimeout");
    if (it != paramsMap.end()) {
      std::string str = it->second;
      if (str == "false" || str == "0") {
        busTimeout = false;
      }
    }
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Max7311::Private::open (OpenMode mode) {
    if (i2c->open (mode)) {

      if (isDebug) {
        std::cout << "Max7311: Device opened successfully." << std::endl;
      }

      if (readAllRegisters ()) {
        if (isDebug) {
          std::cout << "Max7311: Setup updated successfully." << std::endl;
        }
        // regs.config[0] = 0xFF; // Set all pins as inputs by default
        // regs.config[1] = 0xFF; // Set all pins as inputs by default
        // regs.polarity[0] = 0x00; //  Set all pins to normal polarity by default
        // regs.polarity[1] = 0x00; // Set all pins to
        regs.timeout = busTimeout ? 0x01 : 0x00; // Set timeout register based on busTimeout

        if (writeSetup ()) { // Write the setup to the device
          if (isDebug) {
            std::cout << "Max7311: Setup written successfully." << std::endl;
          }
        }
        else {
          if (isDebug) {
            std::cerr << "Max7311: Failed to write setup." << std::endl;
          }
        }

        isConnected = true; // Set the connection status to true
        return Converter::Private::open (mode);
      }
      else {
        if (isDebug) {
          std::cerr << "Max7311: Failed to update setup." << std::endl;
        }
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

        std::cerr << "Max7311: Failed to open device. Error(" << i2c->error() << ") :" << i2c->errorString() << std::endl;
      }
      isConnected = false; // Set the connection status to false
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  void
  Max7311::Private::close() {
    Converter::Private::close();
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Max7311::Private::read() {
    long value = InvalidValue;
    if (readInputPorts()) {
      value = (regs.input[0] << 8) | regs.input[1];
    }
    else {
      if (isDebug) {
        std::cerr << "Max7311: Failed to read input ports." << std::endl;
      }
    }
    return value;
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Max7311::Private::readChannel (int channel, bool differential) {
    long value = InvalidValue;
    uint8_t index = channel / 8; // Determine the index of the input port

    if (readInputPorts (index, 1)) {
      uint8_t bit = channel % 8; // Determine the bit position within the port
      value = (regs.input[index] >> bit) & 0x01; // Read the specific bit
    }
    return value;
  }
  // ---------------------------------------------------------------------------
  // override
  bool
  Max7311::Private::write (long value) {
    regs.output[1] = (value >> 8) & 0xFF; // Set the high byte
    regs.output[0] = value & 0xFF; // Set the low byte
    if (writeOutputPorts()) {
      if (isDebug) {
        std::cout << "Max7311: Output ports written successfully." << std::endl;
      }
      return true;
    }
    if (isDebug) {
      std::cerr << "Max7311: Failed to write output ports." << std::endl;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Max7311::Private::writeChannel (long value, int channel, bool differential) {
    uint8_t index = channel / 8; // Determine the index of the input port
    if (index >= 0 && index <= 1) {
      uint8_t bit = channel % 8; // Determine the bit position within the port

      if (value) {

        regs.output[index] |= (1 << bit); // Set the specific bit to 1
      }
      else {

        regs.output[index] &= ~ (1 << bit); // Set the specific bit to 0
      }

      if (writeOutputPorts (index, 1)) {
        if (isDebug) {
          std::cout << "Max7311: Output channel " << channel << " written successfully" << std::endl;
        }
        return true;
      }
    }
    if (isDebug) {
      std::cerr << "Max7311: Failed to write output channel " << channel << std::endl;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  Max7311::Mode
  Max7311::Private::mode (int channel) const {
    Mode result = NoMode;
    uint8_t index = channel / 8; // Determine the index of the input port

    if (index >= 0 && index <= 1) {

      if (readSetup (index, 1)) { // Read the setup for the specific port
        uint8_t bit = channel % 8; // Determine the bit position within the port

        if ( (regs.config[index] & (1 << bit)) == 0) {

          result |= DigitalOutput; // If the bit is 0, it's an output mode
        }
        else {

          result |= DigitalInput | PullUp; // If the bit is 1, it's an input mode
        }

        if ( (regs.polarity[index] & (1 << bit)) != 0) {

          result |= ActiveLow; // If the bit is set, it's active low
        }

      }
      else {
        if (isDebug) {
          std::cerr << "Max7311: Failed to read setup for channel " << channel << std::endl;
        }
      }
    }
    return result;
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Max7311::Private::setMode (Mode m, int channel) {

    if (m & ~ (DigitalOutput | DigitalInput | ActiveLow)) {

      if (isDebug) {
        std::cerr << "Max7311:setMode: only support DigitalOutput, DigitalInput and ActiveLow modes. Input has always been pulled up." << std::endl;
      }
      return false; // Return false if pull-up or pull-down mode is requested
    }

    if ((m & DigitalOutput) && (m & DigitalInput)) {

      if (isDebug) {
        std::cerr << "Max7311:setMode: cannot set both DigitalOutput and DigitalInput modes at the same time." << std::endl;
      }
      return false; // Return false if both output and input modes are requested
    }

    if (channel >= 0) {
      uint8_t index = channel / 8; // Determine the index of the input port

      if (index >= 0 && index <= 1) {
        if (readSetup (index, 1)) { // Read the setup for the specific port

          uint8_t bit = channel % 8; // Determine the bit position within the port



          if (m & DigitalOutput) {

            regs.config[index] &= ~ (1 << bit); // Set the specific bit to 0 for output
          }
          else if (m & DigitalInput) {

            regs.config[index] |= (1 << bit); // Set the specific bit to 1 for input
          }
          if (m & ActiveLow) {

            regs.polarity[index] |= (1 << bit); // Set the specific bit to 1 for active low
          }
          else {

            regs.polarity[index] &= ~ (1 << bit); // Set the specific bit to 0 for normal polarity
          }

          return writeSetup (index, 1); // Write the setup for the specific port
        }
        if (isDebug) {
          std::cerr << "Max7311: Invalid channel " << channel << std::endl;
        }
        return false; // Return false if the channel index is invalid
      }
      else {
        if (isDebug) {
          std::cerr << "Max7311: Failed to read setup for channel " << channel << std::endl;
        }
        return false; // Return false if reading setup fails
      }
    }
    else { // set all channels

      if (m & DigitalOutput) {

        regs.config[0] = 0x00; // Set all pins as outputs
        regs.config[1] = 0x00; // Set all pins as outputs
      }
      else  if (m & DigitalInput) {

        regs.config[0] = 0xFF; // Set all pins as inputs
        regs.config[1] = 0xFF; // Set all pins as inputs
      }
      if (m & ActiveLow) {

        regs.polarity[0] = 0xFF; // Set all pins to active low
        regs.polarity[1] = 0xFF; // Set all pins to active low
      }
      else {

        regs.polarity[0] = 0x00; // Set all pins to normal polarity
        regs.polarity[1] = 0x00; // Set all pins to normal polarity
      }

      return writeSetup (); // Write the setup to the device
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // internal
  int
  Max7311::Private::readRegister (uint8_t reg, uint8_t *buffer, uint16_t max) const {

    if (i2c->isOpen() && reg <= TimeoutReg) {
      // Implement I2C read operation here

      max = std::min (max, static_cast<uint16_t> (NofRegisters - reg)); // Limit max to the number of bytes available for reading
      i2c->beginTransmission (addr);
      i2c->write (reg); // Write the register address to read from
      if (i2c->endTransmission (false)) {
        if (i2c->requestFrom (addr, max) >= 0) { // The frame is send now ! > START + ADDR + W + REG + RESTART + ADDR + R + DATA READ + STOP

          uint16_t bytesRead = i2c->available();
          i2c->read (buffer, std::min (bytesRead, max));
          isConnected = true;
          return bytesRead; // Return the number of bytes read
        }
      }
    }
    return -1; // Return -1 on error
  }

  // ---------------------------------------------------------------------------
  // internal
  int
  Max7311::Private::readInputRegister (uint8_t *buffer, uint16_t max) const {

    if (i2c->isOpen()) {
      max = std::min (max, static_cast<uint16_t> (2)); // Limit max to 2 bytes for input registers
      if (i2c->requestFrom (addr, max) >= 0) {
        uint16_t bytesRead = i2c->available();
        i2c->read (buffer, std::min (bytesRead, max));
        isConnected = true;
        return bytesRead;
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  // internal
  int Max7311::Private::writeRegister (uint8_t reg, const uint8_t *data, uint16_t size) {

    if (i2c->isOpen()) {

      size = std::min (size, static_cast<uint16_t> (NofRegisters - reg)); // Limit size to the number of bytes available for writing
      i2c->beginTransmission (addr);
      i2c->write (reg);
      i2c->write (data, size);
      if (i2c->endTransmission ()) { // The frame is send now ! > START + ADDR + W + REG + DATA WRITE + STOP
        isConnected = true;
        return size;
      }
    }
    return -1;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::readInputPorts (uint8_t index, uint8_t nb) const {

    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return readRegister (InputPort1Reg  + index, &regs.input[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::readOutputPorts (uint8_t index, uint8_t nb) const {
    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return readRegister (OutputPort1Reg + index, &regs.output[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::writeOutputPorts (uint8_t index, uint8_t nb) {
    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return writeRegister (OutputPort1Reg + index, &regs.output[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool Max7311::Private::readPolarityInversion (uint8_t index, uint8_t nb) const {
    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return readRegister (Polarity1Reg + index, &regs.polarity[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::writePolarityInversion (uint8_t index, uint8_t nb) {

    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return writeRegister (Polarity1Reg + index, &regs.polarity[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::readConfiguration (uint8_t index, uint8_t nb) const {
    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return readRegister (Config1Reg + index, &regs.config[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::writeConfiguration (uint8_t index, uint8_t nb) {
    if (index >= 0 && index <= 1 && nb > 0 && (index + nb) <= 2) {

      return writeRegister (Config1Reg + index, &regs.config[index], nb) == nb;
    }
    return false;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::readTimeout () const {
    return readRegister (TimeoutReg, &regs.timeout, 1) == 1;
  }

  // ----------------------------------------------------------------------
  // internal
  bool
  Max7311::Private::writeTimeout () {
    return writeRegister (TimeoutReg, &regs.timeout, 1) == 1;
  }

} // namespace Piduino
/* ========================================================================== */
