<?php

class msgformat_admin {

	private $contents = array();

	public function __construct($game) {
		$this->game	= $game;
		$this->db	= $game->db;
		$this->players	= $game->getLib('beta5/player');
		$this->forums	= $game->getLib('main/forums');
	}

	public function getRecipient() {
		return utf8entities($this->players->call('getName', $this->player),ENT_COMPAT);
	}
	public function getSender() { return 'LegacyWorlds Administrators'; }

	public function getSLink() { return ""; }
	public function getRLink() { return ""; }
	public function getReplyLink() { return ""; }

	public function getSubject() {
		$c = $this->readContents();
		return $c['subject'];
	}

	public function getContents() {
		$c = $this->readContents();
		return $this->forums->call('substitute', $c['contents'], 't', 'f');
	}

	private function readContents() {
		if (is_null($this->msgContents[$this->data['spam']])) {
			$q = $this->db->query("SELECT * FROM admin_spam WHERE id = {$this->data['spam']}");
			if (!($q && dbCount($q))) {
				return array();
			}
			$this->msgContents[$this->data['spam']] = dbFetchHash($q);
		}

		return $this->msgContents[$this->data['spam']];
	}
}

?>

