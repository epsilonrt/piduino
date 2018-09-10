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

* A programming interface [API](https://en.wikipedia.org/wiki/Application_programming_interface) same as Arduino except adding `#include <Arduino.h>`at the beginning of the program. It does not prohibit offering extensions of the API but provided that stay as independent as possible from the platform and not to make the code incompatible with Arduino. It makes sense to think that users who want to stay in the Arduino world use C ++, PiDuino is intended for this use case. Nevertheless some functions can be used in C (`pinMode ()`, `digitalWrite ()`, ...). An API in Python is planned thanks to [Swig](http://www.swig.org/).

* The **description of Pi boards** that is based on an "Object" model stored **in a database** (SQLite by default), allowing a simple user to add a new "variant" card Pi **WITHOUT** programming.

* An object design in C++ with a clear separation of the part specific to the platform. Support for new SoCs is summarizes to add a part "driver" in the directory `src/arch`

* Utilities for manipulating GPIO (`gpio`) signals, retrieve information from the board (`pinfo`) or manage the Pi boards database.

## Road Map

**PiDuino is in development**, version 0.2 currently but the completed parts are functional on Broadcom SoC BCM283X and AllWinner Hx.

What was done

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

* Tool for managing the database of boards with Qt  
* Enabling daemon mode for loop()
* analogWrite() with Software PWM feature (Kernel driver module)
* analogWrite() with external DAC ([IIO](https://01.org/linuxgraphics/gfx-docs/drm/driver-api/iio/intro.html))  
* analogRead() with external ADC or Sensor ([IIO](https://01.org/linuxgraphics/gfx-docs/drm/driver-api/iio/intro.html))  
* Man Pages for Utilities  
* Database Doxygen Documentation (English)  
* Creating a web page

## Dependencies

The compilation requires a compiler [g ++](https://gcc.gnu.org) managing [C++ 11](https://en.wikipedia.org/wiki/C%2B%2B11), the compilation has been made with gcc version 6.3.0 20170516. This compiler is usually installed with a complete tool chain on Linux systems used on Pi boards.

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

## Examples

The [examples/](https://github.com/epsilonrt/piduino/tree/master/examples) folder 
contains examples from the Arduino world that are can be used directly with 
PiDuino. The only thing to add is the line:

    #include <Arduino.h>

Here is the source code of the example [Blink] () that flashes a led:

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

![PiDuino template for Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/dev/doc/images/codelite-1.png)

In Codelite, one can not only compile, but also edit and especially to debug 
the program:

![Debugging with Codelite](https://raw.githubusercontent.com/epsilonrt/piduino/dev/doc/images/codelite-2.png)
    
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
