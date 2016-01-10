<h2>Step 1 / 6 : game type</h2>
<p>Please select the type of game you want to create from the form below.</p>
<form method="GET" action="?">
 <p>
  <input type="radio" value="m" name="gt" id="gtm" <?=$_SESSION['lw_new_game']['game_type'] == 'm' ? "checked='checked'" : ""?>/> <label for="gtm"><b>Match</b></label><br/>
  <input type="radio" value="r" name="gt" id="gtr" <?=$_SESSION['lw_new_game']['game_type'] == 'r' ? "checked='checked'" : ""?>/> <label for="gtr"><b>Round</b></label><br/>
  <input type="radio" value="c" name="gt" id="gtc" <?=$_SESSION['lw_new_game']['game_type'] == 'c' ? "checked='checked'" : ""?>/> <label for="gtc"><b>Kings of the Hill</b></label>
 </p>
 <p>
  <input type="submit" value="Proceed >>" />
 </p>
</form>
