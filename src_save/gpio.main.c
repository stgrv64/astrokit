// ***************************************************************************************************
void MainGpio(int argc, char**argv)
{
  int i,incr,dir,timeout;
  long frequence ;
  
  mlockall(MCL_CURRENT | MCL_FUTURE);
  signal(SIGINT,GPIO_TRAP) ;
  // signal(SIGINT,GPIO_TRAP) ;
  //signal(SIGKILL,GPIO_TRAP) ;
  
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


