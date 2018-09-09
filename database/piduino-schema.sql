-- Creator:       MySQL Workbench 6.3.6/ExportSQLite Plugin 0.1.0
-- Author:        epsilonrt
-- Caption:       New Model
-- Project:       Name of the project
-- Changed:       2018-09-09 16:56
-- Created:       2018-04-23 13:23
PRAGMA foreign_keys = OFF;

-- Schema: piduino
ATTACH "piduino.db" AS "piduino";
BEGIN;
CREATE TABLE "piduino"."gpio_connector_family"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "columns" INTEGER NOT NULL,
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."pin_mode"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."pin_name"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."board_family"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "i2c_syspath" VARCHAR(256),
  "spi_syspath" VARCHAR(256),
  "uart_syspath" VARCHAR(256),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."arch"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."manufacturer"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."pin_type"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "piduino"."soc_family"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "arch_id" INTEGER NOT NULL,
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_soc_family_arch1"
    FOREIGN KEY("arch_id")
    REFERENCES "arch"("id")
);
CREATE TABLE "piduino"."pin"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "pin_type_id" INTEGER NOT NULL,
  CONSTRAINT "fk_pin_pin_type1"
    FOREIGN KEY("pin_type_id")
    REFERENCES "pin_type"("id")
);
CREATE TABLE "piduino"."gpio"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "board_family_id" INTEGER NOT NULL,
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_gpio_board_family1"
    FOREIGN KEY("board_family_id")
    REFERENCES "board_family"("id")
);
CREATE TABLE "piduino"."pin_has_name"(
  "pin_id" INTEGER NOT NULL,
  "pin_name_id" INTEGER NOT NULL,
  "pin_mode_id" INTEGER NOT NULL,
  CONSTRAINT "fk_pin_has_name_pin1"
    FOREIGN KEY("pin_id")
    REFERENCES "pin"("id"),
  CONSTRAINT "fk_pin_has_name_pin_name1"
    FOREIGN KEY("pin_name_id")
    REFERENCES "pin_name"("id"),
  CONSTRAINT "fk_pin_has_name_pin_mode1"
    FOREIGN KEY("pin_mode_id")
    REFERENCES "pin_mode"("id")
);
CREATE TABLE "piduino"."gpio_connector"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "rows" INTEGER,
  "gpio_connector_family_id" INTEGER NOT NULL,
  CONSTRAINT "fk_gpio_connector_gpio_connector_family"
    FOREIGN KEY("gpio_connector_family_id")
    REFERENCES "gpio_connector_family"("id")
);
CREATE TABLE "piduino"."pin_number"(
  "pin_id" INTEGER PRIMARY KEY NOT NULL,
  "logical_num" INTEGER NOT NULL,
  "mcu_num" INTEGER NOT NULL,
  "system_num" INTEGER NOT NULL,
  CONSTRAINT "fk_soc_pin_number_pin1"
    FOREIGN KEY("pin_id")
    REFERENCES "pin"("id")
);
CREATE TABLE "piduino"."gpio_has_connector"(
  "num" INTEGER NOT NULL,
  "gpio_id" INTEGER NOT NULL,
  "gpio_connector_id" INTEGER NOT NULL,
  CONSTRAINT "fk_gpio_has_connector_gpio1"
    FOREIGN KEY("gpio_id")
    REFERENCES "gpio"("id"),
  CONSTRAINT "fk_gpio_has_connector_gpio_connector1"
    FOREIGN KEY("gpio_connector_id")
    REFERENCES "gpio_connector"("id")
);
CREATE TABLE "piduino"."pin_spics"(
  "pin_id" INTEGER NOT NULL,
  "bus_id" INTEGER NOT NULL,
  "cs_id" INTEGER NOT NULL,
  "pin_mode_id" INTEGER NOT NULL,
  CONSTRAINT "fk_pin_spi_pin1"
    FOREIGN KEY("pin_id")
    REFERENCES "pin"("id"),
  CONSTRAINT "fk_pin_spics_pin_mode1"
    FOREIGN KEY("pin_mode_id")
    REFERENCES "pin_mode"("id")
);
CREATE TABLE "piduino"."gpio_connector_has_pin"(
  "gpio_connector_id" INTEGER NOT NULL,
  "pin_id" INTEGER NOT NULL,
  "row" INTEGER NOT NULL,
  "column" INTEGER NOT NULL,
  CONSTRAINT "fk_gpio_connector_has_pin_gpio_connector1"
    FOREIGN KEY("gpio_connector_id")
    REFERENCES "gpio_connector"("id"),
  CONSTRAINT "fk_gpio_connector_has_pin_pin1"
    FOREIGN KEY("pin_id")
    REFERENCES "pin"("id")
);
CREATE TABLE "piduino"."soc"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "soc_family_id" INTEGER NOT NULL,
  "manufacturer_id" INTEGER NOT NULL,
  "i2c_count" INTEGER NOT NULL DEFAULT 0,
  "spi_count" INTEGER NOT NULL DEFAULT 0,
  "uart_count" INTEGER NOT NULL DEFAULT 0,
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_soc_soc_family1"
    FOREIGN KEY("soc_family_id")
    REFERENCES "soc_family"("id"),
  CONSTRAINT "fk_soc_manufacturer1"
    FOREIGN KEY("manufacturer_id")
    REFERENCES "manufacturer"("id")
);
CREATE TABLE "piduino"."tag"(
  "board_id" INTEGER NOT NULL,
  "tag" VARCHAR(45) PRIMARY KEY NOT NULL,
  CONSTRAINT "fk_tag_board1"
    FOREIGN KEY("board_id")
    REFERENCES "board"("id")
);
CREATE TABLE "piduino"."soc_has_pin"(
  "soc_id" INTEGER NOT NULL,
  "pin_id" INTEGER NOT NULL,
  CONSTRAINT "fk_soc_has_pin_soc1"
    FOREIGN KEY("soc_id")
    REFERENCES "soc"("id"),
  CONSTRAINT "fk_soc_has_pin_pin1"
    FOREIGN KEY("pin_id")
    REFERENCES "pin"("id")
);
CREATE TABLE "piduino"."board"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "ram" INTEGER DEFAULT 0,
  "pcb_revision" VARCHAR(45),
  "board_model_id" INTEGER NOT NULL,
  "gpio_id" INTEGER NOT NULL,
  "manufacturer_id" INTEGER NOT NULL,
  "default_i2c_id" INTEGER,
  "default_spi_id" INTEGER,
  "default_uart_id" INTEGER,
  CONSTRAINT "fk_board_board_model1"
    FOREIGN KEY("board_model_id")
    REFERENCES "board_model"("id"),
  CONSTRAINT "fk_board_gpio1"
    FOREIGN KEY("gpio_id")
    REFERENCES "gpio"("id"),
  CONSTRAINT "fk_board_manufacturer1"
    FOREIGN KEY("manufacturer_id")
    REFERENCES "manufacturer"("id")
);
CREATE TABLE "piduino"."revision"(
  "board_id" INTEGER NOT NULL,
  "revision" INTEGER PRIMARY KEY NOT NULL CHECK("revision">=0),
  CONSTRAINT "fk_revision_board1"
    FOREIGN KEY("board_id")
    REFERENCES "board"("id")
);
CREATE TABLE "piduino"."board_model"(
  "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" VARCHAR(45),
  "board_family_id" INTEGER NOT NULL,
  "soc_id" INTEGER NOT NULL,
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_board_model_board_family1"
    FOREIGN KEY("board_family_id")
    REFERENCES "board_family"("id"),
  CONSTRAINT "fk_board_model_soc1"
    FOREIGN KEY("soc_id")
    REFERENCES "soc"("id")
);
COMMIT;
