<?php

if (!is_array($allAdmins)) {
	require_once("config.inc");
	$cnx = __dbConnect();
	if (!$cnx) {
		print "<b>DATABASE CONNECTION ERROR</b>";
		exit(1);
	}
	$query = pg_query("SELECT id,name FROM main.account WHERE admin AND NOT name LIKE 'AI>%' ORDER BY name");
	if (!$query) {
		print "<b>DATABASE CONNECTION ERROR</b>";
		exit(1);
	}

	$allAdmins = array();
	while ($r = pg_fetch_row($query)) {
		$allAdmins[$r[0]] = $r[1];
	}

	pg_close($cnx);
}

?>
<h2>Step 5 / 6 : silent admins</h2>
<p>"Silent admins" are admins that will join the game for administrative purposes but won't play it. Please beware with this function, as a silent admin will no longer be able to join the game as a player.</p>
<form method="POST" action="?">
 <p>
<?php

foreach ($allAdmins as $id => $name) {
?>
  <input type="checkbox" name="sa[]" value="<?=$id?>" id="sa<?=$id?>" <?=(is_array($_SESSION['lw_new_game']['silent']) && in_array($id, $_SESSION['lw_new_game']['silent'])) ? "checked='checked'" : ""?> /> <label for="sa<?=$id?>"><?=htmlentities($name)?></label><br/>
<?
}

?>
 </p>
 <p>
  <input type="submit" name="back" value="<< Back" />
  <input type="submit" name="go" value="Proceed >>" />
 </p>
</form>
