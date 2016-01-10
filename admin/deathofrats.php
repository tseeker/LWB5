<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Death of Rats</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Death of Rats</h1>
  <p>
   The Death of Rats is LegacyWorlds' experimental multi detection tool.<br/>
   Select page:
<?

set_magic_quotes_runtime(false);

include('config.inc');

switch ($_GET['p']) {
	case 'e': $page = 'execution'; break;
	case 'sl': $page = 'singlelog'; break;
	case 'sp': $page = 'signlepoints'; break;
	case 'ml': $page = 'multilog'; break;
	case 'mp': $page = 'multipoints'; break;
	case 'al': $page = 'actions'; break;
	case 'fp': $page = 'finalpoints'; break;
	case 'il': $page = 'ingamelog'; break;
	default: $page = "status"; break;
}

$pages = array(
	'status'	=> array('s', 'Current status', 'showStatus'),
	'actions'	=> array('al', 'Actions taken', 'showActionLog'),
	'finalpoints'	=> array('fp', 'Decision points', 'showFinalPoints'),
	'ingamelog'	=> array('il', 'In-game checks', 'showInGameChecks'),
	'multipoints'	=> array('mp', 'Multiplayer points', 'showMultiPoints'),
	'multilog'	=> array('ml', 'Multiplayer log', 'showMultiLog'),
	'signlepoints'	=> array('sp', 'Single player points', 'showSinglePoints'),
	'singlelog'	=> array('sl', 'Single player events', 'showSingleLog'),
	'execution'	=> array('e', 'Execution log', 'showExecLog'),
);

foreach ($pages as $pName => $data) {
	if ($pName == $page) {
		echo "<b>";
	} else {
		echo "<a href='?p={$data[0]}'>";
	}
	echo $data[1];
	if ($pName == $page) {
		echo "</b> ";
	} else {
		echo "</a> ";
	}
}
echo "</p>";

$func = $pages[$page][2];
$func();


function showStatus() {
?>
<h2>Current status</h2>
<p>
 The Death of Rats is still in an <b><u>experimental</u></b> stage at this time, and no actual action is taken. What
 it <i>would</i> do if it were fully enabled is logged on the "Actions taken" page nonetheless.<br/>
 Most of the information it provides can be trusted, tho; however, if you suspect it sent a warning or "punished" a
 player for no good reason, manual checks should be performed.
</p>
<p>
 The following checks are performed:
</p>
<ul>
 <li>use of open proxies</li>
 <li>cookie deletion</li>
 <li>trying to log on with a banned account</li>
 <li>passwords shared between multiple accounts</li>
 <li>simple multiing, as well as pass sharing</li>
 <li>"vicious" multiing, implying that the cookies are cleared between each use</li>
 <li>in-game checks for donations, tech exchanges, gifts and sales, and planet retake after abandon</li>
</ul>
<p>
 Checks for concurrent session from the same IP as well as more in-game checks (alliance, posts, messages,
 TA list, battles) are still missing.
</p>
<h2>About the different pages</h2>
<p>
 This tool consists in a few different pages which give different information about the Death of Rats' current status.
 These pages are:
</p>
<ul>
 <li>
  <b>Actions taken</b>: the log of all actions the Death of Rats performed. This includes sending
  warnings and deciding to punish players.
 </li>
 <li>
  <b>Decision points</b>: the current amount of points for each pair of players that has been investigated thoroughly
  by the Death of Rats. Pairs with over 100 points will cause the Death of Rats to act.
 </li>
 <li>
  <b>In-game checks log</b>: the latest 400 entries of the checks performed in-game on suspicious players as well
  as the results of these checks.
 </li>
 <li>
  <b>Multiplayer points</b>: the current amount of points for each pair of accounts that could be multiing or
  abusing pass-sharing.
 </li>
 <li>
  <b>Multiplayer log</b>: the latest 200 entries of the suspicious events detected between pairs of accounts.
 </li>
 <li>
  <b>Single player points</b>: the current amount of points for each account that has performed suspicious actions.
  While these points are normally not a problem, they influence multiplayer scores.
 </li>
 <li>
  <b>Single player log</b>: the latest 200 suspicious events detected for single accounts.
 </li>
 <li>
  <b>Execution log</b>: the latest 200 runs of the Death of Rats.
 </li>
</ul>
<?
}

