   <div style="margin:1px 1px 30px 1px;padding:0;width:100%">
<?php

$manual = handler::$h->lib->call('getStructure', 'en');
$txt = array('Top', 'Up', 'Previous', 'Next');
if (!is_null(handler::$h->page)) {
	$pid = handler::$h->page['id'];
	$navLinks = handler::$h->lib->call('getNavLinks', $pid);
	for ($i=0;$i<4;$i++) {
		$t = is_null($navLinks[$i]) ? "" : "<a href='?p={$navLinks[$i]}'>";
		$t .= $txt[$i] . (is_null($navLinks[$i]) ? "" : "</a>");
		$navLinks[$i] = $t;
	}
} else {
	$pid = null;
	$navLinks = $txt;
}

	$size = 15 + prefs::get('main/font_size', 2) * 3;
?>
     <form action="?" method="POST">
      <table style="width:98%;margin:0% 0% 0% 1%">
       <tr>
        <td style="width:33%">&nbsp;</td>
	<td style="width:34%;text-align:center"><?=$navLinks[0]?></td>
        <td style="width:33%">&nbsp;</td>
       </tr>
       <tr>
	<td><?=$navLinks[2]?></td>
	<td style="text-align:center"><?=$navLinks[1]?></td>
	<td style="text-align:right"><?=$navLinks[3]?></td>
       </tr>
       <tr><td colspan="3">&nbsp;</td></tr>
       <tr>
        <th style="text-align:right"><input type="submit" value="Search:" /></th>
	<td colspan='2'><input type="text" name="ss" value="<?=preg_replace('/"/', '&quot;', handler::$h->searchText)?>" style="width:90%" /></td>
       </tr>
       <tr><td colspan="3">&nbsp;</td></tr>
       <tr><td colspan="3"><h3 style="margin:0;padding:0">Contents</h3></td></tr>
      </table>
      <div id="manual">
<?

function dumpManualStructure(&$sections, $depth) {
	$br = false;
	foreach ($sections as $id => $sd) {
		if ($br) {
			echo "<br/>";
		} else {
			$br = true;
		}
		if ($depth > 0) {
			echo str_repeat('&nbsp;', $depth * 2);
		}
		echo "       <a href='?p={$sd['name']}'>" . utf8entities($sd['title']) . "</a>";
		if (count($sd['subs'])) {
			echo "<br/>";
			dumpManualStructure($sd['subs'], $depth + 1);
		}
	}
}

dumpManualStructure($manual, 0);

?>
      </div>
     </form>
    </div>
