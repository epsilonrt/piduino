// GPIO Pin benchmark
// This programm changes the state of a GPIO pin 100 times as fast as possible to measure
// the processing time that can be measured with an oscilloscope.
// Generates a frame of 100 pulses then waits 10 ms before repeating.
// This program is used to benchmark the GPIO performance.

// Install cpufrequtils to set the CPU frequency to maximum:
//   $ sudo apt install cpufrequtils
//   $ sudo cpufreq-set -g performance

// Compile and run this program and measure the time between the rising edges of the pulses with an oscilloscope.

// Here are the results obtained for different Raspberry Pi models:
// | board                   | pulse width | frequency | cpu clock freq | cpu load avg |
// |-------------------------|-------------|-----------|----------------|--------------|
// | Compute Module 5 (RPi5) | 200ns       | 2,45MHz   | 2,4GHz         | 0,03         |
// | Compute Module 4 (RPi4) |             |           |                |              |

#include <iostream>
#include <piduino/clock.h>
#include <piduino/gpio.h>
#include <piduino/scheduler.h>

using namespace std;
using namespace Piduino;

#warning "Check this pin number, they must match your hardware setup! then comment this line"
const int pinNumber = 1; // iNo number for the pin, use pido to get the pin number

Pin &pin = gpio.pin (pinNumber);  // pin is a reference on pin 11 of the GPIO

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Scheduler scheduler;

  cout << "GPIO Pin benchmark" << endl;
  cout << "Generates 100 pulses at maximum speed on pin:" << endl << pin << endl;
  cout << "Press Ctrl+C to abort ..." << endl;
  gpio.open();
  pin.setMode (Pin::ModeOutput); // the pin pin is an output
  pin.write (false); // turn off the pin

  for (;;) {
    scheduler.noInterrupts();
    for (int i = 0; i < 200; ++i) { // Blink the LED 200 times
      pin.write (true); // turn on the pin
      pin.write (false); // turn off the pin
    }
    scheduler.interrupts();
    clk.delay (10);
  }

  return 0;
}
/* ========================================================================== */
