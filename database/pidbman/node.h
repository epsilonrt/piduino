#ifndef NODE_H
#define NODE_H

#include <QList>
#include <QString>
#include <QSqlDatabase>
#include <QObject>

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

    virtual const QString & name() const;
    virtual void setName (const QString & name);

    Node * parent() const;
    Node * root();
    void setParent (Node * parent);

    const QList<Node *> & children() const;
    void appendChild (Node * child);

    bool isFolder() const;

    QSqlDatabase  & database() const;

  public slots:
    virtual void childrenFromDatabase() = 0;
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


class Database : public Node {
  public:

    Database (QSqlDatabase & db, QObject * parent = 0);
    virtual void childrenFromDatabase();
};

class Folder : public Node {
  public:

    Folder (Type type, const QString & name, Node * parent);
    virtual void childrenFromDatabase();
};

class BoardFamily : public Node {
  public:

    BoardFamily (int id, const QString & name, Node * parent);
    virtual void childrenFromDatabase();
};

class BoardModelPrivate;
class BoardModel : public Node {
  public:

    BoardModel (int id, const QString & name, Node * parent);
    virtual ~BoardModel();
    virtual void childrenFromDatabase();

    int familyId() const;

    int socId() const;
    void setSocId (int soc);

  protected:
    BoardModel (BoardModelPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (BoardModel);
    Q_DISABLE_COPY (BoardModel);
};

class BoardVariantPrivate;
class BoardVariant : public Node {
  public:

    BoardVariant (int id, const QString & name, Node * parent);
    virtual ~BoardVariant();
    virtual void childrenFromDatabase();

    int modelId() const;

    const QString & tag() const;
    void setTag (const QString & str);
    int revision() const;
    void setRevision (int value);
    int ram() const;
    void setRam (int value);
    const QString & pcbRevision() const;
    void setPcbRevision (const QString & str);
    int gpioId() const;
    void setGpioId (int value);
    int manufacturerId() const;
    void setManufacturerId (int value);

  protected:
    BoardVariant (BoardVariantPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (BoardVariant);
    Q_DISABLE_COPY (BoardVariant);
};

class Gpio : public Node {
  public:

    Gpio (int id, const QString & name, Node * parent);
    virtual void childrenFromDatabase();

    int variantId() const;
};

class ConnectorPrivate;
class Connector : public Node {
  public:

    Connector (int id, const QString & name, Node * parent);
    virtual ~Connector();
    virtual void childrenFromDatabase();

    int gpioId() const;

  protected:
    Connector (ConnectorPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (Connector);
    Q_DISABLE_COPY (Connector);
};

class PinPrivate;
class Pin : public Node {
  public:

    Pin (int id, const QString & name, Node * parent);
    virtual ~Pin();
    virtual void childrenFromDatabase();

    int connectorId() const;

  protected:
    Pin (PinPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (Pin);
    Q_DISABLE_COPY (Pin);
};

class FunctionPrivate;
class Function : public Node {
  public:

    Function (int id, const QString & name, Node * parent);
    virtual ~Function();
    virtual void childrenFromDatabase();

    int pinId() const;

  protected:
    Function (FunctionPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (Function);
    Q_DISABLE_COPY (Function);
};

class SocFamilyPrivate;
class SocFamily : public Node {
  public:

    SocFamily (int id, const QString & name, Node * parent);
    virtual ~SocFamily();
    virtual void childrenFromDatabase();

    int archId() const;
    void setArchId (int value);
    QString archName() const;

  protected:
    SocFamily (SocFamilyPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (SocFamily);
    Q_DISABLE_COPY (SocFamily);
};

class SocPrivate;
class Soc : public Node {
  public:

    Soc (int id, const QString & name, Node * parent);
    virtual ~Soc();
    virtual void childrenFromDatabase();

    int familyId() const;
    int manufacturerId() const;
    void setManufacturerId (int value);

  protected:
    Soc (SocPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (Soc);
    Q_DISABLE_COPY (Soc);
};

class ManufacturerPrivate;
class Manufacturer : public Node {
  public:

    Manufacturer (int id, const QString & name, Node * parent);
    virtual ~Manufacturer();
    virtual void childrenFromDatabase();

  protected:
    Manufacturer (ManufacturerPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (Manufacturer);
    Q_DISABLE_COPY (Manufacturer);
};
#endif
