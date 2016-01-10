function        makePlanetsTooltips()
{
	plstt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<40;i++)
			plstt[i] = "";
		return;
	}
	plstt[0] = tt_Dynamic("Click here to swtich between individual and cumulative build times for the planets' build queues");
	plstt[1] = tt_Dynamic("Click here to switch between list and quick builder modes");
	plstt[2] = tt_Dynamic("Click here to get a new planet if you have lost all those you had");
	plstt[10] = tt_Dynamic("Check this checkbox to select this planet and build what you'll define in the quick builder on it");
	plstt[11] = tt_Dynamic("Click here to go to this planet's individual page");
	plstt[12] = tt_Dynamic("Check this checkbox to select this item in this planet's build queue and perform some queue operation on it with the quick builder");
	plstt[20] = tt_Dynamic("Use this radio button to perform operations on factories on the selected planets");
	plstt[21] = tt_Dynamic("Use drop down list to select what operation to perform with factories on the selected planets");
	plstt[22] = tt_Dynamic("Use this text field to type in the number of factories to build or destroy on the selected planets");
	plstt[23] = tt_Dynamic("Use this drop down list to select the type of factories with which to perform the defined operation on the selected planets");
	plstt[24] = tt_Dynamic("Use this radio button to add items to the build queues on the selected planets");
	plstt[25] = tt_Dynamic("Use this text field to type in the number of items to add to the build queue on the selected planets");
	plstt[26] = tt_Dynamic("Use this drop down list to select the kind of items to add to the build queue on the selected planets");
	plstt[27] = tt_Dynamic("Use this radio button to flush the build queue on the selected planets");
	plstt[28] = tt_Dynamic("Use this radio button to delete the selected itesm from the build queues");
	plstt[29] = tt_Dynamic("Use this radio button move the selected items in the build queues");
	plstt[30] = tt_Dynamic("Use this drop down list to select in what direction to move the selected items in the build queues");
	plstt[31] = tt_Dynamic("Use this radio button to replace the selected items in the build queues with the defined new items");
	plstt[32] = tt_Dynamic("Use this text field to type in the number of items to build to replace the selected items");
	plstt[33] = tt_Dynamic("Use this drop down list to select the type of items to build to replace the selected items");
	plstt[34] = tt_Dynamic("Click here to execute the action you've defined with the quick builder and keep the quick builder open");
	plstt[35] = tt_Dynamic("Click here to execute the action you've defined with the quick builder and go back to list mode");
	plstt[36] = tt_Dynamic("Click here to select all planets for quick builder operations");
	plstt[37] = tt_Dynamic("Click here to unselect all planets for quick builder operations");
	plstt[38] = tt_Dynamic("Click here to invert the planets selection for quick builder operations");
}


function drawMainDisplay() {
	var	str, i;
	var cs = ';text-align:center;vertical-align:bottom';
	var csb = ';border-color:white;border-style:solid;border-width: 0px 0px 1px 0px';
	str = '<table cellspacing="0" cellpadding="0" style="margin: 0px 0px 10px 0px">'
		+ '<tr><td class="div2"><h1>Controlled planets</h1></td><td class="flink">'
		+ (vacation ? '' : ('<a ' + plstt[1]
			+ ' href="#" onClick="enableQuickBuilder(); return false;">Quick builder mode</a> - '))
		+ '<a ' + plstt[1] + ' href="manual?p=planets#pop_cp">Help</a></td>'
		+ '</tr><tr><td colspan="2">'
		+ '<table cellspacing="0" cellpadding="0" style="width:100%;border-collapse:true;border-style:solid'
			+ ';border-color:white;border-width:0px 0px 2px 0px">'
		 + '<tr>'
		  + '<th style="width:32px;height:32px' + csb + '" rowspan="2">&nbsp;</th>'
		  + '<th colspan="2" rowspan="2" style="text-align:left;vertical-align:bottom' + csb +'">Planet</th>'
		  + '<th rowspan="2" style="width:10%;max-width:80px' + cs + csb + '">Coord.</th>'
		  + '<th rowspan="2" style="width:8%;max-width:60px' + cs + csb + '">Pop.</th>'
		  + '<th style="width:6%;max-width:50px' + cs + '">Happ.</th>'
		  + '<th style="width:6%;max-width:50px' + cs + '">Ind.</th>'
		  + '<th rowspan="2" style="width:8%;max-width:70px' + cs + csb + '">Turrets</th>'
		  + '<th rowspan="2" style="width:8%;max-width:80px' + cs + csb + '">Profit</th>'
		 + '</tr><tr>'
		  + '<th style="width:6%;max-width:50px' + cs + csb + '">Corr.</th>'
		  + '<th style="width:6%;max-width:50px' + cs + csb + '">Mil.</th>'
		 + '</tr>'
	if (planets.length > 0) {
		for (i = 0; i < planets.length; i ++) {
			str += buildPlanetLine(i);
		}
	} else {
		str += '<tr><td>&nbsp;</td></tr>';
		str += '<tr><th>You do not control any planet.</th></tr>';
		str += '<tr><td><a ' + plstt[2] + '  href="nplanet">Get a new planet</a></td></tr>';
		str += '<tr><td>&nbsp;</td></tr>';
	}
	str += '</table></td></tr></table>';
	return	str;
}

