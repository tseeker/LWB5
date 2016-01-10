  <title>Legacy Worlds<?= is_null($this->title) ? "" : " - {$this->title}" ?></title>
<? if (! is_null($this->cssRes) ) : ?>
  <link rel='stylesheet' type='text/css' href='<?= makeLink('css', 'main', 'css') ?>?id=<?= $this->cssRes ?>' />
<? endif; ?>
<? if (! is_null($this->jsRes) ) : ?>
  <script type='text/javascript' charset='utf-8' src='<?= makeLink('js', 'main', 'js') ?>?id=<?= $this->jsRes ?>'></script>
<? endif; ?>
  <meta name=description content="Massively multiplayer strategy game based on planet conquest, research, spying, diplomacy, war, alliances. Features both permanent and 'speed' games." />
  <meta name=keywords content="massively multiplayer, free, strategy game, wargame, multiplayer games, multiplayer game, strategy games, planet conquest, planets, colonisation, colonization, galaxy, galactic, universe, persistent, permanent, warships, fleets, fighter, massively multiplayer game, cruiser, battle cruiser, star, starship, ground armies, strategy game, planet, nebula, nebulae, players, alliances, online multiplayer game, diplomacy, spy, trade, spying, trading, web, browser, based, multiplayer, strategy" />
 </head>
 <body>
  <div id="extframe">
   <div class="internal" id="intframe">
    <a id="hbutton" href="<?= makeLink('index', 'main') ?>" title="Legacy Worlds home page"><span>Legacy Worlds</span></a>
    <div id="version">
     current version: <?= config::$main['v_game'] ?> -
     engine v<?= config::$main['v_engine'] ?> -
     revision <?= config::$main['v_rev'] ?>
    </div>
    <div id="players"><? include("players.en.inc"); ?></div>
    <div id="lbox"><? include("lbox.en.inc"); ?></div>
