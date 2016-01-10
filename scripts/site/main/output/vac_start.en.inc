<?
include("menu.en.inc");
$this->title = "Enter vacation mode";
$this->addStylesheet("account");
$this->startContents();
?>
<div id="tabs">&nbsp;</div>
<div class="acctab">
 <h1>You are about to enter vacation mode</h1>
 <p>
  For the first 30h or so, the account will still be accessible normally and vacation mode rules will not apply in the games you are playing.
 </p>
 <p>
  After this delay, vacation mode will be enabled. You will no longer be able to play the games or register to new games, but you will still be
  able to access your private messages and the games' forums.
 </p>
 <p>
  Vacation mode will end either when you decide to exit vacation mode, or when you run out of vacation credits; whichever comes first.
 <p>
  Are you sure you want to do that?
 </p>
 <form method="post" action="?#acc-vacation"><div>
  <input type="hidden" name="svmc" value="1" />
  <input type="submit" value="Yes, start the countdown" class="input" />
  <input type="submit" name="cancel" value="No, don't activate vacation mode" class="input" />
 </div></form>
</div>
<? $this->endContents(); ?>
