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

typedef enum {
    
  CODES_POS_IN_TERM=0,
  CODES_POS_IN_LIRC,
  CODES_POS_OUT_ACT  
}
t_en_Codes_Pos ;

struct STR_CODES {
  pthread_mutex_t cod_mutex ;
  char            cod_in_term [ CODES_CODE_NB_CODES][CODES_CODE_BUFFER_SIZE] ;
  char            cod_in_lirc [ CODES_CODE_NB_CODES][CODES_CODE_BUFFER_SIZE] ;
  char            cod_out_act [ CODES_CODE_NB_CODES][CODES_CODE_BUFFER_SIZE] ;
} ;

typedef struct STR_CODES STRUCT_CODES ;

/*----------------------------------------------*/
/* DEFINITION DES CODES d INTERRACTION en INPUT */ 
/*----------------------------------------------*/

/* Les 3 champs du tableau suivant sont respectivement :

- le numero du code dans l interraction TERMIOS
- le nom    du code dans l'interraction LIRC
- le nom    du code dans le code C (action correspondante)
*/


static const char *gc_hach_codes[][ CODES_NB_IN_OUT ] = {

/*--------------------------------*/
/* clavier numerique              */ 
/*--------------------------------*/

{ "48",        "KEY_0","0"}, /* 48 ascii = 0 */
{ "49",        "KEY_1","1"},  
{ "50",        "KEY_2","2"},
{ "51",        "KEY_3","3"},
{ "52",        "KEY_4","4"},
{ "53",        "KEY_5","5"},
{ "54",        "KEY_6","6"},
{ "55",        "KEY_7","7"},
{ "56",        "KEY_8","8"},
{ "57",        "KEY_9","9"},

/*--------------------------------*/
/* objets MES NGC ETO PLA         */
/*--------------------------------*/

{ "47",        "KEY_M","MES"},  /* 47 ascii = touche '/' */
{ "42",        "KEY_N","NGC"},  /* 42 ascii = touche '*' */
{ "45",        "KEY_E","ETO"},  /* 45 ascii = touche '-' */
{ "43",        "KEY_P","PLA"},  /* 43 ascii = touche '+' */

/*--------------------------------*/
/* touches de MENUS               */
/*--------------------------------*/

{ "10",        "KEY_PLAY",    "valider"},  /* 10  ascii = touche 'ENTER' */
{ "obsolete",  "KEY_STOP",    "stop"},     /* 188 ascii = touche 'FIN' */
{ "109",       "KEY_MENU",    "MENU"},     /* 109  ascii = touche 'm' */
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

{ "116",       "KEY_MUTE",   "TIME"},        /* 116 ascii = touche 't' */

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

{ "97",           "KEY_LIST",   "key_azi"},    /* a=azimutal   => touche 'a' */
{ "122",          "KEY_MODE",   "key_equ" },   /* z=equatorial => touche 'z' */ 

/*--------------------------------*/
/* arret du programme / de la carte */
/*--------------------------------*/

/* arret de la carte */
{ "27",        "KEY_POWER",     "key_power"}, /* ascii 27 => touch ESC */
{ "113",       "KEY_EXIT",      "key_exit" },

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

{ "114", "key_reseau_up",   "cfg_reseau_up" },           /* 114 sum ascii = touche 'r' = reseau */ 

{ "108", "key_log_pid", "cfg_log_tps_reel_up" },         /* 108 sum ascii = touche 'l' = activer les traces pid temps reel */ 
{ "107", "key_log_alt", "cfg_log_tps_reel_trace_azi" },  /* 108 sum ascii = touche 'k' = generer les traces pid azi */ 
{ "106", "key_log_azi", "cfg_log_tps_reel_trace_alt" },  /* 108 sum ascii = touche 'j' = generer les traces pid alt */ 

{ "115", "undefined", "undefined"},     /* 115 sum ascii = touche 's'        = log */ 
{ "117", "undefined", "undefined"},     /* 117 sum ascii = touche 'u'     = log */ 
{ "118", "undefined", "aff_variables"}, /* 118 sum ascii = touche 'v' = log */ 
{ "119", "undefined", "undefined"},     /* 119 sum ascii = touche 'w'     = log */

{ "104", "undefined", "aff_thread_infos"}, /* 104 sum ascii = touche 'h' = affichage threads infos */
{ "undefined", "undefined", "undefined"}, /* 121 sum ascii = touche 'w' = reseau */
{ "undefined", "undefined", "undefined"}, /* 122 sum ascii = touche 'w' = reseau */
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
}; 

/*------------------------------------------  */
/* TAILLE TABLEAU 70 = CODES_CODE_NB_CODES    */
/* TODO : redefinir si besoin (doubler a 100) */
/*------------------------------------------  */

void   CODES_INIT_CODE ( STRUCT_CODES *, int, const char * [][CODES_NB_IN_OUT] ) ;
void   CODES_INIT      ( STRUCT_CODES *) ;

#endif