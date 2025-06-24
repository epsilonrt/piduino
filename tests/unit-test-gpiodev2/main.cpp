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

#include <piduino/gpiodev2.h>
#include <piduino/system.h>
#include <piduino/tsqueue.h>
#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace GpioDev2;
using namespace Piduino;

// Configuration settings -----------------------------------

// ChipFixture settings for chip tests
const std::string ChipFixtureDev = "/dev/gpiochip0"; // Default GPIO chip device path

// LineFixture settings for line tests
// Lines used for read only pins (input)
const std::array<uint32_t, 2> LineFixtureInputOffsets = { 23, 27 };
// Line used for write only pin (output)
const uint32_t LineFixtureOutputOffset = 17;
// Line connected to the output pin with a jumper or a wire
const uint32_t LineFixtureInputOffset = 18;

// WaitLineFixture settings for waitForInterrupt() tests
const unsigned int WaitLineFixturePw = 50; // Pulse width in milliseconds
const int WaitLineFixtureLoopCount = 50; // Number of loops to test
const bool WaitLineFixtureInitialState = false;
const uint32_t WaitLineFixtureDebounceMs = 20; // Debounce period in milliseconds
const unsigned int WaitLineFixtureDtmax = 10; // Maximum time to generate short pulses, to generate short pulses at the beginning of each loop, must be even and less than (pw - debounce)

static_assert (WaitLineFixtureDtmax < (WaitLineFixturePw - WaitLineFixtureDebounceMs), "dtmax must be less than (pw - debounce) and even");
static_assert (WaitLineFixtureDtmax % 2 == 0, "dtmax must be even");

// -----------------------------------------------------------------------------
struct ChipFixture {
  const std::string dev = ChipFixtureDev;

  ChipFixture()  : chip (System::progName()) {
    CHECK (chip.isOpen() == false);
  }

  Chip chip;
};

/**
   Synopsis:
   - Check if the chip is closed initially and getters return default values
   - Open the chip and check if it is open
   - Retrieve chip information and print it
   - Iterate through the lines and print their information
   - Close the chip and check if it is closed
   -----------------------------------------------------------------------------
*/
TEST_FIXTURE (ChipFixture, ChipTest) {

  CHECK (chip.isOpen() == false);
  CHECK (chip.consumer() == System::progName());
  CHECK (chip.errorCode() == 0);
  CHECK (chip.result() == 0);

  CHECK (chip.open (dev));
  std::cout << "Error Code: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  REQUIRE CHECK (chip.isOpen() == true);
  CHECK (chip.errorCode() == 0);

  CHECK (chip.path() == dev);

  REQUIRE CHECK (chip.fillInfo());

  std::cout << "GPIO Chip: " << chip.name() << std::endl;
  std::cout << "Consumer: " << chip.consumer() << std::endl;
  std::cout << "Lines: " << chip.lines() << std::endl;
  std::cout << "Label: " << chip.label() << std::endl;

  for (uint32_t i = 0; i < chip.lines(); ++i) {
    LineInfo info;

    CHECK (chip.lineInfo (i, &info));
    std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  }

  CHECK (chip.close());
  std::cout << "Error Code: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  CHECK (chip.isOpen() == false);
  CHECK (chip.errorCode() == 0);
}

// -----------------------------------------------------------------------------
struct LineFixture : public ChipFixture {

  const std::array<uint32_t, 2> inputOffsets = LineFixtureInputOffsets;
  const uint32_t outputOffset = LineFixtureOutputOffset;
  const uint32_t inputOffset = LineFixtureInputOffset;

  LineFixture() : ChipFixture() {
    REQUIRE CHECK (chip.open (dev));
  }

  ~LineFixture() {
    CHECK (chip.close());
  }
};