function showExecLog() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT * FROM main.dor_exec ORDER BY ts DESC LIMIT 200");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
?>
<h2>Previous 200 runs of the Death of Rats</h2>
<p>
 This page shows the log of the previous 200 executions of the Death of Rats tick. The <i>Changes</i> column indicates
 the amount of changes (connection records, password updates) examined; the <i>Events</i> column indicates the amount
 of entries added to either the single player log or the multiplayer log.
</p>
<table border="1">
 <tr>
  <th>Time &amp; date</th>
  <th>Changes</th>
  <th>Events</th>
 </tr>
<?
	foreach ($entries as $entry) {
?>
 <tr>
  <td align="center"><?=$entry['events'] ? "<b>" : ""?><?=gmstrftime("%H:%M:%S / %Y-%m-%d", $entry['ts'])?><?=$entry['events'] ? "</b>" : ""?></td>
  <td align="center"><?=$entry['events'] ? "<b>" : ""?><?=$entry['entries']?><?=$entry['events'] ? "</b>" : ""?></td>
  <td align="center"><?=$entry['events'] ? "<b>" : ""?><?=$entry['events']?><?=$entry['events'] ? "</b>" : ""?></td>
 </tr>
<?
	}
?>
</table>
<?
}

function showSingleLog() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT a.name,l.message,l.ts FROM main.dor_single l, main.account a WHERE a.id = l.account AND a.status NOT IN ('QUIT', 'INAC', 'KICKED') ORDER BY l.ts DESC,a.name ASC LIMIT 200");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
?>
<h2>Previous 200 single player log entries</h2>
<p>
 This page shows the log of the previous 200 log entries generated for single players.
</p>
<table border="1">
 <tr>
  <th>Time &amp; date</th>
  <th>Account</th>
  <th align="left">Message</th>
 </tr>
<?

	$messages = array(
		"ASSHOLE"	=> "Tried to log on using a banned account",
		"PROXY"		=> "Currently using an open proxy",
		"CLCOOK-SIP"	=> "Cleared cookies from the same IP",
		"CLCOOK-DIP"	=> "Cleared cookies from a different (but close) IP",
	);

	foreach ($entries as $entry) {
?>
 <tr>
  <td align="center"><?=gmstrftime("%H:%M:%S / %Y-%m-%d", $entry['ts'])?>
  <td align="center"><?=htmlentities($entry['name'])?></td>
  <td><?=$messages[$entry['message']]?></td>
 </tr>
<?
	}
?>
</table>
<?
}

function showSinglePoints() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT a.name,l.points FROM main.dor_single_points l, main.account a WHERE a.id = l.account AND a.status NOT IN ('QUIT', 'INAC', 'KICKED') ORDER BY l.points DESC,a.name ASC");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
?>
<h2>Single player "badness points"</h2>
<p>
 These points correspond to recent suspicious activities from active accounts.
</p>
<table border="1">
 <tr>
  <th>Account</th>
  <th>Points</th>
 </tr>
<?
	foreach ($entries as $entry) {
?>
 <tr>
  <td align="center"><?=htmlentities($entry['name'])?></td>
  <td align="center"><?=$entry['points']?></td>
 </tr>
<?
	}
?>
</table>
<?
}

function showInGameChecks() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT a1.name as name1, a2.name as name2, l.message, l.ts, l.game FROM main.dor_ingame_check l, main.account a1, main.account a2 WHERE a1.id = l.account1 AND a2.id = l.account2 AND a1.status NOT IN ('QUIT', 'INAC', 'KICKED') AND a2.status NOT IN ('QUIT', 'INAC', 'KICKED') ORDER BY l.ts DESC,a1.name ASC, a2.name ASC LIMIT 400");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
?>
<h2>Latest 400 in-game checks log entries</h2>
<p>
 This page shows the log of the latest 400 log entries generated by in-game checks on players. Events logged here
 belong to different categories:
