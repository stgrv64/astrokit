#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | gpio.c
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | ajout entete
# 01/05/2021  | ajout commentaire sur fonction GPIO_READ2
#               synthese traces sur une ligne (2021)
# 18/01/2022  | * les lignes close( fd ) posent un probleme
# (issue)  lors de utilisation de la librairie ncurses (getch)
# ainsi que termios (cf fichier keyboard.c / .h ) :
# creation entete de la fonction au format doxygen du reliquat de close dans le code
# 23/01/2022  | * ajout condition pour entree dans GPIO_RAQUETTE_READ
#   =>  if ( devices->DEVICE_USE_RAQUETTE ) {  
# 20 mai 2022 | * reprise de la fonction main Gpio pour effectuer
#                 des fonctions de deplacements utiles      
# 24 mai 2022 | * reprise des fonctions PWM
#               * mise en place de marqueurs de temps et de pas
# -------------------------------------------------------------- 
*/

#include <gpio.h>

/*****************************************************************************************
* @fn     : GPIO_TRAP_SIG
* @author : s.gravois
* @brief  : fonction appelle quand un signal est trape dans main gpio
* @param  : int     sig
* @date   : 2022-05-20 creation 
* @todo   : voir s il est preferable de trapper egalement depuis les threads (si c possible)
*****************************************************************************************/

void GPIO_TRAP_SIG(int sig) {

  char c_cmd[ 64 ] ;
  int i ;

  /*----------------------------------*/
  /* Rendre le terminal propre (sane) */
  /*----------------------------------*/

  memset( c_cmd, 0, sizeof(c_cmd)) ;
  sprintf(c_cmd,"%s sane", g_Path_Cmd_Stty ) ;

  if ( system( c_cmd ) < 0 ) {

    SyslogErr("Probleme avec stty sane\n") ;
    Trace("Probleme avec stty sane") ;
    exit(2) ;
  } 
  Trace("appel exit(0)") ;
  
  exit(0) ;
}
/*****************************************************************************************
* @fn     : GPIO_CLIGNOTE
* @author : s.gravois
* @brief  : Cette fonction permet de faire clignoter la LED d ETAT 
* @param  : int gpio
* @param  : int nombre_clignotement
* @param  : int duree_clignotement
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : effectuer la meme fonction mais avec variation continue PWM
*           (necessite utiliser les threads rapports cycliques)
*****************************************************************************************/

void GPIO_CLIGNOTE(int gpio, int nombre_clignotement, int duree_clignotement) {
 int i ;
 i=-1; 
 while(++i<nombre_clignotement) {
  GPIO_SET( GPIO_LED_ETAT, 1 ) ; usleep(duree_clignotement*10000);
  GPIO_SET( GPIO_LED_ETAT, 0 ) ; usleep(duree_clignotement*10000);
 }
}
/*****************************************************************************************
* @fn     : GPIO_TAB_TOKEN
* @author : s.gravois
* @brief  : Cette fonction permet de lire les entrees de type 0,1,2,3 (token)
*           a deplacer eventuellement dans un fichier util.c / util.h
* @param  : int    tab[4]
* @param  : char * buffer
* @param  : char * separator
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : voir si realisable sous forme de MACRO
*****************************************************************************************/

void GPIO_TAB_TOKEN(int tab[4],char * buffer, char * separator) {
  int j ;
  char *str1, *token, *sptr ;
  
  for (j = 0, str1 = buffer ; ; j++, str1 = NULL) {
    token = strtok_r(str1, separator, &sptr);
    if (token == NULL || j == 4) break ;
    tab[j]=atoi(token);
   }
}
/*****************************************************************************************
* @fn     : GPIO_READ
* @author : s.gravois
* @brief  : Cette fonction lit les parametres GPIO dans le fichier de configuration
*           (GPIO_ALT / GPIO_AZI / GPIO_MASQUE )
*           Initilise la frequence PWM a 1000 si aucune entree gpio_frequence_pwm
* @param  : g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void GPIO_READ(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {
  int l,i, j ;
  char *str1, *token, *sptr ;
  
  Trace("Lecture dans fichier de config") ;

  for(l=0;l<DATAS_NB_LIGNES;l++) {

   if(!strcmp("GPIO_ALT",g_Datas[l][0])) {

    // FIXME ajout stephane 2021
    memset( GPIO_ALT,0,sizeof(GPIO_ALT)) ;
    strcpy( GPIO_ALT, g_Datas[l][1] ) ;

    Trace1("GPIO_ALT trouve ligne %d = (%s)", l,GPIO_ALT) ;

    for(j=0;j<GPIO_NB_PHASES_PAR_MOTEUR;j++) gpio_in[j]=-1 ;

    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_alt[j]=atoi(token);
    }
   }
   
   if(!strcmp("GPIO_AZI",g_Datas[l][0])) {

    // FIXME ajout stephane 2021
    memset( GPIO_AZI,0,sizeof(GPIO_AZI)) ;
    strcpy( GPIO_AZI, g_Datas[l][1] ) ;

    Trace1("GPIO_AZI trouve ligne %d = (%s)", l,GPIO_AZI) ;

    for(i=0; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++) gpio_out[i]=-1 ;

    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_azi[j]=atoi(token);
    }
   }
   

   if(!strcmp("GPIO_MASQUE",g_Datas[l][0])) {

    // FIXME ajout stephane 2021
    memset( GPIO_MAS,0,sizeof(GPIO_MAS)) ;
    strcpy( GPIO_MAS, g_Datas[l][1] ) ;

    Trace1("GPIO_MASQUE trouve ligne %d = (%s)", l,GPIO_MAS) ;

    for(i=0; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++) gpio_mas[i]=-1 ;

    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_mas[j]=atoi(token);
    }
   }


   if(!strcmp("GPIO_FREQUENCE_PWM",g_Datas[l][0])) {

    // FIXME ajout stephane 2021
    memset( GPIO_FREQUENCE_PWM,0,sizeof(GPIO_FREQUENCE_PWM)) ;
    strcpy( GPIO_FREQUENCE_PWM, g_Datas[l][1] ) ;

    gpio_frequence_pwm = 1000 ;

    Trace1("GPIO_FREQUENCE_PWM trouve ligne %d = (%s)", l,GPIO_FREQUENCE_PWM) ;

    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_frequence_pwm=(double)atoi(token);
    }
   }
  }
  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) Trace1("gpio_azi[%d]=%d",i,gpio_azi[i]);
  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) Trace1("gpio_alt[%d]=%d",i,gpio_alt[i]);
  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) Trace1("gpio_mas[%d]=%d",i,gpio_mas[i]);
   
  TRACE1("GPIO_FREQUENCE_PWM=%f", gpio_frequence_pwm ) ;
}
/*****************************************************************************************
* @fn     : GPIO_READ
* @author : s.gravois
* @brief  : Cette fonction lit GPIO_INPUT et GPIO_OUTPUT
* @param  : g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) fonction ancienne, remplace explicitement par GPIO_LED_xx etc..
*****************************************************************************************/

void GPIO_READ2(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {

  int l,i, j ;
  char *str1, *token, *sptr ;
  
  for(l=0;l<DATAS_NB_LIGNES;l++) {
   if(!strcmp("GPIO_INPUT",g_Datas[l][0])) {
    for(j=0;j<GPIO_SIZE;j++) gpio_in[j]=-1 ;
    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_in[j]=atoi(token);
    }
   }
   if(!strcmp("GPIO_OUTPUT",g_Datas[l][0])) {
    for(i=0;i<GPIO_SIZE;i++) gpio_out[i]=-1 ;
    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_out[j]=atoi(token);
    }
   }
  }
   for(i=0;i<GPIO_SIZE;i++) TRACE1("gpio_in[%d] =%d\n",i,gpio_in[i]);
   for(i=0;i<GPIO_SIZE;i++) TRACE1("gpio_out[%d]=%d\n",i,gpio_out[i]);
}
/*****************************************************************************************
* @fn     : GPIO_RAQUETTE_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure un clavier matriciel X lignes sur Y colonnes
* @param  : int GPIO_KEY_L[4]
* @param  : int GPIO_KEY_C[4]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) fonction ancienne, remplace par un clavier reel en USB
*****************************************************************************************/

/* ---------------------------------------------------------------------------------------
* Configuration du clavier (clavier matriciel en 4*4) en fonction des entrees de config :
* lecture ou ecriture premieres sur les gpios
* GPIO_KEY_L* et GPIO_KEY_C* sont definies dans le fichier de config
 ---------------------------------------------------------------------------------------*/

