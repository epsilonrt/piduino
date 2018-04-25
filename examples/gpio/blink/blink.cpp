// Blink example
// by epsilonrt <https://github.com/epsilonrt>

// This program flashes an LED connected to a GPIO pin with a frequency of 2 Hz.

// Created 6 March 2018

// This example code is in the public domain.
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <piduino/clock.h>
#include <piduino/gpio.h>

using namespace std;
using namespace Piduino;

// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

Clock clk; // the clock of our program used for time calculation...
Gpio g; // our GPIO port
Pin & led = g.pin (ledPin); // led is a reference on pin 11 of the GPIO

// -----------------------------------------------------------------------------
// Signal Handler
static void
sighandler (int sig) {

  g.close();
  cout << endl << "everything was closed."<< endl << "Have a nice day !" << endl;
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {


  g.open();
  led.setMode (Pin::ModeOutput); // the led pin is an output
  led.write (0); // turn off the led

  // sighandler() intercepts CTRL+C
  signal (SIGINT, sighandler);
  signal (SIGTERM, sighandler);
  cout << "Press Ctrl+C to abort ..." << endl;
  for (;;) {

    led.toggle(); // blinking led
    clk.delay (1000);
    cout << '.' << flush; // print one point per second
  }

  return 0;
}
/* ========================================================================== */
