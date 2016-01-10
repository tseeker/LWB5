<h2><?=$data['id'] ? "Edit" : "Add"?> category</h2>
<form action="?" method="POST">
 <input type="hidden" name="c" value="lk" />
 <input type="hidden" name="ac" value="<?=$data['id'] ? 2 : 0?>" />
 <input type="hidden" name="r" value="1" />
<?php
if ($data['id']) {
?>
 <input type="hidden" name="cid" value="<?=$data['id']?>" />
<?php
}
?>
 <table style="width:80%;padding:0px 0px 0px 30px">
  <tr>
   <td style="font-weight:bold;padding:0px 10px 0px 0px">Name:</td>
   <td><input type="text" name="title" size="40" value="<?=utf8entities($data['title'])?>" /></td>
  </tr>
<?php
if ($data['error'] && $data['error'] < 4) {
	echo "<tr><td>&nbsp;</td><td><b>";
	switch ($data['error']) :
		case 1: echo "This title is too short"; break;
		case 2: echo "This title is too long"; break;
		case 3: echo "There is already such a category"; break;
	endswitch;
	echo "</b></td></tr>\n";
}
?>
  <tr>
   <td style="vertical-align:top;font-weight:bold;padding:0px 10px 0px 0px">Description:</td>
   <td><textarea name="desc" rows="5" cols="40"><?=utf8entities($data['desc'])?></textarea></td>
  </tr>
<?php
if ($data['error'] == 4) {
	echo "<tr><td>&nbsp;</td><td><b>This description is too short.</b></td></tr>\n";
}
?>
  <tr>
   <td style="text-align:right;padding:0px 5px 0px 0px">
    <input type="submit" value="<?=$data['id'] ? "Modify" : "Add"?> category" style="color:white;border-color:white;background-color:green" />
   </td>
   <td style="padding:0px 0px 0px 5px">
    <input type="submit" name="cancel" value="Cancel" style="color:white;border-color:white;background-color:red" />
  </tr>
 </table>

</form>
