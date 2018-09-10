// ASCIIPing

// Wait for the characters on the serial port and return them, repeatedly.

// Created 10 september 2018

// This example code is in the public domain.

#include <Arduino.h> // all the magic is here ;-)
int c;

void setup() {
  // initialize serial:
  Serial.begin (115200);
  Serial.println ("Press any key...");
}

void loop () {

  // if there's any serial available, read it:
  while (Serial.available() > 0) {

    c = Serial.read();
    if (c > 0) { // if the reading was successful, write the character on the port
      Serial.write (c);
    }
  }
}
