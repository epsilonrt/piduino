#ifndef ENUMSTRING_H
#define ENUMSTRING_H

#include <QMap>
#include <QSqlDatabase>

class EnumString : public QMap<int, QString> {

  public:
    EnumString (const QString & table, QSqlDatabase & database);
    QString name(int id) const;
    int id(const QString & name) const;
    QSqlDatabase & database() const;
    QString table() const;

    void updateFromDatabase();
    
  protected:
    QSqlDatabase & m_database;
    QString m_table;
};
#endif
