<?php

set_magic_quotes_runtime(false);

include("config.inc");
include("as_log.inc");

function redirect() {
?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Game status</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Game status</h1>
  <h2>Operation in progress...</h2>
  <p>
   A system operation is in progress. Please wait, the page will update in 5 seconds.
  </p>
<script language="JavaScript">
window.setTimeout('window.location="game_status.php"', 5000);
</script>
 </body>
</html>
<?php
	exit(0);
}

function printStatus($status) {
	static $styles = array(
		"PRE"		=> array('yellow', 'red'),
		"READY"		=> array('red', 'yellow'),
		"RUNNING"	=> array('white', 'green'),
		"VICTORY"	=> array('yellow', 'blue'),
		"ENDING"	=> array('black', 'yellow'),
		"FINISHED"	=> array("white", "black")
	);

	print "<th style='color:" . $styles[$status][0] . ";background-color:"
		. $styles[$status][1] . "'>$status</th>";
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

	redirect();
}

function handleCommand($command, $game) {
	if ($command == 'mv' && $game->status() == 'PRE') {
		__logAdmin("is making game {$game->name} visible");
		sendFifo("READY {$game->name}");
	} elseif ($command == 'te' && $game->status() == 'READY' && $game->firstTick() - time() > 24 * 60 * 60 + 30) {
		__logAdmin("made game {$game->name} start 24h earlier");
		sendFifo("START {$game->name} EARLY");
	} elseif ($command == 'tl' && $game->status() == 'READY') {
		__logAdmin("made game {$game->name} start 24h later");
		sendFifo("START {$game->name} LATE");
	} elseif ($command == 'en' && ($game->status() == 'RUNNING' || $game->status() == "VICTORY")) {
		__logAdmin("terminated game {$game->name}");
		sendFifo("SETEND {$game->name} 0");
	} elseif ($command == 'e24' && $game->status() == 'RUNNING') {
		__logAdmin("set game {$game->name} to end in 24h");
		sendFifo("SETEND {$game->name} 24");
	} elseif ($command == 'kr' && $game->status() == 'ENDING') {
		__logAdmin("prevented game {$game->name} from ending");
		sendFifo("NOEND {$game->name}");
	} elseif ($command == 'ee' && $game->status() == 'ENDING' && $game->lastTick() - time() > 24 * 60 * 60 + 30) {
		__logAdmin("made game {$game->name} end 24h earlier");
		sendFifo("END {$game->name} EARLY");
	} elseif ($command == 'el' && $game->status() == 'ENDING') {
		__logAdmin("made game {$game->name} end 24h later");
		sendFifo("END {$game->name} LATE");
	} elseif ($command == 'en' && $game->status() == 'ENDING') {
		__logAdmin("terminated game {$game->name}");
		sendFifo("END {$game->name} NOW");
	}
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

$games = config::getGames();
dbConnect();


// Handle commands
if ($_GET['c'] != '') {
	$cGame = $_GET['g'];
	if (array_key_exists($cGame, $games) && $cGame != 'main') {
		handleCommand($_GET['c'], $games[$cGame]);
	}
}


?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Game status</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Game status</h1>

  <h2>Game list</h2>
  <p>
   <b>WARNING:</b> make sure you know what you're doing here - there are no second chances on this page;
   if you click something, that "something" <i>will</i> happen <i>at once</i>.
  </p>
  <table border="1" width="100%">
   <tr>
    <th style="text-align:left; width:10%">ID</th>
    <th style="width:10%">Status</th>
    <th style="text-align:left;width:20%">Name</th>
    <th style="text-align:left">Ticks</th>
    <th style="text-align:left;width:30%">Commands</th>
   </tr>
<?php
foreach ($games as $name => $game) {
	if ($name == 'main') {
		continue;
	}

	$status = $game->status();
	$firstTick = $game->firstTick();
	$lastTick = $game->lastTick();

	print "   <tr>\n    <td><b>$name</b></td>\n";
	printStatus($status);
	print "     <td>" . htmlentities($game->text) . "</td>\n";
	print "     <td>";
	if ($firstTick > time()) {
		print "Starting at " . gmstrftime('%H:%M:%S on %Y-%m-%d', $firstTick);
	} elseif ($lastTick == 0) {
		print "Running since " . gmstrftime('%H:%M:%S on %Y-%m-%d', $firstTick);
	} elseif ($lastTick > time()) {
		print "Running until " . gmstrftime('%H:%M:%S on %Y-%m-%d', $lastTick);
	} else {
		print "Stopped since " . gmstrftime('%H:%M:%S on %Y-%m-%d', $lastTick);
	}

	if ($status == 'PRE') {
		$cmd = array(
			array('mv', 'Make visible')
		);
	} elseif ($status == 'READY') {
		$cmd = array(
			array('tl', 'Start 24h later')
		);
		if ($firstTick - time() > 24 * 60 * 60 + 30) {
			array_push($cmd, array('te', 'Start 24h earlier'));
		}
	} elseif ($status == 'RUNNING') {
		$cmd = array(
			array('e24', 'End in 24h'),
			array('en', 'End now')
		);
	} elseif ($status == 'VICTORY') {
		$cmd = array(
			array('en', 'End game')
		);
	} elseif ($status == 'ENDING') {
		$cmd = array(
			array('el', 'Postpone by 24h')
		);
		if ($lastTick - time() > 24 * 60 * 60 + 30) {
			array_push($cmd, array('ee', 'End 24h earlier'));
		}
		array_push($cmd, array('en', 'End now'));
		array_push($cmd, array('kr', 'Keep running'));
	} else {
		$cmd = array();
	}

	print "</td>\n    <td>";
	if (count($cmd)) {
		$lk = array();
		foreach ($cmd as $c) {
			array_push($lk, "<a href='?c=" . $c[0] . "&g=$name' onclick=\"return confirm('You selected \\'"
				. $c[1] . "\\' on game \\'$name\\'. Please confirm.');\">" . $c[1] . "</a>");
		}
		print join(' - ', $lk);
	} else {
		print "&nbsp;";
	}

	print "</td>\n   </tr>\n";
}

?>
  </table>

  <h3>About game status</h3>
  <p>Games can have the following status:</p>
  <table>
   <tr>
    <?printStatus('PRE');?>
    <td>The game is configured, but is hidden for now</td>
   </tr>
   <tr>
    <?printStatus('READY');?>
    <td>The game is visible, but ticks have not started</td>
   </tr>
   <tr>
    <?printStatus('RUNNING');?>
    <td>The game is running normally</td>
   </tr>
   <tr>
    <?printStatus('VICTORY');?>
    <td>The game is still running but someone reached victory</td>
   </tr>
   <tr>
    <?printStatus('ENDING');?>
    <td>The game is still available but is about to end.</td>
   </tr>
   <tr>
    <?printStatus('FINISHED');?>
    <td>The game is no longer running and only visible through the rankings page</td>
   </tr>
  </table>

 </body>
</html>
