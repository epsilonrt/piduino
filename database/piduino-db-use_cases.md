# PiDuino Database management use cases
_Working document allowing to discover the use cases of the database_

## Addition of a new Raspberry Pi model with its variants (CM3+)

| Rev 0x | Rev      | Model | PCB Rev | RAM | Manufacturer |
|--------|----------|-------|---------|-----|--------------|
| A02100 | 10494208 | CM3+  | 1.0     | 1GB | Sony UK      |

### `pinfo` commands to perform

**TODO**

```sh
```

### Corresponding SQL requests

**TODO**

```sql
```

## Addition of a new Raspberry Pi model with its variants (4B)

| Rev 0x | Rev      | Model | PCB Rev | RAM | Manufacturer |
|--------|----------|-------|---------|-----|--------------|
| A03111 | 10498321 | 4B    | 1.1     | 1GB | Sony UK(1)   |
| B03111 | 11546897 | 4B    | 1.1     | 2GB | Sony UK(1)   |
| C03111 | 12595473 | 4B    | 1.1     | 4GB | Sony UK(1)   |
| C03112 | 12595474 | 4B    | 1.2     | 4GB | Sony UK(1)   |

### New board model

#### `pinfo` commands to perform

```sh
list soc
list board_family
# add board_model name family_id soc_id
add board_model "RaspberryPi 4B" 0 5
```

#### Corresponding SQL requests

```sql
-- (id,name,board_family_id,soc_id) 
INSERT INTO "board_model" VALUES(23,'RaspberryPi 4B',0,5);
```

### New board variants

#### `pinfo` commands to perform

```sh
list board_model
list gpio
list manufacturer
list revision # checks for revisions exist
# add board name model_id gpio_id manufacturer_id default_i2c default_spi default_uart [-rREVISION] [-mRAM] [-pPCB_REV]
add board "RaspberryPi 4B (0xA03111)" 23 3 1 1 0 0 -r0xa03111 -m1024 -p"1.1"
add board "RaspberryPi 4B (0xB03111)" 23 3 1 1 0 0 -r0xB03111 -m2048 -p"1.1"
add board "RaspberryPi 4B (0xC03111)" 23 3 1 1 0 0 -r0xC03111 -m4096 -p"1.1"
add board "RaspberryPi 4B (0xC03112)" 23 3 1 1 0 0 -r0xC03112 -m4096 -p"1.2"
```

#### Corresponding SQL requests

```sql
-- (id,name,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id) 
INSERT INTO "board" VALUES(50,'RaspberryPi 4B (0xA03111)',1024,'1.1',23,3,1,1,0,0);
INSERT INTO "board" VALUES(51,'RaspberryPi 4B (0xB03111)',2048,'1.1',23,3,1,1,0,0);
INSERT INTO "board" VALUES(52,'RaspberryPi 4B (0xC03111)',4096,'1.1',23,3,1,1,0,0);
INSERT INTO "board" VALUES(53,'RaspberryPi 4B (0xC03112)',4096,'1.2',23,3,1,1,0,0);

-- (board_id,revision)
INSERT INTO "revision" VALUES(50,10498321);
INSERT INTO "revision" VALUES(51,11546897);
INSERT INTO "revision" VALUES(52,12595473);
INSERT INTO "revision" VALUES(53,12595474);
```

## Addition of Raspberry Pi variants of existing models


| Rev 0x | Rev        | Model             | PCB Rev | RAM     | Manufacturer |
|--------|------------|-------------------|---------|---------|--------------|
| A220A0 | 10625184   | CM3               | 1.0     | 1GB     | Embest(4)    |
| A22083 | 10625155   | 3B                | 1.3     | 1GB     | Embest(4)    |
| 900061 | 9437281    | CM                | 1.1     | 512MB   | Sony UK      |
| A02042 | 10494018   | 2B (with BCM2837) | 1.2     | 1GB     | Sony UK      |

#### `pinfo` commands to perform

