/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.

   Piduino pido tool is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Piduino pido tool is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Piduino pido tool.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <piduino/clock.h>
#include <piduino/gpio.h>
#include <piduino/socpwm.h>
#include <piduino/database.h>
#include "exception.h"
#include "version.h"

using namespace std;
using namespace Piduino;

/* constants ================================================================ */
const string authors = "Pascal JEAN";
const string website = "https://github.com/epsilonrt/piduino";
const string pidoVersion = VERSION_SHORT;
const int pwmDefaultFreq = 1000;
const long pwmDefautRange = 1024;

/* types ==================================================================== */
typedef void (*func) (int argc, char *argv[]);

/* private variables ======================================================== */
Pin::Numbering numbering = Pin::NumberingLogical;
int pinnumber = -1;
int connector = -1;
bool physicalNumbering = false;
Pin *pin = nullptr;
bool debug = false;
bool forceGpioDev = false;
int useGpioDevBeforeWfi = -1;

/* private functions ======================================================== */
void mode (int argc, char *argv[]);
void pull (int argc, char *argv[]);
void read (int argc, char *argv[]);
void write (int argc, char *argv[]);
void toggle (int argc, char *argv[]);
void blink (int argc, char *argv[]);
void readall (int argc, char *argv[]);
void wfi (int argc, char *argv[]);
void pwm (int argc, char *argv[]);
void pwmr (int argc, char *argv[]);
void pwmf (int argc, char *argv[]);
void drive (int argc, char *argv[]);

Pin *getPin (char *c_str);
void usage ();
void version ();
void warranty ();
void sig_handler (int sig);
vector<string> split (const string &s, char seperator);
void debug_pwm (SocPwm &p);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int opt;
  int ret = 0;
  func do_it;

  const map<string, func> str2func = {
    {"mode", mode},
    {"pull", pull},
    {"read", read},
    {"write", write},
    {"toggle", toggle},
    {"blink", blink},
    {"wfi", wfi},
    {"readall", readall},
    {"drive", drive},
    {"pwm", pwm}, // TODO
    {"pwmr", pwmr}, // TODO
    {"pwmf", pwmf} // TODO
  };

  try {
    /* Traitement options ligne de commande */
    while ( (opt = getopt (argc, argv, "gs1dhfvw")) != -1) {

      switch (opt) {

        case 'g':
          numbering = Pin::NumberingMcu;
          break;

        case 's':
          numbering = Pin::NumberingSystem;
          break;

        case '1':
          physicalNumbering = true;
          break;

        case 'd':
          debug = true;
          break;

        case 'f':
          forceGpioDev = true;
          break;

        case 'h':
          usage();
          exit (EXIT_SUCCESS);
          break;

        case 'v':
          version();
          exit (EXIT_SUCCESS);
          break;

        case 'w':
          warranty();
          exit (EXIT_SUCCESS);
          break;

        default:
          /* An invalid option has been used, exit with code EXIT_FAILURE */
          exit (EXIT_FAILURE);
          break;
      }
    }

    if (optind >= argc)    {

      throw Exception (Exception::CommandExpected);
    }

    try {

      do_it = str2func. at (argv[optind]);
    }
    catch (out_of_range &e) {

      throw Exception (Exception::UnknownCommand, argv[optind]);
    }
    optind++;

    gpio.setNumbering (numbering);
    gpio.setDebug (debug);
    gpio.open();

    /* Execute command */
    do_it (argc, argv);
  }
  catch (Exception &e) {

    cerr << System::progName() << ": " << e.what() << " !" << endl;
    cerr << System::progName() << " -h or man " << System::progName() << " for help." << endl;
    ret = -1;
  }
  catch (out_of_range &e) {

    cerr << System::progName() << ": out of range value (" << e.what() << ")" << endl;
    ret = -1;
  }
  catch (invalid_argument &e) {

    cerr << System::progName() << ": invalid argument (" << e.what() << ") !" << endl;
    ret = -1;
  }
  catch (domain_error &e) {

    cerr << System::progName() << ": bad pin type (" << e.what() << ") !" << endl;
    ret = -1;
  }
  catch (exception &e) {

    cerr << System::progName() << ": " << e.what() << " !" << endl;
    ret = -1;
  }
  catch (...) {

    cerr << System::progName() << "Unknown Exception !!!" << endl;
    ret = -1;
  }

  return ret;
}

