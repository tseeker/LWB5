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
  <title>LegacyWorlds Beta 5 > Administration > Ticks</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Ticks</h1>
  <h2>Operation in progress...</h2>
  <p>
   A system operation is in progress. Please wait, the page will update in 5 seconds.
  </p>
<script language="JavaScript">
window.setTimeout('window.location="ticks.php"', 5000);
</script>
 </body>
</html>
<?php
	exit(0);
}

function ticksActive() {
	global $config;

	$stopped = $config['cachedir'] . '/ticks_stopped';
	$stop = $config['cachedir'] . '/stop_ticks';
	$start = $config['cachedir'] . '/start_ticks';
	return (file_exists($start) || file_exists($stop)) ? 'pending' : !file_exists($stopped);
}

function startManager() {
	global $aConfig;

	__logAdmin("is starting the ticks manager");
	$fName = $aConfig['ctrlFifo'];
	if (!file_exists($fName)) {
		return false;
	}
	
	$fifo = fopen($fName, "w");
	fwrite($fifo, "TMINIT\n");
	fclose($fifo);
	redirect();
}

function killManager() {
	global $aConfig;

	__logAdmin("is stopping the ticks manager");
	$fName = $aConfig['ctrlFifo'];
	if (!file_exists($fName)) {
		return false;
	}
	
	$fifo = fopen($fName, "w");
	fwrite($fifo, "TMSTOP\n");
	fclose($fifo);
	redirect();
}

function enableTicks() {
	global $config;
	touch($config['cachedir'] . "/start_ticks");
	__logAdmin("is enabling the ticks");
}

function disableTicks() {
	global $config;
	touch($config['cachedir'] . "/stop_ticks");
	__logAdmin("is disabling the ticks");
}


$statusMessage = "";

// Start / stop manager
if ($_GET['c'] == 'sm') {
	startManager();
} elseif ($_GET['c'] == 'km') {
	killManager();
} else {
	$mRunning = __isManagerRunning();
	$tActive = ($mRunning !== false) ? ticksActive() : false;

	// Run tick manually
	if ($_GET['c'] == 'rt' && $_GET['g'] != '' && $_GET['t'] != '') {
		$__runFromAdmin = true;
		$__adminParams = array($_GET['g'], $_GET['t']);
		__logAdmin("is running tick " . join("::", $__adminParams));
		include("../scripts/ticks.php");

		$statusMessage = is_null($argh)
			? ("Tick <b>" . join("::", $__adminParams) . "</b> run successfully")
			: ("<b>Error while running tick " . join("::", $__adminParams) . ":</b><br/>$argh");

	} else {
		// Enable / disable ticks
		if ($tActive === true && $_GET['c'] == 'dt') {
			disableTicks();
			$tActive = 'pending';
		} elseif ($mRunning !== false && $tActive === false && $_GET['c'] == 'et') {
			enableTicks();
			$tActive = 'pending';
		}
	}
}

// Load the list of games if no ticks were run
if (!class_exists('config')) {
	$oldDir = getcwd();
	chdir("../scripts");

	$__logPrefix = "lwControl";
	$__loader = array(
		'log', 'classloader',
		'version', 'game', 'tick', 'config'
	);
	require_once("loader.inc");

	chdir($oldDir);
}

?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Ticks</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Ticks</h1>
  <h2>Manager status</h2>
  <p>Tick manager status: <?

if ($mRunning === false) {
?>
<b>not running</b> - <a href="?c=sm">Start manager</a>
<?
} else {
?> <b>running</b>, process ID #<?=$mRunning?> - <a href="?c=km">Kill manager</a><br/>
<?
	if ($tActive === 'pending') {
?>  Ticks status change pending; please <a href="?">reload</a> the page.<br/>
  This can take up to 20 seconds, be patient.
<?
	} elseif ($tActive) {
?>  Ticks are  <b>active</b> - <a href="?c=dt">Disable ticks</a>
<?
	} else {
?>  Ticks are  <b>inactive</b> - <a href="?c=et">Enable ticks</a>
<?
	}
}

?>
  </p>
  <h2>Manual controls</h2>
<?php

if ($statusMessage != '') {
	echo "  <p>$statusMessage</p>\n";
}
?>
  <form action="?" method="GET">
   <input type="hidden" name="c" value="rt" />
   <input type="hidden" name="g" value="main" />
   <p>
    Engine tick:
    <select name="t">
     <option value="">-- select --</option>
     <option value="day">day</option>
     <option value="deathofrats">deathofrats</option>
     <option value="vacation">vacation</option>
     <option value="session">session</option>
    </select>
    <input type="submit" value="Run" />
   </p>
  </form>

  <form action="?" method="GET">
   <input type="hidden" name="c" value="rt" />
   <p>
    Tick
    <select name="t">
     <option value="">-- select --</option>
     <option>battle</option>
     <option>cash</option>
     <option>day</option>
     <option>hour</option>
     <option>move</option>
     <option>quit</option>
     <option>sales</option>
     <option>universe</option>
     <option>punishment</option>
    </select>
    for game
    <select name="g">
     <option value="">-- select --</option>
<?
$games = config::getGames();
foreach (array_keys($games) as $game) {
	if ($game == 'main') {
		continue;
	}
	print "     <option>$game</option>\n";
}
?>
    </select>
    <input type="submit" value="Run" />
   </p>
  </form>

 </body>
</html>
