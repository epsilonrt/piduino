// Blink

// Turns an LED on for one second, then off for one second, repeatedly.

// Created 8 March 2018

// This example code is in the public domain.

#include <iostream>
#include <piduino/arduino.h> // all the magic is here ;-)
using namespace std;

// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

int main () {

  // setup begin
  // initialize digital pin ledPin as an output.
  pinMode (ledPin, OUTPUT);
  cout << "Press Ctrl+C to abort ..." << endl;
  // setup end

  for (;;) { // equivalent to the function arduino loop()

    digitalWrite (ledPin, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay (1000);                 // wait for a second
    digitalWrite (ledPin, LOW);   // turn the LED off by making the voltage LOW
    delay (1000);                 // wait for a second
    cout << '.' << flush;         // displays one point in each cycle
  }

  return 0;
}
// -----------------------------------------------------------------------------
