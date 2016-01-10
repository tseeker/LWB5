<?
include("menu.en.inc");
$this->title = "Registration required";
$this->addStylesheet('text');
$this->startContents();
?>
<h1><?=$args['name']?></h1>
<p>
 You are not registered to this game.
</p>
<p>
 Before you can access the game, you have to <a href='<?=makeLink('play', 'main')?>?g=<?=$args['id']?>'>register</a>.
</p>
<? $this->endContents(); ?>
