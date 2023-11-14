/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis astro_config.h
# 2023-10-01  | correction bug sur lettres deja prises (MES => M) ...
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
/* objets MES NGC ETO PLA         */
/*--------------------------------*/

{ "47",  "KEY_M", "MES"},  /* 47 ascii = touche '/' */
{ "42",  "KEY_N", "NGC"},  /* 42 ascii = touche '*' */
{ "45",  "KEY_E", "ETO"},  /* 45 ascii = touche '-' */
{ "43",  "KEY_P", "PLA"},  /* 43 ascii = touche '+' */

/*--------------------------------*/
/* clavier numerique              */ 
/*--------------------------------*/

{ "48",  "KEY_0", "0"}, /* 48 ascii = 0 */
{ "49",  "KEY_1", "1"},  
{ "50",  "KEY_2", "2"},
{ "51",  "KEY_3", "3"},
{ "52",  "KEY_4", "4"},
{ "53",  "KEY_5", "5"},
{ "54",  "KEY_6", "6"},
{ "55",  "KEY_7", "7"},
{ "56",  "KEY_8", "8"},
{ "57",  "KEY_9", "9"},

/*-----------------------------------*/
/* clavier alphabetique (majuscules) */ 
/*-----------------------------------*/

{ "65",  "KEY_A_MAJ",    "KEY_A_MAJ"},  /* 97 ascii = 'a' */    
{ "66",  "KEY_B_MAJ",    "KEY_B_MAJ"},  
{ "67",  "KEY_C_MAJ",    "KEY_C_MAJ"},  
{ "68",  "KEY_D_MAJ",    "KEY_D_MAJ"},  
{ "69",  "KEY_E_MAJ",    "KEY_E_MAJ"},
{ "70",  "KEY_F_MAJ",    "KEY_F_MAJ"},  
{ "71",  "KEY_G_MAJ",    "KEY_G_MAJ"},  
{ "72",  "KEY_H_MAJ",    "KEY_H_MAJ"},  
{ "73",  "KEY_I_MAJ",    "KEY_I_MAJ"},  
{ "74",  "KEY_J_MAJ",    "KEY_J_MAJ"},  
{ "75",  "KEY_K_MAJ",    "KEY_K_MAJ"},  
{ "76",  "KEY_L_MAJ",    "KEY_L_MAJ"},  
{ "77",  "KEY_M_MAJ",    "KEY_M_MAJ"},  
{ "78",  "KEY_N_MAJ",    "KEY_N_MAJ"},  
{ "79",  "KEY_O_MAJ",    "KEY_O_MAJ"},  
{ "80",  "KEY_P_MAJ",    "KEY_P_MAJ"},  
{ "81",  "KEY_Q_MAJ",    "KEY_Q_MAJ"},  
{ "82",  "KEY_R_MAJ",    "KEY_R_MAJ"},  
{ "83",  "KEY_S_MAJ",    "KEY_S_MAJ"},  
{ "84",  "KEY_T_MAJ",    "KEY_T_MAJ"},  
{ "85",  "KEY_U_MAJ",    "KEY_U_MAJ"},  
{ "86",  "KEY_V_MAJ",    "KEY_V_MAJ"},  
{ "87",  "KEY_W_MAJ",    "KEY_W_MAJ"},  
{ "88",  "KEY_X_MAJ",    "KEY_X_MAJ"},  
{ "89",  "KEY_Y_MAJ",    "KEY_Y_MAJ"},  
{ "90",  "KEY_Z_MAJ",    "KEY_Z_MAJ"},  

/*-----------------------------------*/
/* clavier alphabetique (minuscules) */ 
/*-----------------------------------*/

