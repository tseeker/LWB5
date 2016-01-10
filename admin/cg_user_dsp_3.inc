<?php

if ($_SESSION['lw_new_game']['game_type'] == 'c') {
	include('cg_user_dsp_3ctf.inc');
	return;
}

if (!is_array($errors)) {
	$errors = array();
}

$params = array(
	array(
		'id'		=> 'maxplayers',
		'title'		=> 'Maximum amount of players',
		'notes'		=> '0 for unlimited, -1 for "1/system"'
	),
	array(
		'id'		=> 'minsystems',
		'title'		=> 'Minimum amount of free systems',
		'notes'		=> 'Also influences the initial universe'
	),
	array(
		'id'		=> 'maxsystems',
		'title'		=> 'Maximum amount of systems',
		'notes'		=> '0 for unlimited'
	),
	array(
		'id'		=> 'partialtechs',
		'title'		=> 'Partial tech graph',
		'notes'		=> '0 for complete tech graph, 1 for partial'
	),
	array(
		'id'		=> 'initialcash',
		'title'		=> 'Initial amount of money',
		'notes'		=> '0 for default (20,000)'
	),
	array(
		'id'		=> 'zonesize',
		'title'		=> 'Universe generator: zone size',
		'notes'		=> '0 = default (5x5); 1 for 3x3, 2 for 5x5, etc...'
	),
	array(
		'id'		=> 'nebulaprob',
		'title'		=> 'Probability of a nebula',
		'notes'		=> '0 .. 20, -1 = default'
	),
	array(
		'id'		=> 'norealloc',
		'title'		=> 'Prevent planet reallocation',
		'notes'		=> '0 to allow players to get new planets'
	),
	array(
		'id'		=> 'novacation',
		'title'		=> 'Disable vacation mode',
		'notes'		=> '0 = normal / 1 = disable'
	),
	array(
		'id'		=> 'victory',
		'title'		=> 'Victory conditions',
		'notes'		=> '0 = none / 1 = alliance holds 75% of the planets for 1 week'
	),
	array(
		'id'		=> 'lockalliances',
		'title'		=> 'Use predefined alliances',
		'notes'		=> 'Disabled if smaller than 2; maximum value = 8'
	),
	array(
		'id'		=> 'alliancecap',
		'title'		=> 'Alliance capping %',
		'notes'		=> '% of players; 0 to disable (no effect on predefined alliances)'
	),
	array(
		'id'		=> 'prot_after',
		'title'		=> 'Protection after',
		'notes'		=> 'Day ticks. Only applies to rounds.'
	),
	array(
		'id'		=> 'prot_duration',
		'title'		=> 'Protection duration',
		'notes'		=> 'Day ticks. Only applies to rounds.'
	)
);

?>
<h2>Step 3 / 6 : parameters</h2>
<p>Define the new game's parameters here.</p>
<form method="POST" action="?">
 <table border="1">
<?php

foreach ($params as $p) {
	print "<tr><th style='align:right";
	if (in_array($p['id'], $errors)) {
		print ";color:red";
	}
	print "'>" . htmlentities($p['title']) . " : </th><td>"
		. "<input type='text' name='p_{$p['id']}' size='10' maxlength='10' value='"
		. $_SESSION['lw_new_game']['params'][$p['id']] . "' /></td><td><i>"
		. htmlentities($p['notes']) . "</i></td></tr>";
}

?>
  <tr><td colspan="3"><input type="submit" name="gtdef" value="Game type defaults" style="width: 100%" /></td></tr>
 </table>
 <p>
  <input type="submit" name="back" value="<< Back" />
  <input type="submit" name="go" value="Proceed >>" />
 </p>
</form>

