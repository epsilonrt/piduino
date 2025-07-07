// GpioDev2 Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <atomic>
#include <chrono>
#include <map>

#include <gpio/gpio_dev2.h>
#include <piduino/system.h>
#include <piduino/clock.h>
#include <piduino/tsqueue.h>
#include <piduino/gpio.h>
#include <piduino/gpiodevice.h>
#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Gpio2;
using namespace Piduino;

// Configuration settings -----------------------------------
#warning "Check this pin numbers and chip numbers, they must match your hardware setup! then comment this line"

const int Pin1 = 2; // iNo number for first input pin, use pido to get the pin number
const int Pin2 = 0; // iNo number for output pin, use pido to get the pin number
const int Pin3 = 1; // iNo number for input pin connected to the output pin, use pido to get the pin number

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
struct GpioFixture : public TestFixture {

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
  GpioDev2 line;

  Line1Fixture() :
    pin (gpio.pin (Pin1)), line (pin) {

    REQUIRE CHECK (line.chip().isOpen());
    CHECK (line.isOpen() == false);
  }

  void begin (int number, const char title[]) {
    TestFixture::begin (number, title);
    std::cout << "Pin1: " << pin << std::endl << std::endl;
  }

  void OpenCloseTest() {
    // Check if the line is closed initially
    CHECK (line.isOpen() == false);

    // Open the line and check if it is open
    CHECK (line.open());
    REQUIRE CHECK (line.isOpen() == true);

    // Check if the pin and its config were correctly copied and the line is correctly configured
    CHECK_EQUAL (&pin, &line.pin());
    CHECK_EQUAL (pin.mode(), line.mode());
    if (pin.flags() & GpioDevice::hasPullRead) {
      CHECK_EQUAL (pin.pull(), line.pull());
    }
    CHECK_EQUAL (pin.read(), line.read());

    line.close();
    CHECK (line.isOpen() == false);
  }

  void ModePullTest() {

    CHECK_EQUAL (line.mode(), pin.mode());
    if (pin.flags() & GpioDevice::hasPullRead) {
      CHECK_EQUAL (line.pull(), pin.pull());
    }
  }

  void WriteTest() {

    line.write (true);
    CHECK_EQUAL (true, line.read());
    CHECK_EQUAL (true, pin.read());
    line.write (false);
    CHECK_EQUAL (false, line.read());
    CHECK_EQUAL (false, pin.read());
  }

