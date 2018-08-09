#include "node_p.h"

// ---------------------------------------------------------------------------
//                          Abstract Class Node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
void NodePrivate::setRoot (Node * q) {
  Node * r = q;

  q_ptr = q;
  while (r->parent()) {
    r = r->parent();
  }

  if (r->type() == Node::TypeDatabase) {
    root = reinterpret_cast<Database *>(r);
  }
}

// ---------------------------------------------------------------------------
Node::Node (NodePrivate &dd) : d_ptr (&dd) {

  d_ptr->setRoot (this);
}
// ---------------------------------------------------------------------------
Node::Node (Type type, Node * parent) :
  QObject (parent), d_ptr (new NodePrivate (type, parent))  {

  d_ptr->setRoot (this);
}
// ---------------------------------------------------------------------------
Node::Node (Type type, QObject * parent) :
  QObject (parent), d_ptr (new NodePrivate (type, 0))  {

  d_ptr->setRoot (this);
}
// ---------------------------------------------------------------------------
Node::~Node() {

  qDeleteAll (d_ptr->children);
}
// ---------------------------------------------------------------------------
Node::Type Node::type() const {
  return d_ptr->type;
}
// ---------------------------------------------------------------------------
int Node::id() const {
  return d_ptr->id;
}
// ---------------------------------------------------------------------------
void Node::setFolder (bool value) {

  d_ptr->is_folder = value;
}
// ---------------------------------------------------------------------------
bool Node::isFolder() const {

  return d_ptr->is_folder;
}
// ---------------------------------------------------------------------------
Node * Node::parent() const {
  return d_ptr->parent;
}
// ---------------------------------------------------------------------------
void Node::setParent (Node * parent) {
  Q_D (Node);
  d->parent = parent;
  d->setRoot(this);
}
// ---------------------------------------------------------------------------
const QList<Node *> & Node::children() const {
  return d_ptr->children;
}
// ---------------------------------------------------------------------------
Database * Node::root() const {
  return d_ptr->root;
}
// ---------------------------------------------------------------------------
void Node::clearChildren() {
  Q_D (Node);

  if (!d->children.isEmpty()) {
    qDeleteAll (d->children);
    d->children.clear();
  }
}
// ---------------------------------------------------------------------------
void Node::appendChild (Node * child) {

  d_ptr->children.append (child);
}
// ---------------------------------------------------------------------------
QString Node::name() const {
  return d_ptr->name;
}
// ---------------------------------------------------------------------------
void Node::setName (const QString & name) {
  d_ptr->name = name;
}
// ---------------------------------------------------------------------------
void Node::setId (int id) {
  d_ptr->id = id;
}
// ---------------------------------------------------------------------------
QSqlDatabase & Node::database() const {
  return  d_ptr->root->database();
}
// ---------------------------------------------------------------------------
QString Node::toolTip() const {
  return QString();
}
// ---------------------------------------------------------------------------
void Node::updateToDatabase() const {}
// ---------------------------------------------------------------------------
void Node::insertToDatabase() const {}
// ---------------------------------------------------------------------------
void Node::deleteFromDatabase() const {}
// ---------------------------------------------------------------------------
void Node::childrenFromDatabase() {}
