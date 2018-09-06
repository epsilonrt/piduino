/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * SerialPort is a modified and simplified version of QSerialPort,
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
 *  Ce module permet l'utilisation des ports séries.
 *  @{
 */
namespace Piduino {

  class SerialPort : public FileDevice {
  public:

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

      enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8,
        UnknownDataBits = -1
      };

      enum StopBits {
        OneStop = 1,
        OneAndHalfStop = 3,
        TwoStop = 2,
        UnknownStopBits = -1
      };

      enum Parity {
        NoParity = 'N',
        EvenParity = 'E',
        OddParity = 'O',
        SpaceParity = 'S',
        MarkParity = 'M',
        UnknownParity = -1
      };

      enum FlowControl {
        NoFlowControl = ' ',
        HardwareControl = 'H',
        SoftwareControl = 'S',
        Rs485AfterSendControl = 'R',
        Rs485OnSendControl = 'r',
        UnknownFlowControl = -1
      };

      enum Direction  {
        NoDirection = 0,
        Input = 1,
        Output = 2,
        AllDirections = Input | Output
      };
      typedef Flags<Direction> Directions;

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
       * @brief
       */
      class Settings {
        public:
          Settings (int32_t baudRate = Baud9600, DataBits dataBits = Data8,
                    Parity parity = NoParity, StopBits stopBits = OneStop,
                    FlowControl flowControl = NoFlowControl);
          bool operator== (const Settings & other);

          std::string toString() const;
          std::string dataBitsString() const;
          std::string parityString() const;
          std::string stopBitsString() const;
          std::string flowControlString() const;

          friend std::ostream& operator<< (std::ostream& os, const Settings & s);
          static std::string dataBitsToString (DataBits dataBits);
          static std::string parityToString (Parity parity);
          static std::string stopBitsToString (StopBits stopBits);
          static std::string flowControlToString (FlowControl flowControl);

          int32_t inputBaudRate;
          int32_t outputBaudRate;
          DataBits dataBits;
          Parity parity;
          StopBits stopBits;
          FlowControl flowControl;
      };
      
      /**
       * @class Info
       * @brief 
       */
      class Info {

        public:
          Info();
          Info (const Info & other);
          explicit Info (const std::string & name);
          explicit Info (const SerialPort & port);
          virtual ~Info();

          Info& operator= (const Info &other);
          void swap (Info &other);
          bool operator== (const Info & other);

          std::string portName() const;
          std::string systemLocation() const;
          std::string driverName() const;
          std::string manufacturer() const;
          std::string serialNumber() const;
          uint16_t vendorIdentifier() const;
          bool hasVendorIdentifier() const;
          uint16_t productIdentifier() const;
          bool hasProductIdentifier() const;
          bool isNull() const;

          /**
           * @brief Liste des ports disponibles sur le système
           * Dépend du modèle de carte (informations stockées dans la base de données)
           */
          static std::deque<Info> availablePorts ();

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
       * @brief Constructeur par défaut
       */
      SerialPort ();
      SerialPort (const Info & info);
      SerialPort (const std::string & path);

      /**
       * @brief Destructeur
       */
      virtual ~SerialPort();
      /**
       * @brief Discards data
       * Discards data written to the port but not transmitted, and/or data
       * received but not read, depending on the value of directions
       * @param directions
       */
      void discard (Directions directions);

      void setPort (const Info & info);
      void setPortName (const std::string & name);

      std::string portName() const;

      bool setSettings (const Settings & settings);
      Settings settings() const;

      bool setBaudRate (int32_t baudRate, Directions directions = AllDirections);
      int32_t baudRate (Directions directions = AllDirections) const;

      bool setDataBits (DataBits dataBits);
      DataBits dataBits() const;

      bool setParity (Parity parity);
      Parity parity() const;

      bool setStopBits (StopBits stopBits);
      StopBits stopBits() const;

      bool setFlowControl (FlowControl flowControl);
      FlowControl flowControl() const;

      bool setDataTerminalReady (bool set);
      bool isDataTerminalReady();

      bool setRequestToSend (bool set);
      bool isRequestToSend();

      PinoutSignals pinoutSignals();

      bool sendBreak (int duration = 0);
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
#endif /*PIDUINO_SERIALPORT_H defined */
