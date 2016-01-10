<?php

function getCripesBanner() {
	$c = prefs::get('main/colour', 'purple');
	return getStatic("beta5/pics/that-other-theme-$c.png");
}

function getCripesTech() {
	$c = prefs::get('main/colour', 'purple');
	if (input::$IE) {
		$ext = "gif";
	} else {
		$ext = "png";
	}
	return getStatic("beta5/pics/lw-tot-tech-$c.$ext");
}

function getCripesLink($name, $title, $colspan, $width, $class = '', $id = "") {
	$cPage = handler::$h->output;
	return "<td" . ($colspan > 1 ? " colspan='$colspan'" : "") . ($class == "" ? "" : " class='$class'")
		. ($id == "" ? "" : " id='$id'")
		. " style='width: $width' onclick='location.href=\"$name\"'><a href=\"$name\"><h1"
		. ($cPage == $name ? " class='crpsel'" : "") . ">$title</h1></a></td>\n";
}

$game		= input::$game;
$accounts	= $game->getLib('main/account');
$players	= $game->getLib('beta5/player');
$player		= $_SESSION[game::sessName()]['player'];

?>
  <div style="visibility:hidden;display:none"><form action="?" onsubmit="return false"><textarea id="thm-hdr-init"><?

echo thm_getHeaderData();

?></textarea></form></div>
  <div style="background-color:black;background-image: url(<?=getCripesBanner()?>); background-repeat: no-repeat; height: 82px;margin: 0px;padding:22px 0px 60px 30px">
   <table class='crpmenu' cellpadding="0" cellspacing="1">
    <tr>
     <td rowspan="6" class="crptech"><a href="research"><img src="<?=getCripesTech()?>" alt="Research" style="border: none"/></a></td>
     <td rowspan="6" style="width:30px">&nbsp;</td>
<?php
if ($accounts->call('isAdmin', $_SESSION['userid'])) {
?>
     <td colspan="2" class="stime">&nbsp;</td>
<?php
	print getCripesLink('admin', 'Administration', 2, '180px');
} else {
?>
     <td colspan="4" class="stime">&nbsp;</td>
<?php
}

?></td>
     <td colspan="4" class="stime">Server time: <span id='jsservtm'></span></td>
    </tr>
    <tr>
     <?=getCripesLink('overview', 'Overview', 2, '180px')?>
     <?=getCripesLink('empire', 'Empire', 2, '180px')?>
     <?=getCripesLink('alliance', 'Alliance', 2, '180px')?>
     <?=getCripesLink('forums', 'Forums', 2, '180px')?>
    </tr>
    <tr>
     <?=getCripesLink('planets', 'Planets', 2, '180px')?>
     <?=getCripesLink('market', 'Market', 2, '180px')?>
     <?=getCripesLink('map', 'Galactic maps', 2, '180px')?>
     <?=getCripesLink('rank', 'Rankings', 2, '180px')?>
    </tr>
    <tr>
     <?=getCripesLink('fleets', 'Fleets', 2, '180px')?>
     <?=getCripesLink('probes', 'Beacons', 2, '180px')?>
     <?=getCripesLink('enemylist', 'Enemy list', 2, '180px')?>
     <?=getCripesLink('allies', 'Trusted allies', 2, '180px')?>
    </tr>
    <tr>
     <?=getCripesLink('message', "<span id='jspname'></span><span id='jsalliance'></span>", 2, '180px', 'crpmsg', 'msgmenu')?>
     <?=getCripesLink('money', "Cash: <span id='jscash'></span>", 2, '180px', 'crpcash')?>
     <?=getCripesLink('preferences', "Preferences", 2, '180px', 'crpbt')?>
     <?=getCripesLink(makeLink('logout', 'main'), "Logout", 1, '90px', 'crpbt')?>
     <?=getCripesLink('manual', "Help", 1, '90px', 'crpbt')?>
    </tr>
    <tr>
     <td colspan="6" style="font-weight: bold;text-align:center"><?php

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
     <td colspan="3" class="stime" style="text-align:right"><a href="<?=makeLink('index', 'main')?>">Account</a> - <i><?=$game->text?></i></td>
    </tr>
   </table>
  </div>
