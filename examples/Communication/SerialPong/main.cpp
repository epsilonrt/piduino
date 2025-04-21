// SerialPong

// Wait for the characters on the serial port and return them, repeatedly.

// Created 10 september 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.

#include <Piduino.h>  // All the magic is here ;-)

const unsigned long ConsoleBaudrate = 115200;

// Serial constants
const unsigned long SerialBaudrate = 38400;
const uint8_t SerialConfig = SERIAL_8E1;


void setup() {
  
  Console.begin (ConsoleBaudrate);
  Console.print ("SerialPong to ");
  Console.println (Serial.portName());

  // initialize serial:
  Serial.begin (SerialBaudrate, SerialConfig);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop () {
  String ping;

  // Press Ctrl+C to abort ...
  while (Serial.available() > 0) { // if there's any serial available ...
    int c = Serial.read(); //  read it !

    delay (10);
    if (c > 0) { // if the reading was successful ...
      ping.concat ( (char) c);
    }
  }

  if (ping.length() > 0) {
    String pong;

    Console.print (ping); // write the character on the buffer
    Console.println(); // flush the buffer on the console

    pong = ping;
    pong.toLowerCase();
    
    Serial.print (pong); // write the character on the buffer
    Serial.flush();
    
    Console.print (pong); // write the character on the buffer
    Console.println(); // flush the buffer on the console
  }
}

