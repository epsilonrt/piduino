// I2C RTC BQ32000

// Demonstrates use of the Wire library
// Controls BQ32000 Real Time Clock via I2C/TWI

// Console is used with PIDUINO as Serial with ARDUINO.
// If you want to use this example with ARDUINO, replace Console with Serial 
// and add Serial.begin(9600) in setup()

// Created 16 August 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.


#include <Arduino.h>
#include <Wire.h>

void printBcdDigit (byte val, bool end = false) {
  val = (val / 16 * 10) + (val % 16); // BCD to DEC
  
  if (val < 10) {
    Console.write ('0'); // leading zero
  }
  if (end) {
    
    Console.println (val);
  }
  else {
    
    Console.print (val);
    Console.write (':');
  }
}

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {

  Wire.beginTransmission (0x68);
  Wire.write (0);
  Wire.endTransmission (false);
  Wire.requestFrom (0x68, 3);
  
  if (Wire.available() == 3) {
    byte sec = Wire.read();
    byte min = Wire.read();
    byte hour = Wire.read() & 0x3F; // remove CENT_EN and CENT LSB bits

    printBcdDigit (hour);
    printBcdDigit (min);
    printBcdDigit (sec, true);
  }
  else {
    
    Wire.flush();
  }
  delay (1000);
}
