#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include "property.h"

class BoardModelPrivate;
class BoardModel : public Property {
  public:

    BoardModel (QSqlDatabase & database, QObject * parent = 0);
    virtual ~BoardModel();

    Property & family();
    Soc & soc();

  public slots:
    virtual bool readFromDatabase();
    virtual bool writeToDatabase();

  protected:
    BoardModel (BoardModelPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (BoardModel);
    Q_DISABLE_COPY (BoardModel);
};

class Node;
class BoardModelNodePrivate;
class BoardModelNode : public Node {
  public:

    BoardModelNode (int id, const QString & name, Node * parent);
    virtual ~BoardModelNode();
    virtual void childrenFromDatabase();

  protected:
    BoardModelNode (BoardModelNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (BoardModelNode);
    Q_DISABLE_COPY (BoardModelNode);
};
#endif
