#ifndef FOLDER_H
#define FOLDER_H

class QString;
class Node;
class Folder : public Node {
  public:

    Folder (Type type, const QString & name, Node * parent);
    virtual void childrenFromDatabase();
};
#endif
