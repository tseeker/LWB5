<?php


function menuTopLevelBegin($t, $l, $tt = null) {
	static $b5menu = 0;
	if (is_string($tt)) {
		$te = " onmouseover=\"return escape('" . utf8entities($tt,ENT_QUOTES) . "')\"";
	} else {
		$te = "";
	}
	echo "<td><ul class='tmenu'><li class='tmenu'";
	if (input::$IE) {
		echo " onMouseOver='this.obc=this.style.backgroundColor;this.style.backgroundColor=\"black\";thminv_ieDisplay(\"b5tl$b5menu\")'";
		echo " onMouseOut='this.style.backgroundColor=this.obc;thminv_ieHide(\"b5tl$b5menu\")'";
	}
	$t = preg_replace('/ /', '&nbsp;', $t);
	echo "><a class='tmenu' href='$l'$te>$t</a><ul class='tmenu' id='b5tl$b5menu'>\n";
	$b5menu ++;
}

function menuTopLevelEnd() {
	echo "</ul></li></ul></td>\n";
}

function menuTopLevelEntry($t, $l, $tt = null) {
	$te = is_string($tt) ? tooltip($tt) : "";
	$t = preg_replace('/ /', '&nbsp;', $t);
	echo "<td><ul class='tmenu'><li class='tmenu'><a class='tmenu' href='$l'$te>$t</a></li></ul></td>\n";
}

function menuEntry($t, $l, $tt = null) {
	$te = is_string($tt) ? tooltip($tt) : "";
	echo "<li class='tmenu'";
	if (input::$IE) {
		echo " onMouseOver='this.obc=this.style.backgroundColor;this.style.backgroundColor=\"black\";' onMouseOut='this.style.backgroundColor=this.obc'";
	}
	$t = preg_replace('/ /', '&nbsp;', $t);
	echo "><a class='tmenu' href='$l'$te>$t</a></li>\n";
}

function menuSubBegin($t, $l, $tt = null, $sid = null) {
	$te = is_string($tt) ? tooltip($tt) : "";
	$id = ($sid == "") ? "" : " id='$sid'";
	$t = preg_replace('/ /', '&nbsp;', $t);
	echo "<li class='tmenu'><a class='tmenu' href='$l'$te>$t</a><ul class='tmenu'$id>\n";
}

function menuSubEnd() {
	echo "</ul></li>\n";
}

function drawIcon($n, $d) {
	$src = getStatic("beta5/pics/icons/$n." . (input::$IE ? 'gif' : 'png'));
	echo "<img class='icon' src='$src' alt=\"". utf8entities($d) .'" />';
}


$game		= input::$game;
$accounts	= $game->getLib('main/account');
$players	= $game->getLib('beta5/player');
$alliance	= $game->getLib('beta5/alliance');

$player = $_SESSION[game::sessName()]['player'];
$pInfo = $players->call('get', $player);
if ($pInfo['aid']) {
	$privileges = $alliance->call('getPrivileges', $player);
	$techTrade = ($privileges['tech_trade'] > 0);
} else {
	$techTrade = false;
}
$protected = $players->call('getProtectionLevel', $player);


?>
  <div style="visibility:hidden;display:none"><form action="?" onsubmit="return false"><textarea id="thm-hdr-init"><?

echo thm_getHeaderData();

?></textarea><textarea id="thm-plist-init"><?

echo thm_getHeaderPList();

?></textarea></form></div>
  <table class='itopframe' cellspacing='0' cellpadding='0'>
   <tr><td class='topmenu'><table cellspacing='0' cellpadding='0'><tr>
    <td><table cellspacing='0' cellpadding='0' class='topmenu'><tr>
