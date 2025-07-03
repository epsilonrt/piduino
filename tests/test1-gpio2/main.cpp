// Gpio2 Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <atomic>
#include <map>
#include <memory>

#include <piduino/gpio2.h>
#include <piduino/system.h>
#include <piduino/clock.h>
#include <piduino/tsqueue.h>
#include <piduino/gpio.h>
#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Gpio2;
using namespace Piduino;

// Configuration settings -----------------------------------
#warning "Check this pin numbers and chip numbers, they must match your hardware setup! then comment this line"

// Lines used for read only pins (input), must be on the same chip
const int Pin1 = 2; // iNo number for first input pin, use pido to get the pin number
const int Pin2 = 4; // iNo number for second input pin, use pido to get the pin number

// Line used for write only pin (output)
const int Pin3 = 0; // iNo number for output pin, use pido to get the pin number
const int Pin4 = 1; // iNo number for input pin connected to the output pin, use pido to get the pin number

// WaitLineFixture settings for waitForInterrupt() tests
const unsigned int WaitLineFixturePw = 50; // Pulse width in milliseconds
const int WaitLineFixtureLoopCount = 50; // Number of loops to test
const bool WaitLineFixtureInitialState = false;
const uint32_t WaitLineFixtureDebounceMs = 20; // Debounce period in milliseconds
const unsigned int WaitLineFixtureDtmax = 10; // Maximum time to generate short pulses, to generate short pulses at the beginning of each loop, must be even and less than (pw - debounce)

static_assert (WaitLineFixtureDtmax < (WaitLineFixturePw - WaitLineFixtureDebounceMs), "dtmax must be less than (pw - debounce) and even");
static_assert (WaitLineFixtureDtmax % 2 == 0, "dtmax must be even");

/**
   @struct TestPin
   @brief Structure to hold information about a GPIO pin.
*/
class TestPin {

public:
    /**
       @brief Constructor for TestPin.
       @param inoNumber The GPIO pin number (iNo).
       Initializes the pin and opens the corresponding chip if not already opened.
    */
    TestPin (int inoNumber) : m_ino (inoNumber), pin(gpio.pin (inoNumber)) {
        gpio.setNumbering (Pin::NumberingLogical);
        m_offset = pin.chipOffset();
        m_chip = pin.chipNumber();

        if (chips.find (m_chip) == chips.end()) {

            chips[m_chip] = std::make_shared<Chip> (System::progName());
        }
    }

    void print(std::string name = std::string()) {
        std::cout << name << ": Pin iNo#" << pin.logicalNumber() << " (" << pin.name() << ") initialized." << std::endl
                  << "   Type          : " << pin.typeName() << std::endl
                  << "   Id            : " << pin.id() << std::endl
                  << "   Logical Number: " << pin.logicalNumber() << std::endl
                  << "   Mcu Number    : " << pin.mcuNumber() << std::endl
                  << "   System Number : " << pin.systemNumber() << std::endl
                  << "   Row           : " << pin.row() << std::endl
                  << "   Column        : " << pin.column() << std::endl
                  << "   Chip Number   : " << pin.chipNumber() << std::endl
                  << "   Chip Offset   : " << pin.chipOffset() << std::endl;
    }


    /**
       @brief returns the Chip instance associated with this pin.
       @return Pointer to the Chip instance.
    */
    std::shared_ptr<Chip> chip() {
        return chips[m_chip];
    }

    const std::shared_ptr<Chip> chip() const {
        return chips[m_chip];
    }

    /**
       @brief returns the device path for the GPIO chip.
       @return A string representing the device path for the GPIO chip.
       The path is in the format "/dev/gpiochipX" where X is the chip number.
       This is used to open the GPIO chip device.
       @note The chip number is determined by the pin's chip number.
    */
    std::string dev() const {

        return Chip::devPath (m_chip);
    }

