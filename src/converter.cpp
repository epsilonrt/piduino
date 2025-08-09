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

  // ---------------------------------------------------------------------------
  // virtual
  long Converter::readSample (int channel, bool differential) {

    if (openMode() & ReadOnly) {
      PIMP_D (Converter);

      return d->readSample (channel, differential);
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
  long
  Converter::valueToDigital (double voltage, bool differential) const {
    PIMP_D (const Converter);

    return d->valueToDigital (voltage, differential);
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::readAverage (int channel, bool differential, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; ++i) {
      sum += readSample (channel, differential); // Read each sample
    }
    return sum / count;
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::readValue (int channel, bool differential) {

    return digitalToValue (readSample (channel, differential), differential);
  }

  // -----------------------------------------------------------------------------
  // virtual
  double
  Converter::readAverageValue (int channel, bool differential, int count) {

    return digitalToValue (readAverage (channel, differential, count), differential);
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool
  Converter::writeSample (long value, int channel, bool differential) {

    if (openMode() & WriteOnly) {
      PIMP_D (Converter);

      return d->writeSample (d->clampValue (value, differential), channel, differential);
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool Converter::writeValue (double value, int channel, bool differential) {

    return writeSample (valueToDigital (value, differential), channel, differential);
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
  bool Converter::setReference (int referenceId, double fsr) {
    PIMP_D (Converter);

    return d->setReference (referenceId, fsr);
  }

  // ---------------------------------------------------------------------------
  int Converter::reference() const {
    PIMP_D (const Converter);

    return d->reference();
  }

  // ---------------------------------------------------------------------------
  double Converter::fullScaleRange() const {
    PIMP_D (const Converter);
    return d->fullScaleRange();
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
  //
  //                     Converter::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Converter::Private::Private (Converter *q, Type type, unsigned int flags, const std::string &parameters) :
    IoDevice::Private (q), type (type), flags (flags), parameters (split (parameters, ':')) {}

  // ---------------------------------------------------------------------------
  Converter::Private::~Private() = default;

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