<?php
	menuTopLevelBegin('Overview', 'overview', 'This page gives an overview of the state of your empire.');
		menuEntry('Preferences', 'preferences', 'Change the way pages are displayed and modify in-game preferences.');
		menuEntry('My Account', makeLink('index','main'), 'Go back to your account management page.');
		menuEntry('Log out', makeLink('logout','main'), 'Log out from the game.');
	menuTopLevelEnd();

	menuTopLevelBegin('Empire', 'empire', 'Overview of your planets, fleets, and research.');
	if (input::$IE) {
		menuEntry('Planets', 'planets', 'Displays an overview of your planets and allows you to manipulate all of their build queues.');
	} else {
		menuSubBegin('Planets', 'planets', 'Displays an overview of your planets and allows you to manipulate all of their build queues.', 'jspmenu');
		menuSubEnd();
	}
		menuEntry('Fleets', 'fleets', 'Fleet management');
		menuEntry('Beacons', 'probes', 'Allows you to manage hyperspace beacons on your planets');
	if (input::$IE) {
		menuEntry('Research', 'research', 'Implement new technologies, enact and revoke laws, balance your research budget and exchange technologies and scientific knowlege with other empires.');
	} else {
		menuSubBegin('Research', 'research', 'Implement new technologies, enact and revoke laws, balance your research budget and exchange technologies and scientific knowlege with other empires.');
			menuEntry('Topics', 'research?p=t', 'Implement new technologies, view foreseen breakthroughs and already implemented technologies.');
			menuEntry('Laws', 'research?p=l', 'Enact and revoke laws.');
			menuEntry('Budget', 'research?p=b', 'Balance your research budget between fundamental, military and civilian research.');
			menuEntry('Diplomacy', 'research?p=d', 'Give or sell technologies and scientific knowledge with other empires, and examine offers made to you');
		menuSubEnd();
	}
		menuEntry('Money', 'money', 'This page shows the money you earn on the planets you own as well as the money you spend to maintain your fleets and infrastructure.');
	menuTopLevelEnd();

	menuTopLevelBegin('Diplomacy', 'diplomacy', 'Overview of your current diplomatic relations.');
		menuEntry('Alliance', 'alliance', 'Create, join, inspect, manage or spy on alliances.');
		if ($techTrade) {
			menuEntry('Tech. trading', 'techtrade', 'Alliance technology trading tool.');
		}
		if (!$players->call('isRestrained', $player)) {
			menuEntry('Marketplace', 'market', 'Sell stuff to other players and buy other stuff from them.');
		}
		menuEntry('Enemies', 'enemylist', 'Manage your enemy list.');
		menuEntry('Trusted Allies', 'allies', 'Manage your trusted allies list.');
	menuTopLevelEnd();

	menuTopLevelBegin('Universe', 'universe', 'Overview of the game universe.');
	if (input::$IE) {
		menuEntry('Maps', 'map', 'View the maps of the game universe.');
		menuEntry('Rankings', 'rank', 'View the rankings of players and alliances.');
	} else {
		menuSubBegin('Maps', 'map', 'View the maps of the game universe.');
			menuEntry('Planets', 'map?menu=p', 'View the planets in the current universe.');
			menuEntry('Alliances', 'map?menu=a', 'View planets belonging to alliances in the current universe.');
			menuEntry('Listing', 'map?menu=l', 'View a listing of the planets in the game universe.');
		menuSubEnd();
		menuSubBegin('Rankings', 'rank', 'View the rankings of players and alliances.');
			menuEntry('Summary', 'rank?p=s', 'Overview of your current ranking.');
			menuEntry('General', 'rank?p=g', 'Display general player rankings.');
			menuEntry('Detailed', 'rank?p=d', 'Display detailed player rankings.');
			menuEntry('Alliance', 'rank?p=a', 'Display alliance rankings.');
			menuEntry('Overall', 'rank?p=o', 'Display overall player rankings.');
			menuEntry('Damage', 'rank?p=i', 'Display inflicted damage rankings.');
		menuSubEnd();
	}
		menuEntry('Ticks', 'ticks', 'Display details about the ticks.');
		menuEntry('Manual', 'manual', 'The manual. Newbies, please read it. Seriously.');
	menuTopLevelEnd();

	menuTopLevelBegin('Communications', 'comms', 'An overview of your communications with other players');
		menuEntry('Compose', 'message?a=c', 'Compose a new private message.');
		menuEntry('Inbox', 'message?a=f&f=I', 'View the contents of your inbox.');
	if (input::$IE) {
		menuEntry('Transmissions', 'message?a=f&f=T', 'View internal transmissions');
		menuEntry('Folders', 'message?a=mf', 'Manage your custom folders.');
	} else {
		menuSubBegin('Folders', 'message?a=mf', 'Manage your custom folders.', 'jsfmenu');
			menuEntry('Transmissions', 'message?a=f&f=T', 'View internal transmissions');
			menuEntry('Outbox', 'message?a=f&f=O', 'View the messages you sent.');
		menuSubEnd();
	}
		menuEntry('Forums', 'forums?cmd=o', 'Access the forums.');
	menuTopLevelEnd();

	if ($accounts->call('isLeech', $_SESSION['userid'])) {
		menuTopLevelEntry('Contribute!', makeLink('contrib', 'main'), 'Contribute to LegacyWorlds!');
	}
	if ($accounts->call('isAdmin', $_SESSION['userid'])) {
		menuTopLevelEntry('Admin', 'admin', 'Administrative tools');
	}

