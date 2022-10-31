/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date           | commentaires 
--------------------------------------------------------------
octobre 2022 :
  * creation
  * recuperation du contenu des fonctions depuis config.c               
-------------------------------------------------------------- */

#include "astro_codes.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

static const char *g_char_Codes[][ CODES_NB_IN_OUT ] = {

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

{ "47",        "KEY_M","MES"},  /* 47 ascii = lettre '/' */
{ "42",        "KEY_N","NGC"},  /* 42 ascii = lettre '*' */
{ "45",        "KEY_E","ETO"},  /* 45 ascii = lettre '-' */
{ "43",        "KEY_P","PLA"},  /* 43 ascii = lettre '+' */

/*--------------------------------*/
/* touches de MENUS               */
/*--------------------------------*/

{ "10",        "KEY_PLAY",    "valider"},  /* 10  ascii = touche 'ENTER' */
{ "obsolete",  "KEY_STOP",    "stop"},     /* 188 ascii = touche 'FIN' */
{ "109",       "KEY_MENU",    "MENU"},     /* 109  ascii = lettre 'm' */
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

{ "116",       "KEY_MUTE",   "TIME"},        /* 116 ascii = lettre 't' */

/*--------------------------------*/
/* touches affichage informations */
/*--------------------------------*/

/* TODO : pour afficher les informations utiliser touches Fx du clavier   */
/* FIXME : teste sur la cible , sum des codes ascii pour les touches Fx : */
/* F1 : 274  F2 : 275  F3 : 276  F4  : 277  F5  : 278  F6  : 348 */
/* F7 : 349  F8 : 350  F9 : 342  F10 : 343  F11 : 345  F12 : 346 */

{ "274",       "KEY_SCREEN",  "aff_tps_lie" },  /* 274 sum ascii = lettre 'F1'  */ /* info 0 */
{ "275",       "KEY_TV",      "aff_ast_vit" },  /* 274 sum ascii = lettre 'F2'  */ /* info 1 */
{ "276",       "KEY_INFO",    "aff_azi_alt" },  /* 275 sum ascii = lettre 'F3'  */ /* info 2 */
{ "277",       "KEY_ZOOM",    "aff_agh_dec" },  /* 277 sum ascii = lettre 'F4'  */ /* info 3 */

{ "278",       "non_defini",  "aff_asc_dec"       },   /* 278 sum ascii = lettre 'F5'  */ /* info 4 */
{ "348",       "non_defini",  "cfg_gpios_alt_azi" },   /* 348 sum ascii = lettre 'F6'  */ /* info 5 */
{ "349",       "non_defini",  "cfg_gpios_mas_fre" },   /* 348 sum ascii = lettre 'F7'  */ /* info 5 */
{ "350",       "non_defini",  "cfg_gpios_leds"    },   /* 348 sum ascii = lettre 'F8'  */ /* info 5 */
{ "342",       "non_defini",  "cfg_reduction"     },   /* 348 sum ascii = lettre 'F9'  */ /* info 5 */
{ "343",       "non_defini",  "aff_ast_fre"       },   /* 348 sum ascii = lettre 'F10' */ /* info 5 */
{ "345",       "non_defini",  "aff_ast_per"       },   /* 348 sum ascii = lettre 'F11' */ /* info 5 */
{ "346",       "non_defini",  "aff_acc_alt_azi"   },   /* 346 sum ascii = lettre 'F12'  */ /* info 5 */
{ "105",       "non_defini",  "aff_infos"         },   /* 346 sum ascii = lettre 'F12'  */ /* info 5 */

/*--------------------------------*/
/* touches de permutations        */
/*--------------------------------*/

{ "97",           "KEY_LIST",   "key_azi"},    /* a=azimutal   => lettre 'a' */
{ "122",          "KEY_MODE",   "key_equ" },   /* z=equatorial => lettre 'z' */ 

/*--------------------------------*/
/* arret du programme / de la carte */
/*--------------------------------*/

/* arret de la carte */
{ "27",        "KEY_POWER",     "key_power"}, /* ascii 27 => touch ESC */
{ "113",       "KEY_EXIT",      "key_exit" },

/*--------------------------------*/
/* touches obsoletes */
/*--------------------------------*/

{ "obsolete",  "KEY_NEXT",    "plus"},     /* 62  ascii = lettre '>' */
{ "obsolete",  "KEY_PREVIOUS","previous"}, /* action a definir */
{ "obsolete",  "KEY_SETUP",   "SETUP"},    /* non mis en place pour l instant (2022/04) */
{ "obsolete",  "KEY_SOUND",   "TIME"  },   /* remplace par KEY_MUTE sur la telecommande */
{ "obsolete",  "KEY_FORWARD", "forward"},  /* remplace par KEY_VOLUMEUP */
{ "obsolete",  "KEY_REWIND",  "rewind" },  /* remplace par KEY_VOLUMEDOWN */
{ "obsolete",  "KEY_RED",     "red"},      /* remplace par KEY_M (messier) */
{ "obsolete",  "KEY_BLUE",    "blue"},     /* remplace par KEY_N (ngc) */
{ "obsolete",  "KEY_YELLOW",  "yellow"},   /* remplace par KEY_E (etoiles) */
{ "obsolete",  "KEY_GREEN",   "green"},    /* remplace par KEY_P (planetes) */
{ "114", "key_reseau_up",   "cfg_reseau_up" },           /* 114 sum ascii = lettre 'r' = reseau */ 
{ "108", "key_log",     "cfg_log_tps_reel_up" },         /* 108 sum ascii = lettre 'l' = activer les traces temps reel */ 
{ "107", "key_log_alt", "cfg_log_tps_reel_trace_azi" },  /* 108 sum ascii = lettre 'k' = generer les traces azi */ 
{ "106", "key_log_azi", "cfg_log_tps_reel_trace_alt" },  /* 108 sum ascii = lettre 'j' = generer les traces alt */ 
{ "115", "undefined", "undefined"},  /* 115 sum ascii = lettre 's' = reseau */ 
{ "117", "undefined", "undefined"},     /* 117 sum ascii = lettre 'u' = reseau */ 
{ "118", "undefined", "aff_variables"},     /* 118 sum ascii = lettre 'v' = reseau */ 
{ "119", "undefined", "undefined"},     /* 119 sum ascii = lettre 'w' = reseau */
{ "undefined", "undefined", "undefined"}, /* 120 sum ascii = lettre 'w' = reseau */
{ "undefined", "undefined", "undefined"}, /* 121 sum ascii = lettre 'w' = reseau */
{ "undefined", "undefined", "undefined"}, /* 122 sum ascii = lettre 'w' = reseau */
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
{ "undefined", "undefined", "undefined"},
}; 

