# Le Guide Ultime de la Programmation Embarquée
### Un compagnon complet pour la *Piscine Embedded* (bare-metal AVR / ATmega328P)

> **À qui s'adresse ce guide :** à quelqu'un qui connaît un peu de C (pointeurs, fonctions, boucles, `Makefile`s) mais qui n'a peu ou pas de bases en électronique, en microcontrôleurs ou en programmation matérielle bas niveau.
>
> **Ce que vous y trouverez :** tout ce que vous *devez* savoir, *devriez* savoir et *pourriez* avoir envie de savoir pendant les deux semaines de la piscine : Flashing, Timers, UART, RGB, Interruptions (interrupts), Analogique (ADC), I2C, EEPROM, SPI, extenseurs d'E/S (I/O expanders) et afficheurs 7 segments, plus les deux rushes.
>
> **Comment le lire :** la Partie I (Fondamentaux) contient les prérequis. Lisez-la *avant* le Day 00, ou au moins survolez-la et revenez-y quand quelque chose devient flou. La Partie II suit la piscine jour par jour. La Partie III couvre les compétences utilisées au quotidien (architecture, débogage, etc.). Les annexes sont des antisèches (cheat sheets) à garder ouvertes pendant que vous codez.
>
> **Note sur la terminologie :** l'embarqué se pratique largement en anglais (datasheets, forums, noms de registres). Ce guide traduit les termes techniques et indique l'original anglais entre parenthèses à la première occurrence, par exemple « résistance de tirage (pull-up resistor) ». Quand l'usage français courant garde le mot anglais (timer, buffer, bootloader…), le guide le garde aussi.

---

## Un mot de prudence avant de commencer

La piscine utilise très probablement un microcontrôleur **ATmega328P** (la puce de l'Arduino Uno) sur une carte de développement (devboard) spécifique, cadencé à **16 MHz**. On le programme en **C avec avr-gcc et avrdude**, et **sans la bibliothèque Arduino**. Tout ce guide cible cette puce.

Cependant :

- **Le câblage exact de votre carte** (quelle LED est sur quelle broche, quel capteur répond à quelle adresse I2C) **peut différer** de ce qui est écrit ici. Chaque fois que ce guide dit « sur la devboard typique de 42… », **vérifiez avec le schéma électrique (schematic) qui vous est fourni.** Le schéma et la datasheet (fiche technique) font toujours foi. Pas ce guide.
- **Les exercices exacts** peuvent changer d'une année à l'autre. Ceux décrits ici sont *représentatifs* : ce sont ceux que vous rencontrerez le plus probablement, et ils constituent un bon entraînement dans tous les cas.
- Les sujets vont probablement **restreindre ce que vous avez le droit d'utiliser** : souvent seulement `<avr/io.h>`, parfois `<util/delay.h>` ou `<avr/interrupt.h>`, et jamais les fonctions Arduino (`digitalWrite`, `Serial.print`, etc.). Lisez attentivement les règles de chaque sujet.

---

# Table des matières

**Partie I : Fondamentaux**
1. [Qu'est-ce que la programmation embarquée / bare-metal ?](#1-quest-ce-que-la-programmation-embarquée--bare-metal-)
2. [Nombres, bits et opérations bit à bit](#2-nombres-bits-et-opérations-bit-à-bit)
3. [Juste assez d'électronique](#3-juste-assez-délectronique)
4. [À l'intérieur de l'ATmega328P](#4-à-lintérieur-de-latmega328p)
5. [Comment lire une datasheet (et un schéma électrique)](#5-comment-lire-une-datasheet-et-un-schéma-électrique)
6. [La devboard](#6-la-devboard)
7. [Le C pour microcontrôleurs](#7-le-c-pour-microcontrôleurs)
8. [La chaîne de compilation (toolchain) : de `main.c` à la puce qui tourne](#8-la-chaîne-de-compilation-toolchain--de-mainc-à-la-puce-qui-tourne)
9. [Installer votre environnement](#9-installer-votre-environnement)

**Partie II : La piscine, jour par jour**

10. [Day 00 : Flashing & GPIO](#10-day-00--flashing--gpio)
11. [Day 01 : Timers](#11-day-01--timers)
12. [Day 02 : UART](#12-day-02--uart)
13. [Day 03 : RGB](#13-day-03--rgb)
14. [Day 04 : Interruptions](#14-day-04--interruptions)
15. [Rush 00 : Multijoueur](#15-rush-00--multijoueur)
16. [Day 05 : Analogique (ADC)](#16-day-05--analogique-adc)
17. [Day 06 : I2C](#17-day-06--i2c)
18. [Day 07 : EEPROM](#18-day-07--eeprom)
19. [Day 08 : SPI](#19-day-08--spi)
20. [Day 09 : Extenseur d'E/S & afficheur 7 segments](#20-day-09--extenseur-des--afficheur-7-segments)
21. [Rush 01 : Final Boss](#21-rush-01--final-boss)

**Partie III : Compétences transversales**

22. [Architecturer un firmware](#22-architecturer-un-firmware)
23. [Déboguer du code embarqué](#23-déboguer-du-code-embarqué)
24. [Consommation, modes veille et watchdog](#24-consommation-modes-veille-et-watchdog)
25. [Un aperçu de l'assembleur AVR](#25-un-aperçu-de-lassembleur-avr)
26. [Aller plus loin après la piscine](#26-aller-plus-loin-après-la-piscine)

**Annexes**

- [A. Antisèche des registres](#annexe-a--antisèche-des-registres)
- [B. Formulaire](#annexe-b--formulaire)
- [C. Table ASCII & séquences d'échappement du terminal](#annexe-c--table-ascii--séquences-déchappement-du-terminal)
- [D. Extraits de code réutilisables](#annexe-d--extraits-de-code-réutilisables)
- [E. Checklist des pièges](#annexe-e--checklist-des-pièges)
- [F. Glossaire](#annexe-f--glossaire)
- [G. Ressources](#annexe-g--ressources)
- [H. Se préparer et survivre à la piscine](#annexe-h--se-préparer-et-survivre-à-la-piscine)

---

# PARTIE I : FONDAMENTAUX

---

## 1. Qu'est-ce que la programmation embarquée / bare-metal ?

### 1.1 Les systèmes embarqués

Un **système embarqué** (embedded system) est un ordinateur intégré *dans* un appareil pour y accomplir une tâche précise. Par exemple : votre micro-ondes, un lave-linge, le calculateur d'ABS d'une voiture, une télécommande, une montre connectée, un pacemaker, un clavier. Il y en a partout, bien plus que de PC et de téléphones.

Leurs points communs :

| Caractéristique | Ce que cela implique pour vous |
|---|---|
| **Fonction dédiée** | Le programme fait une seule chose, en boucle, de la mise sous tension à l'extinction. |
| **Ressources limitées** | Des kilo-octets de mémoire, pas des giga-octets. Des MHz, pas des GHz. |
| **Interaction directe avec le monde physique** | Le programme lit des boutons et des capteurs, et pilote des LEDs et des moteurs. |
| **Contraintes temps réel (real-time)** | Parfois, « juste mais en retard » veut dire « faux » (pensez à un airbag). |
| **Faible consommation / faible coût** | Les puces coûtent de quelques centimes à quelques euros. Certaines tournent des années sur une pile bouton. |

### 1.2 Microcontrôleur vs microprocesseur

- Un **microprocesseur** (le CPU de votre ordinateur portable) n'est *que* un processeur. La RAM, le stockage, les contrôleurs USB, etc. sont des puces séparées.
- Un **microcontrôleur** (microcontroller, MCU) est un *petit ordinateur complet sur une seule puce* : CPU + mémoire flash (stockage du programme) + RAM + périphériques (timers, ports série, convertisseurs analogique-numérique, etc.).

L'ATmega328P que vous allez utiliser est un microcontrôleur : 32 Ko de flash, 2 Ko de RAM, 1 Ko d'EEPROM, à 16 MHz. À titre de comparaison, un téléphone moderne a environ **4 millions de fois** plus de RAM.

### 1.3 Ce que signifie « bare-metal »

Sur votre PC, votre programme C s'exécute **au-dessus d'un système d'exploitation** (Linux). Quand vous appelez `write(1, "hi", 2)`, c'est le noyau (kernel) qui parle au matériel à votre place. Vous ne touchez jamais directement le matériel, et l'OS protège la machine contre vos bugs.

**Bare-metal** (littéralement « métal nu ») signifie **aucun système d'exploitation**. Votre code est le *seul* code qui tourne sur la puce. Il n'y a :

- pas de `printf` vers un écran (il n'y a pas d'écran !)
- pas de `malloc` en pratique (vous *pourriez*, mais il ne faut pas)
- pas de fichiers, pas de processus, pas de threads
- pas de segfault : écrire à une mauvaise adresse fait silencieusement *quelque chose*, parfois quelque chose de grave
- pas de « retour de `main` » : où retournerait-on ?

À la place, **vous contrôlez le matériel directement en lisant et en écrivant à des adresses mémoire spéciales appelées registres (registers)**. Par exemple, écrire la valeur `0x01` à l'adresse `0x25` allume une LED. C'est l'idée centrale de toute cette piscine.

```
  Programme sur PC                   Programme bare-metal
  ────────────────                   ────────────────────
  votre code                         votre code
     │                                  │
  bibliothèque C (libc)                 │  (petit sous-ensemble de libc, optionnel)
     │                                  │
  Système d'exploitation (noyau,        │
  pilotes)                              │
     │                                  │
  Matériel                           Matériel  ← vous y écrivez directement
```

### 1.4 Pourquoi ne pas simplement utiliser Arduino ?

Arduino est un framework accessible qui cache le matériel : `digitalWrite(13, HIGH)` au lieu de `PORTB |= (1 << PB5)`. C'est parfait pour prototyper, mais :

- il cache *comment les choses fonctionnent réellement*, et c'est justement ce que vous êtes venu apprendre ;
- il est plus lent et plus gros : `digitalWrite` prend ~50 cycles CPU, alors qu'une écriture directe dans un registre en prend 1 ou 2 ;
- en entreprise, vous travaillerez sur des puces sans couche Arduino, avec une datasheet de 500 pages et rien d'autre.

La piscine vous apprend à passer **de la datasheet à un code qui fonctionne**. Une fois que vous savez le faire sur un ATmega328P, vous saurez le faire sur n'importe quel microcontrôleur.

### 1.5 La forme générale de tout programme bare-metal

```c
#include <avr/io.h>

int main(void)
{
    /* 1. Initialisation : configurer le matériel une seule fois */
    DDRB |= (1 << PB0);     // mettre la broche PB0 en sortie

    /* 2. Boucle infinie : faire le travail pour toujours */
    while (1)
    {
        /* lire les entrées, calculer, écrire les sorties */
    }
    /* jamais atteint */
}
```

**`main` ne retourne jamais.** Si c'était le cas, le code de démarrage d'avr-libc placerait le CPU dans une boucle infinie, interruptions désactivées. La puce serait de fait figée.

---

## 2. Nombres, bits et opérations bit à bit

C'est **le** prérequis le plus important. Vous allez manipuler des bits individuels toute la journée, tous les jours. Prenez le temps de maîtriser cette section.

### 2.1 Bits et octets

- Un **bit** est un chiffre binaire : `0` ou `1`. Électriquement : 0 V contre 5 V (ou 3,3 V).
- Un **octet** (byte) fait 8 bits. L'ATmega328P est un microcontrôleur **8 bits** : il traite nativement 8 bits à la fois.
- Un **quartet** (nibble) fait 4 bits (un demi-octet). Un chiffre hexadécimal = un quartet.

Les bits d'un octet sont numérotés **de 0 (le plus à droite, poids faible) à 7 (le plus à gauche, poids fort)** :

```
 numéro du bit :  7   6   5   4   3   2   1   0
 poids :        128  64  32  16   8   4   2   1
                MSB                         LSB
```

- **LSB** : Least Significant Bit, bit de poids faible (bit 0)
- **MSB** : Most Significant Bit, bit de poids fort (bit 7 dans un octet)

### 2.2 Binaire, décimal, hexadécimal

Un même nombre peut s'écrire dans plusieurs bases :

| Décimal | Binaire | Hexadécimal |
|---:|---:|---:|
| 0 | `0000` | `0x0` |
| 1 | `0001` | `0x1` |
| 2 | `0010` | `0x2` |
| 3 | `0011` | `0x3` |
| 4 | `0100` | `0x4` |
| 5 | `0101` | `0x5` |
| 6 | `0110` | `0x6` |
| 7 | `0111` | `0x7` |
| 8 | `1000` | `0x8` |
| 9 | `1001` | `0x9` |
| 10 | `1010` | `0xA` |
| 11 | `1011` | `0xB` |
| 12 | `1100` | `0xC` |
| 13 | `1101` | `0xD` |
| 14 | `1110` | `0xE` |
| 15 | `1111` | `0xF` |

**Apprenez ce tableau par cœur.** Il permet de convertir instantanément entre binaire et hexa, car chaque chiffre hexadécimal correspond exactement à 4 bits :

```
  0xA5  =  1010 0101
            A    5

  0x3C  =  0011 1100
            3    C
```

**Conversion binaire → décimal :** additionnez les poids des bits à 1.

```
  0b01101001
    ││││││││
    0×128 + 1×64 + 1×32 + 0×16 + 1×8 + 0×4 + 0×2 + 1×1 = 64+32+8+1 = 105
```

**Conversion décimal → binaire :** divisez successivement par 2 et lisez les restes de bas en haut.

```
  105 / 2 = 52  r 1   ← LSB
   52 / 2 = 26  r 0
   26 / 2 = 13  r 0
   13 / 2 =  6  r 1
    6 / 2 =  3  r 0
    3 / 2 =  1  r 1
    1 / 2 =  0  r 1   ← MSB
  → 1101001 = 0b01101001
```

**Conversion hexa → décimal :** chaque chiffre vaut 16× celui de sa droite. `0x2F = 2×16 + 15 = 47`.

En C, on peut écrire des littéraux (literals) dans n'importe quelle base :

```c
uint8_t a = 105;          // décimal
uint8_t b = 0x69;         // hexadécimal
uint8_t c = 0b01101001;   // binaire (extension GCC, standard depuis C23)
// a == b == c
```

**Pourquoi l'hexa ?** Parce qu'il est compact *et* correspond directement aux bits. Les datasheets l'utilisent partout.

### 2.3 Puissances de 2 utiles

| 2ⁿ | valeur | remarque |
|---|---:|---|
| 2⁴ | 16 | valeurs d'un quartet |
| 2⁸ | 256 | valeurs d'un octet (0–255) |
| 2¹⁰ | 1024 | « 1 K », résolution de l'ADC de l'ATmega328P |
| 2¹⁶ | 65 536 | valeurs sur 16 bits (0–65 535) |
| 2²⁰ | 1 048 576 | utilisé par le capteur AHT20 (Day 06) |
| 2³² | 4 294 967 296 | valeurs sur 32 bits |

### 2.4 Types entiers : utilisez toujours `<stdint.h>`

Sur AVR, `int` fait **16 bits**, et non 32 comme sur votre PC ! Cette différence cause des bugs vicieux. Utilisez toujours des types à taille fixe (fixed-width types) :

| Type | Taille | Plage |
|---|---|---|
| `uint8_t` | 8 bits | 0 … 255 |
| `int8_t` | 8 bits | −128 … 127 |
| `uint16_t` | 16 bits | 0 … 65 535 |
| `int16_t` | 16 bits | −32 768 … 32 767 |
| `uint32_t` | 32 bits | 0 … 4 294 967 295 |
| `int32_t` | 32 bits | −2 147 483 648 … 2 147 483 647 |

```c
int x = 300 * 200;   // sur AVR : int fait 16 bits → 60000 déborde d'un int16_t → n'importe quoi !
uint32_t y = 300UL * 200;  // correct : 60000 (UL = unsigned long = 32 bits sur AVR)
```

**Règle :** préférez le plus petit type qui convient. Un CPU 8 bits traite un `uint8_t` en 1 instruction, un `uint16_t` en environ 2, et un `uint32_t` en environ 4. Les types non signés (unsigned) sont généralement ce qu'il vous faut pour les valeurs matérielles.

### 2.5 Nombres signés : le complément à deux

Un nombre signé sur 8 bits utilise le **complément à deux** (two's complement). Le MSB a un poids *négatif* (−128) :

```
  0b11111111 = −128 + 64+32+16+8+4+2+1 = −1
  0b10000000 = −128
  0b01111111 = 127
```

Pour obtenir l'opposé d'un nombre : inversez tous les bits et ajoutez 1. `5 = 0b00000101` → inversion → `0b11111010` → +1 → `0b11111011 = −5`.

Les mêmes bits peuvent donc signifier des valeurs différentes : `0xFF` vaut `255` en `uint8_t` mais `−1` en `int8_t`. C'est important quand vous lisez des données de capteurs.

### 2.6 Débordement et rebouclage

L'arithmétique non signée **reboucle** (wrap-around), modulo 2ⁿ :

```c
uint8_t x = 255;
x++;          // x == 0
uint8_t y = 0;
y--;          // y == 255
```

Cela peut être un bug ou une fonctionnalité. Un compteur 8 bits reboucle naturellement de 255 à 0, et les timers matériels font exactement la même chose.

**Le débordement (overflow) signé est un comportement indéfini (undefined behavior) en C**, ce qui signifie que le compilateur peut supposer qu'il n'arrive jamais. Évitez-le.

### 2.7 Opérateurs bit à bit

| Opérateur | Nom | Règle (bit par bit) |
|---|---|---|
| `a & b` | ET (AND) | 1 seulement si les **deux** valent 1 |
| `a \| b` | OU (OR) | 1 si **au moins un** vaut 1 |
| `a ^ b` | OU exclusif (XOR) | 1 s'ils sont **différents** |
| `~a` | NON (NOT) | inverse chaque bit |
| `a << n` | décalage à gauche (left shift) | décale les bits de n vers la gauche ; complète avec des 0 (×2ⁿ) |
| `a >> n` | décalage à droite (right shift) | décale les bits de n vers la droite (÷2ⁿ pour les non signés) |

Exemples sur des octets :

```
      1100 1010        1100 1010        1100 1010      ~ 1100 1010
    & 1111 0000      | 0000 1111      ^ 1111 0000      = 0011 0101
    = 1100 0000      = 1100 1111      = 0011 1010

    0000 0001 << 3  = 0000 1000
    1011 0000 >> 4  = 0000 1011
```

**Ne confondez pas** les opérateurs bit à bit `&`, `|`, `~` avec les opérateurs logiques `&&`, `||`, `!` :

```c
0x0F & 0xF0   // == 0x00  (bit à bit : aucun bit en commun)
0x0F && 0xF0  // == 1     (logique : les deux sont non nuls → vrai)
~0x01         // == 0xFE... (chaque bit inversé ; attention : le résultat est un int !)
!0x01         // == 0     (NON logique)
```

### 2.8 Les quatre idiomes sacrés de manipulation de bits

Vous les écrirez des centaines de fois. `REG` est un registre (ou n'importe quelle variable), `n` un numéro de bit.

```c
REG |=  (1 << n);    // METTRE le bit n à 1 (SET)      (autres bits inchangés)
REG &= ~(1 << n);    // METTRE le bit n à 0 (CLEAR)    (autres bits inchangés)
REG ^=  (1 << n);    // INVERSER le bit n (TOGGLE)     (autres bits inchangés)
if (REG & (1 << n))  // TESTER si le bit n vaut 1 (TEST)
```

Pourquoi ça marche ? Prenons `REG = 0b10100000` et `n = 2` :

```
  (1 << 2)          = 0b00000100          ← le « masque » (mask) : un seul 1 en position 2

  SET :    0b10100000
         | 0b00000100
         = 0b10100100   le bit 2 vaut maintenant 1, les autres inchangés (x | 0 = x, x | 1 = 1)

  CLEAR : ~(1 << 2) = 0b11111011
           0b10100100
         & 0b11111011
         = 0b10100000   le bit 2 vaut maintenant 0, les autres inchangés (x & 1 = x, x & 0 = 0)

  TOGGLE : 0b10100000
         ^ 0b00000100
         = 0b10100100   le bit 2 est inversé (x ^ 0 = x, x ^ 1 = !x)

  TEST :   0b10100100
         & 0b00000100
         = 0b00000100   non nul → vrai
```

**Plusieurs bits à la fois :**

```c
REG |=  (1 << 3) | (1 << 5);      // mettre à 1 les bits 3 et 5
REG &= ~((1 << 3) | (1 << 5));    // mettre à 0 les bits 3 et 5  (attention aux parenthèses !)
```

**La macro `_BV()` :** avr-libc définit `#define _BV(bit) (1 << (bit))` (« bit value »). Ainsi `PORTB |= _BV(PB0);` équivaut à `PORTB |= (1 << PB0);`. Utilisez la forme acceptée par votre sujet.

**Constantes nommées pour les bits :** `<avr/io.h>` définit des noms pour les positions de bits. `PB0` vaut simplement `0`, `PB5` vaut `5`, `TXEN0` vaut `3`, etc. Ce ne sont que des nombres. `(1 << TXEN0)` signifie « un masque pour le bit TXEN0 ».

### 2.9 Masques, champs et extraction

Les registres regroupent souvent plusieurs **champs** (fields) dans un seul octet. Exemple : un registre hypothétique dont les bits 2..0 forment un champ « prescaler » :

```
  bit :  7   6   5   4   3   2   1   0
       [ A | B | - | - | - |  PRESC[2:0] ]
```

Pour **écrire** un champ sans perturber les autres :

```c
#define PRESC_MASK 0x07        // 0b00000111
REG = (REG & ~PRESC_MASK) | (value & PRESC_MASK);
//     └─ efface le champ ─┘  └─ insère la valeur ─┘
```

Pour **lire** un champ situé à la position `p` et de largeur `w` :

```c
uint8_t field = (REG >> p) & ((1 << w) - 1);
// ex. les bits 7..3 de TWSR : (TWSR >> 3) & 0x1F   — ou simplement TWSR & 0xF8
```

**Découper / assembler des octets :**

```c
uint16_t word = 0xABCD;
uint8_t high = word >> 8;       // 0xAB
uint8_t low  = word & 0xFF;     // 0xCD
uint16_t back = ((uint16_t)high << 8) | low;   // 0xABCD — caster AVANT de décaler !

// quartets d'un octet
uint8_t b = 0x5E;
uint8_t hi_nib = b >> 4;     // 0x5
uint8_t lo_nib = b & 0x0F;   // 0xE
```

> ⚠️ `high << 8` où `high` est un `uint8_t` : le C promeut `high` en `int` (16 bits sur AVR) avant le décalage, donc ça fonctionne ici. Pour `<< 16` ou plus, vous **devez** d'abord caster en `uint32_t`, par exemple `((uint32_t)b << 16)`, sinon les bits « tombent » hors du type.

### 2.10 Les décalages comme multiplication/division rapides

```c
x << 1   // x * 2
x << 3   // x * 8
x >> 2   // x / 4  (non signé seulement ; tronque)
```

Le compilateur le fait déjà pour vous quand vous écrivez `x * 8`, donc écrivez ce qui est le plus lisible. Il reste utile de *reconnaître* ces motifs dans les datasheets et dans le code des autres.

### 2.11 Exercices (à faire sur papier)

1. Convertissez `0xC3` en binaire et en décimal. *(→ `1100 0011`, 195)*
2. Que vaut `0x5A & 0x0F` ? `0x5A | 0x80` ? `0x5A ^ 0xFF` ? *(→ `0x0A`, `0xDA`, `0xA5`)*
3. Écrivez l'expression C qui met à 1 les bits 1 et 4 de `PORTD` sans toucher aux autres.
4. `uint8_t v = 0b10110110;` Extrayez les bits 5..3 sous forme de nombre. *(→ `(v >> 3) & 0x7` = `0b110` = 6)*
5. Comptez en binaire de 0 à 15 sur 4 doigts. (C'est littéralement le dernier exercice du Day 00, avec des LEDs.)

---

## 3. Juste assez d'électronique

Vous ne construirez pas de circuits pendant cette piscine : la carte est déjà faite. Mais vous **devez** comprendre à quoi les broches sont reliées, sinon le code n'a aucun sens. Cette section couvre le minimum.

### 3.1 Tension, courant, résistance

Imaginez l'électricité comme de l'eau dans des tuyaux :

| Grandeur | Symbole | Unité | Analogie hydraulique |
|---|---|---|---|
| **Tension** (voltage) | U ou V | volt (V) | différence de *pression* d'eau entre deux points |
| **Courant** (current) | I | ampère (A) | *débit* d'eau (combien passe par seconde) |
| **Résistance** (resistance) | R | ohm (Ω) | à quel point le tuyau est *étroit* |

- Une tension se mesure toujours **entre deux points**. Quand on dit « la broche est à 5 V », on veut dire « 5 V par rapport à la **masse** (GND, ground, 0 V) ».
- Le courant circule dans une **boucle fermée** : du + à travers le circuit, puis retour au − (GND).

**La loi d'Ohm**, la seule formule à connaître absolument :

```
    U = R × I        (volts = ohms × ampères)
    I = U / R
    R = U / I
```

Exemple : 5 V aux bornes de 1 kΩ (1000 Ω) → I = 5 / 1000 = 0,005 A = **5 mA**.

**Puissance :** P = U × I (watts). Une broche qui fournit 20 mA sous 5 V → 0,1 W.

Préfixes courants : m (milli, 10⁻³), µ (micro, 10⁻⁶), n (nano, 10⁻⁹), p (pico, 10⁻¹²), k (kilo, 10³), M (méga, 10⁶).

### 3.2 Niveaux logiques numériques

Une broche de microcontrôleur utilisée en **numérique** (digital) ne connaît que deux états :

- **HAUT / HIGH / 1** : proche de VCC (5 V sur votre carte)
- **BAS / LOW / 0** : proche de GND (0 V)

En entrée, la puce a besoin de niveaux clairs. Pour l'ATmega328P sous 5 V (voir la section « Electrical Characteristics » de la datasheet) :
- en dessous d'environ 0,3×VCC (1,5 V) → lu comme 0
- au-dessus d'environ 0,6×VCC (3 V) → lu comme 1
- entre les deux → **indéfini**, peut être lu comme l'un ou l'autre

C'est pourquoi une broche d'entrée **reliée à rien** (on dit « flottante », floating) pose problème : elle capte le bruit électrique et lit des valeurs aléatoires. D'où les résistances de tirage (pull-up, §3.5).

### 3.3 Les LEDs

Une **LED** (Light-Emitting Diode, diode électroluminescente) s'allume quand le courant la traverse dans **un seul sens** : de l'**anode (+)** vers la **cathode (−)**. La cathode est en général la patte la plus courte et le côté plat du boîtier.

- Une LED a besoin d'environ 1,8 à 3,3 V à ses bornes (sa « tension de seuil », forward voltage, Vf, qui dépend de la couleur) et typiquement de 2 à 20 mA.
- Une LED **doit** avoir une **résistance en série** pour limiter le courant, sinon elle (ou la broche) grille.

Câblage typique (**actif à l'état haut**, active-high : broche HIGH = LED allumée) :

```
   broche MCU ──[ R 330Ω ]──►|── GND
                            LED
```

Calcul de la résistance : broche à 5 V, LED rouge Vf ≈ 2 V, objectif 10 mA :
R = (5 − 2) / 0,010 = 300 Ω → on prend 330 Ω (une valeur normalisée).

Autre câblage (**actif à l'état bas**, active-low : broche LOW = LED allumée) :

```
   VCC ──[ R ]──►|── broche MCU
               LED
```

Ici la LED s'allume quand la broche est à 0 V, car le courant va de VCC vers la broche. **Vérifiez toujours quel câblage utilise votre carte** (regardez le schéma).

### 3.4 Les boutons (boutons-poussoirs / interrupteurs)

Un bouton-poussoir (push-button) n'est que deux contacts métalliques qui se touchent quand on appuie. Seul, il ne produit aucune tension : il *relie* seulement deux points.

Câblage typique sur un MCU :

```
             VCC
              │
             [R] pull-up (ex. 10 kΩ, ou interne)
              │
 broche MCU ──┤
              │
             ─┴─  bouton
             ─┬─
              │
             GND
```

- **Relâché** : la broche est reliée à VCC à travers la résistance → lit **1**.
- **Appuyé** : la broche est reliée directement à GND → lit **0**. Le courant va de VCC → R → bouton → GND. La résistance le limite à 5 V / 10 kΩ = 0,5 mA, ce qui est inoffensif.

On dit que c'est **actif à l'état bas** (active-low) : appuyé = 0. C'est le câblage de bouton le plus courant, donc votre code teste un `0` pour détecter un appui :

```c
if (!(PIND & (1 << PD2)))   // PD2 lit 0 → bouton appuyé
```

### 3.5 Résistances de tirage : pull-up et pull-down

Une résistance de **pull-up** (tirage vers le haut) « tire » faiblement une ligne vers VCC quand rien d'autre ne la pilote. Une **pull-down** la tire vers GND. Elles donnent un état par défaut défini à une ligne qui serait sinon flottante.

« Faiblement » signifie que n'importe quel pilote actif (un bouton vers GND, une autre puce qui tire à l'état bas) l'emporte facilement, car la résistance limite le courant.

L'ATmega328P possède des **pull-ups internes** (~20–50 kΩ) sur chaque broche d'E/S, que l'on active par logiciel (§10). Il n'y a pas de pull-down interne.

Vous retrouverez les pull-ups avec l'**I2C** (Day 06), où elles sont indispensables.

### 3.6 Les rebonds des interrupteurs

Quand les contacts d'un bouton mécanique se ferment, ils **rebondissent** (bounce) physiquement pendant quelques millisecondes ; le signal ressemble alors à ceci :

```
  relâché                     appuyé
  ────────┐ ┌┐ ┌─┐┌┐
          │ ││ │ │││
          └─┘└─┘ └┘└──────────────────
          |<-- ~1–10 ms -->|
```

Votre MCU est rapide (16 millions d'instructions par seconde) : il voit chaque rebond comme un appui distinct. Un appui devient 5 « appuis ». C'est pourquoi l'exercice du Day 00 « inverser une LED à chaque appui » semble bugué au début. Les solutions sont au §10.7.

### 3.7 Les condensateurs

Un **condensateur** (capacitor) stocke une petite quantité de charge, comme une toute petite batterie rechargeable très rapide. Ses usages sur votre carte :
- **Découplage** (decoupling, 100 nF près de l'alimentation de chaque puce) : absorbe le bruit d'alimentation.
- **Filtrage** (sur AREF, sur les signaux analogiques) : lisse le bruit.
- **Anti-rebond** (debouncing, parfois sur les boutons : un filtre RC).
- **Condensateurs de charge du quartz** (~22 pF à côté du quartz 16 MHz).

Il vous suffit surtout de savoir qu'ils sont là et pourquoi.

### 3.8 Les ponts diviseurs de tension (essentiel pour le Day 05)

Deux résistances en série entre VCC et GND. Le point milieu est à une tension intermédiaire :

```
   VCC (5V)
    │
   [R1]
    │
    ├──── Vout = VCC × R2 / (R1 + R2)
    │
   [R2]
    │
   GND
```

Exemple : R1 = R2 = 10 kΩ → Vout = 2,5 V.

- Un **potentiomètre** est un diviseur variable : une résistance avec un contact central mobile. Tourner le bouton déplace Vout entre 0 et VCC.
- Une **LDR** (Light-Dependent Resistor, photorésistance) change de résistance avec la lumière. Associée à une résistance fixe, le Vout du diviseur varie avec la lumière.
- Une **thermistance CTN** (NTC thermistor, Negative Temperature Coefficient : coefficient de température négatif) est une résistance dont la valeur baisse quand la température monte. Même idée : un diviseur transforme la température en tension.

L'ADC (Day 05) mesure ensuite cette tension.

### 3.9 Transistors et sorties à drain ouvert (notions)

Un **transistor** est un interrupteur commandé électriquement. Les broches de sortie du MCU contiennent des transistors qui relient la broche soit à VCC (sortie HIGH), soit à GND (sortie LOW). C'est une sortie **push-pull**.

Une sortie à **drain ouvert** (open-drain, ou collecteur ouvert, open-collector) peut *seulement* tirer vers GND ou relâcher la ligne (haute impédance, « flottante »). Pour obtenir un niveau HIGH, il faut une pull-up externe. **L'I2C utilise des lignes à drain ouvert**, ce qui permet à plusieurs composants de partager un fil sans court-circuit (Day 06).

```
  Sortie push-pull :             Sortie open-drain :
       VCC                            VCC
        │                              │
      [SW1]  ← fermé pour HIGH        [R] pull-up (externe)
        ├───── broche                  ├───── broche/ligne du bus
      [SW2]  ← fermé pour LOW        [SW]  ← fermé = LOW, ouvert = tiré à HIGH
        │                              │
       GND                            GND
```

Ne reliez **jamais** ensemble deux sorties push-pull dont l'une pilote HIGH et l'autre LOW : c'est un court-circuit.

### 3.10 Haute impédance (Hi-Z)

Une broche en entrée présente une résistance très élevée (des mégohms). Elle ne consomme presque aucun courant et se contente « d'écouter ». On dit qu'elle est en **haute impédance** (high impedance) ou **trois états** (tri-state). Une broche configurée en entrée sans pull-up est en Hi-Z (et flotte si rien n'y est relié).

### 3.11 Symboles de schéma que vous rencontrerez

```
  Résistance :   ──[ R ]──   ou  ──/\/\/──
  Condensateur : ──| |──
  LED :          ──►|──     (la flèche = sens du courant, anode → cathode)
  Diode :        ──►|──
  Bouton :       ──o  o──  avec une petite barre au-dessus
  GND :          ─┴─ ou ⏚  (trois traits décroissants)
  VCC :          une flèche ou une barre vers le haut, étiquetée VCC / 5V / +5V
  Quartz :       ──|▯|──
  Étiquettes de net (net labels) : un fil terminé par un nom (ex. « SDA ») est relié
                 à tous les autres fils portant le même nom, même sans trait dessiné.
```

Les **étiquettes de net** (net labels) sont importantes : sur la plupart des schémas, les broches du MCU ne sont pas reliées par des traits aux LEDs. À la place, la broche du MCU porte l'étiquette `PB0` et le fil de la LED aussi. Même nom = électriquement relié.

### 3.12 Limites de courant : ne grillez pas votre puce

D'après la datasheet de l'ATmega328P (Absolute Maximum Ratings, valeurs maximales absolues) :
- 40 mA max par broche d'E/S (maximum absolu ; visez ≤ 20 mA)
- 200 mA max au total à travers VCC/GND

C'est moins important pour vous (la carte est conçue correctement), mais **ne court-circuitez jamais directement à GND une broche configurée en sortie HIGH**.

---

## 4. À l'intérieur de l'ATmega328P

### 4.1 Carte d'identité

| Caractéristique | Valeur |
|---|---|
| Architecture | AVR, RISC 8 bits, Harvard |
| Horloge (clock) | jusqu'à 20 MHz ; **quartz externe de 16 MHz** sur votre carte |
| Flash (programme) | 32 Ko (dont 0,5 Ko occupés par le bootloader sur les cartes type Arduino) |
| SRAM (données) | 2 Ko |
| EEPROM | 1 Ko |
| Registres généraux | 32 × 8 bits (`r0` à `r31`) |
| Broches d'E/S (I/O pins) | 23 (ports B, C, D) |
| Timers | Timer0 (8 bits), Timer1 (16 bits), Timer2 (8 bits, peut être asynchrone) |
| Canaux PWM | 6 |
| ADC | 10 bits, 6 canaux (8 sur les boîtiers TQFP/QFN) + capteur de température interne |
| Série | 1 × USART, 1 × SPI, 1 × TWI (I2C) |
| Autres | Watchdog, comparateur analogique, interruptions externes & de changement d'état de broche, modes veille |
| Tension de fonctionnement | 1,8–5,5 V (5 V sur votre carte) |
| La plupart des instructions | s'exécutent en **1 cycle d'horloge** → ~16 MIPS à 16 MHz |

### 4.2 Schéma bloc (simplifié)

```
                         ┌─────────────────────────────────────────────┐
     quartz 16 MHz ────► │  Système d'horloge                          │
                         │     │                                       │
                         │  ┌──▼──────┐   ┌──────────┐   ┌──────────┐  │
                         │  │  CPU    │◄─►│  Flash   │   │  EEPROM  │  │
                         │  │  AVR    │   │  32 Ko   │   │  1 Ko    │  │
                         │  │ r0..r31 │   │(programme│   │          │  │
                         │  └──┬──────┘   └──────────┘   └────┬─────┘  │
                         │     │  bus de données 8 bits       │        │
                         │ ════╪══════╪═══════╪═══════╪═══════╪══════  │
                         │     │      │       │       │       │        │
                         │  ┌──▼──┐ ┌─▼────┐ ┌▼────┐ ┌▼────┐ ┌▼─────┐  │
                         │  │SRAM │ │Timers│ │USART│ │ SPI │ │ TWI  │  │
                         │  │ 2 Ko│ │0,1,2 │ │     │ │     │ │(I2C) │  │
                         │  └─────┘ └──────┘ └─────┘ └─────┘ └──────┘  │
                         │  ┌─────┐ ┌──────┐ ┌─────────────────────┐   │
                         │  │ ADC │ │ WDT  │ │ GPIO ports B, C, D  │   │
                         │  └─────┘ └──────┘ └─────────────────────┘   │
                         └────────────────────────┬────────────────────┘
                                                  │
                                        23 broches physiques
```

Le CPU communique avec chaque périphérique via des **registres** placés en mémoire (mappés en mémoire). Configurer un timer, envoyer un octet en UART, lire une broche : tout se résume à lire et écrire des registres.

### 4.3 Architecture Harvard : deux mémoires séparées

L'AVR utilise une **architecture Harvard** : la mémoire programme (flash) et la mémoire de données (SRAM) sont des **espaces d'adressage séparés**, avec des bus séparés.

- **Flash** : contient votre programme compilé (et des données constantes si vous le demandez). Elle survit à la coupure de courant. Elle peut être réécrite environ 10 000 fois, normalement seulement par le programmateur ou le bootloader.
- **SRAM** : contient les variables, la pile (stack), etc. Elle est perdue à l'extinction. Seulement 2 Ko !
- **EEPROM** : une troisième mémoire, séparée, pour de petites données persistantes (réglages, meilleurs scores). Accessible via des registres (Day 07).

Conséquence : l'adresse `0x0100` en flash et l'adresse `0x0100` en SRAM sont *deux choses différentes*. Un pointeur normal (`char *`) pointe dans la SRAM. Lire la flash nécessite des instructions spéciales (`LPM`), encapsulées par `pgm_read_byte()` dans `<avr/pgmspace.h>`.

### 4.4 La carte de l'espace de données

C'est de là que vient la magie des « registres qui sont de la mémoire » :

```
  Espace d'adressage des données (côté SRAM)
  ┌────────────────────┐ 0x0000
  │ 32 registres CPU   │         r0..r31
  │ (r0 – r31)         │
  ├────────────────────┤ 0x0020
  │ 64 registres d'E/S │         PINB, DDRB, PORTB, ... (accessibles via IN/OUT, SBI/CBI)
  │                    │
  ├────────────────────┤ 0x0060
  │ 160 registres d'E/S│         TCCR1A, UCSR0A, ADCSRA, TWBR... (seulement via LD/ST)
  │ étendus            │
  ├────────────────────┤ 0x0100
  │                    │
  │   SRAM interne     │  ← .data, .bss (globales), tas (heap) ↑
  │   2048 octets      │
  │                    │  ← pile (stack) ↓ (croît vers le bas depuis le sommet)
  └────────────────────┘ 0x08FF (RAMEND)
```

Donc **`PORTB` est littéralement l'octet à l'adresse de données `0x25`**. Y écrire ne stocke pas une valeur en RAM : cela pilote les broches de sortie. `<avr/io.h>` se contente de donner des noms à ces adresses :

```c
// En gros, ce que fait <avr/io.h> (simplifié) :
#define PORTB (*(volatile uint8_t *)0x25)
#define DDRB  (*(volatile uint8_t *)0x24)
#define PINB  (*(volatile uint8_t *)0x23)
```

Lisez cette déclaration de l'intérieur vers l'extérieur : prendre le nombre `0x25`, le traiter comme un pointeur vers un `volatile uint8_t`, et le déréférencer. Ainsi `PORTB = 1;` écrit 1 à l'adresse 0x25. (`volatile` est expliqué au §7.1.)

Vous pourriez même l'écrire vous-même, et ça marcherait :

```c
*(volatile uint8_t *)0x24 = 0x01;   // DDRB  = 1 → PB0 en sortie
*(volatile uint8_t *)0x25 = 0x01;   // PORTB = 1 → PB0 HIGH → LED allumée (si câblée active-high)
```

> Note : le résumé des registres (register summary) de la datasheet donne deux adresses pour les registres bas, par ex. « 0x05 (0x25) PORTB ». La première est l'**adresse d'E/S** (I/O address, utilisée par les instructions assembleur `IN`/`OUT`), celle entre parenthèses est l'**adresse en mémoire de données** (data memory address, décalée de 0x20). En C, on utilise toujours l'adresse de données, et `<avr/io.h>` s'en charge pour vous.

### 4.5 Registres du CPU et registre d'état (SREG)

Le CPU possède 32 registres généraux de 8 bits (`r0`–`r31`). Le compilateur les gère pour vous. Les paires `r26:r27` (X), `r28:r29` (Y) et `r30:r31` (Z) peuvent servir de pointeurs 16 bits.

**SREG** (Status Register, registre d'état) contient les drapeaux (flags) de la dernière opération :

| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| Nom | **I** | T | H | S | V | N | Z | C |

- **I** : Global Interrupt Enable, autorisation globale des interruptions. `sei()` le met à 1, `cli()` le met à 0 (Day 04).
- C : Carry (retenue), Z : Zero, N : Negative, V : Overflow (débordement), S : Sign (signe), H : Half-carry (demi-retenue), T : bit de stockage/copie.

Seul **I** vous concerne directement.

Il y a aussi le **pointeur de pile** (stack pointer, SP, 16 bits : SPH:SPL). Il est initialisé à RAMEND (0x08FF) au démarrage par le runtime C. Il contient les adresses de retour et les variables locales.

### 4.6 Brochage (pinout) et noms des ports

Les broches GPIO sont regroupées en **ports** de 8 bits. Chaque broche est nommée `P` + lettre du port + numéro de bit : `PB0`, `PC3`, `PD7`…

- **Port B** : PB0–PB7 (PB6/PB7 sont utilisées par le quartz de 16 MHz → indisponibles)
- **Port C** : PC0–PC6 (PC6 est RESET → indisponible ; il n'y a pas de PC7)
- **Port D** : PD0–PD7 (PD0/PD1 sont RX/TX de l'UART, reliées à la puce USB-série)

Presque toutes les broches ont des **fonctions alternatives** (alternate functions). Quand vous activez un périphérique, il prend le contrôle de la broche. Retenez les principales :

| Broche | Fonctions alternatives | Vous l'utiliserez pour… |
|---|---|---|
| **PB0** | ICP1, CLKO, PCINT0 | LED (devboard typique : D1) |
| **PB1** | **OC1A**, PCINT1 | LED D2, sortie PWM A du Timer1 |
| **PB2** | **SS**, **OC1B**, PCINT2 | LED D3, sélection d'esclave SPI (slave select), sortie PWM B du Timer1 |
| **PB3** | **MOSI**, **OC2A**, PCINT3 | sortie de données SPI, PWM A du Timer2 |
| **PB4** | **MISO**, PCINT4 | LED D4, entrée de données SPI |
| **PB5** | **SCK**, PCINT5 | horloge SPI (la LED « pin 13 » de l'Arduino) |
| PB6/PB7 | XTAL1/XTAL2 | le quartz de 16 MHz (n'y touchez pas) |
| **PC0–PC3** | **ADC0–ADC3**, PCINT8–11 | entrées analogiques (potentiomètre, LDR, CTN…) |
| **PC4** | **ADC4, SDA**, PCINT12 | données I2C |
| **PC5** | **ADC5, SCL**, PCINT13 | horloge I2C |
| PC6 | RESET | bouton reset |
| **PD0** | **RXD**, PCINT16 | réception UART |
| **PD1** | **TXD**, PCINT17 | émission UART |
| **PD2** | **INT0**, PCINT18 | bouton SW1 (interruption externe 0) |
| **PD3** | **INT1**, **OC2B**, PCINT19 | PWM B du Timer2 (bleu de la LED RGB sur la devboard typique) |
| **PD4** | XCK, T0, PCINT20 | bouton SW2 |
| **PD5** | **OC0B**, T1, PCINT21 | PWM B du Timer0 (rouge de la LED RGB) |
| **PD6** | **OC0A**, AIN0, PCINT22 | PWM A du Timer0 (vert de la LED RGB) |
| PD7 | AIN1, PCINT23 | |
| ADC6/ADC7 | (TQFP/QFN seulement) broches uniquement analogiques | |

**Récapitulatif des broches de sortie des timers :**

| Timer | Canal A | Canal B |
|---|---|---|
| Timer0 (8 bits) | OC0A = **PD6** | OC0B = **PD5** |
| Timer1 (16 bits) | OC1A = **PB1** | OC1B = **PB2** |
| Timer2 (8 bits) | OC2A = **PB3** | OC2B = **PD3** |

Ce tableau explique pourquoi la devboard est câblée ainsi. La LED RGB est sur PD3/PD5/PD6 précisément parce que ces broches ont une PWM matérielle.

### 4.7 L'horloge

Tout dans la puce avance au rythme d'une **horloge** (clock) : un signal carré. À 16 MHz, un cycle dure **62,5 ns**. La plupart des instructions prennent un cycle.

Sources d'horloge (sélectionnées par les *fusibles*, fuses, §8.8) :
- oscillateur RC interne 8 MHz (imprécis, ±10 %)
- oscillateur interne 128 kHz (pour le watchdog / la très basse consommation)
- **quartz externe** (crystal, précis à ±0,005 %) ← votre carte, à 16 MHz

**`F_CPU`** est une macro que *vous* définissez (généralement avec `-DF_CPU=16000000UL` dans le Makefile) pour indiquer au code la fréquence d'horloge. Elle ne *règle* pas l'horloge : elle informe seulement `_delay_ms()` et vos calculs de baud rate. Si elle est fausse, vos délais et votre UART seront faux.

### 4.8 Reset et démarrage

À la mise sous tension ou au reset (broche reset, watchdog, brown-out) :
1. Tous les registres d'E/S prennent leur **valeur de reset** (listée dans la datasheet, 0 le plus souvent). Toutes les broches sont donc **des entrées sans pull-up**.
2. Le CPU commence à exécuter à l'adresse flash `0x0000` : le **vecteur de reset** (reset vector). (Ou à l'adresse du bootloader si le fusible BOOTRST est actif, comme sur les cartes Arduino : le bootloader s'exécute d'abord, attend brièvement un nouveau programme sur la liaison série, puis saute en 0x0000.)
3. En `0x0000` se trouve un saut vers le code de démarrage du runtime C (`__init` / crt), qui :
   - met `r1` à zéro (le compilateur suppose r1 == 0) ainsi que SREG
   - initialise le pointeur de pile
   - copie les valeurs initiales des globales initialisées (`.data`) de la flash vers la SRAM
   - met à zéro les globales non initialisées (`.bss`)
   - appelle `main()`

Juste après le vecteur de reset se trouve la **table des vecteurs d'interruption** (interrupt vector table) : une entrée par source d'interruption (Day 04).

---

## 5. Comment lire une datasheet (et un schéma électrique)

Le sujet dit que la piscine enseigne « la lecture de datasheets ». C'est la vraie compétence. Une datasheet (fiche technique) est la documentation officielle d'une puce par son fabricant. Celle de l'ATmega328P fait ~300 pages (le document complet « ATmega48A/PA/88A/PA/168A/PA/328/P » en fait ~660). **On ne la lit pas de bout en bout.** On apprend à *y naviguer*.

### 5.1 Trouver le bon document

Cherchez « ATmega328P datasheet » sur microchip.com (Microchip a racheté Atmel). Il existe plusieurs versions. La plus récente « ATmega328P » (DS40002061) et l'ancienne doc Atmel combinée (Atmel-8271) couvrent le même contenu avec des numéros de page différents. Utilisez celle de vos camarades, pour que les références de pages correspondent.

Pour les composants externes, il vous faudra aussi : **AHT20** (capteur de température/humidité), **PCA9555** (extenseur d'E/S), **APA102** / « SK9822 » (LEDs RGB SPI), probablement **PCF8563** (horloge temps réel), et l'**afficheur 7 segments**.

### 5.2 Structure d'une datasheet de microcontrôleur

1. **Features / overview** (caractéristiques / vue d'ensemble) : premières pages ; résumé de ce que sait faire la puce.
2. **Pin configurations** (configuration des broches) : le schéma de brochage et les *Pin Descriptions*.
3. **Architecture overview** : CPU, mémoires, banc de registres.
4. **Memories** : flash, SRAM, **EEPROM** (avec du code d'exemple !).
5. **System clock and clock options** : fusibles, oscillateurs.
6. **Power management and sleep modes** : gestion de l'énergie et modes veille.
7. **System control and reset** : watchdog, brown-out.
8. **Interrupts** : la **table des vecteurs**.
9. **External interrupts** : INT0/INT1, pin change.
10. **I/O ports** : **DDRx, PORTx, PINx**.
11. **Timer/Counter0, Timer/Counter1, Timer/Counter2** : chacun est un gros chapitre.
12. **SPI**, **USART0**, **TWI** (I2C), **Analog Comparator**, **ADC**.
13. **Memory programming / Boot loader** : fusibles, bits de verrouillage (lock bits).
14. **Electrical characteristics** : tensions, courants, timings.
15. **Register summary** (résumé des registres) : *un tableau qui liste chaque registre et ses bits*. **Mettez cette page en favori.**
16. **Instruction set summary** : résumé du jeu d'instructions.

Chaque chapitre de périphérique suit le même schéma :
- **Overview + block diagram** : ce que fait le périphérique
- **Functional description** : comment il fonctionne, avec modes, formules et chronogrammes
- **Code examples** (en assembleur et en C), *très utiles* (UART, EEPROM, SPI et TWI en ont tous)
- **Register description** : une section par registre, avec un tableau des bits

### 5.3 Lire la description d'un registre

Voici à quoi ressemble l'entrée de UCSR0B (registre de contrôle B de l'USART), reconstituée de mémoire :

```
  UCSR0B – USART Control and Status Register 0 B
  Bit           7       6       5       4      3      2       1      0
  (0xC1)     RXCIE0  TXCIE0  UDRIE0  RXEN0  TXEN0  UCSZ02  RXB80  TXB80
  Read/Write   R/W     R/W     R/W    R/W    R/W    R/W      R     R/W
  Initial       0       0       0      0      0      0       0      0

  • Bit 7 – RXCIE0: RX Complete Interrupt Enable 0
    Writing this bit to one enables interrupt on the RXC0 Flag. ...
  • Bit 4 – RXEN0: Receiver Enable 0
    Writing this bit to one enables the USART Receiver. ...
```

Comment la lire :
- **(0xC1)** : l'adresse mémoire.
- **Noms des bits** : chacun a une constante de même nom dans `<avr/io.h>`, dont la valeur est sa position (`RXEN0` = 4).
- **Read/Write** : `R/W` = lecture et écriture ; `R` = lecture seule ; `W` = écriture seule (rare).
- **Initial value** (valeur initiale) : la valeur après reset. C'est **très** important : elle vous dit ce que vous n'avez *pas* besoin de configurer. Par exemple, la valeur initiale `0x06` de UCSR0C signifie déjà « 8 bits de données, pas de parité, 1 bit de stop ».
- Un tiret `–` signifie **réservé** (reserved) : écrivez 0, et ne vous fiez pas à la valeur lue.

### 5.4 Lire les tableaux de champs de bits

Les champs sur plusieurs bits sont décrits par des tableaux. Exemple, le prescaler (prédiviseur) du Timer1 (dans TCCR1B) :

```
  CS12 CS11 CS10   Description
   0    0    0     No clock source (Timer/Counter stopped)
   0    0    1     clk/1 (no prescaling)
   0    1    0     clk/8
   0    1    1     clk/64
   1    0    0     clk/256
   1    0    1     clk/1024
   1    1    0     External clock on T1 pin, falling edge
   1    1    1     External clock on T1 pin, rising edge
```

Pour sélectionner clk/256, il faut CS12=1, CS11=0, CS10=0 :

```c
TCCR1B |= (1 << CS12);    // CS11 et CS10 sont déjà à 0 après le reset
```

Soyez explicite sur les 0 quand le registre a pu être modifié auparavant :

```c
TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11) | (1 << CS10))) | (1 << CS12);
```

### 5.5 Notations particulières

- **« Writing a logic one to this flag clears it »** (écrire un 1 logique dans ce drapeau l'efface) : certains drapeaux (comme `OCF1A` de `TIFR1`, ou `TWINT`) s'effacent en écrivant **1**, pas 0. Ça surprend tout le monde la première fois.
- **« n »** dans des noms comme `TCCRnA`, `OCRnx` est un paramètre : n = numéro du timer (0, 1, 2), x = canal (A, B). `OCR1A` = Output Compare Register, Timer1, canal A.
- **« TOP »**, **« BOTTOM »**, **« MAX »** dans les chapitres timers : BOTTOM = 0, MAX = 0xFF ou 0xFFFF, TOP = la valeur à laquelle le compteur repart (MAX, OCRnA ou ICR1, selon le mode).
- **Notation hexadécimale** : `0x1F`, `$1F` (style assembleur) ou `1Fh`.
- **Plages de bits** : `TWS[7:3]` ou `TWS7..3` signifie les bits 7 à 3.
- **Barres au-dessus** : un nom surmonté d'une barre, ou avec un suffixe/préfixe `#`, `_n`, `/` ou `N` (ex. `RESET` avec une barre), est **actif à l'état bas** (active-low).

### 5.6 Les chronogrammes

Les sections protocoles (UART, SPI, I2C) et les capteurs contiennent des chronogrammes (timing diagrams) : des signaux dessinés en fonction du temps. Comment les lire :
- Le temps s'écoule **de gauche à droite**.
- Chaque ligne horizontale est un signal (SCL, SDA, …).
- Haut = 1, bas = 0. Des lignes croisées (`><`) signifient « la donnée change ici, sa valeur importe peu ».
- Des lignes verticales en pointillés et des flèches indiquent des contraintes de temps (durées minimales/maximales).

Nous les décortiquerons protocole par protocole.

### 5.7 Méthode pour passer de « je veux X » à un code qui marche

1. **Trouver le chapitre** du périphérique (table des matières ou register summary).
2. **Lire l'overview** et regarder le schéma bloc. Quelles sont les entrées, les sorties, les registres principaux ?
3. **Trouver le mode** dont vous avez besoin (ex. « CTC mode ») et lire sa description et sa formule.
4. **Lister les registres et bits** à configurer. Faites un petit tableau sur papier : registre | bit | valeur | pourquoi.
5. **Vérifier les valeurs initiales**. Qu'est-ce qui est déjà correct ?
6. **Chercher l'exemple de code C** du chapitre, s'il y en a un.
7. Écrire le code, **avec un commentaire citant la section de la datasheet** pour chaque ligne non évidente.
8. Tester avec l'observation la plus simple possible (une LED, un affichage UART).

### 5.8 Lire le schéma de la carte

Le schéma électrique montre comment les composants sont reliés. Pour chaque exercice :
1. Trouvez le composant (ex. « D5 » pour la LED RGB).
2. Suivez ses fils ou étiquettes de net jusqu'aux broches du MCU.
3. Notez **actif à l'état haut ou bas** : la broche pilote-t-elle l'anode (active-high) ou la cathode (active-low) ?
4. Notez les résistances (pull-ups ? limitation de courant ?).
5. Pour les puces I2C : notez les **broches d'adresse** (A0, A1, A2) et si elles sont reliées à GND ou VCC. Cela fixe l'adresse I2C.

---

## 6. La devboard

> ⚠️ **Cette section décrit, de mémoire, la carte de développement typiquement utilisée dans la Piscine Embedded de 42. Considérez chaque affectation de broche comme « probable » et confirmez-la avec votre schéma dès le Day 00.**

### 6.1 Ce qu'on y trouve généralement

| Composant | Connexion probable | Utilisé au |
|---|---|---|
| ATmega328P @ 16 MHz | | tous les jours |
| Puce USB-série (CH340 ou similaire) | PD0 (RX), PD1 (TX), plus DTR → RESET pour l'auto-reset | Day 00 (flashage), Day 02 (UART) |
| LED **D1** | PB0 | Day 00 |
| LED **D2** | PB1 (OC1A) | Day 00, 01 |
| LED **D3** | PB2 (OC1B) | Day 00 |
| LED **D4** | **PB4** (pas PB3 !) | Day 00 |
| Bouton **SW1** | PD2 (INT0), actif à l'état bas | Day 00, 04 |
| Bouton **SW2** | PD4, actif à l'état bas | Day 00, 04 |
| LED RGB **D5** | R = PD5 (OC0B), G = PD6 (OC0A), B = PD3 (OC2B) | Day 03 |
| Potentiomètre **RV1** | ADC0 (PC0) | Day 05 |
| LDR (capteur de lumière) | ADC1 (PC1) | Day 05 |
| Thermistance CTN (NTC) | ADC2 (PC2) | Day 05 |
| Capteur température/humidité **AHT20** | I2C, adresse 0x38 | Day 06 |
| LEDs RGB **APA102** D6, D7, D8 | SPI (MOSI PB3, SCK PB5) | Day 08 |
| Extenseur d'E/S **PCA9555** | I2C (adresse 0x20 si A0–A2 = GND) | Day 09 |
| **Afficheur 7 segments** 4 chiffres | via le PCA9555 | Day 09 |
| LEDs supplémentaires D9–D11 & bouton SW3 | via le PCA9555 | Day 09 |
| Horloge temps réel (RTC) **PCF8563** | I2C, adresse 0x51 | Rush 01 |
| Bouton reset | PC6 / RESET | |

Remarquez la conception : **les 4 LEDs simples sont sur PB0, PB1, PB2, PB4**, ce qui saute PB3 car PB3 est MOSI (utilisé par le SPI). Mais PB2 (SS) et PB4 (MISO) sont aussi partagées avec le SPI : pendant le Day 08, les LEDs D3/D4 peuvent donc se comporter bizarrement. C'est normal.

### 6.2 La carte se connecte en USB

Le câble USB alimente la carte **et** fournit un port série. Linux le voit comme `/dev/ttyUSB0` (CH340/FTDI) ou `/dev/ttyACM0` (puces USB natives comme l'ATmega16U2 d'un vrai Uno). Ce port série sert à deux choses :
1. **Flasher** votre programme (avrdude parle au bootloader).
2. **Communiquer en UART** entre votre programme et votre PC (Day 02+).

Vous ne pouvez pas faire les deux en même temps : fermez votre terminal série avant de flasher !

---

## 7. Le C pour microcontrôleurs

Vous connaissez le C. Mais le C sur un MCU 8 bits avec 2 Ko de RAM a ses règles propres.

### 7.1 `volatile` : le mot-clé le plus important

Le compilateur optimise agressivement. S'il voit qu'une variable n'est pas modifiée dans le chemin de code courant, il peut **la garder en cache dans un registre CPU** ou **supprimer des lectures et écritures « inutiles »**. Pour les registres matériels et les variables partagées avec des interruptions, c'est catastrophique.

**Exemple 1 : boucle d'attente supprimée**
```c
void delay(void) {
    for (uint32_t i = 0; i < 100000; i++) { }  // aucun effet de bord → le compilateur supprime la boucle !
}
```
Correction :
```c
void delay(void) {
    for (volatile uint32_t i = 0; i < 100000; i++) { }  // chaque accès à i doit réellement avoir lieu
}
```

**Exemple 2 : drapeau positionné par une interruption**
```c
uint8_t flag = 0;                 // BUG : devrait être volatile
ISR(INT0_vect) { flag = 1; }

int main(void) {
    ...
    while (!flag) { }             // le compilateur lit flag une seule fois, ne voit rien
                                  // dans la boucle qui le modifie → boucle infinie, pour toujours
}
```
Correction : `volatile uint8_t flag = 0;`

**Exemple 3 : registres matériels.** Un registre comme `PINB` change tout seul, quand vous appuyez sur un bouton. Un registre comme `UDR0` a un effet de bord à la lecture (il retire un octet du tampon de réception). C'est pourquoi `<avr/io.h>` déclare tous les registres `volatile`.

**Règle :** une variable doit être `volatile` si elle est modifiée en dehors du flux normal du code, c'est-à-dire par le matériel ou par une routine d'interruption (ISR).

`volatile` signifie : « chaque lecture et chaque écriture du code source doit réellement avoir lieu, dans l'ordre, depuis/vers la mémoire ». **Cela ne rend pas les opérations atomiques** (voir §14.8).

### 7.2 La mémoire est minuscule : pensez à chaque octet

- 2048 octets de RAM au total, partagés entre les globales, la pile (variables locales, appels de fonctions) et un éventuel tas (heap).
- **Pas de `malloc`.** Utilisez des buffers statiques/globaux de taille fixe.
- Évitez les gros tableaux locaux : un `char buf[512]` dans une fonction mange un quart de la RAM sur la pile.
- Évitez la récursion profonde.
- **Les chaînes littérales sont copiées en RAM au démarrage !** `uart_printstr("Hello World\r\n")` : cette chaîne vit en flash *et* est copiée en SRAM par le code de démarrage. Avec beaucoup de chaînes, vous manquez de RAM. Solution : `PROGMEM` (§7.6).
- Vérifiez l'occupation mémoire avec `avr-size` :

```
$ avr-size -C --mcu=atmega328p main.elf
AVR Memory Usage
----------------
Device: atmega328p
Program:     584 bytes (1.8% Full)    ← flash : .text + .data
Data:         38 bytes (1.9% Full)    ← RAM : .data + .bss (pile non comptée !)
```

Quand la RAM déborde, la pile écrase silencieusement vos globales, ce qui produit des bugs *bizarres et aléatoires*.

### 7.3 Évitez (le plus souvent) les nombres à virgule flottante

L'AVR n'a **pas d'unité de calcul flottant** (FPU). Les calculs en `float` sont émulés par logiciel : c'est lent (~100+ cycles par opération) et ça ajoute ~1–2 Ko de code. `double` est identique à `float` (32 bits) par défaut avec avr-gcc.

- `printf("%f")` **ne fonctionne pas** avec la configuration par défaut d'avr-libc (il affiche `?`). Il faudrait lier une bibliothèque spéciale (`-Wl,-u,vfprintf -lprintf_flt`), et de toute façon vos sujets interdisent probablement `printf`.
- Préférez l'**arithmétique entière / en virgule fixe** (fixed-point, §7.4).

Les floats ne sont pas *interdits* par le matériel. Si votre sujet les autorise, quelques calculs en `float` par seconde ne posent aucun problème. Mais il faudra les afficher vous-même.

### 7.4 L'arithmétique en virgule fixe

Idée : au lieu de stocker 23,47 °C dans un float, stockez **2347** (des centièmes de degré) dans un entier.

```c
// Température de l'AHT20 : T = raw / 2^20 * 200 - 50   (raw fait 20 bits)
// En centièmes de °C :  T100 = raw * 20000 / 2^20 - 5000
uint32_t raw = ...;                                   // 0 .. 1048575
int32_t t100 = (int32_t)((raw * 20000ULL) >> 20) - 5000;  // intermédiaire 64 bits pour éviter le débordement
// afficher la partie entière et les décimales :
int32_t integer = t100 / 100;
int32_t decimals = t100 % 100;   // attention aux valeurs négatives : utilisez abs()
```

L'ordre des opérations compte : **multipliez d'abord, divisez en dernier** (pour garder la précision), et **surveillez les débordements**. Ici `raw * 20000` peut atteindre ~2×10¹⁰, ce qui dépasse 32 bits, d'où le `ULL` (64 bits). Une multiplication 64 bits est lente sur AVR, mais acceptable pour quelques opérations. Alternative : simplifier le rapport, par ex. `(raw * 625) >> 15` (car 20000/2²⁰ = 625/2¹⁵), qui tient sur 32 bits (1048575 × 625 ≈ 6,5×10⁸ < 4,29×10⁹).

### 7.5 Pièges de la promotion entière

En C, l'arithmétique sur des types plus petits qu'`int` **promeut** d'abord (integer promotion) ces valeurs en `int` (16 bits signé sur AVR).

```c
uint8_t a = 200, b = 100;
uint16_t s = a + b;          // 300 : correct, calculé en int
uint8_t  t = a + b;          // 44 : tronqué sur 8 bits (300 - 256)

uint8_t x = 0x0F;
if (~x == 0xF0) { }          // FAUX ! ~x est calculé en int : 0xFFF0 != 0x00F0
if ((uint8_t)~x == 0xF0) { } // VRAI

uint32_t big = 1 << 20;      // BUG : 1 est un int (16 bits) → 1 << 20 est indéfini
uint32_t ok  = 1UL << 20;    // correct : 1UL fait 32 bits

uint16_t m = 250 * 1000;     // BUG : 250000 ne tient pas dans un int → débordement
uint32_t n = 250UL * 1000;   // correct
```

**Suffixes des littéraux** : `U` non signé, `L` long (32 bits sur AVR), `UL` unsigned long, `ULL` unsigned long long (64 bits).

### 7.6 `const`, `PROGMEM` et chaînes en flash

```c
#include <avr/pgmspace.h>

const char msg[] PROGMEM = "This string stays in flash only\r\n";

void uart_printstr_P(const char *s)    // s pointe dans la FLASH
{
    char c;
    while ((c = pgm_read_byte(s++)))   // lecture spéciale depuis la flash
        uart_tx(c);
}

uart_printstr_P(msg);
uart_printstr_P(PSTR("Inline flash string\r\n"));   // macro PSTR() : littéral en flash
```

- `PROGMEM` place les données uniquement en flash.
- Vous **ne pouvez pas** les déréférencer normalement (`*s` lirait la *RAM* à cette adresse, donc n'importe quoi). Il faut utiliser `pgm_read_byte()`, `pgm_read_word()`, etc.
- Utile pour les grosses tables de correspondance (lookup tables : polices, encodages 7 segments, tables gamma, tables de sinus) et les menus.

Probablement superflu pour la plupart des exercices, mais salvateur pendant les rushes.

### 7.7 `static` en embarqué

- **Globale / fonction `static`** : visible uniquement dans ce fichier `.c`. C'est une bonne pratique dans les projets multi-fichiers (Rush 01).
- **Variable locale `static`** : conserve sa valeur entre les appels (elle vit dans `.data`/`.bss`, pas sur la pile) :

```c
void on_button_press(void) {
    static uint8_t count = 0;   // initialisée une seule fois, persiste
    count++;
}
```

### 7.8 Fonctions inline et macros

Les petites fonctions utilitaires sont idéales en `static inline` (vérification de type, pas de coût d'appel) :

```c
static inline void led_on(void)  { PORTB |=  (1 << PB0); }
static inline void led_off(void) { PORTB &= ~(1 << PB0); }
```

Les macros sont courantes aussi, mais attention aux parenthèses :

```c
#define SET_BIT(reg, bit)    ((reg) |=  (1 << (bit)))
#define CLEAR_BIT(reg, bit)  ((reg) &= ~(1 << (bit)))
#define TOGGLE_BIT(reg, bit) ((reg) ^=  (1 << (bit)))
#define READ_BIT(reg, bit)   (((reg) >> (bit)) & 1)
```

### 7.9 Pas de libc ? Écrivez vos propres utilitaires

Vos sujets peuvent vous limiter à `<avr/io.h>`. Voici ce que vous réimplémenterez probablement.

**Afficher un nombre en décimal :**
```c
void uart_putnbr(int32_t n)
{
    char buf[11];            // 10 chiffres max en 32 bits + terminateur
    uint8_t i = 0;
    uint32_t u;

    if (n < 0) { uart_tx('-'); u = -(uint32_t)n; }  // attention à INT32_MIN
    else u = n;
    do {
        buf[i++] = '0' + (u % 10);
        u /= 10;
    } while (u);
    while (i)
        uart_tx(buf[--i]);   // les chiffres ont été produits à l'envers
}
```

**Afficher un octet en hexadécimal :**
```c
void uart_puthex8(uint8_t v)
{
    const char *hex = "0123456789ABCDEF";
    uart_tx(hex[v >> 4]);     // quartet de poids fort
    uart_tx(hex[v & 0x0F]);   // quartet de poids faible
}
```

**Afficher en binaire (pour déboguer des registres) :**
```c
void uart_putbin8(uint8_t v)
{
    for (int8_t i = 7; i >= 0; i--)
        uart_tx((v & (1 << i)) ? '1' : '0');
}
```

**Analyser (parser) du texte hexadécimal** (« FF » → 255), utile au Day 03 et au Day 08 :
```c
int8_t hexval(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;   // invalide
}
```

Comparaison de chaînes, longueur, etc. : réécrivez simplement `ft_strcmp` / `ft_strlen` comme d'habitude.

### 7.10 Structs, enums et champs de bits

Les enums rendent les machines à états lisibles :
```c
typedef enum { STATE_IDLE, STATE_WAIT, STATE_PLAY, STATE_OVER } state_t;
```

Les champs de bits du C (bit-fields, `struct { uint8_t a:1; uint8_t b:3; }`) existent, mais leur disposition en mémoire dépend du compilateur. **Ne les utilisez pas** pour représenter des registres matériels. Utilisez des masques et des décalages.

### 7.11 Options du compilateur à connaître

| Option | Signification |
|---|---|
| `-mmcu=atmega328p` | puce cible (définit les adresses des registres, le jeu d'instructions, les tailles mémoire) |
| `-DF_CPU=16000000UL` | définit la macro `F_CPU` |
| `-Os` | optimiser pour la taille (standard sur AVR). **Requis** pour que `_delay_ms` soit précis |
| `-O0` | aucune optimisation ; `_delay_ms` devient très faux et le code énorme |
| `-Wall -Wextra -Werror` | avertissements (style 42) |
| `-std=gnu11` / `-std=gnu99` | norme C (les extensions GNU autorisent les littéraux `0b`) |
| `-g` | informations de débogage (pour objdump -S / les simulateurs) |
| `-ffunction-sections -fdata-sections` + `-Wl,--gc-sections` | supprimer les fonctions inutilisées |

---

## 8. La chaîne de compilation (toolchain) : de `main.c` à la puce qui tourne

### 8.1 Vue d'ensemble

```
  main.c ──► [avr-gcc : préprocesseur → compilation → assemblage] ──► main.o
                                                                        │
  autres .o / avr-libc / crt (code de démarrage) ──► [éditeur de liens] ──► main.elf (ou main.bin)
                                                                        │
                                                          [avr-objcopy -O ihex]
                                                                        │
                                                                    main.hex
                                                                        │
                                          [avrdude] ── USB/série ──► bootloader sur la puce
                                                                        │
                                                                écrit dans la flash
                                                                        │
                                                        reset → votre main() s'exécute
```

C'est de la **compilation croisée** (cross-compilation) : vous compilez *sur* un PC x86-64 *pour* une puce AVR. Votre `gcc` habituel produit du code x86 ; `avr-gcc` produit du code AVR.

### 8.2 Étape par étape

**1. Préprocesseur** (preprocessing) : gère `#include`, `#define`, `#if`. `<avr/io.h>` est développé ; grâce à `-mmcu=atmega328p`, il inclut `<avr/iom328p.h>`, qui définit tous les noms de registres de cette puce.

**2. Compilation** : C → assembleur AVR. Pour la voir : `avr-gcc -S -mmcu=atmega328p -Os main.c` → `main.s`.

**3. Assemblage** (assembly) : assembleur → code machine dans un fichier objet (`.o`, format ELF).

**4. Édition de liens** (linking) : combine vos fichiers objets, les fonctions d'avr-libc que vous avez utilisées et le **code de démarrage** (`crtatmega328p.o` : table des vecteurs + initialisation). Elle place chaque section à son adresse finale :
- `.vectors` en 0x0000
- `.text` (le code) juste après
- `.data` (globales initialisées) est *stockée* en flash après `.text` mais *vit* en SRAM à l'exécution (copiée par le code de démarrage)
- `.bss` (globales initialisées à zéro) vit en SRAM

Le résultat est un fichier ELF (le sujet de 42 peut l'appeler `main.bin` même si c'est de l'ELF ; n'importe quelle extension marche).

**5. objcopy** : extrait les octets bruts destinés à la flash au format **Intel HEX**, que comprend avrdude :
```
avr-objcopy -O ihex -R .eeprom main.elf main.hex
```
(`-R .eeprom` retire la section EEPROM éventuelle ; alternativement `-j .text -j .data` ne garde que ces sections.)

**6. avrdude** : téléverse (upload) le fichier hex :
```
avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex:i
```

### 8.3 Commandes de compilation en une ligne

```bash
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -Wall -Wextra -o main.bin main.c
avr-objcopy -O ihex main.bin main.hex
avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex:i
```

Options d'avrdude :
| Option | Signification |
|---|---|
| `-p atmega328p` (ou `m328p`) | composant cible (part) |
| `-c arduino` | type de programmateur : « parler à un bootloader de type Arduino (STK500v1) » |
| `-P /dev/ttyUSB0` | port série |
| `-b 115200` | débit attendu par le bootloader (Optiboot sur Uno : 115200 ; anciens bootloaders : 57600) |
| `-U flash:w:main.hex:i` | mémoire `flash`, opération `w` (write, écriture), fichier, format `i` (Intel hex) |
| `-U eeprom:r:dump.hex:i` | lire l'EEPROM dans un fichier (pratique au Day 07) |
| `-v` | verbeux ; `-F` forcer ; `-D` désactiver l'effacement automatique |

### 8.4 Le format Intel HEX

Ouvrez un fichier `.hex` : c'est du texte.

```
:100000000C9434000C943E000C943E000C943E0082
:00000001FF
```

Chaque ligne est un **enregistrement** (record) :
```
 :  10    0000    00    0C9434000C943E00...    82
 │  │     │       │     │                      └─ somme de contrôle (complément à deux de la somme des octets)
 │  │     │       │     └─ octets de données (0x10 = 16 octets)
 │  │     │       └─ type d'enregistrement : 00 = données, 01 = fin de fichier
 │  │     └─ adresse de chargement (0x0000)
 │  └─ nombre d'octets
 └─ caractère de début
```

`0C 94 34 00` à l'adresse 0 est l'instruction AVR `jmp 0x68` (un saut vers le code de démarrage, 0x34 mots = 0x68 octets). C'est le vecteur de reset !

### 8.5 Le Makefile

Un sujet de 42 pour le Day 00 demande typiquement un Makefile avec des règles comme `all`, `hex`, `flash`, `clean`, et des variables comme `F_CPU`. Voici un modèle solide. **Les lignes de recette doivent commencer par une TABULATION**, pas par des espaces.

```makefile
# ---- configuration ----
NAME      = main
SRC       = main.c
MCU       = atmega328p
F_CPU     = 16000000UL
BAUD      = 115200
PORT     ?= /dev/ttyUSB0
PROG      = arduino
PROG_BAUD = 115200

CC        = avr-gcc
OBJCOPY   = avr-objcopy
CFLAGS    = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(BAUD) \
            -Os -Wall -Wextra -std=gnu11

# ---- règles ----
all: hex flash

hex: $(NAME).hex

$(NAME).bin: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME).bin

$(NAME).hex: $(NAME).bin
	$(OBJCOPY) -O ihex -R .eeprom $(NAME).bin $(NAME).hex

flash: $(NAME).hex
	avrdude -p $(MCU) -c $(PROG) -P $(PORT) -b $(PROG_BAUD) -U flash:w:$(NAME).hex:i

size: $(NAME).bin
	avr-size -C --mcu=$(MCU) $(NAME).bin

clean:
	rm -f $(NAME).bin $(NAME).hex

re: clean all

.PHONY: all hex flash size clean re
```

Utilisation : `make` (compiler et flasher), `make hex` (compiler seulement), `make flash PORT=/dev/ttyUSB1`, `make clean`.

Règles optionnelles pratiques :

```makefile
screen:
	screen $(PORT) $(BAUD)

disasm: $(NAME).bin
	avr-objdump -d -S $(NAME).bin | less
```

### 8.6 Inspecter ce que le compilateur a produit

```bash
avr-objdump -d main.bin          # désassembler
avr-objdump -S main.bin          # entrelacer avec le source C (nécessite -g à la compilation)
avr-objdump -h main.bin          # en-têtes et tailles des sections
avr-nm --size-sort -S main.bin   # symboles triés par taille (qui mange ma flash ?)
avr-size -C --mcu=atmega328p main.bin
```

Regarder le désassemblage est le meilleur moyen de comprendre ce que change `volatile`, combien de temps dure une boucle, ou pourquoi quelque chose est énorme.

### 8.7 Le bootloader

Un bootloader (chargeur d'amorçage) est un petit programme situé à la fin de la flash (Optiboot utilise les 512 derniers octets). Avec le fusible `BOOTRST` actif, la puce démarre *là* après un reset :

1. Le bootloader écoute l'UART pendant un court instant (~0,5–1 s).
2. Si avrdude lui parle (protocole STK500), il reçoit le nouveau programme et l'écrit dans la flash (l'AVR peut se reprogrammer lui-même depuis la section de boot).
3. Sinon, il saute à l'adresse 0x0000 : votre programme.

**Auto-reset :** quand avrdude ouvre le port série, la ligne **DTR** de la puce USB-série émet une impulsion. À travers un condensateur de 100 nF, cette impulsion active la broche RESET du MCU : le bootloader démarre juste au moment où avrdude veut lui parler. Si le flashage échoue avec `not in sync` / `stk500_recv(): programmer is not responding`, essayez d'appuyer sur le bouton reset juste avant le lancement d'avrdude, et vérifiez le débit (57600 vs 115200).

Sans bootloader, on utiliserait un **programmateur ISP** (In-System Programming : USBasp, AVRISP mkII, un autre Arduino en « Arduino as ISP ») sur les broches SPI : `-c usbasp`.

### 8.8 Les fusibles (fuses) : sachez qu'ils existent, n'y touchez pas

Les **fusibles** (fuses) sont des octets de configuration spéciaux (low, high, extended) écrits avec un programmateur. Ils ne sont *pas* réinitialisés quand vous flashez votre code. Ils contrôlent :
- la source d'horloge (RC interne / quartz externe) et **CKDIV8** (division de l'horloge par 8)
- la taille de la section de boot et BOOTRST
- le seuil de détection de sous-tension (brown-out detection)
- EESAVE (préserver l'EEPROM lors d'un effacement complet de la puce)
- le fonctionnement de la broche reset, l'activation de la programmation ISP…

Fusibles typiques d'un Arduino Uno : `lfuse=0xFF` (quartz externe, pas de div8), `hfuse=0xDE` (Optiboot 512 o, SPIEN activé), `efuse=0xFD` ou `0x05` (BOD 2,7 V).

> ⚠️ De mauvais fusibles peuvent **briquer** (rendre inutilisable) une puce : sélectionner une horloge externe inexistante, désactiver le reset, désactiver la programmation SPI. De toute façon, on ne peut pas écrire les fusibles via le bootloader. **N'y touchez pas pendant la piscine.**

Pour les lire (avec un programmateur ISP uniquement) : `avrdude ... -U lfuse:r:-:h`.

### 8.9 La table des vecteurs d'interruption

Au tout début de la flash se trouve une table de 26 entrées (2 mots chacune = une instruction `JMP`) :

| # | Adresse | Source | Nom avr-libc |
|---|---|---|---|
| 1 | 0x0000 | RESET | |
| 2 | 0x0002 | INT0 | `INT0_vect` |
| 3 | 0x0004 | INT1 | `INT1_vect` |
| 4 | 0x0006 | PCINT0 | `PCINT0_vect` |
| 5 | 0x0008 | PCINT1 | `PCINT1_vect` |
| 6 | 0x000A | PCINT2 | `PCINT2_vect` |
| 7 | 0x000C | WDT | `WDT_vect` |
| 8 | 0x000E | TIMER2 COMPA | `TIMER2_COMPA_vect` |
| 9 | 0x0010 | TIMER2 COMPB | `TIMER2_COMPB_vect` |
| 10 | 0x0012 | TIMER2 OVF | `TIMER2_OVF_vect` |
| 11 | 0x0014 | TIMER1 CAPT | `TIMER1_CAPT_vect` |
| 12 | 0x0016 | TIMER1 COMPA | `TIMER1_COMPA_vect` |
| 13 | 0x0018 | TIMER1 COMPB | `TIMER1_COMPB_vect` |
| 14 | 0x001A | TIMER1 OVF | `TIMER1_OVF_vect` |
| 15 | 0x001C | TIMER0 COMPA | `TIMER0_COMPA_vect` |
| 16 | 0x001E | TIMER0 COMPB | `TIMER0_COMPB_vect` |
| 17 | 0x0020 | TIMER0 OVF | `TIMER0_OVF_vect` |
| 18 | 0x0022 | SPI, STC | `SPI_STC_vect` |
| 19 | 0x0024 | USART, RX | `USART_RX_vect` |
| 20 | 0x0026 | USART, UDRE | `USART_UDRE_vect` |
| 21 | 0x0028 | USART, TX | `USART_TX_vect` |
| 22 | 0x002A | ADC | `ADC_vect` |
| 23 | 0x002C | EE READY | `EE_READY_vect` |
| 24 | 0x002E | ANALOG COMP | `ANALOG_COMP_vect` |
| 25 | 0x0030 | TWI | `TWI_vect` |
| 26 | 0x0032 | SPM READY | `SPM_READY_vect` |

L'ordre est aussi celui de la **priorité** : adresse plus basse = priorité plus haute quand plusieurs interruptions sont en attente en même temps.

Quand vous écrivez `ISR(TIMER1_COMPA_vect) { ... }`, l'éditeur de liens place un saut vers votre fonction dans l'emplacement correspondant. Les emplacements inutilisés sautent vers `__bad_interrupt`, qui redémarre le programme. Donc **si vous activez une interruption mais oubliez d'écrire son ISR, votre puce redémarre en boucle**.

---

## 9. Installer votre environnement

### 9.1 Installer la toolchain (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install gcc-avr binutils-avr avr-libc avrdude make screen
# optionnel mais recommandé :
sudo apt install picocom minicom simavr gdb-avr
```

Vérification :
```bash
avr-gcc --version
avrdude -? 2>&1 | head -3
```

Sur macOS : `brew tap osx-cross/avr && brew install avr-gcc avrdude`.

(À 42, les ordinateurs de l'école ont probablement tout pré-installé, ou vous utiliserez une VM ou un conteneur fourni.)

### 9.2 Permissions du port série

Branchez la carte, puis :
```bash
ls /dev/ttyUSB* /dev/ttyACM*        # quel port est apparu ?
sudo dmesg | tail                    # voir « ch341-uart converter now attached to ttyUSB0 »
ls -l /dev/ttyUSB0                   # crw-rw---- 1 root dialout ...
```
Si vous obtenez `Permission denied`, ajoutez-vous au groupe `dialout` (puis **déconnectez-vous et reconnectez-vous**) :
```bash
sudo usermod -aG dialout $USER
```

Sur certaines distributions, `brltty` (un démon pour afficheurs braille) s'empare des périphériques CH340. Si le port apparaît puis disparaît : `sudo apt remove brltty`.

### 9.3 Terminal série

Pour dialoguer avec votre programme via l'UART (Day 02+) :

```bash
screen /dev/ttyUSB0 115200        # quitter : Ctrl-A puis K puis Y  (ou Ctrl-A puis \)
picocom -b 115200 /dev/ttyUSB0    # quitter : Ctrl-A puis Ctrl-X
minicom -D /dev/ttyUSB0 -b 115200 # quitter : Ctrl-A puis X
```

Connaissez le comportement de votre terminal :
- Appuyer sur **Entrée** envoie `\r` (retour chariot, carriage return, 0x0D), généralement **pas** `\n`.
- Pour passer à la ligne en sortie, envoyez **`"\r\n"`** : `\r` ramène en colonne 0, `\n` descend d'une ligne. N'envoyer que `\n` donne un effet « d'escalier ».
- **Retour arrière** (Backspace) envoie `0x7F` (DEL) ou `0x08` (BS) selon le terminal.
- Le terminal n'affiche **pas** ce que vous tapez (pas d'écho local, local echo). C'est votre programme qui doit renvoyer les caractères si vous voulez les voir.

> ⚠️ **Fermez le terminal série avant de lancer `make flash`**, car deux programmes ne peuvent pas utiliser le port en même temps. Détacher screen (Ctrl-A D) ne suffit pas : il garde le port… quittez-le vraiment.

### 9.4 Configuration de l'éditeur

Pour l'autocomplétion avec les en-têtes AVR (VS Code + clangd, ou l'extension C/C++), créez un `compile_flags.txt` à côté de votre code :

```
--target=avr
-mmcu=atmega328p
-DF_CPU=16000000UL
-D__AVR_ATmega328P__
-I/usr/lib/avr/include
```

ou indiquez à l'extension C/C++ : chemin du compilateur `/usr/bin/avr-gcc`, defines `__AVR_ATmega328P__` et `F_CPU=16000000UL`. `PORTB`, `DDRB`, etc. s'autocomplètent alors et on peut sauter à leur définition. Parcourir `/usr/lib/avr/include/avr/iom328p.h` est un excellent moyen d'apprendre les noms des registres et des bits.

### 9.5 Un simulateur (optionnel, quand vous n'avez pas la carte)

**simavr** exécute des programmes AVR sur votre PC :
```bash
simavr -m atmega328p -f 16000000 main.bin
```
Il peut afficher l'UART dans le terminal et produire des traces VCD (visualisables dans GTKWave). Vous pouvez aussi déboguer avec `avr-gdb`. C'est excellent pour tester la logique, mais il ne simule pas les LEDs et capteurs de la carte.

Le simulateur en ligne **Wokwi** (wokwi.com) simule un Arduino Uno (ATmega328P) avec LEDs, boutons et de nombreux périphériques I2C/SPI, et accepte du C bare-metal. Il est extrêmement utile pour s'entraîner avant la piscine.


---

# PARTIE II : LA PISCINE, JOUR PAR JOUR

Chaque jour suit la même structure :
- **Objectifs** : ce que vous devez comprendre à la fin de la journée
- **Théorie** : les concepts
- **Registres** : quoi configurer, avec références à la datasheet
- **Exemples détaillés** : du code complet et commenté
- **Exercices typiques** : ce que le sujet demandera probablement, avec des indices
- **Pièges** : ce qui tourne mal

---

## 10. Day 00 : Flashing & GPIO

### 10.1 Objectifs

- Compiler un programme avec avr-gcc et le flasher (le téléverser dans la flash) avec avrdude via un Makefile.
- Comprendre les **GPIO** (General Purpose Input/Output, entrées/sorties à usage général) : les registres `DDRx`, `PORTx`, `PINx`.
- Allumer une LED, lire un bouton, inverser une LED, compter en binaire.
- Gérer les **rebonds des boutons** (bouncing) etF la **détection de fronts** (edge detection).

### 10.2 Les trois registres de chaque port

Chaque port (B, C, D) est contrôlé par **trois registres de 8 bits**. Chaque bit de chaque registre correspond à une broche : bit 0 ↔ Px0, bit 1 ↔ Px1, etc.

| Registre | Nom | Rôle |
|---|---|---|
| **DDRx** | Data Direction Register (registre de direction) | bit = 1 → la broche est une **sortie** ; bit = 0 → la broche est une **entrée** |
| **PORTx** | Port Data Register (registre de données du port) | en sortie : 1 = piloter HIGH, 0 = piloter LOW. En entrée : 1 = **activer la pull-up**, 0 = pas de pull-up (Hi-Z) |
| **PINx** | Port Input Pins (broches d'entrée du port) | **lire** le niveau électrique réel de chaque broche. **Écrire 1** dans un bit **inverse** (toggle) le bit correspondant de PORTx |

Les quatre configurations possibles d'une broche :

| DDxn | PORTxn | Comportement de la broche |
|:---:|:---:|---|
| 0 | 0 | Entrée, haute impédance (flottante si non connectée) |
| 0 | 1 | Entrée **avec pull-up interne** (lit 1 sauf si tirée à 0 de l'extérieur) |
| 1 | 0 | Sortie pilotée **LOW** (0 V) |
| 1 | 1 | Sortie pilotée **HIGH** (VCC) |

Le bit `PUD` de `MCUCR` désactive globalement toutes les pull-ups (vous n'en aurez jamais besoin).

Modèle mental d'une broche :

```
                      DDRxn
                        │ (1 = active l'étage de sortie)
                        ▼
   PORTxn ──────►[ étage de sortie ]────┬────── broche physique
     │                                  │
     └──(si entrée & PORTxn=1)──►[pull-up vers VCC]
                                        │
   PINxn ◄──────[ synchroniseur ]◄──────┘   (lit toujours le niveau réel,
                                             même quand la broche est en sortie)
```

Remarques :
- **Au reset, tous les registres DDR et PORT valent 0** : chaque broche est une entrée flottante. Rien n'est piloté tant que vous ne l'avez pas configuré.
- **PINx reflète toujours le niveau physique**, même pour une sortie. La valeur est synchronisée, donc elle a ~1 cycle de retard.
- **Écrire `1` dans un bit de PINx inverse PORTx** (datasheet : « Toggling the Pin »). `PINB = (1 << PB0);` inverse PB0 en une seule instruction atomique. Écrire des 0 n'a aucun effet. Donc `PINB = (1 << PB0)` est correct, tandis que `PINB |= (1 << PB0)` est **faux** : cela lit PINB (dont d'autres bits peuvent valoir 1) et les réécrit, ce qui les inverse aussi !

### 10.3 Allumer une LED

En supposant que la LED D1 est sur PB0, active à l'état haut :

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB0);     // PB0 = sortie
    PORTB |= (1 << PB0);    // PB0 = HIGH → LED allumée

    while (1) { }           // ne pas quitter main
}
```

Seulement deux écritures de registre. Si le sujet interdit les raccourcis `|=` ou exige « uniquement des registres », vous pourriez écrire `DDRB = 0x01; PORTB = 0x01;`, qui fonctionne aussi mais écrase tous les autres bits du port. C'est sans conséquence ici, mais pas en général.

### 10.4 Lire un bouton

En supposant que SW1 est sur PD2, actif à l'état bas, avec une pull-up (externe sur la carte, ou activez l'interne par sécurité) :

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB0);        // LED en sortie
    DDRD &= ~(1 << PD2);       // bouton en entrée (déjà 0 après reset, mais être explicite est bien)
    PORTD |= (1 << PD2);       // activer la pull-up interne (sans risque si une externe existe)

    while (1)
    {
        if (!(PIND & (1 << PD2)))       // la broche lit 0 → bouton appuyé
            PORTB |= (1 << PB0);        // LED allumée
        else
            PORTB &= ~(1 << PB0);       // LED éteinte
    }
}
```

Attention à la priorité des opérateurs : `!(PIND & (1 << PD2))` a besoin des parenthèses. `!PIND & (1 << PD2)` calculerait `(!PIND) & 4`, ce qui est faux.

Une écriture plus propre :

```c
#define SW1_PRESSED() (!(PIND & (1 << PD2)))
```

### 10.5 Inverser une LED à chaque appui : détection de fronts

La version naïve :
```c
while (1) {
    if (SW1_PRESSED())
        PINB = (1 << PB0);      // inverser
}
```
Tant que vous maintenez le bouton, ceci inverse la LED des millions de fois par seconde. Vous la verriez à mi-luminosité, et au relâchement elle s'arrêterait dans un état aléatoire.

Il faut réagir à la **transition** (front, edge) de « relâché » vers « appuyé » (un **front descendant**, falling edge, sur la broche, puisqu'elle est active à l'état bas) :

```c
uint8_t prev = 1;   // 1 = relâché (niveau de la broche)

while (1) {
    uint8_t now = (PIND >> PD2) & 1;   // niveau actuel : 1 relâché, 0 appuyé
    if (prev == 1 && now == 0)          // front descendant = appui
        PINB = (1 << PB0);              // inverser la LED
    prev = now;
}
```

Cela fonctionne pour un appui propre, **mais les rebonds mécaniques** (§3.6) produisent plusieurs fronts par appui. La LED s'inverse un nombre imprévisible de fois, ce qui donne l'impression que « parfois ça marche, parfois non ».

### 10.6 `_delay_ms()` et `<util/delay.h>`

```c
#include <util/delay.h>   // F_CPU doit être défini AVANT l'include (utilisez -DF_CPU dans le Makefile)

_delay_ms(20);    // attente active de ~20 ms
_delay_us(10);    // attente active de ~10 µs
```

- Ce sont des **boucles d'attente active** (busy-wait) calculées à la compilation à partir de `F_CPU`. Le CPU ne fait rien d'autre pendant l'attente.
- Elles nécessitent **l'optimisation activée** (`-Os`) et un **argument constant connu à la compilation**. Sinon elles embarquent du code flottant et deviennent imprécises. Si vous avez besoin d'un délai variable, faites une boucle : `for (i = 0; i < n; i++) _delay_ms(1);`
- Si `F_CPU` n'est pas défini, vous obtenez un avertissement et une valeur par défaut de 1 MHz (chaque délai devient 16× trop long sur votre carte).

Le sujet peut interdire `_delay_ms` dans certains exercices (ex. le Day 01 ex00 vous demande d'écrire votre propre délai).

### 10.7 Techniques anti-rebond (debouncing)

**Technique 1 : attendre après un front** (la plus simple, suffisante pour le Day 00)
```c
while (1) {
    if (SW1_PRESSED()) {
        PINB = (1 << PB0);          // agir une seule fois
        _delay_ms(20);              // laisser les rebonds de l'appui se calmer
        while (SW1_PRESSED()) { }   // attendre le relâchement
        _delay_ms(20);              // laisser les rebonds du relâchement se calmer
    }
}
```
Inconvénient : c'est **bloquant**. Rien d'autre ne se passe tant que le bouton est maintenu. Acceptable pour le Day 00, mauvais par la suite.

**Technique 2 : détection de front non bloquante + délai après le front**
```c
uint8_t prev = 1;
while (1) {
    uint8_t now = (PIND >> PD2) & 1;
    if (now != prev) {             // le niveau a changé
        if (now == 0)              // c'est un appui
            PINB = (1 << PB0);
        prev = now;
        _delay_ms(20);             // ignorer les rebonds pendant 20 ms
    }
    // autre travail ici...
}
```

**Technique 3 : échantillonnage / intégrateur** (robuste, non bloquant ; typiquement dans une interruption de timer toutes les ~5 ms)
```c
// Appelée toutes les 5 ms. L'état n'est accepté qu'après 4 échantillons identiques (20 ms).
static uint8_t history = 0xFF;
static uint8_t stable = 1;

void button_sample(void)
{
    history = (history << 1) | ((PIND >> PD2) & 1);
    if ((history & 0x0F) == 0x00 && stable == 1) { stable = 0; /* événement APPUI */ }
    if ((history & 0x0F) == 0x0F && stable == 0) { stable = 1; /* événement RELÂCHEMENT */ }
}
```

**Technique 4 : matérielle** : un filtre RC + une bascule de Schmitt (Schmitt trigger). Votre carte a peut-être déjà un condensateur sur les boutons.

Durée typique des rebonds : 1–10 ms. Un délai anti-rebond de 20 ms est une valeur par défaut sûre. De toute façon, un humain ne peut pas appuyer plus de ~10 fois par seconde.

### 10.8 Compteur binaire sur 4 LEDs

Exercice classique : SW1 incrémente une valeur, SW2 la décrémente, et la valeur (0–15) est affichée en binaire sur D1–D4.

**Le problème de correspondance :** les LEDs sont sur PB0, PB1, PB2 et **PB4** (devboard typique). Le bit 3 de la valeur doit aller sur PB4, pas sur PB3.

```c
#include <avr/io.h>
#include <util/delay.h>

#define LED_MASK ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))

static void display(uint8_t v)
{
    uint8_t out = (v & 0x07)               // bits 0..2 → directement PB0..PB2
                | ((v & 0x08) << 1);       // bit 3 → décalé en bit 4 (PB4)
    PORTB = (PORTB & ~LED_MASK) | out;     // ne mettre à jour que les broches des LEDs
}

int main(void)
{
    uint8_t value = 0;
    uint8_t prev1 = 1, prev2 = 1;

    DDRB |= LED_MASK;                      // LEDs en sortie
    DDRD &= ~((1 << PD2) | (1 << PD4));    // boutons en entrée
    PORTD |= (1 << PD2) | (1 << PD4);      // pull-ups

    display(value);
    while (1)
    {
        uint8_t now1 = (PIND >> PD2) & 1;
        uint8_t now2 = (PIND >> PD4) & 1;

        if (prev1 && !now1) { value = (value + 1) & 0x0F; display(value); }
        if (prev2 && !now2) { value = (value - 1) & 0x0F; display(value); }

        if (now1 != prev1 || now2 != prev2)
            _delay_ms(20);   // anti-rebond rudimentaire

        prev1 = now1;
        prev2 = now2;
    }
}
```

Remarques :
- `& 0x0F` réalise le rebouclage 15 → 0 et 0 → 15. (Le sujet peut au contraire demander de saturer : s'arrêter à 0 et à 15. Lisez attentivement !)
- `PORTB = (PORTB & ~LED_MASK) | out;` est le motif **lecture-modification-écriture avec masque** (read-modify-write). Il ne perturbe pas les autres broches du port B.

### 10.9 Exercices typiques du Day 00

| Ex | Tâche | Concept clé |
|---|---|---|
| 00 | Makefile avec `hex`, `flash`, `clean`, `all` ; F_CPU passé avec `-D` | toolchain |
| 01 | Allumer la LED D1 (uniquement avec les registres AVR) | DDR / PORT |
| 02 | LED allumée tant que SW1 est maintenu, éteinte sinon | PIN, pull-up, actif à l'état bas |
| 03 | Chaque appui sur SW1 inverse la LED | détection de front, anti-rebond |
| 04 | SW1 = +1, SW2 = −1, valeur affichée en binaire sur D1–D4 | masques, correspondance de bits |

### 10.10 Pièges

- **Oublier `DDRx`** : la broche reste une entrée, et écrire PORTx ne fait qu'activer/désactiver une pull-up. La LED brille très faiblement ou pas du tout.
- **Utiliser `PORTx` pour lire** au lieu de `PINx`. `PORTx` renvoie ce que *vous* avez écrit, pas le niveau réel de la broche.
- **`PINB |= ...` pour inverser** : cela inverse aussi d'autres bits. Utilisez `PINB = (1 << n)`.
- Priorité des opérateurs avec `!` et `&`, et `==` qui est prioritaire sur `&` : `if (PIND & (1 << PD2) == 0)` signifie `PIND & ((1<<PD2) == 0)` → toujours 0. Écrivez `if ((PIND & (1 << PD2)) == 0)`.
- Terminal série toujours ouvert → `avrdude: ser_open(): can't open device`.
- `main` qui retourne → le programme se fige.

---

## 11. Day 01 : Timers

### 11.1 Objectifs

- Comprendre pourquoi l'attente active est mauvaise, et combien de temps prend l'exécution du code.
- Comprendre les **timers/compteurs matériels** (timer/counter) : prescaler, compteur, comparaison, débordement.
- Maîtriser les **modes** : Normal, CTC, Fast PWM, Phase-correct PWM.
- Piloter une broche **entièrement en matériel** (le CPU ne fait rien et la LED clignote quand même).
- Comprendre et générer une **PWM** (rapport cyclique, duty cycle).

### 11.2 Échauffement : un délai sans timer (et sans `_delay_ms`)

Premier exercice typique : « Faire clignoter la LED D2 à 1 Hz sans utiliser de timer ni `_delay_ms`. »

Il faut une boucle qui consomme ~500 ms de temps CPU. À 16 MHz, cela fait **8 000 000 cycles**. Combien de cycles prend une itération de boucle ? Cela dépend du code compilé. Regardez le désassemblage :

```c
void wait_500ms(void)
{
    for (volatile uint32_t i = 0; i < ITERATIONS; i++) { }
}
```

Avec `volatile uint32_t`, chaque itération charge 4 octets depuis la RAM, compare, incrémente et réécrit 4 octets : environ 20–30 cycles selon la version du compilateur. Pour obtenir le chiffre exact :

```bash
avr-gcc -mmcu=atmega328p -Os -g -c main.c -o main.o
avr-objdump -d -S main.o
```

Comptez les cycles de chaque instruction du corps de boucle (voir le manuel du jeu d'instructions : `ld`/`st`/`ldd`/`std` = 2 cycles, `subi`/`sbci`/`cpi`/`cpc` = 1, branchement `brne` pris = 2, …). Si le total vaut **N** cycles, alors `ITERATIONS = 8000000 / N`.

**Alternative pragmatique :** choisissez une valeur, chronométrez 20 clignotements et ajustez proportionnellement. C'est un étalonnage empirique.

**Alternative plus propre**, avec un corps de boucle prévisible : une boucle imbriquée sur des `uint8_t`/`uint16_t` avec un `asm volatile("nop")` à l'intérieur. Le `nop` empêche la suppression de la boucle par l'optimiseur sans avoir besoin de variables `volatile` :

```c
static void delay_ms_approx(uint16_t ms)
{
    while (ms--)
        for (uint16_t i = 0; i < 2667; i++)   // ~6 cycles/itération × 2667 ≈ 16000 cycles ≈ 1 ms
            __asm__ volatile ("nop");          // (vérifiez le vrai compte dans le désassemblage, puis ajustez)
}
```

C'est exactement ce que fait `_delay_ms` en interne, mais avec des nombres de cycles calculés précisément (`__builtin_avr_delay_cycles`).

**Inverser** (clignotement à 1 Hz = 500 ms allumée + 500 ms éteinte) :
```c
DDRB |= (1 << PB1);
while (1) {
    PINB = (1 << PB1);   // inverser
    wait_500ms();
}
```

La leçon : **l'attente active gaspille le CPU et est imprécise**. Les timers règlent ces deux problèmes.

### 11.3 Qu'est-ce qu'un timer ?

Un **timer/compteur** (timer/counter) est un périphérique matériel qui **compte des impulsions d'horloge tout seul**, en parallèle du CPU. Son cœur est un registre, `TCNTn`, incrémenté à chaque tick.

```
  F_CPU (16 MHz)
      │
  ┌───▼───────┐    tick     ┌────────────┐
  │ Prescaler │ ──────────► │  TCNTn     │ ── comparé à OCRnA ──► égalité ! → action sur la broche OCnA
  │ ÷1,8,64,  │             │  compteur  │ ── comparé à OCRnB ──► égalité ! → action sur la broche OCnB
  │ 256,1024  │             │ 0,1,2,...  │ ── atteint TOP / MAX ──► débordement / redémarrage
  └───────────┘             └────────────┘
                                   │
                    drapeaux : TOVn, OCFnA, OCFnB (→ interruptions optionnelles)
```

- **Prescaler** (prédiviseur) : divise l'horloge CPU avant de l'envoyer au compteur, par ex. ÷256 → le compteur avance à 16 MHz / 256 = 62 500 Hz (toutes les 16 µs).
- **TCNTn** : la valeur du compteur (8 bits pour Timer0/2 → 0–255 ; 16 bits pour Timer1 → 0–65535).
- **OCRnA / OCRnB** (Output Compare Registers, registres de comparaison de sortie) : valeurs comparées en permanence à TCNTn. En cas d'égalité, il se produit une **comparaison réussie** (compare match) : le timer peut modifier une broche de sortie, lever un drapeau, déclencher une interruption ou remettre le compteur à zéro.
- **Débordement** (overflow) : quand le compteur dépasse son maximum et repasse à 0, il lève `TOVn`.
- **Drapeaux** (flags, dans `TIFRn`) : levés par le matériel lors d'événements. Vous pouvez les scruter (polling), ou activer les interruptions (`TIMSKn`, Day 04).

L'ATmega328P a trois timers :

| | Timer0 | Timer1 | Timer2 |
|---|---|---|---|
| Taille | 8 bits | **16 bits** | 8 bits |
| Prescalers | 1, 8, 64, 256, 1024 | 1, 8, 64, 256, 1024 | 1, 8, **32**, 64, **128**, 256, 1024 |
| Broches de sortie | OC0A = PD6, OC0B = PD5 | OC1A = PB1, OC1B = PB2 | OC2A = PB3, OC2B = PD3 |
| Extras | | Capture d'entrée (ICR1), plus de modes | Asynchrone (quartz 32 kHz) |
| Utilisé par Arduino pour | `millis()`, `delay()` | bibliothèque Servo | `tone()` |

(Vous n'utilisez pas Arduino : les trois sont à vous.)

### 11.4 Les registres du Timer1 en détail

Le Timer1 est le plus flexible, et c'est celui qui est relié à la LED D2 (PB1 = OC1A).

**TCCR1A** (Timer/Counter1 Control Register A, registre de contrôle A)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | COM1A1 | COM1A0 | COM1B1 | COM1B0 | – | – | WGM11 | WGM10 |

**TCCR1B** (Timer/Counter1 Control Register B, registre de contrôle B)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | ICNC1 | ICES1 | – | WGM13 | WGM12 | CS12 | CS11 | CS10 |

**Autres registres :**
- `TCNT1` (16 bits) : valeur du compteur
- `OCR1A`, `OCR1B` (16 bits) : valeurs de comparaison
- `ICR1` (16 bits) : registre de capture d'entrée (input capture), utilisable aussi comme TOP
- `TIMSK1` : activation des interruptions (`ICIE1`, `OCIE1B`, `OCIE1A`, `TOIE1`)
- `TIFR1` : drapeaux (`ICF1`, `OCF1B`, `OCF1A`, `TOV1`), **effacés en écrivant 1**
- `TCCR1C` : forçage de la comparaison de sortie (`FOC1A`, `FOC1B`), rarement utilisé

**Sélection d'horloge (clock select), CS12:0 :**
| CS12 | CS11 | CS10 | Horloge |
|---|---|---|---|
| 0 | 0 | 0 | **arrêté** (valeur de reset !) |
| 0 | 0 | 1 | clk/1 |
| 0 | 1 | 0 | clk/8 |
| 0 | 1 | 1 | clk/64 |
| 1 | 0 | 0 | clk/256 |
| 1 | 0 | 1 | clk/1024 |
| 1 | 1 | x | broche externe T1 |

**Le timer ne tourne pas tant que vous n'avez pas choisi une source d'horloge non nulle.** Régler les bits CS est généralement la *dernière* étape de configuration : c'est elle qui démarre le timer.

**Mode de génération de forme d'onde (Waveform Generation Mode), WGM13:0**, réparti entre TCCR1A (WGM11, WGM10) et TCCR1B (WGM13, WGM12). C'est le piège classique !

| Mode | WGM13 | WGM12 | WGM11 | WGM10 | Mode de fonctionnement | TOP | Mise à jour de OCR1x | TOV1 levé à |
|---|---|---|---|---|---|---|---|---|
| **0** | 0 | 0 | 0 | 0 | **Normal** | 0xFFFF | immédiate | MAX |
| 1 | 0 | 0 | 0 | 1 | PWM, phase correct, 8 bits | 0x00FF | TOP | BOTTOM |
| 2 | 0 | 0 | 1 | 0 | PWM, phase correct, 9 bits | 0x01FF | TOP | BOTTOM |
| 3 | 0 | 0 | 1 | 1 | PWM, phase correct, 10 bits | 0x03FF | TOP | BOTTOM |
| **4** | 0 | 1 | 0 | 0 | **CTC** | **OCR1A** | immédiate | MAX |
| 5 | 0 | 1 | 0 | 1 | Fast PWM, 8 bits | 0x00FF | BOTTOM | TOP |
| 6 | 0 | 1 | 1 | 0 | Fast PWM, 9 bits | 0x01FF | BOTTOM | TOP |
| 7 | 0 | 1 | 1 | 1 | Fast PWM, 10 bits | 0x03FF | BOTTOM | TOP |
| 8 | 1 | 0 | 0 | 0 | PWM, phase & freq correct | ICR1 | BOTTOM | BOTTOM |
| 9 | 1 | 0 | 0 | 1 | PWM, phase & freq correct | OCR1A | BOTTOM | BOTTOM |
| 10 | 1 | 0 | 1 | 0 | PWM, phase correct | ICR1 | TOP | BOTTOM |
| 11 | 1 | 0 | 1 | 1 | PWM, phase correct | OCR1A | TOP | BOTTOM |
| **12** | 1 | 1 | 0 | 0 | **CTC** | **ICR1** | immédiate | MAX |
| 13 | 1 | 1 | 0 | 1 | (réservé) | – | – | – |
| **14** | 1 | 1 | 1 | 0 | **Fast PWM** | **ICR1** | BOTTOM | TOP |
| **15** | 1 | 1 | 1 | 1 | **Fast PWM** | **OCR1A** | BOTTOM | TOP |

Ceux en gras sont ceux que vous utiliserez.

**Mode de sortie de comparaison (Compare Output Mode), COM1A1:0** (ce qui arrive sur la broche OC1A = PB1). Sa signification dépend du mode :

Modes non PWM (Normal, CTC) :
| COM1A1 | COM1A0 | Effet sur OC1A lors d'une égalité |
|---|---|---|
| 0 | 0 | déconnectée (la broche est un GPIO normal) |
| 0 | 1 | **inversion** (toggle) |
| 1 | 0 | mise à 0 (LOW) |
| 1 | 1 | mise à 1 (HIGH) |

Modes Fast PWM :
| COM1A1 | COM1A0 | Effet |
|---|---|---|
| 0 | 0 | déconnectée |
| 0 | 1 | modes 14/15 : inversion de OC1A à l'égalité (autres modes : déconnectée) |
| 1 | 0 | **non inversé** (non-inverting) : mise à 0 à l'égalité, mise à 1 à BOTTOM |
| 1 | 1 | inversé (inverting) : mise à 1 à l'égalité, mise à 0 à BOTTOM |

**Important :** pour que le timer pilote la broche, **celle-ci doit aussi être configurée en sortie dans DDRx** (`DDRB |= (1 << PB1)`).

### 11.5 Mode Normal

Le compteur compte de 0 → MAX (0xFFFF pour le Timer1) → repasse à 0 et lève TOV1. Période de débordement = prescaler × 65536 / F_CPU.

Avec un prescaler de 256 : 256 × 65536 / 16e6 = **1,048 s**. On pourrait précharger TCNT1 pour raccourcir cette période, mais le mode CTC est bien plus propre.

### 11.6 Mode CTC (Clear Timer on Compare match, remise à zéro sur comparaison)

Le compteur compte de 0 → OCR1A, puis **repart à 0** (au lieu de monter jusqu'à MAX). Donc **OCR1A fixe la période**.

```
 TCNT1
  OCR1A ┤    /|    /|    /|    /|
        │   / |   / |   / |   / |
        │  /  |  /  |  /  |  /  |
        │ /   | /   | /   | /   |
      0 ┼/────┴/────┴/────┴/────┴──► temps
         ↑     ↑     ↑     ↑   égalité : OCF1A levé, OC1A inversée (si COM1A = 01)
 OC1A    ┌─────┐     ┌─────┐
 (PB1)  ─┘     └─────┘     └────   → signal carré, fréquence = fréquence des égalités / 2
```

**Formules :**
- Temps entre deux égalités : `T_match = N × (1 + OCR1A) / F_CPU`, où N = prescaler.
- Fréquence de la sortie en mode inversion : **`f_OC1A = F_CPU / (2 × N × (1 + OCR1A))`**, car deux inversions font une période.
- Donc : `OCR1A = F_CPU / (2 × N × f) − 1`.

**Exemple détaillé : faire clignoter la LED D2 (PB1) à 1 Hz, entièrement en matériel.**
On veut f = 1 Hz, donc une inversion toutes les 0,5 s.
- N = 1 : OCR1A = 16e6 / 2 − 1 = 7 999 999 → trop grand pour 16 bits (max 65535) ✗
- N = 64 : OCR1A = 16e6 / 128 − 1 = 124 999 ✗
- N = 256 : OCR1A = 16e6 / 512 − 1 = **31 249** ✓ (exact, pas d'erreur d'arrondi)
- N = 1024 : OCR1A = 16e6 / 2048 − 1 = 7 811,5 → 7 811 ✓ mais inexact

**Choisissez le plus petit prescaler qui fait tenir OCR** : il donne la meilleure résolution.

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);                 // OC1A (PB1) en sortie : c'est le timer qui la pilote

    TCCR1A = (1 << COM1A0);             // COM1A = 01 : inverser OC1A à chaque égalité
    OCR1A = 31249;                      // 16e6 / 256 / 2 - 1 → 0,5 s entre deux inversions
    TCCR1B = (1 << WGM12)               // mode 4 : CTC avec TOP = OCR1A
           | (1 << CS12);               // prescaler 256 → le timer démarre maintenant

    while (1) { }                       // le CPU ne fait RIEN ; la LED clignote quand même
}
```

C'est souvent l'exercice clé du Day 01 : **une boucle principale vide**, le matériel faisant tout le travail.

### 11.7 PWM : modulation de largeur d'impulsion

La **PWM** (Pulse Width Modulation, modulation de largeur d'impulsion, MLI) est un signal carré dont vous choisissez la **proportion de temps à l'état HIGH** (le **rapport cyclique**, duty cycle) :

```
  rapport 10 %  ▄▁▁▁▁▁▁▁▁▁▄▁▁▁▁▁▁▁▁▁▄▁▁▁▁▁▁▁▁▁
  rapport 50 %  ▄▄▄▄▄▁▁▁▁▁▄▄▄▄▄▁▁▁▁▁▄▄▄▄▄▁▁▁▁▁
  rapport 90 %  ▄▄▄▄▄▄▄▄▄▁▄▄▄▄▄▄▄▄▄▁▄▄▄▄▄▄▄▄▄▁
                |← période →|
```

- **Rapport cyclique** = temps à HIGH / période (0–100 %).
- **Fréquence** = 1 / période.

Pourquoi c'est utile : si la fréquence est assez élevée (> ~100 Hz pour l'œil), une LED pilotée avec un rapport cyclique de 25 % semble être à **25 % de luminosité**, car l'œil moyenne. Les moteurs moyennent aussi (contrôle de vitesse). La PWM est la façon dont une broche numérique « simule » une sortie analogique. C'est la base du Day 03 (couleurs RGB).

À basse fréquence (1 Hz), vous *voyez* le clignotement : avec 10 % à 1 Hz, la LED est allumée 100 ms puis éteinte 900 ms.

### 11.8 Mode Fast PWM

Le compteur compte de 0 → TOP, puis repart à 0 (une seule pente, single slope). En **mode non inversé** (COM1A = 10) :
- à BOTTOM (0) : OC1A passe à **HIGH**
- à l'égalité (TCNT1 == OCR1A) : OC1A passe à **LOW**

```
 TCNT1
   TOP ┤    /|    /|    /|
 OCR1A ┤- -/-|- -/-|- -/-|- - - -
       │  /  |  /  |  /  |
       │ /   | /   | /   |
     0 ┼/────┴/────┴/────┴──► temps
 OC1A  ┌─┐   ┌─┐   ┌─┐
       ┘ └───┘ └───┘ └───         HIGH de 0 à OCR1A, LOW de OCR1A à TOP
```

**Formules :**
- Fréquence PWM : **`f_PWM = F_CPU / (N × (1 + TOP))`**
- Rapport cyclique : **`duty = (OCR1A + 1) / (TOP + 1)`** (non inversé)

**Le mode 14 (TOP = ICR1)** est le plus utile : ICR1 fixe la fréquence, OCR1A (et OCR1B) fixent les rapports cycliques indépendamment.

**Exemple détaillé : LED D2 à 1 Hz avec un rapport cyclique de 10 %, en matériel.**
- f = 1 Hz, N = 256 → TOP = 16e6 / 256 / 1 − 1 = **62 499** (tient sur 16 bits ✓)
- 10 % → OCR1A + 1 = 6 250 → **OCR1A = 6 249**

```c
#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB1);                             // OC1A en sortie

    // Mode 14 : Fast PWM, TOP = ICR1  → WGM13:0 = 1110
    TCCR1A = (1 << COM1A1)                          // non inversé sur OC1A
           | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12);
    ICR1  = 62499;                                  // période = 1 s
    OCR1A = 6249;                                   // 10 % = 100 ms allumée
    TCCR1B |= (1 << CS12);                          // prescaler 256, démarrage

    while (1) { }
}
```

**Réglage du rapport cyclique avec les boutons** (exercice suivant typique : SW1 +10 %, SW2 −10 %, entre 10 % et 100 %) :

```c
uint8_t duty = 10;                     // en pourcents
...
if (sw1_pressed_edge() && duty < 100) duty += 10;
if (sw2_pressed_edge() && duty > 10)  duty -= 10;
OCR1A = (uint32_t)(ICR1 + 1) * duty / 100 - 1;   // calcul en 32 bits pour éviter le débordement !
```

`(ICR1 + 1) * duty` = 62500 × 100 = 6 250 000, ce qui déborde de 16 bits. D'où le cast en `uint32_t`.

**Cas limites du Fast PWM :**
- `OCR1A = TOP` → sortie HIGH pendant toute la période (100 %).
- `OCR1A = 0` → **pas** 0 %. La sortie est HIGH pendant 1 tick du timer par période (une fine impulsion). Pour obtenir un vrai 0 %, déconnectez la broche (mettez COM1A1 à 0) et pilotez-la à LOW, ou utilisez le mode inversé, ou le mode phase correct.

### 11.9 Mode Phase-correct PWM (PWM à phase correcte)

Le compteur compte **en montant** de 0 → TOP, puis **en descendant** de TOP → 0 (double pente, dual slope). La sortie est mise à 0 à l'égalité en montée et mise à 1 à l'égalité en descente. Cela produit une impulsion **symétrique**, centrée dans la période.

- `f_PWM = F_CPU / (2 × N × TOP)`, soit la moitié de la fréquence du fast PWM pour le même TOP.
- OCR = 0 donne un vrai 0 %, et OCR = TOP un vrai 100 %.
- Préféré pour les moteurs. Pour les LEDs, le fast PWM convient très bien.

Le mode « phase *and frequency* correct » (modes 8/9) ne diffère que par le moment où les mises à jour d'OCR/TOP sont appliquées. Il est utile si vous changez TOP à la volée.

### 11.10 Double tampon des registres OCR (double-buffering)

En modes PWM, écrire OCR1A ne prend pas effet immédiatement : la nouvelle valeur est mise en tampon et appliquée à BOTTOM (fast PWM) ou à TOP (phase correct). Cela évite les glitchs (parasites), car un changement au milieu d'une période pourrait produire une impulsion bizarre. En modes Normal/CTC, la mise à jour est immédiate. Donc si en CTC vous abaissez OCR1A en dessous du TCNT1 courant, le compteur rate l'égalité et monte jusqu'à 0xFFFF avant de reboucler (une longue période ponctuelle).

### 11.11 Accès aux registres 16 bits

L'AVR est un processeur 8 bits : un registre 16 bits comme TCNT1 est donc accédé en deux moitiés de 8 bits via un **registre TEMP** caché. La règle : **écrire l'octet de poids fort en premier, lire l'octet de poids faible en premier**. En C, `OCR1A = 31249;` et `x = TCNT1;` génèrent automatiquement le bon ordre. Utilisez simplement les noms 16 bits.

Mais si une interruption accède aussi à des registres 16 bits d'un timer, le registre TEMP partagé peut être corrompu. Protégez ces accès avec `cli()`/`sei()` (Day 04).

### 11.12 Timer0 et Timer2 (8 bits)

Mêmes principes, compteur plus petit (0–255), moins de modes :

**TCCR0A** : `COM0A1 COM0A0 COM0B1 COM0B0 – – WGM01 WGM00`
**TCCR0B** : `FOC0A FOC0B – – WGM02 CS02 CS01 CS00`

| Mode | WGM02 | WGM01 | WGM00 | Fonctionnement | TOP |
|---|---|---|---|---|---|
| 0 | 0 | 0 | 0 | Normal | 0xFF |
| 1 | 0 | 0 | 1 | PWM, phase correct | 0xFF |
| 2 | 0 | 1 | 0 | CTC | OCR0A |
| 3 | 0 | 1 | 1 | **Fast PWM** | **0xFF** |
| 5 | 1 | 0 | 1 | PWM, phase correct | OCR0A |
| 7 | 1 | 1 | 1 | Fast PWM | OCR0A |

Le Timer2 est identique, avec les registres `TCCR2A`, `TCCR2B`, `TCNT2`, `OCR2A`, `OCR2B`, `TIMSK2`, `TIFR2`, et les noms de bits `COM2A1`, `WGM21`, `CS22`… **mais sa table de prescaler est différente** :

| CS22 | CS21 | CS20 | Horloge du Timer2 |
|---|---|---|---|
| 0 | 0 | 1 | clk/1 |
| 0 | 1 | 0 | clk/8 |
| 0 | 1 | 1 | **clk/32** |
| 1 | 0 | 0 | **clk/64** |
| 1 | 0 | 1 | **clk/128** |
| 1 | 1 | 0 | clk/256 |
| 1 | 1 | 1 | clk/1024 |

Un bug classique consiste à recopier les bits CS du Timer0 pour le Timer2 et obtenir la mauvaise fréquence.

**Limitation des modes 5/7 (TOP = OCRnA) :** le canal A sert alors de TOP, donc seul le canal B peut produire une PWM. C'est pourquoi, pour **trois PWM 8 bits indépendantes** (Day 03, RGB), on utilise les modes où TOP = 0xFF.

Fréquence d'une fast PWM 8 bits avec N = 1 : 16e6 / 256 = **62,5 kHz** ; avec N = 8 : 7,8 kHz ; avec N = 64 : 976 Hz. Toutes conviennent aux LEDs.

### 11.13 Choisir un prescaler : la méthode

Pour une période T (ou une fréquence f) cible :
1. Calculez le nombre de cycles CPU : `cycles = T × F_CPU` (ou `F_CPU / f`).
2. Pour chaque prescaler N (du plus petit au plus grand), calculez `TOP = cycles / N − 1`.
3. Prenez le premier N pour lequel TOP ≤ 255 (8 bits) ou 65535 (16 bits).
4. Vérifiez l'erreur d'arrondi si `cycles / N` n'est pas entier.

Valeurs précalculées pour 16 MHz :

| Prescaler | Tick | Fréquence du tick | Période max, 8 bits | Période max, 16 bits |
|---|---|---|---|---|
| 1 | 62,5 ns | 16 MHz | 16 µs | 4,096 ms |
| 8 | 0,5 µs | 2 MHz | 128 µs | 32,77 ms |
| 64 | 4 µs | 250 kHz | 1,024 ms | 262 ms |
| 256 | 16 µs | 62,5 kHz | 4,096 ms | 1,048 s |
| 1024 | 64 µs | 15,625 kHz | 16,38 ms | 4,194 s |

Combinaisons utiles :
- **Tick de 1 ms** (pour un compteur `millis()`) : Timer0 en CTC, N = 64, OCR0A = 249 (250 ticks × 4 µs = 1 ms, exact).
- **Période de 1 s** : Timer1, N = 256, TOP = 62499 (exact).
- **0,5 s** : Timer1, N = 256, TOP = 31249.
- **10 ms** : Timer1, N = 8, TOP = 19999 ; ou Timer2, N = 1024, OCR2A = 155 (~9,98 ms).

### 11.14 Scruter les drapeaux du timer sans interruption

Avant le Day 04, vous pouvez surveiller les drapeaux au lieu d'utiliser des interruptions :

```c
// Timer1 en CTC, période de 1 s, prescaler 256, OCR1A = 62499 (aucune sortie sur broche)
TCCR1B = (1 << WGM12) | (1 << CS12);
OCR1A = 62499;

while (1) {
    if (TIFR1 & (1 << OCF1A)) {       // une égalité a eu lieu
        TIFR1 = (1 << OCF1A);         // effacer le drapeau en ÉCRIVANT 1 (pas |=, pas 0 !)
        do_something_every_second();
    }
    // autre travail...
}
```

C'est un tick d'une seconde **non bloquant** : la boucle continue de tourner et ne réagit que quand le drapeau se lève.

### 11.15 Capture d'entrée (pour les curieux)

Le Timer1 peut **capturer** TCNT1 dans ICR1 quand un front survient sur la broche ICP1 (PB0). On s'en sert pour mesurer précisément des largeurs d'impulsion ou des fréquences (décodage de télécommande IR, capteurs à ultrasons). Vous n'en aurez probablement pas besoin pendant la piscine.

### 11.16 Exercices typiques du Day 01

| Ex | Tâche | Concept clé |
|---|---|---|
| 00 | Faire clignoter D2 à ~1 Hz sans timer ni `_delay_ms` | comptage de cycles, `volatile` |
| 01 | Faire clignoter D2 à 1 Hz avec le Timer1, boucle principale vide, sans interruption | CTC, inversion COM1A |
| 02 | D2 à 1 Hz avec un rapport cyclique de 10 % | Fast PWM mode 14 |
| 03 | SW1/SW2 modifient le rapport cyclique de ±10 % | PWM + boutons + calcul 32 bits |

### 11.17 Pièges

- **Bits WGM répartis entre TCCR1A et TCCR1B.** Mettre `WGM12` dans TCCR1A par erreur active un *autre bit* (le bit 3 de TCCR1A est réservé), car WGM12 n'est que le nombre 3.
- **Utiliser le mauvais registre pour un nom de bit :** le compilateur ne vérifie pas que `COM1A0` appartient à TCCR1A. Tout nom de bit n'est qu'un nombre.
- **Oublier DDR** pour la broche OCnx : le timer tourne, mais la broche ne sort rien.
- **Oublier la sélection d'horloge** : le timer ne démarre jamais (CS = 000 au reset).
- **Les bits de prescaler du Timer2** diffèrent de ceux des Timer0/1.
- **Débordement dans les calculs de rapport cyclique** (`62500 * 100` ne tient pas sur 16 bits).
- **Effacer un drapeau avec `TIFR1 |= (1 << OCF1A)`** : ça marche pour TIFR1, mais cela efface aussi tout autre drapeau qui était levé, car cela réécrit des 1. Écrivez `TIFR1 = (1 << OCF1A);`.
- **Erreur de un (off-by-one)** : le timer compte de 0 à TOP inclus, donc la période fait TOP + 1 ticks. D'où les `− 1` partout.


---

## 12. Day 02 : UART

### 12.1 Objectifs

- Comprendre la **communication série** : asynchrone, trames, débit en bauds (baud rate).
- Configurer le périphérique **USART0** : débit, format de trame.
- Envoyer des caractères et des chaînes (par scrutation, polling), recevoir des caractères (par scrutation, puis par interruption).
- Construire un petit programme interactif dans le terminal de votre PC : écho, inversion de casse, invite de connexion (login).

C'est **votre outil de débogage le plus important** pour tout le reste de la piscine. Une fois l'UART fonctionnel, vous pouvez afficher des valeurs au lieu de deviner ce qui se passe.

### 12.2 Série vs parallèle, synchrone vs asynchrone

- **Parallèle** : 8 fils transportent 8 bits à la fois. Rapide, mais consomme beaucoup de broches.
- **Série** (serial) : un seul fil transporte les bits **les uns après les autres**. Peu de broches. C'est ainsi que presque tout communique aujourd'hui (USB, Ethernet, SATA, UART, SPI, I2C…).

- Une liaison série **synchrone** (SPI, I2C) utilise un fil d'**horloge** séparé qui indique au récepteur exactement quand lire chaque bit.
- Une liaison série **asynchrone** (UART) n'a **pas de fil d'horloge**. Les deux côtés s'accordent *à l'avance* sur la vitesse (le **baud rate**), et le récepteur se resynchronise à chaque octet grâce à un **bit de start**.

**UART** = Universal Asynchronous Receiver/Transmitter (émetteur-récepteur asynchrone universel). Le périphérique de l'AVR s'appelle **USART** (le S signifie qu'il sait aussi faire du mode Synchrone, que vous n'utiliserez pas).

### 12.3 Les fils

```
   ATmega328P                     Puce USB-série (CH340)           PC
   ┌────────┐                     ┌───────────────┐
   │ TXD PD1├────────────────────►│RXD            │
   │ RXD PD0│◄────────────────────┤TXD         USB├═══════ /dev/ttyUSB0
   │    GND ├─────────────────────┤GND            │
   └────────┘                     └───────────────┘
```

- **Le TX de l'un va sur le RX de l'autre** (croisé).
- **La masse (GND) doit être commune.** Une tension n'a de sens que par rapport à une référence partagée.
- Chaque sens est indépendant : c'est du **full duplex** (bidirectionnel simultané), les deux côtés peuvent émettre en même temps.

**Niveaux de tension :** l'AVR utilise des niveaux « TTL » (0 V / 5 V). Les anciens ports série des PC (**RS-232**) utilisaient ±12 V, en logique inversée. Ne les reliez jamais directement. Aujourd'hui, la puce USB-série fait la conversion, et votre PC voit simplement un port série virtuel.

### 12.4 La trame UART

Au repos (idle), la ligne reste à l'état **HAUT**. Pour envoyer un octet :

```
  repos start  D0  D1  D2  D3  D4  D5  D6  D7  stop  repos
  ─────┐     ┌───┐   ┌───────┐   ┌───┐       ┌──────────────
       │     │   │   │       │   │   │       │
       └─────┘   └───┘       └───┘   └───────┘
        0     1   0   1   1   0   1   0   0?  ...
       |<-->| durée d'un bit = 1 / baud rate
```

Trame (frame) pour 'Z' = 0x5A = `0b01011010`, envoyé **LSB en premier** : D0=0, D1=1, D2=0, D3=1, D4=1, D5=0, D6=1, D7=0.

```
  ──┐   ┌───┐   ┌───────┐   ┌───┐   ┌────
    │ S │ 0 │ 1 │ 0 │ 1 1 │ 0 │ 1 │ 0 │ P
    └───┴───┘   └───┘       └───┘   └───┘
    start    D0..D7 (LSB en premier)     stop (HAUT)
```

1. **Bit de start** : 1 bit à l'état BAS. Le front descendant dit au récepteur « un octet commence maintenant ».
2. **Bits de données** : de 5 à 9 (presque toujours **8**), **LSB en premier**.
3. **Bit de parité** (optionnel) : parité paire/impaire pour détecter les erreurs. Généralement **aucune**.
4. **Bit(s) de stop** : 1 ou 2 bits à l'état HAUT. Ils garantissent le retour de la ligne au repos, pour que le bit de start suivant soit visible.

**Notation « 8N1 »** : 8 bits de données, No parity (pas de parité), 1 bit de stop. C'est le réglage par défaut universel. Un octet prend donc **10 durées de bit**.

Le récepteur échantillonne chaque bit en son **milieu** (l'AVR prend 3 échantillons autour du centre et fait un vote majoritaire), en se calant sur le front descendant du bit de start.

### 12.5 Le baud rate

**Baud rate** (débit en bauds) = nombre de symboles (ici de bits) par seconde. Valeurs courantes : 9600, 19200, 38400, 57600, **115200**.

À 115200 bauds :
- un bit dure 1/115200 = **8,68 µs**
- un octet (10 bits en 8N1) prend **86,8 µs**, donc le débit maximal est de **11 520 octets/s**

Les deux côtés doivent utiliser le même baud rate, **à environ ±2–3 % près**. L'horloge du récepteur dérive par rapport à celle de l'émetteur pendant les 10 bits d'une trame ; après ~10 bits, une erreur de 5 % ferait échantillonner le mauvais bit. C'est pourquoi la précision compte.

### 12.6 Génération du baud rate sur l'AVR

L'USART divise F_CPU avec un registre de 12 bits, **UBRR0** (UBRR0H:UBRR0L) :

| Mode | Baud rate | Valeur de UBRR0 |
|---|---|---|
| Normal (U2X0 = 0) | `F_CPU / (16 × (UBRR0 + 1))` | `F_CPU / (16 × BAUD) − 1` |
| Double vitesse (U2X0 = 1) | `F_CPU / (8 × (UBRR0 + 1))` | `F_CPU / (8 × BAUD) − 1` |

**Exemple détaillé à 16 MHz, 115200 bauds :**
- Normal : 16e6 / (16 × 115200) − 1 = 8,68 − 1 = **7,68** → arrondi à **8** → baud réel = 16e6 / (16 × 9) = **111 111** → erreur **−3,5 %** ⚠️ (limite ; peut fonctionner ou non)
- Double vitesse : 16e6 / (8 × 115200) − 1 = 17,36 − 1 = **16,36** → **16** → baud réel = 16e6 / (8 × 17) = **117 647** → erreur **+2,1 %** ✓ (mieux ; c'est ce que fait Arduino)

En C, calculez-le avec un **arrondi** au lieu d'une troncature :

```c
// Vitesse normale, arrondi :  (F_CPU + 8*BAUD) / (16*BAUD) - 1
#define UBRR_VALUE ((F_CPU + 8UL * UART_BAUDRATE) / (16UL * UART_BAUDRATE) - 1)
// Double vitesse, arrondi :  (F_CPU + 4*BAUD) / (8*BAUD) - 1
#define UBRR2X_VALUE ((F_CPU + 4UL * UART_BAUDRATE) / (8UL * UART_BAUDRATE) - 1)
```

L'astuce de l'arrondi : la division entière tronque ; ajouter la moitié du diviseur avant de diviser arrondit donc à l'entier le plus proche. `(a + b/2) / b`.

La datasheet contient des tableaux « Examples of UBRRn Settings for Commonly Used Oscillator Frequencies » avec l'erreur de chaque combinaison. Regardez la colonne 16 MHz : à 115200, U2X=0 → UBRR=8, −3,5 % ; U2X=1 → UBRR=16, 2,1 %.

(avr-libc fournit aussi `<util/setbaud.h>`, qui calcule UBRR_VALUE et USE_2X pour vous, mais votre sujet peut l'interdire.)

### 12.7 Les registres de l'USART

**UDR0** : USART Data Register (registre de données). **Écrivez-y** pour envoyer un octet, **lisez-le** pour obtenir un octet reçu. Derrière cette unique adresse se cachent *deux* tampons séparés : émission (TX) et réception (RX).

**UCSR0A** : Control and Status Register A (registre de contrôle et d'état A)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | **RXC0** | **TXC0** | **UDRE0** | FE0 | DOR0 | UPE0 | **U2X0** | MPCM0 |

- **RXC0** : Receive Complete (réception terminée). Un octet attend dans UDR0.
- **TXC0** : Transmit Complete (émission terminée). Tout a été entièrement envoyé (bit de stop compris).
- **UDRE0** : Data Register Empty (registre de données vide). Vous pouvez écrire l'octet suivant dans UDR0. **(Valeur initiale 1.)**
- FE0 / DOR0 / UPE0 : erreur de trame (frame error), dépassement de données (data overrun : vous n'avez pas lu assez vite), erreur de parité.
- U2X0 : double vitesse.

**UCSR0B** : Control and Status Register B
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | **RXCIE0** | TXCIE0 | UDRIE0 | **RXEN0** | **TXEN0** | UCSZ02 | RXB80 | TXB80 |

- **RXEN0 / TXEN0** : activent le récepteur / l'émetteur. Une fois activés, ils prennent le contrôle de PD0/PD1 (pas besoin de configurer DDR).
- **RXCIE0 / TXCIE0 / UDRIE0** : activation des interruptions « réception terminée », « émission terminée », « registre de données vide ».

**UCSR0C** : Control and Status Register C
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | UMSEL01 | UMSEL00 | UPM01 | UPM00 | USBS0 | **UCSZ01** | **UCSZ00** | UCPOL0 |

- UMSEL0[1:0] : 00 = USART asynchrone (ce que vous voulez)
- UPM0[1:0] : parité (00 = aucune)
- USBS0 : bits de stop (0 = 1 bit de stop)
- UCSZ0[2:0] (UCSZ02 est dans UCSR0B !) : taille des caractères, 011 = **8 bits**
- **Valeur de reset : 0x06** = UCSZ01 et UCSZ00 à 1 = **déjà 8N1**. L'écrire explicitement reste une bonne pratique.

**UBRR0H / UBRR0L** (ou `UBRR0` comme nom 16 bits) : le diviseur de baud rate.

### 12.8 Le tampon d'émission : UDRE0 vs TXC0

```
  votre code ──écrit──► [ UDR0 (tampon TX) ] ──► [ registre à décalage ] ──► broche TX, bit par bit
                              │                          │
                      UDRE0 = 1 quand vide       TXC0 = 1 quand le registre à décalage
                      (vous pouvez réécrire)      est vide ET qu'il n'y a pas de nouvelle donnée
```

L'émetteur a un **double tampon** (double-buffered) : vous pouvez écrire un nouvel octet dans UDR0 pendant que le précédent est encore en train d'être décalé (shifted out). Donc :
- Avant d'écrire : **attendez UDRE0**. C'est l'approche standard.
- TXC0 ne sert que quand il faut savoir que l'émission est *physiquement* terminée (par ex. avant de passer en veille, ou avant d'inverser le sens d'un transceiver RS-485).

### 12.9 Pilote UART minimal (par scrutation)

```c
#include <avr/io.h>

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

void uart_init(void)
{
    // Mode double vitesse pour une meilleure précision à 115200 @ 16 MHz
    uint16_t ubrr = (F_CPU + 4UL * UART_BAUDRATE) / (8UL * UART_BAUDRATE) - 1;  // = 16

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0A = (1 << U2X0);                          // double vitesse
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);          // activer RX et TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8 bits de données, pas de parité, 1 bit de stop
}

void uart_tx(char c)
{
    while (!(UCSR0A & (1 << UDRE0))) { }           // attendre que le tampon TX soit vide
    UDR0 = c;                                       // lancer l'émission
}

char uart_rx(void)
{
    while (!(UCSR0A & (1 << RXC0))) { }            // attendre l'arrivée d'un octet
    return UDR0;                                    // lire UDR0 efface RXC0
}

void uart_printstr(const char *str)
{
    while (*str)
        uart_tx(*str++);
}

int main(void)
{
    uart_init();
    while (1)
    {
        char c = uart_rx();     // lecture bloquante
        uart_tx(c);             // le renvoyer en écho
    }
}
```

Si votre sujet passe le baud rate avec un autre nom de macro ou impose le mode normal, adaptez en conséquence. Avec U2X=0 à 115200, l'erreur de −3,5 % fonctionne généralement quand même avec les adaptateurs CH340, mais c'est moins robuste.

Ensuite, sur votre PC : `screen /dev/ttyUSB0 115200`, tapez des caractères, et ils apparaissent (renvoyés en écho par le MCU).

### 12.10 Enchaînement typique des exercices

**Ex00 : envoyer 'Z' à 1 Hz.**
```c
int main(void) {
    uart_init();
    while (1) {
        uart_tx('Z');
        _delay_ms(1000);       // ou un drapeau / une interruption du Timer1
    }
}
```

**Ex01 : afficher « Hello World! » toutes les 2 secondes, piloté par un timer.** Le sujet peut exiger l'*interruption* du timer et une boucle principale vide. On utilise alors par anticipation des notions du Day 04 :

```c
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect)
{
    uart_printstr("Hello World!\r\n");   // ~14 octets × 87 µs ≈ 1,2 ms : acceptable dans une ISR ici
}

int main(void)
{
    uart_init();
    // Timer1 CTC, prescaler 1024 : 2 s = 32 000 000 cycles / 1024 = 31250 ticks → OCR1A = 31249
    OCR1A  = 31249;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    TIMSK1 = (1 << OCIE1A);            // activer l'interruption de comparaison A
    sei();                              // activation globale des interruptions
    while (1) { }
}
```

(2 s avec un prescaler de 256 nécessiterait 125 000 ticks > 65535, d'où le prescaler 1024.)

**Ex02 : écho par scrutation** (le code du §12.9).

**Ex03 : réception par interruption, inversion de la casse, écho :**
```c
ISR(USART_RX_vect)
{
    char c = UDR0;                       // lecture : efface le drapeau d'interruption
    if (c >= 'a' && c <= 'z')      c -= 32;   // 'a' - 'A' == 32
    else if (c >= 'A' && c <= 'Z') c += 32;
    UDR0 = c;       // sans risque ici : 1 octet reçu ⇒ le précédent est parti depuis longtemps (même débit)
}

int main(void)
{
    uart_init();
    UCSR0B |= (1 << RXCIE0);             // activer l'interruption « réception terminée »
    sei();
    while (1) { }
}
```

Astuce de casse : en ASCII, minuscules et majuscules ne diffèrent que par le bit 5 (0x20) : `c ^= 0x20` inverse la casse d'une lettre.

**Ex04 : invite nom d'utilisateur / mot de passe.** Le programme demande :
```
Enter your login:
    username: spectre
    password: ********
Hello spectre!
Shall we play a game?
```
Les exigences sont typiquement : afficher en écho les caractères tapés pour le nom d'utilisateur, afficher `*` pour le mot de passe, gérer le **retour arrière** (Backspace : effacer le dernier caractère à l'écran), **Entrée** valide, comparer avec des identifiants codés en dur, et allumer des LEDs / afficher un message d'erreur.

Une fonction de lecture de ligne avec écho et gestion du retour arrière :

```c
#define BS  0x08
#define DEL 0x7F

// Lit une ligne dans buf (size-1 caractères max). Si mask est non nul, l'affiche à la place des caractères.
void uart_readline(char *buf, uint8_t size, char mask)
{
    uint8_t len = 0;
    while (1)
    {
        char c = uart_rx();
        if (c == '\r' || c == '\n')            // Entrée
        {
            uart_printstr("\r\n");
            break;
        }
        else if (c == DEL || c == BS)          // Retour arrière
        {
            if (len > 0)
            {
                len--;
                uart_printstr("\b \b");        // reculer, écraser par un espace, reculer
            }
        }
        else if (c >= ' ' && c <= '~' && len < size - 1)   // ASCII imprimable
        {
            buf[len++] = c;
            uart_tx(mask ? mask : c);
        }
        // ignorer tout le reste (les flèches envoient des séquences d'échappement : ESC [ A, etc.)
    }
    buf[len] = '\0';
}
```

`"\b \b"` est l'astuce classique pour effacer un caractère dans un terminal : `\b` recule le curseur sans effacer, l'espace écrase le caractère, et un second `\b` recule à nouveau.

**Touches fléchées et séquences d'échappement (escape sequences) :** appuyer sur ↑ envoie 3 octets : `ESC` (0x1B), `[`, `A`. Un lecteur de ligne robuste les ignore (une petite machine à états : après ESC, sauter `[` et l'octet suivant).

### 12.11 Émission par interruption avec un tampon circulaire (ring buffer)

`uart_tx` par scrutation bloque ~87 µs par caractère. Pour un long message, cela fait de nombreuses millisecondes pendant lesquelles rien d'autre ne s'exécute. La solution professionnelle : placer les caractères dans un **tampon circulaire** (ring buffer, file FIFO circulaire), et laisser l'**interruption UDRE** les envoyer en arrière-plan.

```c
#include <avr/interrupt.h>

#define TX_BUF_SIZE 64                       // puissance de 2 → modulo peu coûteux avec & (SIZE-1)
static volatile char    tx_buf[TX_BUF_SIZE];
static volatile uint8_t tx_head = 0;         // où le prochain caractère est écrit
static volatile uint8_t tx_tail = 0;         // où le prochain caractère est lu

void uart_tx_async(char c)
{
    uint8_t next = (tx_head + 1) & (TX_BUF_SIZE - 1);
    while (next == tx_tail) { }              // tampon plein : attendre (l'ISR libère de la place)
    tx_buf[tx_head] = c;
    tx_head = next;
    UCSR0B |= (1 << UDRIE0);                 // activer l'interruption « registre de données vide »
}

ISR(USART_UDRE_vect)
{
    if (tx_head == tx_tail) {                // plus rien à envoyer
        UCSR0B &= ~(1 << UDRIE0);            // désactiver, sinon elle se déclenche indéfiniment
        return;
    }
    UDR0 = tx_buf[tx_tail];
    tx_tail = (tx_tail + 1) & (TX_BUF_SIZE - 1);
}
```

Fonctionnement d'un tampon circulaire :
```
  indice :  0   1   2   3   4   5   6   7
          [ ] [ ] [H] [e] [l] [ ] [ ] [ ]
                   ↑           ↑
                  tail        head
  - écrire à head, puis head++ (avec rebouclage)
  - lire à tail,  puis tail++ (avec rebouclage)
  - vide quand head == tail
  - plein quand head + 1 == tail (une case est laissée libre pour distinguer plein/vide)
```

La même structure sert côté réception : l'ISR RX empile les octets reçus, et la boucle principale les dépile quand elle a le temps. Ainsi aucun octet n'est perdu pendant que la boucle principale est occupée.

**Producteur unique / consommateur unique (single-producer / single-consumer) :** seul le code principal écrit `tx_head`, et seule l'ISR écrit `tx_tail`. Tant que chaque indice est sur 8 bits (lu/écrit de manière atomique), aucun verrouillage n'est nécessaire.

### 12.12 Afficher des nombres et des sorties formatées

Utilisez les utilitaires du §7.9 (`uart_putnbr`, `uart_puthex8`). En voici d'autres :

```c
void uart_puthex16(uint16_t v) { uart_puthex8(v >> 8); uart_puthex8(v & 0xFF); }

// afficher une valeur avec un nombre fixe de décimales : 2347 avec 2 décimales → "23.47"
void uart_putfixed(int32_t v, uint8_t decimals)
{
    int32_t div = 1;
    for (uint8_t i = 0; i < decimals; i++) div *= 10;
    if (v < 0) { uart_tx('-'); v = -v; }
    uart_putnbr(v / div);
    uart_tx('.');
    int32_t frac = v % div;
    for (int32_t d = div / 10; d > 0; d /= 10) {   // zéros de tête : 5 → "05"
        uart_tx('0' + (frac / d) % 10);
    }
}
```

**À propos de `printf` :** avr-libc *fournit* `printf`, via `fdevopen` ou `FDEV_SETUP_STREAM` pour rediriger stdout vers `uart_tx`. Cela coûte ~1,5–2 Ko de flash, et les floats nécessitent des options de liaison supplémentaires. Votre sujet l'interdit probablement, mais c'est utile dans vos projets personnels :

```c
#include <stdio.h>
static int uart_putchar(char c, FILE *s) { (void)s; if (c == '\n') uart_tx('\r'); uart_tx(c); return 0; }
static FILE uart_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
// dans main : stdout = &uart_out;  printf("x = %d\n", x);
```

### 12.13 Codes d'échappement ANSI : embellir la sortie du terminal

Les terminaux interprètent des séquences spéciales commençant par ESC (`\x1b` ou `\033`) :

| Séquence | Effet |
|---|---|
| `\x1b[2J` | effacer l'écran |
| `\x1b[H` | curseur en haut à gauche |
| `\x1b[K` | effacer jusqu'à la fin de la ligne |
| `\x1b[<ligne>;<col>H` | déplacer le curseur |
| `\x1b[31m` … `\x1b[37m` | texte rouge, vert, jaune, bleu, magenta, cyan, blanc |
| `\x1b[1m` | gras |
| `\x1b[0m` | réinitialiser les attributs |
| `\x1b[?25l` / `\x1b[?25h` | masquer / afficher le curseur |

Exemple : `uart_printstr("\x1b[32mACCESS GRANTED\x1b[0m\r\n");` affiche en vert. Pratique pour les rushes et les tableaux de bord.

### 12.14 Pièges

- **Mauvais F_CPU ou baud rate** → caractères illisibles (`�`, symboles aléatoires). C'est le bug UART n°1. Vérifiez les defines du Makefile, la cohérence entre U2X0 et la formule, et la vitesse du terminal.
- **Terminal ouvert pendant le flashage** → avrdude ne peut pas ouvrir le port.
- **Envoyer `\n` sans `\r`** → texte en escalier.
- **Attendre `\n` à l'appui sur Entrée** → les terminaux envoient `\r`.
- **Lire UDR0 deux fois** → la seconde lecture obtient l'octet *suivant* (ou n'importe quoi).
- **Activer UDRIE0 sans rien à envoyer** → l'interruption se déclenche en continu, et main ne s'exécute jamais.
- **Longues opérations dans les ISR** (afficher une grande chaîne à 115200 dans une ISR) : cela bloque les autres interruptions. Acceptable pour des exercices simples, mauvais pendant les rushes.
- **Débordement en réception** (RX overrun, DOR0) : si vous ne lisez pas les octets assez vite (par ex. pendant un `_delay_ms(1000)`), les octets entrants sont perdus. Le tampon matériel ne contient que 2 octets.

---

## 13. Day 03 : RGB

### 13.1 Objectifs

- Comprendre la **synthèse additive des couleurs** et le fonctionnement d'une LED RGB.
- Piloter trois canaux de LED en sorties numériques, puis avec **trois PWM matérielles simultanées** (Timer0 + Timer2).
- Implémenter une **roue des couleurs** (color wheel, arc-en-ciel).
- Analyser un code couleur hexadécimal (`#RRGGBB`) reçu par UART.

### 13.2 Lumière et couleur

Les écrans et les LEDs utilisent la synthèse **additive** : on part du noir (aucune lumière) et on *ajoute* de la lumière rouge, verte et bleue.

```
            Rouge + Vert         = Jaune
            Vert + Bleu          = Cyan
            Bleu + Rouge         = Magenta
            Rouge + Vert + Bleu  = Blanc
            rien                 = Noir (éteint)
```

(La peinture et l'impression utilisent la synthèse **soustractive**, avec cyan, magenta et jaune : d'autres règles.)

Avec 8 bits par canal (0–255), on obtient 256³ = **16,7 millions de couleurs**, notées **`#RRGGBB`** en hexadécimal :

| Couleur | Hexa | R | G | B |
|---|---|---|---|---|
| Rouge | `#FF0000` | 255 | 0 | 0 |
| Vert | `#00FF00` | 0 | 255 | 0 |
| Bleu | `#0000FF` | 0 | 0 | 255 |
| Jaune | `#FFFF00` | 255 | 255 | 0 |
| Cyan | `#00FFFF` | 0 | 255 | 255 |
| Magenta | `#FF00FF` | 255 | 0 | 255 |
| Blanc | `#FFFFFF` | 255 | 255 | 255 |
| Orange | `#FF8000` | 255 | 128 | 0 |
| Violet | `#800080` | 128 | 0 | 128 |
| Un vert « 42 » | `#00BABC` | 0 | 186 | 188 |

### 13.3 La LED RGB

Une LED RGB, ce sont **trois LEDs (puces, dies) dans un seul boîtier** : rouge, verte, bleue. Elles partagent une broche commune :

```
  Cathode commune :                  Anode commune :
  R ──►|──┐                          VCC ──┬──►|── R
  G ──►|──┼── GND                          ├──►|── G
  B ──►|──┘                                └──►|── B
  broche HIGH = couleur ALLUMÉE      broche LOW = couleur ALLUMÉE (actif à l'état bas)
```

Chaque couleur a sa propre résistance de limitation de courant. **Vérifiez le schéma** pour savoir de quel type est la vôtre. La D5 de la devboard typique est pilotée à l'état haut (cathode commune, common cathode), mais vérifiez ! Avec une LED à anode commune (common anode), il faudrait inverser la logique (ou utiliser le **mode PWM inversé**, COMnx1:0 = 11).

Sur la devboard typique : **Rouge = PD5 (OC0B), Vert = PD6 (OC0A), Bleu = PD3 (OC2B)**. Les trois sont des broches de PWM matérielle, et c'est voulu.

### 13.4 D'abord des couleurs numériques (sans PWM)

Avec seulement allumé/éteint par canal, on obtient 8 combinaisons (2³) : noir, les 3 primaires, les 3 secondaires et le blanc.

```c
#include <avr/io.h>
#include <util/delay.h>

#define R (1 << PD5)
#define G (1 << PD6)
#define B (1 << PD3)

static void set_color(uint8_t mask)
{
    PORTD = (PORTD & ~(R | G | B)) | mask;
}

int main(void)
{
    const uint8_t seq[] = { R, G, B, R | G, G | B, R | B, R | G | B };   // rouge, vert, bleu, jaune, cyan, magenta, blanc
    DDRD |= R | G | B;
    while (1)
        for (uint8_t i = 0; i < sizeof(seq); i++) {
            set_color(seq[i]);
            _delay_ms(1000);
        }
}
```

### 13.5 Trois PWM matérielles

Pour un vrai mélange de couleurs, chaque canal a besoin de sa propre luminosité (0–255), ce qui signifie une **PWM sur trois broches** :
- PD6 = OC0A et PD5 = OC0B → **Timer0**, les deux canaux
- PD3 = OC2B → **Timer2**, canal B

Utilisez le **Fast PWM 8 bits avec TOP = 0xFF** (mode 3) sur les deux timers, pour qu'OCR0A, OCR0B et OCR2B soient tous libres de fixer des rapports cycliques :

```c
void init_rgb(void)
{
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

    // Timer0 : Fast PWM 8 bits (mode 3 : WGM01 = WGM00 = 1), non inversé sur OC0A et OC0B
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);           // prescaler 64 → 16e6/64/256 ≈ 976 Hz

    // Timer2 : Fast PWM 8 bits (mode 3), non inversé sur OC2B
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS22);                          // Timer2 : CS22 seul = prescaler 64 (!)
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r;     // PD5 = rouge
    OCR0A = g;     // PD6 = vert
    OCR2B = b;     // PD3 = bleu
}
```

Notez le **piège du prescaler du Timer2** : pour /64, le Timer0 utilise `CS01|CS00` mais le Timer2 utilise `CS22`. Utiliser la même *fréquence* sur tous les canaux n'est pas strictement obligatoire, mais c'est plus cohérent.

Rappelez-vous la limite du fast PWM : `OCR = 0` produit quand même une impulsion de 1/256 (une faible lueur). Pour un vrai noir, utilisez la PWM phase correct (mode 1 : WGM00 seul), qui atteint vraiment 0 % et 100 %. Ou bien, pour la valeur 0, déconnectez la sortie (mettez COMnx1 à 0) et pilotez la broche à LOW.

### 13.6 La roue des couleurs (color wheel)

Une « roue » associe une position 0–255 à une couleur qui parcourt en douceur l'arc-en-ciel. Le sujet fournira probablement cette fonction (c'est la célèbre fonction NeoPixel d'Adafruit) :

```c
void wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}
```

Déroulons-la (rappelez-vous que `pos` est d'abord inversé) :

| `pos` en entrée | branche prise | résultat |
|---|---|---|
| 0 | troisième (inversé 255 → 85) | (255, 0, 0) **rouge** |
| ~42 | troisième | (~128, ~128, 0) **tirant sur le jaune** |
| 85 | troisième (inversé 170 → 0) | (0, 255, 0) **vert** |
| ~128 | deuxième | (0, ~128, ~128) **tirant sur le cyan** |
| 170 | deuxième (inversé 85 → 0) | (0, 0, 255) **bleu** |
| ~213 | première | (~128, 0, ~128) **tirant sur le magenta** |
| 255 | première (inversé 0) | (255, 0, 0) **rouge** à nouveau |

```
  pos :     0 ───────────── 85 ───────────── 170 ───────────── 255
  couleur : rouge → jaune → vert → cyan → bleu → magenta → rouge
         (dans chaque tiers, un canal descend pendant que le suivant monte ;
          la somme R+G+B reste ~255, donc la luminosité reste à peu près constante)
```

Utilisation : `while (1) { for (uint16_t i = 0; i < 256; i++) { wheel(i); _delay_ms(10); } }` donne un arc-en-ciel fluide d'une période d'environ 2,5 s. Attention à la variable de boucle : avec `uint8_t i; i < 256`, la boucle ne se termine jamais, car un `uint8_t` est toujours < 256. Utilisez un `uint16_t`, ou laissez volontairement un `uint8_t` reboucler avec `for (uint8_t i = 0;; i++)`.

### 13.7 Analyser `#RRGGBB` reçu par UART

Exercice typique : l'utilisateur tape `#FF8000` + Entrée, et la LED devient orange. Une saisie invalide affiche une erreur.

```c
static int8_t hexval(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Convertit 2 caractères hexa dans *out. Renvoie 0 en cas de succès, -1 en cas d'erreur.
static int8_t parse_byte(const char *s, uint8_t *out)
{
    int8_t hi = hexval(s[0]), lo = hexval(s[1]);
    if (hi < 0 || lo < 0) return -1;
    *out = (hi << 4) | lo;
    return 0;
}

int main(void)
{
    char line[16];
    uint8_t r, g, b;

    uart_init();
    init_rgb();
    while (1)
    {
        uart_printstr("Enter color (#RRGGBB): ");
        uart_readline(line, sizeof(line), 0);                 // du Day 02
        if (line[0] == '#' && ft_strlen(line) == 7
            && !parse_byte(line + 1, &r)
            && !parse_byte(line + 3, &g)
            && !parse_byte(line + 5, &b))
            set_rgb(r, g, b);
        else
            uart_printstr("Invalid format\r\n");
    }
}
```

**Validez strictement** : longueur exacte, préfixe `#`, uniquement des chiffres hexadécimaux. Les correcteurs *vont* taper n'importe quoi, et votre programme ne doit ni planter ni produire des couleurs étranges.

### 13.8 Luminosité perçue et correction gamma (bonus)

L'œil humain perçoit la luminosité de façon **non linéaire** : passer de PWM 0 → 25 semble un énorme changement, tandis que 230 → 255 est presque invisible. Pour que les fondus paraissent fluides et linéaires à l'œil, appliquez une **correction gamma** (gamma correction) : `out = 255 × (in / 255)^2,2`.

Précalculez une table de 256 entrées (en flash avec PROGMEM), ou utilisez une approximation peu coûteuse : `out = (in * in) >> 8` (gamma 2,0).

```c
static inline uint8_t gamma8(uint8_t v) { return ((uint16_t)v * v) >> 8; }
set_rgb(gamma8(r), gamma8(g), gamma8(b));
```

De plus, les trois puces n'ont pas le même rendement : des valeurs PWM égales ne donnent pas un blanc parfait. Les produits réels étalonnent chaque canal.

### 13.9 Le modèle de couleur HSV (bonus)

La roue est une correspondance grossière de la **teinte** (hue). Le modèle **HSV** (Hue, Saturation, Value : teinte, saturation, valeur) est plus intuitif pour les animations : faites tourner la teinte pour un arc-en-ciel, baissez la saturation pour des couleurs pastel, baissez la valeur pour atténuer. Il existe de nombreuses implémentations HSV→RGB en entiers uniquement. Cherchez « hsv2rgb integer 8-bit » si vous voulez des effets fluides pendant les rushes.

### 13.10 Exercices typiques du Day 03

| Ex | Tâche | Concept clé |
|---|---|---|
| 00 | D5 alterne rouge → vert → bleu, 1 s chacun | GPIO sur 3 canaux |
| 01 | Cycle rouge, vert, bleu, jaune, cyan, magenta, blanc | synthèse additive |
| 02 | `init_rgb`, `set_rgb` et l'arc-en-ciel `wheel` en PWM | Fast PWM Timer0 + Timer2 |
| 03 | Couleur fixée par une saisie UART `#RRGGBB` | analyse, validation |

### 13.11 Pièges

- Couleurs inversées : vérifiez quelle broche est R, G, B sur *votre* schéma.
- Un canal ne s'allume jamais : la broche OCnx n'est pas configurée en sortie, les bits COM sont dans le mauvais registre, ou l'horloge du timer n'est pas démarrée.
- Les bits de prescaler du Timer2 (encore).
- LED jamais complètement éteinte : l'impulsion du fast PWM avec OCR=0.
- Compteur de boucle `uint8_t` comparé à 256.
- Accepter une saisie mal formée (`#GG0000`, `#FFF`, `FF0000`).


---

## 14. Day 04 : Interruptions

### 14.1 Objectifs

- Comprendre ce qu'est une interruption (interrupt) et ce que fait le CPU quand elle survient.
- Écrire des **ISR** (Interrupt Service Routines, routines de service d'interruption) avec `<avr/interrupt.h>`.
- Utiliser les **interruptions externes** (INT0/INT1), les **interruptions de changement d'état de broche** (pin change interrupts, PCINT) et les **interruptions de timer**.
- Maîtriser les dangers : `volatile`, **atomicité**, conditions de course (race conditions), durée des ISR.

### 14.2 Scrutation (polling) vs interruptions

La **scrutation** (polling), c'est demander sans arrêt « est-ce que c'est arrivé ? » :
```c
while (1) {
    if (button_pressed()) ...
    if (uart_byte_ready()) ...
    if (timer_flag_set()) ...
}
```
Les inconvénients : le CPU est toujours occupé, le temps de réaction dépend de la durée de la boucle, et un événement peut être manqué si la boucle est occupée ailleurs.

Avec les **interruptions**, le matériel *tape sur l'épaule* du CPU quand un événement se produit. Le CPU met en pause ce qu'il fait, exécute une fonction spéciale (l'ISR), puis reprend exactement là où il en était.

Analogie : la scrutation, c'est vérifier sa boîte aux lettres toutes les 30 secondes ; une interruption, c'est la sonnette.

### 14.3 Ce qui se passe, étape par étape

```
  code principal ... instr N ...                          ... instr N+1 ...
                          │                                ▲
   événement ! (drapeau levé)                              │
                          ▼                                │
     1. le CPU termine l'instruction en cours              │
     2. empile l'adresse de retour (PC) sur la pile        │
     3. met à 0 le bit I de SREG (pas d'imbrication)       │
     4. saute à l'entrée de la table des vecteurs → ISR    │
                          │                                │
                     ┌────▼─────────────────────────┐      │
                     │ prologue de l'ISR : empile   │      │
                     │   les registres, sauve SREG  │      │
                     │ corps de votre ISR           │      │
                     │ épilogue : restaure SREG,    │      │
                     │   dépile les registres       │      │
                     │ RETI : retour + remet I à 1 ─┼──────┘
                     └──────────────────────────────┘
```

- **Latence** : au moins 4 cycles pour entrer, plus le prologue du compilateur (sauvegarde des registres), typiquement ~20–60 cycles au total, soit 1–4 µs à 16 MHz. Très rapide comparé à une scrutation dans une longue boucle.
- Le compilateur génère automatiquement le prologue et l'épilogue quand vous utilisez la macro `ISR()`. Il sauvegarde chaque registre utilisé par l'ISR, plus SREG, pour que le code interrompu ne s'aperçoive de rien.

### 14.4 Les trois conditions pour qu'une interruption se déclenche

1. L'**événement se produit** et le matériel lève son **drapeau** (flag, ex. `INTF0` dans `EIFR`, `OCF1A` dans `TIFR1`).
2. Le bit d'**activation individuelle** de cette source est à 1 (ex. `INT0` dans `EIMSK`, `OCIE1A` dans `TIMSK1`).
3. L'**activation globale des interruptions** (le bit I de SREG) est à 1, via `sei()`.

Si les trois sont vraies, l'ISR s'exécute. La plupart des drapeaux sont **effacés automatiquement** au démarrage de l'ISR, mais pas tous (voir §14.10).

### 14.5 Écrire une ISR

```c
#include <avr/interrupt.h>

ISR(INT0_vect)          // nom du vecteur, depuis la table du §8.9 / <avr/iom328p.h>
{
    // restez BREF
}
```

- `sei()` : **active** globalement les interruptions (set, I = 1)
- `cli()` : **désactive** globalement les interruptions (clear, I = 0)
- Une ISR ne prend pas d'argument et ne renvoie rien. Elle communique avec le reste du programme via des **variables globales `volatile`**.
- Si vous faites une faute de frappe dans le nom du vecteur, gcc avertit (`'INT00_vect' appears to be a misspelled signal handler`). **N'ignorez pas cet avertissement** : l'ISR ne sera pas placée dans la table des vecteurs, et la puce redémarrera quand l'interruption se déclenchera.
- `ISR(BADISR_vect) { }` intercepte toute interruption sans gestionnaire, ce qui est utile pour déboguer.

### 14.6 Interruptions externes : INT0 et INT1

INT0 est sur **PD2** (SW1 sur la devboard typique), INT1 sur **PD3**. Elles peuvent se déclencher sur un niveau ou sur un front.

**EICRA** : External Interrupt Control Register A (registre de contrôle des interruptions externes)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | – | – | – | – | ISC11 | ISC10 | ISC01 | ISC00 |

| ISCn1 | ISCn0 | Se déclenche sur |
|---|---|---|
| 0 | 0 | **niveau bas** (se déclenche en continu tant que la broche est basse !) |
| 0 | 1 | **tout changement logique** |
| 1 | 0 | **front descendant** |
| 1 | 1 | **front montant** |

**EIMSK** : External Interrupt Mask (masque) : `INT1` (bit 1) et `INT0` (bit 0) les activent.
**EIFR** : External Interrupt Flag (drapeaux) : `INTF1`, `INTF0` (effacés en écrivant 1, ou automatiquement à l'exécution de l'ISR).

**Exemple : inverser la LED D1 à chaque appui sur SW1, avec des interruptions :**

```c
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(INT0_vect)
{
    PINB = (1 << PB0);                 // inverser la LED D1
}

int main(void)
{
    DDRB |= (1 << PB0);
    PORTD |= (1 << PD2);               // pull-up sur SW1

    EICRA = (1 << ISC01);              // front descendant sur INT0 (appui, actif à l'état bas)
    EIFR  = (1 << INTF0);              // effacer tout drapeau en attente avant d'activer
    EIMSK = (1 << INT0);               // activer INT0
    sei();                             // activer globalement les interruptions

    while (1) { }                      // rien à faire ici
}
```

**Les rebonds sont toujours là !** Un appui produit plusieurs fronts descendants → plusieurs interruptions → des inversions aléatoires. Solutions dans le monde des interruptions :

1. **Désactiver INT0 dans l'ISR et la réactiver plus tard** depuis une ISR de timer (après ~20–50 ms) :
   ```c
   ISR(INT0_vect) {
       PINB = (1 << PB0);
       EIMSK &= ~(1 << INT0);          // ignorer les fronts suivants
       TCNT0 = 0; TIMSK0 |= (1 << OCIE0A);   // lancer un délai anti-rebond de ~16 ms (Timer0 CTC configuré ailleurs)
   }
   ISR(TIMER0_COMPA_vect) {
       static uint8_t n = 0;
       if (++n >= 2) {                 // par ex. 2 × 16 ms
           n = 0;
           TIMSK0 &= ~(1 << OCIE0A);
           EIFR = (1 << INTF0);        // oublier les fronts survenus entre-temps
           EIMSK |= (1 << INT0);       // réécouter
       }
   }
   ```
2. **Approche par horodatage** (timestamp) : maintenir un compteur `millis` depuis une ISR de timer ; dans INT0, ignorer le front si moins de 20 ms se sont écoulées depuis le dernier front accepté.
3. **Se contenter de lever un drapeau** dans l'ISR, et faire l'anti-rebond dans la boucle principale.
4. `_delay_ms(20)` dans l'ISR : ça « marche » dans les exercices, mais c'est une **mauvaise pratique**, car cela bloque toutes les autres interruptions pendant 20 ms. Certains correcteurs vous demanderont pourquoi vous l'avez fait.

### 14.7 Interruptions de changement d'état de broche (PCINT)

INT0/INT1 n'existent que sur deux broches. Mais **toutes** les broches d'E/S peuvent déclencher une **interruption de changement d'état** (pin change interrupt), avec des limitations :
- Elle se déclenche sur **tout changement** (les deux fronts) : impossible de choisir montant/descendant.
- Les broches sont regroupées par port, avec **un vecteur par port** :

| Groupe | Broches | Vecteur | Registre de masque | Bit d'activation dans PCICR |
|---|---|---|---|---|
| PCINT0–7 | PB0–PB7 | `PCINT0_vect` | `PCMSK0` | `PCIE0` |
| PCINT8–14 | PC0–PC6 | `PCINT1_vect` | `PCMSK1` | `PCIE1` |
| PCINT16–23 | PD0–PD7 | `PCINT2_vect` | `PCMSK2` | `PCIE2` |

Donc SW2 sur PD4 = **PCINT20** → vecteur `PCINT2_vect`, bit de masque `PCINT20` dans `PCMSK2`, activation du groupe `PCIE2` dans `PCICR`.

Dans l'ISR, vous devez déterminer **quelle broche a changé et dans quel sens** en lisant le port :

```c
volatile uint8_t sw2_presses = 0;

ISR(PCINT2_vect)
{
    static uint8_t prev = (1 << PD4);          // relâché = 1
    uint8_t now = PIND & (1 << PD4);
    if (prev && !now)                          // 1 → 0 : front descendant = appui
        sw2_presses++;
    prev = now;
}

void init_sw2_interrupt(void)
{
    PORTD |= (1 << PD4);                       // pull-up
    PCMSK2 |= (1 << PCINT20);                  // surveiller uniquement PD4
    PCIFR = (1 << PCIF2);                      // effacer le drapeau en attente
    PCICR |= (1 << PCIE2);                     // activer le groupe PCINT[23:16]
}
```

(Si plusieurs broches d'un même port sont activées dans PCMSK2, elles déclenchent toutes le même `PCINT2_vect`. Comparez l'ancien et le nouvel état de chaque bit pour savoir lequel a bougé.)

### 14.8 Données partagées : `volatile` et atomicité

**Règle 1 : les variables partagées entre une ISR et le code principal doivent être `volatile`.** (Voir §7.1.)

**Règle 2 : les variables sur plusieurs octets doivent être accédées de manière atomique.** L'AVR est un processeur 8 bits : lire un `uint16_t` prend deux instructions. Une interruption peut se déclencher **entre les deux** :

```c
volatile uint16_t ticks;     // incrémentée par une ISR de timer

ISR(TIMER0_COMPA_vect) { ticks++; }

// main :
uint16_t t = ticks;          // compilé en : charger l'octet bas ... [l'ISR peut survenir ici !] ... charger l'octet haut
```

Scénario : `ticks = 0x00FF`. Main lit l'octet bas `0xFF`. L'ISR se déclenche et `ticks` devient `0x0100`. Main lit l'octet haut `0x01`. Résultat : `t = 0x01FF`, ce qui est **complètement faux** (de 256).

Correction : **désactiver les interruptions pendant la lecture** :
```c
uint16_t t;
cli();
t = ticks;
sei();
```
ou, mieux (cela restaure l'état précédent au lieu de réactiver aveuglément) :
```c
#include <util/atomic.h>

uint16_t t;
ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
{
    t = ticks;
}
```

**Règle 3 : la lecture-modification-écriture sur des variables (et registres) partagées avec des ISR n'est pas atomique.** `counter++` dans main, c'est charger / ajouter / stocker. Si l'ISR modifie aussi `counter` entre le chargement et le stockage, une mise à jour est perdue. Protégez-la de la même façon.

Cas particulier : `PORTB |= (1 << PB0);` sur les registres d'E/S *bas* (adresses < 0x40, comme PORTB, DDRB, PIND) est compilé en une seule instruction `SBI`, qui est atomique. Mais `PORTB |= 0x03;` (deux bits), et tout registre étendu comme `TIMSK1 |= ...` ou `UCSR0B |= ...`, sont compilés en chargement/OU/stockage et ne sont pas atomiques. Si une ISR modifie le même registre, protégez l'accès du code principal.

Les variables de 8 bits écrites d'un seul côté (l'ISR écrit, main lit) sont sûres tant qu'elles sont `volatile`.

### 14.9 Bonnes pratiques pour les ISR

1. **Gardez les ISR courtes.** Levez un drapeau, stockez une valeur, mettez à jour un compteur, puis sortez. Faites le gros du travail dans la boucle principale.
2. **Pas de `_delay_ms` dans les ISR.**
3. **Évitez les longs affichages UART dans les ISR.** Chaque caractère prend 87 µs à 115200.
4. **Pendant qu'une ISR s'exécute, les autres interruptions sont bloquées** (bit I à 0). Une ISR lente retarde les autres, ou leur fait manquer des événements (par exemple, des octets UART reçus sont perdus quand le tampon de 2 octets déborde).
5. **N'appelez pas de fonctions non réentrantes** (non-reentrant) à la fois depuis le code principal et depuis une ISR, par ex. une fonction utilisant un buffer statique.
6. Les interruptions imbriquées (nested interrupts) sont possibles (`sei()` dans une ISR, ou `ISR(vector, ISR_NOBLOCK)`), mais c'est avancé et rarement nécessaire.

**Le motif du drapeau (l'architecture standard) :**

```c
volatile uint8_t g_tick = 0;         // levé par l'ISR, consommé par main
volatile uint8_t g_button = 0;

ISR(TIMER1_COMPA_vect) { g_tick = 1; }
ISR(INT0_vect)         { g_button = 1; }

int main(void)
{
    init_everything();
    sei();
    while (1)
    {
        if (g_tick)   { g_tick = 0;   update_display(); }
        if (g_button) { g_button = 0; change_mode(); }
    }
}
```

### 14.10 Comment chaque drapeau d'interruption est effacé

| Interruption | Drapeau effacé par… |
|---|---|
| INT0/INT1, PCINT, comparaison/débordement de timer, ADC | **automatiquement** au démarrage de l'ISR (ou en écrivant 1 dans le drapeau) |
| USART réception terminée (`RXC0`) | **la lecture de UDR0**. Si vous ne la lisez pas, l'ISR se redéclenche indéfiniment |
| USART registre de données vide (`UDRE0`) | **l'écriture de UDR0**. Si vous n'avez rien à écrire, désactivez `UDRIE0` |
| TWI (`TWINT`) | **l'écriture de 1 dans TWINT** dans TWCR (Day 06) |
| SPI (`SPIF`) | la lecture de SPSR puis l'accès à SPDR |
| INT0 en mode **niveau bas** | pas de drapeau : elle se déclenche tant que la broche est basse |

### 14.11 Interruptions de timer

Les bits d'activation sont dans **TIMSKn** :
- `TOIEn` : interruption de débordement (`TIMERn_OVF_vect`)
- `OCIEnA` / `OCIEnB` : égalité de comparaison A / B (`TIMERn_COMPA_vect`, `TIMERn_COMPB_vect`)
- `ICIE1` : capture d'entrée (Timer1 uniquement)

**Une horloge à la milliseconde (`millis`)**, utile dans tous les projets :

```c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

static volatile uint32_t g_ms = 0;

ISR(TIMER0_COMPA_vect) { g_ms++; }

void millis_init(void)
{
    TCCR0A = (1 << WGM01);                 // CTC, TOP = OCR0A
    OCR0A  = 249;                          // 250 ticks
    TCCR0B = (1 << CS01) | (1 << CS00);    // /64 → 4 µs par tick → 1 ms par égalité
    TIMSK0 = (1 << OCIE0A);
}

uint32_t millis(void)
{
    uint32_t m;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { m = g_ms; }
    return m;
}
```

Tâches périodiques non bloquantes avec `millis` :
```c
uint32_t last = 0;
while (1) {
    uint32_t now = millis();
    if (now - last >= 500) {      // la soustraction gère correctement le rebouclage (non signé)
        last += 500;
        PINB = (1 << PB0);        // inverser toutes les 500 ms
    }
    // faire librement autre chose
}
```

(Si vous utilisez le Timer0 pour `millis`, il n'est plus disponible pour la PWM sur PD5/PD6 dans le même programme. Planifiez l'attribution de vos timers !)

### 14.12 Exemple détaillé : LED qui « respire »

Exercice typique : la LED D2 (PB1 = OC1A) s'allume et s'éteint en fondu (période d'environ 1 s) avec **deux timers**. Le Timer1 génère la PWM, et l'interruption du Timer0 met à jour le rapport cyclique.

- Timer1 : Fast PWM 8 bits (mode 5, TOP = 0xFF) sur OC1A, prescaler 1 → 62,5 kHz. Aucun scintillement visible.
- Timer0 : CTC avec interruption. Un cycle complet = 0→255→0 = 510 pas en 1 s → fréquence de mise à jour de **510 Hz**. Avec un prescaler de 256 : 16e6 / 256 / 510 ≈ 122,5 → OCR0A = 121 (≈ 512 Hz, suffisamment proche).

```c
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER0_COMPA_vect)
{
    static uint8_t duty = 0;
    static int8_t  dir  = 1;

    duty += dir;
    if (duty == 255) dir = -1;
    else if (duty == 0) dir = 1;
    OCR1A = duty;                           // mode 8 bits : seul l'octet bas compte
}

int main(void)
{
    DDRB |= (1 << PB1);

    // Timer1 : Fast PWM 8 bits (mode 5 : WGM12 + WGM10), non inversé sur OC1A, pas de prescaler
    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS10);

    // Timer0 : CTC (WGM01), prescaler 256, interruption à ~512 Hz
    TCCR0A = (1 << WGM01);
    OCR0A  = 121;
    TCCR0B = (1 << CS02);
    TIMSK0 = (1 << OCIE0A);

    sei();
    while (1) { }
}
```

Combinez cela avec la correction gamma (§13.8) pour une « respiration » plus naturelle.

### 14.13 Exemple détaillé : compteur avec interruptions

SW1 (INT0) incrémente et SW2 (PCINT20) décrémente une valeur affichée sur D1–D4, avec une boucle principale vide :

```c
volatile uint8_t g_value = 0;

static void display(uint8_t v)
{
    PORTB = (PORTB & ~0x17) | (v & 0x07) | ((v & 0x08) << 1);   // 0x17 = PB0|PB1|PB2|PB4
}

ISR(INT0_vect)                    // front descendant de SW1
{
    g_value = (g_value + 1) & 0x0F;
    display(g_value);
}

ISR(PCINT2_vect)                  // tout changement de SW2
{
    if (!(PIND & (1 << PD4)))     // n'agir que sur l'appui (broche basse)
    {
        g_value = (g_value - 1) & 0x0F;
        display(g_value);
    }
}
```

Ajoutez un anti-rebond, par exemple avec un verrouillage temporisé par timer comme au §14.6.

### 14.14 Dormir en attendant (avant-goût)

Si votre boucle principale est vide, le CPU tourne quand même à pleine puissance. Vous pouvez le faire dormir jusqu'à la prochaine interruption :

```c
#include <avr/sleep.h>
set_sleep_mode(SLEEP_MODE_IDLE);    // les timers continuent de tourner en mode IDLE
while (1) {
    sleep_mode();                   // dormir ; une interruption nous réveille, l'ISR s'exécute, puis on reprend ici
}
```

Plus de détails au §24.

### 14.15 Exercices typiques du Day 04

| Ex | Tâche | Concept clé |
|---|---|---|
| 00 | SW1 inverse D1 via INT0, boucle principale vide | interruption externe, anti-rebond |
| 01 | D2 « respire » (rampe de rapport cyclique PWM), ISR de timer, boucle principale vide | deux timers, interruption de timer |
| 02 | Compteur avec SW1/SW2 sur interruptions, affiché en binaire | INT0 + PCINT, état partagé |

### 14.16 Pièges

- **Oublier `sei()`** : rien ne se déclenche jamais.
- **Nom de vecteur mal orthographié** : la puce redémarre quand l'interruption survient (gcc avertit).
- **Activer une interruption sans ISR** : même redémarrage.
- **Déclenchement sur niveau bas (ISC = 00) pour un bouton** : l'ISR se déclenche en continu tant que le bouton est maintenu, et main ne s'exécute jamais.
- **Variables partagées non volatiles** : main ne voit jamais la mise à jour.
- **Lectures 16/32 bits non atomiques** : des valeurs fausses, rares et aléatoires (le pire type de bug).
- **Drapeaux périmés** (stale flags) : un front survenu *avant* l'activation de l'interruption la déclenche immédiatement. Effacez le drapeau (écrire 1) avant d'activer.
- **ISR longues** : événements manqués, système poussif.

---

## 15. Rush 00 : Multijoueur

### 15.1 À quoi s'attendre

Un **rush** est un projet de groupe court et intense (typiquement un week-end). « Multijoueur » (multiplayer) signifie que **deux cartes (ou plus) communiquent entre elles** pour faire tourner un jeu. La version classique est un **jeu de réflexes** :

- Les deux cartes sont reliées (très probablement en **I2C**, avec SDA/SCL/GND câblés entre elles ; éventuellement en UART).
- Un compte à rebours s'affiche sur les LEDs (par ex. 4 LEDs allumées, qui s'éteignent une à une).
- À la fin du compte à rebours, **le premier joueur qui appuie sur son bouton gagne**. Appuyer *avant* la fin fait perdre.
- Les résultats s'affichent sur les deux cartes (LEDs, couleur RGB : vert = gagné, rouge = perdu), puis une nouvelle manche peut commencer.
- Souvent exigé : **les deux cartes exécutent le même code**, et les rôles (maître/esclave, master/slave) sont décidés automatiquement, par exemple le premier joueur qui appuie devient le maître.

**Lisez précisément le sujet :** il définit les contraintes du protocole, le moyen de transport, ce qui compte comme une victoire et ce qui se passe en cas de déconnexion.

⚠️ **Ce rush a lieu avant le Day 06 (I2C).** Lisez le §17 (I2C) *avant* le rush. Vous aurez besoin des modes TWI maître et esclave.

### 15.2 Architecture : pensez machine à états

Tout, dans un rush, devient gérable si vous concevez d'abord une **machine à états finis** (finite state machine, FSM), sur papier :

```
                ┌───────────────┐
    mise sous   │   WAITING     │  les deux cartes au repos, attendent que les joueurs soient prêts
    tension ───►│  (pas prêt)   │
                └──────┬────────┘
                       │ bouton local appuyé → je suis prêt (le dire à l'autre)
                       ▼
                ┌───────────────┐
                │  READY_WAIT   │  attente que l'autre joueur soit prêt
                └──────┬────────┘
                       │ les deux sont prêts (le maître décide, envoie START)
                       ▼
                ┌───────────────┐
                │  COUNTDOWN    │  LEDs 4→3→2→1→0 (piloté par timer)
                │               │── bouton appuyé trop tôt ──► LOSE (dire à l'autre : WIN)
                └──────┬────────┘
                       │ compte à rebours terminé
                       ▼
                ┌───────────────┐
                │     GO        │  le premier qui appuie gagne
                └──┬─────────┬──┘
  j'ai appuyé en premier│    │l'autre a appuyé en premier
                    ▼         ▼
                 [ WIN ]   [ LOSE ]  → afficher le résultat, puis retour à WAITING
```

Forme du code :

```c
typedef enum { ST_IDLE, ST_READY, ST_COUNTDOWN, ST_GO, ST_WIN, ST_LOSE } state_t;

static state_t state = ST_IDLE;

void game_step(void)
{
    switch (state)
    {
        case ST_IDLE:       /* ... */ break;
        case ST_READY:      /* ... */ break;
        case ST_COUNTDOWN:  /* ... */ break;
        case ST_GO:         /* ... */ break;
        case ST_WIN:
        case ST_LOSE:       /* afficher le résultat, attendre, puis state = ST_IDLE */ break;
    }
}

int main(void)
{
    init_all();
    sei();
    while (1)
        game_step();       // appelée en continu ; ne doit jamais bloquer longtemps
}
```

Chaque événement (bouton local, message de l'autre carte, tick de timer) provoque une transition. **Chaque état ne réagit qu'aux événements qui ont du sens pour lui.** Cela rend faciles à traiter correctement les cas « appui trop tôt » et « les deux appuient en même temps ».

### 15.3 Concevoir le protocole de communication

Définissez des **messages** d'un seul octet (simple et robuste) :

```c
#define MSG_NONE      0x00
#define MSG_READY     0x01   // « je suis prêt »
#define MSG_START     0x02   // maître → esclave : lancer le compte à rebours maintenant
#define MSG_PRESSED   0x03   // « j'ai appuyé » (pendant GO → je revendique la victoire)
#define MSG_EARLY     0x04   // « j'ai appuyé trop tôt » (tu gagnes)
#define MSG_ACK       0x05
```

Avec l'**I2C**, seul le maître peut initier un transfert. Donc :
- **Maître → esclave** : le maître écrit un octet (`START`, résultat…).
- **Esclave → maître** : le maître **interroge** l'esclave régulièrement (polling, par ex. toutes les 10 ms) en lisant un octet. L'ISR de l'esclave répond avec son état courant (`NONE`, `READY`, `PRESSED`…).

Qui gagne dans l'état GO ? **Le maître est l'arbitre** : il sait quand son propre bouton a été appuyé, et il apprend l'appui de l'esclave à l'interrogation suivante. Pour être équitable, interrogez fréquemment (toutes les 1–5 ms), ou faites envoyer un horodatage par l'esclave. Le maître envoie ensuite le verdict à l'esclave.

### 15.4 Négociation des rôles maître/esclave (même code sur les deux cartes)

Une astuce courante : **chaque carte démarre en esclave I2C** à l'écoute sur une adresse connue. Quand un joueur appuie en premier sur son bouton, sa carte **passe en maître** et tente d'adresser l'autre carte. Si l'autre carte répond ACK, les rôles sont fixés : celle qui a appuyé est maître, l'autre reste esclave.

Cas limites à considérer :
- Les deux appuient presque en même temps : les deux tentent de devenir maître → **arbitrage** I2C (le matériel gère deux maîtres : l'un perd, statut `0x38`). Le perdant repasse en mode esclave.
- L'autre carte n'est pas connectée ou pas alimentée : l'adresse reçoit un NACK (`0x20`). Affichez une erreur ou réessayez.
- **Pull-ups** : l'I2C a besoin de résistances de pull-up sur SDA/SCL. Les devboards en ont probablement. Avec deux cartes reliées, les pull-ups sont en parallèle, ce qui ne pose pas de problème.
- **Une masse commune** est indispensable entre les cartes.

Sinon, si le sujet l'autorise, donnez à chaque carte un rôle fixe avec une option de compilation (`make MASTER=1`), ce qui est beaucoup plus simple. Lisez le sujet.

### 15.5 L'esclave I2C en bref (détails au §17)

```c
#define MY_ADDR 0x42

volatile uint8_t g_rx_msg = MSG_NONE;    // dernier octet reçu du maître
volatile uint8_t g_tx_msg = MSG_NONE;    // octet à répondre quand le maître lit

void i2c_slave_init(uint8_t addr)
{
    TWAR = (addr << 1);                                  // notre adresse 7 bits (bit 0 = activer l'appel général)
    TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);      // activer, acquitter notre adresse, interruptions
}

ISR(TWI_vect)
{
    switch (TWSR & 0xF8)
    {
        case 0x60:                     // notre SLA+W reçu, ACK envoyé
            break;
        case 0x80:                     // octet de données reçu, ACK envoyé
            g_rx_msg = TWDR;
            break;
        case 0xA8:                     // notre SLA+R reçu, ACK envoyé → charger l'octet à envoyer
        case 0xB8:                     // donnée envoyée, ACK reçu → le maître en veut plus
            TWDR = g_tx_msg;
            break;
        case 0xC0:                     // donnée envoyée, NACK reçu (le maître a fini)
        case 0xA0:                     // STOP ou START répété reçu
        default:
            break;
    }
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);  // effacer TWINT, continuer à acquitter
}
```

Le côté maître est couvert au §17.

### 15.6 Conseils de survie pour le rush

1. **Lisez le sujet ensemble, deux fois.** Notez toutes les règles et tous les cas limites.
2. **Dessinez la machine à états et le protocole** (qui envoie quoi, quand) *avant* de coder.
3. **Répartissez le travail** : une personne fait le transport I2C (maître + esclave + programme de test), une autre la logique du jeu et les LEDs, puis intégrez.
4. **Testez d'abord la communication isolément** : la carte A envoie un compteur, la carte B l'affiche sur ses LEDs. Ensuite seulement, ajoutez le jeu.
5. **Utilisez des affichages de débogage UART** (les deux cartes sur deux terminaux) pour tracer les états : `"[M] state=GO rx=03\r\n"`.
6. **Ne bloquez jamais** : pas de `_delay_ms(1000)` en attendant l'autre carte. Utilisez des timers/`millis`.
7. **Gérez les cas moches** : débrancher le câble en pleine partie, un reset d'une carte, les deux qui appuient simultanément, un bouton maintenu au démarrage.
8. **Gardez une version qui marche** : commitez souvent (git) et ne cassez pas la démo 10 minutes avant l'évaluation.
9. Les correcteurs adorent appuyer sur les deux boutons en même temps et appuyer pendant le compte à rebours. Testez exactement ça.


---

## 16. Day 05 : Analogique (ADC)

### 16.1 Objectifs

- Comprendre la différence entre signaux analogiques et numériques.
- Comprendre le fonctionnement d'un **ADC** (Analog-to-Digital Converter, convertisseur analogique-numérique, CAN) : résolution, tension de référence, temps de conversion.
- Configurer l'ADC de l'ATmega328P : canaux, référence, prescaler, résultats sur 8 bits ou 10 bits.
- Lire un potentiomètre, un capteur de lumière (LDR), une thermistance (CTN) et le **capteur de température interne**.
- Afficher les valeurs en hexadécimal et en décimal, et les convertir en unités physiques.

### 16.2 Analogique vs numérique

- Un signal **numérique** (digital) a deux états : 0 ou 1.
- Un signal **analogique** (analog) peut prendre **n'importe quelle valeur** dans une plage, par ex. n'importe quelle tension entre 0 et 5 V.

Le monde physique est analogique (température, lumière, son, position). Pour le traiter, un microcontrôleur doit **mesurer** une tension et la transformer en nombre. C'est le rôle de l'ADC.

```
  tension                          lecture ADC (10 bits)
   5 V ┤          ___            1023 ┤          ▄▄▄
       │        /     \               │        ▄▀   ▀▄
       │      /         \             │      ▄▀       ▀▄
       │    /             \           │    ▄▀           ▀▄
   0 V ┼──/─────────────────\──   0   ┼──▄▀───────────────▀▄──
         signal continu                 escalier : échantillonné + quantifié
```

Deux choses se produisent :
- **Échantillonnage** (sampling) : le signal est mesuré à des instants discrets.
- **Quantification** (quantization) : chaque mesure est arrondie à l'un des 2ⁿ niveaux (n = résolution en bits).

### 16.3 Résolution et tension de référence

L'ADC de l'ATmega328P a une **résolution de 10 bits** → 2¹⁰ = **1024 niveaux** (0 à 1023).

L'ADC compare l'entrée à une **tension de référence, VREF** :

```
             Vin × 1024                      ADC × VREF
    ADC  =  ────────────         Vin   =   ─────────────
               VREF                           1024
```

Avec VREF = 5 V : un pas (1 **LSB**, least significant bit, quantum) = 5 / 1024 ≈ **4,88 mV**.
- 0 V → 0
- 2,5 V → 512
- 5 V → 1023 (le maximum ; l'ADC sature à VREF − 1 LSB)

Avec VREF = 1,1 V (interne) : 1 LSB ≈ 1,07 mV. C'est plus précis, mais on ne peut mesurer que 0–1,1 V.

**Choix de la référence** (REFS1:0 dans ADMUX) :

| REFS1 | REFS0 | Référence |
|---|---|---|
| 0 | 0 | broche AREF (tension externe sur la broche AREF) |
| 0 | 1 | **AVCC** (≈ 5 V) avec condensateur externe sur AREF ← **le choix habituel** |
| 1 | 0 | réservé |
| 1 | 1 | **1,1 V interne** (nécessaire pour le capteur de température) |

⚠️ Ne sélectionnez jamais AVCC ou la référence interne si une tension externe est appliquée sur AREF. Les cartes comme l'Uno n'ont qu'un condensateur sur AREF, donc aucun problème.

### 16.4 Fonctionnement d'un ADC SAR (simplifié)

L'AVR utilise un ADC à **approximations successives** (Successive Approximation Register, SAR). Il fonctionne comme une recherche dichotomique (le jeu « devine le nombre ») :

1. **Échantillonneur-bloqueur** (sample & hold) : un petit condensateur se charge à Vin, puis est déconnecté pour que la valeur soit figée pendant la conversion.
2. Essayer le bit 9 : Vin ≥ VREF/2 ? (Comparaison avec la sortie d'un DAC interne.) Oui → garder bit 9 = 1.
3. Essayer le bit 8 : Vin ≥ (estimation actuelle + VREF/4) ? …
4. …10 étapes plus tard, les 10 bits sont décidés.

```
  Vin = 3,2 V, VREF = 5 V
  étape 1 : compare à 2,5    → plus haut → 1xxxxxxxxx
  étape 2 : compare à 3,75   → plus bas  → 10xxxxxxxx
  étape 3 : compare à 3,125  → plus haut → 101xxxxxxx
  étape 4 : compare à 3,4375 → plus bas  → 1010xxxxxx
  ... → 1010001111 = 655  (655 × 5/1024 = 3,198 V ✓)
```

Chaque étape prend un cycle d'**horloge ADC**. Une conversion complète prend **13 cycles d'horloge ADC** (25 pour la toute première conversion après l'activation de l'ADC, qui initialise la partie analogique).

### 16.5 L'horloge de l'ADC (prescaler)

La datasheet dit : *pour une résolution maximale de 10 bits, l'horloge de l'ADC doit être comprise entre 50 kHz et 200 kHz.* L'horloge de l'ADC provient de F_CPU via un prescaler (ADPS2:0 dans ADCSRA) :

| ADPS2 | ADPS1 | ADPS0 | Division | Horloge ADC à 16 MHz |
|---|---|---|---|---|
| 0 | 0 | 0 | 2 | 8 MHz ✗ |
| 0 | 0 | 1 | 2 | 8 MHz ✗ |
| 0 | 1 | 0 | 4 | 4 MHz ✗ |
| 0 | 1 | 1 | 8 | 2 MHz ✗ |
| 1 | 0 | 0 | 16 | 1 MHz ✗ |
| 1 | 0 | 1 | 32 | 500 kHz ✗ (OK en 8 bits) |
| 1 | 1 | 0 | 64 | 250 kHz (un peu élevé) |
| 1 | 1 | 1 | **128** | **125 kHz ✓** |

À 16 MHz, utilisez **/128** → 125 kHz → une conversion = 13 / 125 000 = **104 µs** → ~9 600 échantillons/s.

### 16.6 Les registres de l'ADC

**ADMUX** : ADC Multiplexer Selection Register (sélection du multiplexeur)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | REFS1 | REFS0 | **ADLAR** | – | MUX3 | MUX2 | MUX1 | MUX0 |

- **REFS1:0** : référence (ci-dessus)
- **ADLAR** : ADC Left Adjust Result, justification à gauche du résultat (voir §16.7)
- **MUX3:0** : canal d'entrée

| MUX3:0 | Entrée |
|---|---|
| 0000 | ADC0 (PC0) |
| 0001 | ADC1 (PC1) |
| 0010 | ADC2 (PC2) |
| 0011 | ADC3 (PC3) |
| 0100 | ADC4 (PC4) |
| 0101 | ADC5 (PC5) |
| 0110 | ADC6 (TQFP/QFN seulement) |
| 0111 | ADC7 (TQFP/QFN seulement) |
| **1000** | **ADC8 : capteur de température interne** |
| 1110 | référence de bande interdite (bandgap) 1,1 V (utile pour mesurer VCC !) |
| 1111 | 0 V (GND) |

**ADCSRA** : ADC Control and Status Register A (registre de contrôle et d'état A)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | **ADEN** | **ADSC** | ADATE | ADIF | ADIE | ADPS2 | ADPS1 | ADPS0 |

- **ADEN** : activation de l'ADC (enable)
- **ADSC** : Start Conversion (lancer la conversion). Écrire 1 pour démarrer. Il **se lit à 1 pendant la conversion et revient à 0 à la fin.**
- ADATE : Auto Trigger Enable, déclenchement automatique (mode libre, free-running, ou déclenché par des timers etc., via ADCSRB)
- ADIF : drapeau d'interruption (conversion terminée)
- ADIE : activation de l'interruption (`ADC_vect`)
- ADPS2:0 : prescaler

**ADCL / ADCH** (ou le nom 16 bits **ADC**) : le résultat.
**ADCSRB** : ADTS2:0, source de déclenchement automatique (mode libre, comparaison de timer, etc.).
**DIDR0** : Digital Input Disable (désactivation des entrées numériques). Mettre un bit à 1 désactive le tampon d'entrée numérique sur ADC0–5, ce qui réduit la consommation et le bruit sur les broches analogiques.

### 16.7 Résultat 8 bits vs 10 bits : ADLAR

Le résultat sur 10 bits est réparti sur deux registres :

```
  ADLAR = 0 (justifié à droite, par défaut) :
       ADCH                          ADCL
  [ - - - - - - 9 8 ]         [ 7 6 5 4 3 2 1 0 ]
  → lire ADC (16 bits) → 0..1023

  ADLAR = 1 (justifié à gauche) :
       ADCH                          ADCL
  [ 9 8 7 6 5 4 3 2 ]         [ 1 0 - - - - - - ]
  → lire uniquement ADCH → 8 bits de poids fort → 0..255 (une lecture 8 bits)
```

**Règle d'ordre de lecture :** si vous lisez les deux octets séparément, lisez **ADCL d'abord, puis ADCH**. Lire ADCL verrouille les registres jusqu'à la lecture d'ADCH, ce qui évite de mélanger deux conversions. Lire le registre 16 bits `ADC` en C le fait automatiquement. Avec ADLAR = 1, lire **uniquement ADCH** convient pour des résultats 8 bits.

### 16.8 Pilote de base : conversion simple, par scrutation

```c
#include <avr/io.h>

void adc_init(void)
{
    ADMUX  = (1 << REFS0);                              // référence AVCC, justifié à droite, canal ADC0
    ADCSRA = (1 << ADEN)                                // activer l'ADC
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);// prescaler 128 → 125 kHz
    DIDR0  = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D);// désactiver l'entrée numérique sur les broches analogiques
}

uint16_t adc_read(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);          // sélectionner le canal, garder REFS/ADLAR
    ADCSRA |= (1 << ADSC);                              // lancer la conversion
    while (ADCSRA & (1 << ADSC)) { }                    // attendre la fin (~104 µs)
    return ADC;                                         // résultat 10 bits (ADCL puis ADCH)
}
```

Version 8 bits :
```c
void adc_init8(void)
{
    ADMUX  = (1 << REFS0) | (1 << ADLAR);               // AVCC, justifié à GAUCHE
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t adc_read8(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) { }
    return ADCH;                                        // 8 bits de poids fort
}
```

### 16.9 Exercices typiques

**Ex00 : lire le potentiomètre (RV1 sur ADC0) en 8 bits, l'afficher en hexa toutes les 20 ms.**
```c
int main(void)
{
    uart_init();
    adc_init8();
    while (1)
    {
        uart_puthex8(adc_read8());      // ex. "7f"  (vérifiez la casse demandée par le sujet)
        uart_printstr("\r\n");
        _delay_ms(20);
    }
}
```
Tournez le bouton : la valeur va de `00` à `ff`.

**Ex01 : lire trois capteurs (RV1, LDR, CTN) et afficher `"xx, yy, zz"`.** Changez de canal avant chaque conversion :
```c
uart_puthex8(adc_read8_ch(0)); uart_printstr(", ");
uart_puthex8(adc_read8_ch(1)); uart_printstr(", ");
uart_puthex8(adc_read8_ch(2)); uart_printstr("\r\n");
```

**Ex02 : la même chose en décimal 10 bits** (`"512, 734, 402"`) : utilisez le résultat 16 bits et `uart_putnbr`.

**Ex03 : capteur de température interne en °C.** Voir §16.12.

Autres exercices possibles : afficher la valeur du potentiomètre sous forme de « jauge » sur les 4 LEDs (0–25 %, 25–50 %…), piloter la roue RGB avec le potentiomètre, etc.

### 16.10 Comprendre les capteurs de la carte

**Potentiomètre (RV1)** : un diviseur de tension variable entre GND et VCC. Sa lecture est proportionnelle à l'angle du bouton (0 → 1023). Rien à calculer.

**LDR (photorésistance)** : sa résistance baisse avec la lumière (~1 MΩ dans le noir → ~1 kΩ en pleine lumière). Dans un diviseur avec une résistance fixe R :
```
    VCC ──[ R_fixe ]──┬──[ LDR ]── GND        Vout = VCC × R_ldr / (R_fixe + R_ldr)
                      └── ADC1                 plus de lumière → R_ldr ↓ → Vout ↓
```
(ou l'inverse, selon le côté où se trouve la LDR. Vérifiez le schéma.) Convertir en lux précisément demande un étalonnage ; pour la piscine, « plus clair/plus sombre » suffit.

**Thermistance CTN (NTC)** : sa résistance diminue quand la température augmente. Elle est spécifiée par R₂₅ (par ex. 10 kΩ à 25 °C) et un **coefficient B** (par ex. 3950 K). Avec le diviseur :

1. À partir de la valeur ADC, calculez la résistance. Si la CTN est côté GND avec R_fixe côté VCC :
   `R_ctn = R_fixe × ADC / (1023 − ADC)`
2. Appliquez l'**équation du paramètre Bêta** (Beta equation) :
   ```
     1/T = 1/T₀ + (1/B) × ln(R / R₀)       T en Kelvin, T₀ = 298,15 K (25 °C), R₀ = R₂₅
     T(°C) = T(K) − 273,15
   ```
   Cela nécessite `log()` (float, `<math.h>`, lier avec `-lm`). Sinon, précalculez une **table de correspondance** ADC → °C et interpolez.

### 16.11 Bruit, stabilité et précision

Les lectures de l'ADC fluctuent de quelques LSB. C'est normal. Techniques :
- **Moyennage** : lisez N échantillons et faites la moyenne. N = 16 ou 64 sont de bons choix (divisez par un décalage).
  ```c
  uint16_t adc_read_avg(uint8_t ch)
  {
      uint32_t sum = 0;
      for (uint8_t i = 0; i < 16; i++) sum += adc_read(ch);
      return sum >> 4;               // / 16
  }
  ```
- **Jetez la première conversion** après un changement de référence ou de canal. Le condensateur de l'échantillonneur-bloqueur a besoin de temps pour se stabiliser, surtout avec des sources à haute impédance.
- **Mode veille ADC Noise Reduction** (réduction du bruit) : arrête le CPU pendant la conversion pour réduire le bruit numérique (avancé).
- **Moyenne mobile exponentielle** (exponential moving average, filtre peu coûteux) : `avg = avg + (sample − avg) / 8;` (utilisez des entiers mis à l'échelle ×8 ou ×16 pour garder la précision).

La précision est limitée par VREF : si AVCC vaut en réalité 4,85 V au lieu de 5,00 V, chaque tension calculée avec « 5 V » est fausse de 3 %. **Astuce :** mesurez la référence bandgap interne de 1,1 V (MUX = 1110) avec AVCC comme référence, puis VCC = 1,1 × 1024 / ADC.

### 16.12 Le capteur de température interne

L'ATmega328P a un capteur de température relié au canal 8 de l'ADC. D'après la datasheet :
- Il doit être lu avec la **référence interne de 1,1 V** (REFS1:0 = 11).
- Sortie typique : **~314 mV à 25 °C**, environ **1 mV/°C**. Le tableau de la datasheet donne à peu près 242 mV à −45 °C, 314 mV à +25 °C et 380 mV à +85 °C.
- Il n'est **pas précis** d'usine (±10 °C), et le décalage (offset) varie d'une puce à l'autre. La datasheet recommande de l'étalonner (stocker un décalage en EEPROM).

```c
int16_t read_internal_temp(void)
{
    ADMUX = (1 << REFS1) | (1 << REFS0) | (1 << MUX3);   // réf. 1,1 V, canal 8 (capteur de température)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    _delay_ms(5);                                        // laisser la référence se stabiliser

    ADCSRA |= (1 << ADSC);                               // jeter la première conversion
    while (ADCSRA & (1 << ADSC)) { }

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) { }
    uint16_t raw = ADC;

    // raw → mV :  mV = raw * 1100 / 1024
    // T(°C) ≈ (mV − 314) / 1 + 25   (valeurs typiques de la datasheet ; étalonnez pour être précis)
    int32_t mv = (int32_t)raw * 1100 / 1024;
    return (int16_t)(mv - 314 + 25);
}
```

Une formule empirique couramment utilisée (issue de la note d'application Atmel AVR122 et de divers tests) est `T = (raw − 324,31) / 1,22`. Chaque puce est différente : comparez avec un vrai thermomètre et ajustez le décalage. Ne soyez pas surpris si la puce indique quelques degrés de plus que la température ambiante : elle chauffe un peu toute seule.

### 16.13 Conversions par interruption et en mode libre (bonus)

Au lieu de scruter ADSC :
```c
volatile uint16_t g_adc;

ISR(ADC_vect) { g_adc = ADC; }            // appelée à la fin de chaque conversion

void adc_init_freerun(void)
{
    ADMUX  = (1 << REFS0);                 // AVCC, ADC0
    ADCSRB = 0;                            // ADTS = 000 : mode libre (free running)
    ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE)
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADSC);                 // lancer la première conversion ; ensuite elle redémarre automatiquement
}
```

N'oubliez pas que `g_adc` fait 16 bits : lisez-le de manière atomique dans main (§14.8). Vous pouvez aussi déclencher les conversions sur une **égalité de comparaison de timer** (ADTS = 011 pour la comparaison A du Timer0, 101 pour la comparaison B du Timer1), ce qui donne une fréquence d'échantillonnage exacte. C'est la base de l'échantillonnage audio.

### 16.14 Pièges

- Ne pas attendre la fin de la conversion.
- Lire ADCH avant ADCL en mode 10 bits, ce qui renvoie une valeur périmée.
- Changer MUX pendant une conversion : le changement ne s'applique qu'à la conversion suivante.
- Oublier les bits de référence : REFS = 00 signifie « utiliser la broche AREF », ce qui lit n'importe quoi sur la plupart des cartes.
- Utiliser la référence 5 V pour le capteur de température.
- Débordement entier lors de la conversion en mV : `raw * 5000` dépasse 16 bits → utilisez `(uint32_t)raw * 5000 / 1024`.
- Casse et format de l'affichage hexa : vérifiez si le sujet veut `0x7F`, `7f` ou `7F`.

---

## 17. Day 06 : I2C

### 17.1 Objectifs

- Comprendre le **bus I2C** : deux fils, adresses, maître/esclave, start/stop, ACK/NACK.
- Comprendre les lignes à **drain ouvert** (open-drain) et les résistances de **pull-up**.
- Utiliser le périphérique **TWI** (Two-Wire Interface, interface deux fils) de l'AVR en maître, en lisant ses **codes d'état** (status codes).
- Écrire un pilote pour le capteur de température/humidité **AHT20** : initialisation, mesure, conversion en °C et en %HR.

### 17.2 Le bus I2C

L'**I2C** (Inter-Integrated Circuit, prononcé « I-deux-C » ou « I-carré-C » ; Atmel l'appelle **TWI** pour des raisons de marque déposée) a été inventé par Philips en 1982 pour relier des puces sur une même carte.

- **2 fils**, partagés par tous les composants :
  - **SCL** (Serial Clock, horloge série) : pilotée par le maître
  - **SDA** (Serial Data, données série) : bidirectionnelle
- **Adressé** : chaque esclave a une **adresse sur 7 bits** (0x00–0x7F, certaines réservées). Jusqu'à ~112 composants sur un bus.
- **Maître/esclave** (master/slave) : le maître génère l'horloge et initie chaque transfert. Les esclaves ne font que répondre.
- **Multi-maître** possible, avec arbitrage.
- **Vitesses** : Standard mode 100 kHz, Fast mode 400 kHz (les modes 1 MHz et 3,4 MHz existent aussi).
- **Half-duplex** (semi-duplex) : les données circulent dans un seul sens à la fois sur SDA.

```
         VCC      VCC
          │        │
         [Rp]     [Rp]   pull-ups (typ. 4,7 kΩ à 100 kHz)
          │        │
  SDA ────┴────┬───┼──────┬──────────┬───────
  SCL ─────────┼───┴───┬──┼──────┬───┼───────
               │       │  │      │   │    │
            ┌──┴───────┴┐ ┌┴──────┴┐ ┌┴────┴──┐
            │ ATmega328P│ │ AHT20  │ │PCA9555 │  ...
            │ (maître)  │ │ 0x38   │ │ 0x20   │
            └───────────┘ └────────┘ └────────┘
  ATmega328P : SDA = PC4, SCL = PC5
```

### 17.3 Drain ouvert et pull-ups : pourquoi ?

Chaque composant du bus peut **seulement tirer la ligne à l'état BAS** ou la **relâcher** (drain ouvert, §3.9). Quand personne ne tire, la **résistance de pull-up** ramène la ligne à l'état HAUT.

Conséquences :
- **Pas de court-circuit** : deux composants ne peuvent pas se battre (l'un HIGH, l'autre LOW), car personne ne pilote activement l'état HAUT.
- **ET câblé** (wired-AND) : la ligne n'est HAUTE que si *tous* les composants la relâchent. N'importe quel composant peut la maintenir basse. Cela permet les ACK, l'étirement d'horloge (clock stretching) et l'arbitrage.
- **Sans pull-ups, rien ne fonctionne.** Les lignes restent basses ou flottent. Le TWI se bloque en attendant un TWINT qui ne vient jamais. La devboard a des pull-ups externes ; les pull-ups internes (~30 kΩ) sont trop faibles pour un fonctionnement fiable, mais peuvent dépanner pour des tests.
- Les temps de montée sont limités par la valeur des pull-ups et la capacité du bus, c'est pourquoi l'I2C est lent et prévu pour de courtes distances (sur la carte, quelques dizaines de cm).

### 17.4 Le protocole, bit par bit

**Validité des données :** SDA ne peut changer **que lorsque SCL est BAS**. SDA est lue **lorsque SCL est HAUT**.

**Deux conditions spéciales** enfreignent volontairement cette règle :
- **START (S)** : SDA descend **pendant que SCL est HAUT**. Cela signifie « une transaction commence, tout le monde écoute ».
- **STOP (P)** : SDA monte **pendant que SCL est HAUT**. Cela signifie « la transaction est terminée, le bus est libre ».

```
        START                                                STOP
  SDA ‾‾‾\_____X‾‾‾‾‾X_____X‾‾‾‾X ... X_____/‾‾‾‾‾‾‾‾‾‾‾‾‾/‾‾‾‾
  SCL ‾‾‾‾‾‾\__/‾‾\__/‾‾\__/‾‾\__ ... __/‾‾\____/‾‾‾‾‾‾‾‾‾‾‾‾‾
              bit7  bit6  bit5   ...    ACK
```

**Un transfert d'octet** = 8 bits de données (**MSB en premier**, contrairement à l'UART) + 1 **bit d'ACK** :
- Après les 8 bits, l'émetteur relâche SDA.
- Pendant la 9ᵉ impulsion d'horloge, le **récepteur tire SDA à l'état BAS = ACK** (acquittement : « reçu, continue »).
- Si SDA reste HAUTE = **NACK** (non-acquittement : « non » / « je ne suis pas là » / « ça suffit »).

**Une transaction d'écriture complète** (le maître écrit 2 octets vers l'esclave 0x38) :
```
 ┌───┬───────────────┬───┬─────────────┬───┬─────────────┬───┬───┐
 │ S │ 0x38 <<1 | 0  │ A │  donnée 1   │ A │  donnée 2   │ A │ P │
 └───┴───────────────┴───┴─────────────┴───┴─────────────┴───┴───┘
   le maître envoie ───────► l'esclave acquitte chaque octet
```

**Une transaction de lecture complète** (le maître lit 3 octets depuis 0x38) :
```
 ┌───┬───────────────┬───┬─────────────┬───┬─────────────┬───┬─────────────┬────┬───┐
 │ S │ 0x38 <<1 | 1  │ A │  donnée 1   │ A │  donnée 2   │ A │  donnée 3   │ NA │ P │
 └───┴───────────────┴───┴─────────────┴───┴─────────────┴───┴─────────────┴────┴───┘
            l'esclave acquitte l'adresse   l'esclave envoie, le MAÎTRE acquitte    le maître NACK le dernier octet
```

**Le maître envoie un NACK sur le dernier octet qu'il veut** pour dire à l'esclave « arrête d'envoyer », puis envoie STOP.

### 17.5 L'octet d'adresse

Le premier octet après START est l'**adresse 7 bits décalée d'un cran à gauche**, plus le **bit R/W** (lecture/écriture) en bit 0 :

```
   bit :  7  6  5  4  3  2  1  0
        [ A6 A5 A4 A3 A2 A1 A0 R/W ]      R/W : 0 = écriture (maître → esclave), 1 = lecture (esclave → maître)

   adresse de l'AHT20 0x38 = 0b0111000
   → écriture : (0x38 << 1) | 0 = 0x70
   → lecture :  (0x38 << 1) | 1 = 0x71
```

**Attention :** les datasheets ne sont pas cohérentes entre elles. Certaines donnent l'adresse 7 bits (0x38), d'autres l'« adresse d'écriture » 8 bits (0x70). Si un composant ne répond pas, vérifiez que vous ne décalez pas deux fois, ou pas du tout.

Beaucoup de composants ont des **broches d'adresse** (A0, A1, A2) pour choisir parmi plusieurs adresses, ce qui permet de mettre plusieurs puces identiques sur un même bus. L'adresse du PCA9555 est `0b0100 A2 A1 A0` → 0x20–0x27.

### 17.6 Composants à registres et START répété

La plupart des puces I2C exposent des **registres** internes. Pour **lire** le registre R :
1. START, adresse + **W**, écrire le numéro de registre R (positionne le pointeur interne de la puce)
2. **START répété** (repeated START : un START sans STOP avant), adresse + **R**, lire le ou les octets, NACK sur le dernier, STOP

```
 S | ADDR+W | A | REG | A | Sr | ADDR+R | A | DATA | NA | P
```

Le START répété garde le bus réservé (dans les configurations multi-maîtres), et certains composants l'exigent. Pour **écrire** le registre R : `S | ADDR+W | A | REG | A | VALUE | A | P`.

### 17.7 Étirement d'horloge (clock stretching)

Un esclave qui a besoin de plus de temps peut **maintenir SCL à l'état bas** après un octet. Le maître attend (le matériel TWI le gère automatiquement). Certains capteurs l'utilisent pendant les mesures.

### 17.8 Le périphérique TWI de l'AVR

**Broches :** SDA = **PC4**, SCL = **PC5**. Quand TWEN est à 1, le TWI prend le contrôle de ces broches.

**Registres :**

**TWBR** : Bit Rate Register (registre de débit). Il fixe la fréquence de SCL, avec les bits de prescaler de TWSR :
```
                     F_CPU
  f_SCL = ─────────────────────────────
           16 + 2 × TWBR × Prescaler

  → TWBR = (F_CPU / f_SCL − 16) / (2 × Prescaler)
```
À 16 MHz, prescaler 1 :
- 100 kHz → TWBR = (160 − 16) / 2 = **72**
- 400 kHz → TWBR = (40 − 16) / 2 = **12**

**TWSR** : Status Register (registre d'état)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | TWS7 | TWS6 | TWS5 | TWS4 | TWS3 | – | TWPS1 | TWPS0 |
- TWS7:3 : **code d'état** de la dernière opération. **Masquez toujours : `TWSR & 0xF8`.**
- TWPS1:0 : prescaler (00 = 1, 01 = 4, 10 = 16, 11 = 64)

**TWCR** : Control Register (registre de contrôle)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | **TWINT** | **TWEA** | **TWSTA** | **TWSTO** | TWWC | **TWEN** | – | TWIE |

- **TWINT** : TWI Interrupt Flag (drapeau d'interruption). **Mis à 1 par le matériel quand l'opération en cours est terminée.** Vous **l'effacez en écrivant 1**, et *l'effacer lance l'opération suivante*. Tant que TWINT est à 1, SCL est maintenue basse (le bus vous attend).
- **TWEA** : Enable Acknowledge (activer l'acquittement). En réception : 1 = envoyer ACK, 0 = envoyer NACK.
- **TWSTA** : générer un START.
- **TWSTO** : générer un STOP.
- TWWC : collision d'écriture (vous avez écrit TWDR pendant que TWINT valait 0).
- **TWEN** : activer le TWI.
- TWIE : activation de l'interruption (`TWI_vect`).

**TWDR** : Data Register (registre de données). L'octet à envoyer, ou l'octet reçu.
**TWAR** : (esclave) propre adresse dans les bits 7:1, bit 0 = répondre à l'appel général (general call, adresse 0).

**La mécanique fondamentale :** chaque action TWI consiste à « écrire TWCR avec TWINT=1 + ce que vous voulez, puis attendre que TWINT repasse à 1, puis lire l'état dans TWSR ».

```c
TWCR = (1 << TWINT) | (1 << TWEN) | (autres bits);   // go
while (!(TWCR & (1 << TWINT))) { }                   // attendre la fin
status = TWSR & 0xF8;                                // que s'est-il passé ?
```

⚠️ Écrivez toujours TWCR **avec `=`**, pas `|=`. Avec `|=`, vous risquez de remettre à 1 des bits TWSTA ou TWSTO restés de l'opération précédente.

### 17.9 Codes d'état (mode maître)

| Code | Signification | Mode |
|---|---|---|
| **0x08** | START transmis | MT/MR |
| **0x10** | START répété transmis | MT/MR |
| **0x18** | SLA+W envoyé, **ACK** reçu | MT |
| **0x20** | SLA+W envoyé, **NACK** reçu (personne à cette adresse !) | MT |
| **0x28** | Octet de données envoyé, **ACK** reçu | MT |
| **0x30** | Octet de données envoyé, NACK reçu | MT |
| **0x38** | Arbitrage perdu | MT/MR |
| **0x40** | SLA+R envoyé, **ACK** reçu | MR |
| **0x48** | SLA+R envoyé, NACK reçu | MR |
| **0x50** | Octet de données reçu, ACK renvoyé | MR |
| **0x58** | Octet de données reçu, NACK renvoyé | MR |
| 0xF8 | Aucun état pertinent (TWINT = 0) | |
| 0x00 | Erreur de bus (START/STOP illégal) | |

(MT = Master Transmitter, maître émetteur ; MR = Master Receiver, maître récepteur ; SLA = SLave Address, adresse de l'esclave.)

Codes du mode esclave (pour le Rush 00 / le multijoueur) :

| Code | Signification |
|---|---|
| **0x60** | Notre SLA+W reçu, ACK renvoyé |
| 0x68 | Arbitrage perdu, notre SLA+W reçu |
| 0x70 | Appel général reçu, ACK renvoyé |
| **0x80** | (après notre SLA+W) donnée reçue, ACK renvoyé |
| 0x88 | donnée reçue, NACK renvoyé |
| **0xA0** | STOP ou START répété reçu pendant qu'on était adressé |
| **0xA8** | Notre SLA+R reçu, ACK renvoyé → charger dans TWDR la donnée à envoyer |
| **0xB8** | Donnée transmise, ACK reçu → charger l'octet suivant |
| **0xC0** | Donnée transmise, NACK reçu → le maître a fini |
| 0xC8 | Dernier octet transmis (TWEA = 0), ACK reçu |

Ces constantes sont disponibles par leur nom dans `<util/twi.h>` (`TW_START`, `TW_MT_SLA_ACK`…), si c'est autorisé.

### 17.10 Pilote maître

```c
#include <avr/io.h>

#define I2C_FREQ 100000UL

void i2c_init(void)
{
    TWSR = 0;                                         // prescaler = 1
    TWBR = (uint8_t)((F_CPU / I2C_FREQ - 16) / 2);   // = 72 pour 100 kHz
    TWCR = (1 << TWEN);                               // activer le TWI
}

static inline void i2c_wait(void)
{
    while (!(TWCR & (1 << TWINT))) { }
}

static inline uint8_t i2c_status(void)
{
    return TWSR & 0xF8;
}

// Renvoie le code d'état (0x08 ou 0x10 en cas de succès)
uint8_t i2c_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2c_wait();
    return i2c_status();
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    // le STOP se termine tout seul : TWINT n'est PAS levé après un stop ; attendre plutôt que TWSTO repasse à 0
    while (TWCR & (1 << TWSTO)) { }
}

// Envoie un octet (adresse ou donnée). Renvoie le code d'état.
uint8_t i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    i2c_wait();
    return i2c_status();
}

// Lit un octet en répondant ACK (d'autres octets sont attendus)
uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    i2c_wait();
    return TWDR;
}

// Lit un octet en répondant NACK (dernier octet)
uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);
    i2c_wait();
    return TWDR;
}
```

**Ex00 typique du Day 06 :** « Initialiser l'I2C à 100 kHz, envoyer un START et l'adresse de l'AHT20, afficher chaque code d'état sur l'UART. » Cela vous apprend à lire la table des états. Sortie attendue : `0x08` (START OK), puis `0x18` (adresse acquittée), et après STOP plus rien. Si vous voyez `0x20`, l'adresse est fausse ou le composant ne répond pas.

```c
i2c_init();
uart_puthex8(i2c_start());               uart_printstr("\r\n");   // attendu : 08
uart_puthex8(i2c_write(0x38 << 1 | 0));  uart_printstr("\r\n");   // attendu : 18
i2c_stop();
```

### 17.11 Bonus : scanner de bus I2C

L'outil de débogage I2C le plus utile : essayer chaque adresse et indiquer lesquelles répondent ACK.

```c
void i2c_scan(void)
{
    for (uint8_t addr = 1; addr < 0x78; addr++)
    {
        uint8_t st = i2c_start();
        if (st == 0x08 || st == 0x10)
        {
            if (i2c_write(addr << 1) == 0x18)          // SLA+W acquitté → composant présent
            {
                uart_printstr("Found device at 0x");
                uart_puthex8(addr);
                uart_printstr("\r\n");
            }
        }
        i2c_stop();
    }
}
```
Sur la devboard typique, vous devriez voir 0x20 (PCA9555), 0x38 (AHT20) et 0x51 (RTC PCF8563).

### 17.12 Le capteur de température et d'humidité AHT20

Lisez sa datasheet : elle est courte (~10 pages) et c'est un bon exercice. Points clés :
- Adresse I2C **0x38**
- Après la mise sous tension, attendre **≥ 40 ms** (la datasheet dit 40 ms ; beaucoup de pilotes attendent 100 ms).
- **Octet d'état** (lire 1 octet) : bit 7 = **Busy** (occupé, mesure en cours), bit 3 = **Calibration enabled** (étalonnage activé, CAL).
- Si CAL = 0, envoyer la **commande d'initialisation** : `0xBE 0x08 0x00`, puis attendre 10 ms.
- **Déclencher une mesure** : envoyer `0xAC 0x33 0x00`.
- Attendre **≥ 80 ms** (ou scruter le bit busy).
- **Lire 7 octets** : `état, H1, H2, H3/T1, T2, T3, CRC` :

```
  octet :  [0]       [1]       [2]       [3]         [4]       [5]       [6]
          état    hum[19:12] hum[11:4] hum[3:0]|    temp[15:8] temp[7:0] CRC
                                       temp[19:16]
```

- **Humidité** brute (20 bits) : `S_RH = (b1 << 12) | (b2 << 4) | (b3 >> 4)`
- **Température** brute (20 bits) : `S_T = ((b3 & 0x0F) << 16) | (b4 << 8) | b5`
- Conversion :
  ```
    HR [%]  = S_RH / 2²⁰ × 100
    T  [°C] = S_T  / 2²⁰ × 200 − 50
  ```

Attention aux types : `b1 << 12` sur un `uint8_t` est promu en `int` (16 bits), donc les bits de poids fort sont perdus ! Castez d'abord en `uint32_t`.

```c
#define AHT20_ADDR 0x38

void aht20_init(void)
{
    _delay_ms(40);
    // lire l'état
    i2c_start();
    i2c_write((AHT20_ADDR << 1) | 1);
    uint8_t status = i2c_read_nack();
    i2c_stop();

    if (!(status & 0x08))                      // pas étalonné → envoyer la commande d'initialisation
    {
        i2c_start();
        i2c_write(AHT20_ADDR << 1);
        i2c_write(0xBE);
        i2c_write(0x08);
        i2c_write(0x00);
        i2c_stop();
        _delay_ms(10);
    }
}

// Renvoie 0 en cas de succès ; remplit la température en centièmes de °C et l'humidité en centièmes de %
int8_t aht20_measure(int32_t *t100, uint32_t *rh100)
{
    uint8_t d[7];

    i2c_start();
    i2c_write(AHT20_ADDR << 1);
    i2c_write(0xAC);                           // déclencher la mesure
    i2c_write(0x33);
    i2c_write(0x00);
    i2c_stop();

    _delay_ms(80);

    do {                                       // attendre qu'il ne soit plus occupé
        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 1);
        for (uint8_t i = 0; i < 6; i++)
            d[i] = i2c_read_ack();
        d[6] = i2c_read_nack();                // CRC, dernier octet → NACK
        i2c_stop();
    } while (d[0] & 0x80);

    uint32_t raw_h = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
    uint32_t raw_t = ((uint32_t)(d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];

    // Virgule fixe : ×100 pour deux décimales.  x / 2^20 * 10000 = x * 10000 >> 20
    //   10000 / 2^20 = 625 / 2^16 → tient sur 32 bits : 2^20 * 625 ≈ 6,5e8 < 4,29e9
    *rh100 = (raw_h * 625UL) >> 16;                       // 0 .. 10000
    //   T*100 = raw/2^20 * 20000 − 5000 ; 20000/2^20 = 625/2^15
    *t100  = (int32_t)((raw_t * 625UL) >> 15) - 5000;     // ex. 2347 = 23,47 °C
    return 0;
}
```

Le sujet peut vous demander d'abord d'afficher les octets bruts en hexa (pour valider la communication), puis de calculer les valeurs et de les afficher sous la forme `Temperature: 23.4°C, Humidity: 45.2%`, éventuellement **moyennées sur les 3 dernières mesures**. Afficher `°` nécessite d'envoyer les octets UTF-8 `0xC2 0xB0` ; beaucoup de gens affichent simplement « C ».

Si les floats sont autorisés :
```c
float t = raw_t / 1048576.0f * 200.0f - 50.0f;
```
Il faut quand même afficher le float vous-même : partie entière, puis `(int)((t - (int)t) * 10)` pour une décimale. Attention aux valeurs négatives.

**Vérification CRC-8** (optionnelle mais professionnelle) : le 7ᵉ octet est un CRC (contrôle de redondance cyclique) sur les octets 0 à 5, avec le polynôme 0x31 (x⁸+x⁵+x⁴+1) et la valeur initiale 0xFF :
```c
uint8_t crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++)
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
    }
    return crc;
}
// valide si crc8(d, 6) == d[6]
```

### 17.13 Gestion des erreurs et robustesse

Les vrais pilotes vérifient chaque code d'état et abandonnent proprement :
```c
uint8_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val)
{
    if (i2c_start() != 0x08)            goto fail;
    if (i2c_write(addr << 1) != 0x18)   goto fail;   // pas d'ACK : composant absent
    if (i2c_write(reg) != 0x28)         goto fail;
    if (i2c_write(val) != 0x28)         goto fail;
    i2c_stop();
    return 0;
fail:
    i2c_stop();
    return 1;
}
```

**Bus bloqué** (hung bus) : si un transfert a été interrompu (reset au milieu d'une lecture), un esclave peut maintenir SDA à l'état bas indéfiniment. Récupération : désactiver le TWI, faire basculer SCL manuellement 9 fois en GPIO, puis envoyer un STOP. De plus, une boucle `while (!(TWCR & (1 << TWINT)))` sans délai d'expiration (timeout) bloquera pour toujours si le bus est défaillant, par exemple en l'absence de pull-ups. Ajoutez un compteur de timeout dans un code robuste.

### 17.14 Pièges

- Adresse décalée deux fois (`0x70 << 1`) ou pas du tout.
- Oublier le bit R/W.
- `TWCR |=` au lieu de `TWCR =`.
- Ne pas masquer les bits de prescaler de TWSR (`TWSR & 0xF8`).
- Attendre TWINT après un STOP (il ne vient jamais) : attendez plutôt TWSTO, ou n'attendez pas.
- Acquitter (ACK) le dernier octet d'une lecture : l'esclave continue de piloter SDA et le STOP échoue.
- `uint8_t << 12` déborde : castez d'abord.
- Ne pas attendre assez longtemps après le déclenchement d'une mesure.


---

## 18. Day 07 : EEPROM

### 18.1 Objectifs

- Comprendre les différents types de mémoire, et ce qui rend l'EEPROM particulière.
- Lire et écrire l'EEPROM interne de l'ATmega328P via ses registres (la séquence d'écriture temporisée).
- Afficher la mémoire au format **hexdump**.
- Concevoir un **format de stockage persistant robuste** : nombres magiques (magic numbers), en-têtes, sommes de contrôle (checksums), un petit stockage clé-valeur (key-value store).

### 18.2 Types de mémoire

| Mémoire | Volatile ? | Écrite par | Granularité d'effacement | Endurance | Usage sur AVR |
|---|---|---|---|---|---|
| **SRAM** | oui (perdue à l'extinction) | le CPU, librement | octet | illimitée | variables, pile |
| **Flash** | non | programmateur/bootloader (SPM) | **page** (128 octets sur le 328P) | ~10 000 cycles | code du programme, constantes |
| **EEPROM** | non | le CPU, via des registres | **octet** | ~**100 000** cycles | réglages, étalonnage, scores |
| ROM/PROM/EPROM | non | usine / une seule fois / lumière UV | – | – | historiques |

**EEPROM** = Electrically Erasable Programmable Read-Only Memory (mémoire morte programmable effaçable électriquement). Malgré le « read-only » (lecture seule) de son nom, elle peut être réécrite octet par octet, simplement lentement (~3,3 ms par octet) et un nombre limité de fois.

EEPROM de l'ATmega328P : **1024 octets**, adresses 0x000–0x3FF. Rétention des données : 20 ans à 85 °C / 100 ans à 25 °C.

**L'état effacé de l'EEPROM est `0xFF`**. Une puce neuve (ou effacée) lit 0xFF partout. Ne supposez pas des zéros !

### 18.3 Les registres de l'EEPROM

**EEAR** (EEARH:EEARL) : adresse (10 bits utilisés : 0–1023)
**EEDR** : registre de données (octet à écrire / octet lu)
**EECR** : registre de contrôle
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | – | – | EEPM1 | EEPM0 | EERIE | **EEMPE** | **EEPE** | **EERE** |

- **EERE** : Read Enable (activation de lecture). Déclenche la lecture d'EEAR vers EEDR.
- **EEPE** : Program Enable (activation d'écriture). Lance une écriture. Reste à 1 pendant l'écriture (~3,3 ms) et repasse à 0 à la fin.
- **EEMPE** : Master Program Enable (activation maîtresse d'écriture). Un verrou de sécurité : EEPE ne fonctionne que si EEMPE a été mis à 1 dans les **4 cycles d'horloge précédents**.
- EERIE : EEPROM Ready Interrupt Enable, interruption « EEPROM prête » (`EE_READY_vect`).
- EEPM1:0 : mode de programmation. 00 = effacement + écriture (atomique, 3,4 ms), 01 = effacement seul, 10 = écriture seule.

### 18.4 Lire et écrire (d'après la datasheet)

La datasheet fournit ces fonctions, presque mot pour mot :

```c
#include <avr/io.h>

void eeprom_write(uint16_t addr, uint8_t data)
{
    while (EECR & (1 << EEPE)) { }        // 1. attendre la fin de l'écriture précédente
    EEAR = addr;                          // 2. positionner l'adresse
    EEDR = data;                          // 3. positionner la donnée
    EECR |= (1 << EEMPE);                 // 4. autorisation maîtresse d'écriture
    EECR |= (1 << EEPE);                  // 5. lancer l'écriture — doit être dans les 4 cycles suivant l'étape 4 !
}

uint8_t eeprom_read(uint16_t addr)
{
    while (EECR & (1 << EEPE)) { }        // attendre toute écriture en cours
    EEAR = addr;
    EECR |= (1 << EERE);                  // lancer la lecture (le CPU s'arrête 4 cycles, puis la donnée est prête)
    return EEDR;
}
```

**Pourquoi cette danse EEMPE/EEPE ?** Elle protège l'EEPROM des écritures accidentelles provoquées par un programme qui s'emballe. Un saut aléatoire a très peu de chances d'exécuter les deux instructions dans l'ordre en moins de 4 cycles.

**Le timing critique :** si une **interruption** survient entre l'étape 4 et l'étape 5, plus de 4 cycles s'écoulent et l'écriture n'a silencieusement pas lieu. Donc si les interruptions sont activées, protégez la séquence :

```c
#include <util/atomic.h>
...
ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
{
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}
```

Notez aussi : `EECR |= (1 << EEMPE);` ne doit pas *aussi* mettre EEPE à 1 dans la même écriture, car la datasheet exige deux étapes séparées. Compilez avec les optimisations (`-Os`) pour que les deux instructions deviennent deux `SBI` consécutives.

**Timing :** une écriture prend ~3,3 ms, pendant lesquelles le CPU continue de tourner. Seul le prochain accès EEPROM attend. Écrire les 1024 octets prend ~3,5 secondes.

### 18.5 Économiser les écritures : « update » plutôt que « write »

Chaque écriture use la cellule (~100 000 cycles garantis). Réécrire la même valeur est une usure et une perte de temps inutiles. Comparez toujours d'abord :

```c
void eeprom_update(uint16_t addr, uint8_t data)
{
    if (eeprom_read(addr) != data)
        eeprom_write(addr, data);
}
```

100 000 cycles, cela paraît beaucoup, mais si vous sauvegardez une valeur chaque seconde dans une boucle, vous tuez cet octet en **28 heures**. Sauvegardez seulement en cas de changement, ou sur action explicite de l'utilisateur.

### 18.6 `<avr/eeprom.h>` d'avr-libc (si autorisé)

```c
#include <avr/eeprom.h>

uint8_t  v  = eeprom_read_byte((const uint8_t *)10);
uint16_t w  = eeprom_read_word((const uint16_t *)12);
eeprom_update_byte((uint8_t *)10, 42);
eeprom_update_block(&my_struct, (void *)100, sizeof(my_struct));

// Variables placées dans la section .eeprom (adresses attribuées par l'éditeur de liens) :
uint8_t EEMEM saved_mode = 3;      // la valeur initiale va dans main.eep
uint8_t mode = eeprom_read_byte(&saved_mode);
```

Notez que les valeurs initiales des variables `EEMEM` ne sont **pas** flashées avec `make flash`. Elles vont dans un fichier `.eep` séparé, à téléverser avec `-U eeprom:w:main.eep:i`.

**Le flashage efface-t-il l'EEPROM ?** Un effacement complet de la puce (chip erase, fait avant le flashage via ISP) efface l'EEPROM, sauf si le fusible **EESAVE** est actif. Les bootloaders de type Arduino (Optiboot) ne font pas de chip erase, donc le contenu de l'EEPROM survit généralement au re-flashage. Testez-le sur votre carte. Cela compte pour des exercices du type « un compteur qui survit à un reset / un re-flashage ».

Pour extraire l'EEPROM depuis votre PC :
```bash
avrdude -p m328p -c arduino -P /dev/ttyUSB0 -b 115200 -U eeprom:r:dump.hex:i
```
(Les bootloaders peuvent ne pas supporter la lecture de l'EEPROM. Optiboot la supporte dans ses versions récentes.)

### 18.7 Hexdump

Exercice typique : afficher toute l'EEPROM dans un format semblable à `hexdump`. Regardez ce qu'affichent les outils de votre système :

```
$ hexdump -C file              # format canonique
00000000  48 65 6c 6c 6f 20 57 6f  72 6c 64 0a ff ff ff ff  |Hello World.....|
00000010  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
*

$ hexdump file                 # format par défaut : mots de 16 bits, petit-boutiste (little-endian) !
0000000 6548 6c6c 206f 6f57 6c72 0a64 ffff ffff
```

(`hexdump` sans `-C` affiche des mots de 16 bits en little-endian : les octets `48 65` apparaissent comme `6548`. Lisez attentivement l'exemple de sortie du sujet et reproduisez-le **exactement**.)

Implémentation :

```c
void print_hex_n(uint32_t v, uint8_t digits)     // hexa à largeur fixe avec zéros de tête
{
    const char *hex = "0123456789abcdef";
    while (digits--)
        uart_tx(hex[(v >> (digits * 4)) & 0xF]);
}

void eeprom_hexdump(void)
{
    for (uint16_t addr = 0; addr < 1024; addr += 16)
    {
        print_hex_n(addr, 8);                     // colonne d'adresse
        uart_printstr("  ");
        for (uint8_t i = 0; i < 16; i++)
        {
            print_hex_n(eeprom_read(addr + i), 2);
            uart_tx(' ');
            if (i == 7) uart_tx(' ');             // espace supplémentaire au milieu
        }
        uart_printstr(" |");
        for (uint8_t i = 0; i < 16; i++)          // colonne ASCII
        {
            uint8_t c = eeprom_read(addr + i);
            uart_tx((c >= 0x20 && c <= 0x7E) ? c : '.');
        }
        uart_printstr("|\r\n");
    }
}
```

**Exercice suivant typique :** l'utilisateur tape une adresse et un octet (par ex. `0x10 0x42` ou `10 42`), le programme l'écrit (seulement s'il est différent), et le hexdump **met en évidence l'octet modifié en rouge** avec des codes ANSI (`\x1b[31m` … `\x1b[0m`). Si la valeur est déjà présente, il ne fait rien, ce qui démontre la logique « update ».

### 18.8 Concevoir des données persistantes : les nombres magiques

Problème : après la mise sous tension, comment savoir si l'EEPROM contient **vos** données valides, ou n'importe quoi (0xFF d'une puce neuve, ou restes d'un autre programme, peut-être celui d'un camarade) ?

**Solution : un nombre magique** (magic number), un octet (ou plusieurs) spécifique écrit à côté de vos données, peu susceptible d'apparaître par hasard :

```
  adresse : 0x00   0x01   0x02 ...
           [0x42] [ données ............. ]
           magique
```

```c
#define MAGIC 0x42
#define ADDR_MAGIC 0
#define ADDR_COUNTER 1

uint8_t load_counter(void)
{
    if (eeprom_read(ADDR_MAGIC) != MAGIC)         // jamais initialisé (ou corrompu)
    {
        eeprom_update(ADDR_COUNTER, 0);           // valeur par défaut
        eeprom_update(ADDR_MAGIC, MAGIC);         // écrire le magique EN DERNIER : les données ne sont valides qu'une fois complètes
        return 0;
    }
    return eeprom_read(ADDR_COUNTER);
}
```

**Écrivez le magique en dernier** : si le courant est coupé au milieu de la sauvegarde, le magique n'est pas encore là, donc les données sont ignorées au lieu d'être lues comme un n'importe-quoi à moitié écrit.

Pour des garanties plus fortes, ajoutez une **somme de contrôle** (checksum : somme ou XOR des octets de données, ou un CRC-8) et vérifiez-la au chargement.

Un enchaînement d'exercices typique :
1. Un compteur (0–15 sur les LEDs) incrémenté par SW1, qui **survit à un reset** (stocké en EEPROM avec un octet magique).
2. **Plusieurs compteurs** (par ex. 4, sélectionnés avec SW2), chacun persisté avec son nombre magique.
3. Une API générique : `safe_eeprom_read(void *buffer, size_t offset, size_t length)` / `safe_eeprom_write(...)`, qui stocke un **octet magique avant chaque bloc de données** et refuse de lire les blocs invalides.

### 18.9 Un stockage clé-valeur en EEPROM

Un exercice final classique : implémenter des commandes via l'UART.

```
> WRITE "name" "spectre"
done (at 0x0012)
> READ "name"
"spectre"
> WRITE "name" "other"
already exists
> FORGET "name"
done
> READ "name"
empty
> PRINT
(hexdump de l'eeprom)
```

**Options de conception :**

**Option A : enregistrements de longueur variable, séquentiels.**
```
  [MAGIC][STATUS][KEY_LEN][VAL_LEN][ octets de clé ... ][ octets de valeur ... ]  [MAGIC][STATUS]...
   1 o     1 o      1 o      1 o     KEY_LEN               VAL_LEN
  STATUS : 0x01 = utilisé, 0x00 = supprimé
```
- **READ** : parcourir les enregistrements depuis l'adresse 0. S'arrêter au premier octet qui n'est pas MAGIC (espace libre).
- **WRITE** : vérifier que la clé n'existe pas, puis soit ajouter après le dernier enregistrement, soit réutiliser un enregistrement supprimé assez grand.
- **FORGET** : mettre STATUS à « supprimé ». N'effacez pas tout l'enregistrement, pour minimiser l'usure.
- Problème : **fragmentation** après de nombreuses écritures/suppressions. Une opération de « défragmentation » compacte les enregistrements (coûteuse en usure).

**Option B : emplacements de taille fixe (plus simple).** Par exemple, 1024 / 64 = 16 emplacements (slots) de 64 octets :
```
  slot : [MAGIC 1o][USED 1o][CLÉ 31o + '\0'][VALEUR 30o + '\0']
```
Recherche facile (itérer sur 16 emplacements), pas de fragmentation, mais cela limite la longueur des clés/valeurs et gaspille de la place. C'est généralement suffisant pour la piscine si le sujet autorise ces limites.

**L'analyse des commandes** nécessite un petit analyseur lexical (tokenizer) gérant les chaînes entre guillemets : `WRITE "a key" "a value"`. Parcourez la ligne caractère par caractère en collectant le texte entre guillemets. Rejetez les saisies mal formées : guillemets manquants, trop long, clé vide.

### 18.10 Répartition de l'usure (wear leveling, culture générale)

Pour prolonger la durée de vie de données fréquemment mises à jour, répartissez les écritures sur de nombreuses cellules. Exemple : stocker un compteur dans un anneau de 16 emplacements, en écrivant chaque nouvelle valeur dans l'emplacement suivant avec un numéro de séquence. Au démarrage, cherchez l'emplacement au numéro de séquence le plus élevé. Chaque cellule ne voit alors que 1/16 des écritures. Les SSD et les cartes SD le font en interne à très grande échelle.

### 18.11 Pièges

- Supposer qu'une mémoire effacée lit 0 (elle lit **0xFF**).
- Perdre une écriture parce qu'une interruption a coupé la séquence EEMPE/EEPE.
- Écrire dans une boucle serrée et user l'EEPROM.
- Les adresses au-delà de 1023 rebouclent (seuls 10 bits d'EEAR sont utilisés).
- Oublier qu'`int` fait 16 bits quand on stocke des valeurs multi-octets : écrivez/lisez chaque octet explicitement avec `>> 8` et `& 0xFF`, et définissez l'ordre des octets (le little-endian est naturel sur AVR).
- Format de sortie ne correspondant pas caractère pour caractère au hexdump attendu par le sujet.

---

## 19. Day 08 : SPI

### 19.1 Objectifs

- Comprendre le bus **SPI** : 4 fils, full duplex, maître/esclave, modes d'horloge.
- Configurer le périphérique SPI de l'AVR en maître.
- Piloter des LEDs RGB adressables **APA102** (D6, D7, D8 sur la devboard typique) : format de trame, luminosité, ordre des couleurs.
- Combiner SPI, ADC et UART (jauge, commandes de couleur).

### 19.2 Le bus SPI

Le **SPI** (Serial Peripheral Interface, interface série pour périphériques) a été inventé par Motorola. C'est un bus **synchrone**, **full duplex**, **maître/esclave**, plus simple et beaucoup plus rapide que l'I2C.

| Signal | Nom | Sens | Rôle |
|---|---|---|---|
| **SCK** | Serial Clock (horloge série) | maître → esclave | horloge générée par le maître |
| **MOSI** | Master Out Slave In | maître → esclave | données vers l'esclave |
| **MISO** | Master In Slave Out | esclave → maître | données depuis l'esclave |
| **SS** / CS | Slave Select / Chip Select (sélection d'esclave / de puce) | maître → esclave | **actif à l'état BAS** : sélectionne l'esclave qui écoute |

(Les documents récents utilisent COPI/CIPO/CS : Controller Out Peripheral In, etc.)

Sur l'ATmega328P : **SS = PB2, MOSI = PB3, MISO = PB4, SCK = PB5**.

**Plusieurs esclaves** : MOSI, MISO et SCK sont partagés, et **chaque esclave a sa propre ligne SS**. Le maître tire une seule ligne SS à l'état bas à la fois. **Il n'y a pas d'adresses** : la sélection est purement électrique.

```
   Maître                        Esclave 1        Esclave 2
   ┌──────┐ SCK  ───────────────►│SCK      │───►│SCK     │
   │      │ MOSI ───────────────►│MOSI     │───►│MOSI    │
   │      │ MISO ◄───────────────│MISO     │◄───│MISO    │
   │      │ SS1  ───────────────►│CS       │    │        │
   │      │ SS2  ────────────────┼─────────┼───►│CS      │
   └──────┘                      └─────────┘    └────────┘
```

**Fonctionnement du transfert : deux registres à décalage (shift registers) en anneau.** À chaque impulsion d'horloge, le maître fait sortir un bit sur MOSI et entrer un bit depuis MISO ; l'esclave fait l'inverse. Après 8 impulsions, **l'octet du maître et celui de l'esclave ont été échangés** :

```
     Registre à décalage du maître    Registre à décalage de l'esclave
    ┌─┬─┬─┬─┬─┬─┬─┬─┐  MOSI   ┌─┬─┬─┬─┬─┬─┬─┬─┐
    │7│6│5│4│3│2│1│0│ ──────► │7│6│5│4│3│2│1│0│
    └─┴─┴─┴─┴─┴─┴─┴─┘         └─┴─┴─┴─┴─┴─┴─┴─┘
          ▲                             │
          └───────────  MISO ◄──────────┘
```

Donc **le SPI envoie et reçoit toujours en même temps**. Pour « lire », on envoie un octet factice (souvent 0x00 ou 0xFF) et on récupère ce qui revient. Pour « écrire », on envoie et on ignore ce qui revient.

**SPI vs I2C vs UART :**

| | UART | I2C | SPI |
|---|---|---|---|
| Fils | 2 (TX, RX) + GND | 2 (SDA, SCL) + GND | 4 (SCK, MOSI, MISO, un SS par esclave) + GND |
| Horloge | aucune (asynchrone, baud convenu) | oui (maître) | oui (maître) |
| Duplex | full | half | full |
| Adressage | aucun (point à point) | adresse 7 bits | lignes de chip select |
| Acquittement | aucun | ACK/NACK par octet | aucun |
| Vitesse typique | 9,6k–1M bauds | 100k–400k (1M+) | 1–50 MHz (AVR : jusqu'à 8 MHz) |
| Multi-composants | non | oui (beaucoup sur 2 fils) | oui (1 fil supplémentaire chacun) |
| Usage typique | console, GPS, modules Bluetooth | capteurs, EEPROM, RTC, extenseurs | écrans, cartes SD, mémoires flash, ADC rapides, rubans de LEDs |

### 19.3 Modes SPI : CPOL et CPHA

Maître et esclave doivent s'accorder sur le **moment** où les données sont échantillonnées :
- **CPOL** (Clock Polarity, polarité d'horloge) : niveau de repos de SCK. 0 = repos à l'état BAS, 1 = repos à l'état HAUT.
- **CPHA** (Clock Phase, phase d'horloge) : 0 = données **échantillonnées sur le premier front** (leading edge) ; 1 = échantillonnées sur le second front (trailing edge).

| Mode | CPOL | CPHA | SCK au repos | Échantillonnage sur | Préparation sur |
|---|---|---|---|---|---|
| **0** | 0 | 0 | bas | front montant | front descendant |
| 1 | 0 | 1 | bas | front descendant | front montant |
| 2 | 1 | 0 | haut | front descendant | front montant |
| 3 | 1 | 1 | haut | front montant | front descendant |

Le **mode 0** est de loin le plus courant (l'APA102 utilise le mode 0 ; le mode 3 fonctionne aussi pour lui). La datasheet de l'esclave vous dit quel mode utiliser, généralement avec un chronogramme.

```
  Mode 0 :
  SS   ‾‾\______________________________________/‾‾
  SCK  ____/‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾\________
  MOSI ---X b7 X b6 X b5 X b4 X b3 X b2 X b1 X b0 X---
            ↑ échantillonné à chaque front montant
```

**Ordre des bits :** généralement MSB en premier (le bit DORD le sélectionne).

### 19.4 Les registres SPI de l'AVR

**SPCR** : SPI Control Register (registre de contrôle)
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| | SPIE | **SPE** | DORD | **MSTR** | CPOL | CPHA | SPR1 | SPR0 |

- SPIE : activation de l'interruption (`SPI_STC_vect`)
- **SPE** : activation du SPI
- DORD : 1 = LSB en premier, 0 = MSB en premier
- **MSTR** : 1 = mode maître
- CPOL, CPHA : mode
- SPR1:0 : vitesse d'horloge (avec SPI2X)

**SPSR** : SPI Status Register (registre d'état) : **SPIF** (bit 7 : transfert terminé), WCOL (bit 6 : collision d'écriture), **SPI2X** (bit 0 : double vitesse).

**SPDR** : SPI Data Register (registre de données). L'écrire lance un transfert ; le lire donne l'octet reçu.

**Vitesse d'horloge :**
| SPI2X | SPR1 | SPR0 | SCK | à 16 MHz |
|---|---|---|---|---|
| 0 | 0 | 0 | F_CPU/4 | 4 MHz |
| 0 | 0 | 1 | F_CPU/16 | 1 MHz |
| 0 | 1 | 0 | F_CPU/64 | 250 kHz |
| 0 | 1 | 1 | F_CPU/128 | 125 kHz |
| 1 | 0 | 0 | F_CPU/2 | 8 MHz |
| 1 | 0 | 1 | F_CPU/8 | 2 MHz |
| 1 | 1 | 0 | F_CPU/32 | 500 kHz |
| 1 | 1 | 1 | F_CPU/64 | 250 kHz |

### 19.5 Pilote maître

```c
#include <avr/io.h>

void spi_init(void)
{
    // MOSI (PB3), SCK (PB5) et SS (PB2) en sortie ; MISO (PB4) reste en entrée
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
    // Activer le SPI, mode maître, mode 0, MSB en premier, F_CPU/16 = 1 MHz
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;                             // lancer la transmission
    while (!(SPSR & (1 << SPIF))) { }        // attendre la fin (8 cycles de SCK)
    return SPDR;                             // lire SPDR après SPSR efface SPIF
}
```

**⚠️ Le piège de la broche SS (datasheet : « SS Pin Functionality ») :** en mode maître, si la **broche SS (PB2) est configurée en entrée** et passe à l'état BAS, l'AVR croit qu'un autre maître prend le contrôle du bus. Il **repasse silencieusement en mode esclave** (MSTR mis à 0) et vos transferts cessent de fonctionner. **Configurez toujours PB2 en sortie en mode maître**, même si vous ne l'utilisez pas comme chip select. (Sur la devboard typique, PB2 est aussi la LED D3. Ce n'est pas grave : mettez-la en sortie, et la LED reflétera simplement l'état de SS.)

De même, MISO (PB4) est forcée en entrée par le matériel SPI en mode maître, donc la LED D4 sur PB4 peut se comporter bizarrement pendant le Day 08. C'est attendu.

### 19.6 La LED APA102

L'**APA102** (et son clone SK9822) est une LED RGB avec un **contrôleur intégré**. Elle reçoit les couleurs par une interface 2 fils de type SPI (**données + horloge**, pas de chip select, pas de MISO), et transmet les données à la LED suivante, ce qui permet de les **chaîner** (daisy-chain) par centaines.

```
  MOSI du MCU ──► [DI  APA102  DO] ──► [DI  APA102  DO] ──► [DI  APA102  DO]
  SCK du MCU  ──► [CI   (D6)   CO] ──► [CI   (D7)   CO] ──► [CI   (D8)   CO]
```

**Format de trame** (lisez la datasheet de l'APA102) :

```
  ┌────────────────────┬──────────────┬──────────────┬──────────────┬────────────────────┐
  │  trame de START    │ trame LED 1  │ trame LED 2  │ trame LED 3  │    trame de FIN    │
  │ 32 bits à 0        │   32 bits    │   32 bits    │   32 bits    │ 32 bits à 1        │
  │ 0x00 0x00 0x00 0x00│              │              │              │ 0xFF 0xFF 0xFF 0xFF│
  └────────────────────┴──────────────┴──────────────┴──────────────┴────────────────────┘

  Trame LED (32 bits), MSB en premier :
  ┌─────────────────────────┬──────────┬──────────┬──────────┐
  │ 1 1 1 │ luminosité (5b) │  BLEU    │  VERT    │  ROUGE   │
  │  0b111BBBBB             │  8 bits  │  8 bits  │  8 bits  │
  └─────────────────────────┴──────────┴──────────┴──────────┘
       0xE0 | (0..31)
```

- Chaque LED **prend la première trame LED qu'elle reçoit** et transmet tout ce qui suit. C'est ainsi que la LED 1 reçoit la trame 1, la LED 2 la trame 2, etc.
- La **luminosité globale** (global brightness, 5 bits, 0–31) est une seconde couche d'atténuation, plus lente. Utilisez 31 (maximum) et contrôlez la couleur avec RGB, ou baissez-la pour ne pas vous éblouir (l'APA102 est très lumineuse à pleine puissance).
- **L'ordre des couleurs est B, G, R** (et non R, G, B) ! Certains clones diffèrent : testez d'abord avec du rouge pur.
- La **trame de fin** fournit des impulsions d'horloge supplémentaires pour pousser les données à travers la chaîne (chaque LED retarde les données d'une demi-période d'horloge). La règle est d'au moins n/2 bits pour n LEDs ; 32 bits à 1 suffisent jusqu'à 64 LEDs.

```c
static void apa102_start(void)
{
    for (uint8_t i = 0; i < 4; i++) spi_transfer(0x00);
}

static void apa102_led(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    spi_transfer(0xE0 | (brightness & 0x1F));
    spi_transfer(b);
    spi_transfer(g);
    spi_transfer(r);
}

static void apa102_end(void)
{
    for (uint8_t i = 0; i < 4; i++) spi_transfer(0xFF);
}

// Règle les trois LEDs D6, D7, D8 d'un coup. Les couleurs sont au format 0xRRGGBB.
void set_leds(uint32_t c6, uint32_t c7, uint32_t c8)
{
    uint32_t c[3] = { c6, c7, c8 };
    apa102_start();
    for (uint8_t i = 0; i < 3; i++)
        apa102_led(c[i] >> 16, (c[i] >> 8) & 0xFF, c[i] & 0xFF, 31);
    apa102_end();
}
```

**Vous devez toujours envoyer les trames de toutes les LEDs jusqu'à celle que vous voulez.** Pour ne changer que D8, vous renvoyez aussi D6 et D7. Gardez les couleurs courantes de toutes les LEDs dans un tableau, modifiez-en une, et envoyez-les toutes.

### 19.7 Exercices typiques du Day 08

| Ex | Tâche | Concept clé |
|---|---|---|
| 00 | Allumer D6 en rouge | init SPI, trame APA102 |
| 01 | D6 alterne rouge, vert, bleu, jaune, cyan, magenta, blanc chaque seconde | trames dans une boucle |
| 02 | « Chenillard » : une LED allumée à la fois, D6 → D7 → D8 → éteint → …, toutes les 250 ms | trame pour toutes les LEDs |
| 03 | Jauge : le potentiomètre contrôle le nombre de LEDs allumées (≥33 % → D6, ≥66 % → D7, 100 % → D8) | ADC + SPI |
| 04 | Commandes UART : `#RRGGBBD6` règle cette LED ; `#FULLRAINBOW` anime la roue sur les trois | analyse + SPI + timer |

Pour l'ex04, l'arc-en-ciel doit continuer de tourner pendant que le programme attend une saisie UART. Utilisez une interruption de timer pour l'animation (ou une interruption RX + tampon circulaire), **et non** un `uart_rx()` bloquant dans la boucle principale qui figerait l'arc-en-ciel.

⚠️ Si l'ISR de timer de l'animation envoie des trames SPI pendant que le code principal envoie aussi des trames SPI (pour une nouvelle commande de couleur), les deux flux s'entremêlent et les LEDs affichent n'importe quoi. Gardez **toutes les écritures SPI dans un seul contexte** : soit l'ISR ne fait que lever un drapeau et la boucle principale fait le travail SPI, soit la boucle principale ne fait que mettre à jour un tableau de couleurs et l'ISR l'envoie.

### 19.8 Pièges

- SS (PB2) laissée en entrée → le SPI devient silencieusement esclave.
- Oublier la trame de start → la première trame LED est mal interprétée.
- Octet de luminosité sans le préfixe `111` → la trame est invalide.
- Envoyer RGB au lieu de BGR → mauvaises couleurs.
- N'envoyer qu'une seule trame LED alors que vous vouliez mettre à jour D8.
- Entremêler des écritures SPI depuis une ISR et depuis la boucle principale.

---

## 20. Day 09 : Extenseur d'E/S & afficheur 7 segments

### 20.1 Objectifs

- Comprendre **pourquoi** et **comment** augmenter le nombre de broches d'E/S avec un **extenseur d'E/S I2C** (I/O expander, PCA9555).
- Piloter des LEDs et lire un bouton via l'extenseur.
- Comprendre l'**afficheur 7 segments** (7-segment display) : segments, encodage, anode/cathode commune.
- Implémenter le **multiplexage** (multiplexing) pour afficher plusieurs chiffres grâce à la persistance rétinienne.
- Construire des compteurs et des affichages de capteurs sur le 7 segments.

### 20.2 Pourquoi un extenseur d'E/S ?

L'ATmega328P n'a qu'environ 20 broches d'E/S utilisables, et un afficheur 7 segments de 4 chiffres nécessite à lui seul 8 lignes de segments + 4 lignes de chiffres = **12 broches**. Un **extenseur d'E/S** est une puce qui vous donne des broches GPIO supplémentaires, contrôlées via un bus. Le **PCA9555** fournit **16 broches d'E/S** via l'**I2C** (2 fils, partagés avec les autres composants I2C). Donc 2 broches du MCU en donnent 16 nouvelles.

Le compromis, c'est la vitesse : chaque mise à jour de broche est une transaction I2C (~0,3–0,5 ms à 100 kHz), au lieu d'une écriture de registre en un seul cycle.

### 20.3 Le PCA9555

- 16 broches d'E/S, en deux ports de 8 bits : **IO0_0–IO0_7** (port 0) et **IO1_0–IO1_7** (port 1).
- Adresse I2C : `0 1 0 0 A2 A1 A0` → **0x20–0x27** (0x20 quand A2 = A1 = A0 = GND, ce qui est probable sur la devboard. Vérifiez le schéma).
- Chaque broche est **en entrée par défaut** (registres de configuration = 0xFF à la mise sous tension).
- Une sortie **INT** (drain ouvert, active à l'état bas) signale les changements en entrée, et peut être reliée à une broche d'interruption du MCU.

**Registres (sélectionnés par l'« octet de commande », command byte) :**

| Commande | Registre | Défaut |
|---|---|---|
| 0 | Port d'entrée 0 (lecture seule : niveaux réels des broches) | – |
| 1 | Port d'entrée 1 | – |
| **2** | **Port de sortie 0** (niveau à piloter sur les broches en sortie) | 0xFF |
| **3** | **Port de sortie 1** | 0xFF |
| 4 | Inversion de polarité port 0 (1 = inverser la lecture en entrée) | 0x00 |
| 5 | Inversion de polarité port 1 | 0x00 |
| **6** | **Configuration port 0** (1 = entrée, **0 = sortie**, l'inverse du DDR de l'AVR !) | 0xFF |
| **7** | **Configuration port 1** | 0xFF |

⚠️ La polarité du registre de configuration est **inversée par rapport au DDR de l'AVR** : sur le PCA9555, **0 = sortie**.

**Écrire** un registre : `S | 0x20<<1 | W | A | commande | A | donnée | A | P`

Si vous envoyez deux octets de données, le second va dans le registre *jumeau* (par ex. commande 2, puis donnée pour le port 0, puis donnée pour le port 1). Pratique pour mettre à jour les deux ports en une seule transaction.

**Lire** un registre : `S | ADDR+W | A | commande | A | Sr | ADDR+R | A | donnée | NA | P`

```c
#define PCA9555_ADDR 0x20

void pca_write_reg(uint8_t reg, uint8_t value)
{
    i2c_start();
    i2c_write(PCA9555_ADDR << 1);        // mode écriture
    i2c_write(reg);                      // octet de commande
    i2c_write(value);
    i2c_stop();
}

void pca_write_reg2(uint8_t reg, uint8_t v0, uint8_t v1)   // les deux ports d'un coup
{
    i2c_start();
    i2c_write(PCA9555_ADDR << 1);
    i2c_write(reg);
    i2c_write(v0);
    i2c_write(v1);
    i2c_stop();
}

uint8_t pca_read_reg(uint8_t reg)
{
    i2c_start();
    i2c_write(PCA9555_ADDR << 1);        // écrire d'abord l'octet de commande
    i2c_write(reg);
    i2c_start();                         // START répété
    i2c_write((PCA9555_ADDR << 1) | 1);  // mode lecture
    uint8_t v = i2c_read_nack();
    i2c_stop();
    return v;
}
```

### 20.4 Ce qu'il y a sur l'extenseur (devboard typique ; vérifiez !)

Le câblage de la devboard typique de 42 est à peu près :
- **Port 0** : un bouton **SW3**, trois LEDs **D9, D10, D11**, et les **4 lignes de sélection de chiffre** (digit select) de l'afficheur 7 segments.
- **Port 1** : les **8 lignes de segments** (a, b, c, d, e, f, g, dp).

Les numéros de bits exacts, et le fait que les LEDs soient **actives à l'état bas** (très courant avec les extenseurs, qui absorbent mieux le courant qu'ils n'en fournissent), doivent venir de votre schéma. Notez-les sous forme de `#define` :

```c
// EXEMPLE SEULEMENT — remplacez par la vraie correspondance de votre schéma
#define EXP_SW3     (1 << 0)   // IO0_0
#define EXP_D11     (1 << 1)   // IO0_1
#define EXP_D10     (1 << 2)   // IO0_2
#define EXP_D9      (1 << 3)   // IO0_3
#define EXP_DIG1    (1 << 4)   // IO0_4 ... lignes de sélection de chiffre
#define EXP_DIG2    (1 << 5)
#define EXP_DIG3    (1 << 6)
#define EXP_DIG4    (1 << 7)
```

**Faire clignoter D9 via l'extenseur (ex00 typique) :**
```c
int main(void)
{
    i2c_init();
    pca_write_reg(6, (uint8_t)~EXP_D9);         // config port 0 : D9 = sortie (0), le reste en entrée
    while (1)
    {
        pca_write_reg(2, (uint8_t)~EXP_D9);     // port de sortie 0 : D9 bas → allumée (si active à l'état bas)
        _delay_ms(500);
        pca_write_reg(2, 0xFF);                 // tout à l'état haut → éteinte
        _delay_ms(500);
    }
}
```

**Lire SW3 (ex01 typique : un compteur 3 bits sur D9–D11 incrémenté par SW3) :** scrutez `pca_read_reg(0) & EXP_SW3` (0 = appuyé si actif à l'état bas), avec la détection de front et l'anti-rebond habituels. Une scrutation I2C toutes les ~10 ms convient.

### 20.5 L'afficheur 7 segments

Un chiffre 7 segments, ce sont **8 LEDs** (7 segments + point décimal) dans un boîtier, nommées a–g et dp (decimal point) :

```
       ─ a ─
      │     │
      f     b
      │     │
       ─ g ─
      │     │
      e     c
      │     │
       ─ d ─   • dp
```

**Cathode commune (CC, common cathode)** : toutes les cathodes des segments sont reliées ; segment ALLUMÉ = sa broche à l'état HAUT (et la broche commune à GND). **Anode commune (CA, common anode)** : toutes les anodes sont reliées ; segment ALLUMÉ = sa broche à l'état BAS (commune à VCC). Encore une fois, le schéma vous le dit. Souvent, des transistors pilotent les broches communes, ce qui peut inverser la logique une fois de plus.

**Table d'encodage** (bit 0 = a, bit 1 = b, … bit 6 = g, bit 7 = dp ; 1 = segment ALLUMÉ) :

| Chiffre | Segments ALLUMÉS | g f e d c b a | Hexa |
|---|---|---|---|
| 0 | a b c d e f | 0 1 1 1 1 1 1 | **0x3F** |
| 1 | b c | 0 0 0 0 1 1 0 | **0x06** |
| 2 | a b d e g | 1 0 1 1 0 1 1 | **0x5B** |
| 3 | a b c d g | 1 0 0 1 1 1 1 | **0x4F** |
| 4 | b c f g | 1 1 0 0 1 1 0 | **0x66** |
| 5 | a c d f g | 1 1 0 1 1 0 1 | **0x6D** |
| 6 | a c d e f g | 1 1 1 1 1 0 1 | **0x7D** |
| 7 | a b c | 0 0 0 0 1 1 1 | **0x07** |
| 8 | tous | 1 1 1 1 1 1 1 | **0x7F** |
| 9 | a b c d f g | 1 1 0 1 1 1 1 | **0x6F** |
| A | a b c e f g | 1 1 1 0 1 1 1 | 0x77 |
| b | c d e f g | 1 1 1 1 1 0 0 | 0x7C |
| C | a d e f | 0 1 1 1 0 0 1 | 0x39 |
| d | b c d e g | 1 0 1 1 1 1 0 | 0x5E |
| E | a d e f g | 1 1 1 1 0 0 1 | 0x79 |
| F | a e f g | 1 1 1 0 0 0 1 | 0x71 |
| – (moins) | g | 1 0 0 0 0 0 0 | 0x40 |
| (vide) | aucun | | 0x00 |

Pour un afficheur à anode commune, inversez : `~table[d]`. Si l'ordre de câblage des segments de votre carte diffère (par ex. IO1_0 = dp), construisez votre propre table à partir du schéma, segment par segment.

```c
static const uint8_t SEG[16] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
    0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};
```

**Afficher un chiffre (ex02 typique) :** configurez les deux ports en sortie (les bits nécessaires), sélectionnez un chiffre (sa ligne de sélection active), et écrivez le motif de segments sur le port 1 :

```c
pca_write_reg2(6, 0x00, 0x00);          // les 16 broches en sortie (à adapter : SW3 doit rester en entrée !)
pca_write_reg(3, SEG[2]);               // port 1 : segments du « 2 »
pca_write_reg(2, (uint8_t)~EXP_DIG4);   // port 0 : activer le chiffre 4 (sélection active à l'état bas, exemple)
```

### 20.6 Multiplexage : afficher 4 chiffres avec 12 fils

Les 4 chiffres **partagent les mêmes 8 lignes de segments**. Seules les lignes de sélection de chiffre sont séparées. Donc à un instant donné, **tous les chiffres activés affichent le même motif**. Pour afficher « 42 » :

1. Éteindre tous les chiffres.
2. Mettre les segments du « 4 » sur les lignes de segments, et activer le chiffre 3.
3. Attendre un court instant (~2–5 ms).
4. Éteindre tous les chiffres.
5. Mettre les segments du « 2 », et activer le chiffre 4.
6. Attendre.
7. Recommencer indéfiniment, rapidement.

Si le cycle complet se répète plus de ~50–60 fois par seconde, l'œil ne voit pas le scintillement (**persistance rétinienne**, persistence of vision), et tous les chiffres semblent allumés en même temps. Chaque chiffre n'est en réalité allumé que 1/4 du temps, il est donc moins lumineux qu'un chiffre unique statique.

```
  temps →
  DIG1  ▄▄▄▁▁▁▁▁▁▁▁▁▄▄▄▁▁▁▁▁▁▁▁▁
  DIG2  ▁▁▁▄▄▄▁▁▁▁▁▁▁▁▁▄▄▄▁▁▁▁▁▁
  DIG3  ▁▁▁▁▁▁▄▄▄▁▁▁▁▁▁▁▁▁▄▄▄▁▁▁
  DIG4  ▁▁▁▁▁▁▁▁▁▄▄▄▁▁▁▁▁▁▁▁▁▄▄▄
  SEGS  [c1][c2][c3][c4][c1][c2]...   ← le motif de segments change avec le chiffre actif
        |← un cycle de rafraîchissement →|  (ex. 4 × 4 ms = 16 ms → 62 Hz)
```

**Ghosting (images fantômes) :** si vous changez les segments *avant* de changer de chiffre (ou l'inverse), le chiffre suivant affiche brièvement le motif précédent : un faible chiffre « fantôme » apparaît. **Éteignez toujours d'abord** (tous les chiffres éteints, ou segments éteints), puis positionnez les nouveaux segments, puis activez le nouveau chiffre.

```c
static uint8_t g_digits[4];                          // motifs de segments à afficher

static void display_step(void)
{
    static uint8_t idx = 0;

    pca_write_reg(2, 0xFF);                          // 1. tous les chiffres éteints (sélection active à l'état bas ; à adapter !)
    pca_write_reg(3, g_digits[idx]);                 // 2. segments de ce chiffre
    pca_write_reg(2, (uint8_t)~(EXP_DIG1 << idx));   // 3. activer uniquement ce chiffre (suppose des bits consécutifs)
    idx = (idx + 1) & 3;
}

void display_number(uint16_t n)                      // 0..9999
{
    for (int8_t i = 3; i >= 0; i--) {
        g_digits[i] = SEG[n % 10];
        n /= 10;
    }
}
```

Appelez `display_step()` toutes les ~3–4 ms, indéfiniment. Attention au timing : chaque `pca_write_reg` représente ~4 octets en I2C ≈ 0,4 ms à 100 kHz, donc une étape prend ~1,2 ms. C'est acceptable, mais **envisagez 400 kHz** (TWBR = 12) pour avoir de la marge. Vous pouvez aussi regrouper les étapes en utilisant l'écriture de deux octets (commande 2 → port 0 puis port 1 en une seule transaction).

### 20.7 Problème d'architecture : le rafraîchissement + tout le reste

Le multiplexage ne doit **jamais s'arrêter**, sinon l'affichage scintille ou n'affiche qu'un seul chiffre. Mais vous devez aussi compter les secondes, lire des capteurs, etc. Options :

**Option A : boucle principale non bloquante (recommandée).**
```c
while (1)
{
    if (g_tick_4ms) { g_tick_4ms = 0; display_step(); }          // drapeau venant d'une ISR de timer
    if (g_tick_1s)  { g_tick_1s = 0; counter = (counter + 1) % 10000; display_number(counter); }
    // aucun appel bloquant nulle part !
}
```

**Option B : faire le rafraîchissement dans une ISR de timer.** C'est simple et robuste face à une boucle principale occupée. **Mais** si la boucle principale utilise *aussi* l'I2C (par ex. pour lire l'AHT20 ou SW3), une ISR qui démarre une transaction I2C au milieu d'une autre **corrompt les deux**. Il faut alors désactiver cette interruption de timer autour de chaque accès I2C dans main, ou déplacer tout le trafic I2C dans le même contexte.

**Règle : un bus partagé (I2C, SPI, émission UART) doit être accédé depuis un seul contexte à la fois.** Décidez sur papier quelle partie du code « possède » le bus.

### 20.8 Exercices typiques du Day 09

| Ex | Tâche | Concept clé |
|---|---|---|
| 00 | Faire clignoter D9 (sur l'extenseur) à 1 Hz | registres de config & de sortie du PCA9555 |
| 01 | SW3 (sur l'extenseur) incrémente un compteur 3 bits affiché sur D9–D11 | entrée de l'extenseur, détection de front |
| 02 | Afficher le chiffre « 2 » sur le chiffre le plus à droite | encodage 7 segments, sélection de chiffre |
| 03 | Afficher « 42 » sur les deux chiffres les plus à droite | multiplexage |
| 04 | Un compteur 0000 → 9999, +1 chaque seconde, qui reboucle | multiplexage + timing |
| 05+ | Afficher des valeurs de capteurs : potentiomètre, température, signe « - », animations… | intégration |

### 20.9 Pièges

- Polarité de configuration de l'extenseur (0 = sortie, contrairement au DDR de l'AVR).
- Oublier que le registre de sortie vaut **0xFF** par défaut, donc tout ce qui est actif à l'état bas est éteint au démarrage.
- Écrire une configuration qui transforme SW3 en sortie.
- Ghosting : segments modifiés pendant qu'un chiffre est actif.
- Rafraîchissement trop lent (scintillement visible) ou interrompu par un délai bloquant.
- I2C utilisé à la fois dans une ISR et dans main.
- Zéros de tête : décidez si « 0042 » ou « ␣␣42 » est demandé.


---

## 21. Rush 01 : Final Boss

### 21.1 À quoi s'attendre

Le rush final **rassemble tout**. Un sujet typique demande un programme avec **plusieurs modes** que l'utilisateur fait défiler avec les boutons (par ex. SW1 = mode suivant, SW2 = mode précédent), le numéro du mode courant étant affiché sur les LEDs. Chaque mode utilise un périphérique différent, et le résultat apparaît sur l'**afficheur 7 segments**, les LEDs RGB et/ou l'UART. Par exemple :

| Mode | Contenu probable |
|---|---|
| 0 | Valeur du potentiomètre (ADC, 10 bits) sur le 7 segments |
| 1 | Valeur de la LDR |
| 2 | Valeur de la CTN |
| 3 | Capteur de température interne |
| 4 | RGB animé (couleurs de D5 / des APA102 qui défilent) |
| 5–7 | Température AHT20 en °C, en °F, humidité en % |
| 8–10 | Horloge temps réel : heure HH:MM, date JJ/MM, année (réglées via l'UART) |
| … | Autre chose : une animation au démarrage, toutes les LEDs allumées quelques secondes au boot, etc. |

Le contenu exact sera différent. **Lisez le sujet extrêmement attentivement** : séquences de démarrage, formats d'affichage, ordre des modes et cas limites sont tous notés.

### 21.2 L'horloge temps réel (PCF8563)

S'il y a une RTC (Real-Time Clock, horloge temps réel) sur votre carte, c'est probablement un **NXP PCF8563** à l'adresse I2C **0x51**. Elle garde l'heure (avec son propre quartz de 32,768 kHz) et généralement une alimentation de secours.

Registres clés (lisez la datasheet !) :

| Adresse | Registre | Contenu (BCD) |
|---|---|---|
| 0x00 | Control_status_1 | |
| 0x01 | Control_status_2 | |
| 0x02 | VL_seconds | bit 7 = VL (voltage low, tension basse : l'heure n'est pas fiable), bits 6:0 secondes en BCD |
| 0x03 | Minutes | bits 6:0 BCD |
| 0x04 | Hours | bits 5:0 BCD (24 h) |
| 0x05 | Days | bits 5:0 BCD (1–31) |
| 0x06 | Weekdays | bits 2:0 (0–6) |
| 0x07 | Century_months | bit 7 = siècle, bits 4:0 mois en BCD |
| 0x08 | Years | BCD (00–99) |

**BCD (Binary-Coded Decimal, décimal codé binaire)** : chaque chiffre décimal est stocké dans un quartet. 45 est stocké sous la forme `0x45` (0100 0101), et non `0x2D`. C'est très pratique pour les afficheurs : chaque quartet est directement un chiffre.

```c
static inline uint8_t bcd_to_bin(uint8_t b) { return (b >> 4) * 10 + (b & 0x0F); }
static inline uint8_t bin_to_bcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }
```

Lire l'heure : positionner le pointeur de registre sur 0x02, START répété, lire 7 octets (auto-incrément), masquer les bits inutilisés (par ex. `sec & 0x7F`, `hour & 0x3F`). Régler l'heure : écrire le pointeur 0x02 suivi de 7 octets BCD.

Une commande UART pour régler la date/l'heure, par ex. `12/03/2025 14:30:00`, nécessite une analyse et une validation strictes (mois 1–12, jour valide pour ce mois, années bissextiles…).

### 21.3 Structurer un projet multi-fichiers

À cette échelle, un seul `main.c` devient ingérable. Découpez par module :

```
rush01/
├── Makefile
├── inc/
│   ├── uart.h
│   ├── i2c.h
│   ├── adc.h
│   ├── spi.h
│   ├── timer.h        (millis, ticks)
│   ├── display.h      (7 segments + multiplexage)
│   ├── expander.h
│   ├── aht20.h
│   ├── rtc.h
│   ├── rgb.h
│   └── modes.h
└── src/
    ├── main.c
    ├── uart.c
    ├── i2c.c
    ├── ...
    └── modes.c
```

Makefile pour plusieurs fichiers :

```makefile
SRC  = $(wildcard src/*.c)
OBJ  = $(SRC:src/%.c=obj/%.o)
CFLAGS += -Iinc -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections

$(NAME).bin: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@
```

Exemple d'en-tête (header) :

```c
// inc/uart.h
#ifndef UART_H
# define UART_H
# include <stdint.h>

void    uart_init(void);
void    uart_tx(char c);
void    uart_printstr(const char *s);
void    uart_putnbr(int32_t n);
void    uart_puthex8(uint8_t v);

#endif
```

Placez les globales `volatile` partagées dans **un seul** fichier `.c` et déclarez-les `extern volatile` dans l'en-tête.

### 21.4 Architecture du rush

```c
// main.c : esquisse
int main(void)
{
    hw_init();              // uart, i2c, adc, spi, timers, extenseur, rgb...
    startup_sequence();     // ex. toutes les LEDs allumées 3 s, puis éteintes 1 s
    sei();

    while (1)
    {
        buttons_update();                    // événements anti-rebondis à partir des ticks
        if (button_event(SW1)) mode_next();
        if (button_event(SW2)) mode_prev();

        if (tick_100ms()) mode_update();     // calculer la valeur du mode courant
        if (tick_4ms())   display_step();    // multiplexage, jamais bloqué
        uart_poll();                         // analyseur de commandes non bloquant
    }
}
```

**Table des modes avec pointeurs de fonction** (propre et extensible) :

```c
typedef struct {
    void (*enter)(void);        // appelée en entrant dans le mode (mise en place)
    void (*update)(void);       // appelée périodiquement
    void (*leave)(void);        // nettoyage (ex. éteindre le RGB)
} mode_t;

static const mode_t modes[] = {
    { mode_pot_enter,  mode_pot_update,  NULL },
    { mode_ldr_enter,  mode_ldr_update,  NULL },
    { mode_temp_enter, mode_temp_update, NULL },
    /* ... */
};
#define MODE_COUNT (sizeof(modes) / sizeof(modes[0]))

static uint8_t current = 0;

void mode_set(uint8_t m)
{
    if (modes[current].leave) modes[current].leave();
    current = m;
    show_mode_on_leds(current);
    if (modes[current].enter) modes[current].enter();
}
void mode_next(void) { mode_set((current + 1) % MODE_COUNT); }
void mode_prev(void) { mode_set((current + MODE_COUNT - 1) % MODE_COUNT); }
```

### 21.5 Plan d'attribution des ressources (faites-le d'abord sur papier !)

| Ressource | Utilisée par | Remarques |
|---|---|---|
| Timer0 | PWM RGB de D5 (OC0A/OC0B) | ne peut plus servir de tick à la ms |
| Timer2 | bleu RGB de D5 (OC2B), peut *aussi* fournir un tick via son interruption de débordement | TOV2 à 976 Hz avec /64 |
| Timer1 | tick système (interruption CTC à 1 ms ou 4 ms) | multiplexage, boutons, timing |
| I2C | extenseur (affichage), AHT20, RTC | boucle principale uniquement : un seul propriétaire |
| SPI | APA102 | PB2 doit être en sortie |
| ADC | potentiomètre, LDR, CTN, température interne | changement de référence : jeter le premier échantillon |
| UART | débogage + commandes | interruption RX + tampon circulaire |
| EEPROM | mode / réglages sauvegardés | avec octet magique |

Écrivez ce tableau **avant de coder**. La plupart des bugs de rush viennent de deux fonctionnalités qui se disputent le même timer, la même broche ou le même bus.

### 21.6 Capteurs lents sans bloquer

L'AHT20 a besoin de ~80 ms par mesure. Un `_delay_ms(80)` dans la boucle principale fige le multiplexage de l'afficheur, qui se met à scintiller. À la place, découpez l'opération en **états** :

```c
typedef enum { AHT_IDLE, AHT_WAITING } aht_state_t;
static aht_state_t aht_state = AHT_IDLE;
static uint32_t aht_t0;

void aht20_task(void)       // appelée à chaque itération de la boucle
{
    switch (aht_state)
    {
        case AHT_IDLE:
            aht20_trigger();                   // envoyer 0xAC 0x33 0x00
            aht_t0 = millis();
            aht_state = AHT_WAITING;
            break;
        case AHT_WAITING:
            if (millis() - aht_t0 >= 80) {
                aht20_read_result(&g_temp, &g_hum);
                aht_state = AHT_IDLE;          // ou attendre un peu avant la mesure suivante
            }
            break;
    }
}
```

Ce motif (**lancer → revenir plus tard → terminer**) est la clé du **multitâche coopératif** (cooperative multitasking) sans OS. Voir §22.

### 21.7 Checklist du rush

- [ ] Chaque mode fonctionne seul (testez-les un par un).
- [ ] Changer de mode réinitialise ce qui doit l'être (RGB éteint, afficheur effacé).
- [ ] L'afficheur ne scintille jamais, même en changeant de mode ou en lisant des capteurs.
- [ ] Les boutons sont anti-rebondis : un appui = exactement un changement de mode.
- [ ] Rien ne bloque plus de quelques ms.
- [ ] Séquence de démarrage exactement comme spécifiée.
- [ ] Valeurs formatées exactement comme demandé (unités, décimales, températures négatives, zéros de tête).
- [ ] Les saisies UART sont validées ; n'importe quoi ne fait rien planter.
- [ ] La RTC garde l'heure après un reset (et idéalement après une coupure de courant).
- [ ] Le code est découpé en modules et compile sans avertissement.
- [ ] `avr-size` : l'utilisation de la RAM laisse de la place pour la pile (visez < 75 % en data).

---

# PARTIE III : COMPÉTENCES TRANSVERSALES

---

## 22. Architecturer un firmware

### 22.1 La super-boucle (superloop)

L'architecture la plus simple, celle que vous utiliserez toute la piscine :

```c
int main(void)
{
    init();
    while (1)
    {
        task_a();
        task_b();
        task_c();
    }
}
```

Elle fonctionne bien si **chaque tâche rend la main rapidement** (non bloquante). Un seul `_delay_ms(500)` dans `task_b` retarde toutes les autres tâches de 500 ms.

### 22.2 Super-boucle + interruptions (avant-plan/arrière-plan, foreground/background)

- Les **interruptions** (l'avant-plan) traitent *rapidement* les événements critiques en temps : capturer un octet, lever un drapeau, incrémenter un tick.
- **La boucle principale** (l'arrière-plan) fait le vrai travail quand les drapeaux sont levés.

```
   ISR :   ▲       ▲   ▲            ▲         (courtes, fréquentes)
           │       │   │            │
   main : ═╪═══════╪═══╪════════════╪══════   (travail de longue haleine, interrompu à tout moment)
```

C'est le motif standard des firmwares bare-metal, et celui à utiliser dans les deux rushes.

### 22.3 Machines à états finis

Une FSM (Finite State Machine, machine à états finis) comprend :
- un ensemble d'**états** (enum)
- des **événements** (appui sur un bouton, délai écoulé, octet reçu)
- des **transitions** : (état, événement) → nouvel état + action

Deux façons classiques de les coder :

**À base de switch** (la plus lisible) :
```c
switch (state) {
    case IDLE:
        if (evt == EVT_BTN) { start_timer(); state = RUNNING; }
        break;
    case RUNNING:
        if (evt == EVT_TIMEOUT) { led_on(); state = DONE; }
        else if (evt == EVT_BTN) { state = IDLE; }
        break;
    ...
}
```

**À base de table** (compact pour les grosses FSM) :
```c
typedef struct { state_t from; event_t evt; state_t to; void (*action)(void); } transition_t;
static const transition_t table[] = {
    { IDLE,    EVT_BTN,     RUNNING, start_timer },
    { RUNNING, EVT_TIMEOUT, DONE,    led_on },
    { RUNNING, EVT_BTN,     IDLE,    NULL },
};
```

**Dessinez d'abord le diagramme.** C'est la meilleure documentation, et cela rend l'évaluation bien plus fluide.

Les machines à états sont partout en embarqué : analyseurs de protocoles (commandes UART, séquences d'échappement), anti-rebond des boutons (repos / peut-être appuyé / appuyé / peut-être relâché), pilotes de capteurs (déclencher / attendre / lire), jeux.

### 22.4 Ordonnanceur coopératif par tranches de temps

En généralisant l'idée des « drapeaux de tick » :

```c
typedef struct {
    void     (*fn)(void);
    uint16_t period_ms;
    uint32_t last;
} task_t;

static task_t tasks[] = {
    { display_step,  4,   0 },
    { buttons_scan,  5,   0 },
    { sensors_task,  100, 0 },
    { blink_status,  500, 0 },
};

void scheduler_run(void)
{
    uint32_t now = millis();
    for (uint8_t i = 0; i < sizeof(tasks) / sizeof(tasks[0]); i++)
        if (now - tasks[i].last >= tasks[i].period_ms) {
            tasks[i].last = now;
            tasks[i].fn();
        }
}

int main(void) { init(); sei(); while (1) scheduler_run(); }
```

Chaque tâche doit rendre la main rapidement : l'ordonnanceur (scheduler) est *coopératif*. Une tâche qui doit attendre se découpe elle-même en états (§21.6).

### 22.5 Files d'événements (event queues)

Au lieu de nombreux drapeaux individuels, les ISR peuvent déposer des **événements** dans un tampon circulaire (§12.11), que la boucle principale dépile et distribue. Les événements sont alors traités dans l'ordre et aucun n'est perdu. C'est ainsi que fonctionnent les frameworks d'interface graphique et beaucoup de conceptions à base de RTOS.

### 22.6 Pilotes en couches

Séparez le *quoi* du *comment* :

```
  application (modes, logique du jeu)
        │
  pilotes de composants   (aht20.c, pca9555.c, apa102.c, display.c)
        │
  pilotes de bus          (i2c.c, spi.c, uart.c, adc.c)
        │
  registres               (<avr/io.h>)
```

La logique du jeu ne touche jamais `TWCR`. Le pilote de l'AHT20 appelle les fonctions `i2c_*` et ne se soucie pas des registres TWI. Chaque couche devient ainsi testable et remplaçable (par ex. passer l'I2C à 400 kHz, ou changer de capteur).

### 22.7 Quand aurait-on besoin d'un RTOS ?

Un RTOS (Real-Time Operating System, système d'exploitation temps réel : FreeRTOS, Zephyr…) fournit du **multitâche préemptif** (preemptive multitasking) : chaque tâche a sa propre pile et l'ordonnanceur bascule entre elles, donc une tâche peut « bloquer » sans arrêter les autres. Cela coûte de la RAM (chaque pile prend des centaines d'octets) et de la complexité (mutex, inversion de priorité). Sur un ATmega328P de 2 Ko, c'est possible mais serré. Super-boucle + interruptions + machines à états est le bon outil ici. Sur des puces plus grosses (ESP32, STM32) avec des piles Wi-Fi/Bluetooth, les RTOS sont la norme.

---

## 23. Déboguer du code embarqué

Vous n'avez (généralement) pas de débogueur branché, pas d'écran et pas de `printf`. Déboguer du code embarqué est une compétence à part entière.

### 23.1 La méthode scientifique

1. **Observer** précisément : que se passe-t-il, et que devrait-il se passer ?
2. **Formuler une hypothèse** : qu'est-ce qui pourrait causer cela ?
3. **Tester** une hypothèse à la fois avec une expérience *minimale*.
4. **Réduire** : si un gros programme échoue, dépouillez-le jusqu'à ce que le bug disparaisse. Le dernier morceau retiré est le coupable.

### 23.2 Outils de débogage, du moins cher au plus puissant

**LEDs** : le `printf` de l'embarqué. « Si on arrive ici, allumer la LED D4. » Inversez une LED dans une ISR pour voir si elle s'exécute. Affichez un code d'état en binaire sur 4 LEDs.

**Affichages UART** : une fois le Day 02 fonctionnel, affichez les valeurs des registres (`uart_puthex8(TCCR1A)`), les codes d'état (`TWSR`), les variables et les transitions d'état. N'oubliez pas qu'afficher prend du temps (87 µs/caractère à 115200) et modifie le timing. Dans une ISR, stockez une valeur dans une variable et affichez-la depuis main.

**Vidages de registres** (register dumps) : quand un périphérique ne fonctionne pas, affichez tous ses registres et comparez-les bit par bit avec ce que la datasheet dit qu'ils devraient contenir. Cela attrape 80 % des bugs de configuration (un bit dans le mauvais registre, une activation manquante).

**Multimètre** (multimeter) : mesurez les tensions. La broche est-elle vraiment à 5 V ? L'alimentation est-elle correcte ? Une broche PWM affiche sa tension *moyenne* (50 % → ~2,5 V), ce qui est une vérification rapide de la PWM.

**Analyseur logique** (logic analyzer : clones USB 8 canaux bon marché, ~10 €, avec **PulseView/sigrok**) : enregistre des signaux numériques dans le temps et **décode les protocoles** (UART, I2C, SPI). Il montre exactement quels octets passent sur le fil, les ACK, le timing, etc. Inestimable pour les Days 06–09. Si votre cluster en a un, apprenez à vous en servir.

**Oscilloscope** : voit la forme analogique des signaux (temps de montée, bruit, niveaux de tension). Utile pour les problèmes de pull-up en I2C, la PWM et les entrées ADC.

**Simulateur** (simavr + gdb, Wokwi) : exécution pas à pas et observation des registres. Il ne simulera pas votre carte exacte, mais il est parfait pour les bugs de pure logique.

**Débogueur matériel** (hardware debugger : debugWIRE sur l'ATmega328P, avec un Atmel-ICE ou similaire ; nécessite de changer un fusible) : de vrais points d'arrêt (breakpoints). Rarement disponible pendant une piscine.

### 23.3 Symptôme → cause probable

| Symptôme | Causes probables |
|---|---|
| Rien ne se passe du tout | pas flashé (vérifiez la sortie d'avrdude), mauvaise broche, DDR non configuré, `main` a planté / redémarre en boucle |
| avrdude « not in sync » / « not responding » | terminal ouvert sur le port, mauvais port, mauvais débit (`-b`), problème de bootloader, câble (câble USB de charge uniquement !) |
| LED très faible | broche configurée en entrée avec pull-up (DDR non configuré), ou rapport cyclique PWM faible |
| Le programme semble redémarrer en boucle | interruption activée sans ISR, nom de vecteur mal orthographié, débordement de pile, watchdog activé |
| Caractères UART illisibles | F_CPU faux, désaccord de débit, incohérence U2X, réglages du terminal |
| L'UART n'affiche que le premier caractère, ou s'arrête | TX n'attend pas UDRE0, ISR bloquée, main bloqué |
| Délais 16× trop longs / trop courts | F_CPU faux / non défini (1 MHz par défaut dans util/delay.h), fusible CKDIV8 |
| `_delay_ms` très imprécis | compilé sans optimisation, argument non constant |
| Variable modifiée dans une ISR jamais vue dans main | `volatile` manquant |
| Valeurs fausses rares et aléatoires | accès multi-octets non atomique partagé avec une ISR, ordre de lecture de l'ADC |
| Le timer ne tourne pas | bits de sélection d'horloge à 0, mauvais registre pour les bits WGM |
| La broche PWM ne sort rien | DDR non configuré, bits COM faux, OCR = 0 ou ≥ TOP |
| L'I2C se bloque pour toujours | pas de pull-ups, bus bloqué, attente de TWINT après STOP, composant absent sans timeout |
| État I2C 0x20 / 0x48 | mauvaise adresse (décalage !), composant non alimenté/non connecté |
| Les transferts SPI s'arrêtent après l'init | broche SS en entrée → passage en mode esclave |
| Comportement étrange après ajout de code | débordement de RAM (vérifiez `avr-size`, gros buffers, chaînes littérales → PROGMEM) |
| Marche avec des affichages, échoue sans | bug dépendant du timing (condition de course, délai manquant après une commande à un composant) |

### 23.4 Lire le désassemblage

Quand vous soupçonnez le compilateur d'avoir « fait quelque chose de bizarre » :
```bash
avr-objdump -d -S main.bin | less
```
Cherchez votre fonction. Vous pouvez vérifier si une boucle a été supprimée (`volatile` manquant), si un accès 16 bits a été coupé en deux, quelle est la longueur du prologue d'une ISR, etc. Voir §25.

### 23.5 Habitudes défensives

- **Compilez avec `-Wall -Wextra -Werror`.** Beaucoup de bugs embarqués produisent des avertissements : noms d'ISR mal orthographiés, conversions implicites, résultats non utilisés.
- **Vérifiez les codes de retour** (état I2C).
- **Ajoutez des timeouts** à chaque attente `while (!flag)` dans du code de production.
- **Vérifiez (assert) les états impossibles** : un `default:` dans chaque switch, un motif de clignotement d'erreur.
- **Changez une seule chose à la fois**, et gardez les versions qui marchent (git commit quand quelque chose fonctionne).

---

## 24. Consommation, modes veille et watchdog

### 24.1 Modes veille (sleep modes)

Le CPU peut arrêter son horloge pour économiser de l'énergie, et une interruption le réveille.

| Mode | Ce qui continue de tourner | Sources de réveil | Usage typique |
|---|---|---|---|
| **Idle** (repos) | tous les périphériques (timers, UART, ADC…) | toute interruption | « rien à faire jusqu'au prochain tick » |
| ADC Noise Reduction | ADC, Timer2, correspondance d'adresse TWI, INTx | fin de conversion ADC, etc. | mesures ADC plus silencieuses |
| **Power-down** (arrêt) | presque rien (WDT, niveau INTx, correspondance d'adresse TWI) | INT0/1 (niveau), PCINT, WDT, TWI | appareils sur batterie qui dorment longtemps |
| Power-save | comme power-down, plus le Timer2 asynchrone | + Timer2 | RTC avec quartz 32 kHz |
| Standby / Extended standby | comme power-down mais l'oscillateur continue (réveil rapide) | | |

```c
#include <avr/sleep.h>

set_sleep_mode(SLEEP_MODE_PWR_DOWN);
sleep_enable();
sei();
sleep_cpu();        // zzz... jusqu'à une interruption
sleep_disable();
```

Autres économies : le **PRR** (Power Reduction Register, registre de réduction de consommation) coupe l'horloge des périphériques inutilisés (`PRR |= (1 << PRADC)` etc.), désactivez l'ADC (`ADCSRA &= ~(1 << ADEN)`), ne laissez pas d'entrées flottantes, et réduisez l'horloge.

Un ATmega328P à 16 MHz sous 5 V consomme ~10 mA en fonctionnement ; en power-down avec le WDT éteint, **< 1 µA**. Sur une carte avec une puce USB-série et une LED d'alimentation, cela compte peu, mais sur batterie c'est essentiel.

### 24.2 Le watchdog (chien de garde, WDT)

Le **watchdog** est un timer de sécurité doté de son propre oscillateur à 128 kHz. Si votre programme ne le « nourrit » pas (ne le réinitialise pas) régulièrement, il **redémarre la puce**. C'est la protection classique contre un programme coincé dans une boucle infinie (par ex. un bus I2C bloqué).

```c
#include <avr/wdt.h>

wdt_enable(WDTO_500MS);      // redémarrer si pas nourri dans les 500 ms
while (1) {
    do_work();
    wdt_reset();             // « je suis vivant »
}
```

Il peut aussi générer une **interruption** au lieu (ou avant) du redémarrage, ce qui est utile comme timer de réveil basse consommation depuis le power-down (mode interruption du WDT, périodes de 16 ms à 8 s).

⚠️ Après un redémarrage par le watchdog, le WDT **reste activé** avec le délai le plus court. Si votre code de démarrage ne le désactive pas tôt, la puce redémarre indéfiniment. Consultez `MCUSR` pour la cause du reset et désactivez le WDT tout au début de `main` (ou dans `.init3`). Certains anciens bootloaders gèrent mal ce cas.

### 24.3 Causes de reset

**MCUSR** vous dit pourquoi la puce a redémarré : `PORF` (mise sous tension, power-on), `EXTRF` (broche reset), `BORF` (brown-out : tension trop basse), `WDRF` (watchdog). Lisez-le au démarrage, puis effacez-le. C'est utile pour déboguer des redémarrages mystérieux.

### 24.4 Détection de sous-tension (brown-out detection, BOD)

Si VCC descend sous un seuil (réglé par les fusibles : 1,8, 2,7 ou 4,3 V), la puce est maintenue en reset. Cela évite un comportement erratique et la **corruption de l'EEPROM** à basse tension.

---

## 25. Un aperçu de l'assembleur AVR

Vous n'écrirez pas d'assembleur pendant la piscine, mais savoir en **lire** un peu aide à comprendre le timing, `volatile` et ce que fait le compilateur.

### 25.1 Instructions courantes

| Instruction | Signification | Cycles |
|---|---|---|
| `ldi r24, 0x05` | charger une valeur immédiate dans un registre (r16–r31 seulement) | 1 |
| `mov r24, r25` | copier un registre | 1 |
| `in r24, 0x05` | lire un registre d'E/S (adresse d'E/S) dans un registre | 1 |
| `out 0x05, r24` | écrire un registre dans un registre d'E/S | 1 |
| `sbi 0x05, 0` | mettre à 1 le bit 0 du registre d'E/S 0x05 (PORTB), atomique | 2 |
| `cbi 0x05, 0` | mettre un bit à 0 | 2 |
| `sbic` / `sbis` | sauter l'instruction suivante si le bit d'E/S est à 0 / à 1 | 1–3 |
| `lds r24, 0x0100` | charger depuis l'espace de données (RAM ou E/S étendues) | 2 |
| `sts 0x0100, r24` | stocker dans l'espace de données | 2 |
| `add`, `adc`, `sub`, `sbc`, `and`, `or`, `eor`, `com` | arithmétique/logique | 1 |
| `subi`, `sbci`, `andi`, `ori`, `cpi` | avec valeur immédiate | 1 |
| `adiw r24, 1` | ajouter une valeur immédiate à une paire de registres (16 bits) | 2 |
| `lsl`, `lsr`, `ror`, `rol`, `asr`, `swap` | décalages, rotations, échange de quartets | 1 |
| `mul` | multiplication 8×8 → 16 bits (résultat dans r1:r0) | 2 |
| `rjmp`, `jmp` | saut | 2, 3 |
| `rcall`, `call`, `ret` | appel/retour de fonction | 3/4, 4/5 |
| `reti` | retour d'interruption (remet I à 1) | 4/5 |
| `breq`, `brne`, `brcs`, `brlt`… | branchement conditionnel | 1 (non pris) / 2 (pris) |
| `push`, `pop` | pile | 2 |
| `sei`, `cli` | interruptions globales activées/désactivées | 1 |
| `nop` | ne rien faire | 1 |
| `sleep`, `wdr` | veille, reset du watchdog | 1 |
| `lpm r24, Z` | charger depuis la mémoire programme (flash) | 3 |

### 25.2 Un exemple

```c
DDRB |= (1 << PB0);
PORTB |= (1 << PB0);
```
est compilé en :
```asm
sbi 0x04, 0     ; DDRB  (adresse d'E/S 0x04) bit 0 = 1
sbi 0x05, 0     ; PORTB (adresse d'E/S 0x05) bit 0 = 1
```
Deux instructions, 4 cycles, 250 ns. Comparez avec le `digitalWrite` d'Arduino.

Alors que pour un registre d'E/S étendu :
```c
TIMSK1 |= (1 << OCIE1A);
```
```asm
lds r24, 0x006F   ; charger TIMSK1
ori r24, 0x02     ; mettre le bit 1 à 1
sts 0x006F, r24   ; réécrire   ← 3 instructions : PAS atomique
```

### 25.3 Convention d'appel (pour lire le désassemblage)

- Les arguments sont passés dans des registres, en commençant par r24/r25 (premier argument), r22/r23 (deuxième), et ainsi de suite en descendant.
- Valeurs de retour : r24 (8 bits), r24:r25 (16 bits), r22–r25 (32 bits).
- `r1` vaut toujours 0 (le compilateur en dépend), et `r0` est un registre temporaire.
- r18–r27, r30–r31 sont « écrasables par l'appelé » (call-clobbered : c'est l'appelant qui les sauvegarde) ; r2–r17, r28–r29 sont « préservés » (call-saved).

C'est pourquoi une ISR qui appelle une fonction doit sauvegarder de nombreux registres : le compilateur ne peut pas savoir lesquels la fonction appelée utilise. **Appeler des fonctions depuis une ISR la rend plus lente.**

### 25.4 Assembleur en ligne (inline assembly)

```c
__asm__ __volatile__ ("nop");           // délai d'un cycle
__asm__ __volatile__ ("sei" ::: "memory");
```

---

## 26. Aller plus loin après la piscine

### 26.1 Compétences à approfondir

- **Électronique** : construisez vos propres circuits sur une platine d'essai (breadboard) : transistors pour piloter des moteurs ou des relais, adaptation de niveaux entre 3,3 V et 5 V (level shifting), filtres. *The Art of Electronics* est la bible ; *Make: Electronics* est une introduction plus douce.
- **Autres protocoles** : 1-Wire (DS18B20), CAN (automobile), USB, RS-485/Modbus (industriel), WS2812 « NeoPixel » (protocole mono-fil critique en timing, un défi amusant sur AVR).
- **Autres microcontrôleurs** :
  - **ARM Cortex-M** (STM32 « Blue Pill »/Nucleo, RP2040 du Raspberry Pi Pico, nRF52) : 32 bits, beaucoup plus de RAM, DMA, contrôleur d'interruptions NVIC, arbres d'horloge (clock trees). Les mêmes concepts, avec des datasheets plus grosses (des « reference manuals » de plus de 1000 pages).
  - **ESP32** : Wi-Fi + Bluetooth, généralement avec FreeRTOS et ESP-IDF.
  - MCU **RISC-V** (le CH32V003 coûte ~10 centimes !).
- **RTOS** : FreeRTOS, Zephyr.
- **Linux embarqué** : Raspberry Pi, Yocto/Buildroot, pilotes de périphériques, device trees.
- **Rust embarqué** : `avr-hal`, `embassy` (async), `probe-rs`.
- **Tests** : tests unitaires des pilotes sur PC avec des registres simulés (mocks : Unity, CMock), tests matériels en boucle (hardware-in-the-loop).
- **Sécurité** : démarrage sécurisé (secure boot), injection de fautes (glitching) et canaux auxiliaires (side channels), un terrain de jeu de CTF.

### 26.2 Idées de projets avec la même carte ou un Arduino Uno

- Station météo : AHT20 + RTC + 7 segments + journalisation en EEPROM, extraite via l'UART.
- Jeu de réflexes/mémoire (Simon) avec du son sur un buzzer (Timer2 en CTC pour les notes).
- Thermostat numérique avec hystérésis et un voyant d'état RGB.
- Mini-oscilloscope : échantillonnage ADC rapide, données envoyées à un script Python qui les trace.
- Un shell en ligne de commande via l'UART (commandes pour lire/écrire des registres en direct : `poke 0x25 0x01`).
- Votre propre bootloader (avancé : instruction SPM, section de boot, fusibles).
- Un lecteur de musique : audio en PWM (échantillons à 8 kHz depuis la flash).


---

# ANNEXES

---

## Annexe A : Antisèche des registres

> L'ordre des bits est toujours **7 6 5 4 3 2 1 0**. Vérifiez toujours avec la datasheet.

### A.1 GPIO

| Registre | Bits | Remarques |
|---|---|---|
| `DDRB/C/D` | 1 = sortie | reset : 0 (tout en entrée) |
| `PORTB/C/D` | niveau de sortie / activation de la pull-up | reset : 0 |
| `PINB/C/D` | lire le niveau des broches ; écrire 1 = inverser le bit de PORT | |
| `MCUCR` | `PUD` (bit 4) désactive toutes les pull-ups | |

### A.2 Timer/Counter0 (8 bits), Timer/Counter2 (8 bits)

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `TCCR0A` | COM0A1 | COM0A0 | COM0B1 | COM0B0 | – | – | WGM01 | WGM00 |
| `TCCR0B` | FOC0A | FOC0B | – | – | WGM02 | CS02 | CS01 | CS00 |
| `TIMSK0` | – | – | – | – | – | OCIE0B | OCIE0A | TOIE0 |
| `TIFR0` | – | – | – | – | – | OCF0B | OCF0A | TOV0 |
| `TCCR2A` | COM2A1 | COM2A0 | COM2B1 | COM2B0 | – | – | WGM21 | WGM20 |
| `TCCR2B` | FOC2A | FOC2B | – | – | WGM22 | CS22 | CS21 | CS20 |
| `TIMSK2` | – | – | – | – | – | OCIE2B | OCIE2A | TOIE2 |

Données : `TCNT0`, `OCR0A`, `OCR0B` / `TCNT2`, `OCR2A`, `OCR2B`. Broches : OC0A PD6, OC0B PD5, OC2A PB3, OC2B PD3.

CS du Timer0 : 001 /1, 010 /8, 011 /64, 100 /256, 101 /1024.
CS du Timer2 : 001 /1, 010 /8, 011 /32, 100 /64, 101 /128, 110 /256, 111 /1024.

WGM (0/2) : 0 Normal, 1 PWM phase correct (TOP FF), 2 CTC (TOP OCRA), 3 Fast PWM (TOP FF), 5 PWM phase correct (TOP OCRA), 7 Fast PWM (TOP OCRA).

### A.3 Timer/Counter1 (16 bits)

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `TCCR1A` | COM1A1 | COM1A0 | COM1B1 | COM1B0 | – | – | WGM11 | WGM10 |
| `TCCR1B` | ICNC1 | ICES1 | – | WGM13 | WGM12 | CS12 | CS11 | CS10 |
| `TCCR1C` | FOC1A | FOC1B | – | – | – | – | – | – |
| `TIMSK1` | – | – | ICIE1 | – | – | OCIE1B | OCIE1A | TOIE1 |
| `TIFR1` | – | – | ICF1 | – | – | OCF1B | OCF1A | TOV1 |

Données : `TCNT1`, `OCR1A`, `OCR1B`, `ICR1` (16 bits). Broches : OC1A PB1, OC1B PB2, ICP1 PB0.
Modes clés : 4 CTC (TOP OCR1A), 12 CTC (TOP ICR1), 14 Fast PWM (TOP ICR1), 15 Fast PWM (TOP OCR1A), 5 Fast PWM 8 bits.

Recettes rapides :
```c
// CTC, inversion de OC1A, TOP=OCR1A, /256
TCCR1A = (1 << COM1A0);
TCCR1B = (1 << WGM12) | (1 << CS12);

// Fast PWM mode 14, OC1A non inversé, /256
TCCR1A = (1 << COM1A1) | (1 << WGM11);
TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
```

### A.4 Interruptions externes / de changement d'état de broche

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `EICRA` | – | – | – | – | ISC11 | ISC10 | ISC01 | ISC00 |
| `EIMSK` | – | – | – | – | – | – | INT1 | INT0 |
| `EIFR` | – | – | – | – | – | – | INTF1 | INTF0 |
| `PCICR` | – | – | – | – | – | PCIE2 | PCIE1 | PCIE0 |
| `PCIFR` | – | – | – | – | – | PCIF2 | PCIF1 | PCIF0 |
| `PCMSK0` | PCINT7 … PCINT0 (PB7..PB0) | | | | | | | |
| `PCMSK1` | – PCINT14 … PCINT8 (PC6..PC0) | | | | | | | |
| `PCMSK2` | PCINT23 … PCINT16 (PD7..PD0) | | | | | | | |

ISC : 00 niveau bas, 01 tout changement, 10 front descendant, 11 front montant. INT0 = PD2, INT1 = PD3.

### A.5 USART0

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `UCSR0A` | RXC0 | TXC0 | UDRE0 | FE0 | DOR0 | UPE0 | U2X0 | MPCM0 |
| `UCSR0B` | RXCIE0 | TXCIE0 | UDRIE0 | RXEN0 | TXEN0 | UCSZ02 | RXB80 | TXB80 |
| `UCSR0C` | UMSEL01 | UMSEL00 | UPM01 | UPM00 | USBS0 | UCSZ01 | UCSZ00 | UCPOL0 |

`UDR0` données, `UBRR0` (UBRR0H:UBRR0L) diviseur de baud rate. Vecteurs : `USART_RX_vect`, `USART_UDRE_vect`, `USART_TX_vect`.
16 MHz, 115200 : U2X0=1 → UBRR=16 (+2,1 %) ; U2X0=0 → UBRR=8 (−3,5 %). 9600 : U2X0=0 → UBRR=103 (0,2 %).

### A.6 ADC

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `ADMUX` | REFS1 | REFS0 | ADLAR | – | MUX3 | MUX2 | MUX1 | MUX0 |
| `ADCSRA` | ADEN | ADSC | ADATE | ADIF | ADIE | ADPS2 | ADPS1 | ADPS0 |
| `ADCSRB` | – | ACME | – | – | – | ADTS2 | ADTS1 | ADTS0 |
| `DIDR0` | – | – | ADC5D | ADC4D | ADC3D | ADC2D | ADC1D | ADC0D |

REFS : 01 AVCC, 11 1,1 V interne. MUX 0–5 = ADC0–5 (PC0–PC5), 8 = température, 14 = bandgap 1,1 V, 15 = GND.
Prescaler 111 = /128 → 125 kHz à 16 MHz. Résultat dans `ADC` (10 bits) ou `ADCH` avec ADLAR = 1 (8 bits).

### A.7 TWI (I2C)

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `TWCR` | TWINT | TWEA | TWSTA | TWSTO | TWWC | TWEN | – | TWIE |
| `TWSR` | TWS7 | TWS6 | TWS5 | TWS4 | TWS3 | – | TWPS1 | TWPS0 |
| `TWAR` | TWA6 … TWA0 | | | | | | | TWGCE |

`TWBR` débit, `TWDR` données. f_SCL = F_CPU / (16 + 2·TWBR·prescaler) → 100 kHz : TWBR = 72 ; 400 kHz : TWBR = 12.
SDA = PC4, SCL = PC5. État = `TWSR & 0xF8`. Vecteur : `TWI_vect`.

Recettes rapides :
```c
TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);   // START
TWCR = (1<<TWINT)|(1<<TWEN);              // envoyer TWDR / recevoir + NACK
TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);    // recevoir + ACK
TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);   // STOP
```

Codes d'état : 08 START, 10 START répété, 18 SLA+W ACK, 20 SLA+W NACK, 28 donnée ACK, 30 donnée NACK, 38 arbitrage perdu, 40 SLA+R ACK, 48 SLA+R NACK, 50 donnée reçue ACK, 58 donnée reçue NACK. Esclave : 60 notre SLA+W, 80 donnée reçue, A0 STOP, A8 notre SLA+R, B8 donnée émise ACK, C0 donnée émise NACK.

### A.8 SPI

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `SPCR` | SPIE | SPE | DORD | MSTR | CPOL | CPHA | SPR1 | SPR0 |
| `SPSR` | SPIF | WCOL | – | – | – | – | – | SPI2X |

`SPDR` données. Broches : SS PB2, MOSI PB3, MISO PB4, SCK PB5. Maître : PB2, PB3, PB5 en sortie.
Horloge : SPR 00 /4, 01 /16, 10 /64, 11 /128 ; avec SPI2X : /2, /8, /32, /64.

### A.9 EEPROM

| Registre | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|
| `EECR` | – | – | EEPM1 | EEPM0 | EERIE | EEMPE | EEPE | EERE |

`EEAR` (0–1023), `EEDR`. Écriture : attendre EEPE=0 → EEAR, EEDR → mettre EEMPE à 1 → mettre EEPE à 1 (dans les 4 cycles, interruptions désactivées). Lecture : attendre EEPE=0 → EEAR → mettre EERE à 1 → lire EEDR.

### A.10 Divers

| Registre | Rôle |
|---|---|
| `SREG` | bit 7 = I (activation globale des interruptions) |
| `MCUSR` | cause du reset : WDRF, BORF, EXTRF, PORF |
| `WDTCSR` | contrôle du watchdog |
| `SMCR` | contrôle du mode veille (SM2:0, SE) |
| `PRR` | réduction de consommation : PRTWI, PRTIM2, PRTIM0, PRTIM1, PRSPI, PRUSART0, PRADC |
| `CLKPR` | prescaler de l'horloge système (n'y touchez pas sans savoir pourquoi) |

---

## Annexe B : Formulaire

| Quoi | Formule |
|---|---|
| Loi d'Ohm | U = R × I |
| Puissance | P = U × I |
| Pont diviseur de tension | Vout = Vin × R2 / (R1 + R2) |
| Résistance de LED | R = (VCC − Vf) / I_led |
| Période d'horloge | T = 1 / F_CPU (16 MHz → 62,5 ns) |
| Itérations d'une boucle d'attente | N = (délai × F_CPU) / cycles_par_itération |
| Tick de timer | t_tick = prescaler / F_CPU |
| Période de débordement d'un timer | T = prescaler × 2^bits / F_CPU |
| **Période d'égalité en CTC** | T = prescaler × (1 + OCR) / F_CPU → OCR = T × F_CPU / prescaler − 1 |
| **Fréquence d'inversion en CTC** | f = F_CPU / (2 × prescaler × (1 + OCR)) |
| **Fréquence Fast PWM** | f = F_CPU / (prescaler × (1 + TOP)) |
| **Rapport cyclique Fast PWM** | duty = (OCR + 1) / (TOP + 1) |
| Fréquence PWM phase correct | f = F_CPU / (2 × prescaler × TOP) |
| **UBRR de l'UART (normal)** | UBRR = F_CPU / (16 × baud) − 1 (arrondi) |
| **UBRR de l'UART (U2X)** | UBRR = F_CPU / (8 × baud) − 1 (arrondi) |
| Erreur de baud UART | erreur = (réel / voulu − 1) × 100 % |
| Durée d'un octet UART | 10 / baud (8N1) → 86,8 µs à 115200 |
| **Valeur ADC** | ADC = Vin × 1024 / VREF |
| **ADC → tension** | Vin = ADC × VREF / 1024 |
| Temps de conversion ADC | 13 / f_ADC (25 pour la première) → 104 µs à 125 kHz |
| Résistance CTN (CTN côté GND) | R_ctn = R_fixe × ADC / (1023 − ADC) |
| Équation Bêta | 1/T = 1/T₀ + ln(R/R₀) / B (Kelvin) |
| **Fréquence SCL I2C** | f = F_CPU / (16 + 2 × TWBR × prescaler) |
| Octet d'adresse I2C | (addr7 << 1) \| R/W |
| **Horloge SPI** | F_CPU / diviseur (2…128) |
| Humidité AHT20 | HR% = raw / 2²⁰ × 100 |
| Température AHT20 | T°C = raw / 2²⁰ × 200 − 50 |
| °C → °F | F = C × 9/5 + 32 |
| BCD → binaire | (b >> 4) × 10 + (b & 0x0F) |
| Division entière arrondie | (a + b/2) / b |

---

## Annexe C : Table ASCII & séquences d'échappement du terminal

### C.1 L'essentiel de l'ASCII

| Car. | Déc | Hexa | | Car. | Déc | Hexa | | Car. | Déc | Hexa |
|---|---|---|---|---|---|---|---|---|---|---|
| NUL `\0` | 0 | 00 | | `0` | 48 | 30 | | `A` | 65 | 41 |
| BS `\b` | 8 | 08 | | `1` | 49 | 31 | | `Z` | 90 | 5A |
| TAB `\t` | 9 | 09 | | `9` | 57 | 39 | | `a` | 97 | 61 |
| LF `\n` | 10 | 0A | | `:` | 58 | 3A | | `z` | 122 | 7A |
| CR `\r` | 13 | 0D | | `#` | 35 | 23 | | `~` | 126 | 7E |
| ESC | 27 | 1B | | espace | 32 | 20 | | DEL | 127 | 7F |
| Ctrl-C | 3 | 03 | | `"` | 34 | 22 | | `[` | 91 | 5B |

Astuces :
- chiffre → valeur : `c - '0'` ; valeur → chiffre : `'0' + v`
- minuscule ↔ majuscule : `c ^ 0x20` (lettres seulement), ou ±32
- ASCII imprimable : `0x20 <= c <= 0x7E`
- Ctrl-X envoie X − 64 (Ctrl-C = 3, Ctrl-D = 4)

### C.2 Touches telles qu'envoyées par un terminal

| Touche | Octets |
|---|---|
| Entrée | `\r` (0x0D), parfois `\r\n` |
| Retour arrière | 0x7F (DEL) ou 0x08 (BS) |
| ↑ ↓ → ← | `ESC [ A`, `ESC [ B`, `ESC [ C`, `ESC [ D` |
| Tab | 0x09 |

### C.3 Codes d'échappement ANSI

| Code | Effet |
|---|---|
| `\x1b[2J` | effacer l'écran |
| `\x1b[H` | curseur en haut à gauche |
| `\x1b[K` | effacer jusqu'à la fin de la ligne |
| `\x1b[2K` | effacer toute la ligne |
| `\x1b[<n>A/B/C/D` | déplacer le curseur de n vers le haut/bas/droite/gauche |
| `\x1b[<l>;<c>H` | placer le curseur ligne l, colonne c |
| `\x1b[0m` | réinitialiser le style |
| `\x1b[1m` | gras |
| `\x1b[4m` | souligné |
| `\x1b[7m` | vidéo inversée |
| `\x1b[30m`–`\x1b[37m` | couleur du texte : noir, rouge, vert, jaune, bleu, magenta, cyan, blanc |
| `\x1b[40m`–`\x1b[47m` | couleur de fond |
| `\x1b[38;2;R;G;Bm` | couleur de texte 24 bits (terminaux modernes) |

---

## Annexe D : Extraits de code réutilisables

Un « kit de démarrage » compact à adapter (vérifiez les règles du sujet sur ce que vous avez le droit de réutiliser !).

### D.1 `uart.c`

```c
#include <avr/io.h>
#include <stdint.h>

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200UL
#endif

void uart_init(void)
{
    uint16_t ubrr = (F_CPU + 4UL * UART_BAUDRATE) / (8UL * UART_BAUDRATE) - 1;
    UBRR0H = ubrr >> 8;
    UBRR0L = ubrr & 0xFF;
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx(char c)
{
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = c;
}

char uart_rx(void)
{
    while (!(UCSR0A & (1 << RXC0))) { }
    return UDR0;
}

uint8_t uart_available(void) { return (UCSR0A & (1 << RXC0)) != 0; }

void uart_printstr(const char *s) { while (*s) uart_tx(*s++); }

void uart_puthex8(uint8_t v)
{
    static const char hex[] = "0123456789ABCDEF";
    uart_tx(hex[v >> 4]);
    uart_tx(hex[v & 0xF]);
}

void uart_putnbr(int32_t n)
{
    char buf[11];
    uint8_t i = 0;
    uint32_t u = (n < 0) ? (uart_tx('-'), -(uint32_t)n) : (uint32_t)n;
    do { buf[i++] = '0' + u % 10; u /= 10; } while (u);
    while (i) uart_tx(buf[--i]);
}
```

### D.2 `timer.c` : tick de 1 ms avec le Timer1

```c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

static volatile uint32_t g_ms;

ISR(TIMER1_COMPA_vect) { g_ms++; }

void tick_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);   // CTC, /64 → 4 µs par tick
    OCR1A  = 249;                                        // 250 ticks = 1 ms
    TIMSK1 = (1 << OCIE1A);
}

uint32_t millis(void)
{
    uint32_t m;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { m = g_ms; }
    return m;
}
```

### D.3 Bouton anti-rebondi (scrutation, non bloquant)

```c
typedef struct {
    volatile uint8_t *pin;   // &PIND
    uint8_t  bit;            // PD2
    uint8_t  stable;         // 1 = relâché (actif à l'état bas)
    uint8_t  last_raw;
    uint32_t t_change;
} button_t;

// Renvoie 1 exactement une fois par appui.
uint8_t button_pressed(button_t *b)
{
    uint8_t raw = (*b->pin >> b->bit) & 1;
    uint32_t now = millis();

    if (raw != b->last_raw) {            // l'entrée a bougé : relancer le chronomètre de stabilité
        b->last_raw = raw;
        b->t_change = now;
    }
    if (now - b->t_change >= 20 && raw != b->stable) {   // stable depuis 20 ms et différent
        b->stable = raw;
        return raw == 0;                 // ne signaler que les appuis
    }
    return 0;
}

// utilisation :
// static button_t sw1 = { &PIND, PD2, 1, 1, 0 };
// if (button_pressed(&sw1)) ...
```

### D.4 Tampon circulaire (générique, octets)

```c
#define RB_SIZE 32                          // puissance de deux
typedef struct {
    volatile uint8_t buf[RB_SIZE];
    volatile uint8_t head, tail;
} ringbuf_t;

static inline uint8_t rb_push(ringbuf_t *r, uint8_t c)
{
    uint8_t next = (r->head + 1) & (RB_SIZE - 1);
    if (next == r->tail) return 0;          // plein
    r->buf[r->head] = c;
    r->head = next;
    return 1;
}

static inline int16_t rb_pop(ringbuf_t *r)
{
    if (r->head == r->tail) return -1;      // vide
    uint8_t c = r->buf[r->tail];
    r->tail = (r->tail + 1) & (RB_SIZE - 1);
    return c;
}

// Interruption RX alimentant un tampon circulaire :
static ringbuf_t rx;
ISR(USART_RX_vect) { rb_push(&rx, UDR0); }
// main : int16_t c = rb_pop(&rx); if (c >= 0) handle(c);
```

### D.5 Lecteur de ligne non bloquant (pour analyser des commandes pendant que d'autres choses tournent)

```c
#define LINE_MAX 32
static char    line[LINE_MAX];
static uint8_t line_len;

// À appeler souvent. Renvoie 1 quand une ligne complète est disponible dans `line`.
uint8_t line_poll(void)
{
    int16_t c;
    while ((c = rb_pop(&rx)) >= 0)
    {
        if (c == '\r' || c == '\n') {
            if (line_len == 0) continue;     // ignorer les lignes vides / le \n d'un \r\n
            line[line_len] = '\0';
            line_len = 0;
            uart_printstr("\r\n");
            return 1;
        }
        if ((c == 0x7F || c == 0x08) && line_len) { line_len--; uart_printstr("\b \b"); continue; }
        if (c >= 0x20 && c <= 0x7E && line_len < LINE_MAX - 1) {
            line[line_len++] = c;
            uart_tx(c);
        }
    }
    return 0;
}
```

### D.6 Mini-utilitaires de chaînes (si `<string.h>` est interdit)

```c
uint8_t ft_strlen(const char *s) { uint8_t n = 0; while (s[n]) n++; return n; }

int8_t ft_strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return (uint8_t)*a - (uint8_t)*b;
}

int8_t ft_strncmp(const char *a, const char *b, uint8_t n)
{
    while (n && *a && *a == *b) { a++; b++; n--; }
    return n ? (uint8_t)*a - (uint8_t)*b : 0;
}

// analyse un décimal non signé ; renvoie -1 en cas d'erreur
int32_t ft_atou(const char *s)
{
    int32_t v = 0;
    if (!*s) return -1;
    while (*s) {
        if (*s < '0' || *s > '9') return -1;
        v = v * 10 + (*s++ - '0');
        if (v > 100000) return -1;          // garde-fou arbitraire contre le débordement
    }
    return v;
}
```

---

## Annexe E : Checklist des pièges

Imprimez-la. Relisez-la quand vous êtes bloqué.

**Compilation & flashage**
- [ ] `F_CPU` défini (16000000UL) *avant* `<util/delay.h>`, et cohérent partout
- [ ] Optimisation `-Os` activée
- [ ] Recettes du Makefile indentées avec des TABULATIONS
- [ ] Terminal série fermé avant de flasher
- [ ] Bon port (`/dev/ttyUSB0` ?) et bon débit du programmateur

**C**
- [ ] `volatile` sur chaque variable partagée avec une ISR
- [ ] Accès atomique pour les variables partagées multi-octets
- [ ] Types à taille fixe ; rappel : `int` = 16 bits
- [ ] Suffixes sur les grandes constantes (`UL`), casts avant les grands décalages
- [ ] Parenthèses autour de `&` dans les comparaisons : `(x & m) == 0`
- [ ] `main` ne retourne jamais
- [ ] Utilisation de la RAM vérifiée avec `avr-size`

**GPIO**
- [ ] DDR configuré pour les sorties
- [ ] Pull-ups activées pour les boutons (ou présence de pull-ups externes)
- [ ] Logique active à l'état bas gérée (boutons, peut-être LEDs)
- [ ] Inversion avec `PINx = mask`, pas `PINx |= mask`
- [ ] Anti-rebond

**Timers**
- [ ] Bits WGM correctement répartis entre TCCRnA et TCCRnB
- [ ] Sélection d'horloge réglée (timer démarré)
- [ ] Broche OCnx en sortie
- [ ] Table de prescaler du bon timer (le Timer2 est différent)
- [ ] `− 1` dans les calculs de TOP/OCR
- [ ] Drapeaux effacés en écrivant 1

**Interruptions**
- [ ] `sei()` appelé
- [ ] Nom de vecteur bien orthographié (avertissements de compilation !)
- [ ] Chaque interruption activée a une ISR
- [ ] ISR courtes, sans délais
- [ ] Drapeaux périmés effacés avant activation

**UART**
- [ ] Baud rate & U2X cohérents ; terminal à la même vitesse
- [ ] `\r\n` pour les retours à la ligne
- [ ] Entrée = `\r` ; retour arrière = 0x7F/0x08
- [ ] Saisies validées

**ADC**
- [ ] Bits de référence réglés (REFS0 pour AVCC)
- [ ] Prescaler /128
- [ ] Attendre qu'ADSC repasse à 0
- [ ] ADLAR + ADCH pour 8 bits, `ADC` pour 10 bits

**I2C**
- [ ] Adresse décalée une fois, bit R/W correct
- [ ] `TWCR =` et non `|=`
- [ ] `TWSR & 0xF8`
- [ ] Dernier octet lu acquitté par un NACK
- [ ] Pull-ups présentes
- [ ] Bus utilisé depuis un seul contexte

**SPI**
- [ ] SS (PB2) en sortie en mode maître
- [ ] Bon mode (CPOL/CPHA)
- [ ] APA102 : trame de start, `0xE0 | luminosité`, ordre B-G-R, trame de fin

**EEPROM**
- [ ] EEMPE puis EEPE dans les 4 cycles, interruptions désactivées
- [ ] Effacé = 0xFF
- [ ] Mettre à jour (update), ne pas réécrire des valeurs identiques
- [ ] Nombre magique écrit en dernier

---

## Annexe F : Glossaire

| Terme | Définition |
|---|---|
| **ACK / NACK** | Acknowledge / Not Acknowledge (acquittement / non-acquittement) : bit I2C confirmant (ou refusant) la réception d'un octet |
| **Actif à l'état bas** (active-low) | Signal « actif » quand il est à 0 V (ex. boutons avec pull-ups, SS, RESET) |
| **ADC** (CAN) | Analog-to-Digital Converter, convertisseur analogique-numérique : mesure une tension sous forme de nombre |
| **Adresse (I2C)** | Identifiant 7 bits d'un esclave sur le bus |
| **Arbitrage** (arbitration) | Mécanisme I2C qui décide quel maître l'emporte quand deux démarrent en même temps |
| **Atomique** (atomic) | Opération qui ne peut pas être interrompue en cours de route |
| **AVR** | Famille de microcontrôleurs RISC 8 bits d'Atmel (ATmega, ATtiny) |
| **avrdude** | Programme qui téléverse un firmware dans les puces AVR |
| **avr-gcc** | Compilateur croisé GCC ciblant l'AVR |
| **avr-libc** | La bibliothèque C pour AVR (en-têtes comme `<avr/io.h>`) |
| **Bare-metal** | Code s'exécutant directement sur le matériel, sans OS |
| **Baud rate** (débit en bauds) | Nombre de symboles par seconde sur une ligne série (bits/s pour l'UART) |
| **BCD** | Binary-Coded Decimal, décimal codé binaire : un chiffre décimal par quartet |
| **Bootloader** (chargeur d'amorçage) | Petit programme au démarrage qui peut recevoir un nouveau firmware via la liaison série |
| **BOD** | Brown-Out Detection, détection de sous-tension : redémarre la puce quand la tension est trop basse |
| **Bus** | Ensemble de fils partagés reliant plusieurs composants |
| **Chronogramme** (timing diagram) | Dessin de signaux en fonction du temps |
| **Clock stretching** (étirement d'horloge) | Esclave I2C maintenant SCL basse pour demander au maître d'attendre |
| **Anode / cathode commune** (common anode / cathode) | Boîtiers multi-LED partageant la broche + ou − |
| **Compilateur croisé** (cross-compiler) | Compilateur produisant du code pour une autre architecture que celle sur laquelle il tourne |
| **CPOL / CPHA** | Polarité / phase de l'horloge SPI (le « mode » SPI) |
| **CTC** | Clear Timer on Compare match : mode de timer où le compteur repart à zéro à OCRnA/ICR1 |
| **Datasheet** (fiche technique) | Référence technique d'un composant par son fabricant |
| **DDR** | Data Direction Register : configure les broches en entrée ou en sortie |
| **Anti-rebond** (debouncing) | Filtrage des rebonds d'un interrupteur |
| **Drain ouvert** (open-drain) | Sortie qui ne peut que tirer vers le bas ; nécessite une pull-up |
| **EEPROM** | Mémoire non volatile, effaçable par octet, pour des données persistantes |
| **ELF** | Format d'exécutable produit par l'éditeur de liens |
| **F_CPU** | Macro contenant la fréquence d'horloge du CPU en Hz |
| **Firmware** (micrologiciel) | Logiciel s'exécutant sur un microcontrôleur |
| **Flash** | Mémoire non volatile contenant le programme |
| **Flottante** (floating) | Entrée non reliée à une tension définie (lit des valeurs aléatoires) |
| **Front** (edge) | Transition d'un signal : montant (rising, 0→1) ou descendant (falling, 1→0) |
| **FSM** | Finite State Machine, machine à états finis |
| **Full / half duplex** | Les deux sens simultanément / un seul sens à la fois |
| **Fusibles** (fuses) | Bits de configuration de l'AVR (source d'horloge, bootloader, BOD…) |
| **GND** (masse) | La référence 0 V |
| **GPIO** | General Purpose Input/Output, broche d'entrée/sortie à usage général |
| **Architecture Harvard** | Mémoires/bus séparés pour le programme et les données |
| **Fichier hex** | Format texte Intel HEX décrivant le contenu mémoire à programmer |
| **Hi-Z** (haute impédance) | Broche qui ne pilote ni haut ni bas |
| **I2C / TWI** | Bus série à deux fils, adressé, multi-composants (SDA, SCL) |
| **Interruption** (interrupt) | Signal matériel faisant exécuter une ISR au CPU |
| **ISP** | In-System Programming, programmation via les broches SPI avec un programmateur matériel |
| **ISR** | Interrupt Service Routine, routine de service d'interruption : fonction exécutée quand une interruption survient |
| **LSB / MSB** | Least / Most Significant Bit (ou Byte) : bit (ou octet) de poids faible / fort |
| **Masque** (bit mask) | Valeur utilisée avec `&`, `\|`, `^` pour sélectionner des bits |
| **MCU** | Microcontroller Unit, microcontrôleur |
| **E/S mappées en mémoire** (memory-mapped I/O) | Périphériques contrôlés via des adresses mémoire (registres) |
| **MISO / MOSI / SCK / SS** | Signaux SPI |
| **Multiplexage** (multiplexing) | Partage de lignes entre plusieurs sorties en alternant rapidement |
| **Nombre magique** (magic number) | Constante connue marquant des données stockées valides |
| **Octet** (byte) | 8 bits |
| **PCINT** | Pin Change Interrupt, interruption de changement d'état de broche |
| **Périphérique** (peripheral) | Bloc matériel dans le MCU (timer, UART, ADC…) |
| **Pont diviseur** (voltage divider) | Deux résistances en série produisant une tension intermédiaire |
| **Prescaler** (prédiviseur) | Diviseur d'horloge en amont d'un timer, de l'ADC, etc. |
| **PROGMEM** | Attribut plaçant des données constantes uniquement en flash |
| **Pull-up / pull-down** (résistance de tirage) | Résistance donnant à une ligne un niveau haut/bas par défaut |
| **PWM** (MLI) | Pulse Width Modulation, modulation de largeur d'impulsion : signal carré à rapport cyclique variable |
| **Quartet** (nibble) | 4 bits |
| **Rapport cyclique** (duty cycle) | Fraction du temps pendant laquelle un signal PWM est à l'état HAUT |
| **Rebond** (bounce) | Transitions rapides marche/arrêt d'un contact mécanique |
| **Condition de course** (race condition) | Bug qui dépend du timing relatif de code concurrent (main vs ISR) |
| **Registre** (register) | Emplacement mémoire spécial contrôlant le matériel (ou registre de travail du CPU) |
| **START répété** (repeated START) | START I2C sans STOP préalable |
| **Tampon circulaire** (ring buffer) | File FIFO circulaire |
| **RTC** | Real-Time Clock, horloge temps réel : puce qui garde la date et l'heure |
| **RTOS** | Real-Time Operating System, système d'exploitation temps réel |
| **SAR** | Successive Approximation Register, registre à approximations successives (architecture d'ADC) |
| **Schéma électrique** (schematic) | Diagramme des connexions d'un circuit |
| **Scrutation** (polling) | Vérifier un état en boucle au lieu d'utiliser des interruptions |
| **Débordement** (overflow) | Compteur/variable qui dépasse son maximum et reboucle |
| **SPI** | Bus série synchrone full duplex à 4 fils |
| **SRAM** | RAM statique, mémoire de travail volatile |
| **SREG** | Registre d'état de l'AVR (contient le bit I) |
| **Super-boucle** (superloop) | Architecture à boucle principale `while (1)` |
| **Synchrone / asynchrone** | Avec / sans ligne d'horloge partagée |
| **TOP / BOTTOM / MAX** | Limites du compteur d'un timer |
| **UART / USART** | (Universal Synchronous/) Asynchronous Receiver Transmitter, émetteur-récepteur (a)synchrone universel |
| **VCC** | Tension d'alimentation (5 V sur votre carte) |
| **Volatile** | Mot-clé C forçant chaque accès à avoir réellement lieu |
| **Watchdog** (chien de garde) | Timer qui redémarre le MCU s'il n'est pas rafraîchi |

---

## Annexe G : Ressources

**Documents officiels** (les vraies références, en anglais)
- Datasheet de l'ATmega328P (Microchip) : cherchez « ATmega328P datasheet » sur microchip.com
- AVR Instruction Set Manual (Microchip), le manuel du jeu d'instructions
- Manuel d'avr-libc : <https://avrdudes.github.io/avr-libc/> (documentation en ligne de `<avr/io.h>`, `<avr/interrupt.h>`, `<util/delay.h>`, `<avr/eeprom.h>`, `<avr/pgmspace.h>`, `<util/atomic.h>`)
- Votre fichier d'en-tête : `/usr/lib/avr/include/avr/iom328p.h` (tous les noms de registres et de bits)
- Datasheets des composants : AHT20 (Aosong), PCA9555 (NXP / TI), APA102 (APA Electronic / Shiji Lighting), PCF8563 (NXP)
- Spécification I2C : NXP UM10204 « I2C-bus specification and user manual »
- Le **schéma électrique** de votre devboard (fourni par la piscine)

**Tutoriels & lectures**
- Le forum *AVR Freaks* et sa section de tutoriels (timers, interruptions ; ancien mais précieux)
- Les pages de Nick Gammon sur les interruptions, timers, I2C, SPI et la consommation des AVR (gammon.com.au/forum) : extrêmement détaillées
- *Make: AVR Programming* d'Elliot Williams (O'Reilly) : exactement l'approche AVR bare-metal de la piscine
- *Making Embedded Systems* d'Elecia White : architecture, machines à états, débogage
- Les guides d'apprentissage de SparkFun & Adafruit (I2C, SPI, série, résistances de pull-up, PWM, niveaux logiques)
- Les vidéos de Ben Eater (YouTube) : le fonctionnement des ordinateurs et des protocoles série au niveau électrique

**Outils**
- Wokwi (wokwi.com) : simulateur en ligne d'Arduino Uno/ATmega328P, accepte du C bare-metal
- simavr + GTKWave : simulation locale et visualisation de chronogrammes
- PulseView / sigrok : logiciel d'analyseur logique avec décodeurs de protocoles
- screen / picocom / minicom : terminaux série

---

## Annexe H : Se préparer et survivre à la piscine

### H.1 Avant le premier jour (si vous avez le temps)

1. **Maîtrisez les opérations bit à bit** (§2) : faites les exercices jusqu'à ce qu'ils deviennent automatiques.
2. **Installez la toolchain** (§9) et compilez un programme vide pour l'ATmega328P.
3. **Téléchargez la datasheet de l'ATmega328P** et survolez la table des matières, le chapitre I/O Ports et le Register Summary.
4. Si vous avez un Arduino Uno (ou en utilisant Wokwi), **faites le Day 00 et le Day 01 à l'avance** : faire clignoter une LED uniquement avec les registres, puis avec le Timer1.
5. Lisez le chapitre UART de la datasheet, en particulier les exemples de code.

### H.2 Routine quotidienne

1. Lisez **tout** le sujet du jour d'abord. Notez les restrictions (en-têtes autorisés, fonctions interdites, formats de sortie exacts).
2. Pour chaque exercice : identifier le périphérique → ouvrir le chapitre de la datasheet → lister les registres et bits sur papier → coder → tester.
3. **Commentez votre code avec des références à la datasheet**, par ex. `// datasheet §16.11.1 : WGM13:0 = 14 → Fast PWM, TOP = ICR1`. Cela impressionne les correcteurs et vous aide à défendre votre code.
4. Tenez un **fichier de notes personnel** : les registres utilisés, les pièges rencontrés. Il deviendra votre propre antisèche pour les rushes.
5. Construisez votre **propre mini-bibliothèque** (uart, i2c, adc…) jour après jour. Copiez-la dans les exercices suivants si les règles le permettent.

### H.3 Les évaluations entre pairs (peer evaluations)

Les correcteurs vous demanderont d'**expliquer**, pas seulement de montrer. Attendez-vous à des questions comme :
- « Pourquoi ce prescaler ? Montre-moi le calcul. »
- « Que fait ce bit ? Où est-il dans la datasheet ? »
- « Que se passe-t-il si j'appuie sur les deux boutons en même temps ? »
- « Pourquoi cette variable est-elle volatile ? »
- « Quelle est la différence entre UDRE0 et TXC0 ? »
- « Pourquoi envoies-tu un NACK sur le dernier octet ? »
- « À quoi servent les résistances de pull-up sur l'I2C ? »
- « Que se passerait-il sans le nombre magique ? »

Si vous savez y répondre grâce à ce guide, vous êtes prêt.

### H.4 État d'esprit

- La datasheet est votre amie. Elle est longue, mais elle a **toujours raison** (pour votre puce).
- Bloqué depuis plus de 30 minutes : relisez la section concernée de la datasheet, affichez les registres, et demandez à un pair.
- Les bugs matériels semblent mystérieux, mais ils sont déterministes. Il y a toujours une raison : un bit, un fil, un timing.
- Amusez-vous. Faire s'allumer, clignoter et communiquer des objets physiques est l'une des choses les plus satisfaisantes en programmation.

---

*Fin du guide. Bonne piscine !*