  ~Line1Fixture() {
    // Clean up
    line.close();
    CHECK (line.isOpen() == false);
  }

};

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test1) {
  begin (1, "Open/Close tests");
  OpenCloseTest();

  pin.setMode (Pin::ModeOutput);
  pin.write (true);
  OpenCloseTest();

  pin.setMode (Pin::ModeInput);
  pin.setPull (Pin::PullUp);
  OpenCloseTest();

  pin.setPull (Pin::PullDown);
  OpenCloseTest();

  pin.setPull (Pin::PullOff);
  OpenCloseTest();
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test2) {
  begin (2, "Set mode and pull tests");

  // Open the line and check if it is open
  CHECK (line.open());
  REQUIRE CHECK (line.isOpen() == true);

  line.setMode (Pin::ModeInput);
  CHECK_EQUAL (Pin::ModeInput, line.mode());
  CHECK_EQUAL (Pin::ModeInput, pin.mode());

  line.setPull (Pin::PullUp);
  CHECK_EQUAL (Pin::PullUp, line.pull());
  if (pin.flags() & GpioDevice::hasPullRead) {
    CHECK_EQUAL (Pin::PullUp, pin.pull());
  }

  ModePullTest();

  line.setPull (Pin::PullDown);
  CHECK_EQUAL (Pin::PullDown, line.pull());
  if (pin.flags() & GpioDevice::hasPullRead) {
    CHECK_EQUAL (Pin::PullDown, pin.pull());
  }
  ModePullTest();

  line.setPull (Pin::PullOff);
  CHECK_EQUAL (Pin::PullOff, line.pull());
  if (pin.flags() & GpioDevice::hasPullRead) {
    CHECK_EQUAL (Pin::PullOff, pin.pull());
  }
  ModePullTest();

  line.setMode (Pin::ModeOutput);
  CHECK_EQUAL (Pin::ModeOutput, line.mode());
  CHECK_EQUAL (Pin::ModeOutput, pin.mode());
  ModePullTest();

  line.setPull (Pin::PullUp);
  CHECK_EQUAL (Pin::PullUp, line.pull());
  if (pin.flags() & GpioDevice::hasPullRead) {
    CHECK_EQUAL (Pin::PullUp, pin.pull());
  }
  ModePullTest();

  line.setPull (Pin::PullDown);
  CHECK_EQUAL (Pin::PullDown, line.pull());
  if (pin.flags() & GpioDevice::hasPullRead) {
    CHECK_EQUAL (Pin::PullDown, pin.pull());
  }
  ModePullTest();
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (Line1Fixture, Test3) {
  begin (3, "Read and Write tests");

  // Open the line and check if it is open
  CHECK (line.open());
  REQUIRE CHECK (line.isOpen() == true);

  line.setMode (Pin::ModeInput);
  line.setPull (Pin::PullUp);
  CHECK_EQUAL (true, line.read());

  line.setPull (Pin::PullDown);
  CHECK_EQUAL (false, line.read());

  line.setMode (Pin::ModeOutput);
  WriteTest ();

  line.setPull (Pin::PullUp);
  WriteTest ();

  line.setPull (Pin::PullOff);
  WriteTest ();
  end();
}

// -----------------------------------------------------------------------------
struct LinInOutFixture : public GpioFixture {
  Pin &opin;
  Pin &ipin;
  GpioDev2 input;
  GpioDev2 output;

  LinInOutFixture() :
    opin (gpio.pin (Pin2)), ipin (gpio.pin (Pin3)),
    input (ipin), output (opin) {

    REQUIRE CHECK (input.chip().isOpen());
    REQUIRE CHECK (output.chip().isOpen());

    // Check if the input line is closed initially
    CHECK (input.isOpen() == false);
    CHECK (output.isOpen() == false);

    // Open the input line and check if it is open
    CHECK (input.open());
    REQUIRE CHECK (input.isOpen() == true);

    // Open the output line and check if it is open
    CHECK (output.open());
    REQUIRE CHECK (output.isOpen() == true);

    output.setMode (Pin::ModeOutput);
    input.setMode (Pin::ModeInput);
  }

  void begin (int n, const char title[]) {
    TestFixture::begin (n, title);
    std::cout << "Pin2 (output): " << opin << std::endl;
    std::cout << "Pin3  (input): " << ipin << std::endl;
    std::cout << "<WARNING> Pin iNo#" << opin.logicalNumber() << " must be connected to Pin iNo#" << ipin.logicalNumber() << " with a wire!" << std::endl << std::endl;
  }

  void WriteTest() {

    output.write (true);
    CHECK_EQUAL (true, output.read());
    CHECK_EQUAL (true, opin.read());
    CHECK_EQUAL (true, input.read());
    CHECK_EQUAL (true, ipin.read());

    output.write (false);
    CHECK_EQUAL (false, output.read());
    CHECK_EQUAL (false, opin.read());
    CHECK_EQUAL (false, input.read());
    CHECK_EQUAL (false, ipin.read());
  }

  ~LinInOutFixture() {
    // Clean up
    input.close();
    CHECK (input.isOpen() == false);
    output.close();
    CHECK (output.isOpen() == false);
  }
};

// -----------------------------------------------------------------------------
TEST_FIXTURE (LinInOutFixture, Test4) {
  begin (4, "Output -> Input tests");

  input.setPull (Pin::PullUp);
  WriteTest();

  input.setPull (Pin::PullUp);
  WriteTest();

  input.setPull (Pin::PullOff);
  WriteTest();
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
  }

  void begin (int n, const char title[]) {
    TestFixture::begin (n, title);
    std::cout << "Pin2 (output): " << opin << std::endl;
    std::cout << "Pin3  (input): " << ipin << std::endl;
    std::cout << "Pulse width: " << pw << " ms, loop count: " << loopCount << ", initial state: " << (initialState ? "high" : "low") << std::endl;
    std::cout << "<WARNING> Pin iNo#" << opin.logicalNumber() << " must be connected to Pin iNo#" << ipin.logicalNumber() << " with a wire!" << std::endl << std::endl;
  }

};

void InterruptFixture::isr (Pin::Event event, void *userData) {
  InterruptFixture *f = static_cast<InterruptFixture *> (userData);

  // a new event is available
  f->queue.push (event); // push the event to the queue
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (InterruptFixture, Test5) {
  bool outState = initialState;
  bool inState;
  Pin::Event prevEvent;
  int eventCount = 0;

  begin (5, "Interrupt tests");

  // Attach the interrupt handler
  CHECK (input.attachInterrupt (isr, Pin::EdgeBoth, this));

  outState = ! outState; // Toggle outState to set the initial state
  output.write (outState); // generate the initial event
  REQUIRE CHECK_EQUAL (outState, output.read());

  // Wait for the initial event to be processed
  prevEvent = queue.pop(); // Pop the initial event
  inState = (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
  CHECK_EQUAL (outState, inState); // Check if the input state matches the output state
  CHECK_EQUAL (prevEvent.offset, ipin.chipOffset()); // Check if the event offset matches the input line offset
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

    LineEvent event = queue.pop(); // Pop the event from the queue
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
TEST_FIXTURE (InterruptFixture, Test6) {
  bool outState = initialState;
  bool inState;
  Pin::Event prevEvent;
  int eventCount = 0;

  begin (6, "Debounce Interrupt tests");

  input.setDebounce (debounce); // Set debounce to 20 ms
  CHECK_EQUAL (debounce, input.debounce()); // Check if the debounce value is set correctly
  // Attach the interrupt handler
  CHECK (input.attachInterrupt (isr, Pin::EdgeBoth, this));

  outState = ! outState; // Toggle outState to set the initial state
  output.write (outState); // generate the initial event
  REQUIRE CHECK_EQUAL (outState, output.read());

  // Wait for the initial event to be processed
  prevEvent = queue.pop(); // Pop the initial event
  inState = (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
  CHECK_EQUAL (outState, inState); // Check if the input state matches the output state
  CHECK_EQUAL (prevEvent.offset, ipin.chipOffset()); // Check if the event offset matches the input line offset
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

    LineEvent event = queue.pop(); // Pop the event from the queue
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
