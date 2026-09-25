// Max7311 Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <piduino/max7311.h>
#include <piduino/i2cdev.h>

#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Piduino;

// Configuration settings -----------------------------------
// The MAX7311 must answer at this address (AD0, AD1 and AD2 to ground) on the default I2C bus
const int Address = 0x20;

// Only this channel is driven as an output by the tests, all the others are left as they
// are found (they are only read, or written back with the mode that was read).
// Nothing must be connected to it that a push-pull output could damage, a LED with its
// resistor is fine.
const int TestChannel = 0;

const int NumberOfChannels = 16;

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
// Opens the converter and restores the mode of all the channels at the end
struct ConverterFixture: public TestFixture {
  std::unique_ptr<Max7311> conv;
  std::vector<Converter::Mode> saved;

  ConverterFixture() : conv (new Max7311 (I2cDev::Info::defaultBus().id(), Address)) {

    REQUIRE CHECK (conv->open());
    conv->setEnable (true);
    for (int ch = 0; ch < NumberOfChannels; ch++) {

      saved.push_back (conv->mode (ch));
    }
  }

  ~ConverterFixture() {

    if (conv->isOpen()) {

      conv->writeChannel (true, TestChannel); // the output latch is high at power-up
      for (int ch = 0; ch < NumberOfChannels; ch++) {

        conv->setMode (saved[ch], ch);
      }
      conv->close();
    }
  }

  // true if the mode of the channel is exactly the given flags
  bool modeIs (int channel, long flags) {
    Converter::Mode m = conv->mode (channel);

    return m == Converter::Mode (flags);
  }
};

// -----------------------------------------------------------------------------
TEST_FIXTURE (TestFixture, Test1) {

  begin (1, "Max7311 string constructor test");
  {
    // The address is usually given in hexadecimal, and a decimal or a capital X must work too
    const std::vector<std::string> params = {
      "max7311",
      "max7311:addr=0x20",
      "max7311:addr=0X20",
      "max7311:addr=32",
      "max7311:bus=" + std::to_string (I2cDev::Info::defaultBus().id()) + ":addr=0x20",
      "max7311:bustimeout=false"
    };

    for (const auto &str : params) {

      std::cout << "Parameters: " << str << std::endl;
      Max7311 conv (str);

      CHECK_EQUAL ("max7311", conv.deviceName());
      CHECK_EQUAL (false, conv.isOpen());
      CHECK (conv.open());
      if (conv.isOpen()) {

        Converter::Mode m = conv.mode (TestChannel);
        CHECK (m & (Converter::DigitalInput | Converter::DigitalOutput)); // the device answered at the address
        conv.close();
      }
    }
  }
  {
    // Factory, as used by pido
    std::unique_ptr<Converter> conv (Converter::factory ("max7311:addr=0x20"));

    REQUIRE CHECK (conv != nullptr);
    CHECK_EQUAL ("max7311", conv->deviceName());
    CHECK (conv->open());
    conv->close();
  }
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test2) {

  begin (2, "Max7311 the mode that was read can be written back");

  // An input is read as DigitalInput | PullUp (the inputs are always pulled up),
  // writing this mode back must be accepted and must not change anything.
  // All the channels are set as inputs first (this is harmless, the fixture restores
  // the modes that were found).
  CHECK (conv->setMode (Converter::DigitalInput)); // all the channels
  for (int ch = 0; ch < NumberOfChannels; ch++) {

    Converter::Mode m = conv->mode (ch);

    CHECK (modeIs (ch, Converter::DigitalInput | Converter::PullUp));
    CHECK (conv->setMode (m, ch));
    CHECK (modeIs (ch, m.value()));
  }

  // the same for an output
  CHECK (conv->setMode (Converter::DigitalOutput, TestChannel));
  {
    Converter::Mode m = conv->mode (TestChannel);

    CHECK (modeIs (TestChannel, Converter::DigitalOutput));
    CHECK (conv->setMode (m, TestChannel));
    CHECK (modeIs (TestChannel, m.value()));
  }
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test3) {

  begin (3, "Max7311 output mode, write, read and toggle");

  CHECK (conv->setMode (Converter::DigitalOutput, TestChannel));
  CHECK (modeIs (TestChannel, Converter::DigitalOutput));

  CHECK (conv->writeChannel (false, TestChannel));
  CHECK_EQUAL (0, conv->readChannel (TestChannel));
  CHECK (conv->writeChannel (true, TestChannel));
  CHECK_EQUAL (1, conv->readChannel (TestChannel));

  CHECK (conv->toggle (TestChannel));
  CHECK_EQUAL (0, conv->readChannel (TestChannel));
  CHECK (conv->toggle (TestChannel));
  CHECK_EQUAL (1, conv->readChannel (TestChannel));
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test4) {

  begin (4, "Max7311 input mode, pull-up and polarity");

  CHECK (conv->setMode (Converter::DigitalOutput, TestChannel));
  CHECK (conv->writeChannel (true, TestChannel));

  // changing the direction of an input that is pulled up (this is what pido does)
  CHECK (conv->setMode (Converter::DigitalInput, TestChannel));
  CHECK (modeIs (TestChannel, Converter::DigitalInput | Converter::PullUp));
  CHECK (conv->setMode (Converter::DigitalInput | Converter::PullUp, TestChannel));
  CHECK (modeIs (TestChannel, Converter::DigitalInput | Converter::PullUp));

  // polarity
  long normal = conv->readChannel (TestChannel);

  CHECK (conv->setMode (Converter::DigitalInput | Converter::ActiveLow, TestChannel));
  CHECK (modeIs (TestChannel, Converter::DigitalInput | Converter::PullUp | Converter::ActiveLow));
  CHECK_EQUAL (normal ? 0 : 1, conv->readChannel (TestChannel));

  // the polarity is cleared when it is not given again
  CHECK (conv->setMode (Converter::DigitalInput, TestChannel));
  CHECK (modeIs (TestChannel, Converter::DigitalInput | Converter::PullUp));
  CHECK_EQUAL (normal, conv->readChannel (TestChannel));

  // from an input to an output
  CHECK (conv->setMode (Converter::DigitalOutput, TestChannel));
  CHECK (modeIs (TestChannel, Converter::DigitalOutput));
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test5) {

  begin (5, "Max7311 illegal modes are refused");

  CHECK (conv->setMode (Converter::DigitalInput, TestChannel));
  long before = conv->mode (TestChannel).value();

  CHECK (!conv->setMode (Converter::DigitalInput | Converter::DigitalOutput, TestChannel));
  CHECK (!conv->setMode (Converter::PullDown, TestChannel));
  CHECK (!conv->setMode (Converter::DigitalInput | Converter::PullDown, TestChannel));
  CHECK (!conv->setMode (Converter::DigitalOutput | Converter::PullUp, TestChannel));
  CHECK (!conv->setMode (Converter::DigitalInput, NumberOfChannels)); // no such channel
  CHECK (modeIs (TestChannel, before)); // nothing has changed
  end();
}

// run all tests
int main (int argc, char **argv) {

  return UnitTest::RunAllTests();
}

/* ========================================================================== */
