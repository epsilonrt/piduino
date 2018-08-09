#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QPushButton>

class QAction;
class QWidget;

class ActionButton: public QPushButton
{
    Q_OBJECT
public:
    ActionButton(QAction *action, QWidget *parent = 0);
};


#endif
