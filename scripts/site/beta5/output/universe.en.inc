<div style="display:none;visibility:hidden"><form action="?" onsubmit="return false"><textarea id="init-data"><?=$args?></textarea></form></div>
<table>
 <tr>
  <td class="div2">
   <h1>Universe Information</h1>
    <h2>Galaxy</h2>
     <p>
      Planets: <b id="npl"> </b><br/>
      Neutral planets: <b id="nnpl"> </b><br/>
      Systems occupied by nebulas: <b id="nnsys"> </b><br/>
      Average turrets/planet: <b id="avgt"> </b><br/>
      Average factories/planet: <b id="avgf"> </b>
     </p>
     <p><a href="map" <?=tooltip('Click here to go to the map page')?>>View maps</a></p>
  </td>
  <td class="pc45">
   <h1>Ticks</h1>
    <p>
     <span id="ticks">&nbsp;</span>
     <br/>
     <a href="ticks" <?=tooltip('Click here to go to the ticks page')?>>More details...</a>
    </p>
   <h1>Rankings</h1>
    <p>
     You are ranked <b id="genrank"> </b> with <b id="genpts"> </b> points.<br/>
     Your financial ranking is <b id="finrank"> </b> (<b id="finpts"> </b> points).<br/>
     Your civilization ranking is <b id="civrank"> </b> (<b id="civpts"> </b> points).<br/>
     Your military ranking is <b id="milrank"> </b> (<b id="milpts"> </b> points).<br/>
     Your inflicted damage ranking is <b id="idrank"> </b> (<b id="idpts"> </b> points).<br/>
     <span id="rndrank"> </span><br/>
     There are <b id="rknplayers"> </b> players in the rankings.<br/>
     <br/>
     <a href="rank" <?=tooltip('Click here to go to the rankings page')?>>More details...</a>
    </p>
  </td>
  <td style="width:5%;vertical-align:top"><a href="manual?p=universe_page">Help</a></td>
 </tr>
</table>
