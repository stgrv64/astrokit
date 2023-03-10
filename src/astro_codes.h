/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis astro_config.h
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_CODES_H
#define ASTRO_CODES_H

#include "astro_global.h"

#define CODES_X  CODES_CODE_NB_CODES
#define CODES_Y  CODES_NB_IN_OUT
#define CODES_Z  CONFIG_TAILLE_BUFFER_32

typedef enum {
    
  CODES_0_POS_IN_TERM=0,
  CODES_1_POS_IN_LIRC,
  CODES_2_POS_OUT_ACT  
}
t_en_Codes_Pos ;

/*----------------------------------------------*/
/* DEFINITION DES CODES d INTERRACTION en INPUT */ 
/*----------------------------------------------*/

/* Les 3 champs du tableau suivant sont respectivement :

- le numero du code dans l interraction TERMIOS
- le nom    du code dans l'interraction LIRC
- le nom    du code dans le code C (action correspondante)
*/


static const char gc_hach_codes[CODES_X][ CODES_Y ][CODES_Z] = {

/*--------------------------------*/
/* clavier numerique              */ 
/*--------------------------------*/

{ "48", "KEY_0", "0"}, /* 48 ascii = 0 */
{ "49", "KEY_1", "1"},  
{ "50", "KEY_2", "2"},
{ "51", "KEY_3", "3"},
{ "52", "KEY_4", "4"},
{ "53", "KEY_5", "5"},
{ "54", "KEY_6", "6"},
{ "55", "KEY_7", "7"},
{ "56", "KEY_8", "8"},
{ "57", "KEY_9", "9"},

/*-----------------------------------*/
/* clavier alphabetique (majuscules) */ 
/*-----------------------------------*/

{ "65",  "KEY_A",    "A"},  /* 97 ascii = 'a' */    
{ "66",  "KEY_B",    "B"},  
{ "67",  "KEY_C",    "C"},  
{ "68",  "KEY_D",    "D"},  
{ "69",  "KEY_E",    "E"},  
{ "70",  "KEY_F",    "F"},  
{ "71",  "KEY_G",    "G"},  
{ "72",  "KEY_H",    "H"},  
{ "73",  "KEY_I",    "I"},  
{ "74",  "KEY_J",    "J"},  
{ "75",  "KEY_K",    "K"},  
{ "76",  "KEY_L",    "L"},  
{ "77",  "KEY_M",    "M"},  
{ "78",  "KEY_N",    "N"},  
{ "79",  "KEY_O",    "O"},  
{ "80",  "KEY_P",    "P"},  
{ "81",  "KEY_Q",    "Q"},  
{ "82",  "KEY_R",    "R"},  
{ "83",  "KEY_S",    "S"},  
{ "84",  "KEY_T",    "T"},  
{ "85",  "KEY_U",    "U"},  
{ "86",  "KEY_V",    "V"},  
{ "87",  "KEY_W",    "W"},  
{ "88",  "KEY_X",    "X"},  
{ "89",  "KEY_Y",    "Y"},  
{ "90",  "KEY_Z",    "Z"},  

/*-----------------------------------*/
/* clavier alphabetique (minuscules) */ 
/*-----------------------------------*/

{ "97",  "KEY_LIST", "MODE_AZIMUTAL"},  /* 97 ascii = 'a' */ /* a=azimutal   => touche 'a' */    
{ "98",  "KEY_b",    "b"},  
{ "99",  "KEY_c",    "c"},  
{ "100", "KEY_d",    "d"},     
{ "101", "KEY_e",    "e"},  
{ "102", "KEY_f",    "f"},  
{ "103", "KEY_g",    "g"},     
{ "104", "KEY_h",    "h"}, /* 104 sum ascii = touche 'h' = affichage threads infos */
{ "105", "KEY_i",    "i"},    
{ "106", "KEY_j",    "j" },  /* 108 sum ascii = touche 'j' = generer les traces pid alt */ 
{ "107", "KEY_k",    "k" },  /* 108 sum ascii = touche 'k' = generer les traces pid azi */ 
{ "108", "KEY_l",    "l" },  /* 108 sum ascii = touche 'l' = activer les traces pid temps reel */ 
{ "109", "KEY_MENU", "MENU"},     /* 109  ascii = touche 'm' */
{ "110", "KEY_n",    "n"},     

{ "111", "KEY_o",    "o"},  
{ "112", "KEY_p",    "p"},    
{ "113", "KEY_EXIT", "key_exit" },
{ "114", "KEY_r",    "r" },           /* 114 sum ascii = touche 'r' = reseau */ 
{ "115", "KEY_s",    "s"},     /* 115 sum ascii = touche 's'        = log */ 
{ "116", "KEY_MUTE", "TIME"},        /* 116 ascii = touche 't' */
{ "117", "KEY_u",    "u"},     /* 117 sum ascii = touche 'u'     = log */ 
{ "118", "KEY_v",    "v"}, /* 118 sum ascii = touche 'v' = log */ 
{ "119", "KEY_w",    "w"},     /* 119 sum ascii = touche 'w'     = log */
{ "120", "KEY_x",    "x"},    

{ "121", "KEY_y",    "y"},    
{ "122", "KEY_MODE", "MODE_EQUATORIAL"}, /* z=equatorial => touche 'z' */     

/*--------------------------------*/
/* objets MES NGC ETO PLA         */
/*--------------------------------*/

{ "47",  "KEY_M",    "MES"},  /* 47 ascii = touche '/' */
{ "42",  "KEY_N",    "NGC"},  /* 42 ascii = touche '*' */
{ "45",  "KEY_E",    "ETO"},  /* 45 ascii = touche '-' */
{ "43",  "KEY_P",    "PLA"},  /* 43 ascii = touche '+' */

/*--------------------------------*/
/* touches de MENUS               */
/*--------------------------------*/

{ "10",        "KEY_PLAY",    "valider"},  /* 10  ascii = touche 'ENTER' */
{ "obsolete",  "KEY_STOP",    "stop"},     /* 188 ascii = touche 'FIN' */
{ "32",        "KEY_PAUSE",   "pause"},    /* 32 ascii = espace = pause */

/*--------------------------------*/
/* touches gauche droite haut bas ok */ 
/*--------------------------------*/

{ "296",       "KEY_OK",    "reset"}, /* ascii 296 = touche 'FIN' */

{ "183",       "KEY_UP",    "n"},     /* ascii SUM 183 (nread=3) => fleche du haut clavier */ 
{ "184",       "KEY_DOWN",  "s"},     /* ascii SUM 184 (nread=3) => fleche du bas clavier */ 
{ "185",       "KEY_RIGHT", "e"},     /* ascii SUM 185 (nread=3) => fleche de droite clavier */ 
{ "186",       "KEY_LEFT",  "o"},     /* ascii SUM 186 (nread=3) => fleche de gauche clavier */ 

/*--------------------------------*/
/* touche rattrapages             */
/*--------------------------------*/

{ "297",       "KEY_CHANNELUP" ,  "forwardfast"}, /* ascii 297 (nread=3) => page up */
{ "298",       "KEY_CHANNELDOWN", "rewindfast"},  /* ascii 298 (nread=3) => page down */
{ "45",        "KEY_VOLUMEUP"  ,  "forward"},     /* ascii 45 => - */
{ "43",        "KEY_VOLUMEDOWN" , "rewind"},      /* ascii 43 => + */

/*--------------------------------*/
/* touches de gestion du temps    */
/*--------------------------------*/


/*--------------------------------*/
/* touches affichage informations */
/*--------------------------------*/

/* TODO : pour afficher les informations utiliser touches Fx du clavier   */
/* FIXME : teste sur la cible , sum des codes ascii pour les touches Fx : */
/* F1 : 274  F2 : 275  F3 : 276  F4  : 277  F5  : 278  F6  : 348 */
/* F7 : 349  F8 : 350  F9 : 342  F10 : 343  F11 : 345  F12 : 346 */

{ "274",       "KEY_SCREEN",  "aff_tps_lie" },  /* 274 sum ascii = touche 'F1'  */ /* info 0 */
{ "275",       "KEY_TV",      "aff_ast_vit" },  /* 274 sum ascii = touche 'F2'  */ /* info 1 */
{ "276",       "KEY_INFO",    "aff_azi_alt" },  /* 275 sum ascii = touche 'F3'  */ /* info 2 */
{ "277",       "KEY_ZOOM",    "aff_agh_dec" },  /* 277 sum ascii = touche 'F4'  */ /* info 3 */

{ "278",       "non_defini",  "aff_asc_dec"       },   /* 278 sum ascii = touche 'F5'  */ /* info 4 */
{ "348",       "non_defini",  "cfg_gpios_alt_azi" },   /* 348 sum ascii = touche 'F6'  */ /* info 5 */
{ "349",       "non_defini",  "cfg_gpios_mas_fre" },   /* 348 sum ascii = touche 'F7'  */ /* info 5 */
{ "350",       "non_defini",  "cfg_gpios_leds"    },   /* 348 sum ascii = touche 'F8'  */ /* info 5 */
{ "342",       "non_defini",  "cfg_reduction"     },   /* 348 sum ascii = touche 'F9'  */ /* info 5 */
{ "343",       "non_defini",  "aff_ast_fre"       },   /* 348 sum ascii = touche 'F10' */ /* info 5 */
{ "345",       "non_defini",  "aff_ast_per"       },   /* 348 sum ascii = touche 'F11' */ /* info 5 */
{ "346",       "non_defini",  "aff_acc_alt_azi"   },   /* 346 sum ascii = touche 'F12'  */ /* info 5 */
{ "105",       "non_defini",  "aff_infos"         },   /* 346 sum ascii = touche 'F12'  */ /* info 5 */

/*--------------------------------*/
/* touches de permutations        */
/*--------------------------------*/

/*--------------------------------*/
/* arret du programme / de la carte */
/*--------------------------------*/

/* arret de la carte */
{ "27",        "KEY_POWER",     "key_power"}, /* ascii 27 => touch ESC */

/*--------------------------------*/
/* touches obsoletes */
/*--------------------------------*/

{ "obsolete",  "KEY_NEXT",    "plus"},     /* 62  ascii = touche '>' */
{ "obsolete",  "KEY_PREVIOUS","previous"}, /* action a definir */
{ "obsolete",  "KEY_SETUP",   "SETUP"},    /* non mis en place pour l instant (2022/04) */
{ "obsolete",  "KEY_SOUND",   "TIME"  },   /* remplace par KEY_MUTE sur la telecommande */
{ "obsolete",  "KEY_FORWARD", "forward"},  /* remplace par KEY_VOLUMEUP */
{ "obsolete",  "KEY_REWIND",  "rewind" },  /* remplace par KEY_VOLUMEDOWN */
{ "obsolete",  "KEY_RED",     "red"},      /* remplace par KEY_M (messier) */
{ "obsolete",  "KEY_BLUE",    "blue"},     /* remplace par KEY_N (ngc) */
{ "obsolete",  "KEY_YELLOW",  "yellow"},   /* remplace par KEY_E (etoiles) */
{ "obsolete",  "KEY_GREEN",   "green"},    /* remplace par KEY_P (planetes) */


{ "undefined", "undefined", "undefined"}, /* 121 sum ascii = touche 'w' = reseau */
{ "undefined", "undefined", "undefined"}, /* 122 sum ascii = touche 'w' = reseau */
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
}; 

