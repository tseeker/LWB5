// Orbit
function	Orbit_drawFleetSummary()
{with(this){
	var	f = new Array(document.getElementById('fsum1'), document.getElementById('fsum2'),
		document.getElementById('fsum3'), document.getElementById('fsum4'));
	var	i = 1;

	if	(	fleets[1] == '0' && fleets[2] == '0' && fleets[3] == '0'
		||	fleets[1] == '' && fleets[2] == '' && fleets[3] == '')
	{
		f[0].innerHTML = f[1].innerHTML = f[2].innerHTML = f[3].innerHTML = '&nbsp;';
		return;
	}

	var	ff, ef, ffs, efs;
	if	(fleets[0] == '0')
	{
		f[0].innerHTML = '<b>Local fleets</b>';
		ff = 'Defending'; ef = 'Attacking';
		ffs = fleets[2]; efs = fleets[3];
	}
	else
	{
		ff = 'Friendly'; ef = 'Enemy';
		if	(fleets[0] == '1')
		{
			if	(fleets[3] == '0')
				f[0].innerHTML = '<b>Fleets standing by</b>';
			else
				f[0].innerHTML = '<b>Defending the planet</b>';
			ffs = fleets[2]; efs = fleets[3];
		}
		else
		{
			f[0].innerHTML = '<b>Attacking the planet</b>';
			ffs = fleets[3]; efs = fleets[2];
		}
	}

	if	(fleets[1] != '0')
	{
		f[i].innerHTML = '<em class="phapok">Own fleet power: <b>' + formatNumber(fleets[1]) + '</b></em>';
		i++;
	}
	if	(efs != '0')
	{
		f[i].innerHTML = '<em class="phapmed">' + ff + ' fleet power: <b>' + formatNumber(efs) + '</b></em>';
		i++;
	}
	if	(ffs != '0')
		f[i].innerHTML = '<em class="phapbad">'+ef+' fleet power: <b>' + formatNumber(ffs) + '</b></em>';
}}


// Nebula

function	Nebula_drawLayout()
{
	var	str;
	document.getElementById('plactions').innerHTML = '<a href="fleets?sto='+this.orbit.id+'">Send Fleets</a> - <a href="map?menu=p&ctr='+this.orbit.id+'">Centre Map</a>';
	document.getElementById('pimg').innerHTML = '<img src="'+staticurl+'/beta5/pics/nebula'+this.opacity+'.png" alt="Nebula - Opacity '+this.opacity+'" />';
	document.getElementById('plname').innerHTML = 'Nebula ' + this.orbit.name;
	
	str  = '<h2>Details</h2><table cellspacing="0" cellpadding="0"><tr>';
	str += '<td class="div20">&nbsp;</td><td class="pc70">Coordinates: ('+this.orbit.drawCoords()+')</td>';
	str += '<td class="pc20" id="fsum1">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '<tr><td class="div20">&nbsp;</td><td class="pc70">Opacity: <b>'+this.opacity+'</b></td>';
	str += '<td class="pc20" id="fsum2">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '<tr><td class="pc75" colspan="2">&nbsp;</td><td class="pc20" id="fsum3">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '<tr><td class="pc75" colspan="2">&nbsp;</td><td class="pc20" id="fsum4">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '</table>';
	document.getElementById("pldesc").innerHTML = str;
}


// Remains
function	Remains_drawLayout()
{
	var	str;
	document.getElementById('plactions').innerHTML = '<a href="fleets?sto='+this.orbit.id+'">Send Fleets</a> - <a href="map?menu=p&ctr='+this.orbit.id+'"">Centre Map</a>';
	document.getElementById('pimg').innerHTML = '<img src="'+staticurl+'/beta5/pics/prem_l.png" alt="Planetary Remains" />';
	document.getElementById('plname').innerHTML = 'Planetary Remains ' + this.orbit.name;
	
	str  = '<h2>Details</h2><table cellspacing="0" cellpadding="0"><tr>';
	str += '<td class="div20">&nbsp;</td><td class="pc70">Coordinates: ('+this.orbit.drawCoords()+')</td>';
	str += '<td class="pc20" id="fsum1">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '<tr><td class="div20">&nbsp;</td><td class="pc70">Opacity: <b>1</b></td>';
	str += '<td class="pc20" id="fsum2">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '<tr><td class="pc75" colspan="2">&nbsp;</td><td class="pc20" id="fsum3">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '<tr><td class="pc75" colspan="2">&nbsp;</td><td class="pc20" id="fsum4">&nbsp;</td><td class="div20">&nbsp;</td></tr>';
	str += '</table>';
	document.getElementById("pldesc").innerHTML = str;
}


