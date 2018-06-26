#include <QtCore>
#include <QtSql>

#include "node.h"

// ---------------------------------------------------------------------------
//                          Abstract Class Node
// ---------------------------------------------------------------------------
class NodePrivate {
  public:
    NodePrivate (Node::Type t, Node * p) :
      type (t), id (-1), db (0), parent (p), is_folder (false) {

      setDatabase();
    }
    Node::Type type;
    int id;
    QSqlDatabase * db;
    Node * parent;
    Node * q_ptr;
    bool is_folder;
    QString name;
    QList<Node *> children;

    void setDatabase() {
      if (parent) {
        db = &parent->database();
      }
    }
    Q_DECLARE_PUBLIC (Node);
};
// ---------------------------------------------------------------------------
Node::Node (NodePrivate &dd) : d_ptr (&dd) {

  d_ptr->q_ptr = this;
}
// ---------------------------------------------------------------------------
Node::Node (Type type, Node * parent) :
  QObject (parent), d_ptr (new NodePrivate (type, parent))  {

  d_ptr->q_ptr = this;
}
// ---------------------------------------------------------------------------
Node::Node (Type type, QObject * parent) :
  QObject (parent), d_ptr (new NodePrivate (type, 0))  {

  d_ptr->q_ptr = this;
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
  d->setDatabase();
}
// ---------------------------------------------------------------------------
const QList<Node *> & Node::children() const {
  return d_ptr->children;
}
// ---------------------------------------------------------------------------
Node * Node::root() {
  
  if (parent()) {
    Node * r = parent();
    
    while (r->parent()) {
      r = parent();
    }
    return r;
  }
  return this;
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
const QString & Node::name() const {
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
  return  *d_ptr->db;
}
// ---------------------------------------------------------------------------
void Node::setDatabase (QSqlDatabase * db) {
  Q_D (Node);

  if (db) {

    d->db = db;
  }
  else  {

    d->setDatabase();
  }
}
// ---------------------------------------------------------------------------
void Node::updateToDatabase() const {}
// ---------------------------------------------------------------------------
void Node::insertToDatabase() const {}
// ---------------------------------------------------------------------------
void Node::deleteFromDatabase() const {}

// ---------------------------------------------------------------------------
//                          Class Database
// ---------------------------------------------------------------------------
Database::Database (QSqlDatabase & db, QObject * parent) :
  Node (Node::TypeDatabase, parent) {

  setName (db.databaseName());
  setDatabase (&db);
  childrenFromDatabase();
}


// ---------------------------------------------------------------------------
void Database::childrenFromDatabase() {

  clearChildren();
  appendChild (new Folder (Node::TypeBoardFamily, tr ("Board"), this));
  appendChild (new Folder (Node::TypeSocFamily, tr ("Soc"), this));
  appendChild (new Folder (Node::TypeManufacturer, tr ("Manufacturer"), this));
}

// ---------------------------------------------------------------------------
//                          Class Folder
// ---------------------------------------------------------------------------
Folder::Folder (Type type, const QString & name, Node * parent) :
  Node (type, parent) {

  setFolder (true);
  setName (name);
  setDatabase ();
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Folder::childrenFromDatabase() {

  if (type() == Node::TypeBoardFamily) {

    QSqlQuery q ("SELECT id,name FROM board_family", database());

    q.exec();
    clearChildren();

    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new BoardFamily (i, q.value (1).toString(), this));
      }
    }
  }
  else if (type() == Node::TypeSocFamily) {

    QSqlQuery q ("SELECT id,name FROM soc_family", database());

    q.exec();
    clearChildren();

    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new SocFamily (i, q.value (1).toString(), this));
      }
    }
  }
  else if (type() == Node::TypeManufacturer) {

    QSqlQuery q ("SELECT id,name FROM manufacturer", database());

    q.exec();
    clearChildren();

    while (q.next()) {

      int i = q.value (0).toInt();
      if (i >= 0) {

        appendChild (new Manufacturer (i, q.value (1).toString(), this));
      }
    }
  }
}

