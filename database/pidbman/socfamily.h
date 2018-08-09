#ifndef SOCFAMILY_H
#define SOCFAMILY_H

#include "property.h"

class QString;
class Node;
class SocFamilyNodePrivate;
class SocFamilyNode : public Node {
  public:

    SocFamilyNode (int id, const QString & name, Node * parent);
    virtual ~SocFamilyNode();
    virtual void childrenFromDatabase();

    Property & arch();

  protected:
    SocFamilyNode (SocFamilyNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (SocFamilyNode);
    Q_DISABLE_COPY (SocFamilyNode);
};

#endif