/* -----------------------------------------------------------------------------
  readall [#connector]
    Output a table of all GPIO pins values.
    The values represent the actual values read if the pin is in input mode,
    or the last value written if the pin is in output mode.
*/
void
readall (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc >= 1) {

    int connector = stoi (string (argv[optind]));
    cout << gpio.connector (connector);
  }
  else {
    for (auto p = gpio.connector().cbegin(); p != gpio.connector().cend(); ++p) {
      // p est une pair: first = numéro et second = connecteur
      cout << p->second << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  mode <pin> <in/out/pwm/off/alt{0..9}>
    Get/Set a pin mode into input, output, off, alt0..9 or pwm mode.
*/
void
mode (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {

    pin = getPin (argv[optind]);

    if (paramc > 1) {
      Pin::Mode m;
      string smode (argv[optind + 1]);
      const map<string, Pin::Mode> str2mode = {
        {"in",    Pin::ModeInput   },
        {"out",   Pin::ModeOutput  },
        {"alt0",  Pin::ModeAlt0    },
        {"alt1",  Pin::ModeAlt1    },
        {"alt2",  Pin::ModeAlt2    },
        {"alt3",  Pin::ModeAlt3    },
        {"alt4",  Pin::ModeAlt4    },
        {"alt5",  Pin::ModeAlt5    },
        {"alt6",  Pin::ModeAlt6    },
        {"alt7",  Pin::ModeAlt7    },
        {"alt8",  Pin::ModeAlt8    },
        {"alt9",  Pin::ModeAlt9    },
        {"off",   Pin::ModeDisabled},
        {"pwm",   Pin::ModePwm     }
      };

      m = str2mode.at (smode);

      // Modification à garder après fermeture !
      gpio.setReleaseOnClose (false);
      pin->setMode (m);
    }
    else {
      // Lecture du mode
      cout << pin->modeName() << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  pull <pin> <up/down/off>
    Get/Set the internal pull-up, pull-down or off controls.
*/
void
pull (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    // forceGpioDev = false;
    pin = getPin (argv[optind]);

    if (paramc > 1) {
      Pin::Pull p;
      string pmode (argv[optind + 1]);
      const map<string, Pin::Pull> str2pull = {
        {"off",   Pin::PullOff},
        {"up",    Pin::PullUp},
        {"down",  Pin::PullDown}
      };

      p = str2pull.at (pmode);

      // Modification à garder après fermeture !
      gpio.setReleaseOnClose (false);
      pin->setPull (p);
    }
    else {
      // Lecture de la résistance
      cout << pin->pullName() << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  drive <pin> <level>
    Get/Set the multi-driving output level
*/
void
drive (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    // forceGpioDev = false;
    pin = getPin (argv[optind]);

    if (paramc > 1) {
      int level;

      level = stoi (string (argv[optind + 1]));

      // Modification à garder après fermeture !
      gpio.setReleaseOnClose (false);
      pin->setDrive (level);
    }
    else {
      // Lecture
      cout << pin->drive() << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  read <pin>
    Read  the  digital value of the given pin (0 or 1)
*/
void
read (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {

    pin = getPin (argv[optind]);
    cout << pin->read () << endl;
  }
}

/* -----------------------------------------------------------------------------
  write <pin> <value>
    Write the given value (0 or 1) to the given pin (output).
*/
void
write (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = getPin (argv[optind]);

    value = stoi (string (argv[optind + 1]));
    if ( (value < 0) || (value > 1)) {

      throw Exception (Exception::NotBinaryValue, value);
    }

    if (pin->mode () != Pin::ModeOutput) {

      throw Exception (Exception::NotOutputPin, pinnumber);
    }
    // Modification à garder après fermeture !
    gpio.setReleaseOnClose (false);
    pin->write (value);
  }
}

/* -----------------------------------------------------------------------------
  toggle <pin>
    Changes the state of a GPIO pin; 0 to 1, or 1 to 0 (output).
*/
void
toggle (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {

    pin = getPin (argv[optind]);
    if (pin->mode () != Pin::ModeOutput) {

      throw Exception (Exception::NotOutputPin, pinnumber);
    }
    // Modification à garder après fermeture !
    gpio.setReleaseOnClose (false);
    pin->toggle ();
  }
}

/* -----------------------------------------------------------------------------
  blink <pin> [period]
    Blinks the given pin on/off (explicitly sets the pin to output)
*/
void
blink (int argc, char *argv[]) {
  int paramc = (argc - optind);
  Clock clk;

  if (paramc < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {
    int period = 1000;

    gpio.setReleaseOnClose (true);

    pin = getPin (argv[optind]);
    if (paramc > 1)    {

      period = stoi (string (argv[optind + 1]));
      if (period < 1) {
        period = 1;
        cout << "Warning: Pin " << pin->name() << " the period has been set to " << period << " ms (min.) !" << endl;
      }
    }

    pin->setMode (Pin::ModeOutput);
    period /= 2;

    // sig_handler() intercepte le CTRL+C
    signal (SIGINT, sig_handler);
    signal (SIGTERM, sig_handler);
    cout << "Press Ctrl+C to abort ..." << endl;

    for (;;) {

      pin->toggle ();
      clk.delay (period);
    }
  }
}

/* -----------------------------------------------------------------------------
  wfi <pin> <rising/falling/both> [timeout_ms]
    This set the given pin to the supplied interrupt mode:  rising,  falling  or
    both  then waits  for  the  interrupt  to happen.
    It's a non-busy wait, so does not consume and CPU while it's waiting.
*/
void
wfi (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    const map<string, Pin::Edge>  str2edge = {
      { "none", Pin::EdgeNone },
      { "rising", Pin::EdgeRising },
      { "falling", Pin::EdgeFalling },
      { "both", Pin::EdgeBoth }
    };
    long timeout = -1;
    Pin::Edge e;
    string edge (argv[optind + 1]);

    // forceGpioDev = false;
    pin = getPin (argv[optind]);
    e = str2edge.at (edge);
    if (paramc > 2) {

      timeout = stol (string (argv[optind + 2]));
    }

    // sig_handler() intercepte le CTRL+C
    signal (SIGINT, sig_handler);
    signal (SIGTERM, sig_handler);

    pin->waitForInterrupt (e, timeout);
    cout << pin->read() << endl;
  }
}

/* -----------------------------------------------------------------------------
  pwm <pin> [value]
*/
void
pwm (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1) {

    throw Exception (Exception::ArgumentExpected);
  }

  pin = getPin (argv[optind]);
  if (pin->mode() != Pin::ModePwm) {

    throw Exception (Exception::NotPwmPin, pin->logicalNumber());
  }

  SocPwm socpwm (pin);
  socpwm.setDebug (debug);

  if (!socpwm.open()) {

    throw Exception (Exception::PwmOpenError, pin->logicalNumber());
  }

  if (socpwm.frequency() == 0) {

    socpwm.setRange (pwmDefautRange);
    socpwm.setFrequency (pwmDefaultFreq);
    #ifndef NDEBUG
    cout << "Sets frequency  to " << socpwm.frequency() << endl;
    cout << "Sets range to " << socpwm.range() << endl;
    #endif
  }

  if (paramc >= 2) {

    long value = stol (string (argv[optind + 1]));

    if ( (value < socpwm.min()) || (value > socpwm.max())) {

      throw Exception (Exception::NotPwmValue, value);
    }

    if (!socpwm.write (value)) {

      throw Exception (Exception::PwmWriteError, pin->logicalNumber());
    }
    socpwm.run();
  }
  else {

    cout << socpwm.read() << endl;
  }
  debug_pwm (socpwm);
  socpwm.close();
}

/* -----------------------------------------------------------------------------
  pwmr <pin> [range]
*/
void
pwmr (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1) {

    throw Exception (Exception::ArgumentExpected);
  }

  pin = getPin (argv[optind]);
  if (pin->mode() != Pin::ModePwm) {

    throw Exception (Exception::NotPwmPin, pin->logicalNumber());
  }

  SocPwm socpwm (pin);
  socpwm.setDebug (debug);

  if (!socpwm.open()) {

    throw Exception (Exception::PwmOpenError, pin->logicalNumber());
  }

  if (paramc >= 2) {
    long value = stol (string (argv[optind + 1]));
    if (socpwm.setRange (value) < 0) {

      throw Exception (Exception::BadArguments);
    }
  }
  else {

    cout << socpwm.range() << endl;
  }
  debug_pwm (socpwm);
  socpwm.close();
}

/* -----------------------------------------------------------------------------
  pwmf <pin> [freq]
*/
void
pwmf (int argc, char *argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1) {

    throw Exception (Exception::ArgumentExpected);
  }

  pin = getPin (argv[optind]);
  if (pin->mode() != Pin::ModePwm) {

    throw Exception (Exception::NotPwmPin, pin->logicalNumber());
  }

  SocPwm socpwm (pin);
  socpwm.setDebug (debug);

  if (!socpwm.open()) {

    throw Exception (Exception::PwmOpenError, pin->logicalNumber());
  }

  if (paramc >= 2) {
    long value = stol (string (argv[optind + 1]));
    if (socpwm.setFrequency (value) < 0) {

      throw Exception (Exception::BadArguments);
    }
  }
  else {

    cout << socpwm.frequency() << endl;
  }
  debug_pwm (socpwm);
  socpwm.close();
}

// -----------------------------------------------------------------------------
vector<string>
split (const string &s, char seperator) {
  vector<string> output;
  string::size_type prev_pos = 0, pos = 0;

  while ( (pos = s.find (seperator, pos)) != string::npos) {

    string substring (s.substr (prev_pos, pos - prev_pos));
    output.push_back (substring);
    prev_pos = ++pos;
  }

  output.push_back (s.substr (prev_pos, pos - prev_pos)); // Last word
  return output;
}

// -----------------------------------------------------------------------------
Pin *
getPin (char *c_str) {
  Pin *p;
  string s (c_str);

  string::size_type n = s.find ('.');
  if (n != string::npos) {

    physicalNumbering = true;
  }

  try {

    if (!physicalNumbering) {

      pinnumber = stoi (s);
      p = &gpio.pin (pinnumber);
    }
    else {
      vector<string> v = split (s, '.');

      if (v.size() > 1) {

        connector = stoi (v[0]);
        pinnumber = stoi (v[1]);
      }
      else {

        connector = 1;
        pinnumber = stoi (v[0]);
      }
      p = &gpio.connector (connector)->pin (pinnumber);
    }
  }
  catch (out_of_range &e) {

    throw Exception (Exception::BadPinNumber, s);
  }

  p->enableGpioDev (forceGpioDev);
  return p;
}

// -----------------------------------------------------------------------------
void
sig_handler (int sig) {

  if (gpio.isOpen()) {

    if (useGpioDevBeforeWfi >= 0) {

      pin->enableGpioDev (useGpioDevBeforeWfi != 0);
    }

    cout << endl << "everything was closed.";
  }
  cout << endl << "Have a nice day !" << endl;
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
version() {

  cout << "Piduino pido version " << pidoVersion << endl;
  cout << "Copyright © 2018-" << PIDUINO_BUILD_YEAR << " " << authors << ", " << website << endl;
  cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions; type 'pido -w' for details." << endl << endl;
}

// -----------------------------------------------------------------------------
void
warranty () {
  cout <<
       "Copyright © 2018-" << PIDUINO_BUILD_YEAR << " " << authors << ", All rights reserved.\n\n"

       " Piduino pido is free software: you can redistribute it and/or modify\n"
       " it under the terms of the GNU General Public License as published by\n"
       " the Free Software Foundation, either version 3 of the License, or\n"
       " (at your option) any later version.\n\n"

       " Piduino pido is distributed in the hope that it will be useful,\n"
       " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
       " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
       " GNU General Public License for more details.\n\n"

       " You should have received a copy of the GNU General Public License\n"
       " along with Piduino pido. If not, see <http://www.gnu.org/licenses/>.\n";
}

// -----------------------------------------------------------------------------
void
usage () {
  cout << "usage : " << System::progName() << " [ options ] [ command ]  [ parameters ] [ options ]" << endl;;
  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "Allow the user easy access to the GPIO pins." << endl << endl;

  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "valid options are :" << endl;
  cout << "  -g\tUse the SOC pins numbers rather than PiDuino pin numbers." << endl;
  cout << "  -s\tUse the System pin numbers rather than PiDuino pin numbers." << endl;
  cout << "  -f\tForce to use Gpio2 device interface (/dev/gpiochipX)." << endl;
  cout << "  -1\tUse the connector pin numbers rather than PiDuino pin numbers." << endl;
  cout << "    \ta number is written in the form C.P, eg: 1.5 denotes pin 5 of connector #1." << endl;
  cout << "  -v\tOutput the current version and exit." << endl;
  cout << "  -w\tOutput the warranty and exit." << endl;
  cout << "  -h\tPrint this message and exit" << endl << endl;

  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "valid commands are :" << endl;
  cout << "  mode <pin> <in/out/off/pwm/alt{0..9}>" << endl;
  cout << "    Get/Set a pin mode into input, output, off, alt0..9 or pwm mode." << endl;
  cout << "  pull <pin> <up/down/off>" << endl;
  cout << "    Get/Set the internal pull-up, pull-down or off controls." << endl;
  cout << "  read <pin>" << endl;
  cout << "    Read the digital value of the given pin (0 or 1)" << endl;
  cout << "  readall [#connector]" << endl;
  cout << "    Output a table of all connectors with pins informations." << endl;
  cout << "  write <pin> <value>" << endl;
  cout << "    Write the given value (0 or 1) to the given pin (output)." << endl;
  cout << "  toggle <pin>" << endl;
  cout << "    Changes the state of a GPIO pin; 0 to 1, or 1 to 0 (output)." << endl;
  cout << "  blink <pin> [period]" << endl;
  cout << "    Blinks the given pin on/off (explicitly sets the pin to output)." << endl;
  cout << "  wfi <pin> <rising/falling/both> [timeout_ms]" << endl;
  cout << "    Waits  for  the  interrupt  to happen. It's a non-busy wait." << endl;
  cout << "  pwm <pin> <value>" << endl;
  cout << "    Write/Read a PWM value (0 to pwmr) to the given pin (pwm pin only)." << endl;
  cout << "  pwmr <pin> <range>" << endl;
  cout << "    Change/Read the PWM range. The default is 1024 (pwm pin only)." << endl;
  cout << "  pwmf <pin> <freq>" << endl;
  cout << "    Change/Read the PWM frequency. The default is about 1000Hz (pwm pin only)." << endl;
}

// -----------------------------------------------------------------------------
void debug_pwm (SocPwm &p) {
  #ifndef NDEBUG
  cout << "Pwm " << p.deviceName() << endl;
  cout << "hasEngine : " << p.hasEngine() << endl;
  cout << "isOpen    : " << p.isOpen() << endl;
  cout << "Frequency : " << p.frequency() << endl;
  cout << "Resolution: " << p.resolution() << endl;
  cout << "Range     : " << p.range() << endl;
  cout << "Min value : " << p.min() << endl;
  cout << "Max value : " << p.max() << endl;
  cout << "hasPin    : " << p.hasPin() << endl;
  if (p.hasPin()) {
    cout << "Pin#      : " << p.pin()->logicalNumber() << endl;
    cout << "isEnabled : " << p.isEnabled() << endl;
    cout << "Cur value : " << p.read() << endl;
  }
  #endif
}
/* ========================================================================== */