// Planet
function	Planet_drawLayout()
{
	document.getElementById('pimg').innerHTML = '<img src="'+staticurl+'/beta5/pics/pl/l/'+this.orbit.id+'.png" alt="Planet" />';
	var str  = '<h2>Planet Overview</h2><table cellspacing="0" cellpadding="0"><tr>';
	str += '<td class="div20">&nbsp;</td>';
	str += '<td class="pc40">Coordinates: ('+this.orbit.drawCoords()+')</td>';
	str += '<td class="pc30" id="pprof">&nbsp;</td>';
	str += '<td class="fsum" id="fsum1">&nbsp;</td>';
	str += '<td class="div20">&nbsp;</td>';
	str += '</tr>';
	str += '<tr>';
	str += '<td>&nbsp;</td>';
	str += '<td>Alliance: <span id="ptag"></span></td>';
	str += '<td id="phap">&nbsp;</td>';
	str += '<td class="fsum" id="fsum2">&nbsp;</td>';
	str += '<td>&nbsp;</td>';
	str += '</tr>';
	str += '<tr>';
	str += '<td>&nbsp;</td>';
	str += '<td>Population: <b id="ppop"></b></td>';
	str += '<td id="pcor">&nbsp;</td>';
	str += '<td class="fsum" id="fsum3">&nbsp;</td>';
	str += '<td>&nbsp;</td>';
	str += '</tr>';
	str += '<tr>';
	str += '<td>&nbsp;</td>';
	str += '<td>Turrets: <b id="ptur"></b><span id="dtur">&nbsp;</span></td>';
	str += '<td id="pfact1">&nbsp;</td>';
	str += '<td class="fsum" id="fsum4">&nbsp;</td>';
	str += '<td>&nbsp;</td></tr><tr><td>&nbsp;</td><td>Status: <b id="pstat">&nbsp;</b></td>';
	str += '<td id="pfact2">&nbsp;</td><td>&nbsp;</td>';
	str += '</tr></table><br/>';
	
	str += '<div id="pcontrol" style="display:none"><form action="?" method="post" onSubmit="return false">';
	str += '<table cellspacing="0" cellpadding="0">';
	str += '<tr>';
	str += '<td colspan="2"><h2>Industrial factories</h2></td>';
	str += '<td colspan="2"><h2>Military factories</h2></td>';
	str += '</tr>';
	str += '<tr>';
	str += '<td class="div20">&nbsp;</td>';
	str += '<td class="pc35">Quantity: <b id="ifc2"></b> (Cost: <b id="ifcc"></b>)</td>';
	str += '<td class="div20">&nbsp;</td>';
	str += '<td class="pc55">Quantity: <b id="mfc2"></b> (Cost: <b id="mfcc"></b>)</td>';
	str += '</tr>';
	str += '<tr>';
	str += '<td>&nbsp;</td>';
	str += '<td>';
	str += '<input type="text" name="ifc" id="ifc" value="" size="5" />';
	str += '<input type="button" name="ifi" id="ifi" value="Increase" onClick="factAction(0, 0); return false;" />';
	str += '<input type="button" name="ifd" id="ifd" value="Decrease" onClick="factAction(1, 0); return false;" />';
	str += '</td>';
	str += '<td>&nbsp;</td>';
	str += '<td>';
	str += '<input type="text" name="mfc" id="mfc" value="" size="5" />';
	str += '<input type="button" name="mfi" id="mfi" value="Increase" onClick="factAction(0, 1); return false;" />';
	str += '<input type="button" name="mfd" id="mfd" value="Decrease" onClick="factAction(1, 1); return false;" />';
	str += '</td>';
	str += '</tr>';
	str += '</table>';
	str += '<br/><p>&nbsp;</p>';
	str += '<h2>Build Warfare</h2>';
	str += '<table cellspacing="0" cellpadding="0">';
	str += '<tr>';
	str += '<td class="div20" rowspan="2">&nbsp;</td>';
	str += '<td class="pbwlst" rowspan="2">';
	str += '<span id="bqstuff"></span><br/>';
	str += 'Quantity: <input type="text" size="6" value="" name="bwfq" id="bwfq" /><br/><br/>';
	str += '<input type="button" name="bwfa" id="bwfa" value="Add" onClick="addToQueue(); return false;" />';
	str += '<input type="button" name="bwfr" id="bwfr" value="Replace selected" onClick="replaceItems(); return false;" />';
	str += '</td>';
	str += '<td class="bqt" id="bqueue"></td>';
	str += '<td class="div20" rowspan="2">&nbsp;</td>';
	str += '</tr>';
	str += '<tr>';
	str += '<td class="bqct" id="bqbut"></td>';
	str += '</tr>';
	str += '</table></form></div>';

	str += '<div id="psale" style="display:none"><form action="?" method="post" onSubmit="return false">';
	str += '<h2>Give / Sale Planet</h2><table cellspacing="0" cellpadding="0">';
	str += '<tr><td class="div20" rowspan="4">&nbsp;</td>';
	str += '<td class="pc45"><input type="radio" name="smode" id="sm0" value="0" onClick="setSaleMode(0)" /> <label for="sm0">Gift</label></td>';
	str += '<td class="pc15"><label for="psexp">Offer expires:</label></td><td class="pc30"><select name="psexp" id="psexp" onChange="orbit.spec.sellForm.expires=';
	str += 'this.options[this.selectedIndex].value">';
	var	i, expTime = [0, 6, 12, 24, 48, 72, 96, 120];
	var	expLabel = ['Never', '6 hours', '12 hours', '1 day', '2 days', '3 days', '4 days', '5 days'];
	for	(i=0;i<expTime.length;i++)
		str += '<option value="'+expTime[i]+'">'+expLabel[i]+'</option>';
	str += '</select></td><td class="div20" rowspan="4">&nbsp;</td></tr>';
	str += '<tr><td class="pc45"><input type="radio" name="smode" id="sm1" value="1" onClick="setSaleMode(1)" /> <label for="sm1">Direct Sale</label></td>';
	str += '<td id="pstarl">&nbsp;</td><td id="pstar">&nbsp;</td></tr>';
	str += '<tr><td class="pc45"><input type="radio" name="smode" id="sm2" value="2" onClick="setSaleMode(2)" /> <label for="sm2">Public Sale</label></td>';
	str += '<td id="pspril">&nbsp;</td><td id="pspri">&nbsp;</td></tr>';
	str += '<tr><td class="pc45"><input type="radio" name="smode" id="sm3" value="3" onClick="setSaleMode(3)" /> <label for="sm3">Auction Sale</label></td>';
	str += '<td colspan="2">&nbsp;</td></tr>';
	str += '</table><br/>';
	str += '<h2>Bundled Fleets</h2>';
	str += '<div id="bndflt">&nbsp;</div>';
	str += '</form></div>';

	document.getElementById("pldesc").innerHTML = str;
}


