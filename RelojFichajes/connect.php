<?php


$mysql_servidor = "localhost";
$mysql_base = "cmcasado";
$mysql_usuario = "root";
$mysql_clave = "socram10";

$id_card  = htmlspecialchars($_GET["id_card"],ENT_QUOTES);
$reason = htmlspecialchars($_GET["reason"],ENT_QUOTES);


// Valida que esten presente todos los parametros
if (($id_card!="") and ($reason!="")) {
	mysql_connect($mysql_servidor,$mysql_usuario,$mysql_clave) or die("Imposible conectarse al servidor.");
	mysql_select_db($mysql_base) or die("Imposible abrir Base de datos");
	$sql = "INSERT INTO `cmcasado`.`fichajes` (`id`, `id_card`, `reason`, `date`) VALUES (NULL, '$id_card', '$reason', CURRENT_TIMESTAMP)";
	mysql_query($sql) or die ("Error en la sentencia SQL");

}else{
die ("La peticion HTTP/1.1 no es valida, al menos uno de los parametros se estan enviando vacios. Metodo /GET");
}
?>
