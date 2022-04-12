#define _GNU_SOURCE
#include <gpio.h>
// ---------------------------------------------------------------------------------------
void GPIO_INIT_GPIOS_CONTROLEUR(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][DATAS_TAILLE_BUFFER]) {
  int l,i, j ;
  char *str1, *token, *sptr ;
  
  for(l=0;l<DATAS_NB_LIGNES;l++) {
   if(!strcmp("GPIO_ALT",g_Datas[l][0])) {
    for(j=0;j<GPIO_CONTROLEUR_SIZE;j++) gpio_in[j]=-1 ;
    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_alt[j]=atoi(token);
    }
   }
   if(!strcmp("GPIO_AZI",g_Datas[l][0])) {
    for(i=0; i < GPIO_CONTROLEUR_SIZE ; i++) gpio_out[i]=-1 ;
    for (j = 0, str1 = g_Datas[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gpio_azi[j]=atoi(token);
    }
   }
  }
   for(i=0;i<GPIO_CONTROLEUR_SIZE;i++) TRACE("GPIO_AZI[%d] =%d",i,gpio_azi[i]);
   for(i=0;i<GPIO_CONTROLEUR_SIZE;i++) TRACE("GPIO_ALT[%d]=%d",i,gpio_alt[i]);
}
// ---------------------------------------------------------------------------------------
void GPIO_READ(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][DATAS_TAILLE_BUFFER]) {
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
   for(i=0;i<GPIO_SIZE;i++) printf("GPIO_INPUT[%d] =%d\n",i,gpio_in[i]);
   for(i=0;i<GPIO_SIZE;i++) printf("GPIO_OUTPUT[%d]=%d\n",i,gpio_out[i]);
}
// ---------------------------------------------------------------------------------------
void GPIO_RAQUETTE_CONFIG (int GPIO_KEY_L[4],int GPIO_KEY_C[4]) {
  
  int  i,j, I, J ;
    
  GPIO_KEY_L[0] = GPIO_KEY_L1 ; GPIO_KEY_L[1] = GPIO_KEY_L2 ; GPIO_KEY_L[2] = GPIO_KEY_L3 ; GPIO_KEY_L[3] = GPIO_KEY_L4 ;
  GPIO_KEY_C[0] = GPIO_KEY_C1 ; GPIO_KEY_C[1] = GPIO_KEY_C2 ; GPIO_KEY_C[2] = GPIO_KEY_C3 ; GPIO_KEY_C[3] = GPIO_KEY_C4 ;

  for(i=0;i<4;i++) {
    GPIO_SET( GPIO_KEY_C[i], 0) ;
    for(j=0;j<4;j++)
      if( GPIO_GET(GPIO_KEY_L[j]))
        printf("Clavier ligne %d et colonne %d => %d mis a 5V et GPIO %d allumer\n",i,j,GPIO_KEY_C[i],GPIO_KEY_L[j]);
      else
        printf("Clavier ligne %d et colonne %d => %d mis a 5V et GPIO %d eteint\n",i,j,GPIO_KEY_C[i],GPIO_KEY_L[j]);
  }
}
// ---------------------------------------------------------------------------------------
void GPIO_RAQUETTE_READ (int GPIO_KEY_L[4],int GPIO_KEY_C[4], char KEYBOARD[4][4][GPIO_TAILLE_BUFFER], CLAVIER* clavier) {
  int  i,j, I, J , appui ;
  char val[255] ;
  
  I=-1; ; J=-1 ;
  
  for(i=0;i<4;i++) GPIO_SET( GPIO_KEY_C[i], 0) ;
  //for(j=0;j<4;j++) GPIO_SET( GPIO_KEY_L[i], 0) ;
  
  strcpy(val,"") ;     
  
  // =======================================================================
  // Lecture d'une touche au clavier matriciel 4*4
  // Tant que une touche est appuyee la valeur appui_en_cours est a 1
  // On passe plusieurs fois dans cette boucle en fonction de la fonction appelante
  // qui est dans astro.c
  // =======================================================================
  
  clavier->appui_en_cours = 0 ;
    
  for(i=0;i<4;i++) {
    GPIO_SET( GPIO_KEY_C[i], 1) ;
    
    usleep( clavier->temporisation_clavier ) ;
    
    for(j=0;j<4;j++)  {
      if( GPIO_GET(GPIO_KEY_L[j])) {
        I=i; J=j ;
	  memset( val, '\0', 255 );
          strcpy( val, KEYBOARD[I][J] ) ;
	  if ( strcmp( val, "") ) {
	    //printf("val = %s, keyboard[ %d ][ %d ] = %s\n", val, i,j, KEYBOARD[i][j] ) ;
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
  // dans ceet aprtie de code
  // =======================================================================
  
  if ( clavier->mot_en_cours && ! clavier->appui_en_cours ) {
  
    // printf("mot trouver = %s\n", clavier->mot ) ;
    
    //------------------------------------------------------------
    // On incremente la phrase avec le mot et
    // On incremente le nombre avec le mot si premier n'est pas vide
    //------------------------------------------------------------
    
    if ( strcmp( clavier->mot, clavier->valider) != 0 ) { 
      
      if ( strlen(clavier->phrase) + strlen(clavier->mot) < DATAS_TAILLE_BUFFER_2)
        sprintf(clavier->phrase,"%s%s",clavier->phrase, clavier->mot);
      
      if ( strcmp( clavier->premier, "")) {
        if ( strlen(clavier->nombre) + strlen(clavier->mot) < DATAS_TAILLE_BUFFER_2)
        //printf("Si pas d'appui sur valider et premier non vide => on met le mot dans la phrase !!\n" ) ; 
          sprintf(clavier->nombre,"%s%s",clavier->nombre,clavier->mot);
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
    // Exemple : MENU - MES ou VALIDER
    // Alors on VALIDE la phrase en cours
    //  - en mettant premier dans       SYMBOLE
    //  - et le reste de la phrase dans NOMBRE    
    // et on met le mot dans PREMIER (premier mot de la phrase)
    //------------------------------------------------------------
    
    for( i=0 ; i < clavier->validations_size ; i++ )
    if ( ! strcmp( clavier->phrase, clavier->validations[i]) \
      || ! strcmp( clavier->mot,    clavier->valider )  ) {
      //printf("APPUI sur VALIDER => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1\n" ) ; 
      
      strcpy(clavier->symbole, clavier->premier)  ;
      
      strcpy(clavier->premier,"") ;
      strcpy(clavier->phrase,"")  ;
      strcpy(clavier->mot,"") ;
      
      //printf("TROIS = symbole = %s nombre = %s\n", clavier->symbole, clavier->nombre ) ;
      clavier->phrase_lue=1 ;
    }
    
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // et on met le mot dans PREMIER (premier mot de la phrase)
    //------------------------------------------------------------
    
    for( i=0 ; i < clavier->actions_size ; i++ )
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
//==========================================================
void GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS(int GPIO_KEY_L[4],int GPIO_KEY_C[4], char raquette[4][4][GPIO_TAILLE_BUFFER], SUIVI *suivi) {
  int  i,j, I, J , appui ;
  char val[255] ;
  
  I=-1; ; J=-1 ;
  
  for(i=0;i<4;i++) GPIO_SET( GPIO_KEY_C[i], 0) ;
  
  strcpy(val,"") ;       
  // =======================================================================
  for(i=0;i<4;i++) { GPIO_SET( GPIO_KEY_C[i], 1) ; 
  for(j=0;j<4;j++)  { if( GPIO_GET(GPIO_KEY_L[j])) {
   
    if ( ! strcmp( raquette[i][j], "plus" ) ) {  suivi->pas_acc_plus =1 ; }
    if ( ! strcmp( raquette[i][j], "moins" ) ) { suivi->pas_acc_moins=1 ; }

    if ( ! strcmp( raquette[i][j], "ne" ) ) { suivi->pas_nord=1 ; suivi->pas_est=1   ; }
    if ( ! strcmp( raquette[i][j], "no" ) ) { suivi->pas_nord=1 ; suivi->pas_ouest=1 ; }
    if ( ! strcmp( raquette[i][j], "se" ) ) { suivi->pas_sud=1  ; suivi->pas_est=1   ; }
    if ( ! strcmp( raquette[i][j], "so" ) ) { suivi->pas_sud=1  ; suivi->pas_ouest=1 ; }
    
    if ( ! strcmp( raquette[i][j], "n" ) ) { suivi->pas_nord  = 1 ; }
    if ( ! strcmp( raquette[i][j], "o" ) ) { suivi->pas_ouest = 1 ; }
    if ( ! strcmp( raquette[i][j], "e" ) ) { suivi->pas_est   = 1 ; }
    if ( ! strcmp( raquette[i][j], "s" ) ) { suivi->pas_sud   = 1 ; }
    
    if ( ! strcmp( raquette[i][j], "reset" ) ) { suivi->reset   = 1 ; }
    
  }} GPIO_SET( GPIO_KEY_C[i], 0) ;  }
  //printf("%ld %ld %ld %ld\n", suivi->pas_ouest, suivi->pas_est, suivi->pas_nord, suivi->pas_sud);
}
//==========================================================
void GPIO_TRAP(int sig) {
  printf("Signal %d trappe, action suivantes effectuees :\n",sig) ;
  GPIO_SET(gpio_out[2],0);
  printf("ret gpio close=%d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
  exit(1) ;
}
//==========================================================
void  GPIO_STATUT(void) {

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
  else 
    printf("gpio get %d\n",gpio) ;
    
  return 0 ;
}
//====================================================================================================
int GPIO_SET(int gpio,int val) {
  
  char buf[BUFFER] ;
  int retour ;
  retour=0 ;
  memset(buf,0,BUFFER);
  snprintf(buf,BUFFER,"%d\n",val) ;
  if ( GPIO_OPEN_STATUT == 0 ) retour=pwrite(gpio_fd[gpio],buf,strlen(buf),0) ;
  else printf("gpio %d set %d\n",gpio_fd[gpio], val) ;
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
  
  memset(gpio_fd,0,GPIO_SIZE);
    
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
int GPIO_OPEN_BROCHE_PWM(GPIO_PWM_PHASE *gpwm) {
  
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
  
  if (gpwm->gpio_open_statut != 0) return GPIO_OPEN_STATUT ;
  
  gpwm->gpio_fd=0;
    
  sprintf(exp,"%s/export",          GPIO_PATH ) ;
  sprintf(dir,"%s/gpio%d/direction",GPIO_PATH, gpwm->gpio) ;
  sprintf(val,"%s/gpio%d/value",    GPIO_PATH, gpwm->gpio) ;
  
  TRACE("exp=%s",exp);
  TRACE("dir=%s",dir);
  TRACE("val=%s",val);
    
  //TRACE("gpio ouverture en OUTPUT : dir= %s exp=%s val=%s",dir,exp,val);
    
  if ((f=fopen(exp,"w")) == NULL )                      { gpwm->gpio_open_statut = -4  ; }
  else if ((fprintf(f,"%d\n",gpwm->gpio))<=0)           { gpwm->gpio_open_statut = -5  ; }
  else if ((fclose(f)) != 0 )                           { gpwm->gpio_open_statut = -6  ; }
  else if ((f=fopen(dir,"w")) == NULL)                  { gpwm->gpio_open_statut = -7  ; }
  else if ((fprintf(f,"out\n")<=0))                     { gpwm->gpio_open_statut = -8  ; }
  else if ((fclose(f)!=0))                              { gpwm->gpio_open_statut = -9  ; }
  else if ((gpwm->gpio_fd =open(val, O_WRONLY))<0)     { gpwm->gpio_open_statut = -10  ; }
  
  TRACE("statut=%d",  gpwm->gpio_open_statut);
  
  return GPIO_OPEN_STATUT ;
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
  //close( gpio_fd [ gpio ] ) ;
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
    //close( gpio_fd [ gpio_out [ i ] ] ) ;
    if ((f=fopen(une,"w")) == NULL )   { return -1 ;}
    if ((fprintf(f,"%d\n",gpio_out[i]))<=0) { return -2 ;}
    if ((fclose(f)) != 0 ) { return -3 ;}
  }
  i=-1;
  while(gpio_in[++i]>0) {
    memset(une,0,BUFFER);
    sprintf(une,"%s/unexport",GPIO_PATH) ;
    //close( gpio_fd [ gpio_in [ i ] ] ) ;
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
// ***************************************************************************************************
void main1(int argc, char **argv)
{
  int i,incr,dir,timeout;
  long frequence ;
  
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  if( argc != 4 ) { 
    printf("Usage : %s <dir [0/1] > <clk [frq] > <timeout [en s]>\n",argv[0]) ;
    for(i=0;gpio_in[i]>0 ;i++) printf("gpio_in[%d]  = %d\n", i,gpio_in[i]) ;
    for(i=0;gpio_out[i]>0;i++) printf("gpio_out[%d] = %d\n", i,gpio_out[i]) ;
    exit(1) ;
  }
  
  dir=atoi(argv[1]) ;
  frequence=atol(argv[2]) ;
  timeout=atoi(argv[3]) ;
  
  printf("dir=%d\n",dir) ;
  printf("frequence=%ld\n",frequence) ;
  printf("timeout=%d\n",timeout) ;
  
  for(i=0;i<GPIO_SIZE;i++) printf("gpio_in[%d]=%d\n", i,gpio_in[i]) ;
  for(i=0;i<GPIO_SIZE;i++) printf("gpio_out[%d]=%d\n",i,gpio_out[i]) ;

  printf("ret=%d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
  i=-1;
  
  while(gpio_out[++i]>0) {
    printf("gpio_out[%d]=%d\n",i,gpio_out[i]) ;
    // printf("gpio_fd[%d]=%d\n",i,gpio_fd[gpio_out[i]]) ;
    printf("on met a 1 la broche %d ==> retour de GPIO_SET = %d\n",gpio_out[i],GPIO_SET(gpio_out[i],1));
  }
  i=-1;
  while(gpio_in[++i]>0) { 
    // printf("gpio_fd[%d]=%d\n",i,gpio_fd[gpio_in[i]]) ;
    printf("gpio_in : %d => %d\n",gpio_in[i],GPIO_GET(gpio_in[i]));
  }
  
  GPIO_SET(gpio_out[0],dir);
  GPIO_SET(gpio_out[2],1);
  
  incr=-1;
  
  printf("Application d'une frequence %ld sur la broche %d \n",frequence,gpio_out[1]) ;
  
  while( ++incr < ((int)(frequence * timeout))) {
    i=-1;
    while(gpio_out[++i]>0) {
      
      GPIO_SET(gpio_out[1],1) ;
      usleep((int)(200000.0/(frequence))) ;
  
      GPIO_SET(gpio_out[1],0) ;
      usleep((int)(800000.0/(frequence))) ;
    }
  }
  GPIO_SET(gpio_out[2],0);
  printf("ret=%d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
  
}
// ***************************************************************************************************
// Main 2 = permet d'activer une broche GPIO en particulier
// ***************************************************************************************************

void main2(int argc, char ** argv)
{
  int i,incr,dir,timeout, val, priority, nbcpus, cpu, pid ;
  int frequence ;
  int gpio ;
  struct sched_param param ;
  cpu_set_t cpuset ;
  
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  if( argc < 3 ) { 
    printf("Usage : %s <gpio> <dir> <val(fac)> <fre(fac)> <duree(fac)> <priorite(fac)> <cpu(fac)> \n",argv[0]) ;
    exit(1) ;
  }
  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  printf("pid processus = %d\n", pid) ;
  printf("nombre de cpus = %d\n", nbcpus) ;
  
  gpio=atoi(argv[1]) ;
  dir=atoi(argv[2]) ;
  
  if ( dir && argc > 3 ) val=atoi(argv[3]) ; else val=0 ;
  if ( dir && argc > 4 ) frequence=atoi(argv[4]) ; else frequence=10 ;
  if ( dir && argc > 5 ) timeout=atoi(argv[5]) ; else timeout=5 ;
  if ( dir && argc > 6 ) priority=atoi(argv[6]) ; else priority=-1 ;
  if ( dir && argc > 7 ) cpu=atoi(argv[7]) ; else cpu=-1 ;
  
  if ( priority > 0)  {
    param.sched_priority = priority ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  }  
  if ( cpu >= 0 ) {   
    CPU_ZERO(& cpuset ) ;
    CPU_SET( cpu, &cpuset) ;
    if ( sched_setaffinity( getpid() , sizeof( cpuset ), & cpuset) != 0) { perror("sched_setaffinity"); exit(EXIT_FAILURE); }
    else printf("modification du cpu du precessus :  %d\n", cpu) ;
  }
  // frequence=atol(argv[2]) ;
  // timeout=atoi(argv[3]) ;
  
  printf("gpio = %d\n",gpio ) ;
  printf("dir  = %d\n",dir  ) ;
  printf("val  = %d\n",val  ) ;
  
  // printf("frequence=%ld\n",frequence) ;
  // printf("timeout=%d\n",timeout) ;
  
  printf("ret=%d\n",GPIO_OPEN_BROCHE(gpio,dir)) ;
  
 if ( dir ) printf("on met a %d la broche %d ==> retour de GPIO_SET = %d\n", val, gpio, GPIO_SET( gpio, val ));
 else       printf("on lit la broche gpio %d => %d\n",gpio, GPIO_GET( gpio ));

  if ( dir ) {
  
  printf(" ... application d'une frequence %d HZ pendant %d s sur la broche %d \n",frequence, timeout,gpio) ;
  
  incr=0 ;
 
   while( ++incr < ((int)(frequence * timeout))) {
    GPIO_SET( gpio , 1 ) ; usleep((int)(200000.0/(frequence))) ;
    GPIO_SET( gpio , 0 ) ; usleep((int)(800000.0/(frequence))) ; 
   }
   GPIO_SET(gpio,0);
  }
  printf("ret=%d\n",GPIO_CLOSE_BROCHE( gpio )) ;
  
}
// ***************************************************************************************************
// Main 3 = permet d'activer une sequence moteur mode demi pas 
// a partir du numero de la premier broche GPIO 
// exemple : gpio 5 1 1 100
// activation de la sequence demi pas sur broches 5 6 7 et 8
// avec une frequence de 100 Hz
// ***************************************************************************************************

void main3(int argc, char ** argv)
{
  int i,incr,dir,timeout, val, priority, nbcpus, cpu, pid ;
  int frequence ;
  int gpio , gpio2, gpio3, gpio4 ;
  int iseq, seq[8][4] = \
   {{1,0,0,0,},\
    {1,1,0,0},\
    {0,1,0,0},\
    {0,1,1,0},\
    {0,0,1,0},\
    {0,0,1,1},\
    {0,0,0,1},\
    {1,0,0,1}};
    
  struct sched_param param ;
  cpu_set_t cpuset ;
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  if( argc < 3 ) { 
    printf("Usage : %s <gpios (inf)> <dir> <val(fac)> <fre(fac)> <duree(fac)> <priorite(fac)> <cpu(fac)> \n",argv[0]) ;
     for(iseq=0;iseq<8;iseq++) {
      printf( "seq [%d][0] = %d\n",iseq, seq[iseq][0] ) ;
      printf( "seq [%d][1] = %d\n",iseq, seq[iseq][1] ) ;
      printf( "seq [%d][2] = %d\n",iseq, seq[iseq][2] ) ;
      printf( "seq [%d][3] = %d\n",iseq, seq[iseq][3] ) ;
     }
    exit(1) ;
  }
  
  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  printf("pid processus = %d\n", pid) ;
  printf("nombre de cpus = %d\n", nbcpus) ;
  
  gpio   = atoi(argv[1]) ;
  gpio2 = gpio + 1 ;
  gpio3 = gpio + 2 ;
  gpio4 = gpio + 3 ;
  
  dir=atoi(argv[2]) ;
  
  if ( dir && argc > 3 ) val=atoi(argv[3]) ; else val=0 ;
  if ( dir && argc > 4 ) frequence=atoi(argv[4]) ; else frequence=10 ;
  if ( dir && argc > 5 ) timeout=atoi(argv[5]) ; else timeout=2 ;
  if ( dir && argc > 6 ) priority=atoi(argv[6]) ; else priority=-1 ;
  if ( dir && argc > 7 ) cpu=atoi(argv[7]) ; else cpu=-1 ;
  
  if ( priority > 0)  {
    param.sched_priority = priority ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  }  
  if ( cpu >= 0 ) {   
    CPU_ZERO(& cpuset ) ;
    CPU_SET( cpu, &cpuset) ;
    if ( sched_setaffinity( getpid() , sizeof( cpuset ), & cpuset) != 0) { perror("sched_setaffinity"); exit(EXIT_FAILURE); }
    else printf("modification du cpu du precessus :  %d\n", cpu) ;
  }
  // frequence=atol(argv[2]) ;
  // timeout=atoi(argv[3]) ;
  
  printf("Phases du moteurs ((1a-1b)-(2a-2b)) a mettre sur = %d %d %d %d\n", gpio, gpio2, gpio3, gpio4 ) ;
  //printf("gpio = %d\n",gpio ) ;
  printf("dir  = %d\n",dir  ) ;
  printf("val  = %d\n",val  ) ;
  
  // printf("frequence=%ld\n",frequence) ;
  // printf("timeout=%d\n",timeout) ;
  
  printf("retour GPIO_OPEN_BROCHE(%d, %d) =%d\n",gpio  ,dir,GPIO_OPEN_BROCHE(gpio,dir)) ;
  printf("retour GPIO_OPEN_BROCHE(%d, %d) =%d\n",gpio2,dir,GPIO_OPEN_BROCHE(gpio2,dir)) ;
  printf("retour GPIO_OPEN_BROCHE(%d, %d) =%d\n",gpio3,dir,GPIO_OPEN_BROCHE(gpio3,dir)) ;
  printf("retour GPIO_OPEN_BROCHE(%d, %d) =%d\n",gpio4,dir,GPIO_OPEN_BROCHE(gpio4,dir)) ;
  
 if ( dir ) printf("on met a %d la broche %d ==> retour de GPIO_SET = %d\n", val, gpio, GPIO_SET( gpio, val ));
 else       printf("on lit la broche gpio %d => %d\n",gpio, GPIO_GET( gpio ));

  if ( dir ) {
  
  printf(" ... application d'une frequence %d HZ pendant %d s sur la broche %d \n",frequence, timeout,gpio) ;
  
  incr=0 ;
 
   while( ++incr < ((int)(frequence * timeout))) {
    for(iseq=0;iseq<8;iseq++) {
      GPIO_SET( gpio , seq[iseq][0] ) ;
      GPIO_SET( gpio2 , seq[iseq][1] ) ;
      GPIO_SET( gpio3 , seq[iseq][2] ) ;
      GPIO_SET( gpio4 , seq[iseq][3] ) ;
      usleep((int)(1000000.0/(frequence))) ; 
     }
   }
   GPIO_SET(gpio,0);
  }
  printf("retour GPIO_CLOSE_BROCHE(%d, %d) =%d\n",gpio,dir,GPIO_CLOSE_BROCHE(gpio)) ;
  printf("retour GPIO_CLOSE_BROCHE(%d, %d) =%d\n",gpio,dir,GPIO_CLOSE_BROCHE(gpio2)) ;
  printf("retour GPIO_CLOSE_BROCHE(%d, %d) =%d\n",gpio,dir,GPIO_CLOSE_BROCHE(gpio3)) ;
  printf("retour GPIO_CLOSE_BROCHE(%d, %d) =%d\n",gpio,dir,GPIO_CLOSE_BROCHE(gpio4)) ;  
}
// ***************************************************************************************************

void main4(int argc, char ** argv)
{
  int dir=1 ;
  int gpio  ;
  unsigned long f1, f2 , pulses ;
  struct sched_param param ;
  double delai ;
  
  cpu_set_t cpuset ;
  
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  if( argc < 3 ) { 
    printf("Usage : %s <gpio> <fre deb> <fre fin> <nombre impulsions>\n",argv[0]) ;
    exit(1) ;
  }
  
  gpio    = atoi(argv[1]) ;
  f1      = atol(argv[2]) ;
  f2      = atol(argv[3]) ;
  pulses  = atol(argv[4]) ;
    
  printf("gpio       = %d\n",gpio ) ;
  printf("f1         = %ld\n",f1  ) ;
  printf("f2         = %d\n",f2  ) ;
  printf("impulsions = %ld\n",pulses) ;
  
  printf("ret=%d\n",GPIO_OPEN_BROCHE(gpio,dir)) ;
  
  // nb_pulsation_totales = delai * ( f1 + f2 ) / 2.0 ;
  delai = pulses * 2.0 / ( f1 + f2  ) ;
  
  printf( " nombre impulsions = %ld\n", GPIO_ACCELERATION_1( gpio, (double)f1, (double) f2, delai, 2 ) );
   
  printf("ret=%d\n",GPIO_CLOSE_BROCHE( gpio )) ;
  
}
// ***************************************************************************************************
void main5(int argc, char ** argv)
{
  double frequence ;
  double rapport ; 
  double duration ; 
  int gpio  ;
  int pid  ;
  int nbcpus  ;
  int priority  ;
  struct sched_param param ;
  cpu_set_t cpuset ;
  
  if( argc < 6 ) { 
    printf("Usage : %s <gpio> <frequence PWM (Hz)> <rapport cyclique PWM(0.0-1.0)> <duree(s)> <priority (0-99)>\n",argv[0]) ;
    exit(1) ;
  }
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  gpio      = atoi(argv[1]) ;
  frequence = atof(argv[2]) ;
  rapport   = atof(argv[3]) ;
  duration  = atof(argv[4]) ;
  priority  = atoi(argv[5]) ;
    
  printf("gpio       = %d\n",gpio ) ;
  printf("frequence  = %f\n",frequence  ) ;
  printf("rapport    = %f\n",rapport  ) ;
  printf("duree      = %f\n",duration) ;
  printf("priority   = %d\n",priority) ;
  
  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  printf("pid processus = %d\n", pid) ; 
  printf("nb cppus      = %d\n", nbcpus ) ; 

  if ( priority > 0)  {
    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;

    param.sched_priority = priority ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  }  
  // ==============================================================

  printf("ret=%d\n",GPIO_OPEN_BROCHE(gpio,1)) ;
  
  sleep(1) ; // attendre que les ouvertures GPIO se fassent 

  GPIO_PWM( gpio, frequence, rapport, duration ) ; 
  
  printf("ret=%d\n",GPIO_CLOSE_BROCHE( gpio )) ;
}
//====================================================================================================
int GPIO_PWM(int gpio,double frequence, double rapportcyclique, double duration ) {
  
  char buf0[BUFFER] ;
  char buf1[BUFFER] ;
  double periode ;
  double periode_haut ;
  double periode_bas ;
  double ddiff ;
  double ddiff_haut ;
  double ddiff_bas ;
  double duree_microsec ;
  int retour ;
  struct timeval tbegin, t0, t1, t2 ;

  periode          = GPIO_MICRO_SEC / frequence ;
  periode_haut     = periode * rapportcyclique ;
  periode_bas      = periode - periode_haut ;

  duree_microsec   = duration * GPIO_MICRO_SEC ;
  
  retour=0 ;

  memset(buf0,0,BUFFER);
  snprintf(buf0,BUFFER,"%d\n",0) ;
  
  memset(buf1,0,BUFFER);
  snprintf(buf1,BUFFER,"%d\n",1) ;

  ddiff = ddiff_haut = ddiff_bas = 0 ;

  gettimeofday(&tbegin,NULL) ;
  gettimeofday(&t0,NULL) ;
  gettimeofday(&t1,NULL) ;

  while ( ddiff < duree_microsec ) {
    
    ddiff_haut = ddiff_bas = 0 ;

    // ACTIVATION du front montant

    if ( GPIO_OPEN_STATUT == 0 ) retour=pwrite(gpio_fd[gpio],buf1,strlen(buf1),0) ;
    else TRACE("error : gpio %d set %d\n",gpio_fd[gpio], 1 ) ;
    
    // a completer : mettre en place une temporisation pour eviter conso CPU

    while( ddiff_haut < periode_haut ) {
      ddiff = (( t1.tv_sec - tbegin.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - tbegin.tv_usec ;
      if ( ddiff >= duree_microsec ) break ;
      ddiff_haut = (( t1.tv_sec - t0.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - t0.tv_usec ;
      gettimeofday(&t1,NULL) ;
    }
    if ( ddiff >= duree_microsec ) break ;

    // ACTIVATION du front descendant

    if ( GPIO_OPEN_STATUT == 0 ) retour=pwrite(gpio_fd[gpio],buf0,strlen(buf0),0) ;
    else TRACE("error : gpio %d set %d\n",gpio_fd[gpio], 0 ) ;

    // a completer : mettre en place une temporisation pour eviter conso CPU

    while( ddiff_bas < periode ) {
      ddiff = (( t1.tv_sec - tbegin.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - tbegin.tv_usec ;
      if ( ddiff >= duree_microsec ) break ;
      ddiff_bas = (( t1.tv_sec - t0.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - t0.tv_usec ;
      gettimeofday(&t1,NULL) ;
    }
    if ( ddiff >= duree_microsec ) break ;
    gettimeofday(&t0,NULL) ;
    ddiff = (( t0.tv_sec - tbegin.tv_sec ) * GPIO_MICRO_SEC) + t0.tv_usec - tbegin.tv_usec ;
  }
  return retour ;
}
// ##########################################################################################################
void GPIO_TEMPORISATION(double duration, double percent, struct timeval t0) {
  
  struct timeval t1 ;
  double  tdiff ;
  struct timespec nt, ntt ;
  
  nt.tv_sec = 0 ;
  nt.tv_nsec =  (long)( duration * percent * 1000 ) ;
  nanosleep(&nt,&ntt) ;
   
  tdiff=0;
 
  while( tdiff < duration  ) {
      gettimeofday(&t1,NULL) ;
      tdiff = (double)(( t1.tv_sec - t0.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - t0.tv_usec ;
  }
}
// ##########################################################################################################
double GPIO_DOUBLE_TEMPORISATION(double duration1, double duration2, struct timeval t0, struct timeval tb ) {
  
  struct timeval t1 ;
  double  tdiff1 ;
  double  tdiff2 ;
  tdiff1 = tdiff2 = 0 ;
  while( tdiff1 < duration1 && tdiff2 <  duration2 ) {
      gettimeofday(&t1,NULL) ;
      tdiff1 = (( t1.tv_sec - t0.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - t0.tv_usec ;
      tdiff2 = (( t1.tv_sec - tb.tv_sec ) * GPIO_MICRO_SEC) + t1.tv_usec - tb.tv_usec ;
  }
  return tdiff2 ;
}
/*
  int COEUR_CPU = 1 ;
  cpu_set_t cpuset ;
  pthread_t thread = pthread_self();
  CPU_ZERO(& cpuset ) ;
  CPU_SET(COEUR_CPU, &cpuset) ;
  s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  if (s != 0) perror("pthread_setaffinity_np cpu");
*/
// ##########################################################################################################
void * GPIO_SUIVI_PWM_PHASE(GPIO_PWM_PHASE *gpios_pwm ) {
  
  char   buf0[BUFFER] ;
  char   buf1[BUFFER] ;
  double TUpwm, TUpwm_haut, TUpwm_bas, rap ;
  int    pas, retour, i , j  ;
  
  usleep(150000)  ;
  
  TUpwm = TUpwm_haut  = 0 ;
  
  memset(buf0,0,BUFFER); snprintf(buf0,BUFFER,"%d\n",0) ;
  memset(buf1,0,BUFFER); snprintf(buf1,BUFFER,"%d\n",1) ;

  while ( 1 ) {
     
     pas        = gpios_pwm->pas ; 
     rap        = gpios_pwm->rap[ pas ] ; 
     
     // TRACE("%d %f",pas, rap ) ;   
     
     TUpwm      = (double)GPIO_MICRO_SEC * gpios_pwm->Tpwm ;
     
     TUpwm_haut = TUpwm * rap  ; 
     TUpwm_bas  = TUpwm -  TUpwm_haut ; 

     if ( rap >= 0.0000001 ) pwrite( gpios_pwm->gpio_fd,buf1,strlen(buf1),0) ;
     usleep( TUpwm_haut ) ;  
     pwrite( gpios_pwm->gpio_fd,buf0,strlen(buf0),0) ;
     usleep( TUpwm_bas ) ;
  }
}
// ##########################################################################################################
void * GPIO_SUIVI_PWM_PHASE_A(GPIO_PWM_PHASES_MOTEUR *gpios_pwm ) {
  
  double rap ;
  char   buf0[BUFFER] ;
  char   buf1[BUFFER] ;
  double TUpwm, TUpwm_haut, TUpwm_bas ;
  int    retour, i , j  ;
  
  usleep(150000)  ;
  
  TUpwm = TUpwm_haut  = 0 ;
  
  memset(buf0,0,BUFFER); snprintf(buf0,BUFFER,"%d\n",0) ;
  memset(buf1,0,BUFFER); snprintf(buf1,BUFFER,"%d\n",1) ;

  while ( 1 ) {
    
    pthread_mutex_lock(  &mutex_phase_a ) ;
    
     //gpios_pwm->incrA ++ ;
     rap        = gpios_pwm->rap[ gpios_pwm->phaseA ][ gpios_pwm->pas ] ; 
     TUpwm      = (double)GPIO_MICRO_SEC * gpios_pwm->Tpwm ;
     TUpwm_haut = TUpwm * rap  ; 
     TUpwm_bas  = TUpwm -  TUpwm_haut ; 

     pwrite(gpio_fd[gpios_pwm->gpioApassif],buf0,strlen(buf0),0) ;
     pwrite(gpio_fd[gpios_pwm->gpioA],buf1,strlen(buf1),0) ;
    
    pthread_mutex_unlock(  &mutex_phase_a ) ; 
     usleep( TUpwm_haut ) ;  
    pthread_mutex_lock(  &mutex_phase_a ) ;
     pwrite(gpio_fd[gpios_pwm->gpioA],buf0,strlen(buf0),0) ;
    pthread_mutex_unlock(  &mutex_phase_a ) ; 
     usleep( TUpwm_bas ) ;
  }
  TRACE("GPIO_SUIVI_PWM_PHASE_A end " ) ;
}
// ##########################################################################################################
void * GPIO_SUIVI_PWM_PHASE_B(GPIO_PWM_PHASES_MOTEUR *gpios_pwm ) {
  
  double rap ;
  char   buf0[BUFFER] ;
  char   buf1[BUFFER] ;
  double TUpwm, TUpwm_haut, TUpwm_bas ;
  int    retour, i , j  ;
  
  usleep(150000)  ;
  
  TUpwm = TUpwm_haut  = 0 ;
  
  memset(buf0,0,BUFFER); snprintf(buf0,BUFFER,"%d\n",0) ;
  memset(buf1,0,BUFFER); snprintf(buf1,BUFFER,"%d\n",1) ;

  while ( 1 ) {
    
    pthread_mutex_lock(  &mutex_phase_b ) ;
    
     //gpios_pwm->incrB ++ ;
     rap        = gpios_pwm->rap[ gpios_pwm->phaseB ][ gpios_pwm->pas ] ; 
     TUpwm      = (double)GPIO_MICRO_SEC * gpios_pwm->Tpwm ;
     TUpwm_haut = TUpwm * rap  ; 
     TUpwm_bas  = TUpwm -  TUpwm_haut ; 

     pwrite(gpio_fd[gpios_pwm->gpioBpassif],buf0,strlen(buf0),0) ;
     pwrite(gpio_fd[gpios_pwm->gpioB],buf1,strlen(buf1),0) ;
    
    pthread_mutex_unlock(  &mutex_phase_b ) ; 
     usleep( TUpwm_haut ) ;  
    pthread_mutex_lock(  &mutex_phase_b ) ;
     pwrite(gpio_fd[gpios_pwm->gpioB],buf0,strlen(buf0),0) ;
    pthread_mutex_unlock(  &mutex_phase_b ) ; 
     usleep( TUpwm_bas ) ;
  }
  TRACE("GPIO_SUIVI_PWM_PHASE_A end " ) ;
}
// ##########################################################################################################
void GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES_2(int upas,double rap[5][4][128]) {
  
  int i,j,k;
  double rapportcyclique ;
  i=j=k=0 ; 
  switch(upas) { 
    case 8 : i=0 ;break ;
    case 16 : i=1 ;break ;
    case 32 : i=2 ;break ;
    case 64 : i=3 ;break ;
    case 128: i=4 ;break ;
  }
  for( i=0 ; i<5 ; i++ ) 
    for( j=0 ; j< 4 ; j++ )
      for( k=0 ; k< 128 ;  k++ )
        rap[i][j][k] = 0 ;

  for( j=0 ; j< upas ;  j ++ ) {
    
    rapportcyclique = sin( (double)(2*PI*j) / upas ) ;
    if ( rapportcyclique >  0) { rap[i][0][j] = rapportcyclique ;        rap[i][1][j] = 0.0 ; }
    else                       { rap[i][1][j] = rapportcyclique * -1.0 ; rap[i][0][j] = 0.0 ; }
  
    rapportcyclique = -1.0 * cos( (double)(2*PI*j) / upas ) ;
     if ( rapportcyclique >  0) { rap[i][2][j] = rapportcyclique ;        rap[i][3][j] = 0.0 ; }
    else                        { rap[i][3][j] = rapportcyclique * -1.0 ; rap[i][2][j] = 0.0 ; }
  
    TRACE("rap : %d %.8f %.8f %.8f %.8f", j, rap[i][0][j], rap[i][1][j], rap[i][2][j], rap[i][3][j] ) ; ; 
  }
}
// ##########################################################################################################
void GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(GPIO_PWM_PHASE *gpios_pwm[]) {
  
  int i;
  int j ;
  double t ;
  double rapportcyclique ;
 
  j=0 ;
  
  for( i=0 ; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) 
    for( j=0;j < GPIO_MICROPAS_MAX; j++ )
      gpios_pwm[i]->rap[j] = 0 ;
  
  TRACE("TM %f\t%f",gpios_pwm[0]->Tm ,gpios_pwm[0]->deltat) ;
  
  j=0 ; 
  for( t=0 ; t< gpios_pwm[0]->Tm ;  t += gpios_pwm[0]->deltat ) {
    
    TRACE("t = %f\t%f",t,gpios_pwm[0]->Tm ,gpios_pwm[0]->deltat) ;
    
    rapportcyclique = sin( 2*PI*gpios_pwm[0]->Fm* t ) ;
    
    if ( rapportcyclique >  0) { gpios_pwm[1]->rap[j] = rapportcyclique ;        gpios_pwm[0]->rap[j] = 0.0 ; }
    else                       { gpios_pwm[0]->rap[j] = rapportcyclique * -1.0 ; gpios_pwm[1]->rap[j] = 0.0 ; }
  
    rapportcyclique = -1.0 * cos( 2*PI*gpios_pwm[0]->Fm* t ) ;
     if ( rapportcyclique >  0) { gpios_pwm[3]->rap[j] = rapportcyclique ;        gpios_pwm[2]->rap[j] = 0.0 ; }
    else                        { gpios_pwm[2]->rap[j] = rapportcyclique * -1.0 ; gpios_pwm[3]->rap[j] = 0.0 ; }
 
    j++ ;
  }
  for( i=0 ; i< j ; i++ ) {
    if( (gpios_pwm[0]->rap[i]+gpios_pwm[1]->rap[i]+gpios_pwm[2]->rap[i]+gpios_pwm[3]->rap[i] ) != 0 )
    TRACE("%d\t%f\t%f\t%f\t%f",i,gpios_pwm[0]->rap[i],gpios_pwm[1]->rap[i],gpios_pwm[2]->rap[i],gpios_pwm[3]->rap[i]) ;
  }
  TRACE("Fin calcul rapports cycliques") ;
}
// ##########################################################################################################
void GPIO_CALCUL_PWM_PHASES_MOTEUR(double t, GPIO_PWM_PHASES_MOTEUR *gpios_pwm) {
  
  double rapportcyclique ;
  
  rapportcyclique = sin( 2*PI*gpios_pwm->Fm* t ) ;
  
  if ( rapportcyclique >  0) { gpios_pwm->ra1 = rapportcyclique ; gpios_pwm->ra2 = 0.0 ; }
  else                               { gpios_pwm->ra2 = rapportcyclique * -1.0 ; gpios_pwm->ra1 = 0.0 ; }
  
  rapportcyclique = cos( 2*PI*gpios_pwm->Fm* t ) ;
  if ( rapportcyclique < 0) { gpios_pwm->rb1 = rapportcyclique ;         gpios_pwm->rb2 = 0.0 ; }
  else                               { gpios_pwm->rb2 = rapportcyclique * -1.0 ; gpios_pwm->rb1 =  0.0; }
  
  TRACE("%f %f %f %f %f", t, gpios_pwm->ra1, gpios_pwm->ra2, gpios_pwm->rb1, gpios_pwm->rb2) ;
}
// ##########################################################################################################
void * suivi_main6(GPIO_PWM_PHASES_MOTEUR *gpios_pwm) {
   
   double t ;
   unsigned long incra, incrb ;
   
   gpios_pwm->t=0 ;
   gpios_pwm->pas=0 ;
   gpios_pwm->phaseA = 0 ;
   gpios_pwm->phaseB = 3 ;
   incra = incrb = 0 ;
   
   while (1) {
    
    pthread_mutex_lock( &mutex_phase_a ) ;
    pthread_mutex_lock( &mutex_phase_b ) ;
    pthread_mutex_lock( &mutex_phase_c ) ;
    pthread_mutex_lock( &mutex_phase_d ) ;
    
      gpios_pwm->pas++ ;
      gpios_pwm->t += gpios_pwm->deltat ;

      if ( gpios_pwm->pas >= gpios_pwm->upas ) { gpios_pwm->pas = 0 ; gpios_pwm->t = 0 ; }
      
      if ( gpios_pwm->t < gpios_pwm->Tm / 2 ) { 
         gpios_pwm->gpioA = gpios_pwm->gpioa1 ;
	 gpios_pwm->gpioApassif = gpios_pwm->gpioa2 ;
	 gpios_pwm->phaseA = 0 ;
	 gpios_pwm->phase = 0 ;
      }
      else { 
         gpios_pwm->gpioA = gpios_pwm->gpioa2 ;
	 gpios_pwm->gpioApassif = gpios_pwm->gpioa1 ;
	 gpios_pwm->phaseA = 1 ;
	 gpios_pwm->phase = 1 ;
      }
      if ( gpios_pwm->t > ( gpios_pwm->Tm / 4 ) && gpios_pwm->t < ( 3 * gpios_pwm->Tm) / 4 )  {
        gpios_pwm->gpioB = gpios_pwm->gpiob1 ;
	gpios_pwm->gpioBpassif = gpios_pwm->gpiob2 ;
        gpios_pwm->phaseB = 2 ;
	gpios_pwm->phase = 2 ;
      }
      else {
        gpios_pwm->gpioB = gpios_pwm->gpiob2 ;
	gpios_pwm->gpioBpassif = gpios_pwm->gpiob1 ;
        gpios_pwm->phaseB = 3 ;
	gpios_pwm->phase = 3 ;
      }
      
    TRACE("%f %d : gpios : %d %d phases = %d %d delta_incr = %ld %ld", \
      gpios_pwm->t , gpios_pwm->pas, gpios_pwm->gpioA , gpios_pwm->gpioB, gpios_pwm->phaseA , gpios_pwm->phaseB, \
      gpios_pwm->incrA - incra, gpios_pwm->incrB - incrb) ;
     
    pthread_mutex_unlock( &mutex_phase_a ) ;
    pthread_mutex_unlock( &mutex_phase_b ) ;
    pthread_mutex_unlock( &mutex_phase_c ) ;
    pthread_mutex_unlock( &mutex_phase_d ) ;
    
    usleep( (gpios_pwm->deltat) * GPIO_MICRO_SEC ) ;
  }
}
// ##########################################################################################################
void * suivi_main_M1(GPIO_PWM_PHASE *gpios_pwm[]) {
   
   double t ;
   int pas , i ;
   double deltat ;
   t=0 ;
   pas=0 ;
      
   deltat = gpios_pwm[0]->deltat ;
   
   for( i=0 ; i< GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) {
     gpios_pwm[i]->pas = 0 ;
     gpios_pwm[i]->t = 0 ;
   }
   while (1) {
    for( i=0 ; i< GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) {
    
      if ( i == 0 ) pthread_mutex_lock( &mutex_phase_a ) ;
      if ( i == 1 ) pthread_mutex_lock( &mutex_phase_b ) ;
      if ( i == 2 ) pthread_mutex_lock( &mutex_phase_c ) ;
      if ( i == 3 ) pthread_mutex_lock( &mutex_phase_d ) ;
    
        gpios_pwm[i]->pas++ ;
        gpios_pwm[i]->t += gpios_pwm[i]->deltat ;
        
	if ( gpios_pwm[i]->pas >= gpios_pwm[i]->upas ) { 
	  gpios_pwm[i]->pas = 0 ;
	  gpios_pwm[i]->t = 0 ;
	}

      if ( i == 0 ) pthread_mutex_unlock( &mutex_phase_a ) ;
      if ( i == 1 ) pthread_mutex_unlock( &mutex_phase_b ) ;
      if ( i == 2 ) pthread_mutex_unlock( &mutex_phase_c ) ;
      if ( i == 3 ) pthread_mutex_unlock( &mutex_phase_d ) ;
    }
    usleep( deltat * GPIO_MICRO_SEC ) ;
  }
}
// ##########################################################################################################
void * suivi_main_M2(GPIO_PWM_PHASE *gpios_pwm[]) {
   
   double t ;
   int pas , i ;
   double deltat ;
   t=0 ;
   pas=0 ;
      
   deltat = gpios_pwm[0]->deltat ;
   
   for( i=0 ; i< GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) {
     gpios_pwm[i]->pas = 0 ;
     gpios_pwm[i]->t = 0 ;
   }
   while (1) {
    // TRACE("cc suivi main") ;
    for( i=0 ; i< GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) {
    
      if ( i == 0 ) pthread_mutex_lock( &mutex_phase_e ) ;
      if ( i == 1 ) pthread_mutex_lock( &mutex_phase_f ) ;
      if ( i == 2 ) pthread_mutex_lock( &mutex_phase_g ) ;
      if ( i == 3 ) pthread_mutex_lock( &mutex_phase_h ) ;
    
        gpios_pwm[i]->pas++ ;
        gpios_pwm[i]->t += gpios_pwm[i]->deltat ;
        
	if ( gpios_pwm[i]->pas >= gpios_pwm[i]->upas ) { 
	  gpios_pwm[i]->pas = 0 ;
	  gpios_pwm[i]->t = 0 ;
	}

      if ( i == 0 ) pthread_mutex_unlock( &mutex_phase_e ) ;
      if ( i == 1 ) pthread_mutex_unlock( &mutex_phase_f ) ;
      if ( i == 2 ) pthread_mutex_unlock( &mutex_phase_g ) ;
      if ( i == 3 ) pthread_mutex_unlock( &mutex_phase_h ) ;
    }
    usleep( deltat * GPIO_MICRO_SEC ) ;
  }
}
// ##########################################################################################################
void main6(int argc, char **argv)
{
  int i ;
  double t ;
  double Fm, Fpwm ;
  double rapport ; 
  double deltat ;
  double upas ;
  int gpioa1  ;
  int gpioa2 ;
  int gpiob1 ;
  int gpiob2 ;
  int pid  ;
  int nbcpus  ;
  int priority  ;
  struct sched_param param ;
  cpu_set_t cpuset ;
  
  pthread_t p_thread_A ;
  pthread_t p_thread_B ;
  pthread_t p_thread_main ;
  pthread_t p_thread_interruptions ;
  
  GPIO_PWM_PHASES_MOTEUR *gpios_pwm, gpwm ;
  
  if( argc < 6 ) { 
    printf("Usage : %s <gpio (min)> <frequence MOTEUR (Hz)> <frequence PWM (x*Fm)>  <micro pas> <priority (0-99)>\n",argv[0]) ;
    printf("Commande un moteur sur les pin GPIOS [ gpio - gpio+3 ] en utilisant une modulation PWM\n") ;
    printf("Mettre 0 sur un des arguments Fpwm uPas revient a laisser le programme calculer sa valeur (un seul a la fois)\n") ; 
    exit(1) ;
  }
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  gpioa1     = atoi(argv[1]) ;
  gpiob1     = gpioa1+1;
  gpioa2     = gpioa1+2;
  gpiob2     = gpioa1+3;
  
  Fm         = atof(argv[2]) ;
  Fpwm       = atof(argv[3]) ;
  upas       = atof(argv[4]) ;
  
  priority    = atoi(argv[5]) ;    
  
  if ( Fpwm < Fm * upas )   {  
    Fpwm = Fm * upas ;
    if ( Fpwm >  GPIO_FREQ_MAX ) {
      Fpwm = GPIO_FREQ_MAX ; 
      upas = Fpwm / Fm ;
    }
  }  
  gpios_pwm = &gpwm ;
  gpios_pwm->id_moteur = 0 ;
  gpios_pwm->incrA =0 ;
  gpios_pwm->incrB =0 ;
  gpios_pwm->phaseA = 0 ;
  gpios_pwm->phaseB = 3 ;
  gpios_pwm->Fpwm = Fpwm ;
  gpios_pwm->Fm = Fm ;
  
  gpios_pwm->Tpwm = 1 / gpios_pwm->Fpwm ;
  gpios_pwm->Tm = 1 / gpios_pwm->Fm ;
  gpios_pwm->gpioa1 = gpioa1 ;
  gpios_pwm->gpioa2 = gpioa2 ;
  gpios_pwm->gpiob1 = gpiob1 ;
  gpios_pwm->gpiob2 = gpiob2 ;
  gpios_pwm->upas = upas ;
  gpios_pwm->nbdeltat = (unsigned long)(gpios_pwm->Fm * gpios_pwm->upas) ;
  gpios_pwm->deltat =  1 / ( gpios_pwm->Fm * gpios_pwm->upas ) ;
  
  TRACE1("gpioa1       = %d",gpios_pwm->gpioa1 ) ;
  TRACE1("gpioa2       = %d",gpios_pwm->gpioa2 ) ;
  TRACE1("gpiob1       = %d",gpios_pwm->gpiob1 ) ;
  TRACE1("gpiob2       = %d",gpios_pwm->gpiob2 ) ;
  TRACE("frequence Moteur = %f",gpios_pwm->Fm  ) ;
  TRACE("frequence PWM   = %f", gpios_pwm->Fpwm  ) ;
  TRACE1("periode Moteur = %f",gpios_pwm->Tm  ) ;
  TRACE1("periode PWM   = %f", gpios_pwm->Tpwm  ) ;
  TRACE("upas        = %f", gpios_pwm->upas ) ;
  TRACE("priority   = %d", priority) ;
  TRACE1("nbdeltat = %ld",gpios_pwm->nbdeltat) ;
  TRACE1("deltat = %f",gpios_pwm->deltat) ;
  
  for( i=0;i<4;i++ )  gpios_pwm->rap[i] = (double *) malloc ( sizeof(double) * gpios_pwm->nbdeltat ) ;
  
  GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(gpios_pwm)  ;
  
  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  TRACE1("pid processus = %d", pid) ; 
  TRACE1("nb cppus      = %d", nbcpus ) ; 

  if ( priority > 0)  {
    TRACE("mise en place temps reel : echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    param.sched_priority = priority ;
    TRACE("mise en place temps reel : param.sched_priority = %d", priority) ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  }  
  // ==============================================================
  
  printf("ret GPIO_CLOSE_BROCHE gpioa1 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpioa1)) ;
  printf("ret GPIO_CLOSE_BROCHE gpioa2 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpioa2)) ;
  printf("ret GPIO_CLOSE_BROCHE gpiob1 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpiob1)) ;
  printf("ret GPIO_CLOSE_BROCHE gpiob2 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpiob2)) ;
  
  printf("ret GPIO_OPEN_BROCHE gpioa1 =%d\n", GPIO_OPEN_BROCHE(gpios_pwm->gpioa1,1)) ;
  printf("ret GPIO_OPEN_BROCHE gpioa2 =%d\n", GPIO_OPEN_BROCHE(gpios_pwm->gpioa2,1)) ;
  printf("ret GPIO_OPEN_BROCHE gpiob1 =%d\n", GPIO_OPEN_BROCHE(gpios_pwm->gpiob1,1)) ;
  printf("ret GPIO_OPEN_BROCHE gpiob2 =%d\n", GPIO_OPEN_BROCHE(gpios_pwm->gpiob2,1)) ;
  
  usleep(150000) ; // attendre que les ouvertures GPIO se fassent
    
  pthread_create( &p_thread_main, NULL, (void*)suivi_main6, gpios_pwm ) ;
  // pthread_create( &p_thread_interruptions, NULL, (void*)suivi_interruptions, gpios_pwm ) ;
  
  pthread_create( &p_thread_A,    NULL, (void*)GPIO_SUIVI_PWM_PHASE_A, gpios_pwm ) ;
  pthread_create( &p_thread_B,    NULL, (void*)GPIO_SUIVI_PWM_PHASE_B, gpios_pwm ) ;
  
  pthread_join( p_thread_A, NULL) ;
  pthread_join( p_thread_B, NULL) ;
  pthread_join( p_thread_main, NULL) ;
  
  //  pthread_join( p_thread_interruptions, NULL) ;
  
  TRACE("Avnt le free()"); 
  
  // for( i=0;i<4;i++ ) free( gpios_pwm->rap[i] ) ;
  
  printf("ret GPIO_CLOSE_BROCHE gpioa1 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpioa1)) ;
  printf("ret GPIO_CLOSE_BROCHE gpioa2 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpioa2)) ;
  printf("ret GPIO_CLOSE_BROCHE gpiob1 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpiob1)) ;
  printf("ret GPIO_CLOSE_BROCHE gpiob2 =%d\n", GPIO_CLOSE_BROCHE(gpios_pwm->gpiob2)) ;
  
  for( i=0;i<4;i++ ) free( gpios_pwm->rap[i] ) ; exit(0) ;   
}
// ##########################################################################################################
void main(int argc, char **argv)
{
  int i ;
  double t ;
  double Fm, Fm2 , Fpwm ;
  double rapport ; 
  double deltat ;
  double upas ;
  double x ;

  int pid  ;
  int nbcpus  ;
  int priority  ;
  struct sched_param param ;
  cpu_set_t cpuset ;
  
  pthread_t p_thread_A ;
  pthread_t p_thread_B ;
  pthread_t p_thread_C ;
  pthread_t p_thread_D ;
  
  pthread_t p_thread_E ;
  pthread_t p_thread_F ;
  pthread_t p_thread_G ;
  pthread_t p_thread_H ;
  
  pthread_t p_thread_main_M1 ;
  pthread_t p_thread_main_M2 ;
  
  //GPIO_PWM_MOTEUR *pm0 , m0 ; 
  
  GPIO_PWM_PHASE  gpios_pwm_phase[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  GPIO_PWM_PHASE  gpios_pwm_phase2[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  
  GPIO_PWM_PHASE *gpwms[ GPIO_NB_PHASES_PAR_MOTEUR] ;
  GPIO_PWM_PHASE *gpwms2[ GPIO_NB_PHASES_PAR_MOTEUR] ;
  
  if( argc < 7 ) { 
    printf("Usage : %s <gpio1(min)> <gpio2(min)> <fr MOTEUR1 (Hz)> <fr MOTEUR2 (Hz)> <micro pas> <priority (0-99)>\n",argv[0]) ;
    printf("Commande DEUX moteurs sur les pin GPIOS [ gpio1 - gpio1+3 ][ gpio2 - gpio2+3 ] en utilisant une modulation PWM\n") ;
    printf("La frequence PWM est calculee automatiquement par le programme apres calcul optimisations\n") ;
    exit(1) ;
  }
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  
  Fm         = atof(argv[3]) ;
  Fm2        = atof(argv[4]) ;
  upas       = atof(argv[5]) ;
  priority   = atoi(argv[6]) ;    
  Fpwm       = GPIO_FREQUENCE_PWM ;
  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  TRACE("pid processus = %d", pid) ; 
  TRACE("nb cppus      = %d", nbcpus ) ; 

  if ( priority > 0)  {
    TRACE("mise en place temps reel : echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    param.sched_priority = priority ;
    TRACE("mise en place temps reel : param.sched_priority = %d", priority) ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  } 
  
  // ========== Initialisation de la struture du moteur 1 ==========
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) { 
  
   gpwms[i]           = &gpios_pwm_phase[ i ] ;
   gpwms[i]->phase    = i ;
   gpwms[i]->Fpwm     = Fpwm ;
   gpwms[i]->Fm       = Fm ;
   gpwms[i]->Tm       = 1 / gpwms[i]->Fm ;
   gpwms[i]->Tpwm     = 1 / gpwms[i]->Fpwm ;
   gpwms[i]->upas     = upas ;
   gpwms[i]->nbdeltat = (unsigned long)(gpwms[i]->Fm * gpwms[i]->upas) ;
   gpwms[i]->deltat   =  1 / ( gpwms[i]->Fm * gpwms[i]->upas ) ;
   
   printf(" %d\n", GPIO_OPEN_BROCHE_PWM(gpwms[i]) ) ;
   //sleep(1) ;
  }
  
  gpwms[0]->gpio = atoi(argv[1])+3  ;
  gpwms[1]->gpio = atoi(argv[1])+1  ;
  gpwms[2]->gpio = atoi(argv[1])  ;
  gpwms[3]->gpio = atoi(argv[1])+2;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) printf(" %d\n", GPIO_OPEN_BROCHE_PWM(gpwms[i]) ) ;

  GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(gpwms)  ;
  
  // ========== Initialisation de la struture du moteur 2 ==========

  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) { 
  
   gpwms2[i]           = &gpios_pwm_phase2[ i ] ;
   gpwms2[i]->phase    = i ;
   gpwms2[i]->Fpwm     = Fpwm ;
   gpwms2[i]->Fm       = Fm2 ;
   gpwms2[i]->Tm       = 1 / gpwms2[i]->Fm ;
   gpwms2[i]->Tpwm     = 1 / gpwms2[i]->Fpwm ;
   gpwms2[i]->upas     = upas ;
   gpwms2[i]->nbdeltat = (unsigned long)(gpwms2[i]->Fm * gpwms2[i]->upas) ;
   gpwms2[i]->deltat   =  1 / ( gpwms2[i]->Fm * gpwms2[i]->upas ) ;
   
   printf(" %d\n", GPIO_OPEN_BROCHE_PWM(gpwms2[i]) ) ;
   //sleep(1) ;
  }
  
  gpwms2[0]->gpio = atoi(argv[2])+3  ;
  gpwms2[1]->gpio = atoi(argv[2])+1  ;
  gpwms2[2]->gpio = atoi(argv[2])  ;
  gpwms2[3]->gpio = atoi(argv[2])+2;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) printf(" %d\n", GPIO_OPEN_BROCHE_PWM(gpwms2[i]) ) ;

  GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(gpwms2)  ;
  
  // ========== FIN Initialisations des strutures moteurs ==========
  
  usleep(150000) ; // attendre que les ouvertures GPIO se fassent
      
  pthread_create( &p_thread_main_M1, NULL, (void*)suivi_main_M1, gpwms ) ;
  pthread_create( &p_thread_main_M2, NULL, (void*)suivi_main_M2, gpwms2 ) ;
  
  pthread_create( &p_thread_A,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms[0] ) ;
  pthread_create( &p_thread_B,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms[1] ) ;
  pthread_create( &p_thread_C,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms[2] ) ;
  pthread_create( &p_thread_D,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms[3] ) ;
  
  pthread_create( &p_thread_E,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms2[0] ) ;
  pthread_create( &p_thread_F,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms2[1] ) ;
  pthread_create( &p_thread_G,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms2[2] ) ;
  pthread_create( &p_thread_H,    NULL, (void*)GPIO_SUIVI_PWM_PHASE, gpwms2[3] ) ;
    
  pthread_join( p_thread_A, NULL) ;
  pthread_join( p_thread_B, NULL) ;
  pthread_join( p_thread_C, NULL) ;
  pthread_join( p_thread_D, NULL) ;
  
  pthread_join( p_thread_E, NULL) ;
  pthread_join( p_thread_F, NULL) ;
  pthread_join( p_thread_G, NULL) ;
  pthread_join( p_thread_H, NULL) ;
  
  pthread_join( p_thread_main_M1, NULL) ;
  pthread_join( p_thread_main_M2, NULL) ;
}
// ##########################################################################################################
