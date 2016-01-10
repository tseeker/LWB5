<h2>Accept submission</h2>
<p>
 You are about to accept the following submission:
</p>
<ul>
 <li>Title: <b><?=utf8entities($data['sub']['title'])?></b></li>
 <li>URL: <b><?=utf8entities($data['sub']['url'])?></b></li>
<?php
if ($data['sub']['description'] != '') {
	echo " <li>Description: <b>" . utf8entities($data['sub']['description']) . "</b></li>\n";
}
?>
</ul>
<form action="?" method="GET">
 <input type="hidden" name="c" value="lks" />
 <input type="hidden" name="su" value="<?=utf8entities($data['sub']['url'])?>" />
 <input type="hidden" name="sid" value="<?=$data['sub']['submitted_by']?>" />
 <p>
  Select the category into which the link should be added:
  <select name="cid">
   <option value="">---------------------------------------</option>
<?php
foreach ($data['cats'] as $cat) {
	echo "<option value='{$cat['id']}'>" . utf8entities($cat['title']) . "</option>\n";
}
?>
  </select><br/>
  <input type="submit" value="Add link" style="color:white;border-color:white;background-color:green" />
  <input type="submit" name="cancel" value="Cancel" style="color:white;border-color:white;background-color:red" />
 </p>
</form>