typedef struct STR_CODES STRUCT_CODES ;

struct STR_CODES {
  void          (*cod_lock)       ( STRUCT_CODES *) ;
  void          (*cod_unlock)     ( STRUCT_CODES *) ;  
  void          (*cod_log)        ( STRUCT_CODES *) ;
  void          (*cod_display)    ( STRUCT_CODES *) ;
  pthread_mutex_t cod_mutex ;
  STR_EXT_TIMEVAL cod_tval ; 
  FILE           *cod_file ; 
  int             cod_loglevel ;
  char            cod_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char            cod_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  char            cod_in_term   [ CODES_CODE_NB_CODES][ CODES_CODE_BUFFER_SIZE] ;
  char            cod_in_lirc   [ CODES_CODE_NB_CODES][ CODES_CODE_BUFFER_SIZE] ;
  char            cod_out_act   [ CODES_CODE_NB_CODES][ CODES_CODE_BUFFER_SIZE] ;
  char            cod_hach_codes[ CODES_CODE_NB_CODES][ CODES_NB_IN_OUT ][CONFIG_TAILLE_BUFFER_32] ;

  int             cod_index ;
} ;

/*------------------------------------------  */
/* TAILLE TABLEAU 70 = CODES_CODE_NB_CODES    */
/* TODO : redefinir si besoin (doubler a 100) */
/*------------------------------------------  */

void   CODES_INIT      ( STRUCT_CODES *) ;

#endif