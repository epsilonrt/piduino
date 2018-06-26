#include <QtCore>
#include <QtSql>
#include <QFileIconProvider>

#include "treemodel.h"
#include "node.h"

// ---------------------------------------------------------------------------
TreeModel::TreeModel (const QString & sqlite3Filename, QObject *parent)
  : QAbstractItemModel (parent), rootNode (0)  {

  db = QSqlDatabase::addDatabase ("QSQLITE");

  db.setDatabaseName (sqlite3Filename);
  if (db.open()) {

    rootNode = new Database (db, this);
  }
}

// ---------------------------------------------------------------------------
TreeModel::~TreeModel() {

  db.close();
}

// ---------------------------------------------------------------------------
QModelIndex TreeModel::index (int row, int column,
                              const QModelIndex &parent) const {
  if (rootNode == 0) {
    return QModelIndex();
  }
  Node *parentNode = nodeFromIndex (parent);
  return createIndex (row, column, parentNode->children() [row]);
}

// ---------------------------------------------------------------------------
QModelIndex TreeModel::parent (const QModelIndex &child) const {

  Node *node = nodeFromIndex (child);
  if (!node) {
    return QModelIndex();
  }
  Node *parentNode = node->parent();
  if (!parentNode) {
    return QModelIndex();
  }
  Node *grandparentNode = parentNode->parent();
  if (!grandparentNode) {
    return QModelIndex();
  }

  int row = grandparentNode->children().indexOf (parentNode);
  return createIndex (row, child.column(), parentNode);
}

// ---------------------------------------------------------------------------
int TreeModel::rowCount (const QModelIndex &parent) const {

  Node *parentNode = nodeFromIndex (parent);
  if (!parentNode) {

    return 0;
  }
  return parentNode->children().count();
}

// ---------------------------------------------------------------------------
int TreeModel::columnCount (const QModelIndex & /* parent */) const {

  return 1;
}

// ---------------------------------------------------------------------------
QVariant TreeModel::data (const QModelIndex &index, int role) const {

  if (role == Qt::DecorationRole) {
    QFileIconProvider iconProvider;

    Node * node = nodeFromIndex (index);
    if (!node) {

      return QVariant();
    }

    if (node->isFolder()) {

      return iconProvider.icon (QFileIconProvider::Folder);
    }
    else {

      switch (node->type()) {
        case Node::TypeDatabase:
        case Node::TypeBoardFamily:
        case Node::TypeSocFamily:
          return iconProvider.icon (QFileIconProvider::Folder);
        case Node::TypeBoardModel:
          return QIcon (":/images/board.png");
        case Node::TypeBoardVariant:
          return QIcon (":/images/board_variant.png");
        case Node::TypeGpio:
          return QIcon (":/images/gpio.png");
        case Node::TypeConnector:
          return QIcon (":/images/connector.png");
        case Node::TypePin:
          return QIcon (":/images/pin.png");
        case Node::TypeSoc:
          return QIcon (":/images/soc.png");
        case Node::TypeFunction:
          return QIcon (":/images/function.png");
        case Node::TypeManufacturer:
          return QIcon (":/images/manufacturer.png");
        default:
          break;
      }
    }
  }
  else if (role == Qt::DisplayRole) {

    Node * node = nodeFromIndex (index);
    if (!node) {

      return QVariant();
    }
    return node->name();
  }

  return QVariant();
}

// ---------------------------------------------------------------------------
QVariant TreeModel::headerData (int section,
                                Qt::Orientation orientation,
                                int role) const {

  if (section == 0 && orientation == Qt::Horizontal) {
    QFileInfo finfo (db.databaseName());

    if (finfo.exists()) {

      if (role == Qt::DisplayRole) {

        return finfo.fileName();
      }
      else if (role == Qt::ToolTipRole) {

        return QString ("%1:/%2").arg (db.driverName().toLower()).arg (finfo.absolutePath());
      }
    }
    else {
      if (role == Qt::DisplayRole) {

        return db.databaseName();
      }
      else if (role == Qt::ToolTipRole) {

        return QString ("%1://%2@%3:%4").arg (db.driverName().toLower()).arg (db.userName()).arg (db.hostName()).arg (db.port());
      }
    }
  }
  return QVariant();
}

// ---------------------------------------------------------------------------
Node *TreeModel::nodeFromIndex (const QModelIndex & index) const {

  if (index.isValid()) {

    return static_cast<Node *> (index.internalPointer());
  }
  else {

    return rootNode;
  }
}
