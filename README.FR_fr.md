# PiDuino

Arduino sur cartes Pi, le meilleur des 2 mondes.

## Abstract 

PiDuino est né d'une question d'un de mes élèves qui m'a demandé la 
raison pour laquelle la programmation des entrées-sorties sur NanoPi 
n'était pas aussi simple que sur [Arduino](https://www.arduino.cc/).

PiDuino a donc comme objectif de répondre à ce besoin: _Rendre la 
programmation des entrées-sorties (PIO, Port série, Bus I2C et 
SPI...) sur les cartes Pi ([Raspberry 
Pi](https://www.raspberrypi.org/), [Nano Pi](http://www.nanopi.org/), 
[Orange Pi](http://www.orangepi.org/), [Banana 
Pi](http://www.banana-pi.org/), [Beagle 
Board](https://beagleboard.org/)...) la plus proche possible de celle d'Arduino._

Il existe quelques projets qui ont cherché à répondre à ce besoin mais en se cantonnant à proposer une solution pour un modèle de carte Pi. 

Le plus connu est sans doute [wiringPi](https://github.com/WiringPi). 
wiringPi est une solution prévue pour Raspberry Pi et même si il 
existe des versions dérivées sur d'autres cartes Pi, ces versions 
sont des versions "boiteuses" d'un point de vue informatique de la version originale.

Voilà les raisons qui m'ont poussées à ne pas choisir wiringPi:

* même si il existe une similitude avec la programmation Arduino, il y 
a des différences qui s'accentuent avec le temps.  
* wiringPi a été conçu en C pur, ce qui est un frein certain à 
l'évolutivité et n'est pas très cohérent avec Arduino (Arduino 
c'est du C++ !)  
* wiringPi a été conçu pour le Raspberry Pi et son adaptation pour 
d'autres cartes Pi est de plus en plus inenvisageable, au fur et à 
mesure de l'arrivée de nouvelles cartes Pi.

Ce que propose PiDuino:

* Une interface de programmation 
[API](https://en.wikipedia.org/wiki/Application_programming_interface) identique à Arduino mis à part 
l'ajout du `#include <Arduino.h>` en début de programme. Cela 
n'interdit pas de proposer des extensions de l'API mais à condition de 
rester le plus indépendant possible de la plateforme et de ne pas 
rendre le code incompatible avec Arduino. Il est logique de penser que 
des utilisateurs qui souhaitent rester dans la monde Arduino utilisent 
C++, PiDuino est donc prévu pour ce cas d'utilisation. Néanmoins quelques fonctions sont utilisables en C (`pinMode()`, 
`digitalWrite()`, ...). Une API en Python est prévue grâce à 
[Swig](http://www.swig.org/).

* La **description des cartes Pi** qui repose sur un modèle "Objet" 
stockées **dans une base de données** (SQLite par défaut), permettant à 
un simple utilisateur d'ajouter une nouvelle "variante" de carte Pi **SANS**
programmation, ni connaissance informatique poussées.

* Une conception objet en C++ avec une séparation claire de la partie 
spécifique à la plateforme. La prise en charge de nouveaux SoC se 
résume à ajouter une partie "driver" dans le répertoire `src/arch`

* Des utilitaires permettant de de manipuler les signaux GPIO (`gpio`), de 
récupérer des informations sur la carte (`pinfo`) ou d'ajouter de 
gérer la base de données des cartes Pi.

## Road Map

**PiDuino est en développement**, en version 0.2 actuellement mais les 
parties terminées sont fonctionnelles sur SoC Broadcom BCM283X et 
AllWinner Hx.

Ce qui a été fait 

* Modélisation des GPIO, connecteurs et broches de GPIO  
* Création du modèle de base de données et ajout de toutes les 
variantes de Raspberry Pi, des Nano Pi Neo, Neo2, Neo Plus 2, M1, M1 
Plus.  
* Création des couches d'accès au SoC pour Broadcom BCM283X et 
AllWinner Hx.  
* Création des utilitaires `gpio` et `pinfo`
* Passage de iomap en C++    

Ce qui reste à faire:

* Nettoyage la détection d'architecture   
* Création d'une classe purement virtuelle IoDevice  
* API analogWrite()
* API Port série  
* API Bus I2C
* API Bus SPI  
* Emulation setup() et loop() avec gestion du passage en mode daemon
* Mise à jour README   
* Classes Arduino (String ....)  
* Pages Man pour les utilitaires  
* Outil de gestion de la base de données des cartes sous Qt  
* Documentation Doxygen des classes Database  
* Création d'une page Web  

## Dépendances

La compilation nécessite un compilateur [g++](https://gcc.gnu.org) gérant 
[C++11](https://en.wikipedia.org/wiki/C%2B%2B11), la compilation a 
été effectuée avec gcc version 6.3.0 20170516. Ce compilateur est 
généralement installé avec un chaine de compilation complète sur 
les systèmes Linux utilisés sur les cartes Pi.

Les dépendances sont les suivantes:

* libcppdb-dev qui fournit [CppDB](http://cppcms.com/sql/cppdb/) pour 
l'accès à la base de données  
* Qt5 si on souhaite compiler l'outil de gestion de la base de données.


## Installation

    sudo apt-get update
    sudo apt-get install libcppdb-dev pkg-config cmake libsqlite3-dev sqlite3
    git clone https://github.com/epsilonrt/piduino.git
    cd piduino
    git checkout dev
    mkdir cmake-build-Release
    cd cmake-build-Release
    cmake ..
    make -j4
    sudo make install
    sudo ldconfig

Pour désinstaller :

    cd cmake-build-Release
    sudo make uninstall

On peut aussi générer des paquets Debian avec :

    make package
    sudo dpkg -i *.deb

Pour générer une documentation au format HTML, il faut installer les 
pré-requis :

    sudo apt-get install doxygen doxygen-latex doxygen-doc doxygen-gui graphviz
    make doc

