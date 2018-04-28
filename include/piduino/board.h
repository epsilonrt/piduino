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

#ifndef _PIDUINO_BOARD_H_
#define _PIDUINO_BOARD_H_

#include <piduino/soc.h>

/**
 *  @defgroup piduino_board Board
 *
 *  Ce module fournit informations concernant la carte cible.
 *  @{
 */

namespace Piduino {

  class Board {
    public:

      // -----------------------------------------------------------------------
      //
      //                         Board::Family Class
      //
      // -----------------------------------------------------------------------
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

          void setId (Id i);

        private:
          Id _id;
          std::string _name;
      };

      // -----------------------------------------------------------------------
      //
      //                         Board::Model Class
      //
      // -----------------------------------------------------------------------
      class Model {
        public:

          enum Id {
            RaspberryPiA = 0,
            RaspberryPiB,
            RaspberryPiAPlus,
            RaspberryPiBPlus,
            RaspberryPiComputeModule,
            RaspberryPi2,
            RaspberryPiZero,
            RaspberryPi3,
            RaspberryPiZeroW,
            RaspberryPiComputeModule3,
            NanoPiNeo,
            NanoPiNeoAir,
            NanoPiM1,
            NanoPiM1Plus,
            NanoPiNeo2,
            NanoPiNeoPlus2,

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

      // -----------------------------------------------------------------------
      //
      //                         Board Class
      //
      // -----------------------------------------------------------------------

      Board(int rev = -1);
      Board(const std::string & tag);
      virtual ~Board();

      inline const std::string & name() const {
        return model().name();
      }

      inline const Family & family() const {
        return model().family();
      }

      inline const SoC & soc() const {
        return model().soc();
      }

      inline int id() const {
        return _id;
      }

      inline int revision() const {
        return _revision;
      }

      inline float pcbRevision() const {
        return _pcb_revision;
      }

      inline int gpioId() const {
        return _gpio_id;
      }

      inline int ram() const {
        return _ram;
      }

      inline const std::string & tag() const {
        return _tag;
      }

      inline const Model & model() const {
        return _model;
      }

      inline const Manufacturer & manufacturer() const {
        return _manufacturer;
      }

    protected:
      void setId (int i);
      void setRevision (int rev);
      void setTag (const std::string & tag);
      int findRevision();
      std::string findTag();
      int totalRam();

    private:
      int _id;
      Model _model;
      Manufacturer _manufacturer;
      std::string _tag;
      int _gpio_id; // Révision du GPIO
      int _revision; // Révision carte (raspberry)
      float _pcb_revision;
      int _ram; // RAM en Mo
  };
}
/**
 *  @}
 */

/* ========================================================================== */
#endif /*_PIDUINO_BOARD_H_ defined */