function	Planet_draw()
{
	var	e, str;
	document.getElementById('plname').innerHTML = 'Planet ' + this.orbit.name;

	str = '<a href="fleets?sto='+this.orbit.id+'">Send Fleets</a> - <a href="map?menu=p&ctr='+this.orbit.id+'"">Centre Map</a>';
	if	(this.isOwn)
	{
		if	(this.sellForm)
		{
			str  = '<a href="#" onClick="confirmSale();return false">Confirm Sale</a> - ';
			str += '<a href="#" onClick="closeSaleForm();return false">Cancel</a>';
		}
		else if	(this.cAction == '0')
		{
			if	(this.canRename)
				str += ' - <a href="#" onClick="renamePlanet();return false">Rename</a>';
			if	(this.canAbandon)
				str += ' - <a href="#" onClick="abandonPlanet();return false">Abandon</a>';
			if	(this.canSell)
				str += ' - <a href="#" onClick="sellPlanet();return false">Sell/Give</a>';
			if	(this.canDestroy)
				str += ' - <a href="#" onClick="destroyPlanet();return false">Blow it up!</a>';
		}
		else if	(this.cAction == '1')
		{
			if	(this.time == 0)
			{
				if	(this.sellId == '')
					str += ' - Planet is for sale (<a href="#" onClick="removeSaleOffer();return false">Cancel</a>)';
				else
				{
					str += ' - Planet offered to <a href="message?a=c&ct=0&id='+this.sellId+'">' + this.sellName + '</a> ';
					str += '(<a href="#" onClick="removeSaleOffer();return false">Cancel</a>)';
				}
			}
			else
			{
				str += ' - <a href="message?a=c&ct=0&id='+this.sellId+'">' + this.sellName + '</a> taking control in <b>';
				str += this.time + '</b>h (<a href="#" onClick="removeSaleOffer();return false">Cancel</a>)';
			}
		}
		else if	(this.cAction == '2')
		{
			str += ' - Wormhole Super Nova in <b>' + this.time + '</b> hour' + (this.time > 1 ? 's' : '') + ' (<a href="#" onClick="';
			str += 'cancelDestruction();return false">Cancel</a>)';
		}
		else if	(this.cAction == '3')
		{
			str += ' - Abandoning the planet in ' + this.time + ' hour tick' + (this.time > 1 ? 's' : '') + ' (<a href="#" onClick="';
			str += 'cancelAbandon();return false">Cancel</a>)';
		}
	}
	else
		str += ' - <a href="message?a=c&ct=1&id=' + this.orbit.id + '">Message</a>';
	document.getElementById('plactions').innerHTML = str;

	// Planet overview
	document.getElementById('ptag').innerHTML = (this.tag == '') ? '-' : ('<b>['+this.tag+']</b>');
	document.getElementById('ptur').innerHTML = (this.turrets == '') ? '?' : ('<b>'+formatNumber(this.turrets)+'</b>');
	document.getElementById('pstat').innerHTML = this.protection ? "protected" : (
		this.vacation ? "on vacation" : "normal"
	);
	document.getElementById('ppop').innerHTML = (this.population == '') ? '?' : ('<b>'+formatNumber(this.population)+'</b>');
	if	(this.isOwn)
	{
		str = 'Happiness: <b class="phap';
		if	(this.happiness >= 70)
			str += 'ok';
		else if	(this.happiness >= 40)
			str += 'med';
		else if	(this.happiness >= 20)
			str += 'dgr';
		else
			str += 'bad';
		str += '">' + this.happiness + '%</b>';
		document.getElementById('phap').innerHTML = str;
		str = 'Corruption: <b class="phap';
		if	(this.corruption >= 71)
			str += 'bad';
		else if	(this.corruption >= 41)
			str += 'dgr';
		else if	(this.corruption >= 11)
			str += 'med';
		else
			str += 'ok';
		str += '">' + this.corruption + '%</b>';
		document.getElementById('pcor').innerHTML = str;
		document.getElementById('pprof').innerHTML = 'Planet Income: <b>&euro;' + formatNumber(this.profit) + '</b>';
	}
	else
		document.getElementById('pcor').innerHTML = document.getElementById('phap').innerHTML = document.getElementById('pprof').innerHTML = '&nbsp;';
	if	(this.tFactories != '')
	{
		document.getElementById('pfact1').innerHTML = 'Total factories: <b>' + formatNumber(this.tFactories) + '</b>';
		document.getElementById('pfact2').innerHTML = '&nbsp;'
	}
	else if	(this.iFactories != '')
	{
		document.getElementById('pfact1').innerHTML = 'Industrial factories: <b>' + formatNumber(this.iFactories) + '</b>';
		document.getElementById('pfact2').innerHTML = 'Military factories: <b>' + formatNumber(this.mFactories) + '</b>';
	}

	if	(!this.isOwn || this.sellForm)
	{
		e = document.getElementById('pcontrol');
		if	(e.style)
			e = e.style;
		e.display = 'none';
		if	(!this.isOwn)
			return;
	}

	// "Sell" form
	e = document.getElementById('psale');
	if	(e.style)
		e = e.style;
	if	(this.sellForm)
	{
		e.display = 'block';
		document.getElementById('sm'+this.sellForm.mode).checked = true;
		drawSaleForm(this.sellForm);
		return;
	}
	else
		e.display = 'none';

	e = document.getElementById('pcontrol');
	if	(e.style)
		e = e.style;
	e.display = 'block';

	document.getElementById('ifc2').innerHTML = formatNumber(this.iFactories);
	document.getElementById('ifcc').innerHTML = '&euro;' + formatNumber(this.caps[1]);
	document.getElementById('mfc2').innerHTML = formatNumber(this.mFactories);
	document.getElementById('mfcc').innerHTML = '&euro;' + formatNumber(this.caps[2]);

	var	i, ml = parseInt(this.caps[0], 10);
	str = '';
	for	(i=0;i<ml+2;i++)
	{
		str += '<input type="radio" name="bwft" value="';
		str += i + '" id="bwft'+i+'" onClick="mlSel=' + i + '"';
		if	(i == mlSel)
			str += ' checked="checked"';
		str += '/> <label for="bwft'+i+'">' + getBQItemName(i, true) + ' (&euro;';
		str += formatNumber(this.caps[i+4]) + ')</label><br/>';
	}
	document.getElementById('bqstuff').innerHTML = str;
	document.getElementById('bqueue').innerHTML = this.drawBuildQueue();
	document.getElementById('bqbut').innerHTML = drawBQButtons(this);
	document.getElementById('dtur').innerHTML = ' (<a href="#" onClick="destroyTurrets();return false">Destroy</a>)';
}


