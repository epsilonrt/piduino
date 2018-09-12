// HoneyWell HSC SPI Barometric Pressure Sensor Display

// Shows the output of a Barometric Pressure Sensor on  the console
// In this example we used a HSCMRNN1.6BASA3 sensor (SMT, 0-1600hPa, SPI, 3.3V)
// Uses the SPI library. For details on the sensor, see:
// https://sensing.honeywell.com/spi-comms-digital-ouptu-pressure-sensors-tn-008202-3-en-final-30may12.pdf

// Console is used with PIDUINO as Serial with ARDUINO.

// Created 22 August 2018
// by Pascal JEAN https://github.com/epsilonrt

// This example code is in the public domain.
#ifdef __unix__
#include <Arduino.h>  // Piduino, all the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

#include <SPI.h>

/* constants ================================================================ */
#define HSC_S1  (1 << 7)
#define HSC_S0  (1 << 6)
#define HSC_STATUS_MASK (HSC_S1|HSC_S0)
#define HSC_ISSTALE(_byte) (((_byte) & HSC_STATUS_MASK) == HSC_S1)

// Constants used in the calculation of pressure and temperature
// See ̣§ 3.0 in https://sensing.honeywell.com/spi-comms-digital-ouptu-pressure-sensors-tn-008202-3-en-final-30may12.pdf
const double SensorPressureMin = 0;
const double SensorPressureMax = 1600.0; // 1600 hPa sensor, change for yours

const double HscOutputMin = 1638.0;
const double HscOutputMax = 14745.0;
const double HscTemperatureSlope = (200.0 / 2047.0);
const double HscTemperatureZero = 50.0;

// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to the HSC SS Pin (5) ! <-
// the other you need are controlled by the SPI library:
const int chipSelectPin = 10; // Header Pin 24: GPIO8 for RPi, GPIOC3 for NanoPi
// const int chipSelectPin = 27;

byte buf[4];
SPISettings settings (700000, MSBFIRST, SPI_MODE0);

void setup() {

  Console.begin (115200);

  // start the SPI library:
  SPI.begin();

  // initalize the chip select pin
  pinMode (chipSelectPin, OUTPUT);
  digitalWrite (chipSelectPin, HIGH);
}

void loop () {
  word p, t;

  // Press Ctrl+C to abort ...

  // Waiting new measurement available
  do {

    digitalWrite (chipSelectPin, LOW);
    SPI.beginTransaction (settings);
    SPI.transfer (buf, sizeof (buf));
    SPI.endTransaction();
    digitalWrite (chipSelectPin, HIGH);
  }
  while (HSC_ISSTALE (buf[0]));

  p = (buf[0] << 8) + buf[1];
  t = (buf[2] << 8) + buf[3];
  t >>= 5;
  
  Console.print ("Raw=0x");
  for (byte b = 0; b < sizeof (buf); b++) {
    Console.print (buf[b], HEX);
  }
  
  if ( (buf[0] & HSC_STATUS_MASK) == 0) { // no error
    double value;

    value = ( (static_cast<double> (p) - HscOutputMin) * (SensorPressureMax - SensorPressureMin)) /
            (HscOutputMax - HscOutputMin) + SensorPressureMin;
    Console.print (" Press=");
    Console.print (value, 1);
    Console.print ("(0x");
    Console.print (p, HEX);

    value = static_cast<double> (t) * HscTemperatureSlope - HscTemperatureZero;
    Console.print (") Temp=");
    Console.print (value, 1);
    Console.print ("(0x");
    Console.print (t, HEX);
    Console.println (")");
  }
  else {

    Console.println ("Status Error !");
  }

  delay (1000); // wait for a second
}
