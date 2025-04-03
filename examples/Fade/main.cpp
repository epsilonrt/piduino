/*
  Fade

  This example shows how to fade an LED on pin 0 using the analogWrite()
  function.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
#endif


// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int led = 0;  // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

int brightness = 0; // how bright the LED is
int fadeAmount = 5; // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 0 to be an output:
  pinMode(led, OUTPUT);
}

// the loop routine runs over and over again forever:
// Press Ctrl+C to abort ...
void loop() {
  // set the brightness of pin 0:
  analogWrite(led, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}
