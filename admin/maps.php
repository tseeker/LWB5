<?php

include('config.inc');
include('ctf_map.inc');
session_start();


function handleInput() {
	if ($_GET['c'] == 'n') {
		$op = 'e';
		$_SESSION['edit_map'] = new ctf_map();
	} elseif ($_GET['c'] == 'e') {
		$op = 'e';
		$_SESSION['edit_map'] = new ctf_map($_GET['id']);
	} elseif ($_GET['c'] == 'd') {
		if ($_GET['ok']) {
			$map = new ctf_map($_GET['id']);
			$map->destroy();
			$op = '';
		} else if ($_GET['cancel']) {
			$op = "";
		} else {
			$op = 'd';
		}
	} elseif ($_POST['c'] == 'ms' && $_SESSION['edit_map'] instanceof ctf_map) {
		$map = $_SESSION['edit_map'];
		$map->setName(stripslashes($_POST['name']));
		$map->setDescription(stripslashes($_POST['desc']));
		$map->setWidth((int) $_POST['width']);
		$map->setHeight((int) $_POST['height']);
		$map->setAlliances((int) $_POST['alliances']);

		$minY = -floor($map->getHeight() / 2); $maxY = $minY + $map->getHeight() - 1;
		$minX = -floor($map->getWidth() / 2); $maxX = $minX + $map->getWidth() - 1;

		$layout = explode('#', $_POST['map']);
		for ($y = $minY; $y <= $maxY; $y ++) {
			$str = array_shift($layout);
			for ($x = $minX; $x <= $maxX; $x ++) {
				$type = $str{0};
				$map->setSystemType($x, $y, $type);
				if ($type != 'S') {
					$str = substr($str, 1);
					continue;
				}
				$alloc = (int) $str{1};
				$map->setSystemAlloc($x, $y, $alloc);
				if ($alloc == 0) {
					$str = substr($str, 2);
					continue;
				}
				$map->setSystemSpawn($x, $y, $str{2} == '1');
				$str = substr($str, 3);
			}
		}
		$map->save();
		$_SESSION['edit_map'] = null;
		$op = '';
	} else {
		$op = '';
	}
	return $op;
}



function listMaps() {
?>
  <h2>Available maps</h2>
<?php
	$maps = ctf_map::allMaps();
	if (count($maps) == 0) {
?>
  <p>
   There are no maps on the server at this time.
  </p>
<?php
	} else {
?>
  <table border="1">
   <tr>
    <th align="left">Name &amp; description</th>
    <th align="center">Size</th>
    <th align="center">Alliances</th>
    <th align="left">&nbsp;</th>
   </tr>
<?php

	foreach ($maps as $map) {
?>
   <tr>
    <td style='vertical-align:top'><?="<u>" . htmlentities($map->getName()) . "</u>"
	. (is_null($map->getDescription()) ? "" : ("<br/>" . htmlentities($map->getDescription())))?></td>
    <td style='text-align:center'><?=$map->getWidth()?>x<?=$map->getHeight()?></td>
    <td style='text-align:center'><?=$map->getAlliances()?></td>
    <td><a href="?c=e&id=<?=$map->getID()?>">Edit</a> - <a href="?c=d&id=<?=$map->getID()?>">Delete</a></td>
   </tr>
<?php
	}

?>
  </table>
<?
	}
?>
  <p>
   <a href='?c=n'>Create a map</a>
  </p>
<?php
}


function editMap() {
?>
  <h2>Map editor</h2>
  <form method="POST" action="?" onSubmit="return false;">
   <div id="mapedit"><p>Loading, please wait ...</p></div>
  </form>
  <form method="POST" action="?" id="sendform" style="display: none">
   <input type="hidden" name="c" value="ms" />
   <input type="hidden" id="sf-name" name="name" value="" />
   <input type="hidden" id="sf-desc" name="desc" value="" />
   <input type="hidden" id="sf-width" name="width" value="" />
   <input type="hidden" id="sf-height" name="height" value="" />
   <input type="hidden" id="sf-alliances" name="alliances" value="" />
   <input type="hidden" id="sf-map" name="map" value="" />
  </form>
  <script language="JavaScript"><!--
var initMap = {
	name: '<?=addslashes($_SESSION['edit_map']->getName())?>',
	description: '<?=preg_replace(array('/\\n/', '/\\r/'), array('\\n', '\\r'), addslashes($_SESSION['edit_map']->getDescription()))?>',
	alliances: <?=$_SESSION['edit_map']->getAlliances()?>,
	width: <?=$_SESSION['edit_map']->getWidth()?>,
	height: <?=$_SESSION['edit_map']->getHeight()?>,
	map: [ <?=$_SESSION['edit_map']->jsDump()?> ]
};
//--></script>
<script language="JavaScript" src="map_edit.js"></script>
<?
}


function confirmDelete() {
	$map = new ctf_map((int)$_GET['id']);
?>
  <h2>Map deletion</h2>
  <p>
   You are about to delete the map called <b><?=htmlentities($map->getName())?></b>.<br/>
   Please confirm.
  </p>
  <form method="GET" action="?">
   <input type="hidden" name="c" value="d" />
   <input type="hidden" name="id" value="<?=$map->getID()?>" />
   <input type="submit" name="ok" value="Confirm" />
   <input type="submit" name="cancel" value="Cancel" />
  </form>
<?
}


?>
<html>
 <head>
  <title>LegacyWorlds Beta 5 > Administration > Maps management</title>
 </head>
 <body>
  <h1><a href="index.html">LWB5 > Administration</a> > Maps management</h1>
  <p>
   The purpose of this tool is to create new maps, edit existing ones or delete unused ones.
  </p>
<?php

$h = handleInput();

switch($h) :
case 'e':
	editMap();
	break;
case 'd':
	confirmDelete();
	break;
default:
	listMaps();
	break;
endswitch;

?>
 </body>
</html>
