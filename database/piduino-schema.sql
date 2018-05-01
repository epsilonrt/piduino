BEGIN TRANSACTION;
CREATE TABLE "soc_family"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "arch_id" INTEGER NOT NULL,
  PRIMARY KEY("id","arch_id"),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_soc_family_arch"
    FOREIGN KEY("arch_id")
    REFERENCES "arch"("id")
);
CREATE TABLE "soc"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "soc_family_id" INTEGER NOT NULL,
  "manufacturer_id" INTEGER NOT NULL,
  PRIMARY KEY("id","soc_family_id","manufacturer_id"),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_soc_soc_family"
    FOREIGN KEY("soc_family_id")
    REFERENCES "soc_family"("id"),
  CONSTRAINT "fk_soc_manufacturer"
    FOREIGN KEY("manufacturer_id")
    REFERENCES "manufacturer"("id")
);
CREATE TABLE "manufacturer"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "gpio_pin_type"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "gpio_pin_name"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "gpio_pin_mode"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "gpio_pin_has_name"(
  "gpio_pin_id" INTEGER NOT NULL,
  "gpio_pin_name_id" INTEGER NOT NULL,
  "gpio_pin_mode_id" INTEGER NOT NULL,
  PRIMARY KEY("gpio_pin_id","gpio_pin_name_id","gpio_pin_mode_id"),
  CONSTRAINT "fk_gpio_pin_has_gpio_pin_name_gpio_pin"
    FOREIGN KEY("gpio_pin_id")
    REFERENCES "gpio_pin"("id"),
  CONSTRAINT "fk_gpio_pin_has_gpio_pin_name_gpio_pin_name"
    FOREIGN KEY("gpio_pin_name_id")
    REFERENCES "gpio_pin_name"("id"),
  CONSTRAINT "fk_gpio_pin_has_name_gpio_pin_mode1"
    FOREIGN KEY("gpio_pin_mode_id")
    REFERENCES "gpio_pin_mode"("id")
);
CREATE TABLE "gpio_pin"(
  "id" INTEGER NOT NULL,
  "gpio_pin_type_id" INTEGER NOT NULL,
  "logical_num" INTEGER,
  "mcu_num" INTEGER,
  "system_num" INTEGER,
  PRIMARY KEY("id","gpio_pin_type_id"),
  CONSTRAINT "fk_gpio_pin_gpio_pin_type"
    FOREIGN KEY("gpio_pin_type_id")
    REFERENCES "gpio_pin_type"("id")
);
CREATE TABLE "gpio_has_connector"(
  "num" INTEGER,
  "gpio_id" INTEGER NOT NULL,
  "gpio_connector_id" INTEGER NOT NULL,
  PRIMARY KEY("gpio_id","gpio_connector_id"),
  CONSTRAINT "fk_gpio_has_gpio_connector_gpio"
    FOREIGN KEY("gpio_id")
    REFERENCES "gpio"("id"),
  CONSTRAINT "fk_gpio_has_gpio_connector_gpio_connector"
    FOREIGN KEY("gpio_connector_id")
    REFERENCES "gpio_connector"("id")
);
CREATE TABLE "gpio_connector_has_pin"(
  "gpio_connector_id" INTEGER NOT NULL,
  "gpio_pin_id" INTEGER NOT NULL,
  "row" INTEGER,
  "column" INTEGER,
  PRIMARY KEY("gpio_connector_id","gpio_pin_id"),
  CONSTRAINT "fk_gpio_connector_has_gpio_pin_gpio_connector"
    FOREIGN KEY("gpio_connector_id")
    REFERENCES "gpio_connector"("id"),
  CONSTRAINT "fk_gpio_connector_has_gpio_pin_gpio_pin"
    FOREIGN KEY("gpio_pin_id")
    REFERENCES "gpio_pin"("id")
);
CREATE TABLE "gpio_connector_family"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  "columns" INTEGER,
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "gpio_connector"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "rows" INTEGER,
  "gpio_connector_family_id" INTEGER NOT NULL,
  PRIMARY KEY("id","gpio_connector_family_id"),
  CONSTRAINT "fk_gpio_connector_gpio_connector_family"
    FOREIGN KEY("gpio_connector_family_id")
    REFERENCES "gpio_connector_family"("id")
);
CREATE TABLE "gpio"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45)
);
CREATE TABLE "board_model"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "board_family_id" INTEGER NOT NULL,
  "soc_id" INTEGER NOT NULL,
  PRIMARY KEY("id","board_family_id","soc_id"),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name"),
  CONSTRAINT "fk_board_model_board_family"
    FOREIGN KEY("board_family_id")
    REFERENCES "board_family"("id"),
  CONSTRAINT "fk_board_model_soc"
    FOREIGN KEY("soc_id")
    REFERENCES "soc"("id")
);
CREATE TABLE "board_family"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
CREATE TABLE "board"(
  "id" INTEGER NOT NULL,
  "tag" VARCHAR(45),
  "revision" INTEGER CHECK("revision">=0),
  "ram" INTEGER DEFAULT 0,
  "pcb_revision" DECIMAL,
  "board_model_id" INTEGER NOT NULL,
  "gpio_id" INTEGER NOT NULL,
  "manufacturer_id" INTEGER NOT NULL,
  PRIMARY KEY("id","board_model_id","gpio_id","manufacturer_id"),
  CONSTRAINT "revision_UNIQUE"
    UNIQUE("revision"),
  CONSTRAINT "tag_UNIQUE"
    UNIQUE("tag"),
  CONSTRAINT "fk_board_manufacturer"
    FOREIGN KEY("manufacturer_id")
    REFERENCES "manufacturer"("id"),
  CONSTRAINT "fk_board_gpio"
    FOREIGN KEY("gpio_id")
    REFERENCES "gpio"("id"),
  CONSTRAINT "fk_board_board_model"
    FOREIGN KEY("board_model_id")
    REFERENCES "board_model"("id")
);
CREATE TABLE "arch"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45),
  CONSTRAINT "name_UNIQUE"
    UNIQUE("name")
);
COMMIT;
