// Pin Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <iomanip>
#include <string>

#include <piduino/system.h>
#include <piduino/clock.h>
#include <piduino/tsqueue.h>
#include <piduino/gpio.h>
#include <piduino/database.h>

#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Piduino;

// Configuration settings -----------------------------------
#warning "Check this pin numbers and chip numbers, they must match your hardware setup! then comment this line"
const int Pin1 = 2; // iNo number for first input pin, use pido to get the pin number
const int Pin2 = 0; // iNo number for output pin, use pido to get the pin number
const int Pin3 = 1; // iNo number for input pin connected to the output pin, use pido to get the pin number

namespace Bcm2835 {

  const Pin::Number PinNumber1 (::Pin1, 27, 27, 7, 1, 0, 27); // BCM2835 GPIO pin 2
  const int DriveMin = 0; // Minimum drive strength
  const int DriveMax = 3; // Maximum drive strength
  const int DriveDefault = 1; // Default drive strength
}

namespace AllWinnerHx {

  namespace H5 {
    const Pin::Number PinNumber1 (::Pin1, 2, 2, 7, 1, 1, 2); // AllWinner H5 GPIO pin 2
  }

  namespace H3 {
    const Pin::Number PinNumber1 (::Pin1, 2, 2, 7, 1, 0, 2); // AllWinner H3 GPIO pin 2
  }
  const int DriveMin = 0; // Minimum drive strength
  const int DriveMax = 3; // Maximum drive strength
  const int DriveDefault = 1; // Default drive strength
}

// WaitLineFixture settings for waitForInterrupt() tests
const unsigned int WaitLineFixturePw = 50; // Pulse width in milliseconds
const int WaitLineFixtureLoopCount = 50; // Number of loops to test
const bool WaitLineFixtureInitialState = false;
const uint32_t WaitLineFixtureDebounceMs = 20; // Debounce period in milliseconds
const unsigned int WaitLineFixtureDtmax = 10; // Maximum time to generate short pulses, to generate short pulses at the beginning of each loop, must be even and less than (pw - debounce)

static_assert (WaitLineFixtureDtmax < (WaitLineFixturePw - WaitLineFixtureDebounceMs), "dtmax must be less than (pw - debounce) and even");
static_assert (WaitLineFixtureDtmax % 2 == 0, "dtmax must be even");

// -----------------------------------------------------------------------------
struct TestFixture {

  void begin (int number, const char title[]) {
    std::cout << std::endl << "--------------------------------------------------------------------------->>>" << std::endl;
    std::cout << "Test" << number << ": " << title << std::endl;
  }

  void end() {
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
  }
};

// -----------------------------------------------------------------------------
struct GpioFixture: public TestFixture {

  GpioFixture()  {
    CHECK (gpio.open());
    // Check if the GPIO is open
    REQUIRE CHECK (gpio.isOpen());
    REQUIRE CHECK_EQUAL (gpio.numbering(), Pin::NumberingLogical);
  }

  ~GpioFixture() {
    // Clean up
    gpio.close();
    CHECK (gpio.isOpen() == false);
  }
};

// -----------------------------------------------------------------------------
struct Line1Fixture : public GpioFixture {
  Pin &pin;
  int driveMin;
  int driveMax;
  int driveDefault;
  Pin::Number pinNumber;

  Line1Fixture() : pin (gpio.pin (Pin1)), driveMin (-1), driveMax (-1), driveDefault (-1) {
    REQUIRE CHECK (pin.isOpen());

    switch (db.board().soc().family().id()) {
      case SoC::Family::BroadcomBcm2835:
        pinNumber = Bcm2835::PinNumber1;
        // Set drive strength values for Broadcom BCM2835 SoC
        driveMin = Bcm2835::DriveMin;
        driveMax = Bcm2835::DriveMax;
        driveDefault = Bcm2835::DriveDefault;
        break;
      case SoC::Family::AllwinnerH:
        switch (db.board().soc().id()) {
          case SoC::H5:
            pinNumber = AllWinnerHx::H5::PinNumber1;
            break;
          case SoC::H3:
            pinNumber = AllWinnerHx::H3::PinNumber1;
            break;
          default:
            throw std::out_of_range ("Unsupported SoC: " + db.board().soc().name());
            break;
        }
        // Set drive strength values for AllWinner Hx SoCs
        driveMin = AllWinnerHx::DriveMin;
        driveMax = AllWinnerHx::DriveMax;
        driveDefault = AllWinnerHx::DriveDefault;
        break;
      default:
        throw std::out_of_range ("Unsupported SoC family: " + db.board().soc().family().name());
        break;
    }

  }

