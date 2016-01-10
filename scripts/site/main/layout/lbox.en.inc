<? if ($_SESSION['authok']) : ?>
     Welcome, <b><?= utf8entities($_SESSION['login']) ?></b><br/>
<?	if ($__vacation) : ?>
     Vacation mode activated
<?	endif; ?>
     <br/>
     <a title="Preferences" href="<?= makeLink('settings', 'main') ?>">Preferences</a><br/>
     <a title="Log out" href="<?= makeLink('logout', 'main') ?>">Log out</a>
<? else : ?>
<?  if (input::$path == "main" && input::$page == "logout") : ?>
     <form method="post" action="<?= makeLink('index', 'main') ?>?userlogin=1"><div>
<?  else : ?>
     <form method="post" action="?userlogin=1"><div>
<?  endif; ?>
      Username: <input type="text" name="login" value='' size="15" maxlength="15" class='input' /><br/>
      Password: <input type="password" name="password" value='' size="15" maxlength="65" class='input' /><br/>
      <a title="Recover password" href="<?= makeLink('lostpass', 'main') ?>">Forgot your password?</a>
      <input type="submit" value='Log in' class='input' />
     </div></form>
<? endif; ?>
