<? if ($_SESSION['authok']) : ?>
 <a class="mbutton" id="b0" href="<?= makeLink('index', 'main') ?>" title="Manage your account">My account</a>
 <a class="mbutton" id="b1" href="<?= makeLink('contrib', 'main') ?>" title="Contributions">Contributions</a>
<? else : ?>
 <a class="mbutton" id="b0" href="<?= makeLink('create', 'main') ?>" title="Register">Create an account</a>
 <a class="mbutton" id="b1" href="<?= makeLink('about', 'main') ?>" title="About">About this game</a>
<? endif; ?>
<a class="mbutton" id="b2" href="<?= makeLink('screenshots', 'main') ?>" title="Screenshots">Screenshots</a>
<a class="mbutton" id="b3" href="<?= makeLink('manual', 'main') ?>" title="Manual">Manual</a>
<a class="mbutton" id="b4" href="<?= makeLink('rankings', 'main') ?>" title="Rankings">Rankings</a>
<a class="mbutton" id="b5" href="<?= makeLink('links', 'main') ?>" title="Links">Links</a>
<a class="mbutton" id="b6" href="<?= makeLink('credits', 'main') ?>" title="Credits">Credits</a>
