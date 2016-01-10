<?php

class msgformat_ctf {

	public function __construct($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
		$this->alliance	= $game->getLib('beta5/alliance');
	}

	public function getSLink() { return ""; }
	public function getRLink() { return ""; }
	public function getReplyLink() { return ""; }

	public function getSender() { return "LegacyWorlds"; }
	public function getRecipient() {
		$name = $this->players->call('getName', $this->player);
		return utf8entities($name);
	}


	public function getSubject() {
		// Get the team's data if needed
		if (!is_null($this->data['team'])) {
			$this->team = $this->alliance->call('get', $this->data['team']);
		}

		// Create the subject
		switch ($this->data['msg_type']) :
			default:
			case 0: $rv = "Welcome!"; break;
			case 1: $rv = "A player joined the team"; break;
			case 2: $rv = "We are holding the targets"; break;
			case 3: $rv = $this->team['name'] . " has the targets!"; break;
			case 4: $rv = "Targets lost - we must retake"; break;
			case 5:
			case 6: $rv = "Targets lost"; break;
			case 7: $rv = $this->team['name'] . " could lose the targets"; break;
			case 8:
			case 9: $rv = $this->team['name'] . " lost the targets"; break;
			case 10: $rv = "We are still holding!"; break;
			case 11: $rv = $this->team['name'] . " is still holding!"; break;
			case 12: $rv = "Victory!... sort of."; break;
			case 13: $rv = $this->team['name'] . " owned us"; break;
			case 14: $rv = "WE HAVE WON THE GAME!"; break;
			case 15: $rv = "We were defeated :'("; break;
		endswitch;
		return $rv;
	}

	public function getContents() {
		// Get the team's data if needed
		if (!is_null($this->data['team']) && is_null($this->team)) {
			$this->team = $this->alliance->call('get', $this->data['team']);
		}

		// Generate the contents
		$generators = array(
			"Welcome", "Join",
			"Hold", "OthersHold",
			"LostGrace", "Lost", "GraceExpired",
			"OthersLostGrace", "OthersLost", "OthersGraceExpired",
			"Half", "OthersHalf",
			"Win", "Lose",
			"WonGame", "LostGame"
		);

		$v = "msg" . $generators[$this->data['msg_type']];
		ob_start();
		$this->$v();
		$rv = ob_get_contents();
		ob_end_clean();

		return $rv;
	}


	private function msgWelcome() {
		$game = $this->players->game;
?>
Welcome to this special <i>King of the Hills</i> LegacyWorlds game!<br/>
<br/>
You have been assigned to <b>[<?=$this->team['tag']?>]</b>, the
<?=strtolower($this->team['name'])?>.<br/>
<br/>
Before you start playing, a few explanations on this special minigame may be required.<br/>
<br/>
Here, the goal is for your team to control special star systems called <i>targets</i> for
<b><?=$game->params['v2time']?> hours</b> in a row. If you succeed, the team will earn
<b><?=$game->params['v2points']?></b> points.<br/>
<?php
		if ($game->params['v2grace']) {
?>
<br/>
However, if you lose control on the targets, you will still have
<b><?=$game->params['v2grace']?> hours</b> to recover them without restarting the
countdown to victory. Of course, the time during which your team no longer holds the
targets does not count in the race towards victory.<br/>
<?php
		}
?>
<br/>
When a team wins a round, the targets are made neutral again, and their factories and turrets
are reset. All planets which had been destroyed by Wormhole Supernovas are restored, and all
planets a team owns in another team's territory are neutralized. Players who no longer had
planets are respawned at their original location. In addition, fleets are equalized so that
no-one starts the new round with more fleets than the others.<br/>
<br/>
The first team to reach <b>100</b> points will win the game.
<?php
	}


	private function msgJoin() {
		$name = $this->players->call('getName', $this->data['time_stamp']);
?>
A new player, <a href='?a=c&ct=0&id=<?=$this->data['time_stamp']?>'><b><?=utf8entities($name)?></b></a>,
has been assigned to the <?=strtolower($this->team['name'])?>.
<?php
	}