{ "97",  "KEY_LIST",  "key_azi"},    /* 97 ascii = 'a' */ /* a=azimutal   => touche 'a' */    
{ "98",  "KEY_B_MIN", "key_b_min"},  
{ "99",  "KEY_C_MIN", "key_c_min"},  
{ "100", "KEY_D_MIN", "key_d_min"},     
{ "101", "KEY_E_MIN", "key_e_min"},  
{ "102", "KEY_F_MIN", "key_f_min"},  
{ "103", "KEY_G_MIN", "key_g_min"},     
{ "104", "KEY_H_MIN", "key_h_min"},    /* 104 sum ascii = touche 'h' = affichage threads infos */
{ "105", "KEY_I_MIN", "key_i_min"},    /* 105 sum ascii = touche 'i' = affichage infos generales astre / vitesses / etc */
{ "106", "KEY_J_MIN", "key_j_min" },   /* 108 sum ascii = touche 'j' = generer les traces pid alt */ 
{ "107", "KEY_K_MIN", "key_k_min" },   /* 108 sum ascii = touche 'k' = generer les traces pid azi */ 
{ "108", "KEY_L_MIN", "key_l_min" },   /* changement longitude */ 
{ "109", "KEY_MENU",  "MENU"},         /* 109  ascii = touche 'm' */
{ "110", "KEY_N_MIN", "key_n_min"},     
{ "111", "KEY_O_MIN", "key_o_min"},  
{ "112", "KEY_P_MIN", "key_pid"},       /* 112 sum ascii = touche 'p' = activer les traces pid temps reel */ 
{ "113", "KEY_EXIT",  "key_exit" },     /* 113 sum ascii = touche 'q' = 'quit' = quitter le programme */ 
{ "114", "KEY_R_MIN", "key_r_min" },            /* 114 sum ascii = touche 'r' = reseau */ 
{ "115", "KEY_S_MIN", "key_s_min"},             /* 115 sum ascii = touche 's'        = log */ 
{ "116", "KEY_MUTE",  "TIME"},          /* 116 sum ascii = touche 't' = temps */
{ "117", "KEY_U_MIN", "key_u_min"},             /* 117 sum ascii = touche 'u'     = log */ 
{ "118", "KEY_V_MIN", "key_v_min"},             /* 118 sum ascii = touche 'v' = log */ 
{ "119", "KEY_W_MIN", "key_w_min"},             /* 119 sum ascii = touche 'w'     = log */
{ "120", "KEY_X_MIN", "key_x_min"},    
{ "121", "KEY_Y_MIN", "key_y_min"},    
{ "122", "KEY_MODE",  "key_equ"},    /* z=equatorial => touche 'z' */     

/*--------------------------------*/
/* touches de MENUS               */
/*--------------------------------*/

{ "10",        "KEY_PLAY",    "valider"},  /* 10  ascii = touche 'ENTER' */
{ "obsolete",  "KEY_STOP",    "stop"},     /* 188 ascii = touche 'FIN' */
{ "32",        "KEY_PAUSE",   "pause"},    /* 32 ascii = espace = pause */

/*--------------------------------*/
/* touches rattrapages & reset    */ 
/*--------------------------------*/

{ "188",       "KEY_OK",          "reset"},       /* ascii 188 = touche FIN  => reset (annule tous les rattrapages) */
{ "183",       "KEY_UP",          "nord"},        /* ascii SUM 183 (nread=3) => fleche du haut clavier */ 
{ "184",       "KEY_DOWN",        "sud"},         /* ascii SUM 184 (nread=3) => fleche du bas clavier */ 
{ "185",       "KEY_RIGHT",       "est"},         /* ascii SUM 185 (nread=3) => fleche de droite clavier */ 
{ "186",       "KEY_LEFT",        "ouest"},       /* ascii SUM 186 (nread=3) => fleche de gauche clavier */ 
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
{ "348",       "non_defini",  "aff_gpios_alt_azi" },   /* 348 sum ascii = touche 'F6'  */ /* info 5 */
{ "349",       "non_defini",  "aff_gpios_mas_fre" },   /* 348 sum ascii = touche 'F7'  */ /* info 5 */
{ "350",       "non_defini",  "aff_gpios_leds"    },   /* 348 sum ascii = touche 'F8'  */ /* info 5 */
{ "342",       "non_defini",  "aff_reductions"     },   /* 348 sum ascii = touche 'F9'  */ /* info 5 */
{ "343",       "non_defini",  "aff_ast_fre"       },   /* 348 sum ascii = touche 'F10' */ /* info 5 */
{ "345",       "non_defini",  "aff_ast_per"       },   /* 348 sum ascii = touche 'F11' */ /* info 5 */
{ "346",       "non_defini",  "aff_acc_alt_azi"   },   /* 346 sum ascii = touche 'F12'  */ /* info 5 */
{ "105",       "non_defini",  "non_defini"         },   /* 346 sum ascii = touche 'F12'  */ /* info 5 */

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
{ "obsolete",  "KEY_PREVIOUS","key_previous"}, /* action a definir */
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
  void          (*cod_display_act)( STRUCT_CODES *, const char * ) ;

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