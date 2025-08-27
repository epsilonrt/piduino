// AdcSimple

// This example demonstrates how to read voltage levels using an ADC.
// We measure the voltage on Single-Ended channel 0 of the MAX11615 ADC
// every second and print the results to the console.

#include <Piduino.h>
#include <Converters.h>

// Create a MAX11615 ADC instance
std::unique_ptr<Converter> adc (Converter::factory ("max1161x:bus=1:max=15:ref=int4"));

void setup() {

  Console.begin (115200);

  if (!adc->open()) {
    Console.println ("Failed to open ADC");
    exit (EXIT_FAILURE);
  }
}

void loop() {
  // Read digital value from channel 0
  long digitalValue = adc->readChannel (0);

  // Convert to analog voltage
  double voltage = adc->digitalToValue (digitalValue);

  Console.print ("Channel 0: ");
  Console.print (digitalValue);
  Console.print (" (");
  Console.print (voltage);
  Console.println ("V)");

  delay (1000);
}

