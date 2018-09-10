/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * SerialPort is a modified and simplified version of SerialPort,
 * from Qt according to the LGPL and Copyright (C) 2015 The Qt Company Ltd.
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
 */
#ifndef PIDUINO_SERIALPORT_H
#define PIDUINO_SERIALPORT_H

#include <deque>
#include <string>
#include <piduino/filedevice.h>

/**
 *  @defgroup piduino_serialport Serial port
 *
 *  Provides objects to access serial ports.
 *  @{
 */
namespace Piduino {

  /**
   * @class SerialPort
   * @brief Provides functions to access serial ports.

      You can get information about the available serial ports using the
      SerialPort::Info helper class, which allows an enumeration of all the serial
      ports in the system. This is useful to obtain the correct name of the
      serial port you want to use. You can pass an object
      of the helper class as an argument to the setPort() or setPortName()
      methods to assign the desired serial device.

      After setting the port, you can open it in read-only (r/o), write-only
      (w/o), or read-write (r/w) mode using the open() method.

      @note The serial port is always opened with exclusive access
      (that is, no other process or thread can access an already opened serial port).

      Use the close() method to close the port and cancel the I/O operations.

      Having successfully opened, SerialPort tries to determine the current
      configuration of the port and initializes itself. You can reconfigure the
      port to the desired setting using the setBaudRate(), setDataBits(),
      setParity(), setStopBits(), and setFlowControl() methods.

      There are a couple of properties to work with the pinout signals namely:
      SerialPort::dataTerminalReady, SerialPort::requestToSend. It is also
      possible to use the pinoutSignals() method to query the current pinout
      signals set.

      Once you know that the ports are ready to read or write, you can
      use the read() or write() methods.

      SerialPort provides a set of functions that suspend the
      calling thread until certain signals are emitted. These functions
      can be used to implement blocking serial ports:

      @list

      @li waitForReadyRead() blocks calls until new data is available for
      reading.

      @li waitForBytesWritten() blocks calls until one payload of data has
      been written to the serial port.

      @endlist
   */
  class SerialPort : public FileDevice {
    public:

      /**
       * @enum BaudRate
        This enum describes the baud rate which the communication device operates
        with.

        @note Only the most common standard baud rates are listed in this enum.

        @value Baud1200     1200 baud.
        @value Baud2400     2400 baud.
        @value Baud4800     4800 baud.
        @value Baud9600     9600 baud.
        @value Baud19200    19200 baud.
        @value Baud38400    38400 baud.
        @value Baud57600    57600 baud.
        @value Baud115200   115200 baud.
        @value UnknownBaud  Unknown baud. This value is obsolete. It is provided to
                            keep old source code working. We strongly advise against
                            using it in new code.

        @sa SerialPort::baudRate
       */
      enum BaudRate {
        Baud1200 = 1200,
        Baud2400 = 2400,
        Baud4800 = 4800,
        Baud9600 = 9600,
        Baud19200 = 19200,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200,
        UnknownBaud = -1
      };

      /**
       * @enum DataBits

        This enum describes the number of data bits used.

        @value Data5            The number of data bits in each character is 5. It
                                is used for Baudot code. It generally only makes
                                sense with older equipment such as teleprinters.
        @value Data6            The number of data bits in each character is 6. It
                                is rarely used.
        @value Data7            The number of data bits in each character is 7. It
                                is used for true ASCII. It generally only makes
                                sense with older equipment such as teleprinters.
        @value Data8            The number of data bits in each character is 8. It
                                is used for most kinds of data, as this size matches
                                the size of a byte. It is almost universally used in
                                newer applications.
        @value UnknownDataBits  Unknown number of bits. This value is obsolete. It
                                is provided to keep old source code working. We
                                strongly advise against using it in new code.

        @sa SerialPort::dataBits
       */
      enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8,
        UnknownDataBits = -1
      };

      /**
       * @enum

        This enum describes the number of stop bits used.

        @value OneStop          1 stop bit.
        @value TwoStop          2 stop bits.
        @value UnknownStopBits  Unknown number of stop bits. This value is obsolete.
                                It is provided to keep old source code working. We
                                strongly advise against using it in new code.

        @sa SerialPort::stopBits
       */
      enum StopBits {
        OneStop = 1,
        TwoStop = 2,
        UnknownStopBits = -1
      };

