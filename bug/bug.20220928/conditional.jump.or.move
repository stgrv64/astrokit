root@stef-20qqs01l00:/home/stef/git/astrokit# valgrind ./astrokit.septembre.2022.host
==10871== Memcheck, a memory error detector
==10871== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==10871== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==10871== Command: ./astrokit.septembre.2022.host
==10871== 

ARGUMENTS_GERER_REP_HOME        :CONFIG_REP_HOME = /home/stef/git/astrokit
CONFIG_PATH_FIND        :Test chemin /bin/stty
main    :Chemin retenu : /bin/stty
CONFIG_READ     :open /home/stef/git/astrokit/config.txt ok
GPIO_READ       :Lecture dans fichier de config
CONFIG_AFFICHER_VARIABLES       :TEMPO_RAQ = 10000
CONFIG_AFFICHER_VARIABLES       :TEMPO_MENU = 20000
CONFIG_AFFICHER_VARIABLES       :TEMPO_IR = 27000
CONFIG_AFFICHER_VARIABLES       :TEMPO_TERMIOS = 27500
CONFIG_AFFICHER_VARIABLES       :TEMPO_CLAVIER = 27500
CONFIG_AFFICHER_VARIABLES       :TEMPO_CAPTEURS = 27500
CONFIG_AFFICHER_VARIABLES       :TEMPO_LCD_LOOP = 250000
CONFIG_AFFICHER_VARIABLES       :TEMPO_LCD_DISP = 100000
CONFIG_AFFICHER_VARIABLES       :PID_ECH = 3.000000
CONFIG_AFFICHER_VARIABLES       :PID_KI = 0.000000
CONFIG_AFFICHER_VARIABLES       :PID_KP = 0.050000
CONFIG_AFFICHER_VARIABLES       :PID_KD = 0.050000
CONFIG_INIT_LOG :start
GPIO_RAQUETTE_CONFIG    :
CONFIG_INIT_CLAVIER     :==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x1144CB: CALCUL_TEMPS_DEC_VERS_HMS (calculs.c:731)
==10871==    by 0x114D85: CALCUL_TEMPS_SIDERAL (calculs.c:1036)
==10871==    by 0x10E159: main (astro.c:1885)
==10871== 

LCD_INIT        :FAHW-Lib: Fail to I2CWriteByte

LCD_INIT        :Fail to init LCD1602Init
PID_INIT        :ouverture log/ (KO)
PTHREADS_INIT   :user is :  (getuid) 0 
PTHREADS_INIT   :Mise en place temps reel et parallelisme 
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_INFRAROUGE = 1
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_CAPTEURS   = 0
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_RAQUETTE   = 0
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_BLUETOOTH  = 0
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_KEYBOARD   = 1
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_CONTROLER  = 0
CONFIG_AFFICHER_DEVICES_USE     :gp_Dev->DEVICE_USE_LCD        = 0
main    :gpio_alt         : 21 26 19 13
main    :gpio_azi         : 6 5 7 11
main    :gpio_mas         : 3 1 0 2
main    :GPIO_LED_ETAT    : 4
main    :ASTRE_PAR_DEFAUT : NGC5061
main    :PID_ECH = 3.000000
CAT_FIND        :(trouve) : (nom) NGC5061 (infos) NGC5061: 
CAT_FIND        : NGC5061 : asc 13.18.0 (hms) dec -26.83 (deg)
ARGUMENTS_GERER_FACON_CLASSIQUE :as (null) pris en comptefin ARGUMENTS_GERER

GPIO_CLOSE_BROCHE       :fermeture GPIO 0
GPIO_OPEN_BROCHE        :gpio ouverture en OUTPUT : dir= /sys/class/gpio/gpio4/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio4/value
GPIO_OPEN_BROCHE_PWM    :open gpio 13 : dir= /sys/class/gpio/gpio13/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio13/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 13 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 26 : dir= /sys/class/gpio/gpio26/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio26/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 26 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 21 : dir= /sys/class/gpio/gpio21/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio21/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 21 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 19 : dir= /sys/class/gpio/gpio19/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio19/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 19 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 11 : dir= /sys/class/gpio/gpio11/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio11/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 11 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 5 : dir= /sys/class/gpio/gpio5/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio5/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 5 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 6 : dir= /sys/class/gpio/gpio6/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio6/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 6 en erreur -6
GPIO_OPEN_BROCHE_PWM    :open gpio 7 : dir= /sys/class/gpio/gpio7/direction exp=/sys/class/gpio/export val=/sys/class/gpio/gpio7/value sta=-6
GPIO_OPEN_BROCHE_PWM    :ouverture du gpio 7 en erreur -6
main    :pm_azi->Fm 12.500000 pm_azi->periode_mot = 0.080000
main    :pm_alt->Fm 12.500000 pm_alt->periode_mot = 0.080000
suivi_main_M    :start
suivi_main_M    :start==10871== Thread 17:
==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x1111AA: SUIVI_LCD (astro.c:1358)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 
==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x1111CF: SUIVI_LCD (astro.c:1359)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 

GPIO_SUIVI_PWM_PHASE    :start
SUIVI_MENU      :start
GPIO_SUIVI_PWM_PHASE    :start==10871== Thread 12 SUIVI_MENU:
==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x11142A: SUIVI_TRAITEMENT_MOT (astro.c:249)
==10871==    by 0x112C83: SUIVI_MENU (astro.c:950)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 
==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x111448: SUIVI_TRAITEMENT_MOT (astro.c:250)
==10871==    by 0x112C83: SUIVI_MENU (astro.c:950)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 

