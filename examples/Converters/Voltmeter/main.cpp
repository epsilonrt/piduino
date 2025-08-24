// Voltmeter 

// This example demonstrates how to read voltage levels using an ADC.
// We measure the voltage on Single-Ended channel 0 and 1 the MAX11615 ADC, then
// measure the differential voltage AIN0-AIN1 and AIN1-AIN0.
// The output can be used to generate the graphs with the vscode extension teleplot.

// To view the output with teleplot, you need to install the teleplot extension and open the Teleplot panel 
// in VSCode and install the ncat utility:
// $ sudo apt-get install ncat
// Open the teleplot window from vscode (do not open any serial port)
// $ ./Voltmeter | ncat -u -w2 127.0.0.1 47269

// It use the abstract class Converter, which provides a common interface for all converters.
// This allows for easy switching between different ADCs without changing the main code.
// We use the Max11615 converter in this example.
// A string configuration is used to select the desired ADC.
// All converters can be listed with pido utility :
// $ pido converters
// Name                Type      Parameters
// --------------------------------------------------------------------------------
// gpiopwm             adc       pin[:range:freq]
// max1161x            adc       bus=id:max={12,13,14,15,16,17}:ref={ext,vdd,int1,int2,int3,int4}:fsr=value:bipolar={1,0}:clk={int,ext}
// max7311             gpioexp   bus=id:addr={0x20...0xDE}:bustimeout={0,1}
// mcp4725             adc       bus=id:addr={0x60..0x67}:fsr=value:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}
// mcp4728             adc       bus=id:addr={0x60..0x67}:ref={vdd,int}:fsr=value:gain={1,2}:mode={norm,fast,eeprom,pd1k,pd100k,pd500k}

// Converter::factory() is use to construct the desired converter object based on the configuration string.

// This example code is in the public domain.
#include <Piduino.h>  // All the magic is here ;-)
#include <Converters.h>

// Name                Type      Parameters
// --------------------------------------------------------------------------------
// max1161x            adc       bus=id:max={12,13,14,15,16,17}:ref={ext,vdd,int1,int2,int3,int4}:fsr=value:bipolar={1,0}:clk={int,ext}
// without options, the ADC use :
// - the default bus (e.g. /dev/i2c-1 on Raspberry Pi), it can be changed with bus=id option, e.g. bus=0 for /dev/i2c-0
// - the default MAX model (e.g. 15 for MAX11615), it can be changed with max=id option, e.g. max=12
// - the default reference (e.g. ext for MAX11615), it can be changed with ref=id option, e.g. ref=vdd
// - the default full-scale range (e.g. 3.3 for MAX11615), it can be changed with fsr=value option, e.g. fsr=25.
//   The full-scale range is the maximum input voltage of the ADC, it depends of the electronic components connected to the input of the ADC (amplifier, transducter...)
// - the default bipolar mode (e.g. 1 for MAX11615), it can be changed with bipolar={1,0} option, e.g. bipolar=0
// - the default clock source (e.g. int for MAX11615), it can be changed with clk=id option, e.g. clk=int

// Choose or modify ADC configuration
// here, ref=int4 choose the internal reference 2.048V and this voltage is connected to REFOUT
constexpr char AdcConfig[] = "max1161x:ref=int4"; 

constexpr int ChanA = 0; // Choose ADC channel
constexpr int ChanB = 1; // Choose ADC channel

Converter *adc;

void setup() {
  Console.begin (115200);
  Console.println ("Voltmeter with ADC conversion");

  adc = Converter::factory (AdcConfig);

  if (adc->type() != Converter::Type::AnalogToDigital) {
    Console.println ("The selected converter is not an ADC!");
    while (1) {
      delay (1000);
    }
  }

  if (!adc->open ()) {
    Console.println ("Failed to open ADC converter!");
    while (1) {
      delay (1000);
    }
  }

  Console.println ("Setup complete\nCTRL+C to exit");
}


void loop() {
  double v0,v1,dv0,dv1;

  v0 = adc->readValue (ChanA);
  if (adc->error() != 0) {
    Console.print ("Failed to read ADC channel A! Error=");
    Console.print (adc->error());
    Console.print (":");
    Console.println (adc->errorString());
  }
  else {
    Console.print ("V0:");
    Console.println (v0);
  }

  v1 = adc->readValue (ChanB);
  if (adc->error() != 0) {
    Console.print ("Failed to read ADC channel B! Error=");
    Console.print (adc->error());
    Console.print (":");
    Console.println (adc->errorString());
  }
  else {
    Console.print ("V1:");
    Console.println (v1);
  }

  adc->setBipolar (true);
  dv0 = adc->readValue (ChanA, true); // AIN0 - AIN1
  dv1 = adc->readValue (ChanB, true); // AIN1 - AIN0
  adc->setBipolar(false);

  Console.print("dV0:");
  Console.println(dv0);
  Console.print("dV1:");
  Console.println(dv1);

  delay (100);
}
