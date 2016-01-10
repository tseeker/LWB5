<?
include("menu.en.inc");
$this->title = "My account";
$this->addScript("account");
$this->addStylesheet("account");
$this->startContents();

function displayUnregistered($a) {
	static $status = array(
		'READY'		=> 'Open for registration',
		'RUNNING'	=> 'Running',
		'VICTORY'	=> 'Victory',
		'ENDING'	=> 'Ending'
	);
?>
 <table>
  <tr><th style="text-align:left">Game name</th><th>Players</th><th style="text-align:left">Status</th></tr>
<?php
	foreach	($a as $g => $c) {
		echo "<tr><td>";
		if ($c[3]) {
			echo "<a href='".makeLink('play')."?g=$g'>{$c[0]}</a>";
		} else {
			echo "{$c[0]} (closed)";
		}
		echo "</td><td style='text-align:center'>{$c[1]}</td>";
		echo "<td>".$status[$c[2]]."</td>";
		echo "</tr>\n";
		if ($c[2] == "READY") {
			print "<tr><td>&nbsp;</td><td colspan='2' style='text-align:center'>Starting at "
				. gmstrftime('%H:%M:%S on %Y-%m-%d', $c[4]) . "</td></tr>";
		} elseif ($c[2] == "ENDING") {
			print "<tr><td>&nbsp;</td><td colspan='2' style='text-align:center'>Ending at "
				. gmstrftime('%H:%M:%S on %Y-%m-%d', $c[4]) . "</td></tr>";
		}
	}
?>
 </table>
<?php
}


function displayRegistered($a) {
	static $status = array(
		'PRE'		=> 'Hidden',
		'READY'		=> 'Open',
		'RUNNING'	=> 'Running',
		'VICTORY'	=> 'Victory',
		'ENDING'	=> 'Ending'
	);
?>
 <table>
  <tr><th class="gname">Game name</th><th>Planets</th><th>Cash</th><th>Game status</th></tr>
<?php
	foreach	($a as $g => $c) {
		echo "<tr><td><a href='".makeLink('play',$g)."'>{$c[0]}</a></td>";
		echo "<td style='text-align:center'>{$c[1]}</td><td style='text-align:center'>&euro;"
			. number_format($c[2]) . "</td>";
		echo "<td style='text-align:center'>" . $status[$c[4]] . "</td></tr>\n";
		if ($c[4] == "READY") {
			print "<tr><td>&nbsp;</td><td colspan='3' style='text-align:center'>Starting at "
				. gmstrftime('%H:%M:%S on %Y-%m-%d', $c[5]) . "</td></tr>";
		} elseif ($c[4] == "ENDING") {
			print "<tr><td>&nbsp;</td><td colspan='3' style='text-align:center'>Ending at "
				. gmstrftime('%H:%M:%S on %Y-%m-%d', $c[5]) . "</td></tr>";
		}
	}
?>
 </table>
<?php
}

?>
<div id='tabs'>
 <a href="#acc-games" id="tab-games">Games</a>
<? if (is_null($__quit)) : ?>
 <a href="#acc-vacation" id="tab-vacation">Vacation</a>
<? endif; ?>
 <a href="#acc-quit" id="tab-quit">Quit LegacyWorlds</a>
</div>
<div id="acc-games" class="acctab">
 <h1>Welcome to Legacy Worlds, <?=$_SESSION['login']?>!</h1>
<?php

$vac = $args['vac'];
$onVacation = ($vac['status'] == 'VAC');


if (count($args['play'])) {
	echo "<p>You are currently playing the following games:</p>\n";
	displayRegistered($args['play']);
	if (count($args['other'])) {
		if ($onVacation) {
			echo "<p>Other games are available, but you must exit vacation mode to see them.</p>\n";
		} elseif (!$_SESSION['show_unregistered']) {
			echo "<p>You may also register to the following games:";
			echo " (<a href='?sw=1'>hide</a>):</p>\n";
			displayUnregistered($args['other']);
		} else {
			echo "<p><a href='?sw=1'>Show all games</a></p>\n";
		}
	} else {
		echo "<p>There are currently no other available games.</p>\n";
	}
} else if (!$onVacation) {
?>
 <p>
  <b>You are currently not registered to any game!</b><br/>
  You may register to any of the games in the list below by clicking its name:
 </p>
<?php
	displayUnregistered($args['other']);
} else {
?>
 <p>
  <b>You are currently not registered to any game!</b><br/>
  You will have to leave vacation mode before selecting a game to play.
 </p>
<?php
}

echo "<p>&nbsp;</p>\n";

if ($args['leech']) {
?>

<h1>Contribute to LegacyWorlds!</h1>
<p>
 Learn about how you can help in the <a href='<?=makeLink('contrib', 'main')?>'>Contributions</a> page.
</p>
<?php
}