SUIVI_MENU      :appel : 0 : MENU_AZIMUTAL==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x1144CB: CALCUL_TEMPS_DEC_VERS_HMS (calculs.c:731)
==10871==    by 0x114D85: CALCUL_TEMPS_SIDERAL (calculs.c:1036)
==10871==    by 0x1151F0: CALCUL_TOUT (calculs.c:1262)
==10871==    by 0x112D79: SUIVI_MENU (astro.c:1000)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 

GPIO_SUIVI_PWM_PHASE    :start
GPIO_SUIVI_PWM_PHASE    :start
SUIVI_MANUEL_BRUT       :reset..    : Sh 1 Sa -1 pas_azi = -1 pas_alt = 1 acc_azi = 1.000000 acc_alt = 1.000000
GPIO_SUIVI_PWM_PHASE    :start
GPIO_SUIVI_PWM_PHASE    :start
GPIO_SUIVI_PWM_PHASE    :start
GPIO_SUIVI_PWM_PHASE    :start
SUIVI_INFRAROUGE        :startdo_connect: could not connect to socket
connect: No such file or directory

LIRC_READ       :start==10871== Warning: invalid file descriptor -2 in syscall read()
==10871== Warning: invalid file descriptor -2 in syscall close()

SUIVI_INFRAROUGE        :Stop
SUIVI_LCD       :start
SUIVI_LCD       :Stop
SUIVI_CLAVIER_TERMIOS   :start==10871== Thread 13:
==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x11028F: SUIVI_VOUTE (astro.c:1185)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 
==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x1102B4: SUIVI_VOUTE (astro.c:1186)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 

SUIVI_CAPTEURS  :start
SUIVI_CAPTEURS  :Stop
SUIVI_VOUTE     :start==10871== Conditional jump or move depends on uninitialised value(s)
==10871==    at 0x1144CB: CALCUL_TEMPS_DEC_VERS_HMS (calculs.c:731)
==10871==    by 0x114D85: CALCUL_TEMPS_SIDERAL (calculs.c:1036)
==10871==    by 0x1151F0: CALCUL_TOUT (calculs.c:1262)
==10871==    by 0x11040A: SUIVI_VOUTE (astro.c:1238)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 

CONFIG_AFFICHER_MODE_LONG       : NGC5061 : infos         : NGC5061
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : type          : ASTRE_CIEL_PROFOND
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : mode calcul   : MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : latitude      : 43.28 (deg) 
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : longitude     : -0.37 (deg) 
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : vitesses      : -9.04 (Va) -10.50 (Vh)
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : azimut        : 106.49 (deg) 
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : altitude      : -22.31 (deg) 
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : declinaison   : -26.83 (deg) 
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : ascension dro : 199.50 (deg) 13h18m0s (HH.MM.SS)
CONFIG_AFFICHER_MODE_LONG       : NGC5061 : angle horaire : 263.80 (deg) 17h35m12s (HH.MM.SS)
CONFIG_AFFICHER_MODE_LONG       :----------------------------
CONFIG_AFFICHER_MODE_STELLARIUM :Va / Vh    : -9.04 / -10.50
CONFIG_AFFICHER_MODE_STELLARIUM :AD / Dec   :   13h18m 0s / - 26  d 49'
CONFIG_AFFICHER_MODE_STELLARIUM :AH / Dec   :   17h35m12s / - 26  d 49'
CONFIG_AFFICHER_MODE_STELLARIUM :AZ./ Haut. : + 106 d 29' / - 22  d 18'
LCD_DISPLAY_CURRENT     :==10871== Jump to the invalid address stated on the next line
==10871==    at 0x0: ???
==10871==    by 0x11044C: SUIVI_VOUTE (astro.c:1250)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==10871== 
==10871== 
==10871== Process terminating with default action of signal 11 (SIGSEGV)
==10871==  Bad permissions for mapped region at address 0x0
==10871==    at 0x0: ???
==10871==    by 0x11044C: SUIVI_VOUTE (astro.c:1250)
==10871==    by 0x4874608: start_thread (pthread_create.c:477)
==10871==    by 0x4B72132: clone (clone.S:95)
==10871== 
==10871== HEAP SUMMARY:
==10871==     in use at exit: 37,440 bytes in 24 blocks
==10871==   total heap usage: 164 allocs, 140 frees, 252,058 bytes allocated
==10871== 
==10871== LEAK SUMMARY:
==10871==    definitely lost: 0 bytes in 0 blocks
==10871==    indirectly lost: 0 bytes in 0 blocks
==10871==      possibly lost: 4,352 bytes in 16 blocks
==10871==    still reachable: 33,088 bytes in 8 blocks
==10871==         suppressed: 0 bytes in 0 blocks
==10871== Rerun with --leak-check=full to see details of leaked memory
==10871== 
==10871== Use --track-origins=yes to see where uninitialised values come from
==10871== For lists of detected and suppressed errors, rerun with: -s
==10871== ERROR SUMMARY: 230 errors from 10 contexts (suppressed: 0 from 0)
Segmentation fault (core dumped)

