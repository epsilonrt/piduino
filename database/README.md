# piduino-database
  Arduino on Pi boards, the best of both worlds !
  
## Abstract
  Database...

## Installation

## import depuis sql

    sqlite3  < piduino-schema.sql
    sqlite3 piduino.db < piduino-data.sql

## export schema vers sql

    .output schema.sql
    .schema

## export data vers sql

    echo "BEGIN TRANSACTION;" >  piduino-data.sql; sqlite3 piduino.db .dump | grep '^INSERT INTO' >> piduino-data.sql; echo "COMMIT;" >>  piduino-data.sql

## export vers sql de données

    .mode insert
    .output data.sql
    select * from board;

## export vers csv de données

    .mode csv
    .output data.csv
    select * from board;

## export depuis MySQL Workbench

https://github.com/tatsushid/mysql-wb-exportsqlite

puis

    sed -i -e '/^CREATE\ INDEX/d' piduino-schema.sql
    sed -i -e 's/piduino\.sdb/piduino\.db/g' piduino-schema.sql

ExportSQLite is a plugin for
[MySQL Workbench](http://www.mysql.com/products/workbench/) to export a schema
catalog as SQLite's CREATE SQL queries. This is a Python port of
[original Lua script](https://gist.github.com/mrprompt/7252060) for using with
MySQL Workbench 6.2 or above.

### Installation

1. Open MySQL Workbench
2. Choose "Install Plugin/Module..." in "Scripting" menu
3. Open `export_sqlite_grt.py` file
4. Restart MySQL Workbench

### Usage

Once you open a database model in MySQL Workbench, you can see "Export SQLite
CREATE script" in "Tools > Catalog" menu. Just choose it.