      /**
       * @enum Parity

        This enum describes the parity scheme used.

        @value NoParity         No parity bit it sent. This is the most common
                                parity setting. Error detection is handled by the
                                communication protocol.
        @value EvenParity       The number of 1 bits in each character, including
                                the parity bit, is always even.
        @value OddParity        The number of 1 bits in each character, including
                                the parity bit, is always odd. It ensures that at
                                least one state transition occurs in each character.
        @value SpaceParity      Space parity. The parity bit is sent in the space
                                signal condition. It does not provide error
                                detection information.
        @value MarkParity       Mark parity. The parity bit is always set to the
                                mark signal condition (logical 1). It does not
                                provide error detection information.
        @value UnknownParity    Unknown parity. This value is obsolete. It is
                                provided to keep old source code working. We
                                strongly advise against using it in new code.

        @sa SerialPort::parity
       */
      enum Parity {
        NoParity = 'N',
        EvenParity = 'E',
        OddParity = 'O',
        SpaceParity = 'S',
        MarkParity = 'M',
        UnknownParity = -1
      };

      /**
       * @enum

        This enum describes the flow control used.

        @value NoFlowControl          No flow control.
        @value HardwareControl        Hardware flow control (RTS/CTS).
        @value SoftwareControl        Software flow control (XON/XOFF).
        @value Rs485AfterSendControl  RS485 Mode, RTS is used to enable transmission in the logic high state.
        @value Rs485OnSendControl     RS485 Mode, RTS is used to enable transmission in the logic low state.
        @value UnknownFlowControl     Unknown flow control. This value is obsolete. It
                                      is provided to keep old source code working. We
                                      strongly advise against using it in new code.

        @sa SerialPort::flowControl
       */
      enum FlowControl {
        NoFlowControl = ' ',
        HardwareControl = 'H',
        SoftwareControl = 'S',
        Rs485AfterSendControl = 'R',
        Rs485OnSendControl = 'r',
        UnknownFlowControl = -1
      };

      /**
       * @enum Direction
       *
       * This enum describes the possible directions of the data transmission.

          @note This enumeration is used for setting the baud rate of the device
          separately for each direction on some operating systems (for example,
          POSIX-like).

          @value Input            Input direction.
          @value Output           Output direction.
          @value AllDirections    Simultaneously in two directions.
       */
      enum Direction  {
        NoDirection = 0,
        Input = 1,
        Output = 2,
        AllDirections = Input | Output
      };
      typedef Flags<Direction> Directions;

      /**
       * @enum

        This enum describes the possible RS-232 pinout signals.

        @value NoSignal                       No line active
        @value TransmittedDataSignal          TxD (Transmitted Data). This value is
                                              obsolete. It is provided to keep old
                                              source code working. We strongly
                                              advise against using it in new code.
        @value ReceivedDataSignal             RxD (Received Data). This value is
                                              obsolete. It is provided to keep old
                                              source code working. We strongly
                                              advise against using it in new code.
        @value DataTerminalReadySignal        DTR (Data Terminal Ready).
        @value DataCarrierDetectSignal        DCD (Data Carrier Detect).
        @value DataSetReadySignal             DSR (Data Set Ready).
        @value RingIndicatorSignal            RNG (Ring Indicator).
        @value RequestToSendSignal            RTS (Request To Send).
        @value ClearToSendSignal              CTS (Clear To Send).
        @value SecondaryTransmittedDataSignal STD (Secondary Transmitted Data).
        @value SecondaryReceivedDataSignal    SRD (Secondary Received Data).

        @sa pinoutSignals(), SerialPort::dataTerminalReady,
        SerialPort::requestToSend
       */
      enum PinoutSignal {
        NoSignal = 0x00,
        TransmittedDataSignal = 0x01,
        ReceivedDataSignal = 0x02,
        DataTerminalReadySignal = 0x04,
        DataCarrierDetectSignal = 0x08,
        DataSetReadySignal = 0x10,
        RingIndicatorSignal = 0x20,
        RequestToSendSignal = 0x40,
        ClearToSendSignal = 0x80,
        SecondaryTransmittedDataSignal = 0x100,
        SecondaryReceivedDataSignal = 0x200
      };
      typedef Flags<PinoutSignal> PinoutSignals;


      /**
       * @class Settings
       * @brief Provides settings information about serial ports.
       */
      class Settings {
        public:
          /**
           * Constructs a new SerialPort::Settings object.
           *
           * @param baudRate data baud rate (same as input and output)
           * @param dataBits number of data bits used
           * @param parity the parity checking mode
           * @param stopBits the number of stop bits in a frame
           * @param flowControl the desired flow control mode
           */
          Settings (int32_t baudRate = Baud9600, DataBits dataBits = Data8,
                    Parity parity = NoParity, StopBits stopBits = OneStop,
                    FlowControl flowControl = NoFlowControl);

