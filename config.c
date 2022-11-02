
MENU_PAR_DEFAUT  MENU_AZIMUTAL
ASTRE_PAR_DEFAUT NGC6755

//  ----------------------- parametres repertoires et fichiers 

CONFIG_REP_CAT       cat
CONFIG_REP_CFG       cfg
CONFIG_REP_LOG       log
CONFIG_REP_IN        inp
CONFIG_REP_OUT       out
CONFIG_REP_SCR       scr
CONFIG_FIC_LED       /tmp/astrokit-boo-recup-led-ir.txt
CONFIG_FIC_LOG       astrokit.log.new
CONFIG_FIC_DATE      date.txt
CONFIG_FIC_HHMM      hhmm.txt

CONFIG_SCR_KERNEL   script_kernel.sh

//  ----------------------- parametres temporisation                       
//  tempo_menu     doit etre legerement inferieur aux autres tempo         
//  tempo_raq      doit etre etre deux fois inferieur aux autres tempos    

TEMPO_MENU      1000000
TEMPO_RAQ       500000
TEMPO_IR        1000010
TEMPO_TERMIOS   1000020
TEMPO_CLAVIER   50000
TEMPO_CAPTEURS  50000
TEMPO_LCD_LOOP  1000000
TEMPO_LCD_DISP  1000000
TEMPO_PID_LOOP  5       

//  ----------------------- parametres asservissement de frequence PID 

PID_ECH 1.0
PID_KP  1.0
PID_KI  0.05 
PID_KD  0.05

//  ----------------------- parametres acquisitions 

DEVICE_USE_INFRAROUGE       1  //  flag qui indique utilisation de infrarouge comme source input
DEVICE_USE_KEYBOARD         1  //  flag qui indique utilisation du clavier comme source input
DEVICE_USE_LCD              0  //  flag qui indique utilisation de ecran LCD1602 / PCA8574
DEVICE_USE_CONTROLER        0  //  flag qui indique utilisation des controleurs de moteur externes
DEVICE_USE_CAPTEURS         0  //  flag qui indique utilisation de capteurs comme source input
DEVICE_USE_RAQUETTE         0  //  flag qui indique utilisation d une raquette (gpio) comme source input
DEVICE_USE_BLUETOOTH        0  //  flag qui indique utilisation du bluetooth comme source input

//  ----------------------- parametres geographiques 

//  Latitude et longitude Escout (64) 

LATITUDE   43.28
LONGITUDE  -0.37
ALTITUDE   100

//  Latitude et longitude Orgeres (35) 
//  LATITUDE   48.0 
//  LONGITUDE  -1.67 
//  ALTITUDE   100 

//  Latitude et longitude de test   
//  LATITUDE   61.0 
//  LONGITUDE  -149.9 
//  ALTITUDE   100 

// -------------------------------------------------------------------------- 
//  Conf corecte pour carte essai carre (celle qui s enfiche sur rpi3aplus) 
//  et moteurs en metal NEMA 11 (pas ceux qui sont sur la monture vixen gp) 
// -------------------------------------------------------------------------- 

GPIO_LED_ETAT   4
GPIO_ALT        21,26,19,13
GPIO_AZI        6,5,7,11
//  GPIO_MASQUE     0,1,2,3 

// -------------------------------------------------------------------------- 
//  Masque corect pour carte essai carre (celle qui s enfiche sur rpi3aplus) 
// -------------------------------------------------------------------------- 

//  GPIO_LED_ETAT   4 
//  GPIO_ALT        21,26,19,13 
//  GPIO_AZI        6,5,7,11 

GPIO_MASQUE     3,1,0,2

//  ----------------------- parametres pwm 

GPIO_FREQUENCE_PWM  250 

//  ----------------------- parametres reductions altitude 
//  Parametres des moteurs NEQ corriges a la date du 27 mars 2022 : reduction = 5760  
//  mesure precisemebnt sur 5 tours en 28800 pas avec le binaire /usr/local/bin/gpio23  

ALT_R1  144      //  reduction liee a la monture (nombre de dents de la roue dentee de l axe) 
ALT_R2  100       //  reducteur du moteur (nombre de pas) : R total = 5760 (64*90) (32*180) 
ALT_R3  90       //  reducteur du moteur (gearbox) : R total = 5760 (64*90) (32*180) 
ALT_R4  8       //  mode micro pas utilise (1/x) 
ALT_R5  1        //  reduction liee a la poulie 
ALT_R6  1        //  reduction liee au cpu 
ALT_R7  1        //  reduction non decrite plus haut .... 
ALT_ROT 0        //  Flag de reversibilite du sens de rotation 
ALT_ACC 2        //  Facteur de multiplication en mode MANUEL 

//  ----------------------- parametres reductions azimut 

AZI_R1  144      //  reduction lee a la monture (nombre de dents de la roue dentee de l axe) 
AZI_R2  100       //  reducteur du moteur (nombre de pas) : R total = 5760 (64*90) (32*180) 
AZI_R3  90       //  reducteur du moteur (gearbox) : R total = 5760 (64*90) (32*180) 
AZI_R4  8       //  mode micro pas utilise (1/x) 
AZI_R5  1        //  reduction liee a la poulie 
AZI_R6  1        //  reduction liee au cpu 
AZI_R7  1        //  reduction non decrite plus haut .... 
AZI_ROT 1        //  Flag de reversibilite du sens de rotation 
AZI_ACC 2        //  Facteur de multiplication en mode MANUEL 

//  ----------------------- parametres reductions altaz 

ALTAZ_FORWARD        1.005   //  affinage vitesses : acceleration globale tres lente (azi + alt) 
ALTAZ_REWIND         1.005   //  affinage vitesses : acceleration globale tres lente (azi + alt) 

ALTAZ_FORWARD_FAST   1.5     //  acceleration globale rapide (azi + alt) 
ALTAZ_REWIND_FAST    1.5     //  acceleration globale rapide (azi + alt) 