/*
  Synopsis:
  - Configure a line and check if this configuration may be read
  -----------------------------------------------------------------------------
*/
TEST_FIXTURE (LineFixture, LineConfigTest) {
  LineInfo info;
  Line line (&chip, outputOffset);
  CHECK (line.isOpen() == false);
  CHECK (line.offset() == outputOffset);

  LineConfig config;
  config.flags = GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED;

  // Open the line with the configuration
  CHECK (line.open (config));
  std::cout << "line.open: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  REQUIRE CHECK (line.isOpen());

  // Check if the configuration is store correctly
  CHECK_EQUAL (line.config().flags, config.flags);
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  /*
      GPIO_V2_LINE_FLAG_ACTIVE_LOW    = _BITULL (1), ///< line active state is physical low
      GPIO_V2_LINE_FLAG_INPUT     = _BITULL (2), ///< line is an input
      GPIO_V2_LINE_FLAG_OUTPUT    = _BITULL (3), ///< line is an output
      GPIO_V2_LINE_FLAG_EDGE_RISING   = _BITULL (4), ///< line detects rising (inactive to active) edges
      GPIO_V2_LINE_FLAG_EDGE_FALLING    = _BITULL (5), ///< line detects falling (active to inactive) edges
      GPIO_V2_LINE_FLAG_OPEN_DRAIN    = _BITULL (6), ///< line is an open drain output
      GPIO_V2_LINE_FLAG_OPEN_SOURCE   = _BITULL (7), ///< line is an open source output
      GPIO_V2_LINE_FLAG_BIAS_PULL_UP    = _BITULL (8), ///< line has pull-up bias enabled
      GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN  = _BITULL (9), ///< line has pull-down bias enabled
      GPIO_V2_LINE_FLAG_BIAS_DISABLED   = _BITULL (10), ///< line has bias disabled

  */
  CHECK (info.offset == outputOffset);
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);
  CHECK_EQUAL (info.num_attrs, 0); // No attributes set

  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_UP | GPIO_V2_LINE_FLAG_ACTIVE_LOW;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);

  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN | GPIO_V2_LINE_FLAG_EDGE_RISING;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);

  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED | GPIO_V2_LINE_FLAG_EDGE_FALLING;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);

  config.flags = GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_OPEN_DRAIN;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);

  config.flags = GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_OPEN_SOURCE;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);

  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN | GPIO_V2_LINE_FLAG_EDGE_RISING;
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
  config.attrs[0].attr.debounce_period_us = 20000; // 20 ms debounce
  config.attrs[0].mask = 1 << 0; // Apply to the first line
  config.num_attrs = 1;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);
  CHECK_EQUAL (info.num_attrs, 1);
  CHECK_EQUAL (info.attrs[0].id, config.attrs[0].attr.id); // Check if the attribute ID matches
  CHECK_EQUAL (info.attrs[0].debounce_period_us, config.attrs[0].attr.debounce_period_us); // 20 ms debounce

  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
  config.num_attrs = 0;
  CHECK (line.setConfig (config));
  CHECK (line.getInfo (&info));
  std::cout << "Line " << info.offset << ": " << info.name << ", attrs: " << info << std::endl;
  CHECK_EQUAL ( (info.flags & ~GPIO_V2_LINE_FLAG_USED), config.flags);
  CHECK_EQUAL (info.num_attrs, 0);

  // Clean up
  CHECK (line.close());
  CHECK (line.isOpen() == false);
}

