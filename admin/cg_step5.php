<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

include("config.inc");
include("as_manager.inc");

class dbParser {

	static private $instructions = array();
	static private $aConnection;
	static private $uConnection;
	static private $cConnection = null;

	static public function parse($contents) {
		$all = explode("\n", $contents);
		$state = 0;

		foreach ($all as $line) {
//			print "PARSING LINE: $line<br/>\n";
			if ($state == 0) {
				// Remove comments
				$line = preg_replace('/\s*--.*$/', '', $line);
				if ($line == '') {
					continue;
				}

				// \c ?
				if (preg_match('/^\\\\c\s+[^\s]+\s([^\s]+)\s*$/', $line, $match)) {
					if ($match[1] == "legacyworlds_admin") {
						//echo "<b>CONNECT AS ADMIN</b><br/>";
						array_push(self::$instructions, array("CADM"));
					} else {
						//echo "<b>CONNECT AS USER</b><br/>";
						array_push(self::$instructions, array("CUSR"));
					}
				}
				// \i ?
				elseif (preg_match('/^\\\\i\s+([^\s]+)\s*$/', $line, $match)) {
					self::parseFile($match[1]);
				}
				// COPY table FROM STDIN ?
				elseif (preg_match('/^COPY\s+([^\s]+)\s+FROM\s+STDIN\s*;\s*$/i', $line, $match)) {
					//echo "<b>COPY DATA</b> INTO {$match[1]}: ";
					array_push(self::$instructions, array("COPY", $match[1]));
					$state = 1;
					$copyArray = array();
				}
				// Other commands
				else {
					$rv = self::parseCommand($line);
					if ($rv[0]) {
						//echo "<b>EXECUTE QUERY</b> " . htmlentities($rv[1]) . "<br/>\n";
						array_push(self::$instructions, array("QUERY", $rv[1]));
					} else {
						$buffer = $rv[1];
						$state = 2;
					}
				}
			} elseif ($state == 1) {
				if ($line == "\\.") {
					//echo count($copyArray) . " LINE(S) OF DATA<br/>\n";
					self::$instructions[count(self::$instructions) - 1][2] = $copyArray;
					$state = 0;
				} else {
					array_push($copyArray, "$line\n");
				}
			} elseif ($state == 2) {
				$rv = self::parseCommand($line, $buffer);
				if ($rv[0]) {
					//echo "<b>EXECUTE QUERY</b> " . htmlentities($rv[1]) . "<br/>\n";
					array_push(self::$instructions, array("QUERY", $rv[1]));
					$state = 0;
				} else {
					$buffer = $rv[1];
				}
			}
		}
	}

	private static function parseFile($fileName) {
		$file = fopen("../sql/$fileName", "r");
		$text = "";
		while (($line = fgets($file)) !== FALSE) {
			$text .= $line;
		}
		fclose($file);
		self::parse($text);
	}

	private static function parseCommand($line, $buffer = "") {
		$state = 0;
		for ($i = 0; $i < strlen($line); $i ++) {
			if ($state == 0) {
				if ($line{$i} == ';') {
					return array(true, $buffer);
				}
				$buffer .= $line{$i};
				if ($line{$i} == "'") {
					$state = 1;
				}
			} else {
				$buffer .= $line{$i};
				if ($line{$i} == "'") {
					$state = 0;
				}
			}
		}
		return array(false, "$buffer ");
	}

	public static function connect() {
		self::$uConnection = __dbConnect(false);
		if (!self::$uConnection) {
			$argh = "Error while accessing the database in user mode";
			include('cg_argh.inc');
			exit(1);
		}

		self::$aConnection = __dbConnect(true);
		if (!self::$aConnection) {
			$argh = "Error while accessing the database in admin mode";
			include('cg_argh.inc');
			exit(1);
		}
	}

	public static function execute($schema) {
		foreach (self::$instructions as $instr) {
			$iType = array_shift($instr);

			if ($iType == 'CADM') {
				self::setConnection(self::$aConnection);
			} elseif ($iType == 'CUSR') {
				self::setConnection(self::$aConnection);
			} elseif ($iType == 'QUERY') {
				if (!pg_query(self::$cConnection, $instr[0])) {
					$argh = "Could not execute query {$instr[0]}";
					pg_query(self::$aConnection, "DROP SCHEMA \"$schema\" CASCADE");
					include('cg_argh.inc');
					exit(1);
				}
			} elseif ($iType == 'COPY') {
				if (!pg_copy_from(self::$cConnection, $instr[0], $instr[1])) {
					$argh = "Copy failed for table {$instr[0]}";
					pg_query(self::$aConnection, "DROP SCHEMA \"$schema\" CASCADE");
					include('cg_argh.inc');
					exit(1);
				}
			}
		}
	}

	public static function setConnection($cnx) {
		self::$cConnection = $cnx;
	}
}

$gtData = array(
	"r"	=> array('round', 'r'),
	"m"	=> array('match', 'm'),
	"c"	=> array('ctf', 'm'),
);
$gData = $gtData[$_SESSION['lw_new_game']['game_type']];

$substStr = "b5" . $gData[1] . "X";

// Load base match script
$fileName = "../sql/beta5/beta5-" . $gData[0] . ".sql";
$baseFile = fopen($fileName, "r");
$base = "";
while (($line = fgets($baseFile)) !== FALSE) {
	$base .= preg_replace("/$substStr/", $_SESSION['lw_new_game']['found_id'], $line);
}
fclose($baseFile);

dbParser::parse($base);
dbParser::connect();
dbParser::execute($_SESSION['lw_new_game']['found_id']);

if (__isManagerRunning()) {
	$op = array(
		"pc"	=> 50,
		"text"	=> "Stopping all ticks",
		"delay"	=> 1,
		"to"	=> "cg_step6.php"
	);
} else {
	$op = array(
		"pc"	=> 60,
		"text"	=> "Generating configuration",
		"delay"	=> 1,
		"to"	=> "cg_step8.php"
	);
}

include('cg_operation.inc');

?>
