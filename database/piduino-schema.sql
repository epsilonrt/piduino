BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "arch" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "board" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(256),
	"ram"	INTEGER DEFAULT 0,
	"pcb_revision"	VARCHAR(45),
	"board_model_id"	INTEGER NOT NULL,
	"gpio_id"	INTEGER NOT NULL,
	"manufacturer_id"	INTEGER NOT NULL,
	"default_i2c_id"	INTEGER,
	"default_spi_id"	INTEGER,
	"default_uart_id"	INTEGER,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "fk_board_board_model1" FOREIGN KEY("board_model_id") REFERENCES "board_model"("id"),
	CONSTRAINT "fk_board_gpio1" FOREIGN KEY("gpio_id") REFERENCES "gpio"("id"),
	CONSTRAINT "fk_board_manufacturer1" FOREIGN KEY("manufacturer_id") REFERENCES "manufacturer"("id")
);
CREATE TABLE IF NOT EXISTS "board_family" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"i2c_syspath"	VARCHAR(256),
	"spi_syspath"	VARCHAR(256),
	"uart_syspath"	VARCHAR(256),
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "board_model" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"board_family_id"	INTEGER NOT NULL,
	"soc_id"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name"),
	CONSTRAINT "fk_board_model_board_family1" FOREIGN KEY("board_family_id") REFERENCES "board_family"("id"),
	CONSTRAINT "fk_board_model_soc1" FOREIGN KEY("soc_id") REFERENCES "soc"("id")
);
CREATE TABLE IF NOT EXISTS "connector" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"rows"	INTEGER,
	"connector_family_id"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "fk_connector_connector_family" FOREIGN KEY("connector_family_id") REFERENCES "connector_family"("id")
);
CREATE TABLE IF NOT EXISTS "connector_family" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"columns"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "connector_has_pin" (
	"connector_id"	INTEGER NOT NULL,
	"pin_id"	INTEGER NOT NULL,
	"row"	INTEGER NOT NULL,
	"column"	INTEGER NOT NULL,
	CONSTRAINT "fk_connector_has_pin_connector" FOREIGN KEY("connector_id") REFERENCES "connector"("id"),
	CONSTRAINT "fk_connector_has_pin_pin" FOREIGN KEY("pin_id") REFERENCES "pin"("id")
);
CREATE TABLE IF NOT EXISTS "gpio" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"board_family_id"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name"),
	CONSTRAINT "fk_gpio_board_family1" FOREIGN KEY("board_family_id") REFERENCES "board_family"("id")
);
CREATE TABLE IF NOT EXISTS "gpio_has_connector" (
	"num"	INTEGER NOT NULL,
	"gpio_id"	INTEGER NOT NULL,
	"connector_id"	INTEGER NOT NULL,
	CONSTRAINT "fk_gpio_has_connector_connector" FOREIGN KEY("connector_id") REFERENCES "connector"("id"),
	CONSTRAINT "fk_gpio_has_connector_gpio" FOREIGN KEY("gpio_id") REFERENCES "gpio"("id")
);
CREATE TABLE IF NOT EXISTS "manufacturer" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "pin" (
	"id"	INTEGER NOT NULL,
	"pin_type_id"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "fk_pin_pin_type" FOREIGN KEY("pin_type_id") REFERENCES "pin_type"("id")
);
CREATE TABLE IF NOT EXISTS "pin_has_name" (
	"pin_id"	INTEGER NOT NULL,
	"pin_name_id"	INTEGER NOT NULL,
	"pin_mode_id"	INTEGER NOT NULL,
	CONSTRAINT "fk_pin_has_name_pin" FOREIGN KEY("pin_id") REFERENCES "pin"("id"),
	CONSTRAINT "fk_pin_has_name_pin_mode" FOREIGN KEY("pin_mode_id") REFERENCES "pin_mode"("id"),
	CONSTRAINT "fk_pin_has_name_pin_name" FOREIGN KEY("pin_name_id") REFERENCES "pin_name"("id")
);
CREATE TABLE IF NOT EXISTS "pin_mode" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "pin_name" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "pin_number" (
	"pin_id"	INTEGER NOT NULL,
	"logical_num"	INTEGER NOT NULL,
	"mcu_num"	INTEGER NOT NULL,
	"system_num"	INTEGER NOT NULL,
	"chip_num"	INTEGER NOT NULL,
	"chip_offset"	INTEGER NOT NULL,
	PRIMARY KEY("pin_id"),
	CONSTRAINT "fk_soc_pin_number_pin" FOREIGN KEY("pin_id") REFERENCES "pin"("id")
);
CREATE TABLE IF NOT EXISTS "pin_spics" (
	"pin_id"	INTEGER NOT NULL,
	"bus_id"	INTEGER NOT NULL,
	"cs_id"	INTEGER NOT NULL,
	"pin_mode_id"	INTEGER NOT NULL,
	CONSTRAINT "fk_pin_pin_spics" FOREIGN KEY("pin_id") REFERENCES "pin"("id"),
	CONSTRAINT "fk_pin_spics_pin_mode" FOREIGN KEY("pin_mode_id") REFERENCES "pin_mode"("id")
);
CREATE TABLE IF NOT EXISTS "pin_type" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name")
);
CREATE TABLE IF NOT EXISTS "revision" (
	"board_id"	INTEGER NOT NULL,
	"revision"	INTEGER NOT NULL CHECK("revision" >= 0),
	PRIMARY KEY("revision"),
	CONSTRAINT "fk_revision_board1" FOREIGN KEY("board_id") REFERENCES "board"("id")
);
CREATE TABLE IF NOT EXISTS "schema_version" (
	"id"	INTEGER NOT NULL,
	"valid_from"	DATETIME NOT NULL,
	"valid_to"	DATETIME,
	"major"	INTEGER NOT NULL,
	"minor"	INTEGER NOT NULL,
	"revision"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "soc" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"soc_family_id"	INTEGER NOT NULL,
	"manufacturer_id"	INTEGER NOT NULL,
	"i2c_count"	INTEGER NOT NULL DEFAULT 0,
	"spi_count"	INTEGER NOT NULL DEFAULT 0,
	"uart_count"	INTEGER NOT NULL DEFAULT 0,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name"),
	CONSTRAINT "fk_soc_manufacturer1" FOREIGN KEY("manufacturer_id") REFERENCES "manufacturer"("id"),
	CONSTRAINT "fk_soc_soc_family1" FOREIGN KEY("soc_family_id") REFERENCES "soc_family"("id")
);
CREATE TABLE IF NOT EXISTS "soc_family" (
	"id"	INTEGER NOT NULL,
	"name"	VARCHAR(45),
	"arch_id"	INTEGER NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	CONSTRAINT "name_UNIQUE" UNIQUE("name"),
	CONSTRAINT "fk_soc_family_arch1" FOREIGN KEY("arch_id") REFERENCES "arch"("id")
);
CREATE TABLE IF NOT EXISTS "soc_has_pin" (
	"soc_id"	INTEGER NOT NULL,
	"pin_id"	INTEGER NOT NULL,
	CONSTRAINT "fk_soc_has_pin_pin" FOREIGN KEY("pin_id") REFERENCES "pin"("id"),
	CONSTRAINT "fk_soc_has_pin_soc" FOREIGN KEY("soc_id") REFERENCES "soc"("id")
);
CREATE TABLE IF NOT EXISTS "tag" (
	"board_id"	INTEGER NOT NULL,
	"tag"	VARCHAR(45) NOT NULL,
	PRIMARY KEY("tag"),
	CONSTRAINT "fk_tag_board1" FOREIGN KEY("board_id") REFERENCES "board"("id")
);
COMMIT;