function	drawSaleForm()
{
	var	i,e = document.getElementById('psexp');
	for	(i=0;i<e.options.length;i++)
		if	(e.options[i].value == orbit.spec.sellForm.expires)
		{
			e.selectedIndex = i;
			break;
		}

	e = document.getElementById('pstarget');
	if	(e && orbit.spec.sellForm.mode > 1)
	{
		orbit.spec.sellForm.player = e.value;
		document.getElementById('pstarl').innerHTML = document.getElementById('pstar').innerHTML = '&nbsp;';
	}
	else if	(!e && orbit.spec.sellForm.mode <= 1)
	{
		document.getElementById('pstar').innerHTML = '<input type="text" name="pstarget" id="pstarget" />';
		document.getElementById('pstarl').innerHTML = '<label for="pstarget">Target player:</label>';
		document.getElementById('pstarget').value = orbit.spec.sellForm.player;
	}

	e = document.getElementById('psprice');
	if	(e && orbit.spec.sellForm.mode < 1)
	{
		orbit.spec.sellForm.player = e.value;
		document.getElementById('pspril').innerHTML = document.getElementById('pspri').innerHTML = '&nbsp;';
	}
	else if	(!e && orbit.spec.sellForm.mode >= 1)
	{
		document.getElementById('pspri').innerHTML = '<input type="text" name="psprice" id="psprice" />';
		document.getElementById('psprice').value = orbit.spec.sellForm.player;
	}
	if	(orbit.spec.sellForm.mode == 0)
		document.getElementById('pspril').innerHTML = '&nbsp;';
	else if	(orbit.spec.sellForm.mode == 3)
		document.getElementById('pspril').innerHTML = '<label for="psprice">Minimum bid:</label>';
	else
		document.getElementById('pspril').innerHTML = '<label for="psprice">Price:</label>';

	if	(orbit.spec.sellForm.fleets.length == 0)
		document.getElementById('bndflt').innerHTML = '<p>No fleets are available.</p>';
	else
	{with(orbit.spec.sellForm){
		var	str;
		str = '<table class="fbundle"><tr><th class="fbsel">&nbsp;</th><th class="fbnam">Name</th><th class="fshp">G.A. Ships</th>'
			+ '<th class="fshp">Fighters</th><th class="fshp">Cruisers</th><th class="fshp">Battle Cruisers</th>'
			+ '<th class="fpwr">Power</th></tr>';
		for	(i=0;i<fleets.length;i++)
			str += '<tr><td class="fbsel"><input type="checkbox" name="fbsel" onClick="orbit.spec.sellForm.fleets[' + i
				+ '][7] = !orbit.spec.sellForm.fleets[' + i + '][7]" ' + (fleets[i][7] ? 'checked="checked"' : '')
				+ '/></td><td class="fbnam">' + fleets[i][6] + '</td><td class="fshp">' + formatNumber(fleets[i][1])
				+ '</td><td class="fshp">' + formatNumber(fleets[i][2]) + '</td><td class="fshp">' + formatNumber(fleets[i][3])
				+ '</td><td class="fshp">' + formatNumber(fleets[i][4]) + '</td><td class="fpwr">' + formatNumber(fleets[i][5])
				+ '</td></tr>';
		str += '</table>';
		document.getElementById('bndflt').innerHTML = str;
	}}
}



