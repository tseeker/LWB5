<h2>Step 2 / 6 : game description</h2>
<p>Please type in the name and description of the new game.</p>
<form method="POST" action="?">
 <p>
  <b>Game name:</b> <input type="text" name="gn" size="31" maxlength="30" value="<?=htmlentities($_SESSION['lw_new_game']['name'], ENT_QUOTES)?>" /><?php

switch ($err1) :
	case 1: print " <span style='color:red;font-weigth:bold'>Too short!</span>\n"; break;
	case 2: print " <span style='color:red;font-weigth:bold'>Invalid characters!</span>\n"; break;
endswitch;

?><br/>
  This is the name that will appear in menus and lists.
 </p>
 <p>
  <b>Description:</b> <input type="text" name="gd" size="50" maxlength="200" value="<?=htmlentities($_SESSION['lw_new_game']['descr'], ENT_QUOTES)?>" /><?php

switch ($err2) :
	case 1: print " <span style='color:red;font-weigth:bold'>Too short!</span>\n"; break;
	case 2: print " <span style='color:red;font-weigth:bold'>Invalid characters!</span>\n"; break;
endswitch;

?><br/>
  This is the description that will appear next to the name on people's account pages.
 </p>
 <p>
  <input type="submit" name="back" value="<< Back" />
  <input type="submit" name="go" value="Proceed >>" />
 </p>
</form>
