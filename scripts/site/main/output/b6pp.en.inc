<?
include('menu.en.inc');
$this->title = 'Beta 6 planets';
$this->addStylesheet('b6pp');

$envType = array(
	"0"	=> "excellent",
	"1"	=> "good",
	"2"	=> "not so great",
	"3"	=> "bad",
	"4"	=> "infernal",
	"5"	=> "not so great",
	"6"	=> "bad",
	"7"	=> "good"
);
$pType = array(
//	"0"	=> array( "Earth-like planet", "this planet is very similar to Earth"),
	"0"	=> array( "temperate planet", "this planet is very similar to Earth, but the temperature is more constant - the planet has no icecaps and no dry deserts"),
	"1"	=> array( "planetary ocean", "this planet's surface is covered by an ocean; it should be quite cloudy, but not to the point of hiding the surface"),
	"2"	=> array( "Mars-like planet", "this planet is very similar to Mars; it has a relatively thin atmosphere, but isn't too friendly" ),
	"3"	=> array( "dead world", "this very rocky planet has almost no atmosphere"),
	"4"	=> array( "lava world", "this planet's volcanism is downright crazy; it has an atmosphere, but you sure as hell wish it didn't." ),
	"5"	=> array( "desert world", "this planet is a sand desert, with a very clear atmosphere." ),
	"6"	=> array( "ice world", "this planet is very cold, covered in snow; even the surface of its ocean is frozen." ),
	"7"	=> array( "jungle world", "this planet is covered in forests and jungles; it doesn't really have seas and oceans, but rather gigantic swamps." )
);

$this->startContents();
?>
<h2>Beta 6 planet pictures</h2>
<p><a href="?c=">Rate a picture</a> - <a href="?c=t">Top ratings</a> - You rated <?=$args['rated']?> out of <?=$args['total']?> pictures.</p>
<p>&nbsp;</p>
<? if ($args['page'] == 'nu'): ?>
<h3>There are no more pictures for you to rate</h3>
<p>
 It would seem that you have rated all of the currently available pictures.
</p>
<p>
 Please come back later!
</p>
<? elseif ($args['page'] == 'vp') : ?>
<table style="margin: 0px auto; border-width:0px; width: 90%">
 <tr>
  <td style="vertical-align: middle; border-width:0px; text-align:center">
   <img style="border: 1px solid white; margin: 1px" src="<?= getStatic('beta6-planets/' . $args['pic']['id'] . '-small.png') ?>" alt="small" />
  </td>
  <td style="vertical-align: middle; border-width:0px; text-align:center">
   <img style="border: 1px solid white; margin: 1px" src="<?= getStatic('beta6-planets/' . $args['pic']['id'] . '-medium.png') ?>" alt="medium" />
  </td>
  <td style="vertical-align: top; border-width: 0px; padding: 5px; text-align: left" rowspan="2">
   <h3>Planet details</h3>
   <p>Planet size: <?= $args['pic']['p_size'] ?> / 10</p>
   <p>Environment: <?= $envType[$args['pic']['p_type']] ?></p>
   <p>Type: <?= $pType[$args['pic']['p_type']][0] ?></p>
   <p>Description: <?= $pType[$args['pic']['p_type']][1] ?></p>
<? if (is_null($args['cr'])) : ?>
   <p>&nbsp;</p>
   <h3>Rate this picture</h3>
   <div>
    <form action="?" method="get">
     <div><p>
      <input type="hidden" name="c" value="r" />
      <input type="hidden" name="id" value="<?= $args['pic']['id'] ?>" />
      (worst) &nbsp; 
<?php
for ($i = 1; $i <= 5; $i ++) {
?>
      <input type="radio" name="r" value="<?=$i?>" onclick="this.form.submit();" />&nbsp;<?=$i?> &nbsp;
<?
}
?> &nbsp;(best)
     </p></div>
    </form>
   </div>
   <p>&nbsp;</p>
   <p>Each planet you rate grants you 120 <a href='<?=makeLink('contrib', 'main')?>'>contribution credits</a>.</p>
<? else: ?>
   <h3>Ratings</h3>
   <p>Your rating: <?= $args['cr'] ?> / 5</p>
   <p>Average rating: <?= $args['ar'] ?></p>
   <p>Total votes: <?= $args['nv'] ?></p>
<? endif; ?>
  </td>
 </tr>
 <tr>
  <td style="vertical-align: middle; border-width:0px; text-align:center; width: 200px" colspan="2">
   <img style="border: 1px solid white; margin: 1px" src="<?= getStatic('beta6-planets/' . $args['pic']['id'] . '-large.png') ?>" alt="large" />
  </td>
 </tr>
</table>
<? elseif ($args['page'] == 'tt') : ?>
<h3 style="margin: 5px 0px; text-align: center">Top 50 planet pictures</h3>
<table style="margin: 0px auto; border-style: none">
<?php
foreach ($args['pics'] as $pic) {
?>
 <tr>
  <td style="vertical-align: middle; border-width:0px; text-align:center; width: 36px">
   <a href="?c=v&id=<?=$pic['picture']?>"><img style="border: 1px solid white; margin: 1px" src="<?= getStatic('beta6-planets/' . $pic['picture'] . '-small.png') ?>" alt="planet" /></a>
  </td>
  <td style="vertical-align: middle; border-width:0px; text-align:center; padding: 0px 20px">
   <?= $pic['votes'] ?> vote<?= $pic['votes'] > 1 ? "s" : "" ?>
  </td>
  <td style="vertical-align: middle; border-width:0px; text-align:center; padding: 0px 20px">
   <?= sprintf("%.2f", $pic['rating']) ?> / 5.00
  </td>
 </tr>
<?
}
?>
</table>
<? elseif ($args['page'] == 'nt') : ?>
<h3>No planets have been rated at the moment</h3>
<p>... which kind of explains why this page is blank.</p>
<? endif; ?>
<? $this->endContents(); ?>
