/* Copyright © 2020 Pascal JEAN, All rights reserved.
 *
 * Piduino pidbm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Piduino pidbm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Piduino pidbm.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <cppdb/frontend.h>
#include "pidbm.h"
#include <iostream>
#include <vector>

class Pidbm::Private {

  public:
    Private (Pidbm * q);
    Private (Pidbm * q, int argc, char **argv);
    virtual ~Private();
    bool findConnectionInfo ();
    
    void list();
    void add();
    void mod();
    void remove();
    void show();
    void copy();

    long long printRecordEqual (const std::vector<std::string> & what,
                                const std::string & from,
                                const std::string & where = std::string(),
                                const std::string & condition = std::string(),
                                bool like = false,
                                const std::string & orderby = std::string(),
                                const std::string & groupby = std::string());
    long long printRecord (const std::vector<std::string> & what,
                           const std::string & from,
                           const std::string & where = std::string(),
                           const std::vector<std::string> & condition = std::vector<std::string>(),
                           const std::string & orderby = std::string(),
                           const std::string & groupby = std::string());
    long long selectRecordEqual (cppdb::result & res,
                                 const std::vector<std::string> & what,
                                 const std::string & from,
                                 const std::string & where = std::string(),
                                 const std::string & condition = std::string(),
                                 bool like = false,
                                 const std::string & orderby = std::string(),
                                 const std::string & groupby = std::string());
    template <class T>
    long long selectRecord (cppdb::result & res,
                            const std::vector<std::string> & what,
                            const std::string & from,
                            const std::string & where = std::string(),
                            const std::vector<T> & condition = std::vector<T>(),
                            const std::string & orderby = std::string(),
                            const std::string & groupby = std::string());
    // if ifNotExists set, first 'what' must be the id field to returned
    template <class T>
    long long insertRecord (const std::vector<std::string> & what,
                            const std::string & to,
                            const std::vector<T> & values,
                            bool ifNotExists = false);
    void updateRecord (const std::vector<std::string> & what,
                       const std::string & to,
                       const std::string & where,
                       const std::vector<std::string> & condition,
                       const std::vector<std::string> & values);
    void deleteRecord (const std::vector<std::string> & what,
                       const std::string & from,
                       const std::string & where,
                       const std::string & condition,
                       bool like = false);
    void setWhereCondition (size_t pos, std::string & where,
                            std::string & condition, bool & like);
    bool readArg (size_t pos, const std::string & from, std::string & id, bool caseInsensitive = false);
    bool readArg (size_t pos, const std::string & from, long long & id, bool caseInsensitive = false);
    long long nameExists (const std::string & from, const std::string & name, bool caseInsensitive = false);
    bool idExists (const std::string & from, const std::string & id);
    bool idExists (const std::string & from, const long long & id);
    static void printTable (cppdb::result & res, std::vector<int> & colen,
                            const std::vector<std::string> & what);
    static void printTableHeader (cppdb::result & res, std::vector<int> & colen,
                                  const std::vector<std::string> & what);
    static void printTableLine (std::vector<int> & colen);
    static std::string columnNameCleanup (const std::string & name);

    Pidbm * const q_ptr;

    Popl::OptionParser op;
    bool opHelp;
    bool opWarranty;
    bool opVersion;
    bool opQuiet;
    std::shared_ptr<Popl::Value<std::string>> opRevision;
    std::shared_ptr<Popl::Value<std::string>> opMemory;
    std::shared_ptr<Popl::Value<std::string>> opTag;
    std::shared_ptr<Popl::Value<std::string>> opPCB;

    std::string cinfo;
    mutable cppdb::session db;

    static const std::string Authors;
    static const std::string Website;
    static const std::string Description;
    static const  std::map<std::string, std::vector<std::string>> WhatMap;
    PIMP_DECLARE_PUBLIC (Pidbm)
};

// -----------------------------------------------------------------------------
template <class T>
long long Pidbm::Private::insertRecord (const std::vector<std::string> & what,
                                        const std::string & to,
                                        const std::vector<T> & values,
                                        bool ifNotExists) {
  cppdb::statement st;
  cppdb::result res;
  std::ostringstream req;
  std::string where;
  std::vector<std::string> what_cleaned;
  long long id = -1;

  for (size_t i = 0; i < what.size(); i++) {
    std::string w = what[i];

    size_t pos = w.rfind ('.');
    if (pos != std::string::npos) {

      w = what[i].substr (pos + 1);
    }
    if (ifNotExists) {
      where += w + "=?";
      if (what.size() > 1 && i < (what.size() - 1)) {

        where += " AND ";
      }
    }
    what_cleaned.push_back (w);
  }

  if (ifNotExists) {

    selectRecord (res, what_cleaned, to, where, values);
    if (res.next()) {
      res >> id;
    }
  }

  if (id < 0) {
    req << "INSERT INTO " << to << "(";
    for (size_t i = 0; i < what_cleaned.size(); i++) {

      req << what_cleaned[i];
      if (i < (what_cleaned.size() - 1)) {
        req << ',';
      }
    }
    req << ") VALUES(";
    for (size_t i = 0; i < what_cleaned.size(); i++) {
      req << '?';
      if (i < (what_cleaned.size() - 1)) {
        req << ',';
      }
    }
    req << ')';
    std::cout << req.str() << std::endl; // debug

    st = db << req.str();
    for (auto v : values) {
      st << v;
    }
    st.exec();

    id = st.last_insert_id();
    if (!opQuiet) {

      std::cout << "New " << to << " (id:" <<  id << ") added." << std::endl;
    }
  }
  else {

    if (!opQuiet) {

      std::cout << "this record is already in the " << to << " table, nothing to add." <<  std::endl;
    }
  }
  return id;
}


// -----------------------------------------------------------------------------
template <class T>
long long Pidbm::Private::selectRecord (cppdb::result & res,
                                        const std::vector<std::string> & what,
                                        const std::string & from,
                                        const std::string & where,
                                        const std::vector<T> & condition,
                                        const std::string & orderby,
                                        const std::string & groupby) {
  long long n = 0;
  std::ostringstream req;
  cppdb::statement st;

  // query to count the number of lines
  req << "SELECT COUNT(*) FROM " << from;

  if (where.size() && condition.size()) {

    req << " WHERE " << where;
    st = db << req.str();
    for (auto c : condition) {
      st << c;
    }
  }
  else {
    st = db << req.str();
  }
  //std::cout << req.str() << std::endl; // debug
  res = st.row();

  if (!res.empty()) {

    res >> n;
    if (n > 0) {

      // query to be performed the result
      req.str ("");
      req.clear();

      req << "SELECT ";
      for (size_t i = 0; i < what.size(); i++) {
        std::string str (what[i]);

        if (str[0] == '%') {
          str.erase (0, 1);
          str = "printf(\"0x%x\"," + str + ")";
        }
        req << str;
        if (i < (what.size() - 1)) {
          req << ',';
        }
      }
      req << " FROM " << from;
      if (where.size() && condition.size()) {
        req << " WHERE " << where;
      }
      if (groupby.size()) {
        req << " GROUP BY " << groupby;
      }
      if (orderby.size()) {
        req << " ORDER BY " << orderby;
      }

      //std::cout << req.str() << std::endl; // debug

      st = db << req.str();
      if (where.size() && condition.size()) {

        for (auto c : condition) {
          st << c;
        }
      }
      res = st.query();
    }
  }
  return n;
}

/* ========================================================================== */
