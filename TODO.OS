Modifications a apporte au system : 

* copier les libairies 
   libncurses.so	
   liblirc-client.so
  dans
  
  pour que l'interpreteur ld puisse delinker l'executable sur la cible 
  ou bien exporter la variable LD_LIBRARY_PATH
  
=>  export LD_LIBRARY_PATH=$PWD/libtarget


  
  
