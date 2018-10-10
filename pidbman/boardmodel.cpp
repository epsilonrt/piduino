#include "node_p.h"
#include "boardmodel.h"
#include "boardvariant.h"

// ---------------------------------------------------------------------------
//                          Class BoardModelNode
// ---------------------------------------------------------------------------
class BoardModelNodePrivate : public NodePrivate {
  public:
    BoardModelNodePrivate (Node * parent) :
      NodePrivate (Node::TypeBoardModel, parent), soc_id (-1) {
    }
    int soc_id;
    Q_DECLARE_PUBLIC (BoardModelNode);
};
// ---------------------------------------------------------------------------
BoardModelNode::BoardModelNode (BoardModelNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
BoardModelNode::BoardModelNode (int id, const QString & name, Node * parent) :
  Node (* new BoardModelNodePrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT soc_id "
             "FROM board_model "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    setSocId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void BoardModelNode::childrenFromDatabase() {
  QSqlQuery q (database());

  q.prepare ("SELECT id,revision,tag "
             "FROM board "
             "WHERE board_model_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {
      QString str;

      if (q.value (1).isNull()) {
        if (!q.value (2).isNull()) {

          str = q.value (2).toString();
        }
      }
      else {

        str = QString ("Rev. 0x%1").arg (q.value (1).toInt(), 0, 16);
      }

      if (!str.isEmpty()) {
        appendChild (new BoardVariantNode (i, str, this));
      }
    }
  }
}
// ---------------------------------------------------------------------------
BoardModelNode::~BoardModelNode() {}
// ---------------------------------------------------------------------------
int BoardModelNode::familyId() const {
  return parent()->id();
}
// ---------------------------------------------------------------------------
int BoardModelNode::socId() const {
  Q_D (const BoardModelNode);
  return d->soc_id;
}
// ---------------------------------------------------------------------------
void  BoardModelNode::setSocId (int soc) {
  Q_D (BoardModelNode);
  d->soc_id = soc;
}