```
list board_model
list gpio
list manufacturer
list revision # checks for revisions exist
# add board name model_id gpio_id manufacturer_id default_i2c default_spi default_uart [-rREVISION] [-tTAG] [-mRAM] [-pPCB_REV]
add board "RaspberryPi Compute Module 3 (0xA220A0)" 9 3 4 1 0 0 -r0xa220a0 -m1024 -p"1"
add board "RaspberryPi 3B (0xA22083)" 7 3 4 1 0 0 -r0xa22083 -m1024 -p"1.3"
add board "RaspberryPi Compute Module (0x900061)" 4 3 1 1 0 0 -r0x900061 -m512 -p"1.1"
add board "RaspberryPi 2B (0xA02042)" 5 3 1 1 0 0 -r0xa02042 -m1024 -p"1.2"
```

#### Corresponding SQL requests

```sql
-- (id,name,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id) 
INSERT INTO "board" VALUES(46,'RaspberryPi Compute Module 3 (0xA220A0)',1024,'1',9,3,4,1,0,0);
INSERT INTO "board" VALUES(47,'RaspberryPi 3B (0xA22083)',1024,'1.3',7,3,4,1,0,0);
INSERT INTO "board" VALUES(48,'RaspberryPi Compute Module (0x900061)',512,'1.1',4,3,1,1,0,0);
INSERT INTO "board" VALUES(49,'RaspberryPi 2B (0xA02042)',1024,'1.2',5,3,1,1,0,0);

-- (board_id,revision)
INSERT INTO "revision" VALUES(46,10625184);
INSERT INTO "revision" VALUES(47,10625155);
INSERT INTO "revision" VALUES(48,9437281);
INSERT INTO "revision" VALUES(49,10494018);
```

## Addition of manufacturer

Add manufacturer, existing card (30)

| Rev 0x | Rev      | Model | PCB Rev | RAM | Manufacturer  |
|--------|----------|-------|---------|-----|---------------|
| A32082 | 10690690 | 3B    | 1.2     | 1GB | Sony Japan    |

#### `pinfo` commands to perform

```
list manufacturer # checks for manufacturer exist
# add manufacturer name
add manufacturer "Sony Japan"
mod board 30 manufacturer 8
```

#### Corresponding SQL requests

```sql
-- (id,name)
INSERT INTO "manufacturer" VALUES(8,'Sony Japan');

-- (id,name,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id) 
UPDATE board
SET manufacturer_id = 8
WHERE id=30;
```

## Add board with new GPIO and connectors (here NanoPi Duo2)

| SoC | iNo |          Name | GPIO1 | Pin# | Pin# | GPIO2 | Name                            | iNo | SoC |
|:---:|:---:|--------------:|------:|:----:|:----:|-------|---------------------------------|:---:|:---:|
|     |     |        VDD_5V |    5V |   1  |   2  | RXD   | DEBUG_RX(UART_RXD0)/GPIOA5/PWM0 |  18 |  5  |
|     |     |        VDD_5V |    5V |   3  |   4  | TXD   | DEBUG_TX(UART_TXD0)/GPIOA4      |  17 |  4  |
|     |     |      SYS_3.3V |   3V3 |   5  |   6  | GND   | GND                             |     |     |
|     |     |           GND |   GND |   7  |   8  | SCL   | I2C0_SCL/GPIOA11                |  9  |  11 |
| 363 |  20 | GPIOL11/IR-RX |  IRRX |   9  |  10  | SDA   | I2C0_SDA/GPIOA12                |  8  |  12 |
| 203 |  7  |       GPIOG11 |  PG11 |  11  |  12  | CS    | UART3_TX/SPI1_CS/GPIOA13        |  27 |  13 |
|     |     |       USB-DM3 |   DM3 |  13  |  14  | CLK   | UART3_RX/SPI1_CLK/GPIOA14       |  29 |  14 |
|     |     |       USB-DP3 |   DP3 |  15  |  16  | MISO  | UART3_CTS/SPI1_MISO/GPIOA16     |  24 |  16 |
|     |     |       USB-DM2 |   DM2 |  17  |  18  | MOSI  | UART3_RTS/SPI1_MOSI/GPIOA15     |  28 |  15 |
|     |     |       USB-DP2 |   DP2 |  19  |  20  | RX1   | UART1_RX/GPIOG7                 |  16 | 199 |
|     |     |      EPHY-RXN |   RD- |  21  |  22  | TX1   | UART1_TX/GPIOG6                 |  15 | 198 |
|     |     |      EPHY-RXP |   RD+ |  23  |  24  | CVBS  | CVBS                            |     |     |
|     |     |      EPHY-TXN |   TD- |  25  |  26  | LL    | LINEOUT_L                       |     |     |
|     |     |      EPHY-TXP |   TD+ |  27  |  28  | LR    | LINEOUT_R                       |     |     |
|     |     | EPHY-LED-LINK |   LNK |  29  |  30  | MP    | MIC_P                           |     |     |
|     |     |  EPHY-LED-SPD |   SPD |  31  |  32  | MN    | MIC_N                           |     |     |

