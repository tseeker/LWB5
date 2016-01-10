<div style="display:none;visibility:hidden"><form action="?" onsubmit="return false"><textarea id="init-data"><?=$args?></textarea></form></div>
  <table>
   <tr>
    <td class='div2'>
     <h1>Planets</h1>
      <h2>Overview</h2>
       <p>
        Planets owned: <b id='plcnt'></b><br/>
	Average happiness: <span id='plahap'></span><br/>
	Average corruption: <span id='placor'></span><br/>
	Total population: <b id='plpop'></b><br/>
	Average population: <span id='plapop'></span><br/>
	Total factories: <b id='plfct'></b><br/>
	Average factories: <span id='plafct'></span><br/>
	Total turrets: <b id='pltrt'></b><br/>
	Average turrets: <span id='platrt'></span><br/>
	<span id='platt'></span>
	<a href='planets' <?=tooltip('Click here to go to the main planets page')?>>Planet list</a>
       </p>
      <h2>Quick links</h2>
       <p id='pllst'></p>
     <h1>Research</h1>
      <h2>Budget</h2>
       <p>
        Fundamental research: <b id='rsbf'></b> (<b id='rspf'></b> points/day)<br/>
        Military research: <b id='rsbm'></b> (<b id='rspm'></b> points/day)<br/>
        Civilian research: <b id='rsbc'></b> (<b id='rspc'></b> points/day)<br/>
	<a href='research' <?=tooltip('Click here to go to the research management page')?>>Manage research</a>
       </p>
      <div id="rsst"></div>
    </td>
    <td class='pc45'>
     <h1>Money</h1>
      <p>
       Total income: &euro;<b id='minc'>&nbsp;</b><br/>
       Daily profits: &euro;<b id='mprof'>&nbsp;</b><br/>
       <a href='money' <?=tooltip('Click here to go to the money page')?>>Details ...</a>
     <h1>Fleets</h1>
      <h2>Overview</h2>
       <p>
        Total fleet power: <b id='fltot'></b><br/>
	Fleet upkeep: &euro;<b id='flupk'></b><br/>
	Number of fleets: <b id='flcnt'></b><br/>
	Fleets engaged in battle: <b id='flbat'></b><br/>
	<a href='fleets' <?=tooltip('Click here to go to the fleets management page')?>>View fleets</a>
       </p>
      <h2>Fleets at home</h2>
       <p>
	Number of fleets: <b id='flhcnt'></b><br/>
	Fleets engaged in battle: <b id='flhbat'></b>
       </p>
      <h2>Other fleets</h2>
       <p>
	Fleets on foreign planets: <b id='flocnt'></b><br/>
	Fleets engaged in battle: <b id='flobat'></b><br/>
	Moving fleets: <b id='flomv'></b><br/>
	Fleets waiting in Hyperspace: <b id='flowt'></b>
       </p>
      <h2>Statistics</h2>
       <p>
        GA ships: <b id='flgas'></b><br/>
        Fighters: <b id='flfgt'></b><br/>
        Cruisers: <b id='flcru'></b><br/>
        Battle cruisers: <b id='flbcr'></b><br/>
        Total ships: <b id='flsht'></b>
       </p>
    </td>
    <td style="width:5%;text-align:left;vertical-align:top;">
     <a href="manual?p=empire_overview">Help</a>
    </td>
   </tr>
  </table>

