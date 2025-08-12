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
#include "max1161x_p.h"
#include "config.h"


namespace Piduino {

  /**
     @brief Max1161x Constants for registers and configurations.
  */
  namespace Max1161xRegs {

    /* Setup Byte defined as follows
      Bit | Name      | Description
      7   | REG       | Register bit | 1 == setup byte, 0 == config byte
      6   | SEL2      | --
      5   | SEL1      | Select the reference for the chip, defaults to 000 at power up, see defines or docs for more info
      4   | SEL0      | --
      3   | CLK       | 1 = external clock, 0 = internal clock, defaults to 0
      2   | BIP/UNI   | 1 = bipolar, 0 = unipolar, defaults to 0
      1   | RST       | 1 = no action, 0 = resets configuration register to default, but setup register is unchanged
      0   | No Care   | Dont Care, set it to whatever youd like
    */
    const uint8_t REG = BIT (7); // Register bit, 1 for setup byte, 0 for config byte
    const uint8_t SEL2 = BIT (6); // Select bit 2 for reference
    const uint8_t SEL1 = BIT (5); // Select bit 1 for reference
    const uint8_t SEL0 = BIT (4); // Select bit 0 for reference
    const uint8_t CLK = BIT (3); // Clock bit, 1 for external
    const uint8_t BIP_UNI = BIT (2); // Bipolar/Unipolar bit, 1 for bipolar, 0 for unipolar
    const uint8_t RST = BIT (1); // Reset bit, 1 for no action, 0 for reset


    // | SEL2 | SEL1 | SEL0 |     Ref Voltage    | AIN_/REF 12-13,16-17 |     REF 14-15    |  Ref State |
    // |:----:|:----:|:----:|:------------------:|:--------------------:|:----------------:|:----------:|
    // |   0  |   0  |   X  |         VDD        |     Analog input     |   Not connected  | Always off |
    // |   0  |   1  |   X  | External reference |    Reference input   |  Reference input | Always off |
    // |   1  |   0  |   0  | Internal reference |     Analog input     |   Not connected  | Always off |
    // |   1  |   0  |   1  | Internal reference |     Analog input     |   Not connected  |  Always on |
    // |   1  |   1  |   0  | Internal reference |   Reference output   | Reference output | Always off |
    // |   1  |   1  |   1  | Internal reference |   Reference output   | Reference output |  Always on |
    const uint8_t RefMask = SEL2 | SEL1 | SEL0; // Mask for reference bits
    const uint8_t RefVdd = 0; // Vdd reference
    const uint8_t RefExt = SEL1; // External reference
    const uint8_t RefInt1 = SEL2; // Internal reference 1
    const uint8_t RefInt2 = SEL2 | SEL0; // Internal reference 2
    const uint8_t RefInt3 = SEL2 | SEL1; // Internal reference 3
    const uint8_t RefInt4 = SEL2 | SEL1 | SEL0; // Internal reference 4
    const uint8_t RefInt = RefInt4; // Default to Internal reference

    /* Config Byte defined as follows

      Bit | Name      | Description
      7   | REG       | Register bit | 1 == setup byte, 0 == config byte
      6   | SCAN1     | Scan bits, see defines or docs for description
      5   | SCAN0     | --
      4   | CS3       | --
      3   | CS2       | Channel select pins
      2   | CS1       | See defines or docs for description
      1   | CS0       |--
      0   | SGL\DIF   | 1 == single ended, 0 == differential
    */
    const uint8_t SCAN1 = BIT (6); // Scan bit 1
    const uint8_t SCAN0 = BIT (5); // Scan bit 2
    const uint8_t CS3 = BIT (4); // Channel select bit 3
    const uint8_t CS2 = BIT (3); // Channel select bit 2
    const uint8_t CS1 = BIT (2); // Channel select bit 1
    const uint8_t CS0 = BIT (1); // Channel select bit 0
    const uint8_t SGL = BIT (0); // Single-ended, 1 for single-ended, 0 for differential

