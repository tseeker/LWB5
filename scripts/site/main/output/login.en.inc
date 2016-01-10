<?php
include("menu.en.inc");
$this->addStylesheet('text');
$this->startContents();
$this->title = "Please log in";
if ($args['error']) {
?>
 <h1>Invalid credentials</h1>
 <p>The username or password you entered are invalid.</p>
 <p>Please try typing them again.</p>
<?php
} else {
?>
 <h1>Please log in</h1>
 <p>The page you are trying to view requires you to be a registered LegacyWorlds player.</p>
 <p>Please type in your username and password into the login form at the top right corner of this page.</p>
<?php
}
$this->endContents();
?>
