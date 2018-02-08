# Protocole de communication UART

## Principe général

Les échanges sont constitués de trames binaires structurées telles que suivant :

* OpCode sur 1 octet
* Taille sur 1 octets
* Data de 0 à 255 octets

## Sens BeagleBone -> µC

### 1 setAngle

opCode : 1
data sur 1 octet, représentant l'angle à donner (de 0 à 180)

### 2 setSpeed

opCode : 2
data À DÉFINIR

### 3 setLight

opCode : 3
data À DÉFINIR

