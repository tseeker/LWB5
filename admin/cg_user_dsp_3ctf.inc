<?php

if (!is_array($ctfMaps)) {
	require_once("config.inc");
	$cnx = __dbConnect();
	if (!$cnx) {
		print "<b>DATABASE CONNECTION ERROR</b>";
		exit(1);
	}
	$query = pg_query("SELECT * FROM main.ctf_map_def ORDER BY name");
	if (!$query) {
		print "<b>DATABASE ERROR</b>";
		exit(1);
	}

	$ctfMaps = array();
	while ($r = pg_fetch_assoc($query)) {
		$ctfMaps[$r['id']] = $r;
		$query2 = pg_query("SELECT COUNT(*) FROM main.ctf_map_layout WHERE map={$r['id']} AND spawn_here");
		if (!$query2) {
			print "<b>DATABASE ERROR</b>";
			exit(1);
		}
		list($ctfMaps[$r['id']]['players']) = pg_fetch_array($query2);
		pg_free_result($query2);
	}
	pg_free_result($query);

	pg_close($cnx);
}

if (!is_array($errors)) {
	$errors = array();
}

$params = array(
	array(
		'id'		=> 'initialcash',
		'title'		=> 'Initial amount of money',
		'notes'		=> '0 for default (20,000)'
	),
	array(
		'id'		=> 'v2time',
		'title'		=> 'Time for victory',
		'notes'		=> 'Number of hours an alliance must hold the targets to win'
	),
	array(
		'id'		=> 'v2grace',
		'title'		=> 'Grace period',
		'notes'		=> 'Number of hours an alliance has to retake the targets. Set to 0 to disable.'
	),
	array(
		'id'		=> 'v2points',
		'title'		=> 'Points per victory',
		'notes'		=> 'Number of points an alliance gains at each victory. Must be > 0 and < 100'
	)
);

?>
<h2>Step 3 / 6 : parameters</h2>
<script language="JavaScript"><!--

var games = [];
<?php

foreach ($ctfMaps as $id => $map) {
	print "games[$id] = {\n\tdescription: '" . addslashes($map['description'])
		. "',\n\tsize: '{$map['width']}x{$map['height']}',\n\t"
		. "alliances: {$map['alliances']},\n\tplayers: {$map['players']}\n};\n";
}

?>

function showDetails(selector) {
	var sel = selector.options[selector.selectedIndex].value;
	var str;

	if (sel == '') {
		str = "&nbsp;";
	} else {
		str = "<h4>Map information</h4><p><b>Players:</b> " + games[sel].players
			+ " - <b>Teams:</b> " + games[sel].alliances
			+ " - <b>Size:</b> " + games[sel].size;
		if (games[sel].description != '') {
			str += '<br/><b>Description:</b> ' + games[sel].description;
		}
		str += '</p>';
	}
	document.getElementById('mapinfo').innerHTML = str;
}

// --></script>
<p>Define the new game's parameters here.</p>
<form method="POST" action="?">
 <div id="mapinfo" style="float:right;width: 50%;padding: 0px 10px">&nbsp;</div>
 <table border="1" width="45%">
  <tr>
   <th style='align:right<?php if (in_array('map', $errors)) { print ";color:red"; } ?>'>
    Map to use:
   </th>
   <td colspan="2">
    <select name="map" onChange="showDetails(this)">
     <option value="">-- Select --</option>
<?php

foreach ($ctfMaps as $id => $map) {
	print "     <option value='$id'";
	if ($_SESSION['lw_new_game']['ctfmap'] == $id) {
		print " selected='selected'";
	}
	print ">" . htmlentities($map['name']) . "</option>\n";
}

?>
    </select>
   </td>
  </tr>
<?php

foreach ($params as $p) {
	print "<tr><th style='align:right";
	if (in_array($p['id'], $errors)) {
		print ";color:red";
	}
	print "'>" . htmlentities($p['title']) . " : </th><td>"
		. "<input type='text' name='p_{$p['id']}' size='10' maxlength='10' value='"
		. $_SESSION['lw_new_game']['ctfparams'][$p['id']] . "' /></td><td><i>"
		. htmlentities($p['notes']) . "</i></td></tr>";
}

?>
 </table>
 <p>
  <input type="submit" name="back" value="<< Back" />
  <input type="submit" name="go" value="Proceed >>" />
 </p>
</form>
