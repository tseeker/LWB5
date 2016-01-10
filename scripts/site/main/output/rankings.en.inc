<?
include("menu.en.inc");
$this->addStylesheet('rankings');
$this->title = "Rankings";
$this->startContents();
?>
<form action="?" method="get">
 <div style="text-align:center; margin: 0px 0px 10px 0px">
  <select class="input" name="g" onchange="this.form.submit();"><?

foreach ($args['games'] as $v => $t) {
	echo "   <option value='$v'";
	if ($v == $args['cGame'])
		echo " selected='selected'";
	echo ">" . utf8entities($t) . "&nbsp;&nbsp;&nbsp;</option>\n";
}

?></select>  / <select class="input" name="t" onchange="this.form.submit();"><?

foreach ($args['types'] as $t => $d) {
	echo "   <option value='$t'";
	if ($t == $args['cType'])
		echo " selected='selected'";
	echo ">" . utf8entities($d['name']) . "</option>\n";
}

?></select>
  <input class="input" type="submit" value="Display rankings" /></div>
 </form>
 <div style="text-align: justify; margin: 0px 5px 10px 5px">
  <?=utf8entities($args['types'][$args['cType']]['description'])?>
 </div>

 <table style="width:100%;border-style:none;">
  <tr class="chead">
   <th style="text-align:center;width: 10%;">Rank</th>
   <th style="text-align:left">Name</th>
   <th style="text-align:center;width: 20%">Points</th>
  </tr>
<?
foreach ($args['rankings'] as $data)
	echo "  <tr>\n   <td style='text-align:center;'><b>#"
		. $data['ranking'] . "</b></td>\n   <td>" . utf8entities($data['id'])
		. "</td>\n   <td style='text-align:center;'>"
		. number_format($data['points']) . "</td>\n  </tr>\n";
?>
 </table>
<? $this->endContents(); ?>
