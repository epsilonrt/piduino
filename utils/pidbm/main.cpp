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
#include "pidbm.h"

using namespace std;

int
main (int argc, char **argv) {
  Pidbm m;
  int ret = EXIT_SUCCESS;

  try {

    if (m.parse (argc, argv) > 0) {

      m.open();
      m.exec();
    }
  }
  catch (const Popl::invalid_option & e) {

    cerr << "Syntax error: " << e.what() << endl << endl;
    m.help (cerr);
    ret = EXIT_FAILURE;
  }
  catch (const std::invalid_argument & e) {

    cerr << "Syntax error: " << e.what() << endl << endl;
    m.help (cerr);
    ret = EXIT_FAILURE;
  }
  catch (const std::exception& e) {

    cerr << "Error: " << e.what() << endl;
    ret =  EXIT_FAILURE;
  }

  m.close();
  return ret;
}

/* ========================================================================== */