  ~Line1Fixture() {}

  void begin (int number, const char title[]) {
    TestFixture::begin (number, title);
    std::cout << "Pin1: " << pin << std::endl << std::endl;
  }
};

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test1) {
  begin (1, "Type and Numbering tests");

  REQUIRE CHECK_EQUAL (Pin::TypeGpio, pin.type());

  CHECK_EQUAL ("Gpio", pin.typeName());

  CHECK_EQUAL (pinNumber.logical, pin.number (Pin::NumberingLogical));
  CHECK_EQUAL (pinNumber.logical, pin.logicalNumber());
  CHECK_EQUAL (pinNumber.mcu, pin.number (Pin::NumberingMcu));
  CHECK_EQUAL (pinNumber.mcu, pin.mcuNumber());
  CHECK_EQUAL (pinNumber.system, pin.number (Pin::NumberingSystem));
  CHECK_EQUAL (pinNumber.system, pin.systemNumber());
  CHECK_EQUAL (pinNumber.row, pin.row());
  CHECK_EQUAL (pinNumber.column, pin.column());
  CHECK_EQUAL (pinNumber.chip, pin.chipNumber());
  CHECK_EQUAL (pinNumber.offset, pin.chipOffset());
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test2) {
  begin (2, "Mode and Name tests");

  for (const auto &mode : pin.modes()) {

    std::cout << "  Mode: " << mode.first << ", Name: " << mode.second << std::endl;
    CHECK_EQUAL (pin.modeName (mode.first), mode.second);

    try {

      pin.setMode (mode.first);
    }
    catch (const std::invalid_argument &e) {

      std::cerr << "  Unable to setting mode " << mode.second << ": " <<   e.what() << std::endl;
      continue; // Skip unsupported modes
    }

    CHECK_EQUAL (mode.first, pin.mode());
    CHECK_EQUAL (mode.second, pin.modeName());
    try {
      const std::string &name = pin.name (mode.first); // std::out_of_range exception if no name for the mode
      CHECK_EQUAL (name, pin.name());
      std::cout << "  Pin name for mode " << mode.second << ": " << name << std::endl;
    }
    catch (...) {
      // No name for the mode, continue
    }
  }
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test3) {
  begin (3, "Gpiodev mode tests");

  // Check if the pin is a GPIO pin
  CHECK (pin.enableGpioDev (true));
  REQUIRE CHECK (pin.isGpioDevEnabled());
  CHECK_EQUAL (2, pin.modes().size()); // GpioDev2 supports only 2 modes: Input and Output

  for (const auto &mode : pin.modes()) {

    std::cout << "  Mode: " << mode.first << ", Name: " << mode.second << std::endl;
    try {

      pin.setMode (mode.first);
    }
    catch (const std::invalid_argument &e) {

      std::cerr << "  Unable to setting mode " << mode.second << ": " <<   e.what() << std::endl;
      continue; // Skip unsupported modes
    }

    CHECK_EQUAL (mode.first, pin.mode());
    CHECK_EQUAL (mode.second, pin.modeName());
    try {
      const std::string &name = pin.name (mode.first); // std::out_of_range exception if no name for the mode
      CHECK_EQUAL (name, pin.name());
      std::cout << "  Pin name for mode " << mode.second << ": " << name << std::endl;
    }
    catch (...) {
      // No name for the mode, continue
    }
  }
  CHECK_EQUAL (false, pin.enableGpioDev (false)); // Disable GpioDev
  REQUIRE CHECK (pin.isGpioDevEnabled() == false);
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test4) {
  begin (4, "Pull resistors tests");

  for (const auto &pull : pin.pulls()) {

    std::cout << "  Pull: " << pull.first << ", Name: " << pull.second << std::endl;
    CHECK_EQUAL (pin.pullName (pull.first), pull.second);
    if (pull.first == Pin::PullUnknown) {
      std::cout << "  Pull Unknown, skipping..." << std::endl;
      continue; // Skip unknown pull
    }

    try {

      pin.setPull (pull.first);
    }
    catch (const std::invalid_argument &e) {

      std::cerr << "  Unable to setting pull " << pull.second << ": " <<   e.what() << std::endl;
      continue; // Skip unsupported pulls
    }

    CHECK_EQUAL (pull.first, pin.pull());
    CHECK_EQUAL (pull.second, pin.pullName());
  }
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test5) {
  begin (5, "GpioDev Pull resistors tests");

  // Check if the pin is a GPIO pin
  CHECK (pin.enableGpioDev (true));
  REQUIRE CHECK (pin.isGpioDevEnabled());

  for (const auto &pull : pin.pulls()) {

    std::cout << "  Pull: " << pull.first << ", Name: " << pull.second << std::endl;

    if (pull.first == Pin::PullUnknown) {
      std::cout << "  Pull Unknown, skipping..." << std::endl;
      continue; // Skip unknown pull
    }

    try {

      pin.setPull (pull.first);
    }
    catch (const std::invalid_argument &e) {

      std::cerr << "  Unable to setting pull " << pull.second << ": " <<   e.what() << std::endl;
      continue; // Skip unsupported pulls
    }

    CHECK_EQUAL (pull.first, pin.pull());
  }

  CHECK_EQUAL (false, pin.enableGpioDev (false)); // Disable GpioDev
  REQUIRE CHECK (pin.isGpioDevEnabled() == false);
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test6) {
  begin (6, "Output drive tests");

  try {
    int drive = pin.drive(); // Get the current output drive
    std::cout << "  Current Output Drive: " << drive << std::endl;
    CHECK_EQUAL (driveDefault, drive); // Default drive is 0

    for (int i = driveMin; i <= driveMax; i++) {
      std::cout << "  Setting Output Drive: " << i << std::endl;
      pin.setDrive (i); // Set the output drive
      CHECK_EQUAL (i, pin.drive()); // Check if the drive is set correctly
    }
    pin.setDrive (driveDefault); // Reset to default drive
  }
  catch (std::domain_error &e) {
    std::cerr << "  Unable to set Output Drive: " << e.what() << std::endl;
    return; // Skip if the mode is not supported
  }
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test7) {
  begin (7, "Read/Write tests");

  pin.setMode (Pin::ModeInput);
  REQUIRE CHECK_EQUAL (Pin::ModeInput, pin.mode());

  pin.setPull (Pin::PullUp); // Set pull-up resistor
  REQUIRE CHECK_EQUAL (Pin::PullUp, pin.pull());
  CHECK_EQUAL (true, pin.read()); // Read should return true with pull-up resistor
  pin.setPull (Pin::PullDown); // Set pull-down resistor
  REQUIRE CHECK_EQUAL (Pin::PullDown, pin.pull());
  CHECK_EQUAL (false, pin.read()); // Read should return false with pull-down resistor

  pin.setMode (Pin::ModeOutput);
  REQUIRE CHECK_EQUAL (Pin::ModeOutput, pin.mode());
  pin.write (true); // Write true to the pin
  CHECK_EQUAL (true, pin.read()); // Read should return true
  pin.write (false); // Write false to the pin
  CHECK_EQUAL (false, pin.read()); // Read should return false
  pin.toggle(); // Toggle the pin state
  CHECK_EQUAL (true, pin.read()); // Read should return true after toggle
  pin.toggle(); // Toggle the pin state again
  CHECK_EQUAL (false, pin.read()); // Read should return false after toggle

  pin.setMode (Pin::ModeInput);
  CHECK_EQUAL (Pin::ModeInput, pin.mode());
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test8) {
  begin (8, "GpioDev Read/Write tests");

  CHECK (pin.enableGpioDev ());
  REQUIRE CHECK (pin.isGpioDevEnabled());

  pin.setMode (Pin::ModeInput);
  REQUIRE CHECK_EQUAL (Pin::ModeInput, pin.mode());

  pin.setPull (Pin::PullUp); // Set pull-up resistor
  REQUIRE CHECK_EQUAL (Pin::PullUp, pin.pull());
  CHECK_EQUAL (true, pin.read()); // Read should return true with pull-up resistor
  pin.setPull (Pin::PullDown); // Set pull-down resistor
  REQUIRE CHECK_EQUAL (Pin::PullDown, pin.pull());
  CHECK_EQUAL (false, pin.read()); // Read should return false with pull-down resistor

  pin.setMode (Pin::ModeOutput);
  REQUIRE CHECK_EQUAL (Pin::ModeOutput, pin.mode());
  pin.write (true); // Write true to the pin
  CHECK_EQUAL (true, pin.read()); // Read should return true
  pin.write (false); // Write false to the pin
  CHECK_EQUAL (false, pin.read()); // Read should return false

  pin.setMode (Pin::ModeInput);
  CHECK_EQUAL (Pin::ModeInput, pin.mode());

  CHECK_EQUAL (false, pin.enableGpioDev (false)); // Disable GpioDev
  REQUIRE CHECK (pin.isGpioDevEnabled() == false);
  end();
}

