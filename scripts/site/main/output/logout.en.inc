<?
include("menu.en.inc");
$this->addStylesheet('text');
$this->title = "Logged out";
$this->startContents();
?>
<h1>Logged out</h1>
<p>
 You are now logged out of Legacy Worlds.
</p>
<p>
 Thanks for playing!
</p>
<p>
 In the meantime, feel free to visit our <a href="<?=makeLink('about')?>#other">other games.</a>
</p>
<p>
 See you soon!
</p>
<? $this->endContents(); ?>
