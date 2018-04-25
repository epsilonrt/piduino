-- Creator:       MySQL Workbench 6.3.6/ExportSQLite Plugin 0.1.0
-- Author:        epsilonrt
-- Caption:       New Model
-- Project:       Name of the project
-- Changed:       2018-04-24 18:07
-- Created:       2018-04-23 13:23
PRAGMA foreign_keys = OFF;

-- Schema: piduino
ATTACH "piduino.sdb" AS "piduino";
BEGIN;
CREATE TABLE "piduino"."board_family"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45)
);
INSERT INTO "board_family"("id","name") VALUES(0, 'RaspberryPi');
INSERT INTO "board_family"("id","name") VALUES(1, 'NanoPi');
INSERT INTO "board_family"("id","name") VALUES(2, 'OrangePi');
INSERT INTO "board_family"("id","name") VALUES(3, 'BananaPi');
INSERT INTO "board_family"("id","name") VALUES(-1, 'Unknown');
CREATE TABLE "piduino"."gpio"(
  "id" INTEGER PRIMARY KEY NOT NULL
);
INSERT INTO "gpio"("id") VALUES(1);
INSERT INTO "gpio"("id") VALUES(2);
INSERT INTO "gpio"("id") VALUES(3);
INSERT INTO "gpio"("id") VALUES(4);
INSERT INTO "gpio"("id") VALUES(5);
INSERT INTO "gpio"("id") VALUES(6);
CREATE TABLE "piduino"."arch"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45)
);
INSERT INTO "arch"("id","name") VALUES(0, 'ARM');
INSERT INTO "arch"("id","name") VALUES(-1, 'Unknown');
CREATE TABLE "piduino"."manufacturer"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "name" VARCHAR(45)
);
INSERT INTO "manufacturer"("id","name") VALUES(0, 'Broadcom');
INSERT INTO "manufacturer"("id","name") VALUES(1, 'Sony');
INSERT INTO "manufacturer"("id","name") VALUES(2, 'Qisda');
INSERT INTO "manufacturer"("id","name") VALUES(3, 'Egoman');
INSERT INTO "manufacturer"("id","name") VALUES(4, 'Embest');
INSERT INTO "manufacturer"("id","name") VALUES(5, 'Allwinner');
INSERT INTO "manufacturer"("id","name") VALUES(6, 'Friendly ARM');
INSERT INTO "manufacturer"("id","name") VALUES(-1, 'Unknown');
CREATE TABLE "piduino"."soc_family"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "arch_id" INTEGER NOT NULL,
  PRIMARY KEY("id","arch_id"),
  CONSTRAINT "fk_soc_family_arch1"
    FOREIGN KEY("arch_id")
    REFERENCES "arch"("id")
);
CREATE INDEX "piduino"."soc_family.fk_soc_family_arch1_idx" ON "soc_family" ("arch_id");
INSERT INTO "soc_family"("id","name","arch_id") VALUES(0, 'BroadcomBcm2835', 0);
INSERT INTO "soc_family"("id","name","arch_id") VALUES(1, 'AllwinnerH', 0);
INSERT INTO "soc_family"("id","name","arch_id") VALUES(-1, 'Unknown', -1);
CREATE TABLE "piduino"."soc"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "soc_family_id" INTEGER NOT NULL,
  "manufacturer_id" INTEGER NOT NULL,
  PRIMARY KEY("id","soc_family_id","manufacturer_id"),
  CONSTRAINT "fk_soc_soc_family1"
    FOREIGN KEY("soc_family_id")
    REFERENCES "soc_family"("id"),
  CONSTRAINT "fk_soc_manufacturer1"
    FOREIGN KEY("manufacturer_id")
    REFERENCES "manufacturer"("id")
);
CREATE INDEX "piduino"."soc.fk_soc_soc_family1_idx" ON "soc" ("soc_family_id");
CREATE INDEX "piduino"."soc.fk_soc_manufacturer1_idx" ON "soc" ("manufacturer_id");
INSERT INTO "soc"("id","name","soc_family_id","manufacturer_id") VALUES(0, 'Bcm2708', 0, 0);
INSERT INTO "soc"("id","name","soc_family_id","manufacturer_id") VALUES(1, 'Bcm2709', 0, 0);
INSERT INTO "soc"("id","name","soc_family_id","manufacturer_id") VALUES(2, 'Bcm2710', 0, 0);
INSERT INTO "soc"("id","name","soc_family_id","manufacturer_id") VALUES(3, 'H3', 1, 5);
INSERT INTO "soc"("id","name","soc_family_id","manufacturer_id") VALUES(4, 'H5', 1, 5);
INSERT INTO "soc"("id","name","soc_family_id","manufacturer_id") VALUES(-1, 'Unknown', -1, -1);
CREATE TABLE "piduino"."board"(
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
  CONSTRAINT "fk_board_manufacturer1"
    FOREIGN KEY("manufacturer_id")
    REFERENCES "manufacturer"("id"),
  CONSTRAINT "fk_board_gpio1"
    FOREIGN KEY("gpio_id")
    REFERENCES "gpio"("id"),
  CONSTRAINT "fk_board_board_model1"
    FOREIGN KEY("board_model_id")
    REFERENCES "board_model"("id")
);
CREATE INDEX "piduino"."board.fk_board_manufacturer1_idx" ON "board" ("manufacturer_id");
CREATE INDEX "piduino"."board.fk_board_gpio1_idx" ON "board" ("gpio_id");
CREATE INDEX "piduino"."board.fk_board_board_model1_idx" ON "board" ("board_model_id");
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(0, NULL, 0x0002, 256, 1.0, 1, 1, -1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(1, NULL, 0x0003, 256, 1.0, 1, 1, -1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(2, NULL, 0x0004, 256, 2.0, 1, 2, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(3, NULL, 0x0005, 256, 2.0, 1, 2, 2);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(4, NULL, 0x0006, 256, 2.0, 1, 2, 3);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(5, NULL, 0x0007, 256, 2.0, 0, 2, 3);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(6, NULL, 0x0008, 256, 2.0, 0, 2, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(7, NULL, 0x0009, 256, 2.0, 0, 2, 2);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(8, NULL, 0x000d, 512, 2.0, 1, 2, 3);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(9, NULL, 0x000e, 512, 2.0, 1, 2, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(10, NULL, 0x000f, 512, 2.0, 1, 2, 2);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(11, NULL, 0x0010, 512, 1.0, 3, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(12, NULL, 0x0011, 512, 1.0, 4, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(13, NULL, 0x0012, 256, 1.0, 2, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(14, NULL, 0x0013, 512, 1.2, 3, 3, -1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(15, NULL, 0x0014, 512, 1.0, 4, 3, 4);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(16, NULL, 0x0015, 512, 1.1, 2, 3, 4);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(17, NULL, 0x900021, 512, 1.1, 2, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(18, NULL, 0x900032, 512, 1.2, 3, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(19, NULL, 0xa01040, 1024, 1.0, 5, 3, -1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(20, NULL, 0xa01041, 1024, 1.1, 5, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(21, NULL, 0xa21041, 1024, 1.1, 5, 3, 4);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(22, NULL, 0xa22042, 1024, 1.2, 5, 3, 4);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(23, NULL, 0x900092, 512, 1.2, 6, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(24, NULL, 0x900093, 512, 1.3, 6, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(25, NULL, 0x920093, 512, 1.3, 6, 3, 4);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(26, NULL, 0x9000c1, 512, 1.1, 8, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(27, NULL, 0xa02082, 1024, 1.2, 7, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(28, NULL, 0xa020a0, 1024, 1.0, 9, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(29, NULL, 0xa22082, 1024, 1.2, 7, 3, -1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(30, NULL, 0xa32082, 1024, 1.2, 7, 3, 1);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(31, 'nanopineo', NULL, NULL, NULL, 10, 4, 6);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(32, 'nanopineoair', NULL, NULL, NULL, 11, 4, 6);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(33, 'nanopim1', NULL, NULL, NULL, 12, 5, 6);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(34, 'nanopim1plus', NULL, NULL, NULL, 13, 6, 6);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(35, 'nanopineo2', NULL, NULL, NULL, 14, 4, 6);
INSERT INTO "board"("id","tag","revision","ram","pcb_revision","board_model_id","gpio_id","manufacturer_id") VALUES(36, 'nanopineoplus2', NULL, NULL, NULL, 15, 4, 6);
CREATE TABLE "piduino"."board_model"(
  "id" INTEGER NOT NULL,
  "name" VARCHAR(45),
  "board_family_id" INTEGER NOT NULL,
  "soc_id" INTEGER NOT NULL,
  PRIMARY KEY("id","board_family_id","soc_id"),
  CONSTRAINT "fk_board_model_board_family1"
    FOREIGN KEY("board_family_id")
    REFERENCES "board_family"("id"),
  CONSTRAINT "fk_board_model_soc1"
    FOREIGN KEY("soc_id")
    REFERENCES "soc"("id")
);
CREATE INDEX "piduino"."board_model.fk_board_model_board_family1_idx" ON "board_model" ("board_family_id");
CREATE INDEX "piduino"."board_model.fk_board_model_soc1_idx" ON "board_model" ("soc_id");
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(0, 'RaspberryPi A', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(1, 'RaspberryPi B', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(2, 'RaspberryPi A+', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(3, 'RaspberryPi B+', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(4, 'RaspberryPi Compute Module', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(5, 'RaspberryPi 2', 0, 1);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(6, 'RaspberryPi Zero', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(7, 'RaspberryPi 3', 0, 2);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(8, 'RaspberryPi Zero Wifi', 0, 0);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(9, 'RaspberryPi Compute Module 3', 0, 2);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(10, 'NanoPi Neo', 1, 3);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(11, 'NanoPi Neo Air', 1, 3);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(12, 'NanoPi M1', 1, 3);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(13, 'NanoPi M1+', 1, 3);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(14, 'NanoPi Neo 2', 1, 4);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(15, 'NanoPi Neo+ 2', 1, 4);
INSERT INTO "board_model"("id","name","board_family_id","soc_id") VALUES(-1, 'Unknown', -1, -1);
COMMIT;
