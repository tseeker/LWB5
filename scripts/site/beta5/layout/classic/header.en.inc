<?php

$game		= input::$game;
$accounts	= $game->getLib('main/account');
$players	= $game->getLib('beta5/player');
$player		= $_SESSION[game::sessName()]['player'];

?>
  <div style="visibility:hidden;display:none"><form action="?" onsubmit="return false"><textarea id="thm-hdr-init"><?

echo thm_getHeaderData();

?></textarea></form></div>
  <table cellspacing='0' cellpadding='0'>
   <tr><td><table class='player' cellspacing='0' cellpadding='0'><tr>
    <td class='player'><span<?=tooltip('Your player name as well as the tag of the alliance you are a member of.');?>>Player <b id='jspname'></b><span id='jsalliance'></span></td>
    <td style="width: 20%;text-align:center"><?php

if ($accounts->call('isAdmin', $_SESSION['userid'])) {
	echo "<a href='admin'>Administration</a>";
} else {
	echo "&nbsp;";
}

?></td>
    <td class='stime'><span<?=tooltip('Date and time on the Legacy Worlds server; this gives you a reference which allows you to synchronize your actions with players from the other side of the Earth.');?>>Server time: <span id='jsservtm'></span></span></td>
   </tr></table></td></tr>
   <tr><td><table class='topmenu'>
    <tr>
     <td colspan="2"><a href="overview"><h1>Overview</h1></a></td>
     <td colspan="2"><a href="fleets"><h1>Fleets</h1></a></td>
     <td colspan="2"><a href="alliance"><h1>Alliance</h1></a></td>
     <td colspan="2" id="msgmenu"><a href="message?a=f&f=I"><h1>Messages</h1></a></td>
    </tr>
    <tr>
     <td colspan="2"><a href="planets"><h1>Planets</h1></a></td>
     <td colspan="2"><a href="research"><h1>Research</h1></a></td>
     <td colspan="2"><a href="market"><h1>Marketplace</h1></a></td>
     <td colspan="2"><a href="forums"><h1>Forums</h1></a></td>
    </tr>
    <tr>
     <td class="bottom"><a href="money"><h1>Money</h1></a></td>
     <td class="bottom"><a href="probes"><h1>Beacons</h1></a></td>
     <td class="bottom"><a href="map"><h1>Maps</h1></a></td>
     <td class="bottom"><a href="rank"><h1>Rankings</h1></a></td>
     <td class="bottom"><a href="enemylist"><h1>Enemies</h1></a></td>
     <td class="bottom"><a href="allies"><h1>Allies</h1></a></td>
     <td class="bottom"><a href="preferences"><h1>Preferences</h1></a></td>
     <td class="bottom"><a href="<?=makeLink('logout', 'main')?>"><h1>Log out</h1></a></td>
    </tr>
   </table></td></tr>
   <tr><td><table><tr>
    <td class="funds"><span<?=tooltip('The money you currently own.')?>>Current funds: <b id='jscash'></b></span></td>
    <td style="width:34%;text-align:center;color:red;font-weight:bold;padding:5px 5px 5px 0px" id="hdrvmode"><?php

$pinf = $players->call('get', $player);
if ($accounts->call('getQuitCountdown', $_SESSION['userid'])) {
	echo "CLOSING ACCOUNT";
} else if (!is_null($pinf['qts'])) {
	echo "LEAVING GAME";
} else if ($players->call('isOnVacation', $player)) {
	echo "ON VACATION";
} else if ($players->call('getProtectionLevel', $player)) {
	echo "UNDER PROTECTION";
} else {
	echo "&nbsp;";
}

?></td>
    <td style="width:33%;text-align:right; padinng: 5px 5px 5px 0px"><i>Game: <?=$game->text?></i></td>
   </tr></table></td></tr>
  </table>
