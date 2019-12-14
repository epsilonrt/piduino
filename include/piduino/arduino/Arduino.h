/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2018 by Pascal JEAN (pascal.jean@piduino.org) for piduino library
*/
#ifndef Arduino_h
#define Arduino_h

#ifdef __DOXYGEN__

/**
 *  @defgroup piduino_arduino Arduino
 *
 *  Ce module fournit une API Arduino sur cartes Pi.
 *  @{
 */

/**
 * @enum ArduinoBool
 * @brief Valeur binaire
 */
enum ArduinoBool {
  HIGH = true,  ///< état haut
  LOW = false   ///< état bas
};

/**
 * @enum ArduinoPinMode
 * @brief Mode d'utilisation d'une broche digitale
 */
enum ArduinoPinMode {
  INPUT, ///< digital input without pull-up resistor
  OUTPUT, ///< digital input
  INPUT_PULLUP, ///< digital input with pull-up resistor
  INPUT_PULLDOWN, ///< digital input with pull-down resistor, @warning Not supported by Arduino !
  PULLUP, ///<  pull-up resistor enabled, @warning Not supported by Arduino !
  PULLDOWN  ///<  pull-down resistor enabled, @warning Not supported by Arduino !
};

/**
 * @enum ArduinoIntEdge
 * @brief Type de front déclenchant une interrption
 *
 * @warning le déclenchement sur niveau logique bas et haut (LOW/HIGH)
 * ne sont pas pris en charge par Piduino
 */
enum ArduinoIntEdge {
  /// LOW & HIGH unsupported by Piduino !
  RISING, ///< front montant
  FALLING ///< front descendant
  CHANGE, ///< front montant et descendant
};

/**
 * @brief Transforme un numéro de broche en numéro d'interruption
 *
 * Cette macro est présente uniquement pour la compatibilté Arduino car elle
 * ne fait que recopier le numéro de broche.
 */
#define digitalPinToInterrupt(p)

/**
 * @brief Routine d'interruption
 *
 * Une routine d'interruption Arduino ne prend et ne renvoie aucun paramètre.
 */
typedef void (* Isr) (void);

/**
 * @brief Routine d'interruption qui reçoit un pointeur sur des données
 *
 * @warning Extension propre à Piduino uniquement en C++
 */
typedef void (* IsrWithData) (void * user_data);

// Digital pins ----------------------------------------------------------------
/**
 * @brief Modification du mode d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
void pinMode (int pin, ArduinoPinMode mode);

/**
 * @brief Modification d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
void digitalWrite (int pin, int value);

/**
 * @brief Lecture d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
int digitalRead (int pin);

/**
 * @brief Basculement de l'état  d'une broche digitale
 *
 *  @warning Non disponible dans Arduino !
 */
void digitalToggle (int pin);

// Analog pins -----------------------------------------------------------------
/**
 * @brief Writes an analog value (PWM wave) to a pin.
 * @param pin  the pin to write to.
 * @param value the duty cycle: between 0 (always off) and 255 (always on).
 */
void analogWrite (int pin, int value);

//int analogRead(uint8_t);
//void analogReference(uint8_t mode);

// Interrupts ------------------------------------------------------------------
/**
 * @brief Installe une routine d'interruption (isr)
 *
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
void attachInterrupt (int pin, Isr isr, ArduinoIntEdge mode);

/**
 * @brief Installe une routine d'interruption isr avec passage de données
 *
 * Surcharge de la fonction Arduino
 * @warning Extension propre à Piduino uniquement en C++
 */
void attachInterrupt (int pin, IsrWithData isr, ArduinoIntEdge mode, void * data);

/**
 * @brief Désinstalle une routine d'interruption (Isr)
 *
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
void detachInterrupt (int pin);

// Time ------------------------------------------------------------------------
/**
 * @brief Pause pour une durée en millisecondes
 * @param ms durée de la pause en ms, -1 pour endormir le thread
 */
void delay (unsigned long ms);

/**
 * @brief Pause pour une durée en microsecondes
 * @param ms durée de la pause en us, -1 pour endormir le thread
 */
void delayMicroseconds (unsigned long us);

/**
 * @brief Nombre de millisecondes depuis le lancement du programme
 */
unsigned long millis();

/**
 * @brief Nombre de microsecondes depuis le lancement du programme
 */
unsigned long micros();

/**
 * @brief Définie la priorité en temps réel du thread appelant
 *
 * L'algorithme choisie est le round-robin. Sur un système Linux, la
 * valeur normale de priorité est de 20, la valeur minimale est de 1 et
 * la valeur maximale est de 99. Donner une valeur de 99 est une très
 * mauvaise idée qui peut bloquer le système...
 *
 * @param priority valeur de la priorité
 * @warning Extension propre à Piduino
 */
