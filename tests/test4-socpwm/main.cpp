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

namespace Broadcom {
  const int Pin1Number = 1; // iNo number for PWM output pin, use pido to get the pin number, must be a PWM pin
  const int Pin2Number = 0; // iNo number for input pin connected to the output pin, use pido to get the pin number
  const long Freq1 = 1000;
  const long Range1 = 4096;
  const int Resolution1 = 12;
  const long Freq2 = 1000;
  const long Range2 = 1024;
  const int Resolution2 = 10;
  const long Freq3 = 500;
  const int Resolution3 = 9;
  const long Range3 = 2000;
}

namespace Allwinner {
  const int Pin1Number = 18; // iNo number for PWM output pin, use pido to get the pin number, must be a PWM pin
  const int Pin2Number = 0; // iNo number for input pin connected to the output pin, use pido to get the pin number
  // The clock divisor list on Allwinner H3/H5 is  1,120,240,360,480,12000,24000,36000,48000 and 72000.
  // As the base frequency is 24MHz, we cannot have the correct frequencies if the range is not chosen correctly.
  const long Freq1 = 1000;
  const long Range1 = 24000; // clock divisor 1 with 24MHz base frequency gives 1000Hz when range is 24000
  const int Resolution1 =  14;
  const long Freq2 = 1465;
  const long Range2 = 16384; // clock divisor 1 with 24MHz base frequency gives 2000Hz when range is 16384
  const int Resolution2 = 14;
  const long Freq3 = 500;
  const long Range3 = 48000; // clock divisor 1 with 24MHz base frequency gives 500Hz when range is 48000
  const int Resolution3 = 16;
}

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
  int Pin1;
  int Pin2;
  Pin *output;
  Pin *input;
  SocPwm *pwm;
  long maxOffset;
  long Freq1;
  long Range1;
  int Resolution1;
  long Freq2;
  long Range2;
  int Resolution2;
  long Freq3;
  long Range3;
  int Resolution3;

  PwmFixture() :
    output (nullptr), input (nullptr), pwm (nullptr), maxOffset (0) {

    switch (db.board().soc().id()) {
      case SoC::Bcm2712:
        maxOffset = 1; // For Bcm2712, max() is 1 more than range()
      case SoC::Bcm2708:
      case SoC::Bcm2709:
      case SoC::Bcm2710:
      case SoC::Bcm2711:
        Pin1 = Broadcom::Pin1Number;
        Pin2 = Broadcom::Pin2Number;
        Freq1 = Broadcom::Freq1;
        Range1 = Broadcom::Range1;
        Resolution1 = Broadcom::Resolution1;
        Freq2 = Broadcom::Freq2;
        Range2 = Broadcom::Range2;
        Resolution2 = Broadcom::Resolution2;
        Freq3 = Broadcom::Freq3;
        Range3 = Broadcom::Range3;
        Resolution3 = Broadcom::Resolution3;
        break;
      case SoC::H3:
      case SoC::H5:
        maxOffset = -1;
        Pin1 = Allwinner::Pin1Number;
        Pin2 = Allwinner::Pin2Number;
        Freq1 = Allwinner::Freq1;
        Range1 = Allwinner::Range1;
        Resolution1 = Allwinner::Resolution1;
        Freq2 = Allwinner::Freq2;
        Range2 = Allwinner::Range2;
        Resolution2 = Allwinner::Resolution2;
        Freq3 = Allwinner::Freq3;
        Range3 = Allwinner::Range3;
        Resolution3 = Allwinner::Resolution3;
        break;
      default:
        std::cerr << "Unsupported SoC for PWM tests." << std::endl;
        return; // Exit if unsupported SoC
    }
    output = &gpio.pin (Pin1);
    input = &gpio.pin (Pin2);


    REQUIRE CHECK (input->isOpen() == true);
    REQUIRE CHECK (output->isOpen() == true);

    input->setPull (Pin::PullUp);
    input->setMode (Pin::ModeInput);
    REQUIRE CHECK_EQUAL (Pin::ModeInput, input->mode());
    REQUIRE CHECK_EQUAL (Pin::PullUp, input->pull());

    {
      // Check if input is connected to output
      bool inState, outState;
      std::string errorMessage = "<ERROR> Pin iNo#" + std::to_string (output->logicalNumber()) + " must be connected to Pin iNo#" + std::to_string (input->logicalNumber()) + " with a wire!";

      output->setPull (Pin::PullUp);
      output->setMode (Pin::ModeInput); // Set output pin to input mode to release the input

      inState = input->read();
      CHECK_EQUAL (true, inState);

      output->setMode (Pin::ModeOutput); // Set output pin to output mode

      output->write (true);
      outState = output->read();
      inState = input->read();
      CHECK_EQUAL (true, outState);
      CHECK_EQUAL (true, inState);
      M_Assert (inState == outState, errorMessage);

      output->write (false); // Initialize output pin to low
      outState = output->read();
      inState = input->read();
      CHECK_EQUAL (false, inState);
      CHECK_EQUAL (false, inState);
      M_Assert (inState == outState, errorMessage);
    }

    output->setMode (Pin::ModePwm); // Set output pin to PWM mode
    REQUIRE CHECK_EQUAL (Pin::ModePwm, output->mode());

    pwm = new SocPwm (output); // Create SocPwm instance with the output pin
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

  REQUIRE CHECK (pwm->pin() == output);
  std::cout << "PwmEngine: " << pwm->deviceName() << std::endl;
  CHECK_EQUAL (pwm->type(), Converter::DigitalToAnalog);
  CHECK_EQUAL (pwm->isBipolar(), false);

  CHECK_EQUAL (false, pwm->isOpen());
  CHECK_EQUAL (true, pwm->open());
  REQUIRE CHECK_EQUAL (true, pwm->isOpen());

  if (pwm->isEnabled()) { // If PWM is enabled, disable it, to ensure a clean state

    std::cout << "PWM is enabled, exiting test to restore a clean state, re-run the test" << std::endl;
    REQUIRE CHECK_EQUAL (false, pwm->isEnabled());
  }

  pwm->setRange (Range1);
  CHECK_EQUAL (Range1, pwm->range());
  pwm->setFrequency (Freq1);
  CHECK_CLOSE (Freq1, pwm->frequency(), 100);
  CHECK_EQUAL (Resolution1, pwm->resolution());

  CHECK_EQUAL (pwm->range() + maxOffset, pwm->max());  // Check maximum value, should be 4096 + maxOffset
  CHECK_EQUAL (0, pwm->min()); // Check minimum value
  pwm->setResolution (Resolution2);
  CHECK_EQUAL (Resolution2, pwm->resolution());
  CHECK_EQUAL (Range2, pwm->range());
  pwm->setFrequency (Freq2);
  CHECK_CLOSE (Freq2, pwm->frequency(), 100);

  CHECK (pwm->write (Range2 / 2)); // Set Duty Cycle to 50%
  CHECK_EQUAL (Range2 / 2, pwm->read());

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
    input->attachInterrupt (isr, Pin::EdgeBoth, this);
    REQUIRE CHECK (input->isGpioDevEnabled());
  }

  ~InterruptFixture() {
    // Detach the interrupt handler
    input->detachInterrupt();
    CHECK_EQUAL (false, input->enableGpioDev (false));
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

    pwm->setRange (tp.range);

    in.frequency = pwm->setFrequency (tp.frequency);
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
    CHECK_CLOSE (in.dutyCycle, out.dutyCycle, in.dutyCycle * 15 / 100); // Allow a tolerance of 10%
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
    TestPoint (Freq3, Range3, (Range3 * 50) / 100), // Test with 50% duty cycle
    TestPoint (Freq3, Range3, (Range3 * 5) / 100),
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
    TestPoint (Freq3, Range3, (Range3 * 95) / 100),
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
