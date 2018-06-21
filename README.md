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
* wiringPi was designed in pure C, which is a definite brake on scalability and is not very consistent with Arduino (Arduino it's C ++ !)  
* wiringPi was designed for the Raspberry Pi and its adaptation for other Pi boards is increasingly unthinkable, as and when measurement of the arrival of new Pi boards.

What PiDuino offers:

* A programming interface [API](https://en.wikipedia.org/wiki/Application_programming_interface) same as Arduino except adding `#include <Arduino.h>` at the beginning of the program. It does not prohibit offering extensions of the API but provided that stay as independent as possible from the platform and not to make the code incompatible with Arduino. It makes sense to think that users who want to stay in the Arduino world use C ++, PiDuino is intended for this use case. Nevertheless some functions can be used in C (`pinMode ()`, `digitalWrite ()`, ...). An API in Python is planned thanks to [Swig](http://www.swig.org/).

* The **description of Pi boards** that is based on an "Object" model stored **in a database** (SQLite by default), allowing a simple user to add a new "variant" card Pi **WITHOUT** programming.

* An object design in C++ with a clear separation of the part specific to the platform. Support for new SoCs is summarizes to add a part "driver" in the directory `src/arch`

* Utilities for manipulating GPIO (`gpio`) signals, retrieve information from the board (`pinfo`) or manage the Pi boards database.

## Road Map

**PiDuino is in development**, version 0.2 currently but the completed parts are functional on Broadcom SoC BCM283X and AllWinner Hx.

What was done

* GPIO modeling, GPIO connectors and pins  
* Creation of database model and addition of all variants of Raspberry Pi, Nano Pi Neo, Neo2, Neo Plus 2, M1, M1 Plus.  
* Creation of SoC access layers for Broadcom BCM283X and AllWinner Hx.  
* Creating `gpio` and` pinfo` utilities  
* Switching iomap in C++  
* Creating a purely virtual class IoDevice  
* analogWrite() with GPIO software PWM feature (Polling with thread) 
* Emulate setup() and loop() for Arduino compatibility (in Arduino.h)
* Cleaning the architecture detection  

The rest of the things to do:

* Tool for managing the database of boards with Qt  
* Serial Port API  
* I2C Bus API  
* SPI Bus API  
* Enabling daemon mode for loop()
* analogWrite() with Software PWM feature (Kernel driver module)
* analogWrite() with external DAC ([IIO](https://01.org/linuxgraphics/gfx-docs/drm/driver-api/iio/intro.html))  
* analogRead() with external ADC or Sensor ([IIO](https://01.org/linuxgraphics/gfx-docs/drm/driver-api/iio/intro.html))  
* Update README  
* Arduino Classes (String ....)  
* Man Pages for Utilities  
* Database Doxygen Documentation (English)  
* Creating a web page

## Dependencies

The compilation requires a compiler [g ++](https://gcc.gnu.org) managing [C++ 11](https://en.wikipedia.org/wiki/C%2B%2B11), the compilation has been made with gcc version 6.3.0 20170516. This compiler is usually installed with a complete tool chain on Linux systems used on Pi boards.

The dependencies are as follows:

* libcppdb-dev which provides [CppDB](http://cppcms.com/sql/cppdb/) for access to the database  
* Qt5 if you want to compile the database management tool.


## Installation

    sudo apt-get update
    sudo apt-get install libcppdb-dev pkg-config cmake libsqlite3-dev sqlite3
    git clone https://github.com/epsilonrt/piduino.git
    cd piduino
    git checkout dev
    mkdir cmake-build-Release
    cd cmake-build-Release
    cmake ..
    make -j4
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