/*
  Synopsis:
  - Create a Line object with multiple input offsets
  - Open the line with input configuration with pull-up
  - Check if the line is open
  - Retrieve line information and check values (high level)
  - Change the line configuration to pull-down
  - Check if the values are updated correctly (low level)
  -----------------------------------------------------------------------------
*/
TEST_FIXTURE (LineFixture, LineTestInput) {
  const unsigned int numLines = inputOffsets.size();

  // Create a Line object with multiple input offsets
  Line line (&chip, inputOffsets.size(), inputOffsets.data());
  // Chech getters
  CHECK (line.isOpen() == false);
  CHECK (line.numLines() == numLines);
  for (unsigned int i = 0; i < numLines; ++i) {
    CHECK (line.offset (i) == inputOffsets[i]);
  }
  std::cout << "Line offsets: ";
  for (const auto &offset : inputOffsets) {
    std::cout << offset << " ";
  }
  std::cout << std::endl;

  // Configure the line as input with pull-up, then open it
  LineConfig config;
  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_UP;

  CHECK (line.open (config));
  std::cout << "line.open: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  REQUIRE CHECK (line.isOpen());

  // Read line informations
  LineInfo info [numLines];
  CHECK (line.getInfo (info, numLines));
  for (unsigned int i = 0; i < numLines; ++i) {
    std::cout << "Line " << info[i].offset << ": " << info[i].name << ", attrs: " << info[i] <<   std::endl;
    CHECK (line.getValue (i));
  }

  // Change the line configuration to pull-down and check values
  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
  CHECK (line.setConfig (config));
  std::cout << "line.setConfig: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  for (unsigned int i = 0; i < numLines; ++i) {
    CHECK (line.getValue (i) == false);
  }

  // Clean up
  CHECK (line.close());
  CHECK (line.isOpen() == false);
}

