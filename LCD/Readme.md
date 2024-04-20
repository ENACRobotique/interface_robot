# Carte LCD

## Afficher 

Envoyer sur l'UART 37 charactères:
- 16*2 charactères pour le LCD 
- 3 leds(R,G,B): `'0'` pour éteindre, n'importe quel autre charactère pour allumer
- buzzer:
    - `< 'A'` : arrête le buzzer
    - `['A'-'G'] + 7*octave` : note à cette octave (0<=octave<=2)
- `'\0'`: fin de chaine de charactère


## Recevoir

Le LCD envoie un message: 
- lors d'un événement
- périodiquement

Le message est formaté comme suit:

`<ok> <ret> <color> <tirette> <potar>\n`

Avec `<potar>` étant un entier, et les autres champs étant un charactère parmis:
- `'P'`: bouton appuyé (événement)
- `'R'`: bouton relâché (événement)
- `'O'`: bouton ouvert (état)
- `'C'`: bouton fermé (état)

## Programmer la carte

Installer pymcuprog : `python3 -m pip install pymcuprog`

Il faut ensuite utiliser un convertisseur USB<=>UART branché au µC de cette manière :

```

                        Vcc                     Vcc
                        +-+                     +-+
                         |                       |
 +---------------------+ |                       | +--------------------+
 | Serial port         +-+                       +-+  AVR device        |
 |                     |      +----------+         |                    |
 |                  TX +------+   1k     +---------+ UPDI               |
 |                     |      +----------+    |    |                    |
 |                     |                      |    |                    |
 |                  RX +----------------------+    |                    |
 |                     |                           |                    |
 |                     +--+                     +--+                    |
 +---------------------+  |                     |  +--------------------+
                         +-+                   +-+
                         GND                   GND
```


Références:
> https://pypi.org/project/pymcuprog/
