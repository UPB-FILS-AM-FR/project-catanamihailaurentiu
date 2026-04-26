# Thermin Digital Pro

| | |
|-|-|
|`Author` | Catana Mihai-Laurentiu

## Description
Theremin Numérique Pro est un instrument de musique numérique sans contact physique. L'utilisateur contrôle la hauteur et le volume du son en déplaçant ses mains au-dessus de deux capteurs à ultrasons, sans aucun contact physique. La main gauche contrôle la note musicale (Do, Ré, Mi...), tandis que la main droite contrôle le volume. Un potentiomètre permet de changer d'octave, et deux boutons offrent les fonctions de sourdine et de changement de mode. La note actuelle et le volume sont affichés en temps réel sur un écran OLED, et des LEDs RGB fournissent un retour visuel synchronisé avec le son.

## Motivation
J'ai toujours été fasciné par la musique et l'électronique. Ce projet me permet de combiner ces deux passions en créant un instrument que l'on joue sans le toucher — une idée qui me semblait presque magique avant de comprendre comment la réaliser. 
## Architecture
```
[HC-SR04 Gauche] ──← note musicale
      │
      ▼
[ESP32 DevKit V1]
      │
      ├──← [HC-SR04 Droite]
      │         volume
      │
      ├──← [Potentiomètre 10kΩ]
      │         sélection octave (ADC)
      │
      ├──← [Bouton 1]
      │         sourdine (interruption matérielle)
      │
      ├──← [Bouton 2]
      │         changement de mode (interruption matérielle)
      │
      ├──→ [OLED SSD1306]
      │         note actuelle + barre de volume (I2C)
      │
      ├──→ [PAM8403 + Haut-parleur 3W]
      │         sortie audio (PWM)
      │
      └──→ [NeoPixel x8]
                couleur synchronisée avec la note
```
Le système est composé de plusieurs modules :

Module de contrôle : ESP32 DevKit V1
Module de détection pitch : HC-SR04 Gauche (distance main → note musicale)
Module de détection volume : HC-SR04 Droite (distance main → volume)
Module de sélection : potentiomètre 10kΩ (sélection de l'octave via ADC)
Module d'affichage : écran OLED SSD1306 (note actuelle + barre de volume via I2C)
Module audio : amplificateur PAM8403 + haut-parleur 3W (sortie audio via PWM)
Module visuel : bande NeoPixel x8 (couleur synchronisée avec la note)
Module interaction : 2 boutons poussoirs (sourdine + changement de mode via interruptions matérielles)

Le processus de fonctionnement est le suivant :

Lecture de la distance de la main gauche via HC-SR04 Gauche.
Conversion de la distance en fréquence musicale (Do, Ré, Mi...).
Lecture de la distance de la main droite via HC-SR04 Droite.
Conversion de la distance en niveau de volume.
Lecture du potentiomètre via ADC pour déterminer l'octave active.
Génération du signal audio via PWM vers le module PAM8403.
Mise à jour en temps réel de l'écran OLED :

note musicale actuelle,
barre de volume.


Mise à jour de la couleur des LEDs NeoPixel selon la note jouée.

Le système propose deux modes de jeu :

Free Play — fréquence continue et variable selon la distance exacte,
Quantized — le son se verrouille sur la note musicale la plus proche (Do, Ré, Mi, Fa, Sol, La, Si).

Les boutons déclenchent des interruptions matérielles pour une réponse instantanée, sans délai lié à la boucle principale du programme.
### Block diagram

<!-- Make sure the path to the picture is correct -->
![Block Diagram](theremin_block_diagram.svg)

### Schematic

![Schematic](schematics/kicad_schematic.png)

### Components


<!-- This is just an example, fill in with your actual components -->

| Device | Usage | Price |
|--------|--------|-------|
| Activ Buzzer | Buzzer | [1.5 RON](https://www.optimusdigital.ro/ro/audio-buzzere/635-buzzer-activ-de-3-v.html?search_query=buzzer&results=61) |
| Push Button | Button | [1 RON](https://www.optimusdigital.ro/ro/butoane-i-comutatoare/1119-buton-6x6x6.html?search_query=buton&results=222) |
| Jumper Wires | Connecting components | [7 RON](https://www.optimusdigital.ro/ro/fire-fire-mufate/884-set-fire-tata-tata-40p-10-cm.html?search_query=set+fire&results=110) |
| Breadboard | Project board | [10 RON](https://www.optimusdigital.ro/ro/prototipare-breadboard-uri/8-breadboard-830-points.html?search_query=breadboard&results=145) |

### Libraries

<!-- This is just an example, fill in the table with your actual components -->

| Library | Description | Usage |
|---------|-------------|-------|
| [lib-name1](link-to-lib) | official description of the lib | Used for accesing the peripherals of the microcontroller  |
| [lib-name2](link-to-lib) | official description of the lib | Used for accesing the peripherals of the microcontroller  |

## Log

<!-- write every week your progress here -->

### Week 6 - 12 May

### Week 7 - 19 May

### Week 20 - 26 May


## Reference links

<!-- Fill in with appropriate links and link titles -->

[Tutorial 1](https://www.youtube.com/watch?v=wdgULBpRoXk&t=1s&ab_channel=BenEater)

[Article 1](https://www.explainthatstuff.com/induction-motors.html)

[Link title](https://projecthub.arduino.cc/)
