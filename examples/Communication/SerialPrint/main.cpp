// SerialPrint

// Wait for the characters on the serial port and return them, repeatedly.

// Created 10 september 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.

#include <Piduino.h>  // All the magic is here ;-)

const unsigned long ConsoleBaudrate = 115200;

// Serial constants
const unsigned long SerialBaudrate = 38400;
const uint8_t SerialConfig = SERIAL_8E1;

const String hello = "Hello World, Have a nice day!";

void setup() {
  
  Console.begin (ConsoleBaudrate);
  Console.print ("SerialPrint to ");
  Console.println (Serial.portName());

  // initialize serial:
  Serial.begin (SerialBaudrate, SerialConfig);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop () {

  // Press Ctrl+C to abort ...
  Serial.print (hello);
  Serial.flush();
  
  Console.print (hello);
  Console.println();

  delay (500);
}

