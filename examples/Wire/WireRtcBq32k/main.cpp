// I2C RTC BQ32000

// Demonstrates use of the Wire library
// Controls BQ32000 Real Time Clock via I2C/TWI

// Console is used with PIDUINO as Serial with ARDUINO.

// Created 16 August 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

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

  Console.begin (115200);
  Wire.begin(); // Starting the i2c master
}

void loop() {

  Wire.beginTransmission (0x68); // start of the frame for the RTC at slave address 0x68
  Wire.write (0); // write the address of the register in the RTC, 0 first register
  Wire.endTransmission (false); // restart condition
  Wire.requestFrom (0x68, 3); // 3-byte read request

  if (Wire.available() == 3) { // if the 3 bytes have been read
    byte sec = Wire.read();
    byte min = Wire.read();
    byte hour = Wire.read() & 0x3F; // remove CENT_EN and CENT LSB bits

    // time display
    printBcdDigit (hour);
    printBcdDigit (min);
    printBcdDigit (sec, true);
  }
  exit (0); // exit the loop() function without ever coming back.
  // On Arduino, exit() performs an infinite loop as explained on
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html
  // on a Pi board, exit () stops the program by returning the supplied value.
}
