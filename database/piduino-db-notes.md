# Nouvelle carte avec nouveau GPIO et connecteurs

## Nouvelle broche CVBS

-- (id,pin_type_id)
INSERT INTO "pin" VALUES(98,4);

-- (id,name)
INSERT INTO "pin_name" VALUES(258,'CVBS');

-- (pin_id,pin_name_id,pin_mode_id)
INSERT INTO "pin_has_name" VALUES(98,258,0);

-- (soc_id,pin_id)
INSERT INTO "soc_has_pin" VALUES(3,98);
INSERT INTO "soc_has_pin" VALUES(4,98);

## Nouveaux connecteurs

-- (id,name,rows,connector_family_id)
INSERT INTO "connector" VALUES(17,'j1_j2',16,1);
INSERT INTO "connector" VALUES(18,'inner',7,0);

-- (connector_id,pin_id,row,column)
INSERT INTO "connector_has_pin" VALUES(17,2,1,1);
INSERT INTO "connector_has_pin" VALUES(17,53,1,2);
INSERT INTO "connector_has_pin" VALUES(17,2,2,1);
INSERT INTO "connector_has_pin" VALUES(17,52,2,2);
INSERT INTO "connector_has_pin" VALUES(17,1,3,1);
INSERT INTO "connector_has_pin" VALUES(17,5,3,2);
INSERT INTO "connector_has_pin" VALUES(17,5,4,1);
INSERT INTO "connector_has_pin" VALUES(17,37,4,2);
INSERT INTO "connector_has_pin" VALUES(17,74,5,1);
INSERT INTO "connector_has_pin" VALUES(17,36,5,2);
INSERT INTO "connector_has_pin" VALUES(17,38,6,1);
INSERT INTO "connector_has_pin" VALUES(17,69,6,2);
INSERT INTO "connector_has_pin" VALUES(17,77,7,1);
INSERT INTO "connector_has_pin" VALUES(17,72,7,2);
INSERT INTO "connector_has_pin" VALUES(17,76,8,1);
INSERT INTO "connector_has_pin" VALUES(17,68,8,2);
INSERT INTO "connector_has_pin" VALUES(17,59,9,1);
INSERT INTO "connector_has_pin" VALUES(17,71,9,2);
INSERT INTO "connector_has_pin" VALUES(17,58,10,1);
INSERT INTO "connector_has_pin" VALUES(17,40,10,2);
INSERT INTO "connector_has_pin" VALUES(17,87,11,1);
INSERT INTO "connector_has_pin" VALUES(17,39,11,2);
INSERT INTO "connector_has_pin" VALUES(17,85,12,1);
INSERT INTO "connector_has_pin" VALUES(17,98,12,2);
INSERT INTO "connector_has_pin" VALUES(17,86,13,1);
INSERT INTO "connector_has_pin" VALUES(17,81,13,2);
INSERT INTO "connector_has_pin" VALUES(17,84,14,1);
INSERT INTO "connector_has_pin" VALUES(17,80,14,2);
INSERT INTO "connector_has_pin" VALUES(17,82,15,1);
INSERT INTO "connector_has_pin" VALUES(17,78,15,2);
INSERT INTO "connector_has_pin" VALUES(17,83,16,1);
INSERT INTO "connector_has_pin" VALUES(17,79,16,2);
INSERT INTO "connector_has_pin" VALUES(18,54,1,1);
INSERT INTO "connector_has_pin" VALUES(18,55,2,1);
INSERT INTO "connector_has_pin" VALUES(18,73,3,1);
INSERT INTO "connector_has_pin" VALUES(18,47,4,1);
INSERT INTO "connector_has_pin" VALUES(18,48,5,1);
INSERT INTO "connector_has_pin" VALUES(18,50,6,1);
INSERT INTO "connector_has_pin" VALUES(18,51,7,1);

## Nouveau gpio

-- (id,name,board_family_id) 
INSERT INTO "gpio" VALUES(10,'nanopiduo2',1);

--- (num,gpio_id,connector_id)
INSERT INTO "gpio_has_connector" VALUES(1,10,17);
INSERT INTO "gpio_has_connector" VALUES(2,10,18);

# Nouvelle carte

-- (id,name,board_family_id,soc_id) 
INSERT INTO "board_model" VALUES(22,'NanoPi Duo2',1,3);

-- (id,name,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id) 
INSERT INTO "board" VALUES(45,'NanoPi Duo2',NULL,NULL,22,10,6,0,0,1);

-- (board_id,tag)
INSERT INTO "tag" VALUES(45,'nanopiduo2');


PIDUINO_CONN_INFO="sqlite3:db=/home/pascal/piduino.db"

# Affichage des broches d'un connecteur

select row,column,pin_has_name.pin_id,name,connector_id from connector_has_pin 
inner join  pin_has_name on connector_has_pin.pin_id = pin_has_name.pin_id
inner join  pin_name on pin_name.id = pin_has_name.pin_name_id 
where connector_id=7 and pin_has_name.pin_mode_id = 0
order by row,column;

# Affichage des broches d'un SoC

select soc_has_pin.pin_id,name from soc_has_pin 
inner join  pin_has_name on soc_has_pin.pin_id = pin_has_name.pin_id
inner join  pin_name on pin_name.id = pin_has_name.pin_name_id 
where soc_id=3 and pin_has_name.pin_mode_id = 0
order by name;

# Affichage des broches d'alimentation

select pin.id,name from pin 
inner join  pin_has_name on pin.id = pin_has_name.pin_id
inner join  pin_name on pin_name.id = pin_has_name.pin_name_id 
where pin_type_id=1 and pin_has_name.pin_mode_id = 0
order by name;

# Affichage des noms d'une broche

select pin_id,name from pin_has_name 
inner join  pin_name on pin_name.id = pin_has_name.pin_name_id 
where pin_id=73
order by name;

# Listing des cartes Armbian

select name,tag from board
inner join  tag on board.id = tag.board_id;

# Listing des cartes Raspberry Pi

select name,printf("0x%X",revision) from board
inner join  revision on board.id = revision.board_id;