/*****************************************************************************************
* @fn     : CODES_INIT_CODE
* @author : s.gravois
* @brief  : Cette fonction initialise un code (appelle par CODES_INIT)
* @param  : STRUCT_VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CODES_INIT_CODE( \

  STRUCT_CODES * gp_Cod, \
  int          li_pos, \
  const char * gl_char_Codes[][CODES_NB_IN_OUT] ) {

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("li_pos %d %-16s %-16s %-16s", \
    li_pos, \
    gl_char_Codes[li_pos][ CODES_POS_IN_TERM ], \
    gl_char_Codes[li_pos][ CODES_POS_IN_LIRC], \
    gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] \
  );

  strcpy( gp_Cod->in_term[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_TERM ] ) ;
  strcpy( gp_Cod->in_lirc[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_LIRC] ) ;
  strcpy( gp_Cod->out_act[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] ) ;
}
/*****************************************************************************************
* @fn     : CODES_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure des codes *STRUCT_CODES
* @brief  : etablie la correspondance entre les KEY du fichier lircd.conf (partie LIRC)
* @brief  : et les codes utilises dans le programme principal (car pas de hachage en c)
* @brief  : etablie aussi la correspondance entre les KEY du clavier termios (partie TERMIOS)
* @brief  : et les codes utilises dans le programme principal (code_action)
* @param  : STRUCT_CODES *gp_Cod
* @date   : 2022-03-21 creation entete
* @todo   : modifier : completer avec une fonction de hachage (regarder si API sur net)
*****************************************************************************************/

void CODES_INIT(STRUCT_CODES *gp_Cod) {

  int i_pos ;
  
  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) memset( gp_Cod->out_act[i_pos], CONFIG_ZERO_CHAR, sizeof(gp_Cod->out_act[i_pos]) ) ;
  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) memset( gp_Cod->in_lirc[i_pos],  CONFIG_ZERO_CHAR, sizeof(gp_Cod->in_lirc[i_pos]) ) ;

  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) {
    memset( gp_Cod->in_term[i_pos],  CONFIG_ZERO_CHAR, sizeof(gp_Cod->in_term[i_pos]) ) ;
    memset( gp_Cod->in_lirc[i_pos],  CONFIG_ZERO_CHAR, sizeof(gp_Cod->in_lirc[i_pos]) ) ;
    memset( gp_Cod->out_act[i_pos],  CONFIG_ZERO_CHAR, sizeof(gp_Cod->out_act[i_pos]) ) ;
  }
  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) {

    CODES_INIT_CODE( gp_Cod, i_pos, g_char_Codes  ) ; 
  }
  // FIXME :  ATTENTION !!! 
  // le nom des gp_Cod codes en MAJUSCULE servent a definir des actions dans config.c et le reste du programme

}