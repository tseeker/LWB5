<?php

class beta5_leaveGame {

	function beta5_leaveGame($lib) {
		$this->lib	= $lib;
		$this->game	= $lib->game;
		$this->db	= $this->lib->game->db;
		$this->rankings	= $this->lib->game->getLib('main/rankings');
		$this->alliance	= $this->lib->game->getLib('beta5/alliance');
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->sales	= $this->lib->game->getLib('beta5/sale');
	}


	function run($id, $reason) {
		if ($this->game->params['victory'] != 0) {
			return;
		}
		logText("Player #$id is LEAVING game ($reason)!");
		if ($reason == 'KICKED') {
			$reason = 'KICK';
		}

		$pinf = $this->players->call('get', $id);
		$name = $pinf['name'];
		$msgs = array();

		// Leave alliance
		if (!is_null($pinf['aid'])) {
			$this->alliance->call('leave', $id, true);
			$a = "'" . addslashes($pinf['alliance']) . "'";
			$q = $this->db->query("SELECT id FROM player WHERE alliance={$pinf['aid']} AND a_status='IN'");
			while ($r = dbFetchArray($q)) {
				$msgs[$r[0]] = array($a, dbBool(0), 0, 0);
			}
		}

		// Marketplace
		$q = $this->db->query("SELECT id,player,finalized,sold_to FROM sale WHERE player=$id OR sold_to=$id");
		while ($r = dbFetchArray($q)) {
			list($sid,$seller,$fin,$buyer) = $r;
			if (is_null($fin)) {
				$ga = 'cancel';
			} else {
				$ga = 'cancelTransfer';

				if ($seller == $id) {
					$tInc = 0;
					$fInc = 1;
					$t = $buyer;
				} else {
					$tInc = 1;
					$fInc = 0;
					$t = $seller;
				}

				if (is_array($msgs[$t])) {
					$msgs[$t][2] += $tInc;
					$msgs[$t][3] += $fInc;
				} else {
					$msgs[$t] = array('NULL', dbBool(0), $tInc, $fInc);
				}
			}
			$this->sales->call($ga, $seller, $sid);
		}

		// FIXME: probes
		$this->db->query("DELETE FROM fleet WHERE owner=$id");

		// Remove planets and build queues
		$q = $this->db->query(
			"SELECT p.id, s.id FROM planet p, system s "
				. "WHERE p.owner = $id AND s.id = p.system "
				. "FOR UPDATE OF p, s"
		);
		$systems = array();
		while ($r = dbFetchArray($q)) {
			if (!in_array($r[1], $systems)) {
				$systems[] = $r[1];
			}
			$this->db->query("DELETE FROM buildqueue WHERE planet=".$r[0]);
			$this->db->query("DELETE FROM planet_abandon_time WHERE id = {$r[0]}");
//			$this->db->query("DELETE FROM built_probes WHERE planet={$r[0]}");
			$this->planets->call('updateMaxPopulation', $r[0], $pid, null);
		}
		foreach ($systems as $systemID) {
			$this->db->query("UPDATE system SET prot = 0 WHERE id = $systemID");
		}
		$this->db->query("UPDATE planet SET owner=NULL,sale=NULL,bh_prep=NULL,abandon=NULL,beacon=0,"
			. "vacation='NO',built_probe=FALSE,probe_policy=NULL WHERE owner=$id");

		// Remove research data
		$this->db->query("UPDATE player SET res_assistance=NULL WHERE id=$id OR res_assistance=$id");
		$this->db->query("DELETE FROM research_player WHERE player=$id");
		// FIXME: doesn't work for some reason - investigate
		//$this->db->query("DELETE FROM rule WHERE player=$id");

		// Remove player from TA lists and enemy lists
		$this->db->query("DELETE FROM trusted WHERE player=$id");
		$q = $this->db->query("SELECT player,level FROM trusted WHERE friend=$id");
		while ($r = dbFetchArray($q)) {
			list($pl,$l) = $r;

			if (!is_array($msgs[$pl])) {
				$msgs[$pl] = array('NULL', dbBool(1), 0, 0);
			} else {
				$msgs[$pl][1] = dbBool(1);
			}

			$this->db->query("DELETE FROM trusted WHERE friend=$id AND player=$pl");
			$q2 = $this->db->query("SELECT level FROM trusted WHERE level > $l AND player=$pl "
				. "ORDER BY level FOR UPDATE");
			while ($r2 = dbFetchArray($q2)) {
				$this->db->query("UPDATE trusted SET level=level-1 WHERE player=$pl AND level={$r2[0]}");
			}
		}
		$this->db->query("DELETE FROM enemy_alliance WHERE player=$id");
		$this->db->query("DELETE FROM enemy_player WHERE player=$id");

		// Remove player from the rankings
		$types = array('general','financial','military','civ','round','idr');
		foreach ($types as $t) {
			$rt = $this->rankings->call('getType', "p_$t");
			$this->rankings->call('delete', $rt, $name);
		}

		// Inform other players
		$tm = time();
		foreach ($msgs as $pid => $data) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($pid,$tm,'leave','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$pid AND sent_on=$tm AND mtype='leave' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_leave VALUES($mid,$id,'$reason'," . join(',', $data) . ")");
		}

		$this->db->query("UPDATE player SET quit=UNIX_TIMESTAMP(NOW()) - 86401 WHERE id=$id");
	}
}

?>
