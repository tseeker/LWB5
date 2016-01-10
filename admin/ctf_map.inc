<?php


class ctf_map_sys {

	private $x;
	private $y;
	private $type;
	private $alloc;
	private $spawn;

	public function __construct($x, $y, $type = null, $alloc = null, $spawn = null) {
		$this->x = $x;
		$this->y = $y;
		$this->type = $type;
		$this->alloc = $alloc;
		if (is_bool($spawn)) {
			$this->spawn = $spawn;
		} elseif (is_string($spawn)) {
			$this->spawn = ($spawn == 't');
		} else {
			$this->spawn = null;
		}
	}

	public function setType($type) {
		if (is_null($type) || !in_array((string) $type, array('S', '1', '2', '3', '4'))) {
			throw new Exception('Invalid type');
		}

		$this->type = $type;
		if ($this->type != 'S') {
			$this->alloc = $this->spawn = null;
		} elseif (is_null($this->alloc)) {
			$this->alloc = 0;
			$this->spawn = null;
		}
	}

	public function setAllocation($alloc) {
		if ($alloc < 0 || $this->type != 'S') {
			throw new Exception('Invalid allocation type');
		}
		$this->alloc = $alloc;

		if ($this->alloc != 0 && is_null($this->spawn)) {
			$this->spawn = false;
		} elseif ($this->alloc = 0) {
			$this->spawn = null;
		}
	}

	public function setSpawn($spawn) {
		if (is_null($this->alloc) || $this->alloc == 0 || !is_bool($spawn)) {
			throw new Exception("Invalid spawning value");
		}
		$this->spawn = $spawn;
	}


	public function getType() {
		return $this->type;
	}

	public function getAllocation() {
		return $this->alloc;
	}

	public function getSpawn() {
		return $this->spawn;
	}

	public function getX() {
		return $this->x;
	}

	public function getY() {
		return $this->y;
	}


	public function store($db, $map) {
		$q = pg_query_params($db, "INSERT INTO main.ctf_map_layout(map,sys_x,sys_y,sys_type,alloc_for,spawn_here)"
			. " VALUES ($map,$1,$2,$3,$4,$5)", array(
				$this->x, $this->y, $this->type, $this->alloc,
				($this->type == 'S' && $this->alloc > 0) ? ($this->spawn ? 't' : 'f') : null
			));
		if (!$q) {
			throw new Exception("Failed to store system at {$this->x};{$this->y}");
		}
	}
}


class ctf_map {

	private $dbID;
	private $name;
	private $description;
	private $width;
	private $height;
	private $alliances;
	private $map;

	public static function allMaps() {
		$db = __dbConnect();
		if (! $db) {
			throw new Exception('Unable to connect to the database');
		}

		$result = pg_query($db, "SELECT id FROM main.ctf_map_def ORDER BY id");
		if (!$result) {
			throw new Exception("Database error");
		}

		$IDs = array();
		while ($row = pg_fetch_array($result)) {
			array_push($IDs, $row[0]);
		}

		pg_free_result($result);
		pg_close($db);

		$maps = array();
		foreach ($IDs as $id) {
			$maps[$id] = new ctf_map($id);
		}
		return $maps;
	}

	public function __construct($dbID = null) {
		if (is_null($dbID)) {
			$this->dbID = $this->name = $this->description = null;
			$this->alliances = 2;
			$this->width = $this->height = 3;
		} else {
			$db = __dbConnect();
			if (! $db) {
				throw new Exception("Unable to connect to the database");
			}

			$result = pg_query($db, "SELECT * FROM main.ctf_map_def WHERE id = $dbID");
			if (!($result && pg_num_rows($result) == 1)) {
				pg_close($db);
				throw new Exception("Definition '$dbID' not found");
			}

			$row = pg_fetch_assoc($result);
			pg_free_result($result);
			pg_close($db);

			$this->dbID = $dbID;
			$this->name = $row['name'];
			$this->description = $row['description'];
			$this->width = $row['width'];
			$this->height = $row['height'];
			$this->alliances = $row['alliances'];
		}
		$this->map = null;
	}


	public function getID() {
		return $this->dbID;
	}

	public function getName() {
		return $this->name;
	}

	public function getDescription() {
		return $this->description;
	}

	public function getWidth() {
		return $this->width;
	}

	public function getHeight() {
		return $this->height;
	}

	public function getAlliances() {
		return $this->alliances;
	}


	public function setName($value) {
		$this->name = $value;
	}

	public function setDescription($value) {
		$this->description = ($value == '') ? null : $value;
	}

	public function setAlliances($value) {
		$this->alliances = $value;
	}


	public function setWidth($value) {
		if (is_null($this->map) && !is_null($this->dbID)) {
			$this->load();
		}

		$this->width = $value;
		if (is_array($this->map)) {
			$this->initBlankMap();
		}
	}

	public function setHeight($value) {
		if (is_null($this->map) && !is_null($this->dbID)) {
			$this->load();
		}

		$this->height = $value;
		if (is_array($this->map)) {
			$this->initBlankMap();
		}
	}


	public function getMapInfo($x, $y) {
		if (! is_array($this->map)) {
			if ($this->dbID) {
				$this->load();
			} else {
				return null;
			}
		}

		$offset = $this->getOffset($x, $y);
		return $this->map[$offset];
	}