          int32_t inputBaudRate; ///< data baud rate for the desired direction
          int32_t outputBaudRate; ///< data baud rate for the desired direction
          DataBits dataBits; ///< number of data bits used
          Parity parity; ///< the parity checking mode
          StopBits stopBits; ///< the number of stop bits in a frame
          FlowControl flowControl; ///< the desired flow control mode

          /**
           * @brief equality operator
           */
          bool operator== (const Settings & other);

          /**
           * @brief Convert to string
           */
          std::string toString() const;

          /**
           * @brief Print settings to stream
           */
          friend std::ostream& operator<< (std::ostream& os, const Settings & s);

          /**
           * @brief Convert current data bits to string
           */
          std::string dataBitsString() const;

          /**
           * @brief Convert current parity to string
           */
          std::string parityString() const;

          /**
           * @brief Convert current stop bits to string
           */
          std::string stopBitsString() const;

          /**
           * @brief Convert current flow control to string
           */
          std::string flowControlString() const;

          /**
           * @brief Convert data bits to string
           */
          static std::string dataBitsToString (DataBits dataBits);

          /**
           * @brief Convert parity to string
           */
          static std::string parityToString (Parity parity);

          /**
           * @brief Convert stop bits to string
           */
          static std::string stopBitsToString (StopBits stopBits);

          /**
           * @brief Convert flow control to string
           */
          static std::string flowControlToString (FlowControl flowControl);
      };

      /**
       * @class Info
       * @brief Provides information about existing serial ports.

          Use the static functions to generate a list of SerialPort::Info
          objects. Each SerialPort::Info object in the list represents a single
          serial port and can be queried for the port name, system location,
          description, and manufacturer. The SerialPort::Info class can also be
          used as an input parameter for the setPort() method of the SerialPort
          class.

          @sa SerialPort
       */
      class Info {

        public:

          /**
            Constructs an empty SerialPort::Info object.

            @sa isNull()
           */
          Info();

          /**
            Constructs a copy of @a other.
           */
          Info (const Info & other);

          /**
            Constructs a SerialPort::Info object from serial port @a name.

            This constructor finds the relevant serial port among the available ones
            according to the port name @a name, and constructs the serial port info
            instance for that port.
           */
          explicit Info (const std::string & name);

          /**
            Constructs a SerialPort::Info object from serial @a port.
           */
          explicit Info (const SerialPort & port);

          /**
            Destroys the SerialPort::Info object. References to the values in the
            object become invalid.
           */
          virtual ~Info();

          /**
            Sets the SerialPort::Info object to be equal to @a other.
           */
          Info& operator= (const Info &other);

          /**
            Swaps SerialPort::Info @a other with this SerialPort::Info. This operation is
            very fast and never fails.
           */
          void swap (Info &other);

          /**
           * @brief
           * @param other
           */
          bool operator== (const Info & other);
          
          /**
           * @brief
           * @param other
           */
          bool operator!= (const Info & other);

          /**
            Returns the name of the serial port.

            @sa systemLocation()
           */
          std::string portName() const;

          /**
            Returns the system location of the serial port.

            @sa portName()
           */
          std::string systemLocation() const;

          /**
            Returns the driver name string of the serial port,
            if available; otherwise returns an empty string.
           */
          std::string driverName() const;

          /**
            Returns the description string of the serial port,
            if available; otherwise returns an empty string.

            @sa manufacturer(), serialNumber()
           */
          std::string description() const;

          /**
            Returns the manufacturer string of the serial port,
            if available; otherwise returns an empty string.

            @sa description(), serialNumber()
           */
          std::string manufacturer() const;

          /**
            Returns the serial number string of the serial port,
            if available; otherwise returns an empty string.

            @note The serial number may include letters.

            @sa description(), manufacturer()
           */
          std::string serialNumber() const;

          /**
            Returns the 16-bit vendor number for the serial port, if available;
            otherwise returns zero.

            @sa hasVendorIdentifier(), productIdentifier(), hasProductIdentifier()
           */
          uint16_t vendorIdentifier() const;

          /**
            Returns @c true if there is a valid @c 16-bit vendor number present; otherwise
            returns @c false.

            @sa vendorIdentifier(), productIdentifier(), hasProductIdentifier()
           */
          bool hasVendorIdentifier() const;

