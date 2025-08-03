// GpioPwm Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <iomanip>
#include <string>
#include <array>

#include <piduino/system.h>
#include <piduino/clock.h>
#include <piduino/tsqueue.h>
#include <piduino/gpio.h>
#include <piduino/gpiopwm.h>

#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Piduino;

// Configuration settings -----------------------------------
#warning "Check this pin numbers and chip numbers, they must match your hardware setup! then comment this line"

const int Pin1 = 1; // iNo number for PWM output pin, use pido to get the pin number
const int Pin2 = 0; // iNo number for input pin connected to the output pin, use pido to get the pin number

const long FreqMin = 100;
const long FreqMax = 1000; // tick = 10us for range 100

const long FreqTolerance = 10; // Tolerance for frequency checks, in %
const long DutyCycleTolerance = 10; // Tolerance for duty cycle checks, in %

const long Freq1 = 100;
const long Range1 = 1000;

const long Freq3 = 100;
const long Range3 = 200;

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
struct PwmFixture : public GpioFixture {

  Pin &output;
  Pin &input;
  GpioPwm *pwm;

  PwmFixture() :
    output (gpio.pin (Pin1)), input (gpio.pin (Pin2)), pwm (nullptr) {

    REQUIRE CHECK (input.isOpen() == true);
    REQUIRE CHECK (output.isOpen() == true);

    // checks input and output pin before proceeding
    input.setPull (Pin::PullUp);
    input.setMode (Pin::ModeInput);
    REQUIRE CHECK_EQUAL (Pin::ModeInput, input.mode());
    REQUIRE CHECK_EQUAL (Pin::PullUp, input.pull());

    {
      // Check if input is connected to output
      bool inState, outState;
      std::string errorMessage = "<ERROR> Pin iNo#" + std::to_string (output.logicalNumber()) + " must be connected to Pin iNo#" + std::to_string (input.logicalNumber()) + " with a wire!";

      output.setPull (Pin::PullUp);
      output.setMode (Pin::ModeInput); // Set output pin to input mode to release the input

      inState = input.read();
      CHECK_EQUAL (true, inState);

      output.setMode (Pin::ModeOutput); // Set output pin to output mode

      output.write (true);
      outState = output.read();
      inState = input.read();
      CHECK_EQUAL (true, outState);
      CHECK_EQUAL (true, inState);
      M_Assert (inState == outState, errorMessage);

      output.write (false); // Initialize output pin to low
      outState = output.read();
      inState = input.read();
      CHECK_EQUAL (false, inState);
      CHECK_EQUAL (false, inState);
      M_Assert (inState == outState, errorMessage);
    }

    output.setMode (Pin::ModeInput);

  }

  ~PwmFixture() {

    delete pwm;
  }

};

// -----------------------------------------------------------------------------
TEST_FIXTURE (PwmFixture, Test1) {

  begin (1, "GpioPwm basic tests");

  pwm = new GpioPwm (&output, Range1, Freq1);
  std::cout << "PwmEngine: " << pwm->deviceName() << std::endl;
  CHECK_EQUAL (pwm->type(), Converter::DigitalToAnalog);
  CHECK_EQUAL (pwm->bipolar(), false);

  CHECK (pwm->flags() & Converter::hasFrequency);
  CHECK (pwm->flags() & Converter::hasRange);

  CHECK_EQUAL (false, pwm->isEnabled());
  CHECK_EQUAL (pwm->frequency(), Freq1);
  CHECK_EQUAL (pwm->range(), Range1);
  CHECK_EQUAL (false, pwm->isOpen());

  CHECK_EQUAL (true, pwm->open());
  REQUIRE CHECK_EQUAL (true, pwm->isOpen());


  CHECK_EQUAL (pwm->range(), pwm->max());
  CHECK_EQUAL (0, pwm->min());

  CHECK (pwm->write ( (Range1 * 50) / 100));
  CHECK_EQUAL ( (Range1 * 50) / 100, pwm->read());
  std::cout << "PWM value: " << pwm->read() << std::endl;

  pwm->run ();
  CHECK (pwm->isEnabled());
  clk.delay (2000);

  for (int i = pwm->min(); i <= pwm->max(); i += (pwm->max() - pwm->min()) / 10) {

    CHECK (pwm->write (i)); // Write a value to PWM
    CHECK_EQUAL (i, pwm->read()); // Read back the value
    std::cout << "PWM value: " << pwm->read() << std::endl;
    clk.delay (2000); // Wait for 2 seconds
  }

  CHECK (pwm->write (pwm->min()));
  CHECK_EQUAL (pwm->min(), pwm->read());
  CHECK (pwm->write (pwm->max()));
  CHECK_EQUAL (pwm->max(), pwm->read());
  CHECK (pwm->write (pwm->min() - 1)); // Write a value below minimum
  CHECK_EQUAL (pwm->min(), pwm->read()); // Should clamp to minimum
  CHECK (pwm->write (pwm->max() + 1)); // Write a value above maximum
  CHECK_EQUAL (pwm->max(), pwm->read()); // Should clamp to maximum

  pwm->stop(); // Disable PWM
  CHECK (pwm->isEnabled() == false); // Check if PWM is disabled
  CHECK (pwm->write (0)); // Write a value of 0
  CHECK_EQUAL (0, pwm->read()); // Read back the value, should be
  pwm->close(); // Close the PWM device
  REQUIRE CHECK (pwm->isOpen() == false); // Ensure PWM is closed

  end();
}