void GPIO_RAQUETTE_CONFIG (int GPIO_KEY_L[4],int GPIO_KEY_C[4]) {
  
  int  i,j ;

  Trace("") ;

  if ( devices->DEVICE_USE_RAQUETTE ) {
    
    GPIO_KEY_L[0] = GPIO_KEY_L1 ; 
    GPIO_KEY_L[1] = GPIO_KEY_L2 ; 
    GPIO_KEY_L[2] = GPIO_KEY_L3 ; 
    GPIO_KEY_L[3] = GPIO_KEY_L4 ;

    GPIO_KEY_C[0] = GPIO_KEY_C1 ; 
    GPIO_KEY_C[1] = GPIO_KEY_C2 ; 
    GPIO_KEY_C[2] = GPIO_KEY_C3 ; 
    GPIO_KEY_C[3] = GPIO_KEY_C4 ;

    for(i=0;i<4;i++) {

      GPIO_SET( GPIO_KEY_C[i], 0) ;

      for(j=0;j<4;j++)

        if( GPIO_GET(GPIO_KEY_L[j])) {

          TRACE("Clavier ligne %d et colonne %d => %d mis a 5V et GPIO %d allumer\n",\
            i,j,GPIO_KEY_C[i],GPIO_KEY_L[j]); }
        else {
          TRACE("Clavier ligne %d et colonne %d => %d mis a 5V et GPIO %d eteint\n",\
          i,j,GPIO_KEY_C[i],GPIO_KEY_L[j]); }
    }
  }
}
/*****************************************************************************************
* @fn     : GPIO_RAQUETTE_READ
* @author : s.gravois
* @brief  : Cette fonction lie une touche sur un clavier matriciel 4-4
* @param  : int       GPIO_KEY_L[4]
* @param  : int       GPIO_KEY_C[4]
* @param  : CLAVIER * clavier
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-18 avoid -Wrestrict passing pointers via buffer_recopie
* @todo   : (obsolete) fonction ancienne, remplace par un clavier reel en USB
*****************************************************************************************/

void GPIO_RAQUETTE_READ (int GPIO_KEY_L[4],int GPIO_KEY_C[4], CLAVIER* clavier) {

  int  i=0,j=0;
  int I=0, J=0  ;
  char val[255] ;
  
  char buffer_recopie [ CONFIG_TAILLE_BUFFER_32 ] ;

  if ( devices->DEVICE_USE_RAQUETTE ) {

    I=-1; ; J=-1 ;
    
    for(i=0;i<4;i++) GPIO_SET( GPIO_KEY_C[i], 0) ;
    //for(j=0;j<4;j++) GPIO_SET( GPIO_KEY_L[i], 0) ;
    
    strcpy(val,"") ;     
    
    /* =======================================================================
    * Lecture d'une touche au clavier matriciel 4*4
    * Tant que une touche est appuyee la valeur appui_en_cours est a 1
    * On passe plusieurs fois dans cette boucle en fonction de la fonction appelante
    * qui est dans astro.c
    ======================================================================= */
    
    clavier->appui_en_cours = 0 ;
      
    for(i=0;i<4;i++) {
      GPIO_SET( GPIO_KEY_C[i], 1) ;
      
      usleep( clavier->temporisation_clavier ) ;
      
      for(j=0;j<4;j++)  {
        if( GPIO_GET(GPIO_KEY_L[j])) {
          I=i; J=j ;
      memset( val, '\0', 255 );
            strcpy( val, keyboard[I][J] ) ;
      if ( strcmp( val, "") ) {
        //printf("val = %s, keyboard[ %d ][ %d ] = %s\n", val, i,j, keyboard[i][j] ) ;
        strcpy( clavier->mot, val ) ; 
              clavier->appui_en_cours = 1 ;
        clavier->mot_en_cours = 1 ;
      }
        }
      }
      GPIO_SET( GPIO_KEY_C[i], 0) ; 
    }
    
    // =======================================================================
    // Quand la touche est relacheee, on traite
    // ensuite appui en cours est remis a ZERO pour qu'on ne passe qu'une fois
    // dans cette partie de code
    // =======================================================================
    
    if ( clavier->mot_en_cours && ! clavier->appui_en_cours ) {
    
      // printf("mot trouver = %s\n", clavier->mot ) ;
      
      //------------------------------------------------------------
      // On incremente la phrase avec le mot et
      // On incremente le nombre avec le mot si premier n'est pas vide
      //------------------------------------------------------------
      
      if ( strcmp( clavier->mot, clavier->valider) != 0 ) { 
        
        if ( strlen(clavier->phrase) + strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32)

          /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
          memset( buffer_recopie, 0, sizeof(buffer_recopie) ) ;
          strcpy( buffer_recopie, clavier->phrase ) ;
          /* sprintf( clavier->phrase,"%s%s", clavier->phrase, clavier->mot); */
          sprintf( clavier->phrase,"%s%s", buffer_recopie, clavier->mot);
        
        if ( strcmp( clavier->premier, "")) {
          if ( strlen(clavier->nombre) + strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32)
          //printf("Si pas d'appui sur valider et premier non vide => on met le mot dans la phrase !!\n" ) ; 
            /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
            memset( buffer_recopie, 0, sizeof(buffer_recopie) ) ;
            strcpy( buffer_recopie, clavier->nombre ) ;
            /* sprintf(clavier->nombre,"%s%s", clavier->nombre, clavier->mot); */
            sprintf(clavier->nombre,"%s%s", buffer_recopie, clavier->mot); 
        }
      }    
      //------------------------------------------------------------
      // On met le mot dans premier si il est vide 
      //------------------------------------------------------------
      
      if (   strcmp( clavier->premier, "") ==0 ){ 
        // printf("Si premier est vide on met le mot en cours dedans\n" ) ; 
        strcpy( clavier->premier, clavier->mot);
      }
      
      //------------------------------------------------------------
      // Si la phrase en cours ou le mot en cours est une VALIDATION 
      // Exemple : MENU - MES ou valider
      // Alors on VALIDE la phrase en cours
      //  - en mettant premier dans       SYMBOLE
      //  - et le reste de la phrase dans NOMBRE    
      // et on met le mot dans PREMIER (premier mot de la phrase)
      //------------------------------------------------------------
      
      for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )
      if ( ! strcmp( clavier->phrase, clavier->validations[i]) \
        || ! strcmp( clavier->mot,    clavier->valider )  ) {

        TRACE1("APPUI sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1\n" ) ; 
        
        strcpy(clavier->symbole, clavier->premier)  ;
        
        strcpy(clavier->premier,"") ;
        strcpy(clavier->phrase,"")  ;
        strcpy(clavier->mot,"") ;
        
        TRACE1("TROIS = symbole = %s nombre = %s\n", clavier->symbole, clavier->nombre ) ;

        clavier->phrase_lue=1 ;
      }
      
      //------------------------------------------------------------
      // Si le mot est une ACTION, on efface la phrase en cours    
      // et on met le mot dans PREMIER (premier mot de la phrase)
      //------------------------------------------------------------
      
      for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ )
      if ( ! strcmp( clavier->mot, clavier->actions[i] )) {
          printf("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier") ;
          strcpy(clavier->premier,clavier->mot) ;
          strcpy(clavier->nombre,"")  ;
          strcpy(clavier->phrase,"")  ;
          strcpy(clavier->mot,"") ;
      }
      
      clavier->mot_en_cours = 0 ;
      clavier->appui_en_cours = 0 ;
    }
  }
}
/*****************************************************************************************
* @fn     : GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS
* @author : s.gravois
* @brief  : Cette fonction lie une touche sur un clavier matriciel 4-4
* @param  : int       GPIO_KEY_L[4]
* @param  : int       GPIO_KEY_C[4]
* @param  : CLAVIER * clavier
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-18 avoid -Wrestrict passing pointers via buffer_recopie
* @todo   : (obsolete) fonction ancienne, remplace par un clavier reel en USB
*****************************************************************************************/

void GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS(int GPIO_KEY_L[4],int GPIO_KEY_C[4], SUIVI *suivi) {
  
  int  i,j;
  char val[255] ;
  
  if ( devices->DEVICE_USE_RAQUETTE ) {
    
    Trace("");
    
    for(i=0;i<4;i++) {
      GPIO_SET( GPIO_KEY_C[i], 0) ;
    }
  
    strcpy(val,"") ;       
    
    for(i=0;i<4;i++) { 
      
      GPIO_SET( GPIO_KEY_C[i], 1) ; 
      
      for(j=0;j<4;j++)  { 

        if( GPIO_GET(GPIO_KEY_L[j])) {
   
          if ( ! strcmp( raquette[i][j], "plus" ) )  {  suivi->pas_acc_plus =1 ; }
          if ( ! strcmp( raquette[i][j], "moins" ) ) { suivi->pas_acc_moins=1 ; }
          if ( ! strcmp( raquette[i][j], "ne" ) )    { suivi->pas_nord=1 ; suivi->pas_est=1   ; }
          if ( ! strcmp( raquette[i][j], "no" ) )    { suivi->pas_nord=1 ; suivi->pas_ouest=1 ; }
          if ( ! strcmp( raquette[i][j], "se" ) )    { suivi->pas_sud=1  ; suivi->pas_est=1   ; }
          if ( ! strcmp( raquette[i][j], "so" ) )    { suivi->pas_sud=1  ; suivi->pas_ouest=1 ; }
          if ( ! strcmp( raquette[i][j], "n" ) )     { suivi->pas_nord  = 1 ; }
          if ( ! strcmp( raquette[i][j], "o" ) )     { suivi->pas_ouest = 1 ; }
          if ( ! strcmp( raquette[i][j], "e" ) )     { suivi->pas_est   = 1 ; }
          if ( ! strcmp( raquette[i][j], "s" ) )     { suivi->pas_sud   = 1 ; }
          if ( ! strcmp( raquette[i][j], "reset" ) ) { suivi->reset   = 1 ; }
    
        }
      } 
    
      GPIO_SET( GPIO_KEY_C[i], 0) ;  
    }
    /* Trace("%ld %ld %ld %ld\n", \
      suivi->pas_ouest, \
      suivi->pas_est, \
      suivi->pas_nord, \
      suivi->pas_sud);
    */
  }
}
//==========================================================
void GPIO_STATUT(void) {

  int i ;
  
  printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
      
  for(i=0;gpio_in[i]>0 ;i++) 
    printf("statut de la broche gpio_in[%3d]=%3d ==> %3d\n", i,\
      gpio_in[i], GPIO_GET( gpio_in[i] ) ) ;
  
  printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
}
//====================================================================================================
int GPIO_GET(int gpio) {
  
  char buf[BUFFER] ;
  int  val ;
  memset(buf,0,BUFFER);
  
  if ( GPIO_OPEN_STATUT == 0 ) {
    if(pread(gpio_fd[gpio],buf,BUFFER,0)>0)
      if(sscanf(buf,"%d",&val)==1)
        return val ;
  }
  else {
    Trace2("GPIO non dispos : %d\n",gpio) ;
  }
  return 0 ;
}
//====================================================================================================
int GPIO_SET(int gpio,int val) {
  
  char buf[BUFFER] ;
  int retour ;
  retour=0 ;
  memset(buf,0,BUFFER);
  snprintf(buf,BUFFER,"%d\n",val) ;

 /* PWM :   pwrite( ph->gpio_fd,buf0,strlen(buf0),0) ; */

  if ( GPIO_OPEN_STATUT == 0 ) {
     retour=pwrite(gpio_fd[gpio],buf,strlen(buf),0) ;
  }
  else {
    Trace2("GPIO non dispo : simulation : gpio %d set %d\n",gpio_fd[gpio], val) ;
  }
  return retour ;
}
//====================================================================================================
int GPIO_OPEN_BROCHE(int gpio,int output) {
  
  FILE *f ;
  
  char dir[BUFFER] ;
  char val[BUFFER] ;
  char exp[BUFFER] ;
  
  GPIO_OPEN_STATUT=0 ;
  
  memset(exp,0,BUFFER);
  memset(dir,0,BUFFER);
  memset(val,0,BUFFER);
  
  memset(gpio_fd,0,GPIO_SIZE*sizeof(int));
    
  sprintf(exp,"%s/export",GPIO_PATH) ;
  sprintf(dir,"%s/gpio%d/direction",GPIO_PATH,gpio) ;
  sprintf(val,"%s/gpio%d/value",GPIO_PATH,gpio) ;
  
  TRACE1("exp=%s",exp);
  TRACE1("dir=%s",dir);
  TRACE1("val=%s",val);
    
  if ( output ) {
    
    TRACE("gpio ouverture en OUTPUT : dir= %s exp=%s val=%s",dir,exp,val);
    
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -1  ; }
    else if ((fprintf(f,"%d\n",gpio))<=0)                 { GPIO_OPEN_STATUT = -2  ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -3  ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -4  ; }
    else if ((fprintf(f,"out\n")<=0))                     { GPIO_OPEN_STATUT = -5  ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -6  ; }
    else if ((gpio_fd[ gpio ]=open(val, O_WRONLY))<0)     { GPIO_OPEN_STATUT = -7  ; }
  }
  else {

    TRACE("gpio ouverture en INPUT : dir=%s exp=%s val=%s",dir,exp,val);
    
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -8   ; }
    else if ((fprintf(f,"%d\n",gpio))<=0)                 { GPIO_OPEN_STATUT = -9   ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -10  ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -11  ; }
    else if ((fprintf(f,"in\n")<=0))                      { GPIO_OPEN_STATUT = -12  ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -13  ; }
    else if ((gpio_fd[ gpio ]=open(val, O_RDONLY))<0)     { GPIO_OPEN_STATUT = -14  ; }
  }
  
  return GPIO_OPEN_STATUT ;
}
//====================================================================================================
// modifier / finir :
// faire fonction close_broche_pwm

