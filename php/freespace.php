<?php

error_reporting(E_ALL);

print '<body bgcolor=#aaaaaa>'."<br>\n" ;

$repbase='' ;

//===============================================================================
function freespace() {

  if ( disk_free_space($repbase) != FALSE ) $df = disk_free_space($repbase);  
  else                                      $df=0 ;

  print "\t".$rep.": free space = ".$df."<br>\n";

  $cmd1 = 'du -sh '.$repbase.'/*' ;
  $cmd2 = 'du -sh '.$repbase ;

  system ( $cmd1 );print "<br>" ;
  system ( $cmd2 );print "<br>" ;
}
//===============================================================================
function env() {
  
  $arr = get_defined_vars();
  
  foreach ($arr as $value) {    
    print 'variable :'.$value.'='.$arr[$value]."<br>\n" ;
  }
}
//===============================================================================
function env2() {

print "=====================<br>\n" ;
 foreach($_SERVER as $key => $val) echo '$_SERVER["'.$key.'"]='.$val.'<br />' ;
print "=====================<br>\n" ;
 foreach($_ENV as $key => $val) echo '$_ENV["'.$key.'"]='.$val.'<br />' ;
print "=====================<br>\n" ;
 foreach($_POST as $key => $val) echo '$_POST["'.$key.'"]='.$val.'<br />' ;
print "=====================<br>\n" ;
 foreach($_GET as $key => $val) echo '$_GET["'.$key.'"]='.$val.'<br />' ;
print "=====================<br>\n" ;
 foreach($_FILES as $key => $val) echo '$_FILES["'.$key.'"]='.$val.'<br />' ;
print "=====================<br>\n" ;
 foreach($_COOKIE as $key => $val) echo '$_COOKIE["'.$key.'"]='.$val.'<br />' ;
}
//===============================================================================
function scan_dir($base) {
  $cdir = scandir($base);
  foreach ($cdir as $key => $value) {
    if (!in_array($value,array(".",".."))) {
      print 'Lu :'.$value."<br>\n" ;
    }
  }
}
//===============================================================================
function list_dir($base) {

  if ($dir = opendir($base)) {
    while($rep = readdir($dir)) {
      if(is_dir($base.'/'.$rep)) {
        print "Rep ".$rep."<br>\n" ;
        $df = disk_free_space($base.'/'.$rep);
        $dt = disk_total_space($base.'/'.$rep);
        print "\t".$rep.": free space = ".$df."\ttotal space".$dt."<br>\n";
        
        //list_dir($rep);
      }
      else print "File ".$rep."<br>\n" ;
    }
    closedir($dir);
  }
}
//===============================================================================

echo 'Current PHP version: ' . phpversion();

print "=====================<br>\n" ;

if ( isset($_ENV['DOCUMENT_ROOT'])) $repbase = $_ENV['DOCUMENT_ROOT']; 
else print 'Variable $_ENV[\'DOCUMENT_ROOT\'] non défini'. "<br>\n" ;

print "=====================<br>\n" ;

env() ;

print "=====================<br>\n" ;

env2() ;

print "=====================<br>\n" ;

//list_dir($repbase);

print '</body>'."<br>\n" ;
?>


