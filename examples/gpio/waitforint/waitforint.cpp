// waitForInt() example

// This program toggles the state of a led at each rising or falling edge sent 
// to an input used as an interrupt request.
// The time difference between 2 successive edges is displayed.

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
const int irqPin = 3; // Header Pin 15: GPIO22 for RPi, GPIOA3 for NanoPi

Clock clk; // the clock of our program used for time calculation...
Gpio g; // our GPIO port
Pin & led = g.pin (ledPin); // led is a reference on pin 11 of the GPIO
Pin & irq = g.pin (irqPin); // irq is a reference on pin 15 of the GPIO
unsigned long t1, t2; // for calculating time differences between interruptions

// -----------------------------------------------------------------------------
// Signal Handler
static void
sighandler (int sig) {

  g.close();
  cout << endl << "everything was closed."<< endl << "Have a nice day !\n" << endl;
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  
  g.open();
  led.setMode(Pin::ModeOutput); // the led pin is an output
  led.write(0); // turn off the led
  irq.setPull(Pin::PullUp); // activates the pull-up resistor if the input is 
                            // not connected or connected to a push button.
  irq.setMode(Pin::ModeInput);  // the irq pin is an input that will be used 
                                // as an interrupt request.

  // sighandler() intercepts CTRL+C
  signal (SIGINT, sighandler);
  signal (SIGTERM, sighandler);
  cout << "Press Ctrl+C to abort ..." << endl;
  
  t1 = clk.millis(); // first time
  for (;;) { // Infinite loop, Press Ctrl+C to abort ...
    
    irq.waitForInterrupt(Pin::EdgeBoth);  // waiting for a rising or falling edge 
                                    // under interruption
    t2 = clk.millis(); // second time
    led.toggle(); // toggle the led state
    cout << t2 - t1 << ":\t" << irq.read() << endl;  // prints the time 
                      // difference between  edges and the state of the irq pin.
    t1 = t2; // the new time becomes the first for the next loop
  }

  return 0;
}
/* ========================================================================== */