void GPIO_OPEN_BROCHE_PWM(GPIO_PWM_PHASE *gpwm) {
  
  FILE *f ;
  char dir[BUFFER] ;
  char val[BUFFER] ;
  char exp[BUFFER] ;
  char unexport[BUFFER] ;
  
  memset(unexport,0,BUFFER);
  memset(exp,0,BUFFER);
  memset(dir,0,BUFFER);
  memset(val,0,BUFFER);
  
  sprintf(unexport,"%s/unexport",GPIO_PATH) ;
  //close( gpwm->gpio_fd ) ;
  
  gpwm->gpio_open_statut = 0 ;
  
  if ((f=fopen(unexport,"w")) == NULL )        { gpwm->gpio_open_statut = -1 ;}
  else if ((fprintf(f,"%d\n",gpwm->gpio))<=0)  { gpwm->gpio_open_statut = -2 ;}
  else if ((fclose(f)) != 0 )                  { gpwm->gpio_open_statut = -3 ;}
  
  gpwm->gpio_open_statut = 0 ;
  
  gpwm->gpio_fd=0;
    
  sprintf(exp,"%s/export",          GPIO_PATH ) ;
  sprintf(dir,"%s/gpio%d/direction",GPIO_PATH, gpwm->gpio) ;
  sprintf(val,"%s/gpio%d/value",    GPIO_PATH, gpwm->gpio) ;
  
  Trace1("exp=%s",exp);
  Trace1("dir=%s",dir);
  Trace1("val=%s",val);
    
  if ((f=fopen(exp,"w")) == NULL )                      { gpwm->gpio_open_statut = -4  ; }
  else if ((fprintf(f,"%d\n",gpwm->gpio))<=0)           { gpwm->gpio_open_statut = -5  ; }
  else if ((fclose(f)) != 0 )                           { gpwm->gpio_open_statut = -6  ; }
  else if ((f=fopen(dir,"w")) == NULL)                  { gpwm->gpio_open_statut = -7  ; }
  else if ((fprintf(f,"out\n")<=0))                     { gpwm->gpio_open_statut = -8  ; }
  else if ((fclose(f)!=0))                              { gpwm->gpio_open_statut = -9  ; }
  else if ((gpwm->gpio_fd =open(val, O_WRONLY))<0)      { gpwm->gpio_open_statut = -10  ; }
  
  // FIXME : synthese traces sur une ligne (2021)
  Trace("open gpio %d : dir= %s exp=%s val=%s sta=%d",\
    gpwm->gpio,\
    dir,\
    exp,\
    val,\
    gpwm->gpio_open_statut);
  
  if ( gpwm->gpio_open_statut != 0 ) {
    SyslogErrFmt("ouverture du gpio %d en erreur %d",gpwm->gpio,gpwm->gpio_open_statut);
    Trace("ouverture du gpio %d en erreur %d",gpwm->gpio,gpwm->gpio_open_statut)
  }
}
//====================================================================================================
int GPIO_OPEN(int gpio_in[GPIO_SIZE],int gpio_out[GPIO_SIZE]) {
  FILE *f ;
  char exp[BUFFER] ;
  char dir[BUFFER] ;
  char val[BUFFER] ;
  char err[BUFFER] ;
  int i ;
  
  i=-1;
  
  GPIO_OPEN_STATUT=0 ;
  
  while(gpio_out[++i]>0) {
  
    memset(err,0,BUFFER) ;
    memset(exp,0,BUFFER);
    memset(dir,0,BUFFER);
    memset(val,0,BUFFER);
    
    sprintf(exp,"%s/export",GPIO_PATH) ;
    sprintf(dir,"%s/gpio%d/direction",GPIO_PATH,gpio_out[i]) ;
    sprintf(val,"%s/gpio%d/value",GPIO_PATH,gpio_out[i]) ;
    /*
    printf("exp=%s\n",exp);
    printf("dir=%s\n",dir);
    printf("val=%s\n",val);
    */
    //sleep(1);
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -1 ; break ; }
    else if ((fprintf(f,"%d\n",gpio_out[i]))<=0)          { GPIO_OPEN_STATUT = -2 ; break ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -3 ; break ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -4 ; break ; }
    else if ((fprintf(f,"out\n")<=0))                     { GPIO_OPEN_STATUT = -5 ; break ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -6 ; break ; }
    else if ((gpio_fd[gpio_out[i]]=open(val,O_WRONLY))<0) { GPIO_OPEN_STATUT = -7 ; break ; }
  }
  
  i=-1;
  
  if ( GPIO_OPEN_STATUT == 0 )
   while(gpio_in[++i]>0) {
    
    memset(dir,0,BUFFER);
    memset(exp,0,BUFFER);
    memset(val,0,BUFFER);
    
    sprintf(exp,"%s/export",GPIO_PATH) ;
    sprintf(dir,"%s/gpio%d/direction",GPIO_PATH,gpio_in[i]) ;
    sprintf(val,"%s/gpio%d/value",GPIO_PATH,gpio_in[i]) ;
    
    printf("gpio ouverture en INPUT : exp=%s val=%s\n",exp,val);
    
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -8 ; break ; }
    else if ((fprintf(f,"%d\n",gpio_in[i]))<=0)           { GPIO_OPEN_STATUT = -9 ; break ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -10 ; break ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -11 ; break ; }
    else if ((fprintf(f,"in\n")<=0))                      { GPIO_OPEN_STATUT = -12 ; break ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -13 ; break ; }
    else if ((gpio_fd[gpio_in[i]]=open(val,O_RDONLY))<0)  { GPIO_OPEN_STATUT = -14 ; break ; }
   }
  
  return GPIO_OPEN_STATUT ;
}
//====================================================================================================
int GPIO_CLOSE_BROCHE(int gpio) {
  
  FILE *f ;
  char une[BUFFER] ;
  
  memset(une,0,BUFFER);
  sprintf(une,"%s/unexport",GPIO_PATH) ;

  Trace("fermeture GPIO %d", gpio_fd [ gpio ] ) ;

  if ((f=fopen(une,"w")) == NULL )   { return -1 ;}
  if ((fprintf(f,"%d\n",gpio))<=0) { return -2 ;}
  if ((fclose(f)) != 0 ) { return -3 ;}

  return 0 ;
}
//====================================================================================================
int GPIO_CLOSE(int gpio_in[GPIO_SIZE],int gpio_out[GPIO_SIZE]) {
  FILE *f ;
  char une[BUFFER] ;
  int i ;
  
  i=-1;
  while(gpio_out[++i]>0) {
    memset(une,0,BUFFER);
    sprintf(une,"%s/unexport",GPIO_PATH) ;
    close( gpio_fd [ gpio_out [ i ] ] ) ;
    if ((f=fopen(une,"w")) == NULL )   { return -1 ;}
    if ((fprintf(f,"%d\n",gpio_out[i]))<=0) { return -2 ;}
    if ((fclose(f)) != 0 ) { return -3 ;}
  }
  i=-1;
  while(gpio_in[++i]>0) {
    memset(une,0,BUFFER);
    sprintf(une,"%s/unexport",GPIO_PATH) ;
    close( gpio_fd [ gpio_in [ i ] ] ) ;
    if ((f=fopen(une,"w")) == NULL )   { return -4 ;}
    if ((fprintf(f,"%d\n",gpio_in[i]))<=0) { return -5 ;}
    if ((fclose(f)) != 0 ) { return -6 ;}
  }
  return 0 ;
}
//==========================================================
void GPIO_SET_ALT(int dir, int slp, int clk, int rst, int mmm, int ena) {

  GPIO_SET( GPIO_DIR_ALT,dir);   
  GPIO_SET( GPIO_CLK_ALT,slp);      
  GPIO_SET( GPIO_SLP_ALT,clk);     
  GPIO_SET( GPIO_RST_ALT,rst);     
  GPIO_SET( GPIO_MMM_ALT,mmm);      
  GPIO_SET( GPIO_ENA_ALT,ena);  
}
//==========================================================
void GPIO_SET_AZI(int dir, int slp, int clk, int rst, int mmm, int ena) {

  GPIO_SET( GPIO_DIR_AZI,dir);   
  GPIO_SET( GPIO_CLK_AZI,slp);      
  GPIO_SET( GPIO_SLP_AZI,clk);     
  GPIO_SET( GPIO_RST_AZI,rst);     
  GPIO_SET( GPIO_MMM_AZI,mmm);      
  GPIO_SET( GPIO_ENA_AZI,ena);  
}
//==========================================================
void GPIO_MOVE_1(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) {
  
  unsigned long demi_periode ;
  unsigned long incr ;
  
  //periode_micro = ( periode * GPIO_MICRO_SEC ) ;
  demi_periode = (long) (periode * GPIO_MICRO_SEC / 2.0) ;
  
  printf("deplacement sens=%d / demi_periode(us)=%ld / nombre impulsions=%f\n",sens ,demi_periode , nb_pulse) ;
  
  incr=0 ;
  
  if (sens <=0) GPIO_SET( gpio_dir, 0 );
  else          GPIO_SET( gpio_dir, 1 );
  
  // nb_pulse = 10000 ;  
  
  while( ++incr < (unsigned long)nb_pulse ) {
    //printf("deplacement sens=%d / demi_periode(us)=%ld / nombre impulsions=%f\n",sens ,demi_periode , nb_pulse) ;
    usleep( demi_periode) ;
    GPIO_SET( gpio_clk, 0 ) ;
    
    usleep( demi_periode ) ;
    GPIO_SET( gpio_clk, 1 ) ;
  }
  printf("Fin GPIO_MOVE_1\n ") ;
}
//==========================================================
void GPIO_MOVE_2(int sens, double periode,unsigned  long nb_pulse) {
  
  double periode_micro ;

  unsigned long incr ;
  
  incr=0 ;
  
  if (sens <=0) {
    GPIO_SET( GPIO_DIR_ALT, 0 );
    GPIO_SET( GPIO_DIR_AZI, 0 );
  }
  else {
    GPIO_SET( GPIO_DIR_ALT, 1 );
    GPIO_SET( GPIO_DIR_AZI, 1 );
  } 
  periode_micro = ( periode * GPIO_MICRO_SEC ) ;
  
  while( ++incr < nb_pulse+1 ) {
  
    usleep( (long) (periode_micro / 2.0) ) ;
    
    GPIO_SET( GPIO_CLK_ALT, 0 );
    GPIO_SET( GPIO_CLK_AZI, 0 );
    
    usleep( (long) (periode_micro / 2.0) ) ;
    
    GPIO_SET( GPIO_CLK_ALT, 1 );
    GPIO_SET( GPIO_CLK_AZI, 1 );
  }
}
//==========================================================
void GPIO_MOVE_3(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) {
  
  double         demi_periode_sleep ;
  double         t_diff, nbpulse ;
  struct timeval t00,t01 ;
  double         duree_microsec ;
  
  gettimeofday(&t00,NULL) ;
  
  duree_microsec       =  periode * GPIO_MICRO_SEC ;    
  demi_periode_sleep   = periode * GPIO_MICRO_SEC / 2.2 ;
  
  printf("deplacement sens=%d / demi_periode(us)=%f / nombre impulsions=%f\n",sens ,demi_periode_sleep	 , nb_pulse) ;
    
  if (sens <=0) GPIO_SET( gpio_dir, 0 );
  else          GPIO_SET( gpio_dir, 1 );
    
  nbpulse=0 ;
  
  while( nbpulse < nb_pulse) {
    
    t_diff=0 ; 
    nbpulse = nbpulse +1 ;
    
    usleep( demi_periode_sleep) ;
    GPIO_SET( gpio_clk, 0 ) ;
    
    usleep( demi_periode_sleep ) ;
    GPIO_SET( gpio_clk, 1 ) ;
      
    while( t_diff < duree_microsec ) {
      t_diff = (( t01.tv_sec - t00.tv_sec ) * GPIO_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
      gettimeofday(&t01,NULL) ;
    }
    gettimeofday(&t00,NULL) ;
  }
  printf("Fin GPIO_MOVE_3\n ") ;
}
//====================================================================================================================
long GPIO_ACCELERATION_1(int gpio, double f_deb,double f_fin, double delai,long nano_moins)
{
  long   incr, demi_slp ;
  double per_deb, per_fin, incr_per, nb_pulsation_totales ;
  struct timespec nt,ntt ;
  
  per_deb = 1/f_deb ;
  per_fin = 1/f_fin ;
  
  nb_pulsation_totales = delai * ( f_deb + f_fin ) / 2.0 ;
  
  if( per_deb > per_fin )  incr_per = per_deb * ( per_deb - per_fin ) / ( delai + per_fin + per_deb ) ; 
  else                     incr_per = per_fin * ( per_fin - per_deb ) / ( delai + per_deb + per_fin ); 
  
  incr=0 ;
  if ( per_deb < per_fin )
  while( per_deb < per_fin ) {
    demi_slp = (long)((double)(GPIO_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(gpio,0) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(gpio,1) ;  
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  if ( per_deb > per_fin )
  while( per_deb > per_fin ) {
    demi_slp = (long)((double)(GPIO_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(gpio,0) ;  
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(gpio,1) ;  
    nanosleep(&nt,&ntt) ;
    
    per_deb -= incr_per ;
    incr++ ;
  }
  if ( per_deb == per_fin )
  while( incr < nb_pulsation_totales ) {
    demi_slp = (long)((double)(GPIO_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(gpio,0) ;  
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(gpio,1) ;  
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  return incr ;
}
//==========================================================
long GPIO_ACCELERATION_2(int alt, int azi, double f_deb,double f_fin, double delai,long nano_moins)
{
  long   incr, demi_slp ;
  double per_deb, per_fin, incr_per, nb_pulsation_totales ;
  struct timespec nt,ntt ;
  
  per_deb = 1/f_deb ;
  per_fin = 1/f_fin ;
  
  nb_pulsation_totales = delai * ( f_deb + f_fin ) / 2.0 ;
  
  if( per_deb > per_fin )  incr_per = per_deb * ( per_deb - per_fin ) / ( delai + per_fin + per_deb ) ; 
  else                     incr_per = per_fin * ( per_fin - per_deb ) / ( delai + per_deb + per_fin ); 
  
  incr=0 ;
  if ( per_deb < per_fin )
  while( per_deb < per_fin ) {
    demi_slp = (long)((double)(GPIO_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(alt,1) ; GPIO_SET(azi,1) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(alt,0) ; GPIO_SET(azi,0) ; 
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  if ( per_deb > per_fin )
  while( per_deb > per_fin ) {
    demi_slp = (long)((double)(GPIO_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(alt,1) ; GPIO_SET(azi,1) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(alt,0) ; GPIO_SET(azi,0) ; 
    nanosleep(&nt,&ntt) ;
    
    per_deb -= incr_per ;
    incr++ ;
  }
  if ( per_deb == per_fin )
  while( incr < nb_pulsation_totales ) {
    demi_slp = (long)((double)(GPIO_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(alt,1) ; GPIO_SET(azi,1) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(alt,0) ; GPIO_SET(azi,0) ; 
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  return incr ;
}
// ##########################################################################################################
double GPIO_FONCTION_RAPPORT_CYCLIQUE(int num_bobine, double t, GPIO_PWM_MOTEUR *pm) {
  
  double rc  ;         // rapport cyclique a calculer
  double d = PI/2.0 ;  // dephasage entre bobines
  double w = 2*PI*pm->Fm ; // pulsation du courant dans les bobines
  double sigc, trc, sc, dc , pi = PI ;
  rc = 0 ;

  if ( num_bobine == 0 ) { 
    
    TRACE1("%f %f %f %d %f %f", t, w, d, pm->type_fonction, pm->param0, pm->param1 ) ;
    
    switch ( pm->type_fonction) {  
     case 1 : rc = pm->param1 * sin(w*t) ;                                      TRACE1("fonction sinusoidale") ;break;
     case 0 : rc = pm->param1 * acos(cos(2*w*t))/(PI/2)-1 ;                     TRACE1("fonction triangulaire") ;break ;
     case 2 : rc = pm->param1 * sin(w*t)+SGN(cos(w*t))* pm->param0 *sin(4*w*t) ;TRACE1("fonction mixte") ;break;
     case 3 : 
// a=0.5 b=2
// a=0.6 b=1.66
// a=0.7 b=1.43
// a=0.9 b=1.1
// a=1.0 b=1.0
       sigc = SGN(sin(w*t)) ;
       trc  = -(1+pm->param0)*(acos(cos(w*t))/(pi/2)-1)	 ;
       sc   = cos(w*t) ;
       if (pm->param0 != 0 ) dc   = ((-sc*(trc-sc))+(pm->param0))/pm->param0 ;
       else                  dc   = (-sc*(trc-sc))+(pm->param0) ;
       rc   = pm->param1 *  dc * sigc ;
       TRACE1("fonction la meilleure pour les micro pas") ;
       break ;
    }
  }
  else {
    switch ( pm->type_fonction) {  
     case 1 : rc = pm->param1 * sin(w*t+d) ;                                         TRACE1("fonction sinusoidale") ;break;
     case 0 : rc = pm->param1 * acos(cos(2*w*t+d))/(PI/2)-1 ;                        TRACE1("fonction triangulaire") ;break;
     case 2 : rc = pm->param1 * sin(w*t+d)+SGN(cos(w*t+d))* pm->param0 *sin(4*w*t) ; TRACE1("fonction mixte") ;break;
     case 3 : 
       sigc = SGN(cos(w*t)) ;
       trc  = -(1+pm->param0)*(acos(cos(w*t-pi/2))/(pi/2)-1)	 ;
       sc   = sin(w*t) ; 
       if (pm->param0 != 0 ) dc   = ((-sc*(trc-sc))+(pm->param0))/pm->param0 ;
       else                  dc   = (-sc*(trc-sc))+(pm->param0) ;
       
       rc   = pm->param1 * dc * sigc ;
       TRACE1("fonction la meilleure pour les micro pas") ;
       break ;
    }
  }
  
  return rc ;

  // BOBINE1
  // rc = sin( 2 * PI * pm->Fm * t ) ;
  // rc = sin(2*PI*pm->Fm*t)+SGN(cos(2*PI*pm->Fm*t))*0.15*sin(2*PI*4*pm->Fm*t) ;
  // rc = acos(cos(2*PI*pm->Fm*2*t))/(PI/2)-1 ; // fonction triangulaire

  // BOBINE2
  // rc = -1.0 * cos( 2 * PI * pm->Fm * t ) ;
  // rc = sin(2*PI*pm->Fm*t+PI/2)+SGN(cos(2*PI*pm->Fm*t+PI/2))*0.15*sin(2*PI*4*pm->Fm*t) ;
  // rc =  acos(cos(2*PI*pm->Fm*2*t+PI/2))/(PI/2)-1 ; // fonction triangulaire
}
// ##########################################################################################################
void GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(GPIO_PWM_MOTEUR *pm) {
  
  int i, j;
  double t  ;
  double rc  ;         // rapport cyclique a calculer

  j=0 ;
  
  for( i=0 ; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) 
    for( j=0;j < GPIO_MICROPAS_MAX; j++ )
      pm->phase[i]->rap[j] = 0 ;
    
  j=0 ; 

  Trace1("pm->periode_mic = %f", pm->periode_mic ) ;

  for( t=0 ; t< pm->Tm ;  t += pm->periode_mic ) {
    
    Trace1("t = %f\t%f\t%f",t,pm->Tm ,pm->periode_mic) ;
    
    rc = GPIO_FONCTION_RAPPORT_CYCLIQUE(0, t, pm )  ;
    if ( rc >  0)  { 
      pm->phase[1]->rap[j] = rc ;        
      pm->phase[0]->rap[j] = 0.0 ; 
    }
    else  { 
      pm->phase[0]->rap[j] = rc * -1.0 ; 
      pm->phase[1]->rap[j] = 0.0 ; 
    }

    rc = GPIO_FONCTION_RAPPORT_CYCLIQUE(1, t, pm )  ;    

    if ( rc >  0)  { 
      pm->phase[3]->rap[j] = rc ;        
      pm->phase[2]->rap[j] = 0.0 ; 
    }
    else { 
      pm->phase[2]->rap[j] = rc * -1.0 ; 
      pm->phase[3]->rap[j] = 0.0 ; 
    }
 
    j++ ;
  }
  for( i=0 ; i< j ; i++ ) {
    if( ( pm->phase[0]->rap[i]+ \
          pm->phase[1]->rap[i]+ \
	  pm->phase[2]->rap[i]+ \
	  pm->phase[3]->rap[i] ) != 0 )

    Trace1("%d\t%f\t%f\t%f\t%f",i,pm->phase[0]->rap[i],pm->phase[1]->rap[i],pm->phase[2]->rap[i],pm->phase[3]->rap[i]) ;
  }
}
// ##########################################################################################################
// Cette fonction, pour chaque phase, en parallele dans 4 threads de meme priorite,
// module les etats hauts et bas de la phase
// avec les rapports cycliques calcules pour chaque micro pas
// et suivant une frequence fPWM superieure a la frequence max potentielle de deplacement des axes
/* @date   : 2022-05-24 ajout protection par mutex de suivi−>p_threads_id[ g_id_thread++ ] */
// ##########################################################################################################

void * GPIO_SUIVI_PWM_PHASE(GPIO_PWM_PHASE *ph ) {
  
  char   buf0[BUFFER] ;
  char   buf1[BUFFER] ;
  double TUpwm, TUpwm_haut, TUpwm_bas, rap ;

  struct sched_param param;
  
  Trace("start") ;

  if ( priority > 0) {
    param.sched_priority = PTHREAD_POLICY_10 ;  
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_PWM_PHASES, & param) != 0) {
      perror("GPIO_SUIVI_PWM_PHASE");
      exit(EXIT_FAILURE);}
  }

  pthread_mutex_lock( & suivi->mutex_pthread) ;
  pthread_setname_np( pthread_self(), "gpio_suivi_pwm" ) ;
  ph->suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
  pthread_mutex_unlock( & suivi->mutex_pthread) ;

  TUpwm = TUpwm_haut  = TUpwm_bas = rap = 0 ;

  memset(buf0,0,BUFFER); snprintf(buf0,BUFFER,"%d\n",0) ;
  memset(buf1,0,BUFFER); snprintf(buf1,BUFFER,"%d\n",1) ;
 
  while(1) {
    
    pthread_mutex_lock( & ph->mutex ) ;
    
      rap   = ph->rap[ ph->micropas ] ;
      TUpwm = (double)GPIO_MICRO_SEC * ph->Tpwm ;
    
      Trace2("id phase = %d\tpas_en_cours=%d \trapport_cyclique=%f",ph->id,ph->micropas, ph->rap[ ph->micropas ] ) ;
    
    pthread_mutex_unlock( & ph->mutex ) ;
    
    TUpwm_haut = TUpwm * rap  ; 
    TUpwm_bas  = TUpwm - TUpwm_haut ;
    
    // =================================================================
    // ecriture sur GPIO
    // si la periode moteur est trop longue (> 1 seconde) 
    // on n'attend pas et on n'ecrit pas sur les GPIOS
    // Cela evite de mettre le thread moteur en etat d'attente dans un usleep long
    // =================================================================

    if ( rap > 0 )
      if ( ph->periode_mot < GPIO_SUIVI_MAIN_ATTENTE_MAX )
        pwrite( ph->gpio_fd,buf1,strlen(buf1),0) ;

    usleep( TUpwm_haut ) ;  

    pwrite( ph->gpio_fd,buf0,strlen(buf0),0) ;
    
    usleep( TUpwm_bas ) ;
  }
}
// ##########################################################################################################
/* 24 mai 2022 : correction pm->temps += pm->periode ; 
                            pm->temps += pm->periode_mot ; 
                            */
void * suivi_main_M(GPIO_PWM_MOTEUR *pm) {

  int i=0 , sens=0 ;
  double periode_mic=0 ;
  double periode_mot=0 ;
  double  Tpwm = 0; 
  //double tdiff=0 ;
  long   i_incr=0 ; 
  struct sched_param param;
  int     pas=0 ;
  TRACE("start") ;

  if ( priority > 0) {
    param.sched_priority = PTHREAD_POLICY_5 ;  
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_PWM_MAIN, & param) != 0) {
      perror("suivi_main_M");
      exit(EXIT_FAILURE);
    }
  }

  pthread_mutex_lock( & pm->suivi->mutex_pthread) ;
  pthread_setname_np( pthread_self(), "suivi_main_m" ) ;
  pm->suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
  pthread_mutex_unlock( & pm->suivi->mutex_pthread) ;

  pm->micropas = 0 ;
  pm->pas = 0 ;
  pm->t = 0 ;
  periode_mic = pm->periode_mic ;
  periode_mot = pm->periode_mot ;

  gettimeofday(&pm->tval,NULL) ; 

  while (1) {

    /* En fonction de l'id moteur (ALT ou AZI) 
        on recopie les donnees de suivi* sur les structures moteurs */

    switch ( pm->id ) {

      case 0 : 
        pthread_mutex_lock( & pm->suivi->mutex_alt ) ;

          periode_mic         = pm->suivi->Th   ;
          periode_mot         = pm->suivi->Th_mot ;
          sens                = pm->suivi->Sh ;
          /*pm->suivi->temps_h  = pm->temps ;*/

        pthread_mutex_unlock( & pm->suivi->mutex_alt ) ;
        break ;

      case 1 :
          pthread_mutex_lock( & pm->suivi->mutex_azi ) ;

          periode_mic         = pm->suivi->Ta   ; 
          periode_mot         = pm->suivi->Th_mot ;
          sens                = pm->suivi->Sa ;
          /*pm->suivi->temps_a  = pm->temps  ;*/

        pthread_mutex_unlock( & pm->suivi->mutex_azi ) ;
        break ;
    }

    //TRACE("periode_mic=%f",periode_mic) ;
    // correction bug 26/08/2016 :
    // incorrect : if ( sens > 0 ) { if ( pm->micropas == pm->nbmicropas ) { pm->micropas = 0 ; }         else { pm->micropas++ ; }}
    /* reinitialisation du compteur de micro pas lorsque max atteint */

    /* exemple avec upas = 64 , cela permet de boucler de 0 a 63 */
    
    pthread_mutex_unlock( & pm->mutex ) ;

    pthread_mutex_lock( & pm->mutex ) ;

    if ( sens > 0 ) { 
      if ( pm->micropas == pm->nbmicropas - 1 ) { 
        pm->micropas = 0 ; 
        /* Si pm->micropas = 0 , cela veut dire que tout le cycle de micro pas a ete ecoule */
        pm->pas ++ ;
        pm->temps_reel += CALCUL_DUREE_SECONDES( & pm->tval ) ;
        // Trace("%lld %d %d ", pm->pas, g_i_trace, (int)(llabs(pm->pas) % g_i_trace)) ;

        if ( g_i_trace>0 && ( (int)fabs(pm->temps) % g_i_trace ) == 0 ) {
          Trace("bou %-5ld mot %-5d pas %-5lld micropas %-5d : per_mic %f pm->per_mic %f per_mot %f tps %.1f tps_reel %.1f",\
            i_incr,    \
            pm->id,    \
            pm->pas,   \
            pm->micropas,   \
            periode_mic,    \
            pm->periode_mic,    \
            periode_mot,    \
            pm->temps , \
            pm->temps_reel  \
          ) ;
        }
        /* Sortie du programme si max nb pas atteint */

        if ( g_i_max_nb_pas >0 && pm->pas > g_i_max_nb_pas ) {
          GPIO_TRAP_SIG(0);
        }
      }
      else { 
        pm->micropas++ ; 
      }
    }
    else { 
      if ( pm->micropas == 0 ) { 
        pm->micropas = pm->nbmicropas-1 ;
        /* Si pm->micropas = pm->nbmicropas-1 , cela veut dire que tout le cycle de micro pas a ete ecoule */
        pm->pas -- ;

        // Trace("%lld %d %d ", pm->pas, g_i_trace, (int)(llabs(pm->pas) % g_i_trace)) ;
        pm->temps_reel += CALCUL_DUREE_SECONDES( & pm->tval ) ;

        if ( g_i_trace>0 && ( (int)fabs(pm->temps) % g_i_trace ) == 0 ) {
          Trace("bou %-5ld mot %-5d pas %-5lld micropas %-5d : per_mic %f pm->per_mic %f per_mot %f tps %.1f tps_reel %.1f",\
            i_incr,    \
            pm->id,    \
            pm->pas,   \
            pm->micropas,   \
            periode_mic,    \
            pm->periode_mic,    \
            periode_mot,    \
            pm->temps,  \
            pm->temps_reel  \
          ) ;
        }
      } 
      else { 
        pm->micropas-- ; 
      }
    }
    pas  =  pm->micropas ;
    Tpwm =  pm->Tpwm ;

    pthread_mutex_unlock( & pm->mutex ) ;

    //printf("%d\t",pm->micropas) ;

    // On recopie les devices de la structure moteur qui vont servir dans 
    // la structure phase (4 phases a priori dans un moteur pas a pas)

    for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {

      pthread_mutex_lock( & pm->phase[i]->mutex ) ;

        pm->phase[i]->micropas    = pas ;
        pm->phase[i]->Tpwm        = Tpwm ;
        pm->phase[i]->rc          = pm->phase[i]->rap[ pm->micropas ] ;
        pm->phase[i]->periode_mot = periode_mot ; 
        pm->phase[i]->periode_mic = periode_mic ; 

      pthread_mutex_unlock( & pm->phase[i]->mutex ) ;
      //printf("\t%f",pm->phase[i]->rc ) ;
    }

    // si periode_mic est trop grand (>GPIO_SUIVI_MAIN_ATTENTE_MAX) on ne peut pas attendre plus ..
    // on attend GPIO_SUIVI_MAIN_ATTENTE_MAX maximum (c est un seuil d attente maximum donc)

    if (periode_mot < GPIO_SUIVI_MAIN_ATTENTE_MAX ) {

      usleep( periode_mic * GPIO_MICRO_SEC ) ;
      /* on met a jour le temps attendu pour le moteur */

      pthread_mutex_lock(  & pm->mutex ) ;
      pm->temps += periode_mic ;
      pthread_mutex_unlock(& pm->mutex ) ;
    }
    else {
      /*
      usleep( GPIO_SUIVI_MAIN_ATTENTE_MAX * GPIO_MICRO_SEC * 0.1 ) ; 
      pthread_mutex_lock(  & pm->mutex ) ;
      pm->temps += GPIO_SUIVI_MAIN_ATTENTE_MAX * 0.1 ;
      pthread_mutex_unlock(& pm->mutex ) ;
      */
      Trace("depassement periode") ;
    }

    // FIXME : calcule la duree reelle d une iteration dans la boucle

    //tdiff = CALCUL_DUREE_MICROSEC( &pm->tval ) ; 
    i_incr++ ;
  } // FIXME : fin boucle while 
}
// ##########################################################################################################
/* mai 2022 : ajout pm->phase[i]->suivi            = pm->suivi ; */
/* mai 2022 :  */

void GPIO_INIT_PWM_MOTEUR(GPIO_PWM_MOTEUR *pm, int gpios[ GPIO_NB_PHASES_PAR_MOTEUR ], int masque[ GPIO_NB_PHASES_PAR_MOTEUR ],  double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1) {

  int i ;
  char cmd[255] ;
  
  pthread_mutex_init( & pm->mutex, NULL ) ;
  
  memset(cmd,'\0',sizeof(cmd)) ;

  pm->type_fonction = type_fonction ;
  pm->param0        = param0 ;
  pm->param1        = param1 ;
  pm->id            = id ;
  pm->Fpwm          = fpwm ;
  pm->Fm            = fm / 8 ;
  pm->nbmicropas    = upas ;
  pm->Tm            = 1 / pm->Fm ;
  pm->Tpwm          = 1 / pm->Fpwm ;
  pm->nbdeltat      = (long)(pm->Fm * pm->nbmicropas) ;
  pm->periode_mot   =  1 / ( pm->Fm  ) ;
  pm->periode_mic   =  1 / ( pm->Fm * pm->nbmicropas ) ;

  pm->temps            = 0 ; 
  pm->temps_reel       = 0 ; 
  pm->temps_moyen      = 0 ; 
  pm->temps_reel_moyen = 0 ; 

  gettimeofday(&pm->tval,NULL) ;

  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    
    pm->phase[i]=(GPIO_PWM_PHASE *)malloc(sizeof(GPIO_PWM_PHASE)) ;
    
    pthread_mutex_init( & pm->phase[i]->mutex, NULL ) ;
    
    pm->phase[i]->suivi            = pm->suivi ;
    pm->phase[i]->gpio             = gpios[ masque[i] ]  ;
    pm->phase[i]->id               = i ;
    pm->phase[i]->Tpwm             = 0 ;
    pm->phase[i]->micropas         = 0 ;
    pm->phase[i]->gpio_open_statut = 0 ;
    pm->phase[i]->gpio_fd          = 0 ;
    
    GPIO_OPEN_BROCHE_PWM( pm->phase[i] ) ;
    /*
    if ( pm->phase[i]->gpio_open_statut < 0 )  {
      sprintf(cmd,"printf \"GPIO_OPEN_BROCHE_PWM : ouverture gpio %d = %d\" > /tmp/astrokit.log", i, pm->phase[i]->gpio_open_statut ) ;
      perror(cmd) ;
      system(cmd) ;
    }*/
    /* TRACE("") ; */
    usleep(100000) ;
  }
  GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES( pm )  ;
}
// ======================================= suivi clavier ===================================================
void * suivi_clavier() {
  printf("------ %c -------\n", getc(stdin));
  return NULL ;
}
// ##########################################################################################################
// Main10 : Idem main9 mais avec choix du NOMBRE DE MOTEURS (pour test sur raspi b+ = un CPU)
//          ET choix de la fonction de rapport cyclique
// ##########################################################################################################

/*****************************************************************************************
* @fn     : main(Gpio)xxx : main
* @author : s.gravois
* @brief  : point entree du programme GPIO (se concentre sur le test des moteurs)
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 amelioration globale du code pour pouvoir faire des mesures
* @date   : 2022-05-24  et d invoquer de facon plus simple les arguments
* @fixme  : les autres main ont ete sauvegardes dnas gpio.fonctions.utiles.c
* @todo   : amelioration continue
*****************************************************************************************/

int mainGpio(int argc, char **argv) {

  char c='0', ch='0' ;
  int nread =0 ; 
  int         i, pid, nbcpus, nbm , type_fonction;
  int         Fpwm , priority , upas ;
  char        c_gpio_alt [ 16 ] ;
  char        c_gpio_azi [ 16 ] ;
  char        c_masque [ 16 ] ;
  int         gpiosM1[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  int         gpiosM2[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  int         masque[  GPIO_NB_PHASES_PAR_MOTEUR ] ;
  float       param0, param1 ;
  float       fr_alt, fr_azi ;
  struct sched_param param ;
  int incr=0 ;
  //cpu_set_t          cpuset ;
  
  pthread_t p_phases_alt[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_phases_azi[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_mot_alt ;
  pthread_t p_mot_azi ;
  pthread_t p_thread_getc ;
  
  GPIO_PWM_MOTEUR *pm_alt , m0 ; 
  GPIO_PWM_MOTEUR *pm_azi , m1 ;

  Trace("BEGIN GPIO programme") ;  

  /* affectation / dereferencement de divers pointeurs */

  pm_alt = &m0 ;
  pm_azi = &m1 ;
  suivi =  &sui ;

  pm_alt->suivi = (SUIVI*)suivi ;   // pour permettre l'acces des membres de SUIVI dans GPIO_PWM_MOTEUR
  pm_azi->suivi = (SUIVI*)suivi ;   // pour permettre l'acces des membres de SUIVI dans GPIO_PWM_MOTEUR

  signal(SIGINT,GPIO_TRAP_SIG) ;
  signal(SIGALRM,GPIO_TRAP_SIG) ;

  CONFIG_PATH_FIND( g_Path_Cmd_Stty, "stty") ;

  Trace1("Chemin retenu : %s", g_Path_Cmd_Stty) ;

  sem_init( & gpio_sem , 0, 0 ) ;
  clock_gettime(CLOCK_REALTIME, & gpio_tm_now ) ; 

  /* Definition des valeurs par defaut */

  g_i_trace = 0 ;
  g_i_timeout = 0 ;
  g_i_max_nb_pas = 0 ;
  g_i_max_nb_micropas = 0 ;
  g_id_thread = 0 ;

  strcpy( c_gpio_alt , GPIO_DEFAULT_ALT_MOTOR_GPIOS ) ;
  strcpy( c_gpio_azi , GPIO_DEFAULT_AZI_MOTOR_GPIOS ) ;
  strcpy( c_masque ,   GPIO_DEFAULT_MAS_MOTOR_ORDER ) ;

  fr_alt        = 5 ;
  fr_azi        = 5 ;
  upas          = 100 ;
  priority      = 1 ;      
  nbm           = 0 ;
  Fpwm          = 1000 ;
  type_fonction = GPIO_CURRENT_FONCTION ;
  param0        = GPIO_CURRENT_FONCTION_PARAM0 ;
  param1        = GPIO_CURRENT_FONCTION_PARAM1 ;

   if( argc == 2 && (! strcmp(argv[1],"-h"))) { 

    Trace("Usage : %s\n\
      -L log / traces            ( exemple 1 )             - (facultatif, defaut %d) \n\
      -T timeout  (max temps)    ( exemple 1 )             - (facultatif, defaut %d) \n\
      -P pas      (max nb pas)   ( exemple 1000 )          - (facultatif, defaut %d) \n\
      -M micropas (max nb upas)  ( exemple 1000 )          - (facultatif, defaut %d) \n\
      -u nb micro pas            ( exemple 64 )            - (facultatif, defaut %d) \n\
      -g gpios moteur1           ( exemple 26,19,13,6 )    - (facultatif, defaut %s) \n\
      -G gpios moteur2           ( exemple 5,11,9,10 )     - (facultatif, defaut %s) \n\
      -m masque                  ( exemple 3,1,0,2 )       - (facultatif, defaut %s) \n\
      -f fr MOTEUR1 = alt (Hz)   ( exemple 10 )            - (facultatif, defaut %f) \n\
      -F fr MOTEUR2 = azi (Hz)   ( exemple 20 )            - (facultatif, defaut %f) \n\
      -w fr Fpwm                 ( exemple 1000 )          - (facultatif, defaut %d) \n\
      -n nb moteurs a utiliser   ( 0=1 moteur, 1=les deux) - (facultatif, defaut %d) \n\
      -c priority CPU (0-99)     ( exemple 1 )             - (facultatif, defaut %d) \n\
      -x type_fonction           ( exemple 3 = sin applati)- (facultatif, defaut %d) \n\
      -y param0 aplatissement    ( exemple 0.7 )           - (facultatif, defaut %f) \n\
      -z param1 ajustement a 5v  ( exemple 0.99 )          - (facultatif, defaut %f) \n\
    ", \
      argv[0], \
      g_i_trace, \
      g_i_timeout, \
      g_i_max_nb_pas, \
      g_i_max_nb_micropas, \
      upas, \
      c_gpio_alt, \
      c_gpio_azi, \
      c_masque, \
      fr_alt, \
      fr_azi, \
      Fpwm, \
      priority, \
      nbm, \
      type_fonction, \
      param0, \
      param1 \
    ) ;

    Trace("Commande DEUX moteurs en utilisant une modulation PWM\n") ;
    Trace("Utilisation des logs") ;
    Trace("0 : pas de logs / traces") ;
    Trace("x : affiche des stats (nb pas , temps moyen moteur, temps moyen reel, ..) par multiple de x secondes") ;

    exit(1) ;
  }
  /* GPIO_GETOPT(argc,argv) ; */

  while ((c = getopt (argc, argv, "hg:G:H:m:f:F:u:p:n:P:T:L:x:y:z:")) < 255 && c > 0) {
      incr++ ;
      switch (c) {

        case 'L':
          if ( optarg == NULL  ) {
            Trace("optarg NULL") ; 
          }
          else {
            Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
            g_i_trace = atoi(optarg) ; 
          }
          break ;  

        case 'T':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          g_i_timeout = atoi(optarg) ; 
          break ;   

        case 'P':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          g_i_max_nb_pas = atoi(optarg) ; 
          break ;   

        case 'M':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          g_i_max_nb_micropas = atoi(optarg) ; 
          break ;   
          
        case 'g':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          strcpy( c_gpio_alt , optarg ) ;
          break ;

        case 'G':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          strcpy( c_gpio_azi , optarg ) ;
          break ;       

        case 'm':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          strcpy( c_masque , optarg ) ;
          break ;    

        case 'f':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          fr_alt = atof(optarg) ; 
          break ;    

        case 'F':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          fr_azi = atof(optarg) ; 
          break ;    

        case 'u':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          upas = atoi(optarg) ; 
          break ;    

        case 'p':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          priority = atoi(optarg) ; 
          break ;    
  
        case 'n':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          nbm = atoi(optarg) ; 
          break ;    

        case 'w':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          Fpwm = atoi(optarg) ; 
          break ;    

        case 'x':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          type_fonction = atof(optarg) ; 
          break ;   

        case 'y':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          param0 = atof(optarg) ; 
          break ;   

        case 'z':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          param0 = atof(optarg) ; 
          break ;   

        default :
          Trace("c=%d %c", c,c);
          break ;
      }
  }

  /* Affichage des valeurs retenues */

  printf("\n\
    traces / log          %d \n\
    timeout               %d \n\
    gpios moteur1         %s \n\
    gpios moteur2         %s \n\
    masque                %s \n\
    fr MOTEUR1 = alt (Hz) %f \n\
    fr MOTEUR2 = azi (Hz) %f \n\
    micro pas             %d \n\
    priority CPU (0-99)   %d \n\
    nb moteurs a utiliser %d \n\
    Fpwm                  %d \n\
    type_fonction         %d \n\
    param0 aplatissement  %f \n\
    param1 ajustement a 5v%f \n\
    \n",\
    g_i_trace, \
    g_i_timeout, \
    c_gpio_alt, \
    c_gpio_azi, \
    c_masque, \
    fr_alt, \
    fr_azi, \
    upas, \
    priority, \
    nbm, \
    Fpwm, \
    type_fonction, \
    param0, \
    param1 \
  ) ;
    
  GPIO_TAB_TOKEN( gpiosM1, c_gpio_alt , "," ) ;
  GPIO_TAB_TOKEN( gpiosM2, c_gpio_azi , "," ) ;
  GPIO_TAB_TOKEN( masque,  c_masque , "," ) ;

  for(i=0;i<4;i++) Trace1("gpio %d : %d",i,gpiosM1[i]) ;
  for(i=0;i<4;i++) Trace1("gpio %d : %d",i,gpiosM2[i]) ;
  for(i=0;i<4;i++) Trace1("masq %d : %d",i,masque[i]) ;

  pm_alt->suivi->Fa_mot = (double)fr_azi ;
  pm_azi->suivi->Fh_mot = (double)fr_alt ;

  pm_alt->suivi->Fa     = pm_alt->suivi->Fa_mot * (double)upas  ;
  pm_azi->suivi->Fh     = pm_azi->suivi->Fh_mot * (double)upas ;

  pm_alt->suivi->Ta_mot = 1 / pm_alt->suivi->Fa_mot ;
  pm_azi->suivi->Th_mot = 1 / pm_azi->suivi->Fh_mot ;

  pm_alt->suivi->Ta = 1 / pm_alt->suivi->Fa ;
  pm_azi->suivi->Th = 1 / pm_alt->suivi->Fh ;

  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  Trace("ALTITUDE : periode mot = %.2f f = %.f ", pm_alt->suivi->Ta_mot, pm_alt->suivi->Fa_mot ) ; 
  Trace("AZIMUT   : periode mot = %.2f f = %.f ", pm_azi->suivi->Th_mot, pm_azi->suivi->Fh_mot ) ; 

  Trace("ALTITUDE : periode mic = %.2f f = %.f ", pm_alt->suivi->Ta, pm_alt->suivi->Fa ) ; 
  Trace("AZIMUT   : periode mic = %.2f f = %.f ", pm_azi->suivi->Th, pm_azi->suivi->Fh ) ; 
/*
  Trace("correct(y/n)?") ;
  nread = read( 0, &ch, 1)  ;
  Trace("ch = %c nread = %d", ch, nread) ;
*/
  Trace("pid processus = %d", pid) ; 
  Trace("nb cppus      = %d", nbcpus ) ; 

  if ( g_i_timeout > 0) {
    alarm( g_i_timeout ) ;
  }

  if ( priority > 0)  {
    mlockall(MCL_CURRENT | MCL_FUTURE);
    Trace("mise en place temps reel : echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    param.sched_priority = priority ;
    Trace("mise en place temps reel : param.sched_priority = %d", priority) ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  } 
 
  GPIO_INIT_PWM_MOTEUR(\
    pm_alt, \
    gpiosM1, \
    masque, \
    upas, \
    fr_alt, \
    Fpwm, \
    0, \
    type_fonction, \
    param0, \
    param1) ;

  if(nbm) {
    GPIO_INIT_PWM_MOTEUR(\
    pm_azi, \
    gpiosM2, \
    masque, \
    upas, \
    fr_azi, \
    Fpwm, \
    1, \
    type_fonction, \
    param0, \
    param1) ;
  }
  
  // lancement des pthreads moteurs ----------------------------------------------------
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_create( &p_phases_alt[i], NULL,(void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[i] ) ;
  }
  pthread_create( &p_mot_alt, NULL, (void*)suivi_main_M, pm_alt ) ;
  
  if(nbm) {
    for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
      pthread_create( &p_phases_azi[i], NULL,(void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[i] ) ;
    }
    pthread_create( &p_mot_azi, NULL, (void*)suivi_main_M, pm_azi ) ;
  }

  // lancement des autres pthreads  ----------------------------------------------------

  pthread_create( &p_thread_getc, NULL, (void*)suivi_clavier, NULL ) ;

  CONFIG_AFFICHER_ETAT_THREADS(suivi);

  // Join des threads -------------------------------------------------------------------
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_phases_alt[i], NULL) ; 
  }
  pthread_join( p_mot_alt, NULL) ;

  if(nbm) {  
   for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
     pthread_join( p_phases_azi[i], NULL) ; 
   }
   pthread_join( p_mot_azi, NULL) ;
  }
  pthread_join( p_thread_getc, NULL) ;

  return 0 ;
}
// ##########################################################################################################