void setPriority (int priority);

/**
 * @brief Création d'un fichier contenant le pid du programme en cours
 *
 * Le fichier est détruit automatiquement à la fermeture du programme.
 *
 * @param path chemin complet du fichier à créer, si NULL, le chemin du fichier
 * est `/var/run/xxx.pid` avec xxx nom du prgramme.
 * @warning Extension propre à Piduino
 */
void createPidFile (const char * path);

/**
 * @brief Supprime le fichier pid
 * @warning Extension propre à Piduino
 */
void deletePidFile ();

/**
* @}
*/

#else /* End of the documentation, __DOXYGEN__ not defined ------------------ */

// -----------------------------------------------------------------------------
// part C/C++

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "binary.h"

/* types ==================================================================== */
typedef unsigned int word;
typedef uint8_t boolean;
typedef uint8_t byte;
typedef void (* Isr) (void);

/* constants ================================================================ */
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

/* macros =================================================================== */

#ifndef __cplusplus
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#endif

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif
#ifndef radians
#define radians(deg) ((deg)*DEG_TO_RAD)
#endif
#ifndef degrees
#define degrees(rad) ((rad)*RAD_TO_DEG)
#endif
#ifndef sq
#define sq(x) ((x)*(x))
#endif

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define bit(b) (1UL << (b))

#define digitalPinToInterrupt(p) (p)

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
// -----------------------------------------------------------------------------
// part only C++
//
#include <iostream>
#include <algorithm>
#include <piduino/gpio.h>
#include "HardwareSerial.h"
#include "WCharacter.h"
#include "WString.h"
#include <Console.h>
#include <piduino/popl.h>
#include <piduino/syslog.h>

extern PiConsole Console;
extern Piduino::OptionParser CmdLine;

#define EXTERN_C extern "C"

typedef Piduino::Pin::Isr IsrWithData;

enum ArduinoPinMode {
  INPUT = Piduino::Pin::ModeInput,
  OUTPUT = Piduino::Pin::ModeOutput,
  INPUT_PULLUP,
  // Not supported by Arduino !
  INPUT_PULLDOWN,
  PULLUP,
  PULLDOWN
};

enum ArduinoIntEdge {
  // LOW & HIGH unsupported !
  RISING = Piduino::Pin::EdgeRising,
  FALLING = Piduino::Pin::EdgeFalling,
  CHANGE = Piduino::Pin::EdgeBoth,
};

enum ArduinoBool {
  HIGH = true,
  LOW = false
};

void attachInterrupt (int pin, IsrWithData isr, ArduinoIntEdge mode, void * data = 0);

uint16_t makeWord (uint16_t w);
uint16_t makeWord (byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

// WMath prototypes
long random (long);
long random (long, long);
void randomSeed (unsigned long);
long map (long, long, long, long, long);

#else /* __cplusplus not defined */
// -----------------------------------------------------------------------------
// part only C
//
#include <stdbool.h>
#include <stdio.h>

#define EXTERN_C

typedef enum {
  INPUT = 0,
  OUTPUT,
  INPUT_PULLUP,
  // Not supported by Arduino !
  INPUT_PULLDOWN,
  PULLUP,
  PULLDOWN
} ArduinoPinMode;

typedef enum {
  // LOW & HIGH unsupported !
  RISING = 1,
  FALLING,
  CHANGE
} ArduinoIntEdge ;

typedef enum  {
  HIGH = true,
  LOW = false
} ArduinoBool;

// -----------------------------------------------------------------------------
#endif /* __cplusplus not defined */

// -----------------------------------------------------------------------------
// part C/C++
#ifdef __cplusplus
extern "C" {
#endif

void pinMode (int pin, ArduinoPinMode mode);
void digitalWrite (int pin, int value);
int digitalRead (int pin);
void analogWrite (int pin, int value);
void attachInterrupt (int pin, Isr isr, ArduinoIntEdge mode);
void detachInterrupt (int pin);
void delay (unsigned long ms);
void delayMicroseconds (unsigned long us);
unsigned long millis();
unsigned long micros();
void yield(void);

// Not supported by Arduino !
void digitalToggle (int pin); // Not supported by Arduino !
void setPriority (int priority);
void createPidFile (const char * path);
void deletePidFile();

void _interrupts();
void _noInterrupts();

static inline void interrupts() {
#ifdef ARDUINO_INTERRUPTS
  _interrupts();
#endif
}

static inline  void noInterrupts() {
#ifdef ARDUINO_INTERRUPTS
  _noInterrupts();
#endif
}

#ifdef __cplusplus
}
#endif

extern int argc;
extern char ** argv;
extern bool isDaemon;

#endif /* __DOXYGEN__ not defined */
/* ========================================================================== */
#endif /*Arduino_h defined */