	private function msgHold() {
?>
Our team is now holding all the targets! We must hold them until
<b><?=gmstrftime("%H:%M", $this->data['time_stamp'] + 60)?></b> (<?=gmstrftime("%d/%m/%Y", $this->data['time_stamp'])?>)
to win this round.
<?php
	}


	private function msgOthersHold() {
?>
<b><?=$this->team['tag']?></b>, the <?=strtolower($this->team['name'])?>, is now holding all the targets!<br/>
We must retake them at all cost, otherwise the <?=strtolower($this->team['name'])?> will win this round at
<b><?=gmstrftime("%H:%M", $this->data['time_stamp'] + 60)?></b> (<?=gmstrftime("%d/%m/%Y", $this->data['time_stamp'])?>).
<?php
	}


	private function msgLostGrace() {
?>
Our team is no longer holding the targets... If we do not retake them before
<b><?=gmstrftime("%H:%M", $this->data['time_stamp'] - 60)?></b> (<?=gmstrftime("%d/%m/%Y", $this->data['time_stamp'])?>),
the countdown to victory will be reset and we'll need to do it all over again...
<?php
	}


	private function msgLost() {
?>
Our team is no longer holding the targets :-(<br/>
The countdown to victory has been reset and we need to start all over again...
<?php
	}

	private function msgGraceExpired() {
?>
Our team is no longer holding the targets, and we lost our chance to retake them in time...<br/>
The countdown to victory has been reset and we need to start all over again...
<?php
	}

	private function msgOthersLostGrace() {
?>
<b><?=$this->team['tag']?></b>, the <?=strtolower($this->team['name'])?>, is no longer holding the targets!<br/>
However, if they retake the targets before <b><?=gmstrftime("%H:%M", $this->data['time_stamp'] - 60)?></b>
(<?=gmstrftime("%d/%m/%Y", $this->data['time_stamp'])?>),
the countdown to their victory will start again ... We must prevent them from retaking!
<?php
	}

	private function msgOthersLost() {
?>
<b><?=$this->team['tag']?></b>, the <?=strtolower($this->team['name'])?>, is no longer holding the targets!
The countdown has been cancelled!
<?php
	}

	private function msgOthersGraceExpired() {
?>
<b><?=$this->team['tag']?></b>, the <?=strtolower($this->team['name'])?>, is no longer holding the targets and
didn't manage to retake them in time! The countdown has been cancelled!
<?php
	}


	private function msgHalf() {
?>
Our team is now holding all the targets! We must hold them until
<b><?=gmstrftime("%H:%M", $this->data['time_stamp'] + 60)?></b> (<?=gmstrftime("%d/%m/%Y", $this->data['time_stamp'])?>)
to win this round.
<?php
	}

	private function msgOthersHalf() {
?>
<b><?=$this->team['tag']?></b>, the <?=strtolower($this->team['name'])?>, is still holding all the targets!<br/>
We must retake them at all cost, otherwise the <?=strtolower($this->team['name'])?> will win this round at
<b><?=gmstrftime("%H:%M", $this->data['time_stamp'] + 60)?></b> (<?=gmstrftime("%d/%m/%Y", $this->data['time_stamp'])?>).
<?php
	}


	private function msgWin() {
?>
Our team has just won a round of the game!<br/>
<br/>
As a consequence, the map has been reset and everyone has been respawned...<br/>
Keep sharp, the battle isn't over yet.
<?php
	}


	private function msgLose() {
?>
We just lost a round of the game to <b>[<?=$this->team['tag']?>]</b>, the <?=strtolower($this->team['name'])?>.<br/>
<br/>
As a consequence, the map has been reset and everyone has been respawned...<br/>
We still stand a chance of winning!
<?php
	}


	private function msgWonGame() {
?>
The game is finished - AND WE WON!
<?php
	}


	private function msgLostGame() {
?>
The game is finished - but we lost. <b>[<?=$this->team['tag']?>]</b>, the <?=strtolower($this->team['name'])?>,
overpowered us.
<?php
	}

}

?>