	public function setSystemType($x, $y, $t) {
		if (! is_array($this->map)) {
			if ($this->dbID) {
				$this->load();
			} else {
				$this->initBlankMap();
			}
		}

		$offset = $this->getOffset($x, $y);
		$v = $this->map[$offset];

		if (is_null($v)) {
			$v = $this->map[$offset] = new ctf_map_sys($x, $y, $t);
		} else {
			$v->setType($t);
		}
	}

	public function setSystemAlloc($x, $y, $a) {
		if (! is_array($this->map)) {
			if ($this->dbID) {
				$this->load();
			} else {
				$this->initBlankMap();
			}
		}

		$offset = $this->getOffset($x, $y);
		$v = $this->map[$offset];

		if (is_null($v)) {
			$v = $this->map[$offset] = new ctf_map_sys($x, $y, 'S');
		}
		if ($v->getType() != 'S') {
			throw new Exception("Can't set allocation type for a nebula");
		}
		if ($a < 0 || $a > $this->alliances) {
			throw new Exception("Invalid allocation type $a");
		}
		$v->setAllocation($a);
	}

	public function setSystemSpawn($x, $y, $s) {
		if (! is_array($this->map)) {
			if ($this->dbID) {
				$this->load();
			} else {
				$this->initBlankMap();
			}
		}

		$offset = $this->getOffset($x, $y);
		$v = $this->map[$offset];

		if (is_null($v)) {
			$v = $this->map[$offset] = new ctf_map_sys($x, $y, 'S', 1, $s);
		} else {
			if ($v->getType() != 'S' || $v->getAllocation() < 1) {
				throw new Exception("Can't set spawning point");
			}
			$v->setSpawn($s);
		}
	}


	public function jsDump() {
		$parts = array();

		if (is_null($this->map)) {
			if ($this->dbID) {
				$this->load();
			} else {
				$this->initBlankMap();
			}
		}

		foreach ($this->map as $s) {
			if (is_null($s)) {
				continue;
			}
			$str = '[' . $s->getX() . ',' . $s->getY() . ',"' . $s->getType() . '",';
			if ($s->getType() != 'S') {
				$str .= 'null,null';
			} else {
				$str .= $s->getAllocation() . ',';
				if ($s->getAllocation()) {
					$str .= $s->getSpawn() ? 'true' : 'false';
				} else {
					$str .= 'null';
				}
			}
			array_push($parts, "$str]");
		}

		return join(',', $parts);
	}


	public function save() {
		$db = __dbConnect();
		if (!$db) {
			throw new Exception("Database error");
		}

		pg_query($db, "BEGIN TRANSACTION");
		pg_query($db, "SET search_path = public, main");
		if ($this->dbID) {
			$id = $this->dbID;
			pg_query_params($db, "UPDATE main.ctf_map_def SET name=$2, description=$3, alliances=$4, "
				. "width=$5, height=$6 WHERE id=$1", array(
					$id, $this->name, $this->description, $this->alliances,
					$this->width, $this->height
				));
			pg_query($db, "DELETE FROM main.ctf_map_layout WHERE map=$id");
		} else {
			pg_query_params($db, "INSERT INTO main.ctf_map_def(name,description,alliances,width,height) "
				. "VALUES ($1, $2, $3, $4, $5)", array(
					$this->name, $this->description, $this->alliances,
					$this->width, $this->height
				));
			$q = pg_query($db, "SELECT last_inserted('ctf_map_def')");
			if (!($q && pg_num_rows($q))) {
				throw new Exception("Database error");
			}
			list($id) = pg_fetch_array($q);
		}

		foreach ($this->map as $sys) {
			$sys->store($db, $id);
		}

		pg_query($db, "COMMIT");
		pg_close($db);

		$this->dbID = $id;
	}

	public function destroy() {
		if (! $this->dbID) {
			return;
		}
		$db = __dbConnect();
		if (!$db) {
			throw new Exception("Database error");
		}

		pg_query($db, "DELETE FROM main.ctf_map_def WHERE id={$this->dbID}");
		pg_close($db);
	}


	private function load() {
		$db = __dbConnect();
		if (!$db) {
			throw new Exception("Database error");
		}
		$result = pg_query($db, "SELECT * FROM main.ctf_map_layout WHERE map = {$this->dbID}");
		if (!($result && pg_num_rows($result) == $this->height * $this->width)) {
			throw new Exception("Unable to load map");
		}

		$map = array();
		while ($row = pg_fetch_assoc($result)) {
			$offset = $this->getOffset($row['sys_x'], $row['sys_y']);
			$map[$offset] = new ctf_map_sys($row['sys_x'], $row['sys_y'],
				$row['sys_type'], $row['alloc_for'], $row['spawn_here']);
		}
		$this->map = $map;

		pg_free_result($result);
		pg_close($db);
	}

	private function initBlankMap() {
		$map = array();
		for ($i = 0; $i < $this->width * $this->height; $i ++) {
			$map[$i] = null;
		}
		$this->map = $map;
	}

	private function getOffset($x, $y) {
		return $this->width * (floor($this->height / 2) + $y) + floor($this->width / 2) + $x;
	}
}

?>