?>
</div>
<?

if (is_null($args['quit'])) {
?>

<div id='acc-vacation' class="acctab">
 <h1>Vacation mode</h1>
  <p>
<?php

	if ($vac['vac_credits'] > 0) {
?>
  You have <b><?=$vac['vac_credits']?></b> vacation credit<?=$vac['vac_credits'] > 1 ? "s" : ""?>, which translates to
<?php

		$cred = $vac['vac_credits'];
		$crMod = ($cred % 4);
		$crDays = ($cred - $crMod) / 4;
		$crHours = $crMod * 6;

		if ($crDays > 0) {
			echo "<b>$crDays</b> day" . ($crDays > 1 ? "s" : "") . " ";
			if ($crHours > 0) {
				echo "and ";
			}
		}
		if ($crHours > 0) {
			echo "<b>$crHours</b> hours";
		}
		echo ".";
	} else {
		echo "You don't have any vacation credit left.";
	}

?>
 </p>
<?php

	if ($onVacation) {
?>
 <p>
  You are currently <b>on vacation</b>.
 </p>
 <p>
  You can choose to leave vacation mode by clicking the button below.
  However, if you do so, you will not be able to enter vacation mode again for the next 7 days.
 </p>
 <form method='get' action=''><div>
  <input type='hidden' name="evm" value="1" />
  <input type='submit' value="Exit vacation mode" class="input" />
 </div></form>
<?php
	} elseif ($vac['can_set']) {
?>
 <p>
  Vacation mode allows you to take a break from the actual game. You can activate
  it by clicking the button below.
 </p>
 <form method='get' action=''><div>
  <input type='hidden' name="svm" value="1" />
  <input type='submit' value="Enter vacation mode" class="input"  />
 </div></form>
<?php
	} elseif (!is_null($vac['vac_start'])) {
		$vs = $vac['vac_start'];
		$mod = $vs % 21600;
		$startDate = $vs - $mod + ($mod ? 21600 : 0);
?>
 <p>
  Your account will enter vacation mode at <?=gmstrftime("<b>%H:%M:%S (%Y-%m-%d)</b>", $startDate)?> Server Time.
 </p>
 <p>
  Clicking the button below will allow you to prevent your account from entering vacation mode.
 </p>
 <form method='get' action=''><div>
  <input type='hidden' name="cvms" value="1" />
  <input type='submit' value="Cancel vacation mode"class="input"  />
 </div></form>
<?php
	} elseif ($vac['vac_credits'] == 0) {
?>
 <p>
  It is therefore impossible for you to enter vacation mode. You will earn a new credit at the next day tick.
 </p>
<?php
	} else {
		$q = dbQuery("SELECT t FROM main.account_log WHERE account={$_SESSION['userid']} AND action='VEND'");
		list($ts) = dbFetchArray($q);
		$ts += 24 * 3600 * 7;
?>
 <p>
  You exited vacation mode less than a week ago, it is therefore impossible for you to re-enter vacation mode
  before <b><?=gmstrftime("%H:%M:%S", $ts)?></b> (on <b><?=gmstrftime("%Y-%m-%d", $ts)?></b>).
 </p>
<?php
	}
	echo "</div>\n";
}

?>
<div id="acc-quit" class="acctab">
 <h1>Quitting Legacy Worlds</h1>
<?php

if (is_null($args['quit'])) {
?>
 <p>
  Had enough of Legacy Worlds? Well, you may close your account if you wish to do so.
  <br/>
  Closing your account will cause you to disappear from all of the games you're
  currently playing on Legacy Worlds; it will also disable your account.<br/>
  You will be able to use your account again at a later time if you wish to; more
  details will be provided later.
 </p>
 <p>
  Click the button below if you wish to close your account.
 </p>
 <form method="get" action=""><div>
  <input type='hidden' name='rq' value='1' />
  <input type="submit" value="Close my account" class="input" />
 </div></form>
<?php
} else {
	$qt = $args['quit'] + 86400;
	$mod = $qt % 86400;
	$quitDate = $qt - $mod + ($mod ? 86400 : 0) + 7 * 3600;
?>
 <p>
  Your account will be closed at <?=gmstrftime("<b>%H:%M</b> on the <b>%Y-%m-%d</b>", $quitDate)?> (Server Time).
 </p>
 <p>
  Until then, you can choose to cancel this action by clicking the button below.
 </p>
 <form method="get" action=""><div>
  <input type='hidden' name='crq' value='1' />
  <input type="submit" value="Don't close my account" class="input" />
 </div></form>
<?php
}
?>
 </div>
</div>
