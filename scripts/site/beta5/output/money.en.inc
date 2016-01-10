  <table cellspacing='0' cellpadding='0'>
   <tr>
    <td class='div2'>
     <h1>Current Funds</h1>
      <p>Your empire has <b id='cfunds'></b>.</p>
    </td>
    <td rowspan='2' class='pc45'>
     <h1>Daily Profits</h1>
      <p>
       <a href='#pinc' <?=tooltip('Click here to go to the planets profits and loss section of the page')?>>Planet Income:</a> <b id='pinc'></b><br/>
       <a href='#fukp' <?=tooltip('Click here to go to fleets upkeep section of the pahe')?>>Fleet Upkeep:</a> <b id='fupk'></b><br/>
       <b>Daily Profits: &euro;<span id='dprof'></span></b>
      </p>
    </td>
    <td rowspan="2" style="width:5%;vertical-align:top"><a href="manual?p=money">Help</a></td>
   </tr>
   <tr>
    <td class='div2'>
     <div id='transfer'>&nbsp;</div>
    </td>
   </tr>
   <tr><td colspan='3'><hr/></td></tr>

   <tr><td colspan='3'><a name='pinc'></a><h1>Planet Profit and Loss</h1></td></tr>
   <tr><td colspan='3' id='planets'></td></tr>
   <tr><td colspan='3'><hr/></td></tr>

   <tr><td colspan='3'><a name='fukp'></a><h1>Fleet Upkeep Expenses</h1></td></tr>
   <tr><td colspan='3' id='fleets'></td></tr>
  </table>
