// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.
#ifdef __unix__
#include <Arduino.h>  // Piduino, all the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

#include <Wire.h>

void setup() {

  Console.begin (115200);
  Wire.begin();        // join i2c bus (address optional for master)
}

void loop() {
  Wire.requestFrom (8, 6);    // request 6 bytes from slave device #8

  while (Wire.available()) {  // slave may send less than requested
    char c = Wire.read();     // receive a byte as character
    Console.print (c);        // print the character
  }

  delay (500);
}
