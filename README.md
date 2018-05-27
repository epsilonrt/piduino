# piduino

  Arduino API on Pi boards, the best of both worlds !
  
## Abstract

  To Do...

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

## Build doc

    sudo apt-get install doxygen doxygen-latex doxygen-doc doxygen-gui graphviz
