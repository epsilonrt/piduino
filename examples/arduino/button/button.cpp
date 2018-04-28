/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 0,
  when pressing a pushbutton attached to pin 3.

  The circuit:
  - LED attached from pin 0 to ground
  - pushbutton attached to pin 3 from GND

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Button
*/
#include <iostream>
#include <Arduino.h> // all the magic is here ;-)
using namespace std;

// constants won't change. They're used here to set pin numbers:
// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi
const int buttonPin = 3; // Header Pin 15: GPIO22 for RPi, GPIOA3 for NanoPi

// variables will change:
int buttonState = HIGH;  // variable for reading the pushbutton status

int main () {

  // setup begin
  // initialize the LED pin as an output:
  pinMode (ledPin, OUTPUT);
  // initialize the pushbutton pin as an input with pull-up resistor:
  pinMode (buttonPin, INPUT_PULLUP);
  cout << "Press Ctrl+C to abort ..." << endl;
  // setup end

  for (;;) { // equivalent to the function arduino loop()
  
    // read the state of the pushbutton value:
    buttonState = digitalRead (buttonPin);

    // check if the pushbutton is pressed. If it is, the buttonState is LOW:
    if (buttonState == LOW) {
      // turn LED on:
      digitalWrite (ledPin, HIGH);
    }
    else {
      // turn LED off:
      digitalWrite (ledPin, LOW);
    }
  }
}
// -----------------------------------------------------------------------------

