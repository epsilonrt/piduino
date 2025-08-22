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

#include <piduino/mcp4728.h>
#include "converter_p.h"

namespace Piduino {

  /**
     @class Mcp4728::Private
     @brief Private implementation class for Mcp4728.

     This class provides the private implementation for the Mcp4728 device, inheriting from Converter::Private.
     It manages the communication with the MAX7311 I2C I/O expander, providing methods for opening, closing,
     reading, writing, and configuring the device. The class supports multiple channels and exposes device-specific
     properties such as resolution, range, and operational modes.

     @note This class is intended for internal use within the Mcp4728 implementation and should not be used directly.
  */
  class Mcp4728::Private  : public Converter::Private {

    public:
      /**
         @brief Constructs a Private implementation object for Mcp4728.
         @param q Pointer to the parent Mcp4728 object.
         @param dev Shared pointer to the I2C device used for communication.
         @param address I2C address of the MAX7311 device.
      */
      Private (Mcp4728 *q, std::shared_ptr<I2cDev> dev, int address);

      /**
         @brief Constructs a Private implementation object for Mcp4728 using parameter string.
         @param q Pointer to the parent Mcp4728 object.
         @param parameters String containing initialization parameters.
      */
      Private (Mcp4728 *q, const std::string &parameters);

      /**
         @brief Destructor for the Private implementation object.
      */
      ~Private() = default;

      /**
          @brief Returns the name of the device.
          @return The device name as a string, the same as Mcp4728::registeredName().
      */
      virtual const std::string &deviceName() const override {
        static const std::string name = Mcp4728::registeredName();
        return name;
      }

      /**
         @brief Opens the converter device with the specified mode.
         @param mode The mode in which to open the device.
         @return True if the device was successfully opened, false otherwise.
      */
      virtual bool open (OpenMode mode) override;

      /**
         @brief Closes the converter device.
      */
      virtual void close() override;

      /**
         @brief Reads a value from the default channel of the converter device.
         @return The value read from the default channel (Channel 0).
      */
      virtual long read () override;

      /**
         @brief Reads a value from the converter device for a specific channel.
         @param channel The channel to read from.
         @param differential If true, reads in differential mode (default is false).
         @return The value read from the device
      */
      virtual long readChannel (int channel = 0, bool differential = false) override;

      /**
         @brief Writes a value to all channels of the converter device.
         @param value The value to write, this value will be clamped to the valid range.
         @return True if the value was successfully written, false otherwise.
      */
      virtual bool write (long value) override;

      /**
         @brief Writes a value to the converter device for a specific channel.
         @param value The value to write, this value will be clamped to the valid range.
         @param channel The channel to write to. -1 to all channels
         @param noUpdate If true, prevents updating the channel (default is false).
         @return True if the value was successfully written, false otherwise.
      */
      virtual bool writeChannel (long value, int channel = -1, bool noUpdate = false) override;


      /**
         @brief Returns the current mode of a channel.
         @param channel The channel number (default is 0).
         @return The current mode of the channel.
      */
      virtual Mode mode (int channel = 0) const override;

      /**
         @brief Sets the mode of the converter.
         @param m The mode to set.
         @param channel The channel number (default is -1, indicating all channels).
         @return True if the mode was successfully set, false otherwise. The default implementation returns false.
      */
      virtual bool setMode (Mode m, int channel = -1) override;

      /**
         @brief Returns the number of channels supported by the converter.
         @return The number of channels, a channel is numbering from 0 to numberOfChannels() - 1.
      */
      virtual int numberOfChannels() const override {
        return NofChannels; // Default implementation returns 4, indicating 4 channels
      }

      /**
         @brief Checks if the converter is enabled.
         @return true if enabled, false otherwise.
      */
      virtual bool isEnabled () const override {
        return isConnected;
      }

      /**
         @brief Gets the digital range of the converter.
         @return The range value in LSB, e.g. max() - min() + 1.
      */
      virtual long range() const {
        return 1 << 12; // Default implementation returns 4096, indicating a 12-bit range
      }

      /**
         @brief Returns the maximum value supported by the converter.
         @param differential If true, returns the maximum value for differential mode.
         @return The maximum value.
      */
      virtual long max (bool differential = false) const override {
        return range() - 1; // For unipolar, max is the full range minus one
      }

      /**
         @brief Returns the minimum value supported by the converter.
         @param differential If true, returns the minimum value for differential mode.
         @return The minimum value.
      */
      virtual long min (bool differential = false) const override {
        return 0; // For unipolar, min is always 0
      }

      /**
         @brief Returns the resolution of the converter in bits.
         @return The resolution in bits.
      */
      virtual int resolution() const override {
        return 12;
      }

      /**
        @brief Gets the current full-scale range of the converter.

        This function is used by default by \c valueToDigital() and \c digitalToValue() to calculate the appropriate scaling factors.

        @return The full-scale range value, typically in volts but may vary depending on the converter model.
      */
      virtual double fullScaleRange (int channel = 0) const override {
        return chan.at (channel).fsr; // Returns the full-scale range for the specified channel, throws if out of bounds
      }

      /**
        @brief Sets the full-scale range of the converter.
        @param fsr The desired full-scale range value.
        @return True if the full-scale range was successfully set, false otherwise.
      */
      virtual bool setFullScaleRange (double fsr, int channel = -1) override;


      /**
         @brief Set the reference ID for a channel.
         @param referenceId The ID of the reference to set.
         @param fsr The full-scale range value, used for external reference (default is 0.0).
         @param channel The channel number to set the reference for (default is -1, indicating all channels).
         @return True if the reference  was successfully set, false otherwise.
      */
      virtual bool setReference (int referenceId, double fsr = 0.0, int channel = -1) override;

