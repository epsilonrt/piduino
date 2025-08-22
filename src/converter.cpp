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
#include <piduino/converter.h>
#include "converter_p.h"
#include "config.h"
#include <functional>
#include <map>
#include <vector>

namespace Piduino {

  // -----------------------------------------------------------------------------
  //
  //                             Converter Class
  //
  // -----------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Converter::Converter (Converter::Private &dd) : IoDevice (dd) {}

  // -----------------------------------------------------------------------------
  Converter *Converter::factory (const std::string &deviceName, const std::string &parameters) {
    auto &registry = Private::getRegistry();
    std::string params;
    std::string name;

    // Check if deviceName contains the ':' character
    if (deviceName.find (':') != std::string::npos) {
      // deviceName contains parameters
      // Split the deviceName into name and parameters
      params = deviceName.substr (deviceName.find (':') + 1);
      name = deviceName.substr (0, deviceName.find (':'));
    }
    else {
      // Pas de paramètres, utilise le nom complet
      name = deviceName;
      params = parameters; // Utilise les paramètres passés
    }

    auto it = registry.find (name);

    if (it != registry.end()) {
      return it->second.creator (params); // Appelle le créateur avec les paramètres
    }

    return nullptr; // Classe non trouvée
  }

  // -----------------------------------------------------------------------------
  void Converter::registerConverter (const std::string &deviceName,
                                     std::function<Converter* (const std::string &) > creator,
                                     const std::string &type,
                                     const std::string &parameters) {
    auto &registry = Private::getRegistry();
    Private::registryKey key (creator, type, parameters);
    registry[deviceName] = key;
  }

  // -----------------------------------------------------------------------------
  std::vector<Converter::Info> Converter::availableConverters() {
    std::vector<Info> infos;
    auto &registry = Private::getRegistry();

    for (const auto &pair : registry) {
      Info info;
      info.name = pair.first; // The name of the converter class
      info.type = pair.second.type; // The type of the converter (e.g., "dac", "adc")
      info.parameters = pair.second.parameters; // Parameters for the converter, a colon-separated list of values
      infos.push_back (info);
    }

    return infos;
  }

