<?php

set_magic_quotes_runtime(false);

include('config.inc');
include('../scripts/config.inc');
include('as_manager.inc');
include('as_log.inc');

function redirect() {
?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > IRC bot</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > IRC bot</h1>
  <h2>Operation in progress...</h2>
  <p>
   A system operation is in progress. Please wait, the page will update in a few seconds.
  </p>
<script language="JavaScript">
window.setTimeout('window.location="bot.php"', 1000);
</script>
 </body>
</html>
<?php
	exit(0);
}

function sendCommand($cmd) {
	global $aConfig;

	$fName = $aConfig['ctrlFifo'];
	if (!file_exists($fName)) {
		return false;
	}
	
	$fifo = fopen($fName, "w");
	fwrite($fifo, "$cmd\n");
	fclose($fifo);
	redirect();
}


if ($_GET['c'] == 'kb') {
	__logAdmin("is stopping the IRC bot");
	sendCommand("BOTOFF");
} elseif ($_GET['c'] == 'sb') {
	__logAdmin("is starting the IRC bot");
	sendCommand("BOTON");
}

$running = file_exists($config['cs_path'] . "/ircbot.pid");

?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > IRC bot</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > IRC bot</h1>
<?php

if ($running) {
?>
 <p>
  IRC bot is (probably) running. <a href="?c=kb">Kill bot</a>
 </p>
<?php
} else {
?>
 <p>
  IRC bot is not running. <a href="?c=sb">Start bot</a>
 </p>
<?php
}
?>
 </body>
</html>
