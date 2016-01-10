<?php

function printOption($value, $text, $cVal) {
	return "<option value='$value'" . ($cVal === $value ? " selected='selected'" : "") . ">" . utf8entities($text) . "</option>";
}

function printPrompt($id) {
	$prompts = array(
		"You are about to send this planet a warning regarding its name.",
		"You are about to validate this planet name.",
		"You are about to reset this planet."
	);
	return " onclick='return confirm(\"{$prompts[$id]}\\nPlease confirm.\")'";
}

?>
<h2>Planet names</h2>
<form action="?" method="GET">
 <input type="hidden" name="c" value="p" />
 <table style="width:90%;margin: 0 4%">
  <tr>
   <td style="text-align:center;width:50%">List: <select name="m" onchange="this.form.submit()">
    <?=printOption("r", "Unmoderated", $data['mode'])?>
    <?=printOption("o", "Validated", $data['mode'])?>
    <?=printOption("p", "Pending", $data['mode'])?>
    <?=printOption("w", "Renamed after warning", $data['mode'])?>
   </select></td>
   <td style="text-align:center"><?php

if ($data['pages'] == 0) {
	echo "&nbsp;";
} else {
	echo "Page <select name='p' onchange='this.form.submit()'>";
	for ($i=0;$i<$data['pages'];$i++) {
		echo printOption($i, $i+1, $data['page']);
	}
	echo "</select> / {$data['pages']}";
}

?></td>
  </tr>
  <tr>
<?php

if (count($data['list'])) {
?>
   <td colspan='2'><table class='list'>
    <tr>
     <th style='text-align:left;width:33%'>Planet name</th>
     <th style='text-align:left;width:33%'>Owner</th>
     <th style='text-align:left;width:34%'>Actions</th>
    </tr>
<?php
	for ($i=0;$i<count($data['list']);$i++) {
		$planet = $data['list'][$i];
?>
    <tr>
     <td><a href="planet?id=<?=$planet['id']?>"><?=utf8entities($planet['name'])?></a></td>
     <td><?php
		if (is_null($planet['owner'])) {
			echo "(neutral)";
		} else {
			echo "<a href='message?a=c&ct=0&id={$planet['owner']}'>" . utf8entities($planet['oname']) . "</a>";
		}
?></td>
     <td>
      <a target='_blank' href="map?menu=p&ctr=<?=$planet['id']?>">Centre map</a><?php
		if ($data['mode'] == 'r' || $data['mode'] == 'o') {
			if (is_null($planet['owner'])) {
				echo "      - <a href='?c=p&m=r&p={$data['page']}&pc=r&id={$planet['id']}'" . printPrompt(2) . ">Reset</a>\n";
			} else {
				echo "      - <a href='?c=p&m=r&p={$data['page']}&pc=w&id={$planet['id']}'" . printPrompt(0) . ">Send warning</a>\n";
			}
			if ($data['mode'] == 'r') {
				echo "      - <a href='?c=p&m=r&p={$data['page']}&pc=v&id={$planet['id']}'" . printPrompt(1) . ">Validate</a>\n";
			}
		} elseif ($data['mode'] != 'p') {
			echo "      - <a href='?c=p&m=r&p={$data['page']}&pc=r&id={$planet['id']}'>Reset</a>\n";
			if ($data['mode'] == 'w') {
				echo "      - <a href='?c=p&m=r&p={$data['page']}&pc=v&id={$planet['id']}'" . printPrompt(1) . ">Validate</a>\n";
			}
		}
?>
     </td>
    </tr>
<?php
	}
?>
   </table></td>
<?php
} else {
	echo "   <td colspan='2' style='text-align:center'>No planets in this list.</td>\n";
}

?>
  </tr>
 </table>
</form>
