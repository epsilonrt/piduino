#include <QtCore>
#include <QtSql>
#include "node.h"
#include "folder.h"
#include "boardfamily.h"
#include "gpio.h"
#include "socfamily.h"
#include "manufacturer.h"

// ---------------------------------------------------------------------------
//                          Class Folder
// ---------------------------------------------------------------------------
Folder::Folder (Type type, const QString & name, Node * parent) :
  Node (type, parent) {

  setFolder (true);
  setName (name);
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Folder::childrenFromDatabase() {

  clearChildren();

  if (type() == Node::TypeBoardFamily) {
    QSqlQuery q ("SELECT id,name FROM board_family", database());
    
    q.exec();
    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new BoardFamilyNode (i, q.value (1).toString(), this));
      }
    }
  }

  else if (type() == Node::TypeGpio) {
    QSqlQuery q ("SELECT id,name FROM gpio", database());

    q.exec();
    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new GpioNode (i, q.value (1).toString(), this));
      }
    }
  }
  else if (type() == Node::TypeSocFamily) {
    QSqlQuery q ("SELECT id,name FROM soc_family", database());

    q.exec();
    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new SocFamilyNode (i, q.value (1).toString(), this));
      }
    }
  }
  else if (type() == Node::TypeManufacturer) {
    QSqlQuery q ("SELECT id,name FROM manufacturer", database());

    q.exec();
    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new Manufacturer (i, q.value (1).toString(), this));
      }
    }
  }
}
