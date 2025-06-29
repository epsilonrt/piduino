# Units tests for GPIO driver using gpiod
# This file contains unit tests for the GPIO driver using gpiod.

## mode -> Ok

```bash
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido mode 0
in
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido mode 0 out
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido mode 0
out
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido readall
                                           J8 (#1)
+-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
| sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
+-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
|     |     |     3.3V |      |      |   |  1 || 2  |   |      |      | 5V       |     |     |
|   2 |   8 |     SDA1 | ALT0 |   UP | 1 |  3 || 4  |   |      |      | 5V       |     |     |
|   3 |   9 |     SCL1 | ALT0 |   UP | 1 |  5 || 6  |   |      |      | GND      |     |     |
|   4 |   7 |    GPIO4 |   IN |   UP | 1 |  7 || 8  | 1 | OFF  | ALT5 | TXD1     | 15  | 14  |
|     |     |      GND |      |      |   |  9 || 10 | 1 | UP   | ALT5 | RXD1     | 16  | 15  |
|  17 |   0 |   GPIO17 |  OUT | DOWN | 1 | 11 || 12 | 1 | DOWN | IN   | GPIO18   | 1   | 18  |
|  27 |   2 |   GPIO27 |   IN | DOWN | 1 | 13 || 14 |   |      |      | GND      |     |     |
|  22 |   3 |   GPIO22 |   IN | DOWN | 0 | 15 || 16 | 0 | DOWN | IN   | GPIO23   | 4   | 23  |
|     |     |     3.3V |      |      |   | 17 || 18 | 0 | DOWN | IN   | GPIO24   | 5   | 24  |
|  10 |  12 | SPI0MOSI | ALT0 | DOWN | 0 | 19 || 20 |   |      |      | GND      |     |     |
|   9 |  13 | SPI0MISO | ALT0 | DOWN | 0 | 21 || 22 | 0 | DOWN | IN   | GPIO25   | 6   | 25  |
|  11 |  14 | SPI0SCLK | ALT0 | DOWN | 0 | 23 || 24 | 1 | UP   | OUT  | GPIO8    | 10  | 8   |
|     |     |      GND |      |      |   | 25 || 26 | 1 | UP   | OUT  | GPIO7    | 11  | 7   |
|   0 |  30 |    GPIO0 |   IN |   UP | 1 | 27 || 28 | 1 | UP   | IN   | GPIO1    | 31  | 1   |
|   5 |  21 |    GPIO5 |   IN |   UP | 1 | 29 || 30 |   |      |      | GND      |     |     |
|   6 |  22 |    GPIO6 |   IN |   UP | 1 | 31 || 32 | 0 | DOWN | IN   | GPIO12   | 26  | 12  |
|  13 |  23 |   GPIO13 |   IN | DOWN | 0 | 33 || 34 |   |      |      | GND      |     |     |
|  19 |  24 |   GPIO19 |   IN | DOWN | 0 | 35 || 36 | 0 | DOWN | IN   | GPIO16   | 27  | 16  |
|  26 |  25 |   GPIO26 |   IN | DOWN | 0 | 37 || 38 | 0 | DOWN | IN   | GPIO20   | 28  | 20  |
|     |     |      GND |      |      |   | 39 || 40 | 0 | DOWN | IN   | GPIO21   | 29  | 21  |
+-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
| sOc | iNo |   Name   | Mode | Pull | V | Ph || Ph | V | Pull | Mode |   Name   | iNo | sOc |
+-----+-----+----------+------+------+---+----++----+---+------+------+----------+-----+-----+
```

**Par contre, une direction modifiée directement sur le controleur PIO ne sera pas prise en compte par le driver. Il faut faire un write ou read pour que le driver prenne en compte la nouvelle direction.**


## read -> Ok
```bash
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido read 0
1
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido write 0 0
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido read 0
0
```

## write -> Ok
```bash
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido write 0 0
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido read 0
0
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido write 0 1
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido read 0
1
```

## pull -> failed in read mode
```bash
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido pull 0
down
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido pull 0 up
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido pull 0
unk
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido pull 0 down
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido pull 0
down
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido pull 0 up
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido pull 0
up
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ sudo ./pido pull 0 off
epsilonrt@rpi-bookworm-arm64:~/src/piduino/build $ pido pull 0
off
```

## wfi

## interrupt

