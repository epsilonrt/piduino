#include "node_p.h"
#include "socfamily.h"
#include "soc.h"

// ---------------------------------------------------------------------------
//                          Class SocFamilyNode
// ---------------------------------------------------------------------------
class SocFamilyNodePrivate : public NodePrivate {
  public:
    SocFamilyNodePrivate (Node * parent) :
      NodePrivate (Node::TypeSocFamily, parent),
      arch (QString("arch"), parent->database(), false, 0) {
    }
    Property arch;
    Q_DECLARE_PUBLIC (SocFamilyNode);
};
// ---------------------------------------------------------------------------
SocFamilyNode::SocFamilyNode (SocFamilyNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
SocFamilyNode::SocFamilyNode (int id, const QString & name, Node * parent) :
  Node (* new SocFamilyNodePrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT arch_id "
             "FROM soc_family "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    arch().setId(q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void SocFamilyNode::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT id,name "
             "FROM soc "
             "WHERE soc_family_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {

      appendChild (new SocNode (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
SocFamilyNode::~SocFamilyNode() {}
// ---------------------------------------------------------------------------
Property & SocFamilyNode::arch()  {
  Q_D (SocFamilyNode);
  return d->arch;
}
