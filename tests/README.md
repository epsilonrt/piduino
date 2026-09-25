# Piduino tests

This directory contains the tests of Piduino. They check that the library works
and that the hardware behaves as expected.

The tests use the [UnitTest++](https://github.com/unittest-cpp/unittest-cpp/wiki)
framework and are run by CTest. **All of them need a real board**: pins wired
together, or an I2C converter. The continuous integration has no board, so it only
compiles them (the tests are labelled `hardware`, see `CMakeLists.txt`).

## Before you start

1. **Free the pins.** Nothing else must be connected to the pins used by the tests
   (LED, button, shield...). A load on a pin changes its level when the pull-up or
   pull-down is tested. A LED whose anode is at 3.3 V, for example, works as a
   pull-up and the pin always reads 1.
2. **Choose the pins.** The pin numbers are constants at the top of each
   `*/main.cpp` (`Pin1`, `Pin2`...): they are `iNo` numbers, as shown by
   `pido readall`. The default values are the ones of the Raspberry Pi header,
   see the table below. The `#warning` line at the top of the file reminds you to
   check them: comment it out when done.
3. **Start from a clean state.** Do not leave a pin in PWM mode from a previous
   run: the PWM tests may fail at their first check. `pido mode <iNo> in` puts it
   back in input.
4. **Build in Debug mode** (the tests are not built in Release mode):

   ```bash
   sudo apt install cmake libunittest++-dev
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build --parallel
   ```

5. **Run the test as root**: the tests open the GPIO registers. Run the binary
   directly with `sudo`, or with CTest, which calls `sudo` itself.

## Default wiring (Raspberry Pi header)

| iNo | BCM (GPIO) | Header pin | Used as                                               |
|-----|------------|------------|-------------------------------------------------------|
| 0   | GPIO17     | 11         | output, or input of the PWM tests                     |
| 1   | GPIO18     | 12         | input, or PWM output (hardware PWM pin)               |
| 2   | GPIO27     | 13         | input for the pull-up and pull-down tests (**free**)  |
| 4   | GPIO23     | 16         | second input of `test1` (**free**)                    |

**Only one wire is needed: between iNo 0 and iNo 1 (header pins 11 and 12).**
iNo 2 and iNo 4 must stay unconnected.

The pin numbers of other boards are in the `Broadcom` and `Allwinner`
namespaces of the tests (choose the right ones in the source if your board is
not a Raspberry Pi).

## The tests, one by one

| Test              | What it checks                                        | Needs                                   | Duration |
|-------------------|-------------------------------------------------------|-----------------------------------------|----------|
| `test1-gpio2`     | GPIO character device access (`Gpio2` classes)        | wire iNo 0 - iNo 1, iNo 2 and 4 free    | ~5 s     |
| `test2-gpiodev2`  | `GpioDev2`: modes, pulls, read/write, interrupts      | wire iNo 0 - iNo 1, iNo 2 free          | ~5 s     |
| `test3-pin`       | `Pin`: numbers, modes, pulls, drive, read/write, IRQ  | wire iNo 0 - iNo 1, iNo 2 free          | ~5 s     |
| `test4-socpwm`    | Hardware PWM of the SoC (`SocPwm`)                    | wire iNo 1 (PWM pin) - iNo 0            | ~10 s    |
| `test5-gpiopwm`   | Software PWM (`GpioPwm`)                              | wire iNo 1 - iNo 0                      | ~1 min   |
| `test6-max1161x`  | `Max1161x` ADC converter (I2C)                        | see below                               | ~20 s    |

Run one test, for example `test3-pin` (from the source root, after the build):

```bash
sudo ./build/tests/test3-pin
```

Or with CTest, which calls `sudo` itself (it asks for the password if needed):

```bash
cd build
ctest -R test3-pin --output-on-failure
```

A test passes when it ends with `Success: N tests passed.`; otherwise each
failure is printed with its line in the source.

### test6-max1161x

The test uses a MAX11615 at the I2C address 0x33 with the default I2C bus of the
board (`I2cDev::Info::defaultBus()`), powered at 3.3 V. Apply to its inputs,
relative to ground:

- about **2 V** on AIN0,
- about **1 V** on AIN1.

The readings must be within 0.05 V of these values, so a bench supply or a
resistor divider is enough.

## Reference results

Compute Module 5 (Debian 13 Trixie, 64 bits) with the wiring above:
`test2`, `test3`, `test4`, `test5` and `test6` pass. `test1` passes except
`Test2`, where the kernel refuses to reconfigure a line from an input with edge
detection and debounce to a plain input with pull-down (probably a limitation of
the RP1 GPIO driver).

## Benchmarks

The `benchmarks` directory contains benchmarks (not tests) that are built with
the tests.
