<?php

//-------------------------------------------------------------
// "Sales tick": makes sales expire and finalize auction sales
//-------------------------------------------------------------


class beta5_ticks_sales_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	public function runTick() {
		// Update sales
		$this->db->safeTransaction(array($this, 'expireSales'));

		// Check CTF victory
		if ($this->game->params['victory'] == 0) {
			$this->db->safeTransaction(array($this, 'checkProtection'));
		} elseif ($this->game->params['victory'] == 2) {
			$this->db->safeTransaction(array($this, 'checkCTFVictory'));
		}
	}

	public function expireSales() {
		$q = $this->db->query("SELECT * FROM sale WHERE expires<UNIX_TIMESTAMP(NOW()) AND finalized IS NULL");
		while ($r = dbFetchHash($q)) {
			// Public offers
			$q2 = $this->db->query("SELECT auction FROM public_offer WHERE offer=".$r['id']);
			if ($q2 && dbCount($q2)) {
				list($auction) = dbFetchArray($q2);
				$this->expirePublicOffer($r, $auction == 't');
				continue;
			}

			// Private offers
			// FIXME
		}
		$this->db->query(
			"DELETE FROM sale_history WHERE end_mode IS NOT NULL AND UNIX_TIMESTAMP(NOW()) - ended > 5184000"
		);
	}

	public function checkCTFVictory() {
		// Check victory status on CTF games
		if (! $this->game->getLib()->call('isFinished')) {
			$this->game->getLib('beta5/ctf')->call('checkTargets');
		}
	}


	public function checkProtection() {
		$q = $this->db->query(
			"SELECT p.id, s.id FROM planet p, system s WHERE s.id = p.system AND s.prot > 0"
		);

		$checked = array();
		$pLib = $this->game->getLib('beta5/prot');

		while ($r = dbFetchArray($q)) {
			if (! in_array($r[1], $checked)) {
				array_push($checked, $r[1]);
				$pLib->call('checkSystem', $r[0]);
			}
		}

		$pLib->call('flushStatus');
	}


	private function expirePublicOffer($offer, $isAuction) {
		$tm = time();

		// If it isn't an auction sale, just delete it
		if (!$isAuction) {
			// Insert entry in player's history
			$this->db->query("UPDATE sale_history SET ended=$tm,end_mode=3,sell_price=NULL WHERE offer=".$offer['id']);
			// FIXME: send message
			$this->db->query("DELETE FROM sale WHERE id=".$offer['id']);
			return;
		}

		// Check for a buyer
		$q = $this->db->query("SELECT player,price FROM auction WHERE offer=".$offer['id']." ORDER BY price DESC LIMIT 1");
		if (!($q && dbCount($q))) {
			// Insert entry in player's history
			$this->db->query("UPDATE sale_history SET ended=$tm,end_mode=3,sell_price=NULL WHERE offer=".$offer['id']);
			// FIXME: send message
			$this->db->query("DELETE FROM sale WHERE id=".$offer['id']);
			return;
		}
		list($buyer,$price) = dbFetchArray($q);

		// Refund players who have failed to buy the item
		$q = $this->db->query("SELECT player,MAX(price) FROM auction WHERE offer=".$offer['id']." AND player<>$buyer GROUP BY player");
		while ($r = dbFetchArray($q)) {
			// FIXME: send message
			$this->db->query("UPDATE player SET cash=cash+".$r[1]." WHERE id=".$r[0]);
		}

		// Mark the transaction as finalized
		$this->db->query("UPDATE sale SET finalized=$tm,sold_to=$buyer WHERE id=".$offer['id']);
		$this->db->query("UPDATE public_offer SET price=$price WHERE offer=".$offer['id']);
		$this->db->query("DELETE FROM auction WHERE offer=".$offer['id']);
		$this->db->query("UPDATE player SET cash=cash+$price WHERE id=".$offer['player']);

		// Inform both the current owner and the buyer
		// FIXME

		// Insert history entry
		$this->db->query("UPDATE sale_history SET ended=$tm,end_mode=2,sell_price=$price,to_player=$buyer WHERE offer=".$offer['id']);

		// Mark the items for sale
		if (!is_null($offer['planet'])) {
			$this->db->query("UPDATE planet SET sale=3 WHERE id=".$offer['planet']);
		}
		if (!is_null($offer['fleet'])) {
			$this->db->query("UPDATE fleet SET sale=3 WHERE id=".$offer['fleet']);
		}
	}
}

?>