/* Synopsis:
  - Configure a GPIO line as output
  - Configure another GPIO line as input, that is connected to the output line
  - Set the output line to high and check if the input line reads high
  - Set the output line to low and check if the input line reads low

  This test assumes that the GPIO lines are connected as described above.
  Make sure to connect the input line to the output line before running this test.
  You can use a jumper wire or any other method to connect the lines physically.
  The test will prompt you to connect the lines before proceeding.
  If the lines are not connected, the test will fail when checking the input value.
  -----------------------------------------------------------------------------
*/
TEST_FIXTURE (LineFixture, LineTestOutput) {

  Line output (&chip, outputOffset);
  CHECK (output.isOpen() == false);
  CHECK (output.offset() == outputOffset);

  Line input (&chip, inputOffset);
  CHECK (input.isOpen() == false);
  CHECK (input.offset() == inputOffset);

  std::cout << "<WARNING> Input line " << input.offset() << " must be connected to output line " << output.offset() << " with a wire!" << std::endl;

  LineConfig config;

  // Configure output line
  config.flags = GPIO_V2_LINE_FLAG_OUTPUT; // Set as output line
  // Set initial output value to high
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
  config.attrs[0].attr.values = 1; // Set output to high
  config.attrs[0].mask = 1 << 0; // Apply to the first line
  config.num_attrs = 1;
  CHECK (output.open (config));

  std::cout << "output.open: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  REQUIRE CHECK (output.isOpen());

  // Configure input line
  config.clear();
  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED;
  CHECK (input.open (config));
  std::cout << "input.open: " << chip.errorCode() << " (" << chip.errorMessage() << ")" << std::endl;
  REQUIRE CHECK (input.isOpen());

  // Check if input reads the output values
  bool state = input.getValue();
  CHECK_EQUAL (true, state); // Check if input reads high

  for (int i = 0; i < 100; ++i) {

    state = !state; // Toggle state
    CHECK (output.setValue (state)); // Set output to high or low
    CHECK_EQUAL (state, output.getValue()); // Check if output reads the same value
    CHECK_EQUAL (state, input.getValue()); // Check if input reads the same value as output
  }

  // Clean up
  CHECK (input.close());
  CHECK (input.isOpen() == false);
  CHECK (output.close());
  CHECK (output.isOpen() == false);
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (LineFixture, LineTestInputActiveLow) {
  LineConfig config (GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_ACTIVE_LOW | GPIO_V2_LINE_FLAG_BIAS_PULL_UP);
  Line line (&chip, inputOffsets[0]);

  CHECK (line.open (config));
  REQUIRE CHECK (line.isOpen());
  CHECK (line.getValue() == false); // Should read low due to pull-up and active low configuration
  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_ACTIVE_LOW | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
  CHECK (line.setConfig (config));
  CHECK (line.getValue() == true); // Should read high due to pull-down and active low configuration
  CHECK (line.close());
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (LineFixture, LineTestOutputActiveLow) {
  LineConfig config (GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_ACTIVE_LOW);
  Line output (&chip, outputOffset);

  CHECK (output.open (config));
  REQUIRE CHECK (output.isOpen());

  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED;
  Line input (&chip, inputOffset);
  CHECK (input.open (config));
  REQUIRE CHECK (input.isOpen());

  CHECK (output.setValue (true)); // Set output to high
  // We can read the output value directly, and it should be at the same state as we wrote
  CHECK (output.getValue() == true); // Should read low due to active low configuration
  CHECK (input.getValue() == false); // Input should read low due to active low configuration

  CHECK (output.setValue (false)); // Set output to low
  // We can read the output value directly, and it should be at the same state as we wrote
  CHECK (output.getValue() == false); // Should read high due to active low configuration
  CHECK (input.getValue() == true); // Input should read high due to active low configuration

  CHECK (output.close());
  CHECK (input.close());
}

// -----------------------------------------------------------------------------
struct WaitLineFixture : public LineFixture {
  const unsigned int pw = WaitLineFixturePw; // Pulse width in milliseconds
  const int loopCount = WaitLineFixtureLoopCount; // Number of loops to test
  const bool initialState = WaitLineFixtureInitialState;
  const uint32_t debounce = WaitLineFixtureDebounceMs; // Debounce period in milliseconds
  const unsigned int dtmax = WaitLineFixtureDtmax; // Maximum time to generate short pulses, to generate short pulses at the beginning of each loop, must be even and less than (pw - debounce)



  std::promise<void> killThread;
  std::thread isrThread;
  TSQueue <LineEvent> queue;

  static void *isrFunc (std::future<void> run, Line &line, TSQueue<LineEvent> &q);

  // -----------------------------------------------------------------------------
  bool attachInterrupt (Line &line) {

    if (!isrThread.joinable()) {

      std::future<void> running = killThread.get_future();
      isrThread = std::thread (isrFunc, std::move (running), std::ref (line), std::ref (queue));
    }
    return isrThread.joinable();
  }

  // -----------------------------------------------------------------------------
  void detachInterrupt () {

    if (isrThread.joinable()) {

      killThread.set_value();
      isrThread.join();
    }
  }
};

// -----------------------------------------------------------------------------
void *WaitLineFixture::isrFunc (std::future<void> run, Line &line, TSQueue<LineEvent> &q) {
  int ret;
  int timeout = 10; // 10 ms

  try {

    while (run.wait_for (std::chrono::milliseconds (1)) == std::future_status::timeout) { // check if the thread is still running
      LineEvent event;

      if (line.waitForEvent (event, timeout)) { // wait for an event, 10 ms timeout
        // a new event is available
        q.push (event);  // push the event to the queue
      }
    }
  }
  catch (std::system_error &e) {

    std::cerr << e.what() << "(code " << e.code() << ")" << std::endl;
    std::terminate();
  }
  catch (...) {

  }

  return 0;
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (WaitLineFixture, LineTestInterrupt) {
  bool outState = !initialState;
  bool inState;
  LineEvent prevEvent;
  int eventCount = 0;
  LineConfig config;

  Line output (&chip, outputOffset);
  Line input (&chip, inputOffset);

  std::cout << "<WARNING> Input line " << input.offset() << " must be connected to output line " << output.offset() << " with a wire!" << std::endl;

  // Configure output line
  config.flags = GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED; // Set as output line
  // Set initial output value to high
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
  config.attrs[0].attr.values = outState; // Set output to high
  config.attrs[0].mask = 1 << 0; // Apply to the first line
  config.num_attrs = 1;
  REQUIRE CHECK (output.open (config));
  REQUIRE CHECK_EQUAL (outState, output.getValue());

  // Configure input line
  config.clear();
  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING | GPIO_V2_LINE_FLAG_BIAS_DISABLED;
  REQUIRE CHECK (input.open (config));

  // Attach an interrupt handler
  CHECK (attachInterrupt (input));

  outState = ! outState; // Toggle outState to set the initial state
  CHECK (output.setValue (outState)); // generate the initial event
  CHECK_EQUAL (outState, output.getValue());

  // Wait for the initial event to be processed
  prevEvent = queue.pop(); // Pop the initial event
  inState = (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
  CHECK_EQUAL (outState, inState); // Check if the input state matches the output state
  CHECK_EQUAL (prevEvent.offset, input.offset()); // Check if the event offset matches the input line offset
  CHECK_EQUAL (prevEvent.line_seqno, 1); // Check if the line sequence number is 1

  for (int i = 0; i <= loopCount; i++) {

    outState = !outState; // Toggle outState
    std::this_thread::sleep_for (std::chrono::milliseconds (pw));
    // Toggle the output value, this should trigger an interrupt
    output.setValue (outState);
  }
  std::this_thread::sleep_for (std::chrono::milliseconds (pw)); // wait for the last event to be processed

  outState = !initialState;
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
  detachInterrupt(); // Detach the interrupt handler

  // Clean up
  CHECK (input.close());
  CHECK (output.close());
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (WaitLineFixture, LineTestDebounce) {
  bool outState = !initialState;
  bool inState;
  LineEvent prevEvent;
  int eventCount = 0;
  LineConfig config;

  Line output (&chip, outputOffset);
  Line input (&chip, inputOffset);

  std::cout << "<WARNING> Input line " << input.offset() << " must be connected to output line " << output.offset() << " with a wire!" << std::endl;

  // Configure output line
  config.flags = GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED; // Set as output line
  // Set initial output value to high
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
  config.attrs[0].attr.values = outState; // Set output to high
  config.attrs[0].mask = 1 << 0; // Apply to the first line
  config.num_attrs = 1;
  REQUIRE CHECK (output.open (config));
  REQUIRE CHECK_EQUAL (outState, output.getValue());

  // Configure input line
  config.clear();
  config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING | GPIO_V2_LINE_FLAG_BIAS_DISABLED;
  REQUIRE CHECK (input.open (config));

  CHECK (input.setDebounce (debounce)); // Set debounce to 20 ms
  CHECK_EQUAL (debounce, input.debounce()); // Check if the debounce value is set correctly

  // Attach an interrupt handler
  CHECK (attachInterrupt (input));

  outState = ! outState; // Toggle outState to set the initial state
  CHECK (output.setValue (outState)); // generate the initial event
  CHECK_EQUAL (outState, output.getValue());

  // Wait for the initial event to be processed
  prevEvent = queue.pop(); // Pop the initial event
  inState = (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE); // state high if the event is a rising edge
  CHECK_EQUAL (outState, inState); // Check if the input state matches the output state
  CHECK_EQUAL (prevEvent.offset, input.offset()); // Check if the event offset matches the input line offset
  CHECK_EQUAL (prevEvent.line_seqno, 1); // Check if the line sequence number is 1

  for (int i = 0; i <= loopCount; i++) {
    bool deb = outState;

    for (unsigned int t = 0; t < dtmax; t++) { // Generate short pulses to test debounce

      deb = !deb; // Toggle the output value
      std::this_thread::sleep_for (std::chrono::milliseconds (1));
      output.setValue (outState);
    }

    outState = !outState; // Toggle outState
    std::this_thread::sleep_for (std::chrono::milliseconds (pw - dtmax));
    // Toggle the output value, this should trigger an interrupt
    output.setValue (outState);
  }
  std::this_thread::sleep_for (std::chrono::milliseconds (pw)); // wait for the last event to be processed

  outState = !initialState;
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
  detachInterrupt(); // Detach the interrupt handler

  // Clean up
  CHECK (input.close());
  CHECK (output.close());
}

// run all tests
int main (int argc, char **argv) {
  return UnitTest::RunAllTests();
}

/* ========================================================================== */
