<?
include('menu.en.inc');
$this->title = 'Contributions';
$this->addStylesheet('text');
$this->startContents();
?>
<h2>Help us keep Legacy Worlds alive!</h2>
<p>
 There are three ways you can help us at the moment.
</p>
<p>
 You can choose to <a href='<?=makeLink('donate', 'main')?>'>donate some money</a>; while we certainly don't expect
 to make a living out of it, the server costs quite a lot of money, and any amount you donate helps.
</p>
<p>
 You can help us with the new version by <a href='<?=makeLink('b6pp', 'main')?>'>rating planet pictures</a> for the
 new version of Legacy Worlds or commenting on the <a href='http://blog.legacyworlds.com' target='_blank'>Beta 6 blog</a>.
</p>
<p>
 Finally, if you report a bug that no-one knew about, you will be credited for it.
</p>
<h3>Your current status</h3>
<p>
 The figures below are an estimate of how much of the server's resources you used while playing the game along with
 an estimate of your contributions to Legacy Worlds (all players start with 9,000 in this field).
</p>
<p style="padding-left: 50px">Resources used: <b><?= number_format($args['resources_used']) ?></b></p>
<p style="padding-left: 50px">Contribution: <b><?= number_format($args['credits_obtained']) ?></b></p>
<p>
 Note: if the resources you used are higher than the estimate of your contributions, you will start getting an
 annoying message every time you log on to LegacyWorlds.
</p>
<? $this->endContents(); ?>
