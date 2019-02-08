// Console Demo

// This program demonstrates how to use Stream derived class functions
// to display or enter text and numbers.

// Created 11 November 2018

// This example code is in the public domain.
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

void outputDemo();
void inputDemo();

void setup() {

  Console.begin ();
}

void loop() {

  outputDemo();
  inputDemo();
  exit (0); // exit the loop() function without ever coming back.
  // On Arduino, exit() performs an infinite loop as explained on
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html
  // on a Pi board, exit () stops the program by returning the supplied val
}

void inputDemo() {
  int c = 0;
  size_t s;

  Console.println ("read() available chars until press <Return>...");

  while (c != '\n') {

    s = Console.available();
    if (s > 0) {

      c = Console.peek();
      if ( (c > 0) && std::isprint (c)) {
        Console.write (c);
        Console.print (", hex: ");
        Console.println (c, HEX);
      }
      c = Console.read();
      if ( (c > 0) && std::isprint (c)) {
        Console.write (c);
        Console.print (", hex: ");
        Console.println (c, HEX);
      }
    }
  }

  Console.setTimeout (-1); // infinite timeout

  Console.println ("parseInt(), enter an integer then press <Return>...");
  long i = Console.parseInt();
  Console.print ("integer=");
  Console.println (i);

  Console.println ("parseFloat(), enter a float then press <Return>...");
  float f = Console.parseFloat();
  Console.print ("float=");
  Console.println (f);

  Console.println ("readStringUntil(), enter a string then press <Return>...");
  String str = Console.readStringUntil ('\n');
  Console.print ("string=");
  Console.println (str);

  Console.setTimeout (3000); //  timeout 3s
  Console.println ("readString(), enter a string then wait...");
  str = Console.readString();
  Console.print ("\nstring=");
  Console.println (str);

  char buf[21];
  Console.println ("readBytes(), enter a string, max 5 chars or/and wait...");
  s = Console.readBytes (buf, 5);
  buf[s] = 0;
  Console.print ("\nstring=");
  Console.println (buf);
  buf[0] = 0;

  Console.setTimeout (-1); // infinite timeout
  Console.println ("readBytesUntil(), enter a string, max 20 chars then press <Return>...");
  s = Console.readBytesUntil ('\n', buf, 20);
  buf[s] = 0;
  Console.print ("string=");
  Console.println (buf);

  bool found;
  Console.setTimeout (10000); //  timeout 3s

  Console.println ("find(), enter a string with 'Azerty' or/and wait...");
  found = Console.find ("Azerty");
  Console.print ("\nfound=");
  Console.println (found);

  Console.println ("find(), enter a string with 'Aze' or/and wait...");
  found = Console.find ("Azerty", 3);
  Console.print ("\nfound=");
  Console.println (found);

  Console.setTimeout (-1); // infinite timeout

  Console.println ("findUntil(), enter a string with 'Azerty' then press <Return>...");
  found = Console.findUntil ("Azerty", "\n");
  Console.print ("\nfound=");
  Console.println (found);

  Console.println ("findUntil(), enter a string with 'Aze' then press <Return>...");
  found = Console.findUntil ("Azerty", 3, "\nLM", 1);
  Console.print ("\nfound=");
  Console.println (found);
}

void outputDemo() {
  int c = '!';

  Console.println ("ASCII Table ~ Character Map");

  while (c <= '~') {

    Console.write (c);

    Console.print (", dec: ");
    Console.print (c);

    Console.print (", hex: ");
    Console.print (c, HEX);

    Console.print (", oct: ");
    Console.print (c, OCT);

    Console.print (", bin: ");
    Console.println (c++, BIN);
  }

  float d = PI;
  Console.println ("\nFloat Pi printing");

  Console.println (d);
  for (int i = 0; i < 10; i++) {
    Console.println (d, i);
  }
}
