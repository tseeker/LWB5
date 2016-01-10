<h2>Request a kick</h2>
<p>
 Please indicate the name of the player to be kicked as well as the reason why the player should be kicked in the form below.<br/>
 The player will only be kicked after another administrator confirms it.
</p>
<?php

if ($data['error'] != 0) {
	echo "<p style='font-weight:bold;color:red'>";
	switch($data['error']) :
	case 1:
		echo "Account not found.";
		break;
	case 2:
		echo "Please specify a reason (&gt;10 characters).";
		break;
	case 3:
		echo "This player is already on the list of pending kicks.";
		break;
	default:
		echo "Unknown error.";
		break;
	endswitch;
	echo "</p>";
}

?>
<form method="POST" action="?">
 <input type="hidden" name="c" value="k" />
 <input type="hidden" name="kr" value="1" />
 <table>
  <tr>
   <th style="text-align:left;width:200px;padding:0px 0px 0px 40px">Player to kick:</th>
   <td><input style="width:50%" maxlength="15" type="text" name="p" value="<?=preg_replace('/"/', '&quot;', $data['name'])?>" /></td>
  </tr>
  <tr>
   <th style="text-align:left;padding:0px 0px 0px 40px">Reason:</th>
   <td><textarea style="width:50%" name="r"><?=utf8entities($data['reason'])?></textarea></td>
  </tr>
  <tr><td colspan="2">&nbsp;</td></tr>
  <tr>
   <td>&nbsp;</td>
   <td>
    <input style="font-weight:bold;border-color:white;border-style:solid;border-width:1px;color:white;background-color:red" type="submit" value="Request kick" />
    <input style="border-color:white;border-style:solid;border-width:1px;color:white;background-color:green" type="submit" name="cancel" value="Cancel" />
   </td>
  </tr>
 </table>
</form>