  // ---------------------------------------------------------------------------
  Converter::~Converter() {

    close();
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::open (OpenMode mode) {

    if (!isOpen()) {
      PIMP_D (Converter);

      return d->open (mode);
    }
    return isOpen();
  }

  // ---------------------------------------------------------------------------
  void
  Converter::close() {

    if (isOpen()) {
      PIMP_D (Converter);

      d->close();
    }
  }

  // ---------------------------------------------------------------------------
  // virtual
  long
  Converter::read () {

    if (openMode() & ReadOnly) {
      PIMP_D (Converter);

      return d->read();
    }
    return InvalidValue;
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool
  Converter::write (long value) {

    if (openMode() & WriteOnly) {
      PIMP_D (Converter);

      return d->write (d->clampValue (value));
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  // virtual
  bool Converter::toggle (int channel) {
    if (openMode() & WriteOnly && type() == GpioExpander) {
      PIMP_D (Converter);

      return d->toggle (channel);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // virtual
  long Converter::readChannel (int channel, bool differential) {

    if (openMode() & ReadOnly) {
      PIMP_D (Converter);

      return d->readChannel (channel, differential);
    }
    return InvalidValue;
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::digitalToValue (long digitalValue, bool differential) const {
    PIMP_D (const Converter);

    return d->digitalToValue (digitalValue, differential);
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::digitalToValue (int channel, long digitalValue, bool differential) const {
    PIMP_D (const Converter);

    return d->digitalToValue (digitalValue, differential, channel);
  }

  // -----------------------------------------------------------------------------
  // virtual
  long
  Converter::valueToDigital (double voltage, bool differential) const {
    PIMP_D (const Converter);

    return d->valueToDigital (voltage, differential);
  }


  // -----------------------------------------------------------------------------
  // virtual
  long
  Converter::valueToDigital (int channel, double value, bool differential) const {
    PIMP_D (const Converter);

    return d->valueToDigital (value, differential, channel);
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::readAverage (int channel, bool differential, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; ++i) {
      sum += readChannel (channel, differential); // Read each sample
    }
    return sum / count;
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::readValue (int channel, bool differential) {

    return digitalToValue (channel, readChannel (channel, differential), differential);
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::readAverageValue (int channel, bool differential, int count) {

    return digitalToValue (channel, readAverage (channel, differential, count), differential);
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool
  Converter::writeChannel (long value, int channel, bool differential) {

    if (openMode() & WriteOnly) {
      PIMP_D (Converter);

      return d->writeChannel (d->clampValue (value, differential), channel, differential);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool Converter::writeValue (double value, int channel, bool differential) {

    if (flags() & hasReferencePerChannel && channel < 0) {
      PIMP_D (Converter);

      d->setError (EINVAL); 
      if (d->isDebug) {
        std::cerr << "Invalid argument: writing an analog value to all channels is not supported when per-channel references are enabled." << std::endl;
      }
      return false;
    }
    return writeChannel (valueToDigital (channel, value, differential), channel, differential);
  }

  // ---------------------------------------------------------------------------
  // virtual
  int Converter::numberOfChannels() const {
    PIMP_D (const Converter);

    return d->numberOfChannels();
  }

  // ---------------------------------------------------------------------------
  const std::string &Converter::deviceName() const {
    PIMP_D (const Converter);

    return d->deviceName();
  }

  // ---------------------------------------------------------------------------
  Converter::Type
  Converter::type() const {
    PIMP_D (const Converter);

    return d->type;
  }

  // ---------------------------------------------------------------------------
  unsigned int
  Converter::flags() const {
    PIMP_D (const Converter);

    return d->flags;
  }

  // ---------------------------------------------------------------------------
  void Converter::setEnable (bool enable) {
    PIMP_D (Converter);

    d->setEnable (enable);
  }

  // ---------------------------------------------------------------------------
  bool Converter::isEnabled () const {
    PIMP_D (const Converter);

    return d->isEnabled();
  }

  // ---------------------------------------------------------------------------
  long
  Converter::max (bool differential) const {
    PIMP_D (const Converter);

    return d->max (differential);
  }

  // ---------------------------------------------------------------------------
  long
  Converter::min (bool differential) const {
    PIMP_D (const Converter);

    return d->min (differential);
  }


  // ------------------------------- Optional API ------------------------------

  // ---------------------------------------------------------------------------
  int
  Converter::resolution() const {
    PIMP_D (const Converter);

    return d->resolution();
  }

  // ---------------------------------------------------------------------------
  int Converter::setResolution (int resolution) {
    PIMP_D (Converter);

    return d->setResolution (resolution);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::isBipolar() const {
    PIMP_D (const Converter);

    return d->isBipolar();
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::setBipolar (bool bipolar) {
    PIMP_D (Converter);

    return d->setBipolar (bipolar);
  }

  // ---------------------------------------------------------------------------
  long
  Converter::range() const {
    PIMP_D (const Converter);

    return d->range();
  }

  // ---------------------------------------------------------------------------
  long
  Converter::setRange (long range) {
    PIMP_D (Converter);

    return d->setRange (range);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::setReference (int referenceId, double fsr) {
    PIMP_D (Converter);

    return d->setReference (referenceId, fsr);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::setReference (int referenceId, int channel, double fsr) {
    PIMP_D (Converter);

    return d->setReference (referenceId, fsr, channel);
  }

  // ---------------------------------------------------------------------------
  int
  Converter::reference() const {
    PIMP_D (const Converter);

    return d->reference();
  }

  // ---------------------------------------------------------------------------
  int
  Converter::reference (int channel) const {
    PIMP_D (const Converter);

    return d->reference (channel);
  }

  // ---------------------------------------------------------------------------
  double
  Converter::fullScaleRange() const {
    PIMP_D (const Converter);

    return d->fullScaleRange();
  }

  // ---------------------------------------------------------------------------
  double
  Converter::fullScaleRange (int channel) const {
    PIMP_D (const Converter);

    return d->fullScaleRange (channel);
  }

  // --------------------------------------------------------------------------
  bool Converter::setFullScaleRange (double fsr) {
    PIMP_D (Converter);

    return d->setFullScaleRange (fsr);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::setFullScaleRange (int channel, double fsr) {
    PIMP_D (Converter);

    return d->setFullScaleRange (fsr, channel);
  }

  // ---------------------------------------------------------------------------
  long Converter::frequency() const {
    PIMP_D (const Converter);

    return d->frequency();
  }

  // ---------------------------------------------------------------------------
  long Converter::setFrequency (long freq) {
    PIMP_D (Converter);

    return d->setFrequency (freq);
  }

  // ---------------------------------------------------------------------------
  int Converter::clock() const {
    PIMP_D (const Converter);

    return d->clock();
  }

  // ---------------------------------------------------------------------------
  bool Converter::setClock (int clock) {
    PIMP_D (Converter);

    return d->setClock (clock);
  }

  // ---------------------------------------------------------------------------
  Converter::Mode Converter::mode (int channel) const {
    PIMP_D (const Converter);

    return d->mode (channel);
  }

  // ---------------------------------------------------------------------------
  bool Converter::setMode (Mode m, int channel) {
    PIMP_D (Converter);

    return d->setMode (m, channel);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::setModeFlags (long flags, long mask, int channel) {
    PIMP_D (Converter);

    return d->setModeFlags (flags, mask, channel);
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::clearModeFlags (long flags, long mask, int channel) {
    PIMP_D (Converter);

    return d->clearModeFlags (flags, mask, channel);
  }

  // ---------------------------------------------------------------------------
  // static
  const std::map<Converter::ModeFlag, std::string> &Converter::modeFlagToStringMap() {

    static const std::map<Converter::ModeFlag, std::string> modeFlagMap = {
      {Converter::DigitalInput, "in"  },
      {Converter::DigitalOutput, "out" },
      {Converter::AnalogInput, "ain" },
      {Converter::AnalogOutput, "aout" },
      {Converter::ActiveLow, "activelow" },
      {Converter::PullUp, "up" },
      {Converter::PullDown, "down" },
      {Converter::NoMode, "off" },
      {Converter::EdgeRising, "rising" },
      {Converter::EdgeFalling, "falling" },
      {Converter::Interrupt, "int" },
      {Converter::Continuous, "continuous" },
      {Converter::SingleShot, "single" },
      {Converter::FastMode, "fast" },
      {Converter::SaveToEEPROM, "eeprom" },
      {Converter::PwrDwnEn, "pden" },
      {Converter::PwrDwn0, "pd0" },
      {Converter::PwrDwn1, "pd1" },
    };
    return modeFlagMap;
  }

  // ---------------------------------------------------------------------------
  // static
  const std::map<std::string, Converter::ModeFlag> &Converter::stringToModeFlagMap() {

    static const std::map<std::string, Converter::ModeFlag> stringMap = {
      {"in",    Converter::DigitalInput  },
      {"out",   Converter::DigitalOutput },
      {"ain", Converter::AnalogInput },
      {"aout", Converter::AnalogOutput },
      {"off",   Converter::NoMode },
      {"activelow", Converter::ActiveLow },
      {"up", Converter::PullUp },
      {"down", Converter::PullDown },
      {"rising", Converter::EdgeRising },
      {"falling", Converter::EdgeFalling },
      {"int", Converter::Interrupt },
      {"continuous", Converter::Continuous },
      {"single", Converter::SingleShot },
      {"fast", Converter::FastMode },
      {"eeprom", Converter::SaveToEEPROM },
      {"pden", Converter::PwrDwnEn },
      {"pd0", Converter::PwrDwn0 },
      {"pd1", Converter::PwrDwn1 },
    };
    return stringMap;
  }

  // ---------------------------------------------------------------------------
  //
  //                     Converter::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Converter::Private::Private (Converter *q, Type type, unsigned int flags, const std::string &parameters) :
    IoDevice::Private (q), type (type), flags (flags), parameters (split (parameters, ':')) {}

  // ---------------------------------------------------------------------------
  Converter::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  bool
  Converter::Private::setModeFlags (long flags, long mask, int channel) {
    Mode m;

    if (channel == -1) {

      for (int i = 0; i < numberOfChannels(); ++i) {
        m = mode (i) & ~mask;
        m |= flags & mask; // Set the specified flags
        if (!setMode (m, i)) {
          if (isDebug) {
            std::cerr << "Converter: Failed to set mode flags for channel: " << i << std::endl;
          }
          return false; // If setting mode fails for any channel, return false
        }
      }
    }
    else if (channel >= 0 && channel < numberOfChannels()) {

      m = mode (channel) & ~mask;
      m |= flags & mask; // Set the specified flags
      return setMode (m, channel);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool
  Converter::Private::clearModeFlags (long flags, long mask, int channel) {
    Mode m;

    if (channel == -1) {

      for (int i = 0; i < numberOfChannels(); ++i) {
        m = mode (i) & ~mask;
        m &= ~flags & mask; // Clear the specified flags
        if (!setMode (m, i)) {
          if (isDebug) {
            std::cerr << "Converter: Failed to clear mode flags for channel: " << i << std::endl;
          }
          return false; // If setting mode fails for any channel, return false
        }
      }
    }
    else if (channel >= 0 && channel < numberOfChannels()) {

      m = mode (channel) & ~mask;
      m &= ~flags & mask; // Clear the specified flags
      return setMode (m, channel);
    }
    return false;
  }

  // -----------------------------------------------------------------------------
  // Converts a string to a vector of strings by splitting it at the given delimiter.
  // If skipEmpty is true, empty tokens are not included in the result.
  // This is useful for parsing parameters or configuration strings.
  std::vector<std::string>
  Converter::Private::split (const std::string &str, char delimiter, bool skipEmpty) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while ( (end = str.find (delimiter, start)) != std::string::npos) {
      std::string token = str.substr (start, end - start);
      if (!skipEmpty || !token.empty())  { // Exclude deviceName from tokens
        tokens.push_back (token);
      }
      start = end + 1;
    }

    // Dernier élément
    std::string lastToken = str.substr (start);
    if (!skipEmpty || !lastToken.empty())  {
      tokens.push_back (lastToken);
    }

    return tokens;
  }

  std::map<std::string, std::string> Converter::Private::parseParameters (std::vector<std::string> &parameters) {
    std::map<std::string, std::string> result;

    auto it = parameters.begin();
    while (it != parameters.end()) {
      std::string::size_type pos = it->find ('=');
      if (pos != std::string::npos) {
        std::string key = it->substr (0, pos);
        std::string value = it->substr (pos + 1);
        result[key] = value;

        // Supprimer l'élément - erase() retourne un itérateur vers l'élément suivant
        it = parameters.erase (it);
        // ❌ PAS de ++it ici car erase() nous a déjà positionné sur l'élément suivant
      }
      else {
        // Pas de '=', passer à l'élément suivant
        ++it;
      }
    }

    return result;
  }

  // -----------------------------------------------------------------------------
  // Returns a Pin object based on the provided pin identifier string.
  // The pin identifier can be a simple number or a more complex string with a connector
  // (e.g., "1.7" for pin 7 of connector 1).
  // If the pin identifier is invalid or out of range, an exception is thrown.
  // If the pin identifier is valid, a pointer to the corresponding Pin object is returned.
  Pin *
  Converter::Private::getPin (const std::string &s) {
    Pin *p = nullptr;
    int pinnumber = -1;
    bool physicalNumbering = false;

    std::string::size_type n = s.find ('.');
    if (n != std::string::npos) {

      physicalNumbering = true;
    }

    try {

      if (!physicalNumbering) {

        pinnumber = stoi (s);
        p = &gpio.pin (pinnumber);
      }
      else {
        int connector;
        std::vector<std::string> v = split (s, '.');

        if (v.size() > 1) {

          connector = stoi (v[0]);
          pinnumber = stoi (v[1]);
        }
        else {

          connector = 1;
          pinnumber = stoi (v[0]);
        }
        p = &gpio.connector (connector)->pin (pinnumber);
      }
    }
    catch (std::out_of_range &e) {

      throw std::out_of_range ("Pin number out of range: " + s);
    }
    catch (...) {

      throw std::runtime_error ("Unknown error occurred while getting pin: " + s);
    }

    return p;
  }

  // -----------------------------------------------------------------------------
  // static
  // Returns a reference to the registry map that holds converter class creators.
  std::map<std::string, Converter::Private::registryKey> &
  Converter::Private::getRegistry() {
    static std::map<std::string, Converter::Private::registryKey> registry;
    return registry;
  }

} // namespace Piduino
/* ========================================================================== */
