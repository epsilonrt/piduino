// interrupt

// This program captures the rising and falling edges sent on a digital pin.
// A led copies the state of the input signal and the console displays the
// time differences between edges.

// How to make this program work ?

// 1- Connect the irqPin to another pin of the GPIO connector with a wire
// (eg Pin 10: Pin Header 24, GPIOC3 for NanoPi, GPIO8 for RaspberryPi).

// 2- Configure pin 10 as output with the pido utility:
// $ pido mode 10 out

// 3- Run this program:
// $ sudo ./interrupt
// You must be root to run this program (use sudo) !

// 4- Generate a clock signal on pin 10 with the pido utility:
// $ pido blink 10 100

// 5- The console should display the time differences between edges (100ms):
//  Press Ctrl+C to abort ...
//  27047:  0
//  100:    1
//  100:    0
//  100:    1

// Created 6 March 2018
// This example code is in the public domain.
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
#endif

// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi
const int irqPin = 3; // Header Pin 15: GPIO22 for RPi, GPIOA3 for NanoPi

unsigned long t1, t2; // for calculating time differences between interruptions

// -----------------------------------------------------------------------------
// Interrupt Service Routine
// Called at each interruption triggered by a rising or falling edge
void isr() {
  int value;

  t2 = millis(); // second time

  // reads the binary value
  value = digitalRead (irqPin);

  // copy irq value to led
  digitalWrite (ledPin, value);

  // prints the time difference between edges and the state of the irq pin.
  Console.print (t2 - t1);
  Console.print (":\t");
  Console.println (value);
  t1 = t2; // the new time becomes the first for the next irq
}

void setup() {

  Console.begin (115200);
  // initialize digital pin ledPin as an output.
  pinMode (ledPin, OUTPUT);
  // initialize digital pin irqPin as an input with pull-up (for button ?)
  pinMode (irqPin, INPUT_PULLUP);
  // attach interrupt service routine isr to irqPin, called for each edge
  attachInterrupt (irqPin, isr, CHANGE);
}

void loop () {

  // Press Ctrl+C to abort ...
  delay (-1); // nothing to do, we sleep ...
}
// -----------------------------------------------------------------------------