    /**
       @brief Opens the GPIO chip device if it is not already open.
       @return true if the chip was successfully opened, false if it was already open.
       This method checks if the chip is already open and opens it if not.
    */
    bool open() {

        if (!chip()->isOpen()) {

            return chip()->open (dev());
        }
        return isOpen();
    }

    /**
       @brief Checks if the GPIO chip device is open.
       @return true if the chip is open, false otherwise.
       This method checks the state of the chip to determine if it is open.
    */
    bool isOpen() const {
        return chip()->isOpen();
    }

    /**
       @brief return the GPIO pin number (iNo).
       @return The GPIO pin number (iNo).
    */
    int iNo() const {
        return m_ino;
    }

    /**
       @brief returns the GPIO pin offset on the chip.
       The offset is the local pin number on the chip, which may differ from the iNo number.
       This is useful for accessing the pin on the chip directly.
       @return The GPIO pin offset.
    */
    uint32_t offset() const {
        return m_offset;
    }

    /**
       @brief returns the chip number where this pin is located.
       The chip number is the index of the GPIO chip in the system, starting from 0.
       @return The chip number.
    */
    int chipNumber() const {
        return m_chip;
    }

private:
    int m_ino;
    uint32_t m_offset; // GPIO pin offset
    int m_chip; // Chip number
    Pin &pin;

    static std::map<int, std::shared_ptr<Chip>> chips; // Global map to hold chip instances
};

std::map<int, std::shared_ptr<Chip>> TestPin::chips;

// -----------------------------------------------------------------------------
void testTitle(int number, const char title[]) {
    std::cout << std::endl << "--------------------------------------------------------------------------->>>" << std::endl;
    std::cout << "Test" << number << ": " << title << std::endl;
}

// -----------------------------------------------------------------------------
// To comment, when check is verified
// TEST (TestPinConstructor) {
//   TestPin testPin1 (Pin1);
//   TestPin testPin2 (Pin2);
//   TestPin testPin3 (Pin3);
//   TestPin testPin4 (Pin4);

//   REQUIRE CHECK_EQUAL (0, testPin1.chipNumber());
//   REQUIRE CHECK_EQUAL (27, testPin1.offset());
//   REQUIRE CHECK_EQUAL (Pin1, testPin1.iNo());
//   REQUIRE CHECK_EQUAL (false, testPin1.isOpen());
//   REQUIRE CHECK_EQUAL ("/dev/gpiochip0", testPin1.dev());

//   REQUIRE CHECK_EQUAL (0, testPin2.chipNumber());
//   REQUIRE CHECK_EQUAL (23, testPin2.offset());
//   REQUIRE CHECK_EQUAL (0, testPin3.chipNumber());
//   REQUIRE CHECK_EQUAL (17, testPin3.offset());
//   REQUIRE CHECK_EQUAL (0, testPin4.chipNumber());
//   REQUIRE CHECK_EQUAL (18, testPin4.offset());
// }

// -----------------------------------------------------------------------------
struct ChipFixture {
    TestPin testPin1;
    Chip &chip;
    std::string dev;

    ChipFixture() : testPin1 (Pin1), chip (*testPin1.chip()), dev (testPin1.dev()) {

        CHECK (chip.isOpen() == false);
    }
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
TEST_FIXTURE (ChipFixture, Test1) {

    testTitle (1, "Chip tests");
    testPin1.print("testPin1");

    CHECK (chip.isOpen() == false);
    CHECK (chip.consumer() == System::progName());
    CHECK (chip.errorCode() == 0);
    CHECK (chip.result() == 0);

    CHECK (chip.open (dev));
    REQUIRE CHECK (chip.isOpen() == true);
    CHECK (chip.errorCode() == 0);

    CHECK (chip.dev() == dev);

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
    CHECK (chip.isOpen() == false);
    CHECK (chip.errorCode() == 0);
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}

// -----------------------------------------------------------------------------
struct LineIn1Fixture {
    TestPin testPin1;
    Line line;

