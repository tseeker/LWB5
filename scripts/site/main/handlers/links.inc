<?php

class page_handler {
	var $needsAuth = false;
	var $ajax = array();


	function makeLinksList() {
		$categories = $this->lib->call('getCategories');
		for ($i=0;$i<count($categories);$i++) {
			$categories[$i]['links'] = $this->lib->call('getLinks', $categories[$i]['id']);
		}

		$this->data = array(
			"mode"	=> 0,
			"data"	=> $categories
		);
	}


	function handleBrokenReport($account, $link, $confirm) {
		// Get existing reports
		$reports = $this->lib->call('getBrokenReports');
		$myReports = array();
		foreach ($reports as $r) {
			if ($r['reported_by'] == $account) {
				array_push($myReports, $r['link']);
			}
		}

		if (is_null($link) || $link == "") {
			// No link reported yet, get the list
			$links = array();
			$categories = $this->lib->call('getCategories');
			foreach ($categories as $cat) {
				$clinks = $this->lib->call('getLinks', $cat['id']);
				foreach ($clinks as $lnk) {
					if (in_array($lnk['id'], $myReports)) {
						continue;
					}
					$links[$lnk['id']] = utf8entities($cat['title'] . " -> " . $lnk['title']);
				}
			}

			$this->data = array(
				"mode"	=> 1,
				"data"	=> $links
			);
		} elseif ($confirm == '') {
			// A link has been reported, but the report hasn't been confirmed yet
			$lk = $this->lib->call('getLink', (int)$link);
			if (is_null($lk) || in_array((int)$link, $myReports)) {
				return;
			}
			$cat = $this->lib->call('getCategory', $lk['category']);
			$lk['long_title'] = utf8entities($cat['title'] . " -> " . $lk['title']);
			$this->data = array(
				"mode"	=> 2,
				"data"	=> $lk
			);
		} else {
			// A link has been reported and the user confirmed
			$lk = $this->lib->call('getLink', (int)$link);
			if (is_null($lk) || in_array((int)$link, $myReports)) {
				return;
			}
			$cat = $this->lib->call('getCategory', $lk['category']);
			$lk['long_title'] = utf8entities($cat['title'] . " -> " . $lk['title']);

			$this->lib->call('reportBroken', (int)$link, $account);

			$this->data = array(
				"mode"	=> 3,
				"data"	=> $lk
			);
		}
	}


	function handleSubmission(&$input) {
		$sl = (int)$input['sl'];
		if ($sl == 1) {
			// Initialise the form
			$this->data = array(
				"mode"	=> 4,
				"data"	=> array(
					"url"	=> "http://",
					"title"	=> "",
					"desc"	=> "",
					"err"	=> 0
				)
			);
		} elseif ($sl == 2) {
			// Check submitted data
			$title = preg_replace('/\s+/', ' ', trim($input['title']));
			$desc = preg_replace('/\s+/', ' ', trim($input['desc']));
			$url = trim($input['url']);

			// Check title
			if (strlen($title) < 5) {
				$error = 1;
			} elseif (strlen($title) > 64) {
				$error = 2;
			// Check description
			} elseif ($desc != "" && strlen($desc) < 10) {
				$error = 3;
			// Check URL
			} elseif (!preg_match('/^(http|https):\/\/(([A-Z0-9][A-Z0-9_-]*)(\.[A-Z0-9][A-Z0-9_-]*)+)(:(\d+))?(\/.*)?$/i', $url, $m)) {
				$error = 4;
			} else {
				list($junk, $proto, $hostname, $port) = $m;
				if (!preg_match('/^\d+\.\d+\.\d+\.\d+$/', $hostname)) {
					$ip = gethostbyname($hostname);
					if ($ip === $hostname) {
						$error = 5;
					} else {
						$error = 0;
					}
				} else {
					$error = 0;
				}
			}

			// If there was no error, check whether this user already submitted the link
			if ($error == 0 && !$this->lib->call('checkSubmission', $url, $_SESSION['userid'])) {
				$error = 6;
			}

			// Generate output
			if ($error) {
				$this->data = array(
					"mode"	=> 4,
					"data"	=> array(
						"url"	=> $url,
						"title"	=> $title,
						"desc"	=> $desc,
						"err"	=> $error
					)
				);
			} else {
				$this->lib->call('submitLink', $url, $title, $desc == "" ? null : $desc, $_SESSION['userid']);
				$this->data = array(
					"mode"	=> 5,
					"data"	=> $url
				);
			}
		}
	}


	function handle($input) {
		$this->lib = $this->game->getLib('main/links');

		if (is_array($_SESSION) && !is_null($_SESSION['userid'])) {
			$account = $_SESSION['userid'];

			if (!is_null($input['rbl']) && is_null($input['cancel'])) {
				$this->handleBrokenReport($account, $input['id'], $input['confirm']);
			} elseif (!is_null($input['sl']) && is_null($input['cancel'])) {
				$this->handleSubmission($input);
			}
		}

		if (is_null($this->data)) {
			$this->makeLinksList();
		}

		$this->output = "links";
	}
}

?>
