<h2><?=$data['id'] ? "Edit" : "Add"?> link</h2>
<form action="?" method="POST">
 <input type="hidden" name="c" value="lk" />
 <input type="hidden" name="ac" value="<?=$data['id'] ? 10 : 1?>" />
 <input type="hidden" name="r" value="1" />
<?php
if ($data['id']) {
?>
 <input type="hidden" name="lid" value="<?=$data['id']?>" />
<?php
} else {
?>
 <input type="hidden" name="cid" value="<?=$data['cid']?>" />
<?php
}
?>
 <table style="width:80%;padding:0px 0px 0px 30px">
  <tr>
   <td style="font-weight:bold;padding:0px 10px 0px 0px">Name:</td>
   <td><input type="text" name="title" size="40" value="<?=utf8entities($data['title'])?>" /></td>
  </tr>
<?php
if ($data['error'] && $data['error'] < 3) {
	echo "<tr><td>&nbsp;</td><td><b>";
	switch ($data['error']) :
		case 1: echo "This title is too short"; break;
		case 2: echo "This title is too long"; break;
	endswitch;
	echo "</b></td></tr>\n";
}
?>
  <tr>
   <td style="font-weight:bold;padding:0px 10px 0px 0px">URL:</td>
   <td><input type="text" name="url" size="40" value="<?=utf8entities($data['url'])?>" /></td>
  </tr>
<?php
if ($data['error'] >= 3 && $data['error'] <= 5) {
	echo "<tr><td>&nbsp;</td><td><b>";
	switch ($data['error']) :
		case 3: echo "This URL is invalid"; break;
		case 4: echo "The server could not be found"; break;
		case 5: echo "The URL is already in the DB"; break;
	endswitch;
	echo "</b></td></tr>\n";
}
?>
  <tr>
   <td style="vertical-align:top;font-weight:bold;padding:0px 10px 0px 0px">Description:</td>
   <td><textarea name="desc" rows="5" cols="40"><?=utf8entities($data['desc'])?></textarea></td>
  </tr>
<?php
if ($data['error'] == 6) {
	echo "<tr><td>&nbsp;</td><td><b>This description is too short.</b></td></tr>\n";
}
?>
  <tr>
   <td style="text-align:right;padding:0px 5px 0px 0px">
    <input type="submit" value="<?=$data['id'] ? "Modify" : "Add"?> link" style="color:white;border-color:white;background-color:green" />
   </td>
   <td style="padding:0px 0px 0px 5px">
    <input type="submit" name="cancel" value="Cancel" style="color:white;border-color:white;background-color:red" />
  </tr>
 </table>

</form>