          /**
            Returns the 16-bit product number for the serial port, if available;
            otherwise returns zero.

            @sa hasProductIdentifier(), vendorIdentifier(), hasVendorIdentifier()
           */
          uint16_t productIdentifier() const;

          /**
            Returns @c true if there is a valid @c 16-bit product number present; otherwise
            returns @c false.

            @sa productIdentifier(), vendorIdentifier(), hasVendorIdentifier()
           */
          bool hasProductIdentifier() const;

          /**
            Returns whether this SerialPort::Info object holds a
            serial port definition.
           */
          bool isNull() const;

          /**
            Returns a list of available serial ports on the system.
           */
          static std::deque<Info> availablePorts ();

          /**
            Returns a list of available standard baud rates supported
            by the target platform.
           */
          static std::deque<int32_t> standardBaudRates();

          /**
           * @brief  Information sur le port série par défaut sur la carte Pi
           * Dépend du modèle de carte (informations stockées dans la base de données)
           * @return informations sur le port qui peut être isNull() si le port
           * n'a pas été trouvé.
           */
          static Info defaultPort ();

        protected:
          class Private;
          Info (Private &dd);
          std::unique_ptr<Private> d_ptr;

        private:
          PIMP_DECLARE_PRIVATE (Info)
      };

      /**
       * Constructs a new serial port object
       */
      SerialPort ();

      /**
       * Constructs a new serial port object to represent the serial port with
       * the specified helper class @a serialPortInfo.
       */
      SerialPort (const Info & serialPortInfo);

      /**
       * Constructs a new serial port object to represent the serial port with
       * the specified @a path.
       *
       * The path should have a specific format; see the FileDevice::setPath() method
       */
      SerialPort (const std::string & path);

      /**
       * @brief Closes the serial port, if necessary, and then destroys object.
       */
      virtual ~SerialPort();

      /**
        Discards all characters from the output or input buffer, depending on
        given directions @a directions. This includes clearing the internal class buffers and
        the UART (driver) buffers. Also terminate pending read or write operations.
        If successful, returns @c true; otherwise returns @c false.

        @note The serial port has to be open before trying to clear any buffered
        data; otherwise returns @c false and sets the NotOpenError error code.
       */
      void clear (Directions directions);

      /**
        Sets the port stored in the serial port info instance @a serialPortInfo.

        @sa portName(), SerialPort::Info
       */
      void setPort (const Info & info);

      /**
        Sets the @a name of the serial port.

        The name of the serial port.

        @sa portName(), SerialPort::Info
       */
      void setPortName (const std::string & name);

      /**
        Returns the name set by setPort() or passed to the SerialPort constructor.
        This name is short, i.e. it is extracted and converted from the internal
        variable system location of the device.

        @sa setPort(), SerialPort::Info::portName()
       */
      std::string portName() const;

      /**
        @property SerialPort::settings
        @brief settings information

        @sa SerialPort::Settings
       */
      bool setSettings (const Settings & settings);
      Settings settings() const;

      /**
        @property SerialPort::baudRate
        @brief the data baud rate for the desired direction

        If the setting is successful or set before opening the port, returns @c true;
        otherwise returns @c false and sets an error code which can be obtained by
        accessing the value of the SerialPort::error property. To set the baud
        rate, use the enumeration SerialPort::BaudRate or any positive int32_t
        value.

        @note If the setting is set before opening the port, the actual serial port
        setting is done automatically in the @l{FileDevice::open()} method right
        after that the opening of the port succeeds.

        The default value is Baud9600, i.e. 9600 bits per second.
       */
      bool setBaudRate (int32_t baudRate, Directions directions = AllDirections);
      int32_t baudRate (Directions directions = AllDirections) const;

      /**
        @property SerialPort::dataBits
        @brief the data bits in a frame

        If the setting is successful or set before opening the port, returns
        @c true; otherwise returns @c false and sets an error code which can be obtained
        by accessing the value of the SerialPort::error property.

        @note If the setting is set before opening the port, the actual serial port
        setting is done automatically in the @l{FileDevice::open()} method right
        after that the opening of the port succeeds.

        The default value is Data8, i.e. 8 data bits.
       */
      bool setDataBits (DataBits dataBits);
      DataBits dataBits() const;

