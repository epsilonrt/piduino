#ifndef NODE_H
#define NODE_H

#include <QList>
#include <QObject>

class QSqlDatabase;
class QString;
class Database;
class NodePrivate;
class Node : public QObject {
    Q_OBJECT
  public:
    enum Type { TypeDatabase,
                TypeBoardFamily, TypeBoardModel, TypeBoardVariant,
                TypeGpio, TypeConnector,
                TypePin, TypeFunction,
                TypeSoc, TypeSocFamily,
                TypeManufacturer
              };

    Node (Type type, Node * parent = 0);
    Node (Type type, QObject * parent = 0);
    virtual ~Node();

    Type type() const;

    int id() const;
    void setId (int id);

    virtual QString name() const;
    virtual void setName (const QString & name);
    virtual QString toolTip() const;

    Node * parent() const;
    void setParent (Node * parent);

    const QList<Node *> & children() const;
    void appendChild (Node * child);

    bool isFolder() const;

    Database * root() const;
    virtual QSqlDatabase & database() const;

  public slots:
    virtual void childrenFromDatabase();
    void clearChildren();
    virtual void updateToDatabase() const;
    virtual void insertToDatabase() const;
    virtual void deleteFromDatabase() const;

  protected:
    Node (NodePrivate &dd);
    void setDatabase (QSqlDatabase * db = 0);
    void setFolder (bool isFolder);

    const QScopedPointer<NodePrivate> d_ptr;

  private:
    Q_DECLARE_PRIVATE (Node);
    Q_DISABLE_COPY (Node);
};
#endif
