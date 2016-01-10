<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Maintenance mode</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Maintenance mode</h1>
<?php

set_magic_quotes_runtime(false);

include('../scripts/config.inc');
include('as_log.inc');

$err = $reason = $duration = null;
if ($_GET['disable'] == '1' && !is_null($config['maintenance'])) {
	unlink($config['cachedir'] . '/maintenance.ser');
	include('../scripts/config.inc');

	__logAdmin("put the server out of maintenance mode");
} elseif ($_POST['enable'] != '' && is_null($config['maintenance'])) {
	$reason = $_POST['reason'];
	$duration = (int) $_POST['duration'];

	if (strlen($reason) < 10) {
		$err = "Reason too short (min 10 characters).";
	} elseif ($duration < 5) {
		$err = "Duration too short (min 5 minutes).";
	} else {
		$maintenance = array(
			"until" => time() + $duration * 60,
			"reason" => $reason
		);

		$f = fopen($config['cachedir'] . '/maintenance.ser', "w");
		fwrite($f, serialize($maintenance));
		fclose($f);
		include('../scripts/config.inc');

		__logAdmin("put the server in maintenance mode for reason: $reason");
	}
}

if (is_null($config['maintenance'])) {
?>
  <p>
   Maintenance mode is currently inactive. Please use the form below to activate it.
  </p>
  <form method="POST" action="?">
   <p>
    Reason for maintenance: <input type="text" maxlength="100" name="reason" size="40" value="<?=htmlentities($reason)?>" /><br/>
    Maintenance mode duration: <input type="text" maxlength="3" name="duration" size="4" value="<?=$duration?>" /> minutes
   </p>
<?php
	if ($err) {
		print "<p style='color:red'>$err</p>\n";
	}
?>
   <p>
    <input type="submit" name="enable" value="Activate maintenance mode" />
   </p>
  </form>
<?php
} else {
?>
  <p>
   Maintenance mode is currently <b>active</b>.
  </p>
  <p>
   <u>Reason:</u> <?=$config['maintenance']['reason']?><br/>
   <u>Until:</u> <?=gmstrftime("%H:%M:%S on %m/%d/%Y", $config['maintenance']['until'])?>
   (current: <?=gmstrftime("%H:%M:%S on %m/%d/%Y", time())?>).
  </p>
  <p><a href="?disable=1">Disable maintenance mode</a></p>
<?
}

?>
 </body>
</html>