    // Table 5. Scanning Configuration
    // | SCAN1 | SCAN0 | SCANNING CONFIGURATION
    // |:-----:|:-----:|----------------------------------------------------------------------------------------------------------------
    // |   0   |   0   | Scans up from AIN0 to the input selected by CS3–CS0. When CS3–CS0 exceeds 1011, the scanning stops at AIN11.
    // |       |       | When AIN_/REF is set to be a REF input/output, scanning stops at AIN2 or AIN10.
    // |   0   |   1   | *Converts the input selected by CS3–CS0 eight times (see Tables 3 and 4)
    // |   1   |   0   | MAX11612/MAX11613: Scans upper half of channels.
    // |       |       |  Scans up from AIN2 to the input selected by CS1 and CS0. When CS1 and CS0 are set for AIN0, AIN1, and AIN2,
    // |       |       |  the only scan that takes place is AIN2 (MAX11612/MAX11613). When AIN/REF is set to be a REF input/output,
    // |       |       |  scanning stops at AIN2.
    // |       |       | MAX11614/MAX11615: Scans upper quartile of channels.
    // |       |       |  Scans up from AIN6 to the input selected by CS3–CS0. When CS3–CS0 is set for AIN0–AIN6,
    // |       |       |  the only scan that takes place is AIN6 (MAX11614/MAX11615).
    // |       |       | MAX11616/MAX11617: Scans upper half of channels.
    // |       |       |  Scans up from AIN6 to the input selected by CS3–CS0. When CS3–CS0 is set for AIN0–AIN6,
    // |       |       |  the only scan that takes place is AIN6 (MAX11616/MAX11617). When AIN/REF is set to be a REF input/output,
    // |       |       |  scanning stops at selected channel or AIN10.
    // |   1   |   1   | *Converts channel selected by CS3–CS0.
    // *When operating in external clock mode, there is no difference between SCAN[1:0] = 01 and SCAN[1:0] = 11, and converting occurs
    // perpetually until not-acknowledge occurs

    const uint8_t ScanToPin = 0x00; // Scan from AIN0 to the input selected by CS3-CS0
    const uint8_t ScanRepeat = SCAN0; // Scan the input selected by CS3-CS0 eight times
    const uint8_t ScanSubset = SCAN1; // Scan the upper half of channels (MAX11612/MAX11613) or upper quartile of channels (MAX11614/MAX11615/MAX11616/MAX11617)
    const uint8_t ScanOff = SCAN1 | SCAN0; // Convert the channel selected by CS3-CS0
    const uint8_t ScanMask = SCAN1 | SCAN0; // Mask for scan bits
    const uint8_t ChannelMask = CS3 | CS2 | CS1 | CS0; // Mask for channel select bits

    // -----------------------------------------------------------------------------
    static inline uint8_t
    config (uint8_t channel, bool differential, uint8_t scan = ScanOff) {
      uint8_t result;

      result = (scan & ScanMask) | ( (channel << 1) & ChannelMask) | (differential ? 0 : SGL);
      return result;
    }

  } // namespace Max1161xRegs

  using namespace Max1161xRegs;

  // -----------------------------------------------------------------------------
  //
  //                             Max1161x Class
  //
  // -----------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  Max1161x::Max1161x (Max1161x::Private &dd) : Converter (dd) {}

  // ---------------------------------------------------------------------------
  Max1161x::Max1161x (int busId, MaxIndex maxId, int referenceId, double fsr, bool bipolar) :
    Converter (*new Private (this, I2cDev::factory (busId), maxId, referenceId, fsr, bipolar)) {}

  // ---------------------------------------------------------------------------
  Max1161x::Max1161x (const std::string &parameters) :
    Converter (*new Private (this, parameters)) {}

  // ---------------------------------------------------------------------------
  Max1161x::~Max1161x() = default;

  // ---------------------------------------------------------------------------
  const Max1161x::Info &Max1161x::info() const {
    PIMP_D (const Max1161x);

    return d->max;
  }

  // ---------------------------------------------------------------------------
  // Register the Max1161x converter with the factory
  REGISTER_CONVERTER (Max1161x, "adc", "bus=id:max={12,13,14,15,16,17}:ref={ext,vdd,int1,int2,int3,int4}:fsr=value:bipolar={1,0}:clk={int,ext}");

  // -----------------------------------------------------------------------------
  //
  //                         Max1161x::Private Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Max1161x::Private::Private (Max1161x *q, std::shared_ptr<I2cDev> dev, MaxIndex maxId, int referenceId, double fsr, bool bipolar) :
    Converter::Private (q, AnalogToDigital, hasReference | hasResolution | hasBipolar | hasRange | hasSetReference | hasSetBipolar | hasClockSelection),
    i2c (dev), max (maxId, i2c->bus().id()), bipolar (bipolar), isConnected (false), clkSetting (InternalClock) {

    setReference (referenceId, fsr); // Set the reference voltage and full-scale range
  }

