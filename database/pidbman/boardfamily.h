#ifndef BOARDFAMILY_H
#define BOARDFAMILY_H

class QString;
class Node;
class BoardFamilyNode : public Node {
  public:

    BoardFamilyNode (int id, const QString & name, Node * parent);
    virtual void childrenFromDatabase();
};
#endif
