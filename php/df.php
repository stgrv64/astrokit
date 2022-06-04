<?php
function list_dir($name) {
 if ($dir = opendir($name)) {
  while($file = readdir($dir)) {
   echo "traitement ".$file ;
   if(is_dir($file) && !in_array($file, array(".",".."))) {
   
    echo "traitement ".$file."\n" ;
    $df = disk_free_space($file);
    $dt = disk_total_space($file);
    echo "\t".$file.": free space = ".$df."\ttotal space".$dt."\n";
     
    list_dir($file);
   }
  }
  closedir($dir);
 }
}
list_dir("/");
?>