      /**
        @property SerialPort::parity
        @brief the parity checking mode

        If the setting is successful or set before opening the port, returns @c true;
        otherwise returns @c false and sets an error code which can be obtained by
        accessing the value of the SerialPort::error property.

        @note If the setting is set before opening the port, the actual serial port
        setting is done automatically in the @l{FileDevice::open()} method right
        after that the opening of the port succeeds.

        The default value is NoParity, i.e. no parity.
       */
      bool setParity (Parity parity);
      Parity parity() const;

      /**
        @property SerialPort::stopBits
        @brief the number of stop bits in a frame

        If the setting is successful or set before opening the port, returns @c true;
        otherwise returns @c false and sets an error code which can be obtained by
        accessing the value of the SerialPort::error property.

        @note If the setting is set before opening the port, the actual serial port
        setting is done automatically in the @l{FileDevice::open()} method right
        after that the opening of the port succeeds.

        The default value is OneStop, i.e. 1 stop bit.
       */
      bool setStopBits (StopBits stopBits);
      StopBits stopBits() const;

      /**
        @property SerialPort::flowControl
        @brief the desired flow control mode

        If the setting is successful or set before opening the port, returns @c true;
        otherwise returns @c false and sets an error code which can be obtained by
        accessing the value of the SerialPort::error property.

        @note If the setting is set before opening the port, the actual serial port
        setting is done automatically in the @l{FileDevice::open()} method right
        after that the opening of the port succeeds.

        The default value is NoFlowControl, i.e. no flow control.
       */
      bool setFlowControl (FlowControl flowControl);
      FlowControl flowControl() const;

      /**
        @property SerialPort::dataTerminalReady
        @brief the state (high or low) of the line signal DTR

        Returns @c true on success, @c false otherwise.
        If the flag is @c true then the DTR signal is set to high; otherwise low.

        @note The serial port has to be open before trying to set or get this
        property; otherwise @c false is returned and the error code is set to
        NotOpenError.

        @sa pinoutSignals()
       */
      bool setDataTerminalReady (bool set);
      bool isDataTerminalReady();

      /**
        @property SerialPort::requestToSend
        @brief the state (high or low) of the line signal RTS

        Returns @c true on success, @c false otherwise.
        If the flag is @c true then the RTS signal is set to high; otherwise low.

        @note The serial port has to be open before trying to set or get this
        property; otherwise @c false is returned and the error code is set to
        NotOpenError.

        @note An attempt to control the RTS signal in the HardwareControl mode
        will fail with error code set to UnsupportedOperationError, because
        the signal is automatically controlled by the driver.

        @sa pinoutSignals()
       */
      bool setRequestToSend (bool set);
      bool isRequestToSend();

      /**
        Returns the state of the line signals in a bitmap format.

        From this result, it is possible to allocate the state of the
        desired signal by applying a mask "AND", where the mask is
        the desired enumeration value from SerialPort::PinoutSignals.

        @note This method performs a system call, thus ensuring that the line signal
        states are returned properly. This is necessary when the underlying
        operating systems cannot provide proper notifications about the changes.

        @note The serial port has to be open before trying to get the pinout
        signals; otherwise returns NoSignal.

        @sa SerialPort::dataTerminalReady, SerialPort::requestToSend
       */
      PinoutSignals pinoutSignals();

      /**
        Sends a continuous stream of zero bits during a specified period
        of time @a duration in msec if the terminal is using asynchronous
        serial data. If successful, returns @c true; otherwise returns @c false.

        If the duration is zero then zero bits are transmitted by at least
        @c 0.25 seconds, but no more than @c 0.5 seconds.

        If the duration is non zero then zero bits are transmitted within a certain
        period of time depending on the implementation.

        @note The serial port has to be open before trying to send a break
        duration; otherwise returns @c false and sets the NotOpenError error code.

        @sa setBreakEnabled()
       */
      bool sendBreak (int duration = 0);

      /**
        @property SerialPort::breakEnabled
        @brief the state of the transmission line in break

        Returns @c true on success, @c false otherwise.
        If the flag is @c true then the transmission line is in break state;
        otherwise is in non-break state.

        @note The serial port has to be open before trying to set or get this
        property; otherwise returns @c false and sets the NotOpenError error code.
        This is a bit unusual as opposed to the regular Qt property settings of
        a class. However, this is a special use case since the property is set
        through the interaction with the kernel and hardware. Hence, the two
        scenarios cannot be completely compared to each other.
       */
      bool setBreakEnabled (bool set = true);
      bool isBreakEnabled() const;

    protected:
      class Private;
      SerialPort (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (SerialPort)
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /* PIDUINO_SERIALPORT_H defined */
