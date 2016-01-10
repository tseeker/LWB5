<?
include("menu.en.inc");
$this->title = "Banned";
$this->addStylesheet('text');
$this->startContents();
?>
<h1>Banned!</h1>
<p>
 You have been banned from the game.
</p>
<p>
 Don't even think about coming back.
</p>
<?

$q = dbQuery("SELECT reason FROM account WHERE id=" . tracking::$data['uid'] . " AND status='KICKED'");
list($reason) = dbFetchArray($q);

if ($reason != "") {
	echo "<p><b>Reason:</b> $reason</p>";
}

$this->endContents();
?>
