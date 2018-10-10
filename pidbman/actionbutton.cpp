#include <QAction>
#include "actionbutton.h"

ActionButton::ActionButton (QAction *action, QWidget *parent) :
  QPushButton (parent) {
  setIcon (action->icon());
  setText (action->text());
  connect (this, SIGNAL (clicked()), action, SLOT (trigger()));
}
