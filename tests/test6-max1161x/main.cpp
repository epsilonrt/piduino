// Max1161x Unit Test
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <limits>

#include <piduino/system.h>
#include <piduino/clock.h>
#include <piduino/max1161x.h>

#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Piduino;

// Configuration settings -----------------------------------
const double FsrInt  = 2.048; // Internal Full Scale Range
const double FsrVdd = 3.3; // Vdd Full Scale Range
const double FsrExt = 2.5; // External Full Scale Range

const double V1 = 2.0; // Voltage 1 -> AIN0
const long D1Int = (V1 * (1 << 12)) / FsrInt; // Digital value for Voltage 1 with Internal reference
const long D1Vdd = (V1 * (1 << 12)) / FsrVdd; // Digital value for Voltage 1 with Vdd reference
const long D1Ext = (V1 * (1 << 12)) / FsrExt; // Digital value for Voltage 1 with External reference

const double V2 = 1.0; // Voltage 2 -> AIN1
const long D2Int = (V2 * (1 << 12)) / FsrInt; // Digital value for Voltage 2 with Internal reference
const long D2Vdd = (V2 * (1 << 12)) / FsrVdd; // Digital value for Voltage 2 with Vdd reference
const long D2Ext = (V2 * (1 << 12)) / FsrExt; // Digital value for Voltage 2 with External reference

// const int bus2Id = -1; // I2C bus ID for testing, use -1 for disabled this test
const int bus2Id = 0;

/**
   @struct Info
   @brief Structure containing information about the Max1161x converter.
*/
struct Info {
  // ------------------ data members ------------------
  Max1161x::MaxIndex id; ///< The Max1161x model ID (12, 13, 14, 15, 16, or 17).
  int addr; ///< The I2C address of the converter.
  int nchan; ///< The number of channels configured for the converter.
  double intref; ///< The internal reference voltage value.
  double fsr; ///< The full-scale range value.
  static const std::map<Max1161x::MaxIndex, Info> maxInfoMap;

  // ------------------ methods ------------------
  /**
     @brief Default constructor for Info.
     @param id The Max1161x model ID (default is Max11615).
     @note This constructor initializes all fields to the values corresponding to the Max11615 model.
  */
  Info (Max1161x::MaxIndex id = Max1161x::Max11615) {

    setId (id); // Initialize the Info structure with the specified MaxIndex
  }

  Info (Max1161x::MaxIndex id, int addr, int nchan, double intref, double fsr)
    : id (id), addr (addr), nchan (nchan), intref (intref), fsr (fsr) {}

  /**
     @brief Sets the Max1161x model ID.
     @param id The Max1161x model ID to set.
     @note This function updates the ID and all other fields to the values corresponding to the specified model.
  */
  void setId (Max1161x::MaxIndex id) {

    auto it = maxInfoMap.find (id);
    if (it == maxInfoMap.end()) {
      std::cerr << "Invalid MaxIndex specified: " << id << ". Using default value of Max11615." << std::endl;
      id = Max1161x::Max11615; // Default to Max11615 if the ID is invalid
    }

    it = maxInfoMap.find (id);
    *this = it->second; // Set the Info structure to the values corresponding to the specified MaxIndex
  }
};

const std::map<Max1161x::MaxIndex, Info> Info::maxInfoMap = {
  { Max1161x::Max11612, { Max1161x::Max11612, 0x34,  4, 4.096, 5.0 } },
  { Max1161x::Max11613, { Max1161x::Max11613, 0x34,  4, 2.048, 3.3 } },
  { Max1161x::Max11614, { Max1161x::Max11614, 0x33,  8, 4.096, 5.0 } },
  { Max1161x::Max11615, { Max1161x::Max11615, 0x33,  8, 2.048, 3.3 } },
  { Max1161x::Max11616, { Max1161x::Max11616, 0x35, 12, 4.096, 5.0 } },
  { Max1161x::Max11617, { Max1161x::Max11617, 0x35, 12, 2.048, 3.3 } }
};

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
struct ConverterFixture: public TestFixture {
  std::unique_ptr<Converter> conv;
  Info info;

  ConverterFixture()  {

  }

  ~ConverterFixture() {

  }
};

