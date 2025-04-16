/* Copyright © 2018 Pascal JEAN, All rights reserved.
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
#include <thread>
#include <future>
#include <memory>
#include <piduino/gpiopin.h>

namespace Piduino {

  class Pin::Private {
    public:
      Private (Pin *q, Connector *parent, const Descriptor *desc);
      virtual ~Private () = default;

      void setHoldMode();
      void setHoldPull();

      void readPull ();
      void writePull();
      void readMode();
      void writeMode ();
      void readEdge();
      void writeEdge ();
      void readDrive ();
      void writeDrive();

      bool sysFsEnable (bool enable);
      void sysFsExport (bool enable);
      bool sysFsIsExport () const;
      bool sysFsOpen();
      void sysFsClose();

      void sysFsGetEdge();
      void sysFsSetEdge ();
      void sysFsGetMode();
      void sysFsSetMode ();

      void sysFsWriteFile (const char *n, const std::string &v);
      std::string sysFsReadFile (const char *n) const;
      bool sysFsFileExist (const char *n) const;

      static int sysFsPoll (int fd, int timeout_ms = -1);
      static int sysFsRead (int fd);
      static int sysFsWrite (int fd, bool value);
      static void *irqThread (std::future<void> run, int fd, Isr isr, void *userData);

    public:
      Pin *const q_ptr;
      bool isopen;
      Connector *parent;
      const Descriptor *descriptor;
      Mode holdMode;
      Pull holdPull;
      bool holdState;
      bool useSysFs;
      int valueFd;
      bool firstPolling;

      Edge edge;
      Mode mode;
      Pull pull;

      std::promise<void> stopRead;
      std::thread thread;

      std::shared_ptr<Converter> dac;
      int drive;

      static const std::map<Pull, std::string> pulls;
      static const std::map<Type, std::string> types;
      static const std::map<Numbering, std::string> numberings;
      static const std::map<Mode, std::string> sysfsmodes;
      static const std::map<Edge, std::string> edges;
      static const std::map<std::string, Edge> str2edge;
      static const std::map<std::string, Mode> str2mode;
      static std::string syspath;

      PIMP_DECLARE_PUBLIC (Pin)
  };
}