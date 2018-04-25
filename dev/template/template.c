/*
 * template.c
 * @brief Description of your program
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <piduino/delay.h>

/* constants ================================================================ */
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {

  // Replace the lines below with your code.
  // vSigIntHandler() intercepts the CTRL+C.
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");
  for (;;) {
    putchar ('.');
    fflush (stdout);
    delay_ms (1000);
  }

  return 0;
}
/* ========================================================================== */