function drawBQSummary(bq) {
	var i, str;
	var types = new Array('Turret', 'GA Ship', 'Fighter', 'Cruiser', 'Battle Cruiser');
	var csb = ';padding: 4px 0px 0px 0px;border-style:solid;border-color:white;border-width: 1px 0px 0px 0px';

	if (bq.length == 0) {
		return '<td>&nbsp;</td>';
	}
	return '<td class="bq">Build queue loaded for <b>' + bq[bq.length - 1].cttb + '</b>h</td>';
}


function buildBuilderLine(ipl) {
	var	str, j;
	var	types = new Array('Turret', 'GA&nbsp;Ship', 'Fighter', 'Cruiser', 'Battle&nbsp;Cruiser');
	str  = '<tr><td><table cellspacing="0" cellpadding="0">';
	str += '<td class="pimg" rowspan="2"><input ' + plstt[10] + ' type="checkbox" name="qb_pl' + planets[ipl].id
		+ '" value="1"';
	if (planets[ipl].selected) {
		str += ' checked="checked"';
	}
	str += ' id="qb_pl' + planets[ipl].id + '" onClick="return selPlanet('+ipl+');" />';
	str += '</td><td class="pname"><label for="qb_pl' + planets[ipl].id + '"><b>' + planets[ipl].name
		+ '</b></label>';
	if (planets[ipl].bq.length > 0) {
		var as = ' href="#" style="color: white;text-decoration:underline;font-weight:normal" ';
		str += ' (select items: <a' + as + 'onclick="selAllAt(' + planets[ipl].id + ');return false">'
			+ 'all</a> - <a' + as + 'onclick="selNoneAt(' + planets[ipl].id + ');return false">'
			+ 'none</a> - <a' + as + 'onclick="invertSelAt(' + planets[ipl].id + ');return false">'
			+ 'invert</a>)';
	}
	str += '</td><td rowspan="2">' + formatNumber(planets[ipl].pop) + '</td><td>'
		+ drawHappiness(planets[ipl].hap) + '</td><td>' + formatNumber(planets[ipl].ind)
		+ '</td><td rowspan="2">' + formatNumber(planets[ipl].tur) + '</td></tr><tr><td class="pname">';

	if (planets[ipl].bq.length == 0) {
		str += 'Build Queue is empty';
	} else {
		for (j=0;j<planets[ipl].bq.length;j++) {
			var name = 'qb_pl' + planets[ipl].id + '_i' + j;
			str += '<input ' + plstt[12] + ' type="checkbox" name="' + name + '" id="' + name
				+ '" value="1" onClick="return selItem(' + ipl + ',' + j + ');"';
			if (planets[ipl].bq[j].selected) {
				str += ' checked="checked"';
			}
			str += '/><label for="'+name+'"><b>' + formatNumber(planets[ipl].bq[j].qt)
				+ '</b>&nbsp;' + types[planets[ipl].bq[j].type];
			if (planets[ipl].bq[j].qt > 1) {
				str += 's';
			}
			var v = (useCTTB ? planets[ipl].bq[j].cttb : planets[ipl].bq[j].ttb);
			str += '&nbsp;(<b>' + v + '</b>h)</label> ';
		}
	}
	str += '</td><td>' + drawCorruption(planets[ipl].corruption) + '</td><td>' + formatNumber(planets[ipl].mil)
		+ '</td></tr></table></td></tr>';
	return	str;
}


