// Wave Generator with DAC conversion

// This example demonstrates how to generate a waveform using a DAC

// It use the abstract class Converter, which provides a common interface for all converters.
// This allows for easy switching between different DACs without changing the main code.
// We use the Mcp4728 converter in this example.
// A string configuration is used to select the desired DAC.
// All converters can be listed with pido utility :
// $ pido converters
// Name                Type      Parameters
// --------------------------------------------------------------------------------
// mcp4725             dac       bus=id:addr={0x60..0x67}:fsr=value:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}
// mcp4728             dac       bus=id:addr={0x60..0x67}:ref={vdd,int}:fsr=value:gain={1,2}:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}

// Converter::factory() is use to construct the desired converter object based on the configuration string.

// This example code is in the public domain.
#include <Piduino.h>  // All the magic is here ;-)
#include <Converters.h>

// mcp4728             dac       bus=id:addr={0x60..0x67}:ref={vdd,int}:fsr=value:gain={1,2}:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}
// without options, the DAC use :
// - the default bus (e.g. /dev/i2c-1 on Raspberry Pi), it can be changed with bus=id option, e.g. bus=0 for /dev/i2c-0
// - the default address (e.g. 0x60 for MCP4728), it can be changed with addr=id option, e.g. addr=0x61
// - the default reference (e.g. vdd for MCP4728), it can be changed with ref=id option, e.g. ref=int
// - the default full-scale range (e.g. 3.3 for MCP4728), it can be changed with fsr=value option, e.g. fsr=25.
//   The full-scale range is the maximum output voltage of the DAC, it depends of the electronic components connected to the output of the DAC (amplifier, transducter...)
// - the default gain (e.g. 1 for MCP4728), it can be changed with gain=value option, e.g. gain=2
// - the default mode (e.g. norm for MCP4728), it can be changed with mode=id option, e.g. mode=fast (fast mode to write all channels with Converter::write())

constexpr char DacConfig[] = "mcp4728"; // Choose or modify DAC configuration
// constexpr char DacConfig[] = "mcp4728:ref=int";
// constexpr char DacConfig[] = "mcp4725";
constexpr int DacChannel = 0; // Choose DAC channel

Converter *dac;
long min, max, value, inc = 1;

void setup() {
  Console.begin (115200);
  Console.println ("Wave Generator with DAC conversion");

  dac = Converter::factory (DacConfig);

  if (dac->type() != Converter::Type::DigitalToAnalog) {
    Console.println ("The selected converter is not a DAC!");
    while (1) {
      delay (1000);
    }
  }

  if (!dac->open ()) {
    Console.println ("Failed to open DAC converter!");
    while (1) {
      delay (1000);
    }
  }

  min = dac->min();
  max = dac->max();
  value = min;
  Console.println ("Setup complete\nCTRL+C to exit");
}


void loop() {
  // Generate a triangle wave
  if (value >= max) {
    inc = -10;
  }
  else if (value <= min) {
    inc = 10;
  }
  value += inc;
  if (!dac->writeChannel (value, DacChannel)) {
    Console.println ("Failed to write to DAC channel!");
  }
  delayMicroseconds (100);
}
