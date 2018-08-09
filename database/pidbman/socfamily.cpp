#include "node_p.h"
#include "property_p.h"
#include "socfamily.h"
#include "soc.h"

// ---------------------------------------------------------------------------
//                          Class SocFamily
// ---------------------------------------------------------------------------
class SocFamilyPrivate : public PropertyPrivate {
  public:
    SocFamilyPrivate (QSqlDatabase & database) :
      PropertyPrivate (QString ("soc_family"), database, false),
      arch (QString("arch"), database, false, 0)  {}
    Property arch;
    Q_DECLARE_PUBLIC (SocFamily);
};
// ---------------------------------------------------------------------------
SocFamily::SocFamily (SocFamilyPrivate &dd) : Property (dd) {}
// ---------------------------------------------------------------------------
SocFamily::SocFamily (QSqlDatabase & database, QObject * parent) :
  Property (* new SocFamilyPrivate (database)) {

  setParent (parent);
}
// ---------------------------------------------------------------------------
SocFamily::~SocFamily() {}
// ---------------------------------------------------------------------------
Property & SocFamily::arch()  {
  Q_D (SocFamily);
  return d->arch;
}
// ---------------------------------------------------------------------------
bool SocFamily::readFromDatabase() {
  Q_D (SocFamily);
  QSqlQuery q (database());

  q.prepare(QString ("SELECT name,arch_id FROM %1 WHERE id=?").arg (table()));
  q.addBindValue (d->id);
  q.exec();
  if (q.next()) {
    bool hasChanged = false;
    QString name = q.value (0).toString();
    int arch_id =  q.value (1).toInt();

    if (d->name != name) {
      d->name = name;
      hasChanged = true;
    }
    if (d->arch.id() != arch_id) {
      d->arch.setId(arch_id);
      hasChanged = true;
    }
    if (hasChanged) {
      emit changed();
    }
    return true;
  }
  return false;
}

// ---------------------------------------------------------------------------
//                          Class SocFamilyNode
// ---------------------------------------------------------------------------
class SocFamilyNodePrivate : public NodePrivate {
  public:
    SocFamilyNodePrivate (Node * parent) :
      NodePrivate (Node::TypeSocFamily, parent),
      arch (QString("arch"), parent->database(), false, 0) {
    }
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
