// Blinkd

// This example shows how to make a daemon that lets you blinking a led on Pi board

// Created 10 February 2019

// This example code is in the public domain.
#include <Piduino.h>  // All the magic is here ;-)
using namespace Piduino;

int ledPin = 0;
unsigned long highDelay = 500;
unsigned long lowDelay = 500;

void setup() {

  auto pin_option = CmdLine.add<Value<int>> ("p", "pin", "set pin number");
  CmdLine.add<Value<unsigned long>> ("H", "high", "set high state delay", 500, &highDelay);
  CmdLine.add<Value<unsigned long>> ("L", "low", "set low state delay", 500, &lowDelay);
  CmdLine.parse (argc, argv);

  if (! pin_option->is_set()) {
    Console.println ("Usage: Blinkd -p pin_number [-H high_delay] [-L low_delay]");
    exit (EXIT_FAILURE);
  }
  ledPin = pin_option->value();

  // initialize digital pin ledPin as an output.
  pinMode (ledPin, OUTPUT);
}

void loop () {
  // Press Ctrl+C to abort ...
  digitalWrite (ledPin, HIGH);
  delay (highDelay);
  digitalWrite (ledPin, LOW);
  delay (lowDelay);
}
