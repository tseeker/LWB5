<?php

if	($args === false)
{
?>
<h1>You Evil, Evil Man</h1>
<p>
 You already have a planet, or many of these. Now begone!
</p>
<?php
}
elseif	($args['ok'])
{
?>
<h1>New Planet Assigned!</h1>
<p>
 Your new planet, <a href='planet?id=<?=$args['id']?>'><?=utf8entities($args['name'])?></a>, has been assigned to your empire!<br/>
 <br/>
 <a href="planets">View the Planet List</a>
</p>
<?php
}
else
{
	echo "<h1>Get a New Planet</h1>";
	echo "<form method='post' action='?'><p>";
	echo "Please type the name of your new planet in the field below, then click OK to get the planet.<br/><br/>";
	if	($args['err'])
	{
		echo "<span id='plerror'>";
		switch ($args['err']) :
			case 1: echo "This planet name is too long (maximum 15 characters)"; break;
			case 2: echo "This planet name is incorrect (letters, numbers, spaces and _.@-+'/ only)"; break;
			case 3: echo "Spaces are not allowed at the beginning or at the end of the planet's name"; break;
			case 4: echo "Multiple spaces are not allowed"; break;
			case 5: echo "This planet name is too short (minimum 2 characters)"; break;
			case 6: echo "A planet by that name already exists"; break;
		endswitch;
		echo ".</span><br/>";
	}
	echo "Name of your new planet: <input type='text' name='name' value=\"".utf8entities($args['name'],ENT_QUOTES)."\" size='20' maxlength='15' /> ";
	echo "<input type='submit' value='OK' /><br/><br/>";
	echo "Please note that getting a new planet will disband all of your fleets and place you under protection from the Peacekeepers.";
	echo "</p></form>";
}
