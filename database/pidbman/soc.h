#ifndef SOC_H
#define SOC_H

#include "property.h"

class QString;
class Node;
class SocNodePrivate;
class SocNode : public Node {
  public:

    SocNode (int id, const QString & name, Node * parent);
    virtual ~SocNode();
    virtual void childrenFromDatabase();

    int familyId() const;
    Property & manufacturer();
    
  protected:
    SocNode (SocNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (SocNode);
    Q_DISABLE_COPY (SocNode);
};

#endif
