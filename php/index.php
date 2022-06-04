<html>
<head>
<title>Ma galerie</title>
</head>

<body bgcolor=grey>
<?php
// on déclare un tableau qui contiendra le nom des fichiers de nos miniatures
$tableau = array();
// on ouvre notre dossier contenant les miniatures
$dossier = opendir ('.');
while ($fichier = readdir ($dossier)) {
        $fTyp = filetype($fichier) ;
        $fExt = pathinfo($fichier, PATHINFO_EXTENSION) ;
        //print "<br>fichier : $fichier : type : " . $fTyp . "extension : " . $fExt ;
	if ( $fichier != '.' && $fichier != '..' && $fichier != 'index.php' && $fTyp != "dir" ) {
          if ( $fTyp == "file" && ( $fExt == "jpg" || $fExt == "JPG" )) {
            $sizes = getimagesize($fichier);
            if ( $sizes[0] > $sizes[1]  ) $tab0[] = $fichier;
            else                          $tab1[] = $fichier;
          }
	}
}
closedir ($dossier);

$nbcol=4;
$nbpics = count($tab0);
if ($nbpics != 0) {echo '<table>'; for ($i=0; $i<$nbpics; $i++){ if($i%$nbcol==0) echo '<tr>';
	echo '<td><a href="./' , $tab0[$i] , '"><img src="./' , $tab0[$i] , '" alt="Image" width="300px" /></a></td>';
	if($i%$nbcol==($nbcol-1)) echo '</tr>'; } echo '</table>'; }

$nbcol=4;
$nbpics = count($tab1);
if ($nbpics != 0) {echo '<table>'; for ($i=0; $i<$nbpics; $i++){ if($i%$nbcol==0) echo '<tr>';
	echo '<td><a href="./' , $tab1[$i] , '"><img src="./' , $tab1[$i] , '" alt="Image" width="300px" /></a></td>';
	if($i%$nbcol==($nbcol-1)) echo '</tr>'; } echo '</table>'; }

?>
</body>
</html>
