<html>
 <head>
  <meta http-equiv='Content-Type' content='text/html; charset=utf-8' />
  <title>Legacy Worlds - Server under maintenance</title>
  <style type="text/css">
<?php

$stylesheets = array("main");
foreach ($stylesheets as $name) {
	$fName = config::$main['staticdir'] . "/main/css/$name.css";
	$file = fopen($fName, "r");
	$css = fread($file, filesize($fName));
	fclose($file);
	print preg_replace('/__STATICURL__/', config::$main['staticurl'], $css);
}

?>
#intframe {
	background-image: url(<?= config::$main['staticurl'] ?>/main/pics/mp-content.jpg);
}

#mframe {
	position: absolute;
	left: 15px;
	top: 264px;
	width: 921px;
	height: 279px;
	overflow: none;
	background-color: transparent;
}

#mframe h1 {
	width: 100%;
	font-size: 48pt;
	font-variant: small-caps;
	text-align: center;
}

#mframe p, #mframe b {
	color: #aaa;
	font-size: 16pt;
}

#mframe p {
	text-align: center;
	margin: 10px 0px;
}
  </style>
  <script type="text/javascript"><!--
window.setTimeout('location.href = location.href', 15000);
// --></script>
 </head>
 <body>
  <div id="extframe">
   <div class="internal" id="intframe">
    <a id="hbutton" href="<?= $_SERVER['SCRIPT_NAME'] ?>" title="LegacyWorlds"><span>Legacy Worlds</span></a>
    <div id="version">
     current version: <?= config::$main['v_game'] ?> -
     engine v<?= config::$main['v_engine'] ?> -
     revision <?= config::$main['v_rev'] ?>
    </div>
    <div id="mframe">
     <h1>Server under maintenance</h1>
     <p>
      <b>Reason:</b> <?=config::$main['maintenance']['reason']?><br/>
      <br/>
      It will be available again around
      <?=gmstrftime("%H:%M:%S on %m/%d/%Y", config::$main['maintenance']['until'])?>.<br/>
      Current server time: <?=gmstrftime("%H:%M:%S on %m/%d/%Y", time())?>.
     </p>
    </div>
   </div>
   <div class="internal" id="footer">
    Copyright (C) 2004-2008,
    <a title="DeepClone Development" href="http://www.deepclone.com">DeepClone development</a>
   </div>
  </div>
 </body>
</html>