  // ---------------------------------------------------------------------------
  //   The parameters for the constructor registered are:
  // - `bus=id` : The I2C bus ID (default is I2cDev::defaultBus().id(), use pinfo to get the default bus ID).
  // - `max={12,13,14,15,16,17}` : The Max1161x model to use, which can be 12, 13, 14, 15, 16, or 17 (default is 15 for Max11615).
  // - `ref={ext,vdd,int1,int2,int3,int4}` : The reference voltage to use (internal, external, or VDD) (default is `vdd`).
  // - `fsr=value` : The full-scale range value. It  must be specified for external reference.
  //                 This value is automatically set based on the reference voltage for internal and VDD, but may be changed.
  // - `bipolar={true|1,false|0}` : If true, enables bipolar mode (default is false).
  // - `clk={int,ext}` : The clock setting, either internal or external (default is internal).
  Max1161x::Private::Private (Max1161x *q, const std::string &params) :
    Converter::Private (q, AnalogToDigital, hasReference | hasResolution | hasBipolar | hasRange | hasSetReference | hasSetBipolar | hasClockSelection, params),
    i2c (I2cDev::factory (I2cDev::Info::defaultBus().id())), max (Max11615, i2c->bus().id()), bipolar (false), isConnected (false), clkSetting (InternalClock) {
    std::map<std::string, std::string> paramsMap = parseParameters (parameters);
    int refId = DefaultReference;
    double fsr = 0.0;

    auto it = paramsMap.find ("bus");
    if (it != paramsMap.end()) {

      int busId = it->second.empty() ? 0 : std::stoi (it->second);
      i2c->setBus (busId); // Set the I2C bus ID, default to 0 if not specified
      max.busId = i2c->bus().id(); // Update the bus ID in the Max1161x info
    }

    it = paramsMap.find ("max");
    if (it != paramsMap.end()) {

      int id = it->second.empty() ? Max11615 : std::stoi (it->second);
      max.setId (static_cast<MaxIndex> (id)); // Set the Max1161x model ID, Max11615 if id is invalid
    }

    it = paramsMap.find ("fsr");
    if (it != paramsMap.end()) {
      double fsrValue = std::stod (it->second);
      if (fsrValue > 0.0) {

        fsr = fsrValue; // Set the full-scale range value
      }
      else {
        // Log a warning if the full-scale range is invalid
        std::cerr << "Max1161x: Invalid full-scale range specified: " << fsrValue << ". Using default value of " << fsr << std::endl;
      }
    }

    it = paramsMap.find ("ref");
    if (it != paramsMap.end()) {
      std::string ref = it->second;
      static const std::map<std::string, int> referenceMap = {
        {"int", InternalReference},
        {"ext", ExternalReference},
        {"vdd", VddReference},
        {"default", DefaultReference},
        {"int1", Internal1Reference},
        {"int2", Internal2Reference},
        {"int3", Internal3Reference},
        {"int4", Internal4Reference}
      };

      auto refIt = referenceMap.find (ref);
      if (refIt != referenceMap.end()) {

        refId = refIt->second;
      }
      else {
        // Log a warning if the reference is invalid
        std::cerr << "Max1161x: Invalid reference specified: " << ref << ". Using default value of Vdd." << std::endl;
      }
    }

    if (!setReference (refId, fsr)) {
      // If setting the reference fails, log an error
      std::cerr << "Max1161x: Failed to set reference voltage." << std::endl;
    }

    it = paramsMap.find ("bipolar");
    if (it != paramsMap.end()) {
      std::string bipolarStr = it->second;
      if (bipolarStr == "true" || bipolarStr == "1") {
        bipolar = true; // Enable bipolar mode
      }
    }

    it = paramsMap.find ("clk");
    if (it != paramsMap.end()) {
      std::string clk = it->second;
      if (clk == "ext") {
        clkSetting = ExternalClock; // Set clock to external
      }
    }
  }

  // ---------------------------------------------------------------------------
  Max1161x::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // override
  bool
  Max1161x::Private::setReference (int referenceId, double fsr) {

    switch (referenceId) {

      case DefaultReference:
      case VddReference:
        this->fsr = max.fsr;
        break;

      case InternalReference:
      case Internal1Reference:
      case Internal2Reference:
      case Internal3Reference:
      case Internal4Reference:
        this->fsr = max.intref; // Use the internal reference voltage value
        break;

      case ExternalReference:
        if (fsr <= 0.0) {
          if (isDebug) {
            std::cerr << "Max1161x::Private::setReference: Invalid external reference value " << fsr << std::endl;
          }
          return false; // Invalid external reference value
        }
        break;

      default:
        if (isDebug) {
          std::cerr << "Max1161x::Private::setReference: Unsupported reference ID " << referenceId << std::endl;
        }
        return false; // Unsupported reference ID
    }

    this->referenceId = referenceId; // Set the reference ID
    if (fsr > 0.0) {
      this->fsr = fsr; // Set the user full-scale range value
    }
    if (isOpen()) {
      return updateSetup(); // Update the setup after changing reference
    }
    return true; // Return true if the setup was updated successfully
  }

