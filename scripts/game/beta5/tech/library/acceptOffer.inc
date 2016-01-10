<?php

class beta5_tech_acceptOffer {

	function beta5_tech_acceptOffer($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Accepts a research offer
	function run($pid, $oid) {
		$q = $this->db->query("SELECT * FROM research_assistance WHERE offer_to=$pid AND id=$oid AND (UNIX_TIMESTAMP(NOW())-moment<=86400)");
		if (!($q && dbCount($q)==1)) {
			return 4;
		}
		$r = dbFetchHash($q);
		if (!is_null($r['accepted'])) {
			return $r['accepted'] == 't' ? 2 : 3;
		}
		if ($r['price'] > 0) {
			$pinf = $this->players->call('get', $pid);
			if ($r['price'] > $pinf['cash']) {
				return 1;
			}
		}

		$this->db->query("UPDATE research_assistance SET accepted=TRUE WHERE id=$oid");
		if (is_null($r['technology'])) {
			$this->db->query("UPDATE player SET res_assistance=" . $r['player'] . " WHERE id=$pid");
		} else {
			$tid = $r['technology'];
			if (!$this->lib->call('checkDependencies', $pid, $tid)) {
				return 5;
			}

			$q = $this->db->query("SELECT points FROM research WHERE id=$tid");
			list($points) = dbFetchArray($q);
			$points = ceil(75 * $points / 100);
			$q = $this->db->query("SELECT * FROM research_player WHERE player=$pid AND research=$tid");
			if (!dbCount($q)) {
				$this->db->query("INSERT INTO research_player VALUES($pid,$tid,TRUE,0,$points," . $r['player'] . ")");
			} else {
				$this->db->query("UPDATE research_player SET possible=TRUE,points=$points,given_by=".$r['player']." WHERE player=$pid AND research=$tid");
			}
		}
		if ($r['price'] > 0) {
			$this->db->query("UPDATE player SET cash=cash-" . $r["price"] . " WHERE id=$pid");
			$this->db->query("UPDATE player SET cash=cash+" . $r["price"] . " WHERE id=".$r['player']);
		}

		$tm = time();
		$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES(".$r['player'].",$tm,'resdipl','INT',TRUE)");
		$q = $this->db->query("SELECT id FROM message WHERE player=".$r['player']." AND sent_on=$tm AND ftype='INT'");
		list($mid) = dbFetchArray($q);
		$this->db->query("INSERT INTO msg_resdipl VALUES($mid,$oid,1)");

		return 0;
	}
}

?>
