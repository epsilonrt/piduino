// interrupt

// This program captures the rising and falling edges sent on a digital pin. 
// A led copies the state of the input signal and the console displays the 
// time differences between edges.

// How to make this program work ?

// 1- Connect the irqPin to another pin of the GPIO connector with a wire 
// (eg Pin 10: Pin Header 24, GPIOC3 for NanoPi, GPIO8 for RaspberryPi).

// 2- Configure pin 10 as output with the gpio utility:
// $ gpio mode 10 out

// 3- Run this program:
// $ sudo ./interrupt
// You must be root to run this program (use sudo) !

// 4- Generate a clock signal on pin 10 with the gpio utility:
// $ gpio blink 10 100

// 5- The console should display the time differences between edges (100ms):
//  Press Ctrl+C to abort ...
//  27047:	0
//  100:	  1
//  100:	  0
//  100:	  1

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
// Interrupt Service Routine
// Called at each interruption triggered by a rising or falling edge
void isr() {

  t2 = clk.millis(); // second time
  led.toggle(); // toggle the led state
  cout << t2 - t1 << ":\t" << irq.read() << endl;  // prints the time difference
                                  // between edges and the state of the irq pin.
  t1 = t2; // the new time becomes the first for the next irq
}

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

  t1 = clk.millis(); // first time
  irq.setPull (Pin::PullUp); // activates the pull-up resistor if the input is
  // not connected or connected to a push button.
  irq.setMode (Pin::ModeInput); // the irq pin is an input that will be used
  // as an interrupt request,
  irq.attachInterrupt (isr, Pin::EdgeBoth); //  triggered on rising or falling edge

  // sighandler() intercepts CTRL+C
  signal (SIGINT, sighandler);
  signal (SIGTERM, sighandler);
  cout << "Press Ctrl+C to abort ..." << endl;

  for (;;) { // Infinite loop, Press Ctrl+C to abort ...

    clk.sleep (-1); // nothing to do, we sleep ...
  }

  return 0;
}
/* ========================================================================== */
