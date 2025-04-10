/* Copyright © 2018 Pascal JEAN, All rights reserved.
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
#ifndef PIDUINO_DATABASE_H
#define PIDUINO_DATABASE_H

#include <string>
#include <memory>
#include <iostream>
#include <cppdb/frontend.h>
#include <piduino/soc.h>
#include <piduino/system.h>

/**
 *  @defgroup piduino_database Database
 *
 *  Les informations concernant la carte cible sont stockées dans une base de
 *  données.
 *  @{
 */

namespace Piduino {

  class Database : public cppdb::session {

    public:
      class Board {
        public:

          // -------------------------------------------------------------------
          //
          //                      Database::Board::Family Class
          //
          // -------------------------------------------------------------------
          class Family {
            public:

              enum Id {
                RaspberryPi = 0,
                NanoPi,
                OrangePi,
                BananaPi,
                // Add New item here and update board_family table in database !

                //---
                Unknown = -1
              };

              Family (Id i = Unknown) {
                setId (i);
              }
              virtual ~Family() {}

              inline Id id() const {
                return _id;
              }

              inline const std::string &name() const {
                return _name;
              }

              inline const std::string & i2cSysPath() const {
                return _i2c_syspath;
              }

              inline const std::string & spiSysPath() const {
                return _spi_syspath;
              }

              inline const std::string & uartSysPath() const {
                return _uart_syspath;
              }

              void setId (Id i);

            private:
              Id _id;
              std::string _name;
              std::string _i2c_syspath;
              std::string _spi_syspath;
              std::string _uart_syspath;
          };

          // -------------------------------------------------------------------
          //
          //                     Database::Board::Model Class
          //
          // -------------------------------------------------------------------
          class Model {
            public:

              enum Id {
                RaspberryPiA = 0,
                RaspberryPiB,
                RaspberryPiAPlus,
                RaspberryPiBPlus,
                RaspberryPiComputeModule,
                RaspberryPi2B,
                RaspberryPiZero,
                RaspberryPi3B,
                RaspberryPiZeroW,
                RaspberryPiComputeModule3,
                NanoPiNeo,
                NanoPiNeoAir,
                NanoPiM1,
                NanoPiM1Plus,
                NanoPiNeo2,
                NanoPiNeoPlus2,
                NanoPiNeoCore,
                NanoPiNeoCore2,
                NanoPiNeoCoreMiniShield,
                NanoPiNeoCore2MiniShield,
                RaspberryPi3BPlus,
                RaspberryPi3APlus,
                NanoPiDuo2,
                RaspberryPi4B,
                RaspberryPiZero2W,
                RaspberryPiComputeModule3Plus,
                RaspberryPiComputeModule4,
                RaspberryPi5,
                RaspberryPiComputeModule5,
                RaspberryPiComputeModule5Lite,
                RaspberryPi400,
                RaspberryPi500,

                // Add New item here and update board_model table in database !

                //---
                Unknown = -1
              };

              Model (Id i = Unknown) {
                setId (i);
              }
              virtual ~Model() {}

              inline Id id() const {
                return _id;
              }

              inline const std::string & name() const {
                return _name;
              }

              inline const Family & family() const {
                return _family;
              }

              inline const SoC & soc() const {
                return _soc;
              }

              void setId (Id i);

            private:
              Id _id;
              std::string _name;
              Family _family;
              SoC _soc;
          };


          // -------------------------------------------------------------------
          //
          //                      Database::Board Class
          //
          // -------------------------------------------------------------------

          Board (int cpuinfoBoardRevision = -1);
          Board (const std::string & armbianBoardTag);
          virtual ~Board();

          bool probingSystem();
          bool setRevision (int rev);
          bool setTag (const std::string & armbianBoardTag);

          inline long long id() const {
            return _id;
          }

          inline const std::string & name() const {
            return model().name();
          }

          inline const std::string & tag() const {
            return _tag;
          }

          inline int revision() const {
            return _revision;
          }

          inline const Family & family() const {
            return model().family();
          }

          inline const SoC & soc() const {
            return model().soc();
          }

          inline std::string pcbRevision() const {
            return _pcb_revision;
          }

          inline long long gpioId() const {
            return _gpio_id;
          }

          inline const Model & model() const {
            return _model;
          }

          inline const Manufacturer & manufacturer() const {
            return _manufacturer;
          }

          inline unsigned long totalRam() const {
            return _ram;
          }

          inline bool found() const {
            return _found;
          }

          /**
           * @brief Numéro du bus I2C par défaut
           */
          inline int defaultI2cBus() const {
            return _default_i2c_id;
          }

          /**
           * @brief Numéro du bus SPI par défaut
           */
          inline int defaultSpiBus() const {
            return _default_spi_id;
          }

          /**
           * @brief Numéro de l'UART par défaut
           */
          inline int defaultUart() const {
            return _default_uart_id;
          }

          friend std::ostream& operator<< (std::ostream& os, const Board& c);

        private:
          long long _id;
          Model _model;
          Manufacturer _manufacturer;
          long long _gpio_id; // Révision du GPIO
          std::string _pcb_revision;
          int _revision;
          bool _found;
          std::string _tag;
          long _ram; // RAM en Mo
          int _default_i2c_id;
          int _default_spi_id;
          int _default_uart_id;
      };

      // -----------------------------------------------------------------------
      //
      //                      Database Class
      //
      // -----------------------------------------------------------------------
      Database (const std::string & connectionInfo = std::string());
      Database (int cpuinfoBoardRevision, const std::string & connectionInfo = std::string());
      Database (const std::string & armbianBoardTag, const std::string & connectionInfo = std::string());
      virtual ~Database();

      static std::string findConnectionInfo (const std::string & connectionInfo = std::string());

      void setConnectionInfo (const std::string & connectionInfo);

      inline const std::string& connectionInfo() const {
        return _cinfo;
      }

      inline const Board & board() const {
        return *_board;
      }

    private:
      std::shared_ptr<Board> _board;
      std::string _cinfo;
  };

  // ---------------------------------------------------------------------------
  //
  //                      Piduino Database Global Object
  //
  // ---------------------------------------------------------------------------
  extern Database db; ///< Piduino Database Global Object
  /**
   * @}
   */
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*PIDUINO_DATABASE_H defined */
