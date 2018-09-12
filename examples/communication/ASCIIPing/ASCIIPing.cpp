// ASCIIPing

// Wait for the characters on the serial port and return them, repeatedly.

// Created 10 september 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.
#ifdef __unix__
#include <Arduino.h>  // Piduino, all the magic is here ;-)
#endif

void setup() {
  
  // initialize serial:
  Serial.begin (115200);
  Serial.println ("Press any key...");
}

void loop () {

  // Press Ctrl+C to abort ...
  while (Serial.available() > 0) { // if there's any serial available ...

    int c = Serial.read(); //  read it !
    if (c > 0) { // if the reading was successful ...

      Serial.write (c); // write the character on the port
    }
  }
}