// -----------------------------------------------------------------------------
TEST_FIXTURE (TestFixture, Test1) {

  begin (1, "Max1161x string constructor test");
  // bus=id:max={12,13,14,15,16,17}:ref={ext,vdd,int1,int2,int3,int4}:fsr=value:bipolar={1,0}:clk={int,ext}
  {
    // Default parameters
    // Create a Max1161x converter instance with default parameters
    // This will use the default model (Max11615) and default reference (Vdd)
    // The full-scale range will be set to 3.3V, and bipolar mode will be disabled
    Max1161x conv ("max1161x");
    conv.setDebug (true); // Enable debug mode for detailed output
    CHECK_EQUAL (Max1161x::Max11615, conv.info().id); // Check if the default model is Max11615
    CHECK_EQUAL (0x33, conv.info().addr); // Check if the default address is 0x33
    CHECK_EQUAL (8, conv.info().nchan); // Check if the default number of channels is 8
    CHECK_CLOSE (2.048, conv.info().intref, 0.001); // Check if the default internal reference is 2.048V
    CHECK_CLOSE (3.3, conv.info().fsr, 0.001); // Check if the default full-scale range is 3.3V
    CHECK_EQUAL (I2cDev::Info::defaultBus().id(), conv.info().busId); // Check if the default bus ID is the default I2C bus ID

    CHECK_EQUAL (false, conv.isOpen());
    CHECK_EQUAL (false, conv.isReadable());
    CHECK_EQUAL (false, conv.isEnabled());
    CHECK_EQUAL (Converter::AnalogToDigital, conv.type());
    CHECK_EQUAL ("max1161x", conv.deviceName());
    CHECK_EQUAL (Converter::hasReference | Converter::hasResolution | Converter::hasBipolar | Converter::hasRange | Converter::hasSetReference | Converter::hasSetBipolar | Converter::hasClockSelection, conv.flags());
    CHECK_EQUAL (12, conv.resolution());
    CHECK_EQUAL (1 << 12, conv.range());
    CHECK_EQUAL (0, conv.min());
    CHECK_EQUAL (conv.range() - 1, conv.max());
  }

  if (bus2Id >= 0) { // Test with a specific I2C bus ID
    // Create a Max1161x converter instance with a specific I2C bus ID
    // This will use the default model (Max11615) and default reference (Vdd)
    // The full-scale range will be set to 3.3V, and bipolar mode will be disabled
    // The bus ID is set to bus2Id, which should be a valid I2C bus ID
    std::cout << "Testing with bus ID: " << bus2Id << std::endl;
    Max1161x conv ("max1161x:bus=" + std::to_string (bus2Id));
    conv.setDebug (true); // Enable debug mode for detailed output
    CHECK_EQUAL (bus2Id, conv.info().busId); // Check if the bus ID is set correctly
  }

  {
    for (const auto &info : Info::maxInfoMap) {
      std::stringstream ss;
      ss << "max1161x:max=" << static_cast<int> (info.first);
      Max1161x conv (ss.str());
      conv.setDebug (true); // Enable debug mode for detailed output
      CHECK_EQUAL (info.first, conv.info().id); // Check if the MaxIndex is set correctly
      CHECK_EQUAL (info.second.addr, conv.info().addr); // Check if the address is set correctly
      CHECK_EQUAL (info.second.nchan, conv.info().nchan); // Check if the number of channels is set correctly
      CHECK_CLOSE (info.second.intref, conv.info().intref, 0.001); // Check if the internal reference is set correctly
      CHECK_CLOSE (info.second.fsr, conv.info().fsr, 0.001); // Check if the full-scale range is set correctly
      CHECK_EQUAL (I2cDev::Info::defaultBus().id(), conv.info().busId); // Check if the bus ID is the default I2C bus ID
    }
  }

  {
    // Test All Reference Types
    std::vector<std::string> references = { "ext", "vdd", "int1", "int2", "int3", "int4", "int" };
    for (const auto &ref : references) {
      std::stringstream ss;
      ss << "max1161x:ref=" << ref;
      if (ref == "ext") {
        ss << ":fsr=" << FsrExt; // Set full-scale range for external reference
      }

      Max1161x conv (ss.str());
      conv.setDebug (true); // Enable debug mode for detailed output
      if (ref == "ext") {
        CHECK_EQUAL (Max1161x::ExternalReference, conv.reference()); // Check if the reference is set to External
        CHECK_CLOSE (FsrExt, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for External reference
      }
      else if (ref == "vdd") {
        CHECK_EQUAL (Max1161x::VddReference, conv.reference()); // Check if the reference is set to Vdd
        CHECK_CLOSE (FsrVdd, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for Vdd reference
      }
      else if (ref == "int1") {
        CHECK_EQUAL (Max1161x::Internal1Reference, conv.reference()); // Check if the reference is set to Internal1
        CHECK_CLOSE (FsrInt, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for Internal1 reference
      }
      else if (ref == "int2") {
        CHECK_EQUAL (Max1161x::Internal2Reference, conv.reference()); // Check if the reference is set to Internal2
        CHECK_CLOSE (FsrInt, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for Internal2 reference
      }
      else if (ref == "int3") {
        CHECK_EQUAL (Max1161x::Internal3Reference, conv.reference()); // Check if the reference is set to Internal3
        CHECK_CLOSE (FsrInt, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for Internal3 reference
      }
      else if (ref == "int4") {
        CHECK_EQUAL (Max1161x::Internal4Reference, conv.reference()); // Check if the reference is set to Internal4
        CHECK_CLOSE (FsrInt, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for Internal4 reference
      }
      else if (ref == "int") {
        CHECK_EQUAL (Max1161x::InternalReference, conv.reference()); // Check if the reference is set to Internal
        CHECK_CLOSE (FsrInt, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly for Internal reference
      }

    }
  }

  {
    // Test Bipolar Mode On
    Max1161x conv ("max1161x:bipolar=1");
    conv.setDebug (true); // Enable debug mode for detailed output
    CHECK_EQUAL (true, conv.isBipolar()); // Check if bipolar mode is enabled
    CHECK_EQUAL (-2048, conv.min (true)); // Check if the minimum value is set
    CHECK_EQUAL (2047, conv.max (true)); // Check if the maximum value is set
  }

  {
    // Test Bipolar Mode Off
    Max1161x conv ("max1161x:bipolar=0");
    conv.setDebug (true); // Enable debug mode for detailed output
    CHECK_EQUAL (false, conv.isBipolar()); // Check if bipolar mode is disabled
    CHECK_EQUAL (0, conv.min (false)); // Check if the minimum value is set
    CHECK_EQUAL (4095, conv.max (false)); // Check if the maximum value is set
  }

  {
    // Test Clock Selection
    // Test Internal Clock
    Max1161x conv ("max1161x:clk=int");
    conv.setDebug (true); // Enable debug mode for detailed output
    CHECK_EQUAL (Max1161x::InternalClock, conv.clock()); // Check if the clock setting is Internal
  }

  {
    // Test External Clock
    Max1161x conv ("max1161x:clk=ext");
    conv.setDebug (true); // Enable debug mode for detailed output
    CHECK_EQUAL (Max1161x::ExternalClock, conv.clock()); // Check if the clock setting is External
  }

  {
    // Test Full Parameter String
    Max1161x conv ("max1161x:ref=int:fsr=500:bipolar=1:clk=ext");
    conv.setDebug (true); // Enable debug mode for detailed output

    CHECK_EQUAL (Max1161x::InternalReference, conv.reference()); // Check if the reference is set to Internal
    CHECK_CLOSE (500.0, conv.fullScaleRange(), 0.001); // Check if the full-scale range is set correctly
    CHECK_EQUAL (true, conv.isBipolar()); // Check if bipolar mode is enabled
    CHECK_EQUAL (-2048, conv.min (true)); // Check if the minimum value is set for bipolar mode
    CHECK_EQUAL (2047, conv.max (true)); // Check if the maximum value is set for bipolar mode
    CHECK_EQUAL (Max1161x::ExternalClock, conv.clock()); // Check if the clock setting is External
  }
  end();
}

TEST_FIXTURE (ConverterFixture, Test2) {

  begin (2, "Max1161x Converter factory tests");

  std::unique_ptr<Converter> conv (Converter::factory ("max1161x:ref=int:fsr=500:bipolar=1"));

  CHECK (conv != nullptr); // Check if the converter instance is created successfully

  conv->setDebug (true); // Enable debug mode for detailed output

  CHECK_EQUAL (false, conv->isOpen());
  CHECK_EQUAL (false, conv->isReadable());
  CHECK_EQUAL (false, conv->isEnabled());
  CHECK_EQUAL (Converter::AnalogToDigital, conv->type());
  CHECK_EQUAL ("max1161x", conv->deviceName());
  CHECK_EQUAL (Converter::hasReference | Converter::hasResolution | Converter::hasBipolar | Converter::hasRange | Converter::hasSetReference | Converter::hasSetBipolar | Converter::hasClockSelection, conv->flags());
  CHECK_EQUAL (12, conv->resolution());
  CHECK_EQUAL (1 << 12, conv->range());
  CHECK_EQUAL (0, conv->min());
  CHECK_EQUAL (conv->range() - 1, conv->max());
  CHECK_EQUAL (Max1161x::InternalReference, conv->reference()); // Check if the reference is set to Internal
  CHECK_CLOSE (500.0, conv->fullScaleRange(), 0.001); // Check if the full-scale range is set correctly
  CHECK_EQUAL (true, conv->isBipolar()); // Check if bipolar mode is enabled
  CHECK_EQUAL (-2048, conv->min (true)); // Check if the minimum value is set for bipolar mode
  CHECK_EQUAL (2047, conv->max (true)); // Check if the maximum value is set for bipolar mode
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test3) {

  begin (3, "Max1161x setters and getters tests");

  // Create a Max1161x converter instance
  // Max1161x (int busId = I2cDev::Info::defaultBus().id(), MaxIndex maxId = Max11615, int referenceId = DefaultReference, double fsr = 0, bool bipolar = false);

  // Use the default parameters for the Max1161x constructor
  // This will create a Max1161x with default bus ID, Max11615 model, Vcc reference, fsr set to 3.3V, and bipolar mode disabled
  conv = std::make_unique<Max1161x>();

  // Perform basic tests on the converter
  CHECK (conv != nullptr);
  conv->setDebug (true);

  CHECK_EQUAL (false, conv->isOpen());
  CHECK_EQUAL (false, conv->isReadable());
  CHECK_EQUAL (false, conv->isEnabled());

  // Feature Tests
  CHECK_EQUAL (Converter::AnalogToDigital, conv->type());
  CHECK_EQUAL ("max1161x", conv->deviceName());
  CHECK_EQUAL (Converter::hasReference | Converter::hasResolution | Converter::hasBipolar | Converter::hasRange | Converter::hasSetReference | Converter::hasSetBipolar | Converter::hasClockSelection, conv->flags());
  CHECK_EQUAL (info.nchan, conv->numberOfChannels());

  // Resolution and Range Tests
  CHECK_EQUAL (12, conv->resolution());
  CHECK_EQUAL (1 << 12, conv->range());
  CHECK_EQUAL (0, conv->min());
  CHECK_EQUAL (conv->range() - 1, conv->max());

  // Open the converter
  CHECK_EQUAL (true, conv->open());
  CHECK_EQUAL (true, conv->isOpen());
  CHECK_EQUAL (true, conv->isReadable());
  CHECK_EQUAL (true, conv->isEnabled());

  // Bipolar Tests
  CHECK_EQUAL (false, conv->isBipolar());
  CHECK_EQUAL (true, conv->setBipolar (true));
  CHECK_EQUAL (-2048, conv->min (true));
  CHECK_EQUAL (2047, conv->max (true));

  // Reference Tests
  CHECK_EQUAL (info.fsr, conv->fullScaleRange());
  CHECK_EQUAL (FsrVdd, conv->fullScaleRange());
  CHECK_EQUAL (Max1161x::DefaultReference, conv->reference());
  CHECK_CLOSE (D1Vdd, conv->valueToDigital (V1), 1);
  CHECK_CLOSE (D2Vdd, conv->valueToDigital (V2), 1);
  CHECK_CLOSE (V1, conv->digitalToValue (D1Vdd), 0.01);
  CHECK_CLOSE (V2, conv->digitalToValue (D2Vdd), 0.01);

  CHECK_EQUAL (true, conv->setReference (Max1161x::InternalReference));
  CHECK_EQUAL (Max1161x::InternalReference, conv->reference());
  CHECK_EQUAL (info.intref, conv->fullScaleRange());
  CHECK_EQUAL (FsrInt, conv->fullScaleRange());
  CHECK_CLOSE (D1Int, conv->valueToDigital (V1), 1);
  CHECK_CLOSE (D2Int, conv->valueToDigital (V2), 1);
  CHECK_CLOSE (V1, conv->digitalToValue (D1Int), 0.01);
  CHECK_CLOSE (V2, conv->digitalToValue (D2Int), 0.01);

  CHECK_EQUAL (false, conv->setReference (Max1161x::ExternalReference));
  CHECK_EQUAL (true, conv->setReference (Max1161x::ExternalReference, FsrExt));
  CHECK_EQUAL (Max1161x::ExternalReference, conv->reference());
  CHECK_EQUAL (FsrExt, conv->fullScaleRange());
  CHECK_CLOSE (D1Ext, conv->valueToDigital (V1), 1);
  CHECK_CLOSE (D2Ext, conv->valueToDigital (V2), 1);
  CHECK_CLOSE (V1, conv->digitalToValue (D1Ext), 0.01);
  CHECK_CLOSE (V2, conv->digitalToValue (D2Ext), 0.01);

  CHECK_EQUAL (true, conv->setReference (Max1161x::VddReference));
  CHECK_EQUAL (Max1161x::VddReference, conv->reference());
  CHECK_EQUAL (FsrVdd, conv->fullScaleRange());
  CHECK_CLOSE (D1Vdd, conv->valueToDigital (V1), 1);
  CHECK_CLOSE (D2Vdd, conv->valueToDigital (V2), 1);
  CHECK_CLOSE (V1, conv->digitalToValue (D1Vdd), 0.01);
  CHECK_CLOSE (V2, conv->digitalToValue (D2Vdd), 0.01);

  conv->close();
  CHECK_EQUAL (false, conv->isOpen());
  CHECK_EQUAL (false, conv->isReadable());
  CHECK_EQUAL (false, conv->isEnabled());

  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test4) {
  double avg[2]; // Average values for AIN0 and AIN1
  double relErr[2];
  const int numSamples = 50; // Number of samples taken
  const double absErr = 0.05; // Absolute error tolerance for voltage readings

  begin (4, "Max1161x Single Ended Converter tests");

  // Create a Max1161x converter instance
  // Max1161x (int busId = I2cDev::Info::defaultBus().id(), MaxIndex maxId = Max11615, int referenceId = DefaultReference, double fsr = 0, bool bipolar = false);

  // Use the default parameters for the Max1161x constructor
  // This will create a Max1161x with default bus ID, Max11615 model, Vcc reference, fsr set to 3.3V, and bipolar mode disabled
  conv = std::make_unique<Max1161x>();

  // Perform basic tests on the converter
  CHECK (conv != nullptr);
  conv->setDebug (true);

  std::cout << "Check the voltage on AIN0, it should be 2V ± " << absErr << "V" << std::endl;
  std::cout << "Check the voltage on AIN1, it should be 1V ± " << absErr << "V" << std::endl;

  conv->setReference (Max1161x::InternalReference);
  CHECK_EQUAL (Max1161x::InternalReference, conv->reference());
  CHECK_EQUAL (FsrInt, conv->fullScaleRange());


  // Open the converter
  CHECK_EQUAL (true, conv->open());

  // Read samples from AIN0 and AIN1 as single-ended inputs with raw digital values
  std::cout << "Reading samples from AIN0 and AIN1..." << std::endl;
  std::cout << "Expected values: AIN0 = " << D1Int << " (Digital), " << V1 << "V (Analog)" << std::endl;
  std::cout << "Expected values: AIN1 = " << D2Int << " (Digital), " << V2 << "V (Analog)" << std::endl;
  std::cout << "Number of samples: " << numSamples << std::endl;
  std::cout << std::fixed << std::setprecision (3); // Set output precision for voltage readings
  std::cout << "--------------------------------------------------------------------------->>>" << std::endl;

  long digitalErr = conv->valueToDigital (absErr);  // Convert absolute error to digital value
  long dMin[2] = { std::numeric_limits<long>::max(), std::numeric_limits<long>::max() }; // Minimum digital values for AIN0 and AIN1
  long dMax[2] = { std::numeric_limits<long>::min(), std::numeric_limits<long>::min() }; // Maximum digital values for AIN0 and AIN1
  long dSum[2] = { 0, 0 }; // Sum of digital values for AIN0 and AIN1

  for (int i = 0; i < numSamples; i++) {
    // Read AIN0
    long ain0 = conv->readSample (0);
    std::cout << "AIN0: " << ain0 << " (Digital), " << conv->digitalToValue (ain0) << "V (Analog)" << std::endl;
    CHECK_CLOSE (D1Int, ain0, digitalErr); // Check if the reading is close to the expected value for V1
    dMin[0] = std::min (dMin[0], ain0);
    dMax[0] = std::max (dMax[0], ain0);
    dSum[0] += ain0;

    // Read AIN1
    long ain1 = conv->readSample (1);
    std::cout << "AIN1: " << ain1 << " (Digital), " << conv->digitalToValue (ain1) << "V (Analog)" << std::endl;
    CHECK_CLOSE (D2Int, ain1, digitalErr); // Check if the reading is close to the expected value for V2
    dMin[1] = std::min (dMin[1], ain1);
    dMax[1] = std::max (dMax[1], ain1);
    dSum[1] += ain1;

    Clock::delay (100); // Delay between readings
  }
  avg[0] = dSum[0] / static_cast<double> (numSamples);
  avg[1] = dSum[1] / static_cast<double> (numSamples);
  relErr[0] = (avg[0] - D1Int) / D1Int * 100.0; // Relative error for AIN0
  relErr[1] = (avg[1] - D2Int) / D2Int * 100.0; // Relative error for AIN1

  std::cout << std::endl;
  std::cout << "AIN0: Expected: " << D1Int << "> Min = " << dMin[0] << ", Max = " << dMax[0] << ", Avg = " << (avg[0]) << ", Rel. Error = " << relErr[0] << "%" << std::endl;
  std::cout << "AIN1: Expected: " << D2Int << "> Min = " << dMin[1] << ", Max = " << dMax[1] << ", Avg = " << (avg[1]) << ", Rel. Error = " << relErr[1] << "%" << std::endl;

  double aMin[2] = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() }; // Minimum analog values for AIN0 and AIN1
  double aMax[2] = { std::numeric_limits<double>::min(), std::numeric_limits<double>::min() }; // Maximum analog values for AIN0 and AIN1
  double aSum[2] = { 0.0, 0.0 }; // Sum of analog values for AIN0 and AIN1

  for (int i = 0; i < numSamples; i++) {
    // Read AIN0
    double a0 = conv->readValue (0);
    std::cout << "AIN0: " << a0 << "V (Analog)" << std::endl;
    CHECK_CLOSE (V1, a0,  absErr); // Check if the reading is close to the expected value for V1
    aMin[0] = std::min (aMin[0], a0);
    aMax[0] = std::max (aMax[0], a0);
    aSum[0] += a0;

    // Read AIN1
    double a1 = conv->readValue (1);
    std::cout << "AIN1: " << a1 << "V (Analog)" << std::endl;
    CHECK_CLOSE (V2, a1,  absErr); // Check if the reading is close to the expected value for V2
    aMin[1] = std::min (aMin[1], a1);
    aMax[1] = std::max (aMax[1], a1);
    aSum[1] += a1;

    Clock::delay (100); // Delay between readings
  }
  avg[0] = aSum[0] / static_cast<double> (numSamples);
  avg[1] = aSum[1] / static_cast<double> (numSamples);
  relErr[0] = (avg[0] - V1) / V1 * 100.0; // Relative error for AIN0
  relErr[1] = (avg[1] - V2) / V2 * 100.0; // Relative error for AIN1
  std::cout << std::endl;
  std::cout << "AIN0: Expected: " << V1 << "> Min = " << aMin[0] << ", Max = " << aMax[0] << ", Avg = " << avg[0] << ", Rel. Error = " << relErr[0] << "%" << std::endl;
  std::cout << "AIN1: Expected: " << V2 << "> Min = " << aMin[1] << ", Max = " << aMax[1] << ", Avg = " << avg[1] << ", Rel. Error = " << relErr[1] << "%" << std::endl;
  end();
}

// -----------------------------------------------------------------------------
TEST_FIXTURE (ConverterFixture, Test5) {
  double avg[2]; // Average values for AIN0 and AIN1
  double relErr[2];
  const int numSamples = 50; // Number of samples taken
  const double absErr = 0.05; // Absolute error tolerance for voltage readings

  begin (5, "Max1161x Differential Bipolar Converter tests");

  // Use the default parameters for the Max1161x constructor
  // This will create a Max1161x with default bus ID, Max11615 model, Vcc reference, fsr set to 3.3V, and bipolar mode disabled
  conv = std::make_unique<Max1161x>();

  // Perform basic tests on the converter
  CHECK (conv != nullptr);
  conv->setDebug (true);

  std::cout << "Check the voltage on AIN0, it should be 2V ± " << absErr << "V" << std::endl;
  std::cout << "Check the voltage on AIN1, it should be 1V ± " << absErr << "V" << std::endl;

  conv->setReference (Max1161x::InternalReference);
  CHECK_EQUAL (Max1161x::InternalReference, conv->reference());
  CHECK_EQUAL (FsrInt, conv->fullScaleRange());
  conv->setBipolar (true); // Enable bipolar mode for differential readings
  CHECK_EQUAL (true, conv->isBipolar());

  // Open the converter
  CHECK_EQUAL (true, conv->open());

  // Read samples from AIN0 and AIN1 as differential inputs with raw digital values
  std::cout << "Reading samples from AIN0 - AIN1 (Differential)..." << std::endl;
  std::cout << "Expected values: AIN0 - AIN1 = " << (D1Int - D2Int) << " (Digital), " << (V1 - V2) << "V (Analog)" << std::endl;
  std::cout << "Number of samples: " << numSamples << std::endl;
  std::cout << std::fixed << std::setprecision (3); // Set output precision for voltage readings
  std::cout << "--------------------------------------------------------------------------->>>" << std::endl;

  double aMin =  std::numeric_limits<double>::max();
  double aMax =  std::numeric_limits<double>::min();
  double aSum = 0.0;

  for (int i = 0; i < numSamples; i++) {
    // Read AIN0 - AIN1 as differential inputs
    double ain0 = conv->readValue (0, true); // Read differential input
    CHECK_CLOSE (V1 - V2, ain0, absErr); // Check if the reading is close to the expected value for V1 - V2
    std::cout << "AIN0 - AIN1: " << ain0 << " (Analog)" << std::endl;

    aMin = std::min (aMin, ain0);
    aMax = std::max (aMax, ain0);
    aSum += ain0;

    Clock::delay (100); // Delay between readings
  }
  avg[0] = aSum / static_cast<double> (numSamples);
  relErr[0] = (avg[0] - (V1 - V2)) / (V1 - V2) * 100.0; // Relative error for differential
  std::cout << std::endl;
  std::cout << "AIN0 - AIN1: Expected: " << (V1 - V2) << "> Min = " << aMin << ", Max = " << aMax << ", Avg = " << avg[0] << ", Rel. Error = " << relErr[0] << "%" << std::endl;

  // ---------------------------------------------------------------
  // Read samples from AIN1 and AIN0 as differential inputs with raw digital values
  std::cout << "Reading samples from AIN1 - AIN0 (Differential)..." << std::endl;
  std::cout << "Expected values: AIN1 - AIN0 = " << (D2Int - D1Int) << " (Digital), " << (V2 - V1) << "V (Analog)" << std::endl;
  std::cout << "Number of samples: " << numSamples << std::endl;
  std::cout << std::fixed << std::setprecision (3); // Set output precision for voltage readings
  std::cout << "--------------------------------------------------------------------------->>>" << std::endl;

  aMin =  std::numeric_limits<double>::max();
  aMax =  std::numeric_limits<double>::min();
  aSum = 0.0;

  for (int i = 0; i < numSamples; i++) {
    // Read AIN1 - AIN0 as differential inputs
    double ain1 = conv->readValue (1, true); // Read differential input
    CHECK_CLOSE (V2 - V1, ain1, absErr); // Check if the reading is close to the expected value for V2 - V1
    std::cout << "AIN1 - AIN0: " << ain1 << " (Analog)" << std::endl;

    aMin = std::min (aMin, ain1);
    aMax = std::max (aMax, ain1);
    aSum += ain1;

    Clock::delay (100); // Delay between readings
  }
  avg[0] = aSum / static_cast<double> (numSamples);
  relErr[0] = (avg[0] - (V2 - V1)) / (V2 - V1) * 100.0; // Relative error for differential
  std::cout << std::endl;
  std::cout << "AIN1 - AIN0: Expected: " << (V2 - V1) << "> Min = " << aMin << ", Max = " << aMax << ", Avg = " << avg[0] << ", Rel. Error = " << relErr[0] << "%" << std::endl;

  end();
}

// run all tests
int main (int argc, char **argv) {

  return UnitTest::RunAllTests();
}

/* ========================================================================== */
