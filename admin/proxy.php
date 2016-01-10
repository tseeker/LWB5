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
  <title>LegacyWorlds Beta 5 > Administration > Proxy detector</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Proxy detector</h1>
  <h2>Operation in progress...</h2>
  <p>
   A system operation is in progress. Please wait, the page will update in 5 seconds.
  </p>
<script language="JavaScript">
window.setTimeout('window.location="proxy.php"', 5000);
</script>
 </body>
</html>
<?php
	exit(0);
}


function startDetector() {
	__logAdmin("is starting the proxy detector");
	__sendControl("PCON");
	redirect();
}

function stopDetector() {
	__logAdmin("is stopping the proxy detector");
	__sendControl("PCOFF");
	redirect();
}


$oldDir = getcwd();
chdir("../scripts");
$__loader = array(
	'log', 'classloader',
	'version', 'game', 'tick',
	'config', 'pcheck'
);
require_once("loader.inc");
chdir($oldDir);


$isRunning = pcheck::isRunning();

if ($_GET['c'] == 'sd') {
	startDetector();
} elseif ($_GET['c'] == 'kd') {
	stopDetector();
} elseif ($_GET['ip'] != '') {
	$ip = $_GET['ip'];
	$status = "";
	if (preg_match('/^\d{1,3}(\.\d{1,3}){3}$/', $ip)) {
		if ($ip == "127.0.0.1") {
			$status = "Host not allowed";
		} else {
			$addr = explode('.', $ip);
			foreach ($addr as $piece) {
				if ($piece > 254) {
					$status = "Invalid IP address";
					break;
				}
			}
			if ($status == "") {
				try {
					$result = pcheck::check(array($ip));
					$status = "$ip - ";
					switch ($result[$ip]) {
						case -1: $status .= "detection failed"; break;
						case 0: $status .= "no proxy detected"; break;
						case 1: $status .= "OPEN PROXY DETECTED!"; break;
					}
				} catch (Exception $e) {
					$status = $e->getMessage();
				}
			}
		}
	} else {
		$status = "Invalid IP address";
	}
} else {
	$status = $ip = "";
}

?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Proxy detector</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Proxy detector</h1>
<?php

if ($isRunning) {
?>
 <p>
  Proxy detector is <b>running</b>; process ID #<?=$isRunning?>. <a href="?c=kd">Stop detector</a>
 </p>
 <form action="?" method="GET">
  <p>
   Manually check address <input type="text" size="16" maxlength="15" name="ip" value="<?=htmlentities($ip, ENT_QUOTES)?>" />
   <input type="submit" value="Scan" />
   <?=($status != '') ? ('<br/><b>' . $status . '</b>') : ''?>
  </p>
 </form>
<?php
} else {
?>
 <p>
  Proxy detector is <b>not running</b>. <a href="?c=sd">Start detector</a>
 </p>
<?php
}
?>
 </body>
</html>
