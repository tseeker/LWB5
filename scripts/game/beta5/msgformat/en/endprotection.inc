<?php

class msgformat_endprotection {

	public function __construct($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	public function getSender() {
		return 'Peacekeeper Headquarters';
	}

	public function getSLink() {
		return '';
	}

	public function getRecipient() {
		$name = $this->players->call('getName', $this->player);
		return utf8entities($name);
	}

	public function getRLink() {
		return '';
	}

	public function getSubject() {
		return 'You are no longer under our protection';
	}

	public function getReplyLink() {
		return	'';
	}

	public function getContents() {
		ob_start();
		switch ($this->data['end_type']) {
		    case 'EXP':
?>
The period of 20 days during which we were meant to protect you has expired.<br/>
We are confident that you are now strong enough to survive, and wish you the best of luck.<br/>
<br/>
Kind regards,<br/>
Peacekeeper Commander Dapkor
<?
			break;
		    case 'BRK':
?>
You have decided that you no longer require our services.<br/>
We wish you the best of luck in your future endeavours, and remain at your disposal
should you need our help again.<br/>
<br/>
Kind regards,<br/>
Peacekeeper Commander Multair
<?
			break;
		    case 'ACT':
?>
You have sent a ship outside your system, therefore revoking our agreement.<br/>
We wish you the best of luck in your future endeavours, and remain at your disposal
should you need our services again.<br/>
<br/>
Kind regards,<br/>
Peacekeeper Commander Hestaks
<?
			break;
		}
		$str = ob_get_contents();
		ob_end_clean();
		return $str;
	}
}

?>
