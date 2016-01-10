<?php

class msgformat_pkwarning {

	public function __construct($game) {
		$this->game	= $game;
		$this->db	= $game->getDBAccess();
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
		switch ($this->data['msg_type']) {
		    case 'W':
			return 'Your fleets have entered protected territory';
		    case 'D':
			return 'Your fleets will be destroyed';
		    case 'E':
			return 'You have been declared an ennemy of the Peacekeepers';
		}
		return 'Huh? wrong message type';
	}

	public function getReplyLink() {
		return	'';
	}

	public function getContents() {
		$planets = "<ul><li>" . join('</li><li>', $this->getPlanets()) . "</li></ul>";

		ob_start();
		switch ($this->data['msg_type']) {
		    case 'W':
?>
This is a warning from Peacekeeper Headquarters. Your fleets have entered Peacekeeper-protected territories. If
you do not retreat before <b><?=gmstrftime("%H:%M", $this->data['delay'])?></b>, we will be forced to take action.<br/>
This warning applies to the following planets:</p>
<?=$planets?>
<p>Best regards,<br/>
Peacekeeper Commander Dapkor
<?
			break;
		    case 'D':
?>
Your hostile actions will not go unpunished! The Peacekeepers will see to it that your fleets are destroyed on the
following planets:</p>
<?=$planets?>
<p>Regards,<br/>
Peacekeeper Commander Multair
<?
			break;
		    case 'E':
?>
Your continued hostile behaviour has forced us to declare you an enemy of the Peacekeepers. Your fleets will be destroyed
on sight whenever you enter protected territory, and we will not protect you if you need it.<br/>
In addition, your fleets will be destroyed on the following planets:
</p>
<?=$planets?>
<p>Sanctions against you will end at <b><?=gmstrftime("%H:%M</b> on <b>%d/%m/%Y", $this->data['delay'])?></b>.
<br/>
Peacekeeper Commander Hestaks
<?
			break;
		}
		$str = ob_get_contents();
		ob_end_clean();
		return $str;
	}


	private function getPlanets() {
		$q = $this->db->query("SELECT planet, p_name FROM pkwarning_planet WHERE id = {$this->data['id']}");
		$planets = array();
		while ($r = dbFetchArray($q)) {
			array_push($planets, "<a href='planet?id={$r[0]}'>" . utf8entities($r[1]) . "</a>");
		}
		return $planets;
	}
}

?>
