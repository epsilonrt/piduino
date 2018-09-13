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
#ifndef PIDUINO_ARDUINO_H
#define PIDUINO_ARDUINO_H

#ifndef __DOXYGEN__

#ifdef __cplusplus
extern "C"{
#endif

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

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
// -----------------------------------------------------------------------------
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <piduino/gpio.h>

#define EXTERN_C extern "C"

enum ArduinoPinMode {
  INPUT = Piduino::Pin::ModeInput,
  OUTPUT = Piduino::Pin::ModeOutput,
  INPUT_PULLUP,
  INPUT_PULLDOWN // Not supported by Arduino !
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

typedef Piduino::Pin::Isr Isr;

#else /* __cplusplus not defined */
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define EXTERN_C

typedef enum {
  INPUT = 0,
  OUTPUT,
  INPUT_PULLUP,
  INPUT_PULLDOWN // Not supported by Arduino !
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

typedef void (* Isr) (void);

// -----------------------------------------------------------------------------
#endif /* __cplusplus not defined */

#define digitalPinToInterrupt(p) (p)

/* types ==================================================================== */
typedef unsigned int word;
typedef uint8_t boolean;
typedef uint8_t byte;

#else /* __DOXYGEN__ defined */

/**
 *  @defgroup piduino_arduino Arduino
 *
 *  Ce module permet de programmer avec Piduino en utilisant une syntaxe proche d'Arduino.
 */

/**
 *  @addtogroup piduino_arduino
 *  @{
 */

/**
 * @enum ArduinoPinMode
 * @brief Mode d'utilisation d'une broche digitale
 */
enum ArduinoPinMode {
  INPUT,  ///< entrée sans résistance de tirage
  OUTPUT, ///< sortie
  INPUT_PULLUP,  ///< entrée avec résistance de tirage à l'état haut
  INPUT_PULLDOWN ///< entrée avec résistance de tirage à l'état bas, mode supplémentaire non disponible dans Arduino
};

/**
 * @enum ArduinoIntEdge
 * @brief Type de front déclenchant une interrption
 *
 * @warning le déclenchement sur niveau logique bas et haut (LOW/HIGH)
 * ne sont pas pris en charge
 */
enum ArduinoIntEdge {
  // LOW & HIGH unsupported !
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
 * Une routine d'interruption ne prend et ne renvoie aucun paramètre.
 */
typedef void (* Isr) (void);

/**
 * @enum ArduinoBool
 * @brief Valeur binaire
 */
enum ArduinoBool {
  HIGH = true,  ///< état haut
  LOW = false   ///< état bas
};


#endif /* __DOXYGEN__ defined */

// Digital pins ----------------------------------------------------------------
/**
 * @brief Modification du mode d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
EXTERN_C void pinMode (int pin, ArduinoPinMode mode);

/**
 * @brief Modification d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
EXTERN_C void digitalWrite (int pin, int value);

/**
 * @brief Lecture d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
EXTERN_C int digitalRead (int pin);

/**
 * @brief Basculement de l'état  d'une broche digitale
 *
 *  @warning Non disponible dans Arduino !
 */
EXTERN_C void digitalToggle (int pin); // Not supported by Arduino !

// Analog pins -----------------------------------------------------------------
/**
 * @brief Writes an analog value (PWM wave) to a pin. 
 * @param pin  the pin to write to.
 * @param value the duty cycle: between 0 (always off) and 255 (always on). 
 */
EXTERN_C void analogWrite(int pin, int value);

//int analogRead(uint8_t);
//void analogReference(uint8_t mode);

// Interrupts ------------------------------------------------------------------
/**
 * @brief Installe une routine d'interruption (Isr)
 *
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
EXTERN_C void attachInterrupt (int pin, Isr isr, ArduinoIntEdge mode);

/**
 * @brief Désinstalle une routine d'interruption (Isr)
 *
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
EXTERN_C void detachInterrupt (int pin);

// Time ------------------------------------------------------------------------
/**
 * @brief Pause pour une durée en millisecondes
 * @param ms durée de la pause en ms, -1 pour endormir le thread
 */
EXTERN_C void delay (unsigned long ms);

/**
 * @brief Pause pour une durée en microsecondes
 * @param ms durée de la pause en us, -1 pour endormir le thread
 */
EXTERN_C void delayMicroseconds (unsigned long us);

/**
 * @brief Nombre de millisecondes depuis le lancement du programme
 */
EXTERN_C unsigned long millis();

/**
 * @brief Nombre de microsecondes depuis le lancement du programme
 */
EXTERN_C unsigned long micros();

/**
 * @brief Définie la priorité en temps réel du thread appelant
 *
 * L'algorithme choisie est le round-robin. Sur un système Linux, la
 * valeur normale de priorité est de 20, la valeur minimale est de 1 et
 * la valeur maximale est de 99. Donner une valeur de 99 est une très
 * mauvaise idée qui peut bloquer le système...
 *
 * @param priority valeur de la priorité
 */
EXTERN_C void setPriority (int priority);

/**
* @}
*/

/* ========================================================================== */
#endif /*PIDUINO_ARDUINO_H defined */
