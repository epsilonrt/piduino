// SocPwm Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <iomanip>
#include <string>
#include <array>

#include <piduino/system.h>
#include <piduino/clock.h>
#include <piduino/tsqueue.h>
#include <piduino/database.h>
#include <piduino/gpio.h>
#include <piduino/socpwm.h>


#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Piduino;

// Configuration settings -----------------------------------
#warning "Check this pin numbers and chip numbers, they must match your hardware setup! then comment this line"
const int Pin1 = 1; // iNo number for PWM output pin, use pido to get the pin number, must be a PWM pin
const int Pin2 = 0; // iNo number for input pin connected to the output pin, use pido to get the pin number

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
  SocPwm *pwm;
  long maxOffset;

  PwmFixture() :
    output (gpio.pin (Pin1)), input (gpio.pin (Pin2)), pwm (nullptr), maxOffset (0) {

    if (db.board().soc().id() == SoC::Bcm2712) {
      maxOffset = 1; // For Bcm2712, max() is 1 more than range()
    }

    REQUIRE CHECK (input.isOpen() == true);
    REQUIRE CHECK (output.isOpen() == true);

    input.setPull (Pin::PullDown);
    input.setMode (Pin::ModeInput);
    output.setMode (Pin::ModePwm); // Set output pin to PWM mode

    REQUIRE CHECK_EQUAL (Pin::ModePwm, output.mode());
    REQUIRE CHECK_EQUAL (Pin::ModeInput, input.mode());

    pwm = new SocPwm (&output); // Create SocPwm instance with the output pin
    REQUIRE CHECK (pwm);
  }

  ~PwmFixture() {

    if (pwm) {
      if (pwm->isOpen()) {

        pwm->setEnable (false); // Disable PWM before closing
        pwm->setFrequency (0); // Reset frequency
        pwm->setRange (0); // Reset range
        pwm->write (0); // Reset output value

        REQUIRE CHECK (pwm->isEnabled() == false);
        pwm->close(); // Close the SocPwm instance
      }
      delete pwm; // Clean up SocPwm instance
      pwm = nullptr;
    }
  }

};

// -----------------------------------------------------------------------------
TEST_FIXTURE (PwmFixture, Test1) {
  begin (1, "SocPwm basic tests");

  REQUIRE CHECK (pwm->hasPin());
  REQUIRE CHECK (pwm->hasEngine());
  REQUIRE CHECK (pwm->pin() == &output);
  std::cout << "PwmEngine: " << pwm->deviceName() << std::endl;
  CHECK_EQUAL (pwm->type(), Converter::DigitalToAnalog);
  CHECK_EQUAL (pwm->bipolar(), false);

  CHECK_EQUAL (false, pwm->isOpen());
  CHECK_EQUAL (true, pwm->open());
  REQUIRE CHECK_EQUAL (true, pwm->isOpen());

  if (pwm->isEnabled()) { // If PWM is enabled, disable it, to ensure a clean state

    std::cout << "PWM is enabled, exiting test to restore a clean state, re-run the test" << std::endl;
    REQUIRE CHECK_EQUAL (false, pwm->isEnabled());
  }

  pwm->setRange (4096); // Set range to 4096
  CHECK_EQUAL (4096, pwm->range());
  pwm->setFrequency (1000); // Set frequency to 1000 Hz
  CHECK_CLOSE (1000, pwm->frequency(), 100); // Allow a tolerance of 25 Hz
  CHECK_EQUAL (12, pwm->resolution()); // Check resolution, should be 12 bits

  CHECK_EQUAL (pwm->range() + maxOffset, pwm->max());  // Check maximum value, should be 4096 + maxOffset
  CHECK_EQUAL (0, pwm->min()); // Check minimum value
  pwm->setResolution (10); // Set precision to 10 bits
  CHECK_EQUAL (10, pwm->resolution()); // Check resolution, should be 10 bits
  CHECK_EQUAL (1024, pwm->range()); // Check maximum value for 10 bits

  CHECK (pwm->write (512)); // Write a value of 512
  CHECK_EQUAL (512, pwm->read()); // Read back the value, should be 512

  pwm->run (); // Enable PWM
  CHECK (pwm->isEnabled()); // Check if PWM is enabled

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

    CHECK_EQUAL (true, pwm->open());
    REQUIRE CHECK (pwm->isOpen() == true);
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

    pwm->setRange (tp.range); // Set range to 4096

    in.frequency = pwm->setFrequency (tp.frequency); // Set frequency to 1000 Hz
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

    CHECK_CLOSE (in.frequency, out.frequency, in.frequency * 10 / 100); // Allow a tolerance of 10%
    CHECK_CLOSE (in.dutyCycle, out.dutyCycle, in.dutyCycle * 10 / 100); // Allow a tolerance of 10%
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

  std::array <const TestPoint, 20> testPoints = {
    TestPoint (500, 2000, 1000),
    TestPoint (500, 2000, 100),
    TestPoint (500, 2000, 200),
    TestPoint (500, 2000, 300),
    TestPoint (500, 2000, 400),
    TestPoint (500, 2000, 500),
    TestPoint (500, 2000, 600),
    TestPoint (500, 2000, 700),
    TestPoint (500, 2000, 800),
    TestPoint (500, 2000, 900),
    TestPoint (500, 2000, 1000),
    TestPoint (500, 2000, 1100),
    TestPoint (500, 2000, 1200),
    TestPoint (500, 2000, 1300),
    TestPoint (500, 2000, 1400),
    TestPoint (500, 2000, 1500),
    TestPoint (500, 2000, 1600),
    TestPoint (500, 2000, 1700),
    TestPoint (500, 2000, 1800),
    TestPoint (500, 2000, 1900)
  };

  for (const auto &tp : testPoints) {
    TestAndCheck (tp);
  }
}

// run all tests
int main (int argc, char **argv) {

  std::cout << std::endl << std::endl
  << "---------------------------------------------------------------------------" << std::endl
  << "<WARNING> Pin iNo#" << Pin1 << " must be connected to Pin iNo#" << Pin2 << " with a wire!" << std::endl
  << "---------------------------------------------------------------------------" << std::endl << std::endl;
  return UnitTest::RunAllTests();
}

/* ========================================================================== */