function	getBQItemName(id, pl)
{
	var	names = ['Turret', 'GA Ship', 'Fighter', 'Cruiser', 'Battle Cruiser'];
	return	names[id] + (pl ? 's' : '');
}


function	drawBQHeader()
{
	var	str;
	str  = '<tr class="bqlf">';
	str += '<td class="bqsel">&nbsp;</td>';
	str += '<td colspan="2">&nbsp;</td>';
	str += '<th colspan="2">Time to build</th>';
	str += '</tr>';
	str += '<tr class="bql">';
	str += '<td class="bqsel">&nbsp;</td>';
	str += '<th class="bqqt">Qty</th>';
	str += '<th class="bqt">Type</th>';
	str += '<th>Ind.</th>';
	str += '<th>Cum.</th>';
	str += '</tr>';
	return	str;
}

function	getEmptyBQ()
{
	return	'<h2>The Build Queue is empty</h2>';
}


function	drawBQButtons(planet)
{
	var	str;
	if	(!planet.bq.length)
		return	"&nbsp;";
	str  = '<input type="button" name="bqmu" id="bqmu" value="Up" onClick="moveItems(1); return false;" />';
	str += '<input type="button" name="bqmd" id="bqmd" value="Down" onClick="moveItems(0); return false;" />';
	str += '<input type="button" name="bqdl" id="bqdl" value="Cancel" onClick="deleteItems(); return false;" />';
	str += '<input type="button" name="bqfl" id="bqfl" value="Flush" onClick="flushQueue(); return false;" />';
	return	str;
}