  // ---------------------------------------------------------------------------
  // override
  bool
  Max1161x::Private::open (OpenMode mode) {

    if (i2c->open (mode)) {
      if (isDebug) {
        std::cout << "Max1161x: Device opened successfully." << std::endl;
      }
      if (updateSetup()) {
        if (isDebug) {
          std::cout << "Max1161x: Setup updated successfully." << std::endl;
        }
        isConnected = true; // Set the connection status to true
        return Converter::Private::open (mode);
      }
      else {
        if (isDebug) {
          std::cerr << "Max1161x: Failed to update setup." << std::endl;
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
        std::cerr << "Max1161x: Failed to open device. Error(" << i2c->error() << ") :" << i2c->errorString() << std::endl;
      }
      isConnected = false; // Set the connection status to false
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // override
  void
  Max1161x::Private::close() {

    // do not close the I2C device, it is shared, std::shared_ptr destruction will handle it
    isConnected = false; // Set the connection status to false
    Converter::Private::close();
  }

  // ---------------------------------------------------------------------------
  // override
  long
  Max1161x::Private::readChannel (int channel, bool differential) {
    long result = InvalidValue; // Default result if read fails

    if (channel >= 0 && channel < max.nchan) {

      if (sendByte (config (channel, differential))) {

        if (getLastConversion (result)) {
          static const long MASK_12 = 0xFFF;         // 12-bit mask

          // When operating in differential mode, the BIP/UNI bit of the set-up byte selects unipolar or bipolar
          // operation. Unipolar mode sets the differential input range from 0 to VREF.
          // A negative differential analog input in unipolar mode causes the digital output code to be zero.
          // Selecting bipolar mode sets the differential input range to ±VREF/2.
          // The digital output code is binary in unipolar mode and two’s complement in bipolar mode.
          // IMPORTANT: In single-ended mode, the BIP/UNI bit is ignored:
          // In single-ended mode, the MAX11612–MAX11617 always operates in unipolar mode irrespective of
          // BIP/UNI. The analog inputs are internally referenced to
          // GND with a full-scale input range from 0 to VREF.

          if (bipolar && differential) {
            // Portable sign extension for 12 bits to long
            static const long SIGN_BIT_12 = 1L << 11;  // Bit 11 = sign bit for 12 bits

            result &= MASK_12;                   // Clean first
            if (result & SIGN_BIT_12) {          // If sign bit is set
              result |= ~MASK_12;                // Portable sign extension
            }
          }
          else {
            // If in unipolar mode, we can return the result directly
            result &= MASK_12; // Mask to ensure we only get the lower 12 bits
          }
          clearError(); // Clear any previous error
          isConnected = true; // Set the connection status to true
        }
      }
    }
    else {

      setError (EINVAL);
      isConnected = false; // Set the connection status to false
      if (isDebug) {
        std::cerr << "Max1161x::Private::readChannel: Invalid channel " << channel << ". Valid range is 0 to " << (max.nchan - 1) << "." << std::endl;
      }
    }
    return result; // Return the default result if read fails
  }


  // ---------------------------------------------------------------------------
  // internal
  bool Max1161x::Private::sendByte (uint8_t data) {

    i2c->beginTransmission (max.addr);
    i2c->write (data);
    if (!i2c->endTransmission()) {

      if (i2c->error()) {

        setError (); // Get errno and set error message
      }
      else {

        setError (EIO); // Set I/O error if no error code is available
      }
      if (isDebug) {

        std::cerr << "Max1161x: Failed to send byte. Error(" << error << ") :" << errorString << std::endl;
      }
      isConnected = false; // Set the connection status to false
      return false;
    }
    isConnected = true; // Set the connection status to true
    return true;
  }

  // ---------------------------------------------------------------------------
  // internal
  bool Max1161x::Private::getLastConversion (long &conversion) {

    if (i2c->requestFrom (max.addr, 2) == 2) {
      // Read two bytes from the device
      int b1 = i2c->read();
      int b2 = i2c->read();
      conversion = ( (b1 & 0x0F) << 8) | b2; // Combine the two bytes into a 12-bit value
      isConnected = true; // Set the connection status to true
      return true;
    }
    else {

      if (i2c->error()) {
        setError (); // Get errno and set error message

      }
      else {

        setError (EIO); // Set I/O error if no error code is available
      }

      if (isDebug) {
        std::cerr << "Max1161x: Failed to read last conversion. Error(" << error << ") :" << errorString << std::endl;
      }
      isConnected = false; // Set the connection status to false
    }
    return false; // Return false if reading the conversion fails
  }

  // ---------------------------------------------------------------------------
  // internal
  bool Max1161x::Private::updateSetup () {
    bool result = false; // Default result if setup update fails

    static const std::map<int, uint8_t> referenceMap = {
      { DefaultReference, RefVdd }, // Default reference, same as Vdd
      { InternalReference, RefInt }, // Internal reference 1
      { VddReference, RefVdd }, // Vdd reference
      { ExternalReference, RefExt }, // External reference
      { Internal1Reference, RefInt1 }, // Internal reference 1
      { Internal2Reference, RefInt2 }, // Internal reference 2
      { Internal3Reference, RefInt3 }, // Internal reference 3
      { Internal4Reference, RefInt4 }, // Internal reference 4
    };

    uint8_t setupByte = REG | RST; // Start with REG=1 and RST=1

    auto it = referenceMap.find (referenceId);
    if (it != referenceMap.end()) {

      setupByte |= it->second; // Set the reference bits based on the referenceId
    }

    if (bipolar) {

      setupByte |= BIP_UNI; // Set bipolar bit if bipolar mode is enabled
    }

    if (clkSetting == ExternalClock) {

      setupByte |= CLK; // Set clock bit for external clock
    }

    result = sendByte (setupByte); // Send the setup byte to the device
    clk.delayMicroseconds (100); // Wait for 100 microseconds after sending the setup byte
    return result;
  }

  // -----------------------------------------------------------------------------
  //
  //                         Max1161x::Info Class
  //
  // -----------------------------------------------------------------------------
  Max1161x::Info::Info (MaxIndex id, int busId) : busId (busId) {

    setId (id); // Initialize the Info structure with the specified MaxIndex
  }

  // -----------------------------------------------------------------------------
  Max1161x::Info::Info (MaxIndex id, int addr, int nchan, double intref, double fsr)
    : id (id), addr (addr), nchan (nchan), intref (intref), fsr (fsr), busId (I2cDev::Info::defaultBus().id()) {}

  // ------------------------------------------------------------------------------------------------------------------
  // |          | Addr. | Chan SE | Chan Dif | DefaultReference | VccReference | InternalReference | ExternalReference |
  // |:--------:|:-----:|:-------:|:--------:|:----------------:|:------------:|:-----------------:|:-----------------:|
  // | Max11612 |  0x34 |    4    |     2    |   VccReference   |       5      |       4,096       |       5 max       |
  // | Max11613 |  0x34 |    4    |     2    |   VccReference   |      3,3     |       2,048       |      3,3 max      |
  // | Max11614 |  0x33 |    8    |     4    |   VccReference   |       5      |       4,096       |       5 max       |
  // | Max11615 |  0x33 |    8    |     4    |   VccReference   |      3,3     |       2,048       |      3,3 max      |
  // | Max11616 |  0x35 |    12   |     6    |   VccReference   |       5      |       4,096       |       5 max       |
  // | Max11617 |  0x35 |    12   |     6    |   VccReference   |      3,3     |       2,048       |      3,3 max      |
  void Max1161x::Info::setId (MaxIndex id) {
    static const std::map<MaxIndex, Info> maxInfoMap = {
      { Max11612, { Max11612, 0x34,  4, 4.096, 5.0 } },
      { Max11613, { Max11613, 0x34,  4, 2.048, 3.3 } },
      { Max11614, { Max11614, 0x33,  8, 4.096, 5.0 } },
      { Max11615, { Max11615, 0x33,  8, 2.048, 3.3 } },
      { Max11616, { Max11616, 0x35, 12, 4.096, 5.0 } },
      { Max11617, { Max11617, 0x35, 12, 2.048, 3.3 } }
    };

    int bid = this->busId; // Use the busId from the Info structure

    auto it = maxInfoMap.find (id);
    if (it == maxInfoMap.end()) {
      std::cerr << "Max1161x::Info: Invalid MaxIndex specified: " << id << ". Using default value of Max11615." << std::endl;
      id = Max11615; // Default to Max11615 if the ID is invalid
    }

    it = maxInfoMap.find (id);
    *this = it->second; // Set the Info structure to the values corresponding to the specified MaxIndex
    this->busId = bid; // Restore the busId from the Info structure
  }
} // namespace Piduino
/* ========================================================================== */
