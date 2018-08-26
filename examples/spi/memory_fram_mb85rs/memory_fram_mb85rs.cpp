// FRAM (Ferroelectric Random Access Memory) Memory SPI Test

// Shows the memory identifiers and performs reads and writes.
// In this example we used a FUJITSU MB85RS256B memory (256 Kbits (32 K × 8) Bit SPI, 3.3V)
// Uses the SPI library. For details on the memory, see:
// https://www.fujitsu.com/uk/Images/MB85RS256B.pdf

// Circuit:
// MB85RS           Connect to
// Pin 1 CS     --> Pin 10 SPI_CS (Header Pin 24: GPIO8 for RPi, GPIOC3 for NanoPi)
// Pin 2 SO     --> Pin 13 SPI_MISO (Header Pin 21: GPIO9 for RPi, GPIOC1 for NanoPi)
// Pin 3 /WP    --> VDD (3.3V)
// Pin 4 GND    --> GND
// Pin 5 SI     --> Pin 12 SPI_MOSI (Header Pin 19: GPIO10 for RPi, GPIOC0 for NanoPi)
// Pin 6 SCK    --> Pin 14 SPI_CLK (Header Pin 23: GPIO11 for RPi, GPIOC2 for NanoPi)
// Pin 7 /HOLD  --> VDD (3.3V)
// Pin 8 VDD    --> VDD (3.3V)

// Console is used with PIDUINO as Serial with ARDUINO.
// If you want to use this example with ARDUINO, replace Console with Serial
// and add Serial.begin(9600) in setup()

// Created 23 August 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.

#include <Arduino.h> // all the magic is here ;-)
#include "mb85rs.h"

byte buf[128];
const size_t bufSize = sizeof (buf);
unsigned long counter = 0;

// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to the MB85RS CS Pin (1) ! <-
// the other you need are controlled by the SPI library:
Mb85rs fram (10, 32768); // Pin 10 = SPI0_CS
// Mb85rs fram (27, 32768);

void setup() {

  // start the SPI library:
  SPI.begin();
  // setting up fram
  fram.begin();
}

void loop () {
  // Press Ctrl+C to abort ...
  Console.print ("# ");
  Console.print (++counter);
  Console.println (" ---------------------------------------------------------");
  
  Mb85rs::DeviceId id = fram.deviceId ();
  Console.print ("Manufacturer ID   : 0x");
  fram.printHex (Console, id.manufacturerId, 2);
  Console.print ("\nContinuation code : 0x");
  fram.printHex (Console, id.continuationCode, 2);
  Console.print ("\nProduct ID        : 0x");
  fram.printHex (Console, id.productId, 4);
  Console.write ('\n');

  uint8_t status = fram.status();
  Console.print ("\nStatus : 0x");
  fram.printHex (Console, status, 2);

  fram.read (0, buf, bufSize);
  Console.println ("\n\nData reads:");
  fram.printBlock (Console, 0, buf, bufSize);

  // Ecriture d'un bloc de bufSize octets
  // On remplit le buffer avec des nombres de 0 à bufSize-1
  for (size_t i = 0; i < bufSize; i++) {

    buf[i] = i;
  }
  Console.println ("\nData written:");
  fram.printBlock (Console, 0, buf, bufSize);

  fram.write (0, buf, bufSize);

  fram.read (0, buf, bufSize);
  Console.println ("\nData reads:");
  fram.printBlock (Console, 0, buf, bufSize);

  // Ecriture d'un bloc de 128 octets
  // On remplit le buffer avec des 0
  for (size_t i = 0; i < bufSize; i++) {

    buf[i] = 0;
  }
  Console.println ("\nData written:");
  fram.printBlock (Console, 0, buf, bufSize);
  
  fram.write (0, buf, bufSize);

  fram.read (0, buf, bufSize);
  Console.println ("\nData reads:");
  fram.printBlock (Console, 0, buf, bufSize);

  Console.write ('\n');
  delay (500); // wait for a 10 seconds
}