function	drawQuickBuilder()
{
	var	str;
	// Header
	str  = '<form action="?"><table cellspacing="0" cellpadding="0">';
	str += '<tr><td class="div2"><h1>Quick builder</h1</td><td class="flink">';
	str += '<a ' + plstt[0] + ' href="#" onClick="switchTTB(); return false;">Display ' + (useCTTB ? 'individual' : 'cumulative') + ' time</a> - ';
	str += '<a ' + plstt[1] + ' href="#" onClick="enableQuickBuilder(); return false;">List mode</a> - ';
	str += '<a href="manual?p=planets#pop_qb">Help</a></td>';
	str += '</tr>';
	// Planet operations header
	str += '<tr><td><table id="qbuild" cellspacing="0" cellpadding="0">';
	str += '<tr><td colspan="3"><h3>Planet operations</h3></td></tr>';
	// Factories
	str += '<tr><td class="dv"></td><td class="dv"><input ' + plstt[20] + ' type="radio" name="qb_ac" value="fac" id="qb_ac1" ';
	if	(action == 1)
		str += 'checked="checked" ';
	str += 'onClick="selAction(1); return true;" /></td>';
	str += '<td class="pc90" onClick="selAction(1)"><select ' + plstt[21] + ' name="qb_fdestr" id="qb_fdestr"><option value="0">Build</option>';
	str += '<option value="1">Destroy</option></select><input ' + plstt[22] + ' type="text" name="qb_fcount" value="" size="6" maxlength="8" id="qb_fcount" />';
	str += '<select ' + plstt[23] + ' name="qb_ftype" id="qb_ftype"><option value="0">industrial</option><option value="1">military</option>';
	str += '</select> factories</td></tr>';
	// Add to queue(s)
	str += '<tr><td></td><td class="dv"><input ' + plstt[24] + ' type="radio" name="qb_ac" id="qb_ac2" ';
	if	(action == 2)
		str += 'checked="checked" ';
	str += 'value="adq" onClick="selAction(2); return true;" /></td>';
	str += '<td onClick="selAction(2)">Add <input ' + plstt[25] + ' type="text" name="qb_adc" id="qb_adc" value="" size="5" maxlength="8" />';
	str += '<select ' + plstt[26] + ' name="qb_adt" id="qb_adt">'+bqItems()+'</select> to the queues</td></tr>';
	// Flush queue(s)
	str += '<tr><td></td><td class="dv"><input ' + plstt[27] + ' type="radio" name="qb_ac" value="fls" id="qb_ac3" ';
	if	(action == 3)
		str += 'checked="checked" ';
	str += 'onClick="selAction(3); return true;" /></td>';
	str += '<td onClick="selAction(3)">Flush build queues</td></tr>';
	str += '</table></td>';
	// Queue operations header
	str += '<td><table id="qbuild" cellspacing="0" cellpadding="0">';
	str += '<tr><td colspan="3"><h3>Queue operations</h3></td></tr>';
	// Delete items
	str += '<tr><td class="dv"></td><td class="dv"><input ' + plstt[28] + ' type="radio" name="qb_ac" value="dsi" id="qb_ac4" ';
	if	(action == 4)
		str += 'checked="checked" ';
	str += 'onClick="selAction(4); return true;" /></td>';
	str += '<td onClick="selAction(4)">Delete selected items</td></tr>';
	// Move items
	str += '<tr><td class="dv"></td><td class="dv"><input ' + plstt[29] + ' type="radio" name="qb_ac" value="dsi" id="qb_ac5" ';
	if	(action == 5)
		str += 'checked="checked" ';
	str += 'onClick="selAction(5); return true;" /></td>';
	str += '<td onClick="selAction(5)">Move selected items <select ' + plstt[30] + ' name="qb_mid" id="qb_mid">';
	str += '<option value="0">down</option><option value="1">up</option></select> in the queue</td></tr>';
	// Replace items
	str += '<tr><td></td><td class="dv"><input ' + plstt[31] + ' type="radio" name="qb_ac" value="adq" id="qb_ac6" ';
	if	(action == 6)
		str += 'checked="checked" ';
	str += 'onClick="selAction(6); return true;" /></td>';
	str += '<td onClick="selAction(6)">Replace items with <input ' + plstt[32] + ' type="text" name="qb_ric" id="qb_ric" value="" size="5" maxlength="8" />';
	str += '<select ' + plstt[33] + ' name="qb_rit" id="qb_rit">'+bqItems()+'</select></td></tr>';
	str += '</table></td></tr>';
	// Buttons
	str += '<tr><td colspan="2" class="fbutton"><input ' + plstt[34] + ' type="button" id="qbex1" value="Execute action" onClick="qbExecute(false); return false;" />';
	str += '<input ' + plstt[35] + ' type="button" value="Execute and hide" id="qbex2" onClick="qbExecute(true); return false;" />';
	str += '</td></tr>';
	// Planet list header
	str += '<tr><td colspan="2"><hr/></td></tr>';
	str += '<tr>';
	str += '<td><h1>Controlled planets</h1></td>';
	str += '<td class="flink">';
	str += 'Planets: <a ' + plstt[36] + ' href="#" onclick="selAllPlanets(); return false;">select all</a> -';
	str += '<a ' + plstt[37] + ' href="#" onclick="selNoPlanets(); return false;">unselect all</a> -';
	str += '<a ' + plstt[38] + ' href="#" onclick="invertPlanets(); return false">invert</a>';
	str += '</td></tr>';
	str += '<tr><td colspan="2" id="qb_planets">';
	// End
	str += '</td></tr></table></form>';

	return	str;
}