    LineIn1Fixture() :
        testPin1 (Pin1), line (testPin1.chip(), testPin1.offset()) {

        REQUIRE CHECK (testPin1.open());
        CHECK (line.isOpen() == false);
        CHECK_EQUAL (testPin1.offset(), line.offset());
    }

    ~LineIn1Fixture() {
        // Clean up
        CHECK (line.close());
        CHECK (line.isOpen() == false);
    }
    unsigned int numLines() const {
        return line.numLines();
    }
};

/*
  Synopsis:
  - Configure a line and check if this configuration may be read
  -----------------------------------------------------------------------------
*/
TEST_FIXTURE (LineIn1Fixture, Test2) {
    LineInfo info;
    LineConfig config (GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED);

    testTitle (2, "LineConfig tests");
    testPin1.print("testPin1");

    // Open the output with the configuration
    CHECK (line.open (config));
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
    CHECK_EQUAL (line.offset(), info.offset);
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
    config.attrs[0].mask = 1 << 0; // Apply to the first output
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
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (LineIn1Fixture, Test3) {

    LineConfig config (GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_ACTIVE_LOW | GPIO_V2_LINE_FLAG_BIAS_PULL_UP);

    testTitle (3, "Active low input tests");
    testPin1.print("testPin1");

    CHECK (line.open (config));
    REQUIRE CHECK (line.isOpen());
    CHECK (line.getValue() == false); // Should read low due to pull-up and active low configuration
    config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_ACTIVE_LOW | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
    CHECK (line.setConfig (config));
    CHECK (line.getValue() == true); // Should read high due to pull-down and active low configuration
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}


// -----------------------------------------------------------------------------
struct LineIn12Fixture  {
    TestPin testPin1;
    TestPin testPin2;
    uint32_t offsets[2];
    Line line;

    LineIn12Fixture() :
        testPin1 (Pin1), testPin2 (Pin2), offsets {testPin1.offset(), testPin2.offset() },
        line (testPin1.chip(), 2, offsets) {

        // REQUIRE CHECK_EQUAL (0, testPin1.chipNumber()); // To comment, when check is verified
        // REQUIRE CHECK_EQUAL (27, testPin1.offset()); // To comment, when check is verified
        REQUIRE CHECK_EQUAL (testPin1.chipNumber(), testPin2.chipNumber());
        REQUIRE CHECK (testPin1.open());
        REQUIRE CHECK (testPin2.open());
        CHECK (line.isOpen() == false);
    }

    ~LineIn12Fixture() {
        // Clean up
        CHECK (line.close());
        CHECK (line.isOpen() == false);
    }
};

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
TEST_FIXTURE (LineIn12Fixture, Test4) {
    const unsigned int numLines = line.numLines();

    testTitle (4, "Multiple Line tests");
    testPin1.print("testPin1");
    testPin2.print("testPin2");

    // Chech getters
    CHECK (line.isOpen() == false);
    for (unsigned int i = 0; i < line.numLines(); ++i) {
        CHECK (line.offset (i) == offsets[i]);
    }
    std::cout << "Line offsets: ";
    for (const auto &offset : offsets) {
        std::cout << offset << " ";
    }
    std::cout << std::endl;

    // Configure the line as input with pull-up, then open it
    LineConfig config;
    config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_UP;

    CHECK (line.open (config));
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
    for (unsigned int i = 0; i < numLines; ++i) {
        CHECK (line.getValue (i) == false);
    }
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}

// -----------------------------------------------------------------------------
struct LineInFixture {
    TestPin testPin4;
    Line input;

    LineInFixture() :
        testPin4 (Pin4), input (testPin4.chip(), testPin4.offset()) {

        REQUIRE CHECK (testPin4.open());
        CHECK (input.isOpen() == false);
        CHECK_EQUAL (testPin4.offset(), input.offset());
    }

    ~LineInFixture() {
        // Clean up
        CHECK (input.close());
        CHECK (input.isOpen() == false);
    }

    unsigned int numLines() const {
        return input.numLines();
    }
};

// -----------------------------------------------------------------------------
struct LineOutFixture {
    TestPin testPin3;
    Line output;

    LineOutFixture() :
        testPin3 (Pin3), output (testPin3.chip(), testPin3.offset()) {

        REQUIRE CHECK (testPin3.open());
        CHECK (output.isOpen() == false);
        CHECK_EQUAL (testPin3.offset(), output.offset());
    }

    ~LineOutFixture() {
        // Clean up
        CHECK (output.close());
        CHECK (output.isOpen() == false);
    }

    unsigned int numLines() const {
        return output.numLines();
    }
};

// -----------------------------------------------------------------------------
struct LineInOutFixture : public LineInFixture, public LineOutFixture {

    LineInOutFixture() : LineInFixture(), LineOutFixture() {}
    void inOutTitle (int n, const char title[]) {
        testTitle (n, title);
        testPin3.print("testPin3 (output)");
        testPin4.print("testPin4  (input)");
        std::cout << "<WARNING> Pin iNo#" << testPin3.iNo() << " must be connected to Pin iNo#" << testPin4.iNo() << " with a wire!" << std::endl;
    }
};

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
TEST_FIXTURE (LineInOutFixture, Test5) {
    LineConfig config;

    inOutTitle (5, "Output -> Input tests");

    // Configure output line
    config.flags = GPIO_V2_LINE_FLAG_OUTPUT; // Set as output line
    // Set initial output value to high
    config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    config.attrs[0].attr.values = 1; // Set output to high
    config.attrs[0].mask = 1 << 0; // Apply to the first line
    config.num_attrs = 1;

    CHECK (output.open (config));
    REQUIRE CHECK (output.isOpen());

    // Configure input line
    config.clear();
    config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED;

    CHECK (input.open (config));
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
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}


// -----------------------------------------------------------------------------
TEST_FIXTURE (LineInOutFixture, Test6) {

    LineConfig config (GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_ACTIVE_LOW);
    inOutTitle (6, "Active low output tests");

    CHECK (output.open (config));
    REQUIRE CHECK (output.isOpen());

    config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED;
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
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}

// -----------------------------------------------------------------------------
struct WaitLineFixture : public LineInOutFixture {
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

        bool b = line.getValue();

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
TEST_FIXTURE (WaitLineFixture, Test7) {
    bool outState = initialState;
    bool inState;
    LineEvent prevEvent;
    int eventCount = 0;
    LineConfig config;

    inOutTitle (7, "Interrupt tests");

    // Configure output line
    config.flags = GPIO_V2_LINE_FLAG_OUTPUT | GPIO_V2_LINE_FLAG_BIAS_DISABLED; // Set as output line
    // Set initial output value to high
    config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    config.attrs[0].attr.values = outState;
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
        clk.delay(pw);
        // Toggle the output value, this should trigger an interrupt
        output.setValue (outState);
    }
    clk.delay(pw);

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
    detachInterrupt(); // Detach the interrupt handler
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (WaitLineFixture, Test8) {
    bool outState = initialState;
    bool inState;
    LineEvent prevEvent;
    int eventCount = 0;
    LineConfig config;

    inOutTitle (8, " Debounce Interrupt tests");

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
            clk.delay(1);

            output.setValue (deb);
        }

        outState = !outState; // Toggle outState
        clk.delay(pw- dtmax);

        // Toggle the output value, this should trigger an interrupt
        output.setValue (outState);
    }
    clk.delay(pw);


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
    detachInterrupt(); // Detach the interrupt handler
    std::cout << "---------------------------------------------------------------------------<<<" << std::endl << std::endl;
}

// run all tests
int main (int argc, char **argv) {
    return UnitTest::RunAllTests();
}

/* ========================================================================== */