// -----------------------------------------------------------------------------
struct LinInOutFixture : public GpioFixture {
  Pin &output;
  Pin &input;
  std::string errorMessage;

  LinInOutFixture() :
    output (gpio.pin (Pin2)), input (gpio.pin (Pin3)) {

    errorMessage = "<ERROR> Pin iNo#" + std::to_string (output.logicalNumber()) + " must be connected to Pin iNo#" + std::to_string (input.logicalNumber()) + " with a wire!";

    REQUIRE CHECK (input.isOpen() == true);
    REQUIRE CHECK (output.isOpen() == true);

    output.setMode (Pin::ModeOutput);
    input.setMode (Pin::ModeInput);

  }

  void WriteTest() {
    bool inState, outState;

    output.write (true);
    outState = output.read();
    inState = input.read();
    CHECK_EQUAL (true, outState);
    CHECK_EQUAL (true, inState);
    M_Assert (inState == outState, errorMessage);

    output.write (false);
    outState = output.read();
    inState = input.read();
    CHECK_EQUAL (false, outState);
    CHECK_EQUAL (false, inState);
    M_Assert (inState == outState, errorMessage);
  }

  ~LinInOutFixture() {
    input.setMode (Pin::ModeInput); // Reset input mode
    input.setPull (Pin::PullDown); // Reset pull resistor
    output.setMode (Pin::ModeInput); // Reset output mode
    output.setPull (Pin::PullDown); // Reset pull resistor
    CHECK_EQUAL (Pin::ModeInput, input.mode());
    CHECK_EQUAL (Pin::PullDown, input.pull());
    CHECK_EQUAL (Pin::ModeInput, output.mode());
    CHECK_EQUAL (Pin::PullDown, output.pull());
  }