function	alertFlush()
{
	return	confirm('Are you sure you want to flush the build queue?');
}


function	plError(en)
{
	var	str;
	switch	(en)
	{
	case	-1:
		str = 'You are no longer the owner of this planet.';
		break;
	case	0:
	case	24:
		str = 'You don\'t have enough money for this operation.';
		break;
	case	1:
		str = 'You must select the type of items to build.';
		break;
	case	2:
		str = 'Please specify the quantity of items to add to the build queues.';
		break;
	case	3:
		str = 'Please specify the quantity of factories to build or destroy.';
		break;
	case	4:
		str = 'Please specify the quantity of replacement items.';
		break;
	case	5:
		str = 'You must select at least one item in the queue.';
		break;
	case	6:
		str = 'You can\'t move up the first item of the build queue.';
		break;
	case	7:
		str = 'You can\'t move down the last item of the build queue.';
		break;
	case	8:
		str = 'The number of factories to destroy exceed the planet\'s quantity of factories.';
		break;
	case	9:
		str = 'Your planet must keep a minimum of 1 military factory.';
		break;
	case	10:
		str = 'You can\'t destroy more than 10% of a planet\'s factories in 24h.';
		break;
	case	11:
		str = 'Control over this planet is being transfered.';
		break;
	case	12:
		str = 'This planet is under siege.';
		break;
	case	13:
		str = 'This planet name is too long (maximum 15 characters).';
		break;
	case	14:
		str = 'This planet name is incorrect (letters, numbers, spaces and _.@-+\'/ only).';
		break;
	case	15:
		str = 'Multiple spaces are not allowed.';
		break;
	case	16:
		str = 'This planet name is too short (minimum 2 characters).';
		break;
	case	17:
		str = 'Planet names must contain at least one letter';
		break;
	case	18:
		str = 'Impossible to cancel the transfer, you do not have enough cash for a refund.';
		break;
	case	19:
		str = 'Please specify a valid amount of turrets to destroy.';
		break;
	case	20:
		str = 'Please specify a number of turrets that\'s actually inferior to the amount\nof turrets on the planet ...';
		break;
	case	21:
		str = 'You can\'t destroy more than 20% of a planet\'s turrets in 24h.';
		break;
	case	22:
		str = 'You can\'t destroy turrets while the planet is being transferred\nto another player.';
		break;
	case	23:
		str = 'You can\'t destroy turrets while the planet is under siege.';
		break;
	case	25:
		str = 'You can\'t build factories while the planet is being transferred\nto another player.';
		break;
	case	26:
		str = 'Your population is too low, you can\'t build that many factories.';
		break;
	case	37:
		str = 'You can\'t destroy factories so soon after building them.\nYou have to wait for two hours after you last built factories.';
		break;
	case	38:
		str = 'This planet name is unavailable.';
		break;
	case 200:
		str = 'You can\'t do anything while in vacation mode.';
		break;
	default:
		str = 'An unkown error has occured: ' + en;
		break;
	}
	alert('Planet Manager: error\n\n' + str);
}


