<?php

date_default_timezone_set("Asia/Tehran");

   if( $_REQUEST["RFIDReader"] ||  $_REQUEST["RFIDTag"]  ) 
   {
   echo " The RFID Reader is: ". $_REQUEST['reader']. "<br />";
   echo " The Identified Tag is: ". $_REQUEST['tag']. " <br />";
   }
	  
	
$var1 = $_REQUEST['reader'];
$var2 = $_REQUEST['tag'];

$WriteMyRequest =
"Date: "  . date("Y/m/d") . " <br> Time: " . date("H:i:s") .
"<br> RFID Reader: "  . $var1 . " <br>".
"Identified Tag: " . $var2 . " <br> <br>"; 

$file = 'index.html';
$current = file_get_contents($file);
$final = $WriteMyRequest . $current;
file_put_contents($file, $final);


?>
