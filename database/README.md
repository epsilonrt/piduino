# piduino-database
  Arduino on Pi boards, the best of both worlds !
  
## Abstract
  Database...

## Installation

## import depuis sql

    sqlite3  < piduino-schema.sql
    sqlite3 piduino.db < piduino-schema.sql

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

    sed -i -e '/^CREATE\ INDEX/d' filename
