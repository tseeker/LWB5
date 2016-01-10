<?php

set_magic_quotes_runtime(false);

include('config.inc');
include('as_log.inc');

function redirect() {
?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Default game</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Default game</h1>
  <h2>Operation in progress...</h2>
  <p>
   A system operation is in progress. Please wait, the page will update in 2 seconds.
  </p>
<script language="JavaScript">
window.setTimeout('window.location="set_default.php"', 2000);
</script>
 </body>
</html>
<?php
	exit(0);
}

function sendFifo($command) {
	global $aConfig;

	$fName = $aConfig['ctrlFifo'];
	if (!file_exists($fName)) {
		return false;
	}
	
	$fifo = fopen($fName, "w");
	fwrite($fifo, "$command\n");
	fclose($fifo);
}


// Load the list of games
$oldDir = getcwd();
chdir("../scripts");
$__logPrefix = "lwControl";
$__loader = array(
	'log', 'classloader',
	'version', 'game', 'tick', 'config',
	'db_connection', 'db_accessor', 'db',
	'library'
);
require_once("loader.inc");
chdir($oldDir);

dbConnect();

if ($_GET['d'] != '') {
	$cDef = config::getDefaultGame();
	$games = config::getGames();
	if ($cDef->name != $_GET['d'] && array_key_exists($_GET['d'], $games) && $_GET['d'] != 'main') {
		sendFifo("SETDEF {$_GET['d']}");
		redirect();
	}
}

$games = config::getGames();
$defGame = config::getDefaultGame();

?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Default game</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Default game</h1>
  <p>
   The <b>default game</b> is the game for which overall round rankings are displayed on the site's
   main page.
  </p>
  <form action="?" method="GET">
   <p>
    Current default game:
    <select name="d">
<?php

foreach ($games as $id => $game) {
	if ($id == "main" || $game->status() == 'PRE') {
		continue;
	}
	print "     <option value='$id'";
	if ($defGame->name == $id) {
		echo " selected='selected'";
	}
	echo ">" . htmlentities($game->text) . "</option>\n";
}

?>
    </select>
    <input type="submit" value="Change" />
   </p>
  </form>
 </body>
</html>
