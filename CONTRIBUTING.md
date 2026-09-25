# Contributing to PiDuino

First off, thank you for considering contributing to PiDuino! 🎉

PiDuino is a C++ library (and the `pido` / `pinfo` tools) to use the GPIO, I2C,
SPI and UART of Pi boards with an Arduino-like API. It targets **ARM boards
only** (`armhf` and `arm64`: Raspberry Pi, NanoPi, Orange Pi, Banana Pi...):
it does not build on x86.

## How to Contribute

### Reporting Bugs

- Check the [existing issues](https://github.com/epsilonrt/piduino/issues) to avoid duplicates.
- Use a clear and descriptive title.
- Describe the steps to reproduce the bug.
- Include your board and OS (output of `pinfo` and `cat /etc/os-release`), the
  PiDuino version (`dpkg -s libpiduino | grep Version` or `git describe`) and any relevant logs.

### Suggesting Features

- Open an issue with the `enhancement` label.
- Explain the use case and why this feature would be useful.
- To add support for a new board, say which board and SoC, and attach the
  output of `pinfo` if PiDuino already runs on it.
- Planned work is tracked in the
  [PiDuino roadmap](https://github.com/users/epsilonrt/projects/4) project:
  check it before opening an issue, and feel free to pick a task there.

### Pull Requests

1. **Fork** the repository.
2. **Create a branch** from `dev` (not `master`):
   ```bash
   git checkout dev
   git pull origin dev
   git checkout -b my-feature
   ```
3. **Make your changes** and commit with clear messages (English, imperative
   mood, for example `Fix PWM range check`).
4. **Test your changes**: make sure the project builds (see below) and, if you
   can, run the hardware tests on a real board.
5. **Push** your branch and open a PR **targeting `dev`**.

> ⚠️ **Important:** All pull requests must target the `dev` branch.
> `master` is reserved for stable releases: it only receives merges of `dev`,
> done by the maintainer.

The continuous integration runs on every pull request and must be green
before the merge:

| Check                          | What it does                                            |
|--------------------------------|---------------------------------------------------------|
| `build-linux (arm64, Release)` | Build in a Debian bookworm `arm64` container            |
| `build-linux (arm64, Debug)`   | Same, Debug build (the unit tests are compiled)         |
| `build-linux (armhf, Release)` | Build in a Debian bookworm `armhf` container            |
| `build-linux (armhf, Debug)`   | Same, Debug build (the unit tests are compiled)         |
| `package-linux (arm64)`        | Build the `.deb` packages (checks names, arch, version) |
| `package-linux (armhf)`        | Same for `armhf`                                        |

A `build-docs` job also builds the Doxygen documentation when a pull request
changes the headers, the `Doxyfile`, the `README.md`, `doc/` or `examples/`.
The warnings it reports are not blocking.

### Coding Guidelines

Follow the existing code style:

- **C++11** (`CMAKE_CXX_STANDARD 11`): do not use later features.
- **Indentation**: 2 spaces, no tabs. The body of a `namespace` and the `case`
  labels of a `switch` are indented too.
- **Spaces**: always a space before the parenthesis of `if`, `for`, `while` and
  `switch`, and before the parenthesis of a call or a declaration that has
  arguments (`pin.setMode (Pin::ModeOutput)`). No space when the argument list
  is empty (`gpio.open()`).
- **Braces** on the same line as the statement.
- **Headers** start with `#pragma once`.
- **Comments in English**. Document the public API with Doxygen (`@brief`,
  `@param`, `@return`, `@note`). Some old comments are still in French: you can
  translate them when you touch the code around.
- **Private implementation**: the classes use the pimpl idiom (`d_ptr` / `q_ptr`,
  a `Private` class in a `*_p.h` file). Keep the public headers in
  `include/piduino/` free of implementation details.
- **License header**: every new source file starts with the LGPL header of the
  existing files (copy it from `include/piduino/gpio.h`, with your name and the
  year).
- Keep commits focused: one logical change per commit.
- Update the documentation (Doxygen comments, `README.md`, the
  [Wiki](https://github.com/epsilonrt/piduino/wiki)) if your change affects the usage.

### Build Instructions

Reference platform: Debian 12 (bookworm) / Raspberry Pi OS, on `armhf` or `arm64`.

```bash
sudo apt install build-essential cmake pkg-config git libcppdb-dev libudev-dev
# Optional: unit tests (Debug build), documentation (Release build), memory checks
sudo apt install libunittest++-dev sudo doxygen graphviz valgrind

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

- A **Release** build also generates the API documentation (needs `doxygen` and `graphviz`).
- A **Debug** build compiles the unit tests (needs `libunittest++-dev`).
- To install: `sudo cmake --install build`.
- To build the Debian packages (Release only, installed under `/usr`):
  ```bash
  sudo apt install fakeroot
  cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
  cmake --build build --parallel
  cd build && fakeroot make package
  ```
- The version comes from `git describe`, so keep the tags of the repository
  (`git fetch --tags` in a fork).

See also the [Wiki](https://github.com/epsilonrt/piduino/wiki/Build-from-source).

> **Known limitation:** on Ubuntu 24.04 and Debian 13 (trixie), the CMake
> configuration fails when it looks for the package `libcppdb0`, which was
> renamed `libcppdb0t64`. Debian 12 is the reference platform for now.

### Running Tests

All the unit tests in `tests/` (UnitTest++, run by CTest) need a **real board**
with wires between GPIO pins, and are run as root (CTest calls `sudo`):

| Test              | What it needs                                                       |
|-------------------|---------------------------------------------------------------------|
| `test1-gpio2`     | GPIO pins wired together (`Gpio2` classes)                          |
| `test2-gpiodev2`  | GPIO pins wired together (`GpioDev2`)                               |
| `test3-pin`       | GPIO pins wired together (`Pin`)                                    |
| `test4-socpwm`    | A hardware PWM pin wired to an input pin                            |
| `test5-gpiopwm`   | A pin wired to an input pin (software PWM)                          |
| `test6-max1161x`  | A MAX1161x ADC on an I2C bus, with test voltages on its inputs      |
| `test7-max7311`   | A MAX7311 GPIO expander at 0x20 on the default I2C bus              |

The pin numbers and the chip numbers are constants at the top of each
`tests/*/main.cpp`: edit them to match your wiring (use `pido readall` to
get the pin numbers) and remove the `#warning` line once done. The step-by-step
procedure of each test (wiring, duration, expected result) is in
[tests/README.md](tests/README.md).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cd build
ctest --output-on-failure          # all the tests, on the board
ctest -R test3-pin -V              # a single test
```

The tests are labelled `hardware` for CTest. The CI has no board: it compiles
them and runs `ctest -LE hardware`, which selects no test today. A test that
does not need hardware should not carry this label.

## Branch Strategy

| Branch    | Purpose                                                        |
|-----------|----------------------------------------------------------------|
| `master`  | Stable releases only (default branch, protected)               |
| `dev`     | Active development, **target of the pull requests**            |

Maintainers only: a release is made by merging `dev` into `master` with a pull
request, then by pushing an **annotated** tag on `master`:

```bash
git checkout master
git pull origin master
git tag -a v0.7.4 -m "Version 0.7.4"
git push origin v0.7.4
```

The tag must be annotated (`-a`): `git describe` ignores lightweight tags and the
package version would be wrong. The `Build Linux Package` workflow then builds
the `armhf` and `arm64` packages, checks that their version is the one of the
tag, and publishes them in the GitHub release of the tag (only for a tag
carried by `master`).

## License

PiDuino is licensed under the [GNU Lesser General Public License v3.0](COPYING.LESSER)
(LGPL-3.0 or later). By contributing, you agree that your contributions will be
licensed under the same terms.