function	factoryConfirm(act, ft, qt)
{
	var	str;
	if	(act == 0)
	{
		var	c = qt;
		str = 'You are about to build ';
		str += formatNumber(qt.toString()) + ' ' + (ft == '1' ? 'military' : 'industrial') + ' factor' + (qt > 1 ? 'ies' : 'y');
		str += '.\nThis operation will cost you ';
		c *= parseInt(orbit.spec.caps[ft == '1' ? 2 : 1], 10);
		str += formatNumber(c.toString()) + ' euros.\nPlease confirm.';
	}
	else
	{
		str  = 'You are about to destroy ' + qt + ' ';
		str += (ft == '1' ? 'military' : 'industrial') + ' factor' + (qt > 1 ? 'ies' : 'y');
		str += '.\nPlease note that your factories will not be refunded.\nPlease confirm this operation.';
	}
	return	confirm(str);
}


function promptNewName() {
	return prompt('Please type in the new name for this planet:', '');
}


function promptDestroyTurrets() {
	return prompt('How many turrets do you wish to destroy?', '1');
}


function saleError(e) {
	var	str;
	switch (e) {
		case 1: str = 'You must indicate a player to give / sale the planet to.'; break;
		case 2: str = 'Please specify a price.'; break;
		case 3: str = 'Auction sales must have an expiration date. Please select one.'; break;
		case 4: str = 'Target player not found.'; break;
		case 5: str = 'Target player is under Peacekeeper protection.'; break;
		case 6: str = 'Unable to sell something to yourself.'; break;
		case 7: str = 'Fleet not found.'; break;
		case 200: str = 'You can\'t do anything while in vacation mode.'; break;
		case 201: str = 'You can\'t sell planets while under Peacekeeper protection.'; break;
		default: str = 'An unknown error has occured.'; break;
	}
	alert('Planet Sale - Error\n' + str);
}

function	userConfirmSale()
{
	return	confirm("Please confirm that you really want to give / sell this planet.");
}


function	confirmAbandon()
{
	return	confirm('You are about the abandon this planet.\nPlease confirm.');
}

function	confirmCancelAbandon()
{
	return	confirm('You were about the abandon this planet.\nPlease confirm you want to cancel this action.');
}

function	confirmCancelSale()
{
	return	confirm('You are about to cancel the sale of this planet.\nPlease confirm.');
}

function	confirmDestroy()
{
	return	confirm(
			'You are about to make this planet\'s wormhole go supernova.\n'
			+ 'The planet will be destroyed, and the people under your rule will be scared to\n'
			+ 'death because of it.\n'
			+ 'It will take the wormhole 4 hours to reach the point where it actually explodes.\n'
			+ 'Please confirm.'
		);
}

function	confirmCancelDestruction()
{
	return	confirm('You are about to cancel the planet\'s destruction\nPlease confirm.');
}
