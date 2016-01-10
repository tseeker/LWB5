<?
include("menu.en.inc");
$this->title = "Disclaimer";
$this->addStylesheet('text');
$this->startContents();
?>
<h1>Terms of use</h1>
<p>Hey! Welcome to Legacy Worlds.</p>
<p>
 The email address you supply when registering to Legacy Worlds is for
 internal use only; it will not be given to any third party under any
 circumstances, and will only be used to send the first time authentication
 code, lost passwords and very occasional informations.
</p>
<p>
 Please do not use a password for your account that you use for bank, email,
 or other accounts. Although every attempt is used to keep the data secure
 there is always the possibility of hackers, etc... Also, don't forget that
 we can see the information!
</p>
<p>
 <b><em>ONE ACCOUNT PER PERSON</em></b>: although this is just a test,
 people need to play fairly or neither the fun aspect nor the balancing
 aspect of the game can be measured.
</p>
<p>
 You will find more informations regarding Legacy World's terms of use in
 the <a href="<?=makeLink('manual')?>?p=general_rules">manual</a>.
</p>
<?=$args?"<p>&nbsp;</p><p><a href='?'>Continue to account creation</a></p>":""?>
<? $this->endContents(); ?>
