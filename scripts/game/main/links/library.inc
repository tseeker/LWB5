<?php

class main_links_library {
	var $index	= array();

	function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function getCategories() {
		$q = $this->db->query("SELECT * FROM lk_category ORDER BY position");
		if (!$q) {
			return array();
		}

		$res = array();
		while ($r = dbFetchHash($q)) {
			array_push($res, $r);
		}

		return $res;
	}

	function getCategory($id) {
		$q = $this->db->query("SELECT * FROM lk_category WHERE id=$id");
		if (!($q && count($q))) {
			return null;
		}
		return dbFetchHash($q);
	}

	function checkCategoryName($name, $id) {
		$qs = is_null($id) ? "" : " AND id<>$id";
		$q = $this->db->query("SELECT * FROM lk_category WHERE title='" . addslashes($name) . "'$qs");
		return (dbCount($q) == 0);
	}

	function createCategory($title, $description, $after = null) {
		$q = $this->db->query("SELECT id,position FROM lk_category ORDER BY position DESC FOR UPDATE");
		if (is_null($after)) {
			while ($r = dbFetchArray($q)) {
				$this->db->query("UPDATE lk_category SET position=position+1 WHERE id={$r[0]}");
			}
			$pos = 0;
		} else {
			$act = false;
			while ($r = dbFetchArray($q)) {
				if ($act) {
					$this->db->query("UPDATE lk_category SET position=position+1 WHERE id={$r[0]}");
				} else {
					$act = ($r[0] == $after);
					if ($act) {
						$pos = $r[1];
					}
				}
			}
		}

		$id = $this->db->query("INSERT INTO lk_category(position,title" . (is_null($description) ? "" : ",description")
			. ") VALUES ($pos,'" . addslashes($title) . "'" . (is_null($description) ? "" : (",'" . addslashes($description) . "'"))
			. ")");
		return $id;
	}

	function changeCategory($id, $title, $description) {
		$this->db->query("UPDATE lk_category SET title='" . addslashes($title) . "',description="
			. ($description == "" ? "NULL" : ("'" . addslashes($description) . "'"))
			. " WHERE id=$id");
	}

	function deleteCategory($id) {
		$this->db->query("SELECT * FROM lk_category FOR UPDATE");

		$q = $this->db->query("SELECT position FROM lk_category WHERE id=$id");
		if (!($q && count($q))) {
			return;
		}
		list($pos) = dbFetchArray($q);

		$this->db->query("DELETE FROM lk_category WHERE id=$id");
		$this->db->query("UPDATE lk_category SET position=position-1 WHERE position>$pos");
	}

	function moveCategory($id, $up) {
		$this->db->query("SELECT * FROM lk_category FOR UPDATE");

		$q = $this->db->query("SELECT MAX(position)+1 FROM lk_category");
		list($mPos) = dbFetchArray($q);
		$q = $this->db->query("SELECT position FROM lk_category WHERE id=$id");
		list($pos) = dbFetchArray($q);

		$this->db->query("UPDATE lk_category SET position=$mPos WHERE id=$id");
		if ($up) {
			$this->db->query("UPDATE lk_category SET position=position+1 WHERE position=" . ($pos-1));
			$this->db->query("UPDATE lk_category SET position=".($pos-1)." WHERE id=$id");
		} else {
			$this->db->query("UPDATE lk_category SET position=position-1 WHERE position=" . ($pos+1));
			$this->db->query("UPDATE lk_category SET position=".($pos+1)." WHERE id=$id");
		}
	}


	function getLinks($category) {
		$q = $this->db->query("SELECT * FROM lk_link WHERE category=$category ORDER BY title");
		if (!$q) {
			return array();
		}

		$res = array();
		while ($r = dbFetchHash($q)) {
			array_push($res, $r);
		}

		return $res;
	}

	function getLink($id) {
		$q = $this->db->query("SELECT * FROM lk_link WHERE id=$id");
		if (!($q && count($q))) {
			return null;
		}
		return dbFetchHash($q);
	}

	function checkLink($url, $id = null) {
		$qs = is_null($id) ? "" : " AND id<>$id";
		$q = $this->db->query("SELECT * FROM lk_link WHERE url='".addslashes($url) . "'$qs");
		return (dbCount($q) == 0);
	}

	function addLink($category, $title, $description, $url) {
		return $this->db->query("INSERT INTO lk_link(category,url,title" . (is_null($description) ? "" : ",description")
			. ") VALUES ($category,'" . addslashes($url) . "','" . addslashes($title) . "'"
			. (is_null($description) ? "" : (",'" . addslashes($description) . "'")) . ")");
	}

	function changeLink($id, $title, $url, $description) {
		$this->db->query("UPDATE lk_link SET title='" . addslashes($title) . "',description="
			. ($description == "" ? "NULL" : ("'" . addslashes($description) . "'"))
			. ",url='" . addslashes($url) . "' WHERE id=$id");
	}

	function deleteLink($id) {
		$this->db->query("DELETE FROM lk_link WHERE id=$id");
	}


	function getBrokenReports() {
		$q = $this->db->query("SELECT * FROM lk_broken");
		if (!$q) {
			return array();
		}

		$res = array();
		while ($r = dbFetchHash($q)) {
			array_push($res, $r);
		}

		return $res;
	}

	function reportBroken($link, $account) {
		$this->db->query("INSERT INTO lk_broken VALUES($link, $account)");
	}

	function deleteReports($link) {
		$this->db->query("DELETE FROM lk_broken WHERE link=$link");
	}


	function getSubmissions() {
		$q = $this->db->query("SELECT * FROM lk_submitted");
		if (!$q) {
			return array();
		}

		$res = array();
		while ($r = dbFetchHash($q)) {
			array_push($res, $r);
		}

		return $res;
	}

	function getSubmission($url, $account) {
		$q = $this->db->query("SELECT * FROM lk_submitted WHERE url='" . addslashes($url)
			. "' AND submitted_by=$account");
		if (!($q && dbCount($q))) {
			return null;
		}
		return dbFetchHash($q);
	}

	function checkSubmission($url, $account) {
		$q = $this->db->query("SELECT * FROM lk_submitted WHERE url='" . addslashes($url)
			. "' AND submitted_by=$account");
		return (dbCount($q) == 0);
	}

	function submitLink($url, $title, $description, $account) {
		$this->db->query("INSERT INTO lk_submitted VALUES('" . addslashes($url) . "',$account,'" . addslashes($title)
			. "'," . (is_null($description) ? "NULL" : ("'" . addslashes($description) . "'")) . ")");
	}

	function deleteSubmissions($url) {
		$this->db->query("DELETE FROM lk_submitted WHERE url='" . addslashes($url) . "'");
	}
}

?>