</p>
<ul>
 <li><b>Somewhat suspicious events</b>: donations of less than &euro;100,000; rejected tech offers</li>
 <li>
  <b>Suspicious events</b>: donations of less than &euro;1,000,000; pending tech offers; accepted techs offer with
  a price greater than &euro;1,000; planets taken by a player within 5 days of being abandonned by the other; sales
  of planets or fleets.
 </li>
 <li>
  <b>Highly suspicious events</b>: donations of less than &euro;10,000,000; gifts; tech offers with a price lower
  than &euro;1,000.
 </li>
 <li>
  <b>Extremely suspicious events</b>: donations of more than &euro;10,000,000.
 </li>
</ul>
<table border="1">
 <tr>
  <th>Time &amp; date</th>
  <th>Game ID</th>
  <th>Account 1</th>
  <th>Account 2</th>
  <th align="left">Message</th>
  <th>Count</th>
 </tr>
<?

	$messages = array(
		"CHECK"		=> "Verifying accounts",
		"VHSE"		=> "Extremely suspicious in-game events",
		"HSE"		=> "Highly suspicious in-game events",
		"SE"		=> "Suspicious in-game events",
		"LSE"		=> "Somewhat suspicious in-game events"
	);

	foreach ($entries as $entry) {
		list($message, $count) = explode('-', $entry['message']);
		if ($message == 'CHECK') {
			$count = "N/A";
		}
?>
 <tr>
  <td align="center"><?=gmstrftime("%H:%M:%S / %Y-%m-%d", $entry['ts'])?>
  <td align="center"><?=$entry['game']?></td>
  <td align="center"><?=htmlentities($entry['name1'])?></td>
  <td align="center"><?=htmlentities($entry['name2'])?></td>
  <td><?=$messages[$message]?></td>
  <td align="center"><?=$count?></td>
 </tr>
<?
	}
?>
</table>
<?
}


function showMultiLog() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT (a1.id || ',' || a2.id) as id, a1.name as name1, a2.name as name2, l.message, l.ts FROM main.dor_multi l, main.account a1, main.account a2 WHERE a1.id = l.account1 AND a2.id = l.account2 AND a1.status NOT IN ('QUIT', 'INAC', 'KICKED') AND a2.status NOT IN ('QUIT', 'INAC', 'KICKED') ORDER BY l.ts DESC,a1.name ASC, a2.name ASC LIMIT 400");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
	$displayed = array();
?>
<h2>Previous 200 multiplayer log entries</h2>
<p>
 This page shows the log of the previous 200 log entries generated for pairs of players.
</p>
<table border="1">
 <tr>
  <th>Time &amp; date</th>
  <th>Account 1</th>
  <th>Account 2</th>
  <th align="left">Message</th>
 </tr>
<?

	$messages = array(
		"SIMPLE"	=> "Simple multiing / open pass sharing detected",
		"SIMPLE-10"	=> "Simple multiing / open pass sharing detected (within 10 seconds!)",
		"PASS"		=> "Accounts are using the same password",
		"NOPASS"	=> "Accounts are no longer using the same password",
		"VICIOUS-LP"	=> "Potential attempt to conceal pass-sharing",
		"VICIOUS-MP"	=> "Probable attempt to conceal pass-sharing",
		"VICIOUS-HP"	=> "Highly probable attempt to conceal pass-sharing",
	);

	foreach ($entries as $entry) {
		$id = explode(',', $entry['id']);
		sort($id);
		$id = join(',', $id) . "-" . $entry['ts'] . "-" . $entry['message'];
		if (in_array($id, $displayed)) {
			continue;
		}
		$displayed[] = $id;
?>
 <tr>
  <td align="center"><?=gmstrftime("%H:%M:%S / %Y-%m-%d", $entry['ts'])?>
  <td align="center"><?=htmlentities($entry['name1'])?></td>
  <td align="center"><?=htmlentities($entry['name2'])?></td>
  <td><?=$messages[$entry['message']]?></td>
 </tr>
<?
	}
?>
</table>
<?
}

function showFinalPoints() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT a1.name as name1, a2.name as name2, l.points FROM main.dor_final_points l, main.account a1, main.account a2 WHERE a1.id = l.account1 AND a2.id = l.account2 AND a1.status NOT IN ('QUIT', 'INAC', 'KICKED') AND a2.status NOT IN ('QUIT', 'INAC', 'KICKED') ORDER BY l.points DESC,a1.name ASC, a2.name ASC");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
?>
<h2>Final decision points</h2>
<p>
 This page displays the current amount of points for each pair of players that has been investigated thoroughly
 by the Death of Rats. Pairs will over 100 points will cause the Death of Rats to act.
