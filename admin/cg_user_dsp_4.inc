<?php

$speeds = array(
	""	=> "-- select speed --",
	"1"	=> "Normal",
	"2"	=> "2x",
	"3"	=> "3x",
	"4"	=> "4x"
);

?>
<h2>Step 4 / 6 : ticks configuration</h2>
<p>Please indicate the speed of the game and the day the game will begin on.</p>
<form method="GET" action="?">
 <p>
  Game speed:
  <select name="gs">
<?php

foreach ($speeds as $val => $disp) {
	print "<option value='$val'";
	if ($_SESSION['lw_new_game']['speed'] == $val) {
		print " selected='selected'";
	}
	print ">$disp</option>\n";
}

?>
  </select> (4x games = bad for the server!)
 </p>
 <p>
  First tick on (YYYY-MM-DD):
  <input type="text" name="ft_y" size="4" maxlength="4" value="<?=htmlentities($_SESSION['lw_new_game']['ft_y'], ENT_QUOTES)?>" /> -
  <input type="text" name="ft_m" size="2" maxlength="2" value="<?=htmlentities($_SESSION['lw_new_game']['ft_m'], ENT_QUOTES)?>" /> -
  <input type="text" name="ft_d" size="2" maxlength="2" value="<?=htmlentities($_SESSION['lw_new_game']['ft_d'], ENT_QUOTES)?>" /><br/>
<?php

if ($ftError) {
	print "<span style='color:red'>Invalid date! Must be in the future.</span>";
} else {
	print "Ticks will start on the specified day, at the first hour tick after 7:00 AM";
}

?>
 </p>
 <p>
  Shift ticks by <input type="text" name="st" size="3" maxlength="3" value="<?=(int) $_SESSION['lw_new_game']['shift_ticks']?>" /> minutes
 </p>
 <p>
  <input type="submit" name="back" value="<< Back" />
  <input type="submit" name="go" value="Proceed >>" />
 </p>
</form>

