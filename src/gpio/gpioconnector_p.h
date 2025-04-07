#pragma once

#include <string>
#include <array>

namespace Piduino {
  namespace ConnectorOstream {

    // ---------------------------------------------------------------------------
    enum Alignment {
      Left,
      Right,
      Center
    };

    // ---------------------------------------------------------------------------
    class Field {
      public:
        std::string::size_type size;
        std::string name;

        Field (std::string::size_type s, const std::string &n) : size (s), name (n) {}
    };

    std::string toUpper (const std::string &s);
    std::string format (const std::string &s, std::string::size_type w, Alignment a);
  }
}