</p>
<table border="1">
 <tr>
  <th>Account 1</th>
  <th>Account 2</th>
  <th>Points</th>
 </tr>
<?
	foreach ($entries as $entry) {
?>
 <tr>
  <td align="center"><?=htmlentities($entry['name1'])?></td>
  <td align="center"><?=htmlentities($entry['name2'])?></td>
  <td><?=$entry['points']?></td>
 </tr>
<?
	}
?>
</table>
<?
}


function showMultiPoints() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db, "SELECT (a1.id || ',' || a2.id) as id, a1.name as name1, a2.name as name2, l.points FROM main.dor_multi_points l, main.account a1, main.account a2 WHERE a1.id = l.account1 AND a2.id = l.account2 AND a1.status NOT IN ('QUIT', 'INAC', 'KICKED') AND a2.status NOT IN ('QUIT', 'INAC', 'KICKED') ORDER BY l.points DESC,a1.name ASC, a2.name ASC");
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
	$displayed = array();
?>
<h2>Multiplayer "badness points"</h2>
<p>
 This page shows the list of "badness points" between pairs of accounts. The higher the badness points, the more likely
 the accounts are multis.
</p>
<table border="1">
 <tr>
  <th>Account 1</th>
  <th>Account 2</th>
  <th>Points</th>
 </tr>
<?
	foreach ($entries as $entry) {
		$id = explode(',', $entry['id']);
		sort($id);
		$id = join(',', $id);
		if (in_array($id, $displayed)) {
			continue;
		}
		$displayed[] = $id;
?>
 <tr>
  <td align="center"><?=htmlentities($entry['name1'])?></td>
  <td align="center"><?=htmlentities($entry['name2'])?></td>
  <td><?=$entry['points']?></td>
 </tr>
<?
	}
?>
</table>
<?
}


function showActionLog() {
	$db = __dbConnect();
	$entries = array();
	$q = pg_query($db,
		"SELECT * FROM ("
			. "SELECT a1.name AS name1, a2.name AS name2, l.ts AS ts, 'WARN' AS atype "
					. "FROM main.dor_warning l, main.account a1, main.account a2 "
				. "WHERE a1.id = l.account1 AND a2.id = l.account2 "
				  . "AND a1.status NOT IN ('QUIT', 'INAC', 'KICKED') "
				  . "AND a2.status NOT IN ('QUIT', 'INAC', 'KICKED') "
			. "UNION SELECT a1.name AS name1, a2.name AS name2, l.ts AS ts, 'PUNISH' AS atype "
					. "FROM main.dor_warning l, main.account a1, main.account a2 "
				. "WHERE a1.id = l.account1 AND a2.id = l.account2 "
				  . "AND a1.status NOT IN ('QUIT', 'INAC', 'KICKED') "
				  . "AND a2.status NOT IN ('QUIT', 'INAC', 'KICKED')"
		. ") AS t ORDER BY t.ts DESC, t.name1 ASC, t.name2 ASC"
	);
	while ($r = pg_fetch_assoc($q)) {
		$entries[] = $r;
	}
	pg_close($db);
?>
<h2>Actions performed by the Death of Rats</h2>
<p>
 This page lists all the actions the Death of Rats has performed.
</p>
<table border="1">
 <tr>
  <th>Time &amp; date</th>
  <th>Account 1</th>
  <th>Account 2</th>
  <th align="left">Message</th>
 </tr>
<?
	$messages = array(
		"WARN"		=> "Warned player",
		"PUNISH"	=> "Slaughtered player with a rat-sized scythe"
	);

	foreach ($entries as $entry) {
?>
 <tr>
  <td align="center"><?=gmstrftime("%H:%M:%S / %Y-%m-%d", $entry['ts'])?>
  <td align="center"><?=htmlentities($entry['name1'])?></td>
  <td align="center"><?=htmlentities($entry['name2'])?></td>
  <td><?=$messages[$entry['atype']]?></td>
 </tr>
<?
	}
?>
</table>
<?
}

?>
 </body>
</html>
