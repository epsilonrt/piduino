#ifndef DATABASE_H
#define DATABASE_H

#include "node.h"

class QSqlDatabase;
class QObject;
class Folder;
class DatabasePrivate;
class Database : public Node {
  public:

    enum SubFolder {
      FolderBoard = 0,
      FolderGpio,
      FolderSoc,
      FolderManufacturer
    };
    
    Database (QSqlDatabase & db, QObject * parent = 0);
    virtual ~Database();
    virtual void childrenFromDatabase();

    virtual QSqlDatabase  & database() const;
    Folder * folder (SubFolder f);
    
  protected:
    Database (DatabasePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (Database);
    Q_DISABLE_COPY (Database);
};
#endif
