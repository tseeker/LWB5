<?php

class page_handler {
	var $needsAuth = true;
	var $ajax = array();

	private function getCount() {
		list($total) = dbFetchArray($this->db->query('SELECT COUNT(*) FROM b6_planet_pics'));
		list($rated) = dbFetchArray($this->db->query("SELECT COUNT(*) FROM b6_planet_votes WHERE account = {$_SESSION['userid']}"));
		$this->data['total'] = $total;
		$this->data['rated'] = $rated;
	}

	private function getRandomUnrated() {
		$user = $_SESSION['userid'];
		$q = $this->db->query(
			"SELECT * FROM b6_planet_pics WHERE id NOT IN (SELECT picture FROM b6_planet_votes WHERE account = $user) ORDER BY RANDOM()"
		);
		if (! dbCount($q)) {
			return 0;
		}

		$data = dbFetchHash($q);
		return $data;
	}

	private function storeRating($id, $rating) {
		if ($rating < 1 || $rating > 5) {
			return;
		}

		$q = $this->db->query("SELECT id FROM b6_planet_pics WHERE id = $id");
		if (! dbCount($q)) {
			return;
		}

		$user = $_SESSION['userid'];

		$q = $this->db->query("SELECT account FROM b6_planet_votes WHERE picture = $id AND account = $user");
		if (dbCount($q)) {
			return;
		}

		$this->db->query("INSERT INTO b6_planet_votes (account, picture, vote) VALUES ($user, $id, $rating)");
		$this->db->query("UPDATE credits SET credits_obtained = credits_obtained + 120 WHERE account = $user");
	}

	private function doRatings() {
		$pd = $this->getRandomUnrated();
		if (! $pd) {
			$this->data = array(
				'page'	=> 'nu'
			);
		} else {
			$this->data = array(
				'page'	=> 'vp',
				'pic'	=> $pd,
				'cr'	=> null,
				'ar'	=> null,
				'nv'	=> null
			);
		}
	}

	private function getPlanet($id) {
		$q = $this->db->query("SELECT * FROM b6_planet_pics WHERE id = $id");
		if (! dbCount($q)) {
			$this->doRatings();
			return;
		}
		$pd = dbFetchHash($q);

		$user = $_SESSION['userid'];
		$q = $this->db->query("SELECT vote FROM b6_planet_votes WHERE picture = $id AND account = $user");
		if (! dbCount($q)) {
			$this->data = array(
				'page'	=> 'vp',
				'pic'	=> $pd,
				'cr'	=> null,
				'ar'	=> null,
				'nv'	=> null
			);
			return;
		}
		list($cr) = dbFetchArray($q);

		$q = $this->db->query("SELECT AVG(vote), COUNT(*) FROM b6_planet_votes WHERE picture = $id");
		list($ar, $nv) = dbFetchArray($q);
		$this->data = array(
			'page'	=> 'vp',
			'pic'	=> $pd,
			'cr'	=> $cr,
			'ar'	=> sprintf("%.2f", $ar),
			'nv'	=> $nv
		);
	}

	private function topRatings() {
		$q = $this->db->query("SELECT COUNT(*) AS votes FROM b6_planet_votes GROUP BY picture");
		if (!dbCount($q)) {
			$this->data = array(
				'page'	=> 'nt'
			);
			return;
		}

		$sum = 0;
		while ($r = dbFetchArray($q)) {
			$sum += $r[0];
		}
		$sum /= dbCount($q);

		$q = $this->db->query("SELECT picture, AVG(vote) AS rating, COUNT(*) AS votes FROM b6_planet_votes "
			. "GROUP BY picture ORDER BY rating DESC, votes DESC, picture");
		if (!dbCount($q)) {
			$this->data = array(
				'page'	=> 'nt'
			);
		} else {
			$this->data = array(
				'page'	=> 'tt',
				'pics'	=> array()
			);
			while ($r = dbFetchHash($q)) {
				if ($r['votes'] >= $sum) {
					array_push($this->data['pics'], $r);
				}
				if (count($this->data['pics']) >= 50) {
					break;
				}
			}
		}
	}

	public function handle($input) {
		$this->db = $this->game->getDBAccess();
		$this->output = 'b6pp';

		$command = $input['c'];
		if ($command == 'v') {
			$id = (int) $input['id'];
			$this->getPlanet($id);
		} elseif ($command == 'r') {
			$id = (int) $input['id'];
			$r = (int) $input['r'];
			$this->storeRating($id, $r);
			$this->doRatings();
		} elseif ($command == 't') {
			$this->topRatings();
		} else {
			$this->doRatings();
		}

		$this->getCount();
	}
}

?>