function drawQBList() {
	var str, i;
	str  = '<table class="list" id="planets" cellspacing="0" cellpadding="0"><tr><td><table cellspacing="0" cellpadding="0"><tr>';
	str += '<th class="pimg" rowspan="2"></th><th class="pname">Planet</th><th rowspan="2">Population</th>'
		+ '<th>Happiness</th><th>Industrial</th><th rowspan="2">Turrets</th></tr>'
		+ '<tr><th class="pname">Build queue</th><th>Corruption</th><th>Military</th></tr>'
		+ '</table></td></tr>';
	// Planet list
	if (planets.length > 0) {
		for (i=0;i<planets.length;i++) {
			str += buildBuilderLine(i);
		}
	} else {
		str += '<tr><td>&nbsp;</td></tr>';
		str += '<tr><th>You do not control any planet.</th></tr>';
		str += '<tr><td><a ' + plstt[2] + ' href="nplanet">Get a new planet</a></td></tr>';
		str += '<tr><td>&nbsp;</td></tr>';
	}
	str += '</table>';
	return	str;
}


function	bqItems()
{
	var	i,str;
	var	types = new Array('turret', 'GA ship', 'fighter', 'cruiser', 'battle cruiser');
	str = '';
	for	(i=0;i<milLevel;i++)
		str += '<option value="'+i+'">' + types[i] + '</option>';
	return	str;
}


function	qbError(en)
{
	var	str;
	switch	(en)
	{
	case	0:
		str = 'You must select an action.';
		break;
	case	1:
		str = 'You must select at least one planet.';
		break;
	case	2:
		str = 'Please specify the quantity of items to add to the build queues.';
		break;
	case	3:
		str = 'Please specify the quantity of factories to build or destroy.';
		break;
	case	14:
	case	4:
		str = 'You don\'t have enough money for this operation.';
		break;
	case	5:
		str = 'The number of factories to destroy exceed one of the planet\'s\nquantity of factories.';
		break;
	case	6:
		str = 'Your planets must keep a minimum of 1 military factory.';
		break;
	case	7:
		str = 'You can\'t destroy more than 10% of a planet\'s factories in 24h.';
		break;
	case	8:
		str = 'At least one of the selected planets is under siege.';
		break;
	case	9:
		str = 'You must select at least one item in the queue.';
		break;
	case	10:
		str = 'Please specify the quantity of replacement items.';
		break;
	case	11:
		str = 'You can\'t move up the first item of a build queue.';
		break;
	case	12:
		str = 'You can\'t move down the last item of a build queue.';
		break;
	case	13:
		str = 'You can\'t use the quick builder while in vacation mode.';
		break;
	case	15:
		str = 'You can\'t build factories while the planet is being transferred\nto another player.';
		break;
	case	16:
		str = 'Your population is too low, you can\'t build that many factories.';
		break;
	case	34:
		str = 'You can\'t destroy factories so soon after building them.\nYou have to wait for two hours after you last built factories.';
		break;
	default:
		str = 'An unkown error has occured: ' + en;
		break;
	}
	alert('Quick Builder: error\n\n' + str);
}