```sh
list pin name "GPIOA*"
list pin name "GPIOG*"
list pin name "GPIOL*"
list pin name "CVBS"
list pin power
list pin net
list pin audio
```

```sql
SELECT pin_id,name FROM pin_has_name 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_name.name LIKE 'GPIOA%'
GROUP BY name
ORDER BY name;
```

```sh
list pin power
list pin usb
list pin audio
list pin video
list pin net
```

```sql
-- power
SELECT pin.id,name FROM pin 
INNER JOIN  pin_has_name on pin.id = pin_has_name.pin_id 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_type_id = 1
ORDER BY name;
-- usb
SELECT pin.id,name FROM pin 
INNER JOIN  pin_has_name on pin.id = pin_has_name.pin_id 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_type_id = 2
ORDER BY name;
-- audio
SELECT pin.id,name FROM pin 
INNER JOIN  pin_has_name on pin.id = pin_has_name.pin_id 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_type_id = 3
ORDER BY name;
-- video
SELECT pin.id,name FROM pin 
INNER JOIN  pin_has_name on pin.id = pin_has_name.pin_id 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_type_id = 4
ORDER BY name;
-- net
SELECT pin.id,name FROM pin 
INNER JOIN  pin_has_name on pin.id = pin_has_name.pin_id 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_type_id = 6
ORDER BY name;
```

### New pin CVBS (Composite Video)

#### `pinfo` commands to perform

```sh
list soc
list pin
list pin soc 3
list pin soc 4
# add pin type name
add pin video "CVBS"
# add name2pin pin_id mode name [mode name] 
## add name2pin 98 alt0 XXX alt5 XXX
# add pin2soc pin_id soc_id
add pin2soc 98 3
add pin2soc 98 4
```

#### Corresponding SQL requests

```sql
-- (id,pin_type_id)
INSERT INTO "pin" VALUES(98,4);

-- (id,name)
INSERT INTO "pin_name" VALUES(258,'CVBS');

-- (pin_id,pin_name_id,pin_mode_id)
INSERT INTO "pin_has_name" VALUES(98,258,0);

-- (soc_id,pin_id)
INSERT INTO "soc_has_pin" VALUES(3,98); -- H3
INSERT INTO "soc_has_pin" VALUES(4,98); -- H5
```

### New connectors

#### `pinfo` commands to perform

```sh
list board_model
list connector board_model 22
add connector "Header2X" "j2-j1" 16
add connector "Header1X" "inner" 7
```

#### Corresponding SQL requests

```sql
-- (id,name,rows,connector_family_id)
INSERT INTO "connector" VALUES(17,'j2-j1',16,1);
INSERT INTO "connector" VALUES(18,'inner',7,0);
```

### New connector pins

#### `pinfo` commands to perform

