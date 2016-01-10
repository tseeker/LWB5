<?php
if ($args['doit']) {
?>

<form action="https://www.paypal.com/cgi-bin/webscr" method="post" name="paypalform"><div>
 <input type="hidden" name="cmd" value="_xclick" />
 <input type="hidden" name="business" value="tseeker@legacyworlds.com" />
 <input type="hidden" name="item_name" value="Contribution to LegacyWorlds" />
 <input type="hidden" name="item_number" value="<?=$args['pid']?>" />
 <input type="hidden" name="no_shipping" value="1" />
 <input type="hidden" name="no_note" value="1" />
 <input type="hidden" name="currency_code" value="EUR" />
 <input type="hidden" name="tax" value="0" />
 <input type="hidden" name="no_shipping" value="1" />
 <input type="hidden" name="lc" value="GB" />
 <input type="hidden" name="return" value="http://www.legacyworlds.com/index.php/main/donate" />
 <input type="hidden" name="cancel_return" value="http://www.legacyworlds.com/index.php/main/donate" />
 <h2>Please wait ...</h2>
 <p>
  You are being redirected to the PayPal website. If nothing happens after about 10 seconds, please click the button below:<br/>
  <input type="image" src="http://images.paypal.com/images/x-click-but04.gif" border="0" name="submit" alt="Make payments with PayPal - it's fast, free and secure!" />
 </p>
</div></form>
<script type="text/javascript">
 document.paypalform.submit();
</script>
<?php
	return;
}

include("menu.en.inc");
$this->title = "Donate";
$this->addStylesheet('text');
$this->startContents();
?>

<h1>Donating to Legacy Worlds</h1>

<h2 style="margin:5px 0px 0px 10px">Why donate?</h2>
<p>
 As you know, the current version is a Beta; therefore we don't want to force people to pay as we feel it
 would be unethical. There are <a href='<?=makeLink('contrib', 'main')?>'>other ways</a> you can contribute...
</p>
<p>
 However, if you really like Legacy Worlds, you are encouraged to contribute a little money. This money will
 be used to pay for the hosting and server.
</p>
<h3>Please read this if you want to make a donation:</h3>
<ul>
 <li>Donations won't give you an in-game advantage.</li>
 <li>You will not get refunds.</li>
 <li>The amount you donate is free for you to determine.</li>
 <li>One euro donated will earn you to 10,000 <a href='<?=makeLink('contrib', 'main')?>'>contribution credits</a>.</li>
 <li>Donations are made through PayPal; we, at LegacyWorlds, only receive the money, but have
 no access to your personal data (credit card number, etc...) in any case.</li>
</ul>
<p>
 If you agree with this, you can proceed to <a href='?doit=1'>making a donation</a>.
</p>

<h2>Current status</h2>
<p>
 <?php
if ($args['selfContrib'] == 0) {
	echo "You have not contributed to LegacyWorlds yet.";
} else {
	echo "You have contributed <b>&euro;" . number_format($args['selfContrib']) . "</b> to LegacyWorlds! Thanks!";
}
?></p><p>
 <?php
if ($args['totalContrib'] == 0) {
	echo "No contributions to LegacyWorlds have been made yet.";
} else {
	echo "A total amount of <b>&euro;" . number_format($args['totalContrib']) . "</b> has been contributed to LegacyWorlds (";
	if ($args['totalMonth'] == 0) {
		echo "no contributions for the past 30 days";
	} else {
		echo "<b>&euro;" . number_format($args['totalMonth']) . "</b> in the past 30 days";
	}
	echo ").";
}
?></p>
<p>
 <b>NOTE:</b> if you just made a contribution, it may not have been accounted for yet.
</p>

<h2>History</h2>
<?php
if (count($args['hist']) == 0) {
	echo "<p>No donations have been accounted for at this time.</p>";
} else {
?>
<table style="width:90%;margin: 0% 4%">
 <tr>
  <th style="text-align: left">Date &amp; time</th>
  <th style="text-align: left">Amount</th>
 </tr>
<?php
	foreach ($args['hist'] as $d) {
		echo "<tr><td>{$d['time']}</td><td>&euro;{$d['amount']}</td></tr>\n";
	}
	echo "</table>\n";
}
$this->endContents();
?>