  void begin (int n, const char title[]) {
    TestFixture::begin (n, title);
    std::cout << "Pin2 (output): " << output << std::endl;
    std::cout << "Pin3  (input): " << input << std::endl;
    std::cout << "<WARNING> Pin iNo#" << output.logicalNumber() << " must be connected to Pin iNo#" << input.logicalNumber() << " with a wire!" << std::endl << std::endl;
  }
};

// -----------------------------------------------------------------------------
TEST_FIXTURE (LinInOutFixture, Test9) {
  begin (9, "Output -> Input tests");

  REQUIRE CHECK_EQUAL (Pin::ModeOutput, output.mode());
  REQUIRE CHECK_EQUAL (Pin::ModeInput, input.mode());

  input.setPull (Pin::PullUp);
  WriteTest();

  input.setPull (Pin::PullUp);
  WriteTest();

  input.setPull (Pin::PullOff);
  WriteTest();
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (LinInOutFixture, Test10) {
  begin (10, "GpioDev Output -> Input tests");

  CHECK_EQUAL (true, input.enableGpioDev (true)); // Disable GpioDev
  REQUIRE CHECK (input.isGpioDevEnabled() == true);
  CHECK_EQUAL (true, output.enableGpioDev (true)); // Disable GpioDev
  REQUIRE CHECK (output.isGpioDevEnabled() == true);

  REQUIRE CHECK_EQUAL (Pin::ModeOutput, output.mode());
  REQUIRE CHECK_EQUAL (Pin::ModeInput, input.mode());

  input.setPull (Pin::PullUp);
  WriteTest();

  input.setPull (Pin::PullUp);
  WriteTest();

  input.setPull (Pin::PullOff);
  WriteTest();

  CHECK_EQUAL (false, input.enableGpioDev (false)); // Disable GpioDev
  REQUIRE CHECK (input.isGpioDevEnabled() == false);
  CHECK_EQUAL (false, output.enableGpioDev (false)); // Disable GpioDev
  REQUIRE CHECK (output.isGpioDevEnabled() == false);
  end();
}


// -----------------------------------------------------------------------------
struct InterruptFixture : public LinInOutFixture {
  const unsigned int pw = WaitLineFixturePw; // Pulse width in milliseconds
  const int loopCount = WaitLineFixtureLoopCount; // Number of loops to test
  const bool initialState = WaitLineFixtureInitialState;
  const uint32_t debounce = WaitLineFixtureDebounceMs; // Debounce period in milliseconds
  const unsigned int dtmax = WaitLineFixtureDtmax; // Maximum time to generate short pulses, to generate short pulses at the beginning of each loop, must be even and less than (pw - debounce)

  TSQueue <Pin::Event> queue;

  static void isr (Pin::Event event, void *userData);

  InterruptFixture() {

    input.setPull (Pin::PullDown); // Set pull-up resistor for input line
    output.write (initialState); // Set initial output value
    REQUIRE CHECK_EQUAL (initialState, output.read());
  }

  ~InterruptFixture() {
    // Detach the interrupt handler
    input.detachInterrupt();
    CHECK_EQUAL (false, input.enableGpioDev (false));
  }

  void begin (int n, const char title[]) {
    TestFixture::begin (n, title);
    std::cout << "Pin2 (output): " << output << std::endl;
    std::cout << "Pin3  (input): " << input << std::endl;
    std::cout << "Pulse width: " << pw << " ms, loop count: " << loopCount << ", initial state: " << (initialState ? "high" : "low") << std::endl;
    std::cout << "<WARNING> Pin iNo#" << output.logicalNumber() << " must be connected to Pin iNo#" << input.logicalNumber() << " with a wire!" << std::endl << std::endl;
  }
};

void InterruptFixture::isr (Pin::Event event, void *userData) {
  InterruptFixture *f = static_cast<InterruptFixture *> (userData);

  // a new event is available
  f->queue.push (event); // push the event to the queue
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (InterruptFixture, Test11) {
  bool outState = initialState;
  bool inState;
  Pin::Event prevEvent;
  int eventCount = 0;

  begin (11, "Interrupt tests");

  input.attachInterrupt (isr, Pin::EdgeBoth, this);
  REQUIRE CHECK (input.isGpioDevEnabled());

  outState = ! outState; // Toggle outState to set the initial state
  output.write (outState); // generate the initial event
  REQUIRE CHECK_EQUAL (outState, output.read());

  // Wait for the initial event to be processed
  prevEvent = queue.pop(); // Pop the initial event
  inState = (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
  CHECK_EQUAL (outState, inState); // Check if the input state matches the output state
  CHECK_EQUAL (prevEvent.offset, input.chipOffset()); // Check if the event offset matches the input line offset
  CHECK_EQUAL (prevEvent.line_seqno, 1); // Check if the line sequence number is 1

  for (int i = 0; i <= loopCount; i++) {

    outState = !outState; // Toggle outState
    clk.delay (pw);
    // Toggle the output value, this should trigger an interrupt
    output.write (outState);
  }
  clk.delay (pw); // wait for the last event to be processed

  outState = initialState;
  double sum = 0;
  while (!queue.empty()) {

    Pin::Event event = queue.pop(); // Pop the event from the queue
    if (event.line_seqno > 2) {
      double duration = static_cast<double> (event.timestamp_ns - prevEvent.timestamp_ns) / 1000000; // Calculate the duration in milliseconds

      sum += duration; // Sum the durations for average calculation
      eventCount++;

      inState = (event.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
      std::cout << "Event " << event.line_seqno << ":\t" << (event.id == GPIO_V2_LINE_EVENT_RISING_EDGE ? "0->1 : " : "1->0 : ") <<  duration << " ms" << std::endl;
      CHECK_EQUAL (inState, outState); // Check if the input state matches the output state
    }
    prevEvent = event; // Update the previous event
    outState = !outState; // Toggle outState for the énext iteration
  }

  double averageDuration = sum / eventCount; // Calculate the average duration
  std::cout << "Average duration: " << averageDuration << " ms, expected: " << pw << " ms" << std::endl;

  CHECK_EQUAL (loopCount, eventCount); // Check if the number of events matches the loop count
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (InterruptFixture, Test12) {
  bool outState = initialState;
  bool inState;
  Pin::Event prevEvent;
  int eventCount = 0;

  begin (12, "Debounce Interrupt tests");

  // Attach the interrupt handler
  input.attachInterrupt (isr, Pin::EdgeBoth, debounce, this);
  REQUIRE CHECK (input.isGpioDevEnabled());

  outState = ! outState; // Toggle outState to set the initial state
  output.write (outState); // generate the initial event
  REQUIRE CHECK_EQUAL (outState, output.read());

  // Wait for the initial event to be processed
  prevEvent = queue.pop(); // Pop the initial event
  inState = (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
  CHECK_EQUAL (outState, inState); // Check if the input state matches the output state
  CHECK_EQUAL (prevEvent.offset, input.chipOffset()); // Check if the event offset matches the input line offset
  CHECK_EQUAL (prevEvent.line_seqno, 1); // Check if the line sequence number is 1

  for (int i = 0; i <= loopCount; i++) {
    bool deb = outState;

    for (unsigned int t = 0; t < dtmax; t++) { // Generate short pulses to test debounce

      deb = !deb; // Toggle the output value
      clk.delay (1);
      output.write (deb);
    }

    outState = !outState; // Toggle outState
    clk.delay (pw - dtmax);
    // Toggle the output value, this should trigger an interrupt
    output.write (outState);
  }
  clk.delay (pw); // wait for the last event to be processed

  outState = initialState;
  double sum = 0;
  while (!queue.empty()) {

    Pin::Event event = queue.pop(); // Pop the event from the queue
    if (event.line_seqno > 2) {
      double duration = static_cast<double> (event.timestamp_ns - prevEvent.timestamp_ns) / 1000000; // Calculate the duration in milliseconds

      sum += duration; // Sum the durations for average calculation
      eventCount++;

      inState = (event.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
      std::cout << "Event " << event.line_seqno << ":\t" << (event.id == GPIO_V2_LINE_EVENT_RISING_EDGE ? "0->1 : " : "1->0 : ") <<  duration << " ms" << std::endl;
      CHECK_EQUAL (inState, outState); // Check if the input state matches the output state
    }
    prevEvent = event; // Update the previous event
    outState = !outState; // Toggle outState for the énext iteration
  }

  double averageDuration = sum / eventCount; // Calculate the average duration
  std::cout << "Average duration: " << averageDuration << " ms, expected: " << pw << " ms" << std::endl;

  CHECK_EQUAL (loopCount, eventCount); // Check if the number of events matches the loop count
  end();
}

// run all tests
int main (int argc, char **argv) {
  return UnitTest::RunAllTests();
}

/* ========================================================================== */