```sh
# add pin2con pin_id connector_id row [col]
add pin2con 2	17	1	1
add pin2con 2	17	2	1
add pin2con 1	17	3	1
add pin2con 5	17	4	1
add pin2con 74	17	5	1
add pin2con 38	17	6	1
add pin2con 77	17	7	1
add pin2con 76	17	8	1
add pin2con 59	17	9	1
add pin2con 58	17	10	1
add pin2con 87	17	11	1
add pin2con 85	17	12	1
add pin2con 86	17	13	1
add pin2con 84	17	14	1
add pin2con 82	17	15	1
add pin2con 83	17	16	1
add pin2con 53	17	1	2
add pin2con 52	17	2	2
add pin2con 5	17	3	2
add pin2con 37	17	4	2
add pin2con 36	17	5	2
add pin2con 69	17	6	2
add pin2con 72	17	7	2
add pin2con 68	17	8	2
add pin2con 71	17	9	2
add pin2con 40	17	10	2
add pin2con 39	17	11	2
add pin2con 98	17	12	2
add pin2con 81	17	13	2
add pin2con 80	17	14	2
add pin2con 78	17	15	2
add pin2con 79	17	16	2
add pin2con 54	18	1	1
add pin2con 55	18	2	1
add pin2con 73	18	3	1
add pin2con 47	18	4	1
add pin2con 48	18	5	1
add pin2con 50	18	6	1
add pin2con 51	18	7	1
```

#### Corresponding SQL requests

```sql
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
```

### new GPIO

#### `pinfo` commands to perform

```sh
list board_family
# add gpio name board_family_id
add gpio "nanopiduo2" 1
list gpio
# add con2gpio connector_id gpio_id connector_number
add con2gpio 17 10 1
add con2gpio 18 10 2
```

#### Corresponding SQL requests

```sql
-- (id,name,board_family_id) 
INSERT INTO "gpio" VALUES(10,'nanopiduo2',1);

--- (num,gpio_id,connector_id)
INSERT INTO "gpio_has_connector" VALUES(1,10,17);
INSERT INTO "gpio_has_connector" VALUES(2,10,18);
```

### new board

#### `pinfo` commands to perform

```sh
# add board_model name family_id soc_id
add board_model "NanoPi Duo2" 1 3
# add board name model_id gpio_id manufacturer_id default_i2c default_spi default_uart [-rREVISION] [-tTAG] [-mRAM] [-pPCB_REV]
add board "NanoPi Duo2" 22 10 6 0 0 1 -tnanopiduo2
```

#### Corresponding SQL requests

```sql
-- (id,name,board_family_id,soc_id) 
INSERT INTO "board_model" VALUES(22,'NanoPi Duo2',1,3);

-- (id,name,ram,pcb_revision,board_model_id,gpio_id,manufacturer_id,default_i2c_id,default_spi_id,default_uart_id) 
INSERT INTO "board" VALUES(45,'NanoPi Duo2',NULL,NULL,22,10,6,0,0,1);

-- (board_id,tag)
INSERT INTO "tag" VALUES(45,'nanopiduo2');
```

## Listings

### List the pins of a connector

```sql
SELECT row,column,pin_has_name.pin_id,name,connector_id FROM connector_has_pin 
INNER JOIN  pin_has_name on connector_has_pin.pin_id = pin_has_name.pin_id
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE connector_id=7 and pin_has_name.pin_mode_id = 0
ORDER BY row,column;
```

### List the pins of a SoC

```sql
SELECT soc_has_pin.pin_id,name FROM soc_has_pin 
INNER JOIN  pin_has_name on soc_has_pin.pin_id = pin_has_name.pin_id
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE soc_id=3 and pin_has_name.pin_mode_id = 0
ORDER BY name;
```

### List the power pins

```sql
SELECT pin.id,name FROM pin 
INNER JOIN  pin_has_name on pin.id = pin_has_name.pin_id
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_type_id=1 and pin_has_name.pin_mode_id = 0
ORDER BY name;
```

### List the names of a pin

```sql
SELECT pin_id,name FROM pin_has_name 
INNER JOIN  pin_name on pin_name.id = pin_has_name.pin_name_id 
WHERE pin_id=73
ORDER BY name;
```

### List Armbian boards

```sql
SELECT name,tag FROM board
INNER JOIN  tag on board.id = tag.board_id;
```

### List Raspberry Pi boards 

```sql
SELECT name,printf("0x%X",revision) FROM board
INNER JOIN  revision on board.id = revision.board_id;
```

## Test notes

### Use of a personal database

```sh
export PIDUINO_CONN_INFO="sqlite3:db=/home/pascal/piduino.db"
```

----
#### `pinfo` commands to perform
#### Corresponding SQL requests
```sql
```
