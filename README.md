# PiDuino

Arduino on Pi boards, the best of both worlds.

## Abstract

PiDuino was born from a question from one of my students who asked me why programming input-output on NanoPi was not as simple as on [Arduino](https://www.arduino.cc/).

PiDuino therefore aims to respond to this need: _API (PIO, Serial port, I2C bus and SPI ...) on Pi boards ([Raspberry Pi](https://www.raspberrypi.org/), [Nano Pi](http://www.nanopi.org/),[Orange Pi](http://www.orangepi.org/), [Banana Pi](http://www.banana-pi.org/), [Beagle Board](https://beagleboard.org/) ...) as close as possible to that of Arduino._

There are some projects that have sought to meet this need but only to propose a solution for a Pi card model.

The best known is probably [wiringPi](https://github.com/WiringPi).
wiringPi is a solution planned for Raspberry Pi and even though it there are derivative versions on other Pi boards, these versions are "lame" versions from a computing point of view of the original version.

These are the reasons that led me not to choose wiringPi:

* Even if there is a similarity with Arduino programming, there is has differences that increase with time.  
* wiringPi was designed in pure C, which is a definite brake on scalability and is not very consistent with Arduino (The Arduino language is C++ !)  
* wiringPi was designed for the Raspberry Pi and its adaptation for other Pi boards is increasingly unthinkable, as and when of the arrival of new Pi boards.

What PiDuino offers:

* A programming interface [API](https://en.wikipedia.org/wiki/Application_programming_interface)
same as Arduino except adding `#include <Arduino.h>`at the beginning of the program. 
It does not prohibit offering extensions of the API but provided that stay as independent
as possible from the platform and not to make the code incompatible with Arduino. 
It makes sense to think that users who want to stay in the Arduino world use C ++, 
PiDuino is intended for this use case. Nevertheless some functions can be used in 
C (`pinMode ()`, `digitalWrite ()`, ...).

* The **description of Pi boards** that is based on an "Object" model stored 
**in a database** (SQLite by default), allowing a simple user to add a new Pi board "variant" **WITHOUT** programming.

* An object design in C++ with a clear separation of the part specific to the platform. 
Support for new SoCs is summarizes to add a part "driver" in the directory `src/gpio/arch`

* Utilities for manipulating GPIO signals: `gpio`, retrieve information from the board: `pinfo` or manage the Pi boards database: `pidbman`

## Road Map

**PiDuino is in development**, version 0.3 currently but the completed parts are functional on Broadcom SoC BCM283X and AllWinner Hx.

The list of models present in the database is as follows:

* NanoPi Core  
* NanoPi Core with Mini Shield  
* NanoPi Core2  
* NanoPi Core2 with Mini Shield  
* NanoPi M1  
* NanoPi M1+  
* NanoPi Neo  
* NanoPi Neo 2  
* NanoPi Neo Air  
* NanoPi Neo+ 2  
* RaspberryPi 2  
* RaspberryPi 3  
* RaspberryPi A  
* RaspberryPi A+  
* RaspberryPi B  
* RaspberryPi B+  
* RaspberryPi Compute Module  
* RaspberryPi Compute Module 3  
* RaspberryPi Zero  
* RaspberryPi Zero Wifi  

**What was done ?**

* GPIO modeling, GPIO connectors and pins  
* Creation of database model and addition of all variants of Raspberry Pi, Nano Pi Neo, Neo2, Neo Plus 2, M1, M1 Plus.  
* Creation of SoC access layers for Broadcom BCM283X and AllWinner Hx.  
* Creating `gpio` and `pinfo` utilities  
* Switching iomap in C++  
* Creating a purely virtual class IoDevice  (and CharDevice)
* analogWrite() with GPIO software PWM feature (Polling with thread) 
* Emulate setup() and loop() for Arduino compatibility (in Arduino.h)
* Cleaning the architecture detection  
* I2C Bus API  
* SPI Bus API  
* Serial Port API  
* Arduino Classes (String, Print, Stream....)  
* Update README  

The rest of the things to do:

* `pidbman` for managing the database of boards with Qt (in development [pidbman](https://github.com/epsilonrt/piduino/tree/dev-pidbman/database/pidbman)) 
* Enabling daemon mode for loop()
* analogWrite() with Software PWM feature (Kernel driver module)
* analogWrite() with external DAC ([IIO](https://01.org/linuxgraphics/gfx-docs/drm/driver-api/iio/intro.html))  
* analogRead() with external ADC or Sensor ([IIO](https://01.org/linuxgraphics/gfx-docs/drm/driver-api/iio/intro.html))  
* Man Pages for Utilities  
* Database Doxygen Documentation (English)  
* Creating a web page

## Dependencies

The compilation requires a compiler [g ++](https://gcc.gnu.org) managing [C++11](https://en.wikipedia.org/wiki/C%2B%2B11), the compilation has been made with gcc version 6.3.0 20170516. This compiler is usually installed with a complete tool chain on Linux systems used on Pi boards.

The dependencies are as follows:

* libcppdb-dev which provides [CppDB](http://cppcms.com/sql/cppdb/) for access to the database  
* libudev-dev which provides [libudev](https://www.freedesktop.org/software/systemd/man/libudev.html) for enumerate devices  
* Qt if you want to compile the database management tool [pidbman](https://github.com/epsilonrt/piduino/tree/dev-pidbman/database/pidbman).

## Installation

    sudo apt-get update
    sudo apt-get install libcppdb-dev pkg-config cmake libsqlite3-dev sqlite3 libudev-dev
    git clone https://github.com/epsilonrt/piduino.git
    cd piduino
    git checkout dev
    mkdir cmake-build-Release
    cd cmake-build-Release
    cmake ..
    make 
    sudo make install
    sudo ldconfig

To uninstall:

     cd cmake-build-Release
     sudo make uninstall

You can also generate Debian packages with:

     make package
     sudo dpkg -i * .deb

To generate documentation in HTML format, you must install the pre-requisites:

    sudo apt-get install doxygen doxygen-latex doxygen-doc doxygen-gui graphviz
    make doc
    
## Configuration

The PI board model is dynamically detected when starting the Piduino program by comparing the signature of the board with the database.

It is possible to force the Pi model choice by using the `/etc/piduino.conf` configuration file.

This may be necessary when it is not possible for the program to detect the configuration of the board.
For example, in the case of the NanoPi Neo Core/Core2, we can indicate that the board is on its shield, in this case, the display of the connector by the command `gpio readall` will be adapted.

Pi board model detection uses two methods:  
* The first method, which applies to Raspberry Pi boards, reads the `/proc/cpuinfo` file to get the microprocessor model in the `Hardware` field and especially the `Revision` field. This revision number is compared with the database to deduce the RaspberryPi model.
* The second method, which applies to boards using ArmBian, comes from reading `/etc/armbian-release` or `/etc/friendlyelec-release` to get board model in `BOARD`. We compare this signature with the database to deduce the RaspberryPi model.

In the `/etc/piduino.conf` configuration file, we will find these two possibilities, which must be filled in (one or the other, but never the two!).

For example if we want to indicate that our NanoPi Neo Core2 is installed on its shield, we will put the `tag` field value `nanopineocore2shield`:

    # PiDuino configuration file
    connection_info="sqlite3:db=/usr/local/share/piduino/piduino.db"

    # Allows you to force the board tag (Armbian)
    # !! Be careful, this is at your own risk !!
    # !! Forcing an incorrect value may destroy GPIO pins !!
    tag="nanopineocore2shield"

    # Allows forcing the revision of the board (Raspbian)
    # !! Be careful, this is at your own risk !!
    # !! Forcing an incorrect value may destroy GPIO pins !!
    #revision=0xa02082

It can be seen that the configuration file also contains the address of the database to use.
The database is by default a local SQLite3 file, but the database can be installed on a MySQL server for example (for the format of the connection_info line see the documentation of [CPPDB](http://cppcms.com/sql/cppdb/connstr.html))

## First Example, Blink !

The [examples](https://github.com/epsilonrt/piduino/tree/master/examples) folder 
contains examples from the Arduino world that are can be used directly with 
PiDuino. The only thing to add is the line:

```c++
#include <Arduino.h>
```

Here is the source code of the example [Blink](https://github.com/epsilonrt/piduino/blob/master/examples/blink/blink.cpp) 
that flashes a led:

```c++
#include <Arduino.h> // all the magic is here ;-)

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
This number depends on your model of Pi board, to know it quickly, we can type 
the command `gpio readall 1`, which gives us, for example, the following display 
on a Raspberry Pi B:

                                        P1 (#1)
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | V | Ph || Ph | V | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    |     |     |     3.3V |      |   |  1 || 2  |   |      | 5V       |     |     |
    |   2 |   8 |    GPIO2 |   IN | 1 |  3 || 4  |   |      | 5V       |     |     |
    |   3 |   9 |    GPIO3 |   IN | 1 |  5 || 6  |   |      | GND      |     |     |
    |   4 |   7 |    GPIO4 |   IN | 1 |  7 || 8  | 1 | ALT0 | TXD0     | 15  | 14  |
    |     |     |      GND |      |   |  9 || 10 | 1 | ALT0 | RXD0     | 16  | 15  |
    |  17 |   0 |   GPIO17 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO18   | 1   | 18  |
    |  27 |   2 |   GPIO27 |   IN | 0 | 13 || 14 |   |      | GND      |     |     |
    |  22 |   3 |   GPIO22 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO23   | 4   | 23  |
    |     |     |     3.3V |      |   | 17 || 18 | 0 | IN   | GPIO24   | 5   | 24  |
    |  10 |  12 |   GPIO10 |   IN | 0 | 19 || 20 |   |      | GND      |     |     |
    |   9 |  13 |    GPIO9 |   IN | 0 | 21 || 22 | 0 | IN   | GPIO25   | 6   | 25  |
    |  11 |  14 |   GPIO11 |   IN | 0 | 23 || 24 | 1 | IN   | GPIO8    | 10  | 8   |
    |     |     |      GND |      |   | 25 || 26 | 1 | IN   | GPIO7    | 11  | 7   |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | V | Ph || Ph | V | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+


The iNo column corresponds to the 'Arduino' number, the number 0 pin corresponds
therefore at pin 11 of the GPIO connector (GPIO17).

To compile the blink program on the command line, you must type the
command:

    $ g++ -o blink blink.cpp $(pkg-config --cflags --libs piduino)

The last part of the command uses `pkg-config` to add the build options to `g++`
in order to compile the program correctly.

To have a more user-friendly development environment, it is advisable to use 
[Codelite](http://epsilonrt.fr/index.php/2018/05/21/utilisation-de-codelite-sous-nanopi/), 
the installation of PiDuino adds a program template for PiDuino:

![PiDuino template for Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/master/doc/images/codelite-1.png)

In Codelite, one can not only compile, but also edit and especially to debug 
the program:

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/master/doc/images/codelite-2.png)

## Second Example

the second example  [rtc_bq32k](https://github.com/epsilonrt/piduino/blob/master/examples/wire/rtc_bq32k/rtc_bq32k.cpp),
uses the Wire library to read the time in a BQ32000 RTC circuit. 

It allows to discover 2 important differences between an Arduino board and a Pi board:  
1. First, on a Pi board, the human-machine interface (screen and keyboard) is done on the command line (the console !). On Arduino, the serial port is used.
2. On a Pi board, a program can finish to give the user a hand. On Arduino, the program never stops (_in fact on a Linux system, the kernel program never stops either..._)

To solve the first problem, PiDuino defines a Console object whose
the usage is identical to the Serial object (it is a class derived from Stream).

In order to allow compilation on both platforms without modifying the source code,
we can add at the beginning of the sketch a block that tests if the target platform is
a Unix/Linux system (PiDuino), if so, the inclusion of the file
`Arduino.h` is done, otherwise we define a Console alias which corresponds to
Serial, ie the human-machine interface is on the serial port.

```c++
#ifdef __unix__
#include <Arduino.h>  // Piduino, all the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

#include <Wire.h>

void printBcdDigit (byte val, bool end = false) {
  val = (val / 16 * 10) + (val % 16); // BCD to DEC

  if (val < 10) {
    Console.write ('0'); // leading zero
  }
  if (end) {

    Console.println (val);
  }
  else {

    Console.print (val);
    Console.write (':');
  }
}

void setup() {

  Console.begin (115200);
  Wire.begin(); // Starting the i2c master
}

void loop() {

  Wire.beginTransmission (0x68); // start of the frame for the RTC at slave address 0x68
  Wire.write (0); // write the address of the register in the RTC, 0 first register
  Wire.endTransmission (false); // restart condition
  Wire.requestFrom (0x68, 3); // 3-byte read request

  if (Wire.available() == 3) { // if the 3 bytes have been read
    byte sec = Wire.read();
    byte min = Wire.read();
    byte hour = Wire.read() & 0x3F; // remove CENT_EN and CENT LSB bits

    // time display
    printBcdDigit (hour);
    printBcdDigit (min);
    printBcdDigit (sec, true);
  }
  exit (0); // exit the loop() function without ever coming back.
  // On Arduino, exit() performs an infinite loop as explained on
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html
  // on a Pi board, exit () stops the program by returning the supplied value.
}
```

To solve the second problem, it is possible to use on the 2
platforms the `exit ()` function (which is defined in the [standard library](https://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html)).

This function, compatible with both platforms, allows to stop the execution
the loop () function. 

On a Unix / Linux system, it stops the program and returns to the command line, 
on Arduino, it performs an infinite loop (after calling the 
[destructor](https://en.wikipedia.org/wiki/Destructor_(computer_programming)) of C ++ objects).

## Utilities

The `pinfo`command allows to know the information on the Pi board:

    Name            : RaspberryPi B
    Family          : RaspberryPi
    Database Id     : 9
    Manufacturer    : Sony
    Board Revision  : 0xe
    SoC             : Bcm2708 (Broadcom)
    Memory          : 512MB
    GPIO Id         : 2
    PCB Revision    : 2
    Serial Ports    : /dev/ttyAMA0

The command displays only information in the list, `-h`, allows to know the 
different options.

The `gpio`command allows you to modify the mode, the pull resistance, to read 
or write logical or analogical states (PWM) ... Its syntax is simple.

    $ gpio readall
                                        P1 (#1)
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | V | Ph || Ph | V | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    |     |     |     3.3V |      |   |  1 || 2  |   |      | 5V       |     |     |
    |   2 |   8 |    GPIO2 |   IN | 1 |  3 || 4  |   |      | 5V       |     |     |
    |   3 |   9 |    GPIO3 |   IN | 1 |  5 || 6  |   |      | GND      |     |     |
    |   4 |   7 |    GPIO4 |   IN | 1 |  7 || 8  | 1 | ALT0 | TXD0     | 15  | 14  |
    |     |     |      GND |      |   |  9 || 10 | 1 | ALT0 | RXD0     | 16  | 15  |
    |  17 |   0 |   GPIO17 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO18   | 1   | 18  |
    |  27 |   2 |   GPIO27 |   IN | 0 | 13 || 14 |   |      | GND      |     |     |
    |  22 |   3 |   GPIO22 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO23   | 4   | 23  |
    |     |     |     3.3V |      |   | 17 || 18 | 0 | IN   | GPIO24   | 5   | 24  |
    |  10 |  12 |   GPIO10 |   IN | 0 | 19 || 20 |   |      | GND      |     |     |
    |   9 |  13 |    GPIO9 |   IN | 0 | 21 || 22 | 0 | IN   | GPIO25   | 6   | 25  |
    |  11 |  14 |   GPIO11 |   IN | 0 | 23 || 24 | 1 | IN   | GPIO8    | 10  | 8   |
    |     |     |      GND |      |   | 25 || 26 | 1 | IN   | GPIO7    | 11  | 7   |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | V | Ph || Ph | V | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+

                                        P5 (#2)
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    | sOc | iNo |   Name   | Mode | V | Ph || Ph | V | Mode |   Name   | iNo | sOc |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
    |     |     |       5V |      |   |  1 || 2  |   |      | 3.3V     |     |     |
    |  28 |  17 |   GPIO28 |   IN | 0 |  3 || 4  | 0 | IN   | GPIO29   | 18  | 29  |
    |  30 |  19 |   GPIO30 |   IN | 0 |  5 || 6  | 0 | IN   | GPIO31   | 20  | 31  |
    |     |     |      GND |      |   |  7 || 8  |   |      | GND      |     |     |
    +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+


For example, to put pin number 0 in output:

    $ gpio mode 0 out

To put this output in high state:

    $ gpio write 0 1

To put it in the low state:

    $ gpio write 0 0

You can also toggle the state:

    $ gpio toggle 0

To put it in input with pull-up resistor:

     $ gpio mode 0 in
     $ gpio pull 0 up

And to read it:

    $ gpio read 0

Or we can wait for a falling front on this entry:

    $ gpio wfi 0 falling

The `-h`option allows to know the different possible actions:

    usage : gpio [ options ] [ command ]  [ parameters ] [ options ]
    Allow the user easy access to the GPIO pins.

    valid options are :
      -g	Use the SOC pins numbers rather than PiDuino pin numbers.
      -s	Use the System pin numbers rather than PiDuino pin numbers.
      -f	Force to use SysFS interface (/sys/class/gpio).
      -1	Use the connector pin numbers rather than PiDuino pin numbers.
          a number is written in the form C.P, eg: 1.5 denotes pin 5 of connector #1.
      -v	Output the current version including the board informations.
      -h	Print this message and exit

    valid commands are :
      mode <pin> <in/out/off/pwm/alt{0..9}>
        Get/Set a pin mode into input, output, off, alt0..9 or pwm mode.
      pull <pin> <up/down/off>
        Get/Set the internal pull-up, pull-down or off controls.
      read <pin>
        Read the digital value of the given pin (0 or 1)
      readall [#connector]
        Output a table of all connectors with pins informations.
      write <pin> <value>
        Write the given value (0 or 1) to the given pin (output).
      toggle <pin>
        Changes the state of a GPIO pin; 0 to 1, or 1 to 0 (output).
      blink <pin> [period]
        Blinks the given pin on/off (explicitly sets the pin to output).
      wfi <pin> <rising/falling/both> [timeout_ms]
        Waits  for  the  interrupt  to happen. It's a non-busy wait.
      pwm <pin> <value>
        Write a PWM value (0-1023) to the given pin (pwm pin only).
