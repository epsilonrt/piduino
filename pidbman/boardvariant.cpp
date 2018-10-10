#include "node_p.h"
#include "boardvariant.h"

// ---------------------------------------------------------------------------
//                          Class BoardVariantNode
// ---------------------------------------------------------------------------
class BoardVariantNodePrivate : public NodePrivate {
  public:
    BoardVariantNodePrivate (Node * parent) :
      NodePrivate (Node::TypeBoardVariant, parent), revision (0), ram (0),
      gpio_id (-1),
      manufacturer (QString ("manufacturer"), parent->database(), true, 0) {}
    int revision;
    int ram;
    int gpio_id;
    Property manufacturer;
    QString tag;
    QString pcb_revision;
    Q_DECLARE_PUBLIC (BoardVariantNode);
};
// ---------------------------------------------------------------------------
BoardVariantNode::BoardVariantNode (BoardVariantNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
BoardVariantNode::BoardVariantNode (int id, const QString & name, Node * parent) :
  Node (* new BoardVariantNodePrivate (parent)) {
  QSqlQuery q (database());

  q.prepare ("SELECT tag,revision,ram,pcb_revision,gpio_id,manufacturer_id "
             "FROM board "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  setId (id);
  setName (name);

  if (q.next()) {

    if (!q.value (0).isNull()) {
      setTag (q.value (0).toString());
    }
    if (!q.value (1).isNull()) {
      setRevision (q.value (1).toInt());
    }
    if (!q.value (2).isNull()) {
      setRam (q.value (2).toInt());
    }
    if (!q.value (3).isNull()) {
      setPcbRevision (q.value (3).toString());
    }
    setGpioId (q.value (4).toInt());
    manufacturer().setId(q.value (5).toInt());
  }
}
// ---------------------------------------------------------------------------
BoardVariantNode::~BoardVariantNode() {}
// ---------------------------------------------------------------------------
int BoardVariantNode::modelId() const {
  return parent()->id();
}
// ---------------------------------------------------------------------------
int BoardVariantNode::revision() const {
  Q_D (const BoardVariantNode);
  return d->revision;
}
// ---------------------------------------------------------------------------
int BoardVariantNode::ram() const {
  Q_D (const BoardVariantNode);
  return d->ram;
}
// ---------------------------------------------------------------------------
const QString & BoardVariantNode::pcbRevision() const {
  Q_D (const BoardVariantNode);
  return d->pcb_revision;
}
// ---------------------------------------------------------------------------
int BoardVariantNode::gpioId() const {
  Q_D (const BoardVariantNode);
  return d->gpio_id;
}
// ---------------------------------------------------------------------------
Property & BoardVariantNode::manufacturer()  {
  Q_D (BoardVariantNode);
  return d->manufacturer;
}
// ---------------------------------------------------------------------------
void BoardVariantNode::setTag (const QString & value) {
  Q_D (BoardVariantNode);
  d->tag = value;
}
// ---------------------------------------------------------------------------
void BoardVariantNode::setRevision (int value) {
  Q_D (BoardVariantNode);
  d->revision = value;
}
// ---------------------------------------------------------------------------
void BoardVariantNode::setRam (int value) {
  Q_D (BoardVariantNode);
  d->ram = value;
}
// ---------------------------------------------------------------------------
void BoardVariantNode::setPcbRevision (const QString & value) {
  Q_D (BoardVariantNode);
  d->pcb_revision = value;
}
// ---------------------------------------------------------------------------
void BoardVariantNode::setGpioId (int value) {
  Q_D (BoardVariantNode);
  d->gpio_id = value;
}