      /**
         @brief Gets the current reference ID of the converter.
         @return The ID reference of the reference voltage, which can be a predefined constant or a custom value depending on the converter model.
      */
      virtual int reference (int channel = 0) const override {

        return chan.at (channel).refId;
      }

      // ----------------------- specific methods -----------------------
      // unable to call this methods whith a Converter object
      virtual bool fastWrite (const uint16_t values[4]);
      virtual bool fastWrite (uint16_t value);

      // ----------------------- types and constants -----------------------
      static constexpr uint8_t NofChannels = 4; ///< Number of channels
      static constexpr long DefaultMode = NormalMode | AnalogOutput | Gain1;

      static constexpr uint8_t RegFastWriteLength = NofChannels * 2; ///< Length of the register fast write buffer

      /**
         @brief Structure representing a channel in the Mcp4728 device.
      */
      struct Channel {
        double fsr; ///< Full-scale range value for the channel
        uint16_t value; ///< Current value of the DAC output for the channel
        uint16_t eeprom; ///< Current value stored in EEPROM for the channel
        Converter::Mode mode; ///< Current mode of the converter for the channel
        int refId; ///< Reference ID for the channel
        bool modeSetByUser; ///< Flag to indicate if the channel was set by the user
        bool refSetByUser; ///< Flag to indicate if the reference was set by the user
        bool isReady; ///< Flag to indicate if the channel is ready
        Channel() :
          fsr (DefaultFullScaleRange), value (0), eeprom (0),
          mode (DefaultMode), refId (DefaultReferenceId),
          modeSetByUser (false), refSetByUser (false), isReady (false) {}
          void setReference (int referenceId, double fsr = 0.0);
      };

      /**
         @brief Structure representing a channel buffer in the Mcp4728 device.

         This structure holds the command and data for a specific channel transfer.
      */
      struct ChannelBuffer {


        /**
           @brief Command byte for the channel.

           Format for writing :
           [             cmd            ]
           B7 B6 B5 B4 B3 B2   B1   B0
           C2 C1 C0 W1 W0 DAC1 DAC0 UDAC

           Format for reading :
           [             cmd            ]
           B7  B6  B5   B4   B3 B2 B1 B0
           RDY POR DAC1 DAC0 0  A2 A1 A0
        */
        uint8_t cmd;

        /**
           @brief Data bytes for the channel.

           The data bytes contain the value and settings for the channel.
           [             data[0]               ][          data[1]            ]
           B15  B14  B13  B12  B11  B10  B9  B8  B7  B6  B5  B4  B3  B2  B1  B0
           VREF PD1  PD0  GX   D11  D10  D9  D8  D7  D6  D5  D4  D3  D2  D1  D0
        */
        uint8_t data[2]; ///< Data bytes for the channel

        // ------------ methods ------------
        /**
           @brief Default constructor.
        */
        ChannelBuffer() : cmd (0), data{0, 0} {}

        /**
           @brief Constructor that initializes the channel buffer with the given channel information.
           @param ch The channel information.
           @param index The channel index.
           @param udac Flag indicating if the channel is a user DAC.
           @param wrCmd The write command (default is 0).
        */
        ChannelBuffer (Channel &ch, uint8_t index, bool udac, uint8_t wrCmd = 0) {

          setBuffer (ch, index, udac, wrCmd);
        }

        /**
           @brief Sets the buffer for the channel.

           Must be called before sending data to the DAC.

           @param ch The channel information.
           @param index The channel index.
           @param udac Flag indicating if the output pin must be updated.
           @param wrCmd The write command (default is 0).
        */
        void setBuffer (Channel &ch, uint8_t index, bool udac, uint8_t wrCmd = 0);

        /**
           @brief Sets the channel information with the buffer read from the DAC.

           Must be called after receiving data from the DAC.

           @param in ch The channel information.
        */
        void setChannel (Channel &ch);

        /**
           @brief Gets the mode of the channel.

           setChannel() must be called before this function.

           @return The mode of the channel.
        */
        Mode mode() const;

        /**
           @brief Gets the reference ID of the channel.

           setChannel() must be called before this function.

           @return The reference ID of the channel.
        */
        int reference() const;

        /**
           @brief Gets the value of the channel.

           setChannel() must be called before this function.

           @return The value of the channel.
        */
        uint16_t value() const;

        /**
           @brief Gets the channel number.

           setChannel() must be called before this function.

           @return The channel number.
        */
        uint8_t channel (bool isRxFrame = false) const;
      };
      static_assert (sizeof (ChannelBuffer) == 3, "ChannelBuffer size mismatch");

      // ------------------------- internal methods -------------------------
      bool readRegisters (ChannelBuffer *buffer) const;
      bool writeRegisters (const uint8_t *buffer, uint16_t length);
      bool writeRegisters (const ChannelBuffer &buffer) {
        return writeRegisters (reinterpret_cast<const uint8_t *> (&buffer), sizeof (ChannelBuffer));
      }
      bool updateSetup();

      // --------------------------- data members ---------------------------
      std::shared_ptr<I2cDev> i2c; ///< I2C device pointer
      uint16_t addr; ///< I2C slave address
      mutable bool isConnected; ///< Connection status flag, updated after each I2C transaction.
      bool modeSetByUser; ///< Flag to indicate if the mode was set by the user

      std::array<Channel, NofChannels> chan; ///< Array of channel information

      PIMP_DECLARE_PUBLIC (Mcp4728)
  };
} // namespace Piduino
/* ========================================================================== */
