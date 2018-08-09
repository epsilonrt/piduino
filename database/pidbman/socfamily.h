#ifndef SOCFAMILY_H
#define SOCFAMILY_H

#include "property.h"

class SocFamilyPrivate;
class SocFamily : public Property {
  public:

    SocFamily (QSqlDatabase & database, QObject * parent = 0);
    virtual ~SocFamily();

    Property & arch();

  public slots:
    virtual bool readFromDatabase();

  protected:
    SocFamily (SocFamilyPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (SocFamily);
    Q_DISABLE_COPY (SocFamily);
};

class Node;
class SocFamilyNodePrivate;
class SocFamilyNode : public Node {
  public:

    SocFamilyNode (int id, const QString & name, Node * parent);
    virtual ~SocFamilyNode();
    virtual void childrenFromDatabase();

    SocFamily & record();

  protected:
    SocFamilyNode (SocFamilyNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (SocFamilyNode);
    Q_DISABLE_COPY (SocFamilyNode);
};

#endif