// -----------------------------------------------------------------------------
struct InterruptFixture : public PwmFixture {
  TSQueue <Pin::Event> queue;
  double lowPulseWidth = 0; // Low pulse width in nanoseconds
  double highPulseWidth = 0; // High pulse width in nanoseconds
  int n = 2; // Test number

  static void isr (Pin::Event event, void *userData);

  struct TestPoint {
    long frequency;
    long range;
    long value;
    TestPoint (long f = 0, long r = 0, long v = 0) : frequency (f), range (r), value (v) {}
  };

  struct TestResult {
    double frequency;
    double period;
    double dutyCycle;
    TestResult (double f = 0, double p = 0, double d = 0) : frequency (f), period (p), dutyCycle (d) {}
  };

  InterruptFixture() {

    input.attachInterrupt (isr, Pin::EdgeBoth, this);
    REQUIRE CHECK (input.isGpioDevEnabled());
  }

  ~InterruptFixture() {
    // Detach the interrupt handler
    input.detachInterrupt();
    CHECK_EQUAL (false, input.enableGpioDev (false));
  }

  void begin (TestPoint tp) {
    PwmFixture::begin (n++, "TestAndCheck test");
    std::cout << " Test values > Frequency: " << tp.frequency << " Hz, Range: " << tp.range << ", Value: " << tp.value << std::endl;
  }

  bool processEvents() {

    if (queue.size() > 1) {
      double lSum = 0;
      double hSum = 0;
      unsigned long lCount = 0;
      unsigned long hCount = 0;

      lowPulseWidth = 0; // Reset low pulse width
      highPulseWidth = 0; // Reset high pulse width

      Pin::Event prevEvent = queue.pop(); // Pop the event from the queue
      while (!queue.empty()) {
        Pin::Event nextEvent = queue.pop();
        if (prevEvent.id == GPIO_V2_LINE_EVENT_FALLING_EDGE && nextEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE) {

          lSum += static_cast<double> (nextEvent.timestamp_ns - prevEvent.timestamp_ns);
          lCount++;
        }
        else if (prevEvent.id == GPIO_V2_LINE_EVENT_RISING_EDGE && nextEvent.id == GPIO_V2_LINE_EVENT_FALLING_EDGE) {
          hSum += static_cast<double> (nextEvent.timestamp_ns - prevEvent.timestamp_ns) ;
          hCount++;
        }
        prevEvent = nextEvent; // Update the event for the next iteration
      }
      if (lCount > 0) {
        lowPulseWidth = lSum / lCount;
      }
      if (hCount > 0) {
        highPulseWidth = hSum / hCount;
      }
      return true;
    }
    return false;
  }