// ---------------------------------------------------------------------------
//                          Class SocFamily
// ---------------------------------------------------------------------------
class SocFamilyPrivate : public NodePrivate {
  public:
    SocFamilyPrivate (Node * parent) :
      NodePrivate (Node::TypeSocFamily, parent), arch_id (-1) {
    }
    int arch_id;
    Q_DECLARE_PUBLIC (SocFamily);
};
// ---------------------------------------------------------------------------
SocFamily::SocFamily (SocFamilyPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
SocFamily::SocFamily (int id, const QString & name, Node * parent) :
  Node (* new SocFamilyPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT arch_id "
             "FROM soc_family "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    setArchId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void SocFamily::childrenFromDatabase() {
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

      appendChild (new Soc (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
SocFamily::~SocFamily() {}
// ---------------------------------------------------------------------------
int SocFamily::archId() const {
  Q_D (const SocFamily);
  return d->arch_id;
}
// ---------------------------------------------------------------------------
void SocFamily::setArchId (int value) {
  Q_D (SocFamily);
  d->arch_id = value;
}
// ---------------------------------------------------------------------------
QString  SocFamily::archName() const {
  QSqlQuery q (database());
  q.prepare ("SELECT name "
             "FROM arch "
             "WHERE id = ?");
  q.addBindValue (archId());
  q.exec();
  if (q.next()) {
    return q.value (0).toString();
  }
  return QString (tr ("Unknown"));
}

// ---------------------------------------------------------------------------
//                          Class Soc
// ---------------------------------------------------------------------------
class SocPrivate : public NodePrivate {
  public:
    SocPrivate (Node * parent) :
      NodePrivate (Node::TypeSoc, parent), manufacturer_id(-1) {
    }
    int manufacturer_id;
    Q_DECLARE_PUBLIC (Soc);
};
// ---------------------------------------------------------------------------
Soc::Soc (SocPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
Soc::Soc (int id, const QString & name, Node * parent) :
  Node (* new SocPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT manufacturer_id "
             "FROM soc "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    setManufacturerId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Soc::childrenFromDatabase() {
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

      // appendChild (new BoardModel (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
int Soc::manufacturerId() const {
  Q_D (const Soc);
  return d->manufacturer_id;
}
// ---------------------------------------------------------------------------
void Soc::setManufacturerId (int value) {
  Q_D (Soc);
  d->manufacturer_id = value;
}

// ---------------------------------------------------------------------------
Soc::~Soc() {}
// ---------------------------------------------------------------------------
int Soc::familyId() const {
  return parent()->id();
}

// ---------------------------------------------------------------------------
//                          Class Manufacturer
// ---------------------------------------------------------------------------
class ManufacturerPrivate : public NodePrivate {
  public:
    ManufacturerPrivate (Node * parent) :
      NodePrivate (Node::TypeManufacturer, parent) {
    }
    Q_DECLARE_PUBLIC (Manufacturer);
};
// ---------------------------------------------------------------------------
Manufacturer::Manufacturer (ManufacturerPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
Manufacturer::Manufacturer (int id, const QString & name, Node * parent) :
  Node (* new ManufacturerPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT soc_id "
             "FROM board_model "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    //setSocId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Manufacturer::childrenFromDatabase() {
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

      // appendChild (new BoardModel (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
Manufacturer::~Manufacturer() {}

// ---------------------------------------------------------------------------
//                          Class BoardFamily
// ---------------------------------------------------------------------------
BoardFamily::BoardFamily (int id, const QString & name, Node * parent) :
  Node (Node::TypeBoardFamily, parent) {

  setId (id);
  setName (name);
  childrenFromDatabase();
}

// ---------------------------------------------------------------------------
void BoardFamily::childrenFromDatabase() {
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

      appendChild (new BoardModel (i, q.value (1).toString(), this));
    }
  }
}

// ---------------------------------------------------------------------------
//                          Class BoardModel
// ---------------------------------------------------------------------------
class BoardModelPrivate : public NodePrivate {
  public:
    BoardModelPrivate (Node * parent) :
      NodePrivate (Node::TypeBoardModel, parent), soc_id (-1) {
    }
    int soc_id;
    Q_DECLARE_PUBLIC (BoardModel);
};
// ---------------------------------------------------------------------------
BoardModel::BoardModel (BoardModelPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
BoardModel::BoardModel (int id, const QString & name, Node * parent) :
  Node (* new BoardModelPrivate (parent)) {
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
void BoardModel::childrenFromDatabase() {
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
        appendChild (new BoardVariant (i, str, this));
      }
    }
  }
}
// ---------------------------------------------------------------------------
BoardModel::~BoardModel() {}
// ---------------------------------------------------------------------------
int BoardModel::familyId() const {
  return parent()->id();
}
// ---------------------------------------------------------------------------
int BoardModel::socId() const {
  Q_D (const BoardModel);
  return d->soc_id;
}
// ---------------------------------------------------------------------------
void  BoardModel::setSocId (int soc) {
  Q_D (BoardModel);
  d->soc_id = soc;
}

// ---------------------------------------------------------------------------
//                          Class BoardVariant
// ---------------------------------------------------------------------------
class BoardVariantPrivate : public NodePrivate {
  public:
    BoardVariantPrivate (Node * parent) :
      NodePrivate (Node::TypeBoardVariant, parent), revision (0), ram (0), gpio_id (-1), manufacturer_id (-1) {
    }
    int revision;
    int ram;
    int gpio_id;
    int manufacturer_id;
    QString tag;
    QString pcb_revision;
    Q_DECLARE_PUBLIC (BoardVariant);
};
// ---------------------------------------------------------------------------
BoardVariant::BoardVariant (BoardVariantPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
BoardVariant::BoardVariant (int id, const QString & name, Node * parent) :
  Node (* new BoardVariantPrivate (parent)) {
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
    setManufacturerId (q.value (5).toInt());
  }

  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
BoardVariant::~BoardVariant() {}
// ---------------------------------------------------------------------------
void BoardVariant::childrenFromDatabase() {
  QSqlQuery q (database());

  clearChildren();

  q.prepare ("SELECT name "
             "FROM gpio "
             "WHERE id = ?");
  q.addBindValue (gpioId());
  q.exec();
  if (q.next()) {

    appendChild (new Gpio (gpioId(), q.value (0).toString(), this));
  }

  q.prepare ("SELECT name "
             "FROM manufacturer "
             "WHERE id = ?");
  q.addBindValue (manufacturerId());
  q.exec();
  if (q.next()) {

    //appendChild (new Manufacturer (manufacturerId(), q.value (0).toString(), this));
  }
}
// ---------------------------------------------------------------------------
int BoardVariant::modelId() const {
  return parent()->id();
}
// ---------------------------------------------------------------------------
int BoardVariant::revision() const {
  Q_D (const BoardVariant);
  return d->revision;
}
// ---------------------------------------------------------------------------
int BoardVariant::ram() const {
  Q_D (const BoardVariant);
  return d->ram;
}
// ---------------------------------------------------------------------------
const QString & BoardVariant::pcbRevision() const {
  Q_D (const BoardVariant);
  return d->pcb_revision;
}
// ---------------------------------------------------------------------------
int BoardVariant::gpioId() const {
  Q_D (const BoardVariant);
  return d->gpio_id;
}
// ---------------------------------------------------------------------------
int BoardVariant::manufacturerId() const {
  Q_D (const BoardVariant);
  return d->manufacturer_id;
}
// ---------------------------------------------------------------------------
void BoardVariant::setTag (const QString & value) {
  Q_D (BoardVariant);
  d->tag = value;
}
// ---------------------------------------------------------------------------
void BoardVariant::setRevision (int value) {
  Q_D (BoardVariant);
  d->revision = value;
}
// ---------------------------------------------------------------------------
void BoardVariant::setRam (int value) {
  Q_D (BoardVariant);
  d->ram = value;
}
// ---------------------------------------------------------------------------
void BoardVariant::setPcbRevision (const QString & value) {
  Q_D (BoardVariant);
  d->pcb_revision = value;
}
// ---------------------------------------------------------------------------
void BoardVariant::setGpioId (int value) {
  Q_D (BoardVariant);
  d->gpio_id = value;
}
// ---------------------------------------------------------------------------
void BoardVariant::setManufacturerId (int value) {
  Q_D (BoardVariant);
  d->manufacturer_id = value;
}

// ---------------------------------------------------------------------------
//                          Class Gpio
// ---------------------------------------------------------------------------
Gpio::Gpio (int id, const QString & name, Node * parent) :
  Node (Node::TypeGpio, parent) {

  setId (id);
  setName (name);
  childrenFromDatabase();
}

// ---------------------------------------------------------------------------
void Gpio::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT gpio_connector.id,gpio_connector.name,num "
             "FROM gpio_has_connector "
             "INNER JOIN gpio_connector ON gpio_has_connector.gpio_connector_id = gpio_connector.id  "
             "WHERE gpio_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {
      /*
        Connector * c = new Connector (i, q.value (1).toString(), this);
        c->setNumber(q.value (2).toInt());
        appendChild (c); */
    }
  }
}
// ---------------------------------------------------------------------------
int Gpio::variantId() const {
  return parent()->id();
}

// ---------------------------------------------------------------------------
//                          Class Connector
// ---------------------------------------------------------------------------
class ConnectorPrivate : public NodePrivate {
  public:
    ConnectorPrivate (Node * parent) :
      NodePrivate (Node::TypeConnector, parent) {
    }
    Q_DECLARE_PUBLIC (Connector);
};
// ---------------------------------------------------------------------------
Connector::Connector (ConnectorPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
Connector::Connector (int id, const QString & name, Node * parent) :
  Node (* new ConnectorPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT soc_id "
             "FROM board_model "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    //setSocId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Connector::childrenFromDatabase() {
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

      // appendChild (new BoardModel (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
Connector::~Connector() {}
// ---------------------------------------------------------------------------
int Connector::gpioId() const {
  return parent()->id();
}

// ---------------------------------------------------------------------------
//                          Class Pin
// ---------------------------------------------------------------------------
class PinPrivate : public NodePrivate {
  public:
    PinPrivate (Node * parent) :
      NodePrivate (Node::TypePin, parent) {
    }
    Q_DECLARE_PUBLIC (Pin);
};
// ---------------------------------------------------------------------------
Pin::Pin (PinPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
Pin::Pin (int id, const QString & name, Node * parent) :
  Node (* new PinPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT soc_id "
             "FROM board_model "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    //setSocId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Pin::childrenFromDatabase() {
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

      // appendChild (new BoardModel (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
Pin::~Pin() {}
// ---------------------------------------------------------------------------
int Pin::connectorId() const {
  return parent()->id();
}

// ---------------------------------------------------------------------------
//                          Class Function
// ---------------------------------------------------------------------------
class FunctionPrivate : public NodePrivate {
  public:
    FunctionPrivate (Node * parent) :
      NodePrivate (Node::TypeFunction, parent) {
    }
    Q_DECLARE_PUBLIC (Function);
};
// ---------------------------------------------------------------------------
Function::Function (FunctionPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
Function::Function (int id, const QString & name, Node * parent) :
  Node (* new FunctionPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT soc_id "
             "FROM board_model "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    //setSocId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Function::childrenFromDatabase() {
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

      // appendChild (new BoardModel (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
Function::~Function() {}
// ---------------------------------------------------------------------------
int Function::pinId() const {
  return parent()->id();
}
