/* Copyright © 2020 Pascal JEAN, All rights reserved.
 *
 * Piduino pidbm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Piduino pidbm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Piduino pidbm.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <popl.h>
#include "pidbm_p.h"
#include "version.h"

using namespace std;
using namespace Popl;

// Nom du programme en cours défini par la glibc
extern char * program_invocation_name; // glibc
extern char * __progname;

// ---------------------------------------------------------------------------
std::string progName() {
#ifdef HAVE_GETPROGNAME_FUNCTION
  return std::string (getprogname ());
#else
  return std::string (__progname);
#endif
}

// ---------------------------------------------------------------------------
//
//                            Pidbm Class
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
Pidbm::Pidbm (Pidbm::Private &dd) : d_ptr (&dd) {

}

// ---------------------------------------------------------------------------
Pidbm::Pidbm () :
  d_ptr (new Private (this))  {

}

// ---------------------------------------------------------------------------
Pidbm::~Pidbm() = default;


// ---------------------------------------------------------------------------
bool
Pidbm::open (int argc, char **argv) {

  if (!isOpen()) {
    PIMP_D (Pidbm);

    d->op.parse (argc, argv);

    d->db.open (d->cinfo);
  }
  return isOpen();
}

// ---------------------------------------------------------------------------
bool
Pidbm::exec () {

  if (isOpen()) {
    PIMP_D (Pidbm);

    // TODO
  }
  return false;
}

// ---------------------------------------------------------------------------
void
Pidbm::close() {

  if (isOpen()) {
    PIMP_D (Pidbm);

    d->db.close();
  }
}

// ---------------------------------------------------------------------------
bool
Pidbm::isOpen() const {
  PIMP_D (const Pidbm);

  return d->db.is_open();
}

// -----------------------------------------------------------------------------
void
Pidbm::version() {

  cout << "Piduino pidbm version " << VERSION_SHORT << endl;
  cout << "Copyright © 2020 " << Private::Authors << ", " << Private::Website << endl;
  cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions; type 'pidbm -w' for details." << endl << endl;
}

// -----------------------------------------------------------------------------
void
Pidbm::warranty () {
  cout <<
       "Copyright © 2020 " << Private::Authors << ", All rights reserved.\n\n"

       " Piduino pidbm is free software: you can redistribute it and/or modify\n"
       " it under the terms of the GNU General Public License as published by\n"
       " the Free Software Foundation, either version 3 of the License, or\n"
       " (at your option) any later version.\n\n"

       " Piduino pidbm is distributed in the hope that it will be useful,\n"
       " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
       " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
       " GNU General Public License for more details.\n\n"

       " You should have received a copy of the GNU General Public License\n"
       " along with Piduino pidbm. If not, see <http://www.gnu.org/licenses/>.\n";
}

// -----------------------------------------------------------------------------
void
Pidbm::usage () {
  cout << "usage : " << progName() << " [ options ]" << endl;
  //01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "Retrieves information from the pi board." << endl;
  cout << "If only one information is requested, it is displayed without formatting," << endl;
  cout << "otherwise each information is displayed on a line preceded by its name." << endl;
  cout << "Return value: 0 if the board is a pi board, 1 otherwise" << endl;
  cout << "valid options are :" << endl;
  //01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "  -a  --all       \tPrints all informations (default)" << endl;
  cout << "  -n  --name      \tPrints the \"human-readable\" name of the board." << endl;
  cout << "  -f  --family    \tPrints the board family (raspberrypi, nanopi, ...)" << endl;
  cout << "  -i  --id        \tPrints the piduino database id (for debug purpose)" << endl;
  cout << "  -s  --soc       \tPrints the SoC model (bcm2708 ...)" << endl;
  cout << "  -r  --revision  \tPrints the revision number of the board in hexadecimal (prefixed by 0x)." << endl;
  cout << "  -t  --tag       \tPrints the identifier tag of the board." << endl;
  cout << "  -g  --gpio      \tPrints the revision number of the GPIO in decimal" << endl;
  cout << "  -m  --mem       \tPrints the RAM size in megabytes" << endl;
  cout << "  -p  --pcb       \tPrints the revision number of the PCB in the form M.m" << endl;
  cout << "  -b  --builder   \tPrints the name of the manufacturer." << endl;
  cout << "  -S  --serial    \tPrints serial ports available on the SoC." << endl;
  cout << "  -h  --help      \tPrints this message" << endl;
  cout << "  -v  --version   \tPrints version and exit" << endl;
  cout << "  -w  --warranty  \tOutput the warranty and exit." << endl;
}

// ---------------------------------------------------------------------------
//
//                         Pidbm::Private Class
//
// ---------------------------------------------------------------------------

#if 0
OptionParser CmdLine ("Allowed options");
int ledPin = 0;
unsigned long highDelay = 500;
unsigned long lowDelay = 500;
auto pin_option = CmdLine.add<Value<int>> ("p", "pin", "set pin number");
CmdLine.add<Value<unsigned long>> ("H", "high", "set high state delay", 500, &highDelay);
CmdLine.add<Value<unsigned long>> ("L", "low", "set low state delay", 500, &lowDelay);
CmdLine.parse (argc, argv);

CmdLine.add<Switch> ("h", "help", "produce help message");
CmdLine.add<Switch> ("v", "verbose", "be verbose");
CmdLine.parse (argc, argv);

if (! pin_option->is_set()) {
  exit (EXIT_FAILURE);
}
ledPin = pin_option->value();
#endif

// ---------------------------------------------------------------------------
Pidbm::Private::Private (Pidbm * q) :
  q_ptr (q), op ("Allowed options") {

  op.add<Switch> ("h", "help", "Prints this message", false, &opHelp);
  op.add<Switch> ("v", "version", "Prints version and exit", false, &opVersion);
  op.add<Switch> ("w", "warranty", "Output the warranty and exit", false, &opWarranty);
}

// ---------------------------------------------------------------------------
Pidbm::Private::~Private() {

  db.close();
}

const std::string Pidbm::Private::Authors = "Pascal JEAN";
const std::string Pidbm::Private::Website = "https://github.com/epsilonrt/piduino";

/* ========================================================================== */
