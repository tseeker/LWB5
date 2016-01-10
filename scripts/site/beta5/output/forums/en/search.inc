<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu('s', $args['cats']); ?>
  </td>
  <td>
   <h1>Search the Forums</h1>
   <form action="?" method="post">
    <input type="hidden" name="cmd" value="s" />
    <table cellspacing="0" cellpadding="0" class="fsrc">
     <tr><td colspan="2">&nbsp;</td></tr>
     <tr>
      <th>Text:</th>
      <td><input <?=tooltip('Use this text field to type in the string you wish to search for in the forums')?> type="text" name="stxt" value="<?=utf8entities($args['sparm']['text'], ENT_QUOTES)?>" size='40' /></td>
     </tr>
     <tr><td>&nbsp;</td><td>Note: use the '*' character for partial matches</td></tr>
     <tr><td colspan="2">&nbsp;</td></tr>
     <tr>
      <th>Search in:</th>
      <td>
       <input <?=tooltip('Use this radio button to search in post titles only')?> type="radio" name="sin" value="0"<?=$args['sparm']['whole']?'':' checked="checked"'?> id="sin0" /> <label for="sin0">Post titles</label>
       <input <?=tooltip('Use this radio button to search in whole posts')?> type="radio" name="sin" value="1"<?=$args['sparm']['whole']?' checked="checked"':''?> id="sin1" /> <label for="sin1">Whole posts</label>
      </td>
     </tr>
     <tr>
      <th>Forum:</th>
      <td <?=tooltip('Use this drop down list to select in which forums to search')?>  ><select name="sif"><option value="">All Available Forums</option><?

foreach	($args['cats'] as $cid => $cat)
{
	$ctype = $cat['type'] . "#";
	foreach	($cat['forums'] as $f)
	{
		$id = $ctype . $f['id'];
		echo "<option value='$id'" . ($args['sparm']['fid']==$id ? " selected='selected'" : "") . ">";
		echo utf8entities($cat['title'] . " - " . $f['title']) . "</option>";
	}
}

?></select></td>
     </tr>
     <tr><td colspan="2">&nbsp;</td></tr>
     <tr>
      <th>Sort by:</th>
      <td <?=tooltip('Use this drop down list to select a sort criteria for the retrieved posts')?>  ><select name="sst">
<?php

$st = array('Post Time', 'Post Title');
for	($i=0;$i<count($st);$i++)
	echo "<option value='$i'" . ($args['sparm']['stype'] == $i ? " selected='selected'" : "") . ">" . $st[$i] . "</option>";

?>
      </select></td>
     </tr>
     <tr>
      <td>&nbsp;</td>
      <td>
       <input <?=tooltip('Use this radio button to use an ascending ordering of the retrieved posts')?>  type="radio" name="sor" value="0"<?=$args['sparm']['sord']?'':' checked="checked"'?> id="sor0" /> <label for="sor0">Ascending</label>
       <input <?=tooltip('Use this radio button to use a descending ordering of the retrieved posts')?>  type="radio" name="sor" value="1"<?=$args['sparm']['sord']?' checked="checked"':''?> id="sor1" /> <label for="sor1">Descending</label>
      </td>
     </tr>
     <tr>
      <th>Display results:</th>
      <td>
       <input <?=tooltip('Use this radio button to display whole posts')?>   type="radio" name="srp" value="0"<?=$args['sparm']['resd']?'':' checked="checked"'?> id="srp0" /> <label for="srp0">As posts</label>
       <input <?=tooltip('Use this radio button to display only the topics of the retrieved posts')?>  type="radio" name="srp" value="1"<?=$args['sparm']['resd']?' checked="checked"':''?> id="srp1" /> <label for="srp1">As topics</label>
      </td>
     </tr>
     <tr><td colspan="2">&nbsp;</td></tr>
     <tr><td>&nbsp;</td><td><input <?=tooltip('Click here to launch the search')?>   type="submit" name="s" value="Search" /></td>
     </tr>
    </table>
   </form>
  </td>
 </tr>
</table>
