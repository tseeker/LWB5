<?
include("menu.en.inc");
$this->title = "Registration";
$this->startContents();
?>
<?php


function redirect($game) {
	$rTo = makeLink('play', $game);
?>
<h1>Please wait ...</h1>
<p>
 You should be redirected to the game's main page shortly.<br/>
 If nothing happens after 10 seconds, please click <a href='<?=$rTo?>'>here</a>.</p>
 <script language='javascript'>
  setTimeout("window.location='<?=$rTo?>'", 100);
 </script>
<?php
}


function displayError($error) {
	switch ($error) :
	case 0:
		$title = "Game not found";
		$descr = "The game you requested to join could not be found in the database. It may have been removed in the meantime.";
		break;
	case 1:
		$title = "You already registered to this game";
		$descr = "It is likely you already registered to this game using a different browser window or a different browser tab. Please go back to "
			. "your <a href='" . makeLink('index', 'main') . "'>account page</a> and enter the game from there.";
		break;
	case 2:
		$title = "Registration failed :-(";
		$descr = "An internal error prevented you from joining the game.<br/>Please contact the <a href='mailto:staff@legacyworlds.com'>staff</a> "
			. "so that we can help you with it.";
		break;
	default:
		$title = "Unknown error";
		$descr = "An unknown error has occured.<br/>Please contact the <a href='mailto:staff@legacyworlds.com'>staff</a>.";
		break;
	endswitch;

	echo "<h1>$title</h1>\n<p>$descr</p>";
}



function displayForm($args) {
?>
<form method='post' action='?'><div>
 <input type='hidden' name='g' value='<?=$args['game']?>' />
 <h1>You are about to join <?=$args['gName']?></h1>
 <p><?=utf8entities($args['desc'])?></p>
 <table style="margin:0px 0px 0px 15px;width:80%">
<?php
	if ($args['planetError']) {
		echo "<tr><td colspan='2' class='center'>";
		switch ($args['planetError']) :
			case 1: echo "This planet name is too long (maximum 15 characters)"; break;
			case 2: echo "This planet name is incorrect (letters, numbers, spaces and _.@-+'/ only)"; break;
			case 3: echo "Multiple spaces are not allowed"; break;
			case 4: echo "This planet name is too short (minimum 2 characters)"; break;
			case 5: echo "Planet names must contain at least a letter"; break;
			case 6: echo "A planet by that name already exists."; break;
			case 7: echo "Spaces are not allowed at the beginning or at the end of the planet's name"; break;
		endswitch;
		echo ".</td></tr>";
	}
?>
  <tr>
   <th style="width:200px">Name of your first planet:</th>
   <td><input type='text' name='p' value="<?=utf8entities($args["planet"])?>" size='16' maxlength='15' class="input" /></td>
  </tr>
<?php
	if ($args['returning']) {
		if ($args['playerError']) {
			echo "<tr><td colspan='2' class='center'>";
			switch ($args['playerError']) :
				case 1: echo "This player name is too long (maximum 15 characters)"; break;
				case 2: echo "This player name is incorrect (letters, numbers, spaces and _.@-+'/ only)"; break;
				case 3: echo "Spaces are not allowed at the beginning or at the end of the player name"; break;
				case 4: echo "Multiple spaces are not allowed"; break;
				case 5: echo "This player name is too short (minimum 2 characters)"; break;
				case 6: echo "A player by that name already exists."; break;
			endswitch;
			echo ".</td></tr>";
		}
?>
  <tr>
   <th>Choose a player name:</th>
   <td><input type='text' name='n' value="<?=utf8entities($args['player'])?>" size='16' maxlength='15' class="input" /></td>
  </tr>
<?php
	}
?>
  <tr><td colspan='2'>&nbsp;</td></tr>
  <tr><td>&nbsp;</td><td><input type='submit' name='c' value='Play this game' class="input" /></td></tr>
 </table>
</div></form>
<?php
}



if (is_null($args['registered'])) {
	if (is_null($args['error'])) {
		displayForm($args);
	} else {
		$this->addStylesheet('text');
		displayError($args['error']);
	}
} else {
	$this->addStylesheet('text');
	redirect($args['registered']);
}

?>
<? $this->endContents(); ?>