  void TestAndCheck (const TestPoint &tp) {
    TestResult in;
    TestResult out;
    double d;

    begin (tp);

    pwm = new GpioPwm (&output, tp.range, tp.frequency);
    CHECK_EQUAL (true, pwm->open());
    REQUIRE CHECK (pwm->isOpen() == true);
    pwm->setEnable (false); // Disable PWM initially
    REQUIRE CHECK (pwm->isEnabled() == false);
    
    in.frequency = pwm->frequency (); // Set frequency to 1000 Hz
    d = (static_cast<double> (102) / in.frequency) * 1000; // Calculate delay in milliseconds
    in.dutyCycle = static_cast<double> (tp.value) / tp.range * 100; // Calculate duty cycle
    in.period = 1000000.0 / in.frequency; // Calculate period in microseconds
    clk.delay (100);
    std::cout << "  In  < Period: " << in.period << " us, Frequency: " << in.frequency << " Hz, Duty Cycle: " << in.dutyCycle << "%" << std::endl;

    CHECK (pwm->write (tp.value)); // Write a value of 2048
    REQUIRE CHECK_EQUAL (tp.value, pwm->read()); // Read back the value, should be
    REQUIRE CHECK (pwm->isEnabled() == false); // Ensure PWM is disabled

    queue.clear(); // Clear the event queue
    pwm->run(); // Enable PWM
    clk.delay (d); // Wait for the PWM to run for at least 100 periods
    pwm->stop(); // Disable PWM
    clk.delay (d); // Wait for the PWM to stop
    REQUIRE CHECK (processEvents()); // Process the events

    out.period = lowPulseWidth + highPulseWidth; // Total period in nanoseconds
    out.frequency = 1000000000.0 / out.period; // Frequency in Hz
    out.dutyCycle = (highPulseWidth / out.period) * 100; // Duty cycle in percentage
    out.period = out.period / 1000.0; // Convert period to microseconds
    std::cout << "  Out > Period: " << out.period << " us, Frequency: " << out.frequency << " Hz, Duty Cycle: " << out.dutyCycle << "%" << std::endl;

    CHECK_CLOSE (in.frequency, out.frequency, in.frequency * FreqTolerance / 100);
    CHECK_CLOSE (in.dutyCycle, out.dutyCycle, in.dutyCycle * DutyCycleTolerance / 100);

    delete pwm; // Clean up the PWM object
    pwm = nullptr; // Set the pointer to null to avoid dangling pointer
    end();
  }

};

void InterruptFixture::isr (Pin::Event event, void *userData) {
  InterruptFixture *f = static_cast<InterruptFixture *> (userData);

  // a new event is available
  f->queue.push (event); // push the event to the queue
}


// -----------------------------------------------------------------------------
TEST_FIXTURE (InterruptFixture, Test2) {

  std::array <const TestPoint, 18> testPoints = {
    TestPoint (Freq3, Range3, (Range3 * 50) / 100), // Test with 50% duty cycle
    TestPoint (Freq3, Range3, (Range3 * 10) / 100),
    TestPoint (Freq3, Range3, (Range3 * 15) / 100),
    TestPoint (Freq3, Range3, (Range3 * 20) / 100),
    TestPoint (Freq3, Range3, (Range3 * 25) / 100),
    TestPoint (Freq3, Range3, (Range3 * 30) / 100),
    TestPoint (Freq3, Range3, (Range3 * 35) / 100),
    TestPoint (Freq3, Range3, (Range3 * 40) / 100),
    TestPoint (Freq3, Range3, (Range3 * 45) / 100),
    TestPoint (Freq3, Range3, (Range3 * 50) / 100),
    TestPoint (Freq3, Range3, (Range3 * 55) / 100),
    TestPoint (Freq3, Range3, (Range3 * 60) / 100),
    TestPoint (Freq3, Range3, (Range3 * 65) / 100),
    TestPoint (Freq3, Range3, (Range3 * 70) / 100),
    TestPoint (Freq3, Range3, (Range3 * 75) / 100),
    TestPoint (Freq3, Range3, (Range3 * 80) / 100),
    TestPoint (Freq3, Range3, (Range3 * 85) / 100),
    TestPoint (Freq3, Range3, (Range3 * 90) / 100),
  };

  for (const auto &tp : testPoints) {
    TestAndCheck (tp);
  }
}

// run all tests
int main (int argc, char **argv) {

  return UnitTest::RunAllTests();
}

/* ========================================================================== */
