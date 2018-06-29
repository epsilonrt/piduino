#include "node_p.h"
#include "soc.h"

// ---------------------------------------------------------------------------
//                          Class SocNode
// ---------------------------------------------------------------------------
class SocNodePrivate : public NodePrivate {
  public:
    SocNodePrivate (Node * parent) :
      NodePrivate (Node::TypeSoc, parent), 
      manufacturer (QString("manufacturer"), parent->database(), true, 0) {
    }
    Property manufacturer;
    Q_DECLARE_PUBLIC (SocNode);
};
// ---------------------------------------------------------------------------
SocNode::SocNode (SocNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
SocNode::SocNode (int id, const QString & name, Node * parent) :
  Node (* new SocNodePrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT manufacturer_id "
             "FROM soc "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    manufacturer().setId(q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void SocNode::childrenFromDatabase() {
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

      // appendChild (new BoardModelNode (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
Property & SocNode::manufacturer()  {
  Q_D (SocNode);
  return d->manufacturer;
}
// ---------------------------------------------------------------------------
SocNode::~SocNode() {}
// ---------------------------------------------------------------------------
int SocNode::familyId() const {
  return parent()->id();
}
