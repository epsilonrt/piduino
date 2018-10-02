# PiDuino

Arduino on Pi boards, the best of both worlds.

## Abstract

PiDuino was born from a question from one of my students who asked me why 
programming input-output on NanoPi was not as simple as on [Arduino](https://www.arduino.cc/).

PiDuino therefore aims to respond to this need: 

<p align="center"><b>An Application Programming Interface (API) on Pi boards as close as possible to that of Arduino.</b></p>

This API must allow to use GPIO, Serial port, I2C bus and SPI... on 
[Raspberry Pi](https://www.raspberrypi.org/), 
[Nano Pi](http://www.nanopi.org/),[Orange Pi](http://www.orangepi.org/), 
[Banana Pi](http://www.banana-pi.org/), [Beagle Board](https://beagleboard.org/)... 
boards as on an Arduino board.

What PiDuino offers:

* A programming interface [API](https://en.wikipedia.org/wiki/Application_programming_interface)
same as Arduino except adding `#include <Piduino.h>`at the beginning of the program. 
It does not prohibit offering extensions of the API but provided that stay as independent
as possible from the platform and not to make the code incompatible with Arduino. 
It makes sense to think that users who want to stay in the Arduino world use C ++, 
PiDuino is intended for this use case. Nevertheless some functions can be used in 
C (`pinMode ()`, `digitalWrite ()`, ...).

* The **description of Pi boards** that is based on an "Object" model stored 
**in a database** (SQLite by default), allowing a simple user to add a new Pi 
board "variant" **WITHOUT** programming.

* An object design in C++ with a clear separation of the part specific to the platform. 
Support for new SoCs is summarizes to add a part "driver" in the directory `src/arch`

* Utilities for manipulating GPIO signals: `pido`, retrieve information from the 
board: `pinfo` or manage the Pi boards database: `pidbman`

**PiDuino is in development**, version 0.3 currently but the completed parts are 
functional on Broadcom SoC BCM283X and AllWinner Hx.

The list of models present in the database is as follows:

| Nano Pi                           | Raspberry Pi                   | 
|-----------------------------------|--------------------------------| 
| NanoPi Neo Core                   | RaspberryPi 2                  | 
| NanoPi Neo Core with Mini Shield  | RaspberryPi 3                  | 
| NanoPi Neo Core2                  | RaspberryPi A                  | 
| NanoPi Neo Core2 with Mini Shield | RaspberryPi A+                 | 
| NanoPi M1                         | RaspberryPi B                  | 
| NanoPi M1+                        | RaspberryPi B+                 | 
| NanoPi Neo                        | RaspberryPi Compute Module     | 
| NanoPi Neo 2                      | RaspberryPi Compute Module 3   | 
| NanoPi Neo Air                    | RaspberryPi Zero               | 
| NanoPi Neo+ 2                     | RaspberryPi Zero Wifi          | 

To learn more about PiDuino, you can check the [wiki](https://github.com/epsilonrt/piduino/wiki), 
but if you're in a hurry, let's go to the quick start version...

## Quickstart guide

### Installation

    sudo apt update
    sudo apt install libcppdb-dev pkg-config cmake libudev-dev
    git clone https://github.com/epsilonrt/piduino.git
    cd piduino
    mkdir cmake-build-Release
    cd cmake-build-Release
    cmake ..
    make 
    sudo make install
    sudo ldconfig
    
### Utilities

    pinfo
    man pinfo
    pido readall
    man pido

### Blink Example

```c++
#include <Piduino.h> // all the magic is here ;-)

const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

void setup() {
  // initialize digital pin ledPin as an output.
  pinMode (ledPin, OUTPUT);
}

void loop () {
  // Press Ctrl+C to abort ...
  digitalWrite (ledPin, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay (1000);                 // wait for a second
  digitalWrite (ledPin, LOW);   // turn the LED off by making the voltage LOW
  delay (1000);                 // wait for a second
}
```

Obviously, you need to know the pin number where you connected the LED ! 

    $ pido readall 1
                                              CON1 (#1)
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    |     |     |     3.3V |      |      |   |  1 || 2  |   |      |      | 5V       |     |     |
    |  12 |   8 |  I2C0SDA | ALT2 |  OFF |   |  3 || 4  |   |      |      | 5V       |     |     |
    |  11 |   9 |  I2C0SCK | ALT2 |  OFF |   |  5 || 6  |   |      |      | GND      |     |     |
    |  91 |   7 |  GPIOG11 |  OFF |  OFF |   |  7 || 8  |   | OFF  | ALT2 | UART1TX  | 15  | 86  |
    |     |     |      GND |      |      |   |  9 || 10 |   | OFF  | ALT2 | UART1RX  | 16  | 87  |
    |   0 |   0 |   GPIOA0 |  OFF |  OFF |   | 11 || 12 |   | OFF  | OFF  | GPIOA6   | 1   | 6   |
    |   2 |   2 |   GPIOA2 |  OFF |  OFF |   | 13 || 14 |   |      |      | GND      |     |     |
    |   3 |   3 |   GPIOA3 |  OFF |  OFF |   | 15 || 16 |   | OFF  | OFF  | GPIOG8   | 4   | 88  |
    |     |     |     3.3V |      |      |   | 17 || 18 |   | OFF  | OFF  | GPIOG9   | 5   | 89  |
    |  22 |  12 |   GPIOC0 |  OFF |  OFF |   | 19 || 20 |   |      |      | GND      |     |     |
    |  23 |  13 |   GPIOC1 |  OFF |  OFF |   | 21 || 22 |   | OFF  | OFF  | GPIOA1   | 6   | 1   |
    |  24 |  14 |   GPIOC2 |  OFF |  OFF |   | 23 || 24 |   | UP   | OFF  | GPIOC3   | 10  | 25  |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+


The iNo column corresponds to the 'Arduino' number, the number 0 pin corresponds
therefore at pin 11 of the GPIO connector (GPIOA0).

To compile, you must type the command:

    $ g++ -o blink blink.cpp $(pkg-config --cflags --libs piduino)

With [Codelite](https://codelite.org/) it's easier and funny, right ? 

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/master/doc/images/codelite-2.png)

You should read the [wiki on the examples](https://github.com/epsilonrt/piduino/wiki/Examples) to learn more...
