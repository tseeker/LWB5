<?php

function displaySubmitLink() {
	if (is_array($_SESSION) && !is_null($_SESSION['userid'])) {
		echo "<p><a href='?sl=1'>Submit link</a> - <a href='?rbl=1'>Report broken link</a></p>";
	}
}

function displayLinks($categories) {
	echo "<h1>Sites related to Legacy Worlds</h1>";
	if (!count($categories)) {
		echo "<p>No sites are currently registered.</p>";
		displaySubmitLink();
		return;
	}

	$hasAccount = (is_array($_SESSION) && !is_null($_SESSION['userid']));
	for ($i=0;$i<count($categories);$i++) {
		echo "<p style='margin:10px 0px 5px 20px;text-align:justify'><b style='text-decoration: underline'>" . utf8entities($categories[$i]['title']) . "</b>";
		if (!is_null($categories[$i]['description'])) {
			echo "<br/>" . preg_replace('/\n/', '<br/>', utf8entities($categories[$i]['description']));
		}
		echo "</p>";

		echo "<ul style='margin:0px 30px 0px 10px'>";
		if (!count($categories[$i]['links'])) {
			echo "<li><i>No links in this category.</i></li>";
		} else {
			foreach ($categories[$i]['links'] as $l) {
				echo "<li><a href='{$l['url']}' title=\"" . utf8entities($l['title']) . "\">" . utf8entities($l['title']) . "</a>";
				if (!is_null($l['description'])) {
					echo "<br/>" . preg_replace('/\n/', '<br/>', utf8entities($l['description']));
				}
				echo "<br/>&nbsp;</li>";
			}
		}
		echo "</ul>";
	}

	displaySubmitLink();
}


function displayReportList($list) {
?>
<h1>Report a broken link</h1>
<form method='get' action='?'><div>
 <input type="hidden" name="rbl" value="1" />
 <p>
  Please select the link to report from the list below.<br/>
  <b>NOTE:</b> users who repeatedly report links as broken for no good reason will be banned from the game. If you made a mistake, please send an in-game private message to TSeeker or send an e-mail to <a href='mailto:staff@legacyworlds.com'>the staff</a>.<br/>
  <br/>
  <select name='id' class="input">
   <option value="">----------------------------------------</option>
<?php
	foreach ($list as $id => $text) {
		echo "   <option value='$id'>$text</option>\n";
	}
?>
  </select><br/>
  <br/>
  <input type="submit" value="Report selected link" class="input" />
  <input type="submit" name='cancel' value="Cancel" class="input" />
 </p>
</div></form>
<?php
}


function displayReportConfirm($link) {
?>
<h1>Report a broken link</h1>
<form method='get' action='?'><div>
 <input type="hidden" name="rbl" value="1" />
 <input type="hidden" name="id" value="<?=$link['id']?>" />
 <p>
  You are about to report the following link as being broken:<br/>
  <br/>
  <b>Link name:</b> <?=$link['long_title']?><br/>
  <b>URL:</b> <a href="<?=$link['url']?>"><?=utf8entities($link['url'])?></a><br/>
  <br/>
  <b>WARNING:</b> users who repeatedly report links as broken for no good reason will be banned from the game. If you make a mistake, please send an in-game private message to TSeeker or send an e-mail to <a href='mailto:staff@legacyworlds.com'>the staff</a>.<br/>
  <br/>
  <input type="submit" name="confirm" value="Confirm report" class="input" />
  <input type="submit" name='cancel' value="Cancel" class="input" />
 </p>
</div></form>
<?php
}


function displayReportDone($link) {
?>
<h1>Link reported</h1>
<p>
 Your report stating that the <b><?=$link['long_title']?></b> link is broken has been registered.<br/>
 The link will be removed after the staff verifies your report.<br/>
 <br/>
 <a href='?'>Back to the list</a>
</p>
<?php
}


function displaySubmitForm($data) {
?>
<h1>Submit a new link</h1>
<form method="post" action="?"><div>
 <input type="hidden" name="sl" value="2" />
 <p>
  Please fill in the form below to submit a new website to be added to the links page.<br/>
  <br/>
  <b>NOTE:</b> the links will be reviewed by the staff before they appear anywhere on the links page. Users who repeatedly submit unrelated websites (and especially commercial, pornographic or illegal sites) will be banned from the game.
 </p>
 <table style="margin:0px 0px 10px 30px;width:80%;min-width:200px;max-width:800px;border-style:none">
  <tr>
   <th style="width:30%;max-width:150px;text-align:left">Link title:</th>
   <td><input type="text" name="title" style="width:100%" value="<?=utf8entities($data['title'])?>" class="input"/></td>
  </tr>
<?php
	if ($data['err'] == 1 || $data['err'] == 2) {
		echo "<tr><td>&nbsp;</td><td style='color:red;font-weight:bold'>This title is too ";
		echo ($data['err'] == 1 ? "short" : "long");
		echo "</td></tr>\n";
	}
?>
  <tr>
   <th style="width:30%;max-width:150px;text-align:left">URL:</th>
   <td><input type="text" name="url" style="width:100%" value="<?=utf8entities($data['url'])?>" class="input"/></td>
  </tr>
<?php
	if ($data['err'] == 4) {
		echo "<tr><td>&nbsp;</td><td style='color:red;font-weight:bold'>This URL is invalid.</td></tr>\n";
	} elseif ($data['err'] == 5) {
		echo "<tr><td>&nbsp;</td><td style='color:red;font-weight:bold'>This server does not exist.</td></tr>\n";
	} elseif ($data['err'] == 6) {
		echo "<tr><td>&nbsp;</td><td style='color:red;font-weight:bold'>You have already submitted this URL.</td></tr>\n";
	}
?>
  <tr>
   <td style="width:30%;max-width:150px;vertical-align:top">Description <i>(optional)</i>:</td>
   <td><textarea class="input" name="desc" cols="auto" rows="5" style="width:100%"><?=utf8entities($data['desc'])?></textarea></td>
  </tr>
<?php
	if ($data['err'] == 3) {
		echo "<tr><td>&nbsp;</td><td style='color:red;font-weight:bold'>Please type in a longer description.</td></tr>\n";
	}
?>
  <tr>
   <td>&nbsp;</td>
   <td>
    <input type="submit" name="confirm" value="Submit this link" class="input" />
    <input type="submit" name='cancel' value="Cancel" class="input" />
   </td>
  </tr>
 </table>
</div></form>
<?php
}


function displaySubmitDone($link) {
?>
<h1>Link submitted</h1>
<p>
 The link to <b><?=utf8entities($link)?></b> has been submitted.<br/>
 It will be added after the staff verifies it.<br/>
 <br/>
 <a href='?'>Back to the list</a>
</p>
<?php
}


include("menu.en.inc");
$this->title = "Links";
$this->startContents();
switch($args['mode']) :
	case 1: displayReportList($args['data']); break;
	case 2: displayReportConfirm($args['data']); break;
	case 3: displayReportDone($args['data']); break;
	case 4: displaySubmitForm($args['data']); break;
	case 5: displaySubmitDone($args['data']); break;
	default: displayLinks($args['data']); break;
endswitch;
$this->endContents();


?>
