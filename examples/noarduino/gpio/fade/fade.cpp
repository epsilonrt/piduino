/*
  Fade

  This example shows how to fade an LED on pin 0 using the analogWrite()
  function.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/
#include <iostream>
#include <piduino/clock.h>
#include <piduino/gpio.h>

using namespace std;
using namespace Piduino;

// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

Pin & led = gpio.pin (ledPin); // led is a reference on pin 11 of the GPIO

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  long inc, min, max;
  gpio.open();
  cout << "Press Ctrl+C to abort ..." << endl;

  min = led.dac().min();
  max = led.dac().max() + 1;
  inc = (max - min) / 128;

  //led.analogWrite (max);
  //for (;;);

  for (;;) {

    // fade in from min to max in increments of 5 points:
    for (long fadeValue = min; fadeValue <= max; fadeValue += inc) {
      // sets the value (range from 0 to 255):
      led.analogWrite (fadeValue);
      // wait for 30 milliseconds to see the dimming effect
      clk.delay (30);
      cout << '+' << flush; // print one point per second
    }
    cout << endl;
    clk.delay (1000);

    // fade out from max to min in increments of 5 points:
    for (long fadeValue = max; fadeValue >= min; fadeValue -= inc) {
      // sets the value (range from 0 to 255):
      led.analogWrite (fadeValue);
      // wait for 30 milliseconds to see the dimming effect
      clk.delay (30);
      cout << '-' << flush; // print one point per second
    }
    cout << endl;
    clk.delay (1000);
  }

  return 0;
}
