<h2>Pending kick requests</h2>
<table class="list">
 <tr>
  <th style="text-align:left;width:15%">Account name</th>
  <th style="text-align:left;width:15%">Requested by</th>
  <th style="text-align:left;width:15%">Date</th>
  <th style="text-align:left">Reason</th>
  <th style="text-align:left;width:15%">Actions</th>
 </tr>
<?php

if (empty($data['lists'][0])) {
	echo "<tr><td colspan='5' style='text-align:center'>No pending kick requests</td></tr>";
} else {
	foreach ($data['lists'][0] as $r) {
		$pn = handler::$h->accounts->call('getUser', $r['to_kick']);
		$an = handler::$h->accounts->call('getUser', $r['requested_by']);
		echo "<tr><td>" . utf8entities($pn['name']) . "</td><td>" . utf8entities($an['name'])
			. "</td><td>" . gmstrftime("%Y-%m-%d %H:%M:%S", $r['requested_at'])
			. "</td><td>" . utf8entities($r['reason']) . "</td><td>";
		if ($r['requested_by'] == $_SESSION['userid']) {
			echo "<a href='?c=kl&i={$r['id']}&a=0'>Cancel</a>";
		} else {
			echo "<a href='?c=kl&i={$r['id']}&a=0'>Reject</a> - <a href='?c=kl&i={$r['id']}&a=1'>Kick player</a>";
		}
		echo "</td></tr>\n";
	}
}

?>
</table>

<h2>Accepted kick requests</h2>
<table class="list">
 <tr>
  <th style="text-align:left;width:15%">Account name</th>
  <th style="text-align:left;width:15%">Requested by</th>
  <th style="text-align:left;width:15%">Date</th>
  <th style="text-align:left">Reason</th>
  <th style="text-align:left;width:15%">Validated by</th>
 </tr>
<?php

if (empty($data['lists'][1])) {
	echo "<tr><td colspan='5' style='text-align:center'>No accepted kick requests</td></tr>";
} else {
	foreach ($data['lists'][1] as $r) {
		$pn = handler::$h->accounts->call('getUser', $r['to_kick']);
		$an = handler::$h->accounts->call('getUser', $r['requested_by']);
		$vn = handler::$h->accounts->call('getUser', $r['examined_by']);
		echo "<tr><td>" . utf8entities($pn['name']) . "</td><td>" . utf8entities($an['name'])
			. "</td><td>" . gmstrftime("%Y-%m-%d %H:%M:%S", $r['requested_at'])
			. "</td><td>" . utf8entities($r['reason']) . "</td><td>"
			. utf8entities($vn['name']) . "</td></tr>\n";
	}
}

?>
</table>

<h2>Rejected kick requests</h2>
<table class="list">
 <tr>
  <th style="text-align:left;width:15%">Account name</th>
  <th style="text-align:left;width:15%">Requested by</th>
  <th style="text-align:left;width:15%">Date</th>
  <th style="text-align:left">Reason</th>
  <th style="text-align:left;width:15%">Rejected by</th>
 </tr>
<?php

if (empty($data['lists'][2])) {
	echo "<tr><td colspan='5' style='text-align:center'>No rejected requests</td></tr>";
} else {
	foreach ($data['lists'][2] as $r) {
		$pn = handler::$h->accounts->call('getUser', $r['to_kick']);
		$an = handler::$h->accounts->call('getUser', $r['requested_by']);
		$vn = handler::$h->accounts->call('getUser', $r['examined_by']);
		echo "<tr><td>" . utf8entities($pn['name']) . "</td><td>" . utf8entities($an['name'])
			. "</td><td>" . gmstrftime("%Y-%m-%d %H:%M:%S", $r['requested_at'])
			. "</td><td>" . utf8entities($r['reason']) . "</td><td>"
			. utf8entities($vn['name']) . "</td></tr>\n";
	}
}

?>
</table>