?>
    </tr></table></td>
    <td id="msgicon">&nbsp;</td>
    <td class='icons'><a
        href='planets'<?=tooltip('Manage your planets')?>><? drawIcon('planets', 'Planet management'); ?></a><a
        href='fleets'<?=tooltip('Manage your fleets')?>><? drawIcon('fleets', 'Fleet management'); ?></a><a
	href='map'<?=tooltip("View maps of the universe")?>><? drawIcon('map', 'Galactic maps'); ?></a><a
	href='alliance'<?=tooltip('Display information about your current alliance or allows you to join an alliance')?>><? drawIcon('alliance', 'Alliance panel'); ?></a><a
	href='../main/logout'<?=tooltip('Log out from the game')?>><? drawIcon('logout', 'Log out'); ?></a></td>
   </tr></table></td></tr>
   <tr><td><table class='player' cellspacing='0' cellpadding='0'><tr>
    <td class='player'><span<?=tooltip('Your player name as well as the tag of the alliance you are a member of.');?>>Player <b id='jspname'></b><span id='jsalliance'></span></td>
    <td class='funds'><span<?=tooltip('The money you currently own.')?>>Current funds: <b id='jscash'></b></span></td>
    <td class='stime'><span<?=tooltip('Date and time on the Legacy Worlds server; this gives you a reference which allows you to synchronize your actions with players from the other side of the Earth.');?>>Server Time: <span id='jsservtm'></span></span></td>
   </tr></table></td></tr>
  </table>
  <div style="text-align:center">
   <div style="float: right; padding: 2px 4px"><i>Game: <?=$game->text?></i></div>
   <table style="width:94%;height:30px;margin:0 2%">
    <tr>
<?php

function drawTitle() {
	$n = handler::$h->output;
	$c = prefs::get('main/colour', 'red');
	$src = getStatic("beta5/pics/ttl/def/en/$c/$n.gif");
	if (!is_null($src)) {
		echo "<img class='title' src='$src' alt=\"$n\" />";
	}
}

$pinf = $players->call('get', $player);
if ($accounts->call('getQuitCountdown', $_SESSION['userid'])) {
?>
     <td style="width:20%;text-align:right;color:red;font-weight:bold;vertical-align:middle">CLOSING ACCOUNT</td>
     <td style="text-align:center"><?php drawTitle(); ?></td>
     <td style="width:20%;text-align:left;color:red;font-weight:bold;vertical-align:middle">CLOSING ACCOUNT</td>
<?php
} else if (!is_null($pinf['qts'])) {
?>
     <td style="width:20%;text-align:right;color:red;font-weight:bold;vertical-align:middle">LEAVING GAME</td>
     <td style="text-align:center"><?php drawTitle(); ?></td>
     <td style="width:20%;text-align:left;color:red;font-weight:bold;vertical-align:middle">LEAVING GAME</td>
<?php
} else if ($players->call('isOnVacation', $player)) {
?>
     <td style="width:20%;text-align:right;color:red;font-weight:bold;vertical-align:middle">ON VACATION</td>
     <td style="text-align:center"><?php drawTitle(); ?></td>
     <td style="width:20%;text-align:left;color:red;font-weight:bold;vertical-align:middle">ON VACATION</td>
<?php
} else if ($protected) {
?>
     <td style="width:20%;text-align:right;color:red;font-weight:bold;vertical-align:middle">UNDER PROTECTION</td>
     <td style="text-align:center"><?php drawTitle(); ?></td>
     <td style="width:20%;text-align:left;color:red;font-weight:bold;vertical-align:middle">UNDER PROTECTION</td>
<?php
} else {
?>
     <td style="text-align:center"><?php drawTitle(); ?></td>
<?php
}

?>
    </tr>
   </table>
  </div>
  <hr/>
