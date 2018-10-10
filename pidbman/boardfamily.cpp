#include <QtCore>
#include <QtSql>
#include "node.h"
#include "boardfamily.h"
#include "boardmodel.h"

// ---------------------------------------------------------------------------
//                          Class BoardFamilyNode
// ---------------------------------------------------------------------------
BoardFamilyNode::BoardFamilyNode (int id, const QString & name, Node * parent) :
  Node (Node::TypeBoardFamily, parent) {

  setId (id);
  setName (name);
  childrenFromDatabase();
}

// ---------------------------------------------------------------------------
void BoardFamilyNode::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT id,name "
             "FROM board_model "
             "WHERE board_family_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {

      appendChild (new BoardModelNode (i, q.value (1).toString(), this));
    }
  }
}
