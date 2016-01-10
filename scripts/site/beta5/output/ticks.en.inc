<table>
 <tr>
  <td style="width:95%">
<?php

$now = time();
$dt = array($now);
foreach	($args as $tid => $td)
{
	if	(!$td['game'])
		continue;
	echo "<h1>" . utf8entities($td['name']) . "</h1>";
	echo "<p>" . utf8entities($td['description']) . "</p>";
	echo "<div id='tick$tid'>&nbsp;</div>";
	echo "<p>&nbsp;</p>";
	array_push($dt, "$tid#" . $td['first'] . "#" . $td['interval'] . "#" . $td['last']);
}

?>
  </td>
  <td style="width:5%;vertical-align:top"><a href="manual?p=ticks">Help</a></td>
 </tr>
</table>
<div id='tickinit' style="display: none; visibility: hidden;"><?=join('#',$dt)?></div>
