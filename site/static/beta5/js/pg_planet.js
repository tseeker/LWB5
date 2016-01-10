var	pdUpdate;
var	orbit;
var	planet;
var	plId;
var	pdIsOwn;
var	miSel;
var	miDir = -1;
var	mlSel = -1;


//---------------------------------------------------------------------------
// PLANET LIST
//---------------------------------------------------------------------------

var	plUpdate;

function	drawPlanetList(data)
{
	var	cs = document.getElementById('oid').value;
	var	str, i, a, n, f;

	a = data.split('\n');
	n = parseInt(a.shift(),10);
	if	(n > 0)
	{
		str  = '';
		f = false;
		for	(i=0;i<n;i++)
		{
			var	d = a.shift().split('#');
			str += '<option value="' + d[0] + '"';
			if	(cs == d[0])
			{
				str += ' selected="selected"';
				f = true;
			}
			str += '>' + d[1] + '</option>';
		}
		str  = '<select name="id" onChange="submit();">' + (f ? '' : '<option>------</option>') + str + '</select>';
	}
	else
		str = '---';
	document.getElementById('psel').innerHTML = str;
	plUpdate = setTimeout('x_getPlanetList(drawPlanetList)', 600000);
}


//---------------------------------------------------------------------------
// GENERIC ORBIT OBJECT
//---------------------------------------------------------------------------

function	Orbit(type,id,x,y,orbit,name)
{
	this.type	= type;
	this.id		= id;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.name	= name;
	this.fleets	= null;
	this.spec	= null;
	this.drawAll	= Orbit_drawAll;
	this.draw	= Orbit_draw;
	this.drawCoords	= Orbit_drawCoords;
	this.drawFleets	= Orbit_drawFleetSummary;
}

function	Orbit_drawAll()
{
	if	(!this.spec)
		return
	this.spec.drawLayout();
	this.draw();
}

function	Orbit_draw()
{
	this.drawFleets();
	if	(!this.spec.draw)
		return
	this.spec.draw();
}

function	Orbit_drawCoords()
{
	return	'<b>'+this.x+','+this.y+'</b>,'+this.orbit;
}


//---------------------------------------------------------------------------
// NEBULA OBJECT
//---------------------------------------------------------------------------

function	Nebula(orbit,opacity)
{
	this.orbit = orbit;
	this.opacity = opacity;
	this.drawLayout = Nebula_drawLayout;
}


//---------------------------------------------------------------------------
// REMAINS OBJECT
//---------------------------------------------------------------------------

function	Remains(orbit)
{
	this.orbit = orbit;
	this.drawLayout = Remains_drawLayout;
}


//---------------------------------------------------------------------------
// PLANET OBJECT
//---------------------------------------------------------------------------

function	Planet(orbit)
{
	this.orbit = orbit;
	this.bq = new Array();
	this.sellForm = null;

	this.drawLayout = Planet_drawLayout;
	this.draw = Planet_draw;
	this.drawBuildQueue = Planet_drawQueue;
}

function	BQItem()
{
	this.selected = false;
}

function	parsePlanetData(planet, l)
{
	var	a = l.shift().split('#');
	planet.isOwn = (a.shift() == '1');
	planet.vacation = (a.shift() == '1');
	planet.protection = (a.shift() == '1');
	planet.turrets = a.shift();
	planet.population = a.shift();
	planet.tFactories = a.shift();
	planet.iFactories = a.shift();
	planet.mFactories = a.shift();
	planet.happiness = a.shift();
	planet.corruption = a.shift();
	planet.profit = a.shift();
	planet.tag = a.join('#');
	if	(!planet.isOwn)
		return;

	// Actions and capabilities
	a = l.shift().split('#');
	planet.cAction = a.shift();
	if	(planet.cAction == '0')
	{
		planet.canRename = (a.shift() == '1');
		planet.canSell = (a.shift() == '1');
		planet.canDestroy = (a.shift() == '1');
		planet.canAbandon = (a.shift() == '1');
	}
	else if	(planet.cAction == '1')
	{
		planet.time = a.shift();
		planet.sellId = a.shift();
		planet.sellName = a.shift();
	}
	else
		planet.time = a.shift();
	planet.caps = l.shift().split('#');

	// Build queue
	var	i = 0, obq = planet.bq;
	planet.bq = new Array();
	while	(l.length)
	{
		var	bqi = (i<obq.length) ? obq[i] : new BQItem();
		a = l.shift().split('#');
		bqi.type = a.shift();
		bqi.qt = a.shift();
		bqi.ttb = a.shift();
		bqi.cttb = a.shift();
		planet.bq.push(bqi);
		i ++;
	}
}

function	Planet_drawQueue()
{
	var	i, str;
	if	(!this.bq.length)
		return getEmptyBQ();

	str  = '<table cellspacing="0" cellpadding="0" class="bqueue">';
	str += drawBQHeader();

	for	(i=0;i<this.bq.length;i++)
	{
		str += '<tr class="bql">';
		str += '<td><input type="checkbox" name="qbsel" value="';
		str += i + '" onClick="orbit.spec.bq['+i+'].selected=!orbit.spec.bq['+i+'].selected; return true;"';
		if	(this.bq[i].selected)
			str += ' checked="checked"';
		str += '/></td>';
		str += '<td class="bqqt"><b>' + this.bq[i].qt + '</b></td>';
		str += '<td>' + getBQItemName(this.bq[i].type, (this.bq[i].qt > 1)) + '</td>';
		str += '<td class="bqqt"><b>' + this.bq[i].ttb + '</b>h</td>';
		str += '<td class="bqqt"><b>' + this.bq[i].cttb + '</b>h</td>';
		str += '</tr>';
	}

	str += '</table>';
	return	str;
}


//---------------------------------------------------------------------------
// PLANET SALE
//---------------------------------------------------------------------------

function	SellForm()
{
	this.mode = 0;
	this.player = '';
	this.price = 0;
	this.expires = 0;
	this.fleets = new Array();
	this.validate = SellForm_validate;
}

function	SellForm_validate()
{
	if	(this.mode < 2)
	{
		this.player = document.getElementById('pstarget').value;
		if	(this.player == '')
			return	1;
	}
	if	(this.mode != 0)
	{
		this.price = parseInt(document.getElementById('psprice').value, 10);
		if	(isNaN(this.price) || this.price <= 0)
			return	2;
	}
	if	(this.mode == 3 && this.expires == 0)
		return	3;

	return	0;
}


function	sellPlanet()
{
	x_getSellableFleets(orbit.id, gotFleetsList);
}


function	gotFleetsList(data)
{
	if	(data != "ERR#-1")
	{
		orbit.spec.sellForm = new SellForm();
		if	(data != "")
		{
			var	l = data.split('\n');
			while	(l.length)
			{
				var	a = l.shift().split('#');
				var	b = new Array();
				b.push(a.shift()); b.push(a.shift()); b.push(a.shift());
				b.push(a.shift()); b.push(a.shift()); b.push(a.shift());
				b.push(a.join('#')); b.push(false);
				orbit.spec.sellForm.fleets.push(b);
			}
		}
	}
	orbit.draw();
}

function	closeSaleForm()
{
	orbit.spec.sellForm = null;
	orbit.draw();
}

function	confirmSale()
{with(orbit.spec.sellForm){
	var	e = validate();
	if	(e)
	{
		saleError(e);
		return;
	}
	var	a = new Array();
	for	(e=0;e<fleets.length;e++)
		if	(fleets[e][7])
			a.push(fleets[e][0]);

	if	(!lockUpdate())
		return;

	if	(!userConfirmSale())
	{
		startUpdate();
		return;
	}

	x_planetSale(plId, mode, player, price, expires, a.join('#'), saleRequestSent);
}}

function	saleRequestSent(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		saleError(parseInt(data.substr(4), 10));
		startUpdate();
	}
	else
	{
		orbit.spec.sellForm = null;
		parseOrbitData(data);
	}
}

function	setSaleMode(m)
{
	orbit.spec.sellForm.mode = m;
	drawSaleForm();
}

function	removeSaleOffer()
{
	if	(!lockUpdate())
		return;
	if	(!confirmCancelSale())
	{
		startUpdate();
		return;
	}
	x_cancelSale(plId, plExecuteOk);
}


//---------------------------------------------------------------------------
// PAGE MANAGEMENT & DATA PARSING
//---------------------------------------------------------------------------

function	lockUpdate()
{
	if	(!pdUpdate)
		return	false;
	var	p = pdUpdate;
	pdUpdate = null;
	clearTimeout(p);
	return	true;
}

function	startUpdate()
{
	if	(pdUpdate)
		return;
	pdUpdate = setTimeout('x_getPlanetData(plId, parseOrbitData)', 60000);
}

function initPlanetPage() {
	plId = document.getElementById('oid').value;
	x_getPlanetList(drawPlanetList);
	x_getPlanetData(plId, parseOrbitData);
}

function	parseOrbitData(data)
{
	var	l = data.split('\n');
	var	a = l.shift().split('#');
	var	t = parseInt(a.shift());

	if	(orbit && orbit.type == t)
	{
		orbit.fleets = l.shift().split('#');
		if	(t == 0)
		{
			a.splice(0,4);
			orbit.name = a.join('#'); 
			parsePlanetData(orbit.spec, l);
		}
		orbit.draw();
	}
	else
	{
		orbit = new Orbit(t, a.shift(), a.shift(), a.shift(), a.shift(), a.join('#'));
		orbit.fleets = l.shift().split('#');
		if	(t >= 2)
			orbit.spec = new Nebula(orbit, t - 1);
		else if	(t == 1)
			orbit.spec = new Remains(orbit);
		else
		{
			orbit.spec = new Planet(orbit);
			parsePlanetData(orbit.spec, l);
		}
		orbit.drawAll();
	}
	startUpdate();
}


//---------------------------------------------------------------------------
// ACTIONS
//---------------------------------------------------------------------------

function	renamePlanet()
{
	if	(!lockUpdate())
		return;
	var	nn = promptNewName();
	if	(typeof nn == 'object' && !nn)
	{
		startUpdate();
		return;
	}
	x_rename(plId, nn, plExecuteOk);
}

function	factAction(ta, tf)
{
	var	n, str;
	if	(!lockUpdate())
		return;

	str = (tf == 1) ? 'mfc' : 'ifc';
	n = parseInt(document.getElementById(str).value, 10);
	if	(isNaN(n) || n <= 0)
	{
		plError(3);
		startUpdate();
		return;
	}
	if	(!factoryConfirm(ta, tf, n))
	{
		startUpdate();
		return;
	}
	x_factoryAction(plId, ta, n, tf, plExecuteOk);
}

function	addToQueue()
{
	if	(!lockUpdate())
		return;

	if	(mlSel == -1)
	{
		plError(1);
		startUpdate();
		return;
	}

	var	n = parseInt(document.getElementById('bwfq').value, 10);
	if	(isNaN(n) || n <= 0)
	{
		plError(2);
		startUpdate();
		return;
	}
	x_addToQueue(plId, n, mlSel, plExecuteOk);
}

function	getSelectedItems()
{
	var	j, s = new Array();
	for	(j=0;j<orbit.spec.bq.length;j++)
	{
		if	(orbit.spec.bq[j].selected)
			s.push(j);
	}
	return	s;
}

function	replaceItems()
{
	if	(!lockUpdate())
		return;

	if	(mlSel == -1)
	{
		plError(1);
		startUpdate();
		return;
	}

	var	n = parseInt(document.getElementById('bwfq').value, 10);
	if	(isNaN(n) || n <= 0)
	{
		plError(4);
		startUpdate();
		return;
	}

	var	sel = getSelectedItems();
	if	(sel.length == 0)
	{
		plError(5);
		startUpdate();
		return;
	}

	x_replaceItems(plId, sel.join('#'), n, mlSel, plExecuteOk);
}

function	moveItems(dir)
{
	if	(!lockUpdate())
		return;

	var	sel = getSelectedItems();
	if	(!sel.length)
	{
		plError(5);
		startUpdate();
		return;
	}

	var	n, ts = '!' + sel.join('!') + '!';
	if	(dir == 1)
	{
		if	(ts.indexOf('!0!') != -1)
		{
			plError(6);
			startUpdate();
			return;
		}
	}
	else
	{
		n = orbit.spec.bq.length - 1;
		if	(ts.indexOf('!' + n + '!') != -1)
		{
			plError(7);
			startUpdate();
			return;
		}
	}
	miSel = sel;
	miDir = dir;
	x_moveItems(plId, sel.join('#'), dir, plExecuteOk);
}

function	deleteItems()
{
	if	(!lockUpdate())
		return;
	var	sel = getSelectedItems();
	if	(sel.length == 0)
	{
		plError(5);
		startUpdate();
		return;
	}
	x_deleteItems(plId, sel.join('#'), plExecuteOk);
}

function	flushQueue()
{
	if	(!lockUpdate())
		return;
	var	c = alertFlush();
	if	(!c)
	{
		startUpdate();
		return;
	}
	x_flushQueue(plId, plExecuteOk);
}

function	moveSelection()
{
	var	i, j, s, ni;
	s = '';
	for	(i=0;i<miSel.length;i++)
	{
		j = parseInt(miSel[i], 10);
		ni = j + (miDir ? -1 : 1);
		if	(s.indexOf('!'+j+'!') == -1)
			orbit.spec.bq[j].selected = false;
		orbit.spec.bq[ni].selected = true;
		s += '!'+ni+'!';
	}
}

function	plExecuteOk(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		plError(parseInt(data.substr(4), 10));
		if	(data.substr(4) == "-1")
		{
			history.go(0);
			return;
		}
		startUpdate();
	}
	else
	{
		if	(miDir != -1)
		{
			moveSelection();
			miDir = -1;
		}
		updateHeader();
		parseOrbitData(data);
	}
}

function	abandonPlanet()
{
	if	(!lockUpdate())
		return;
	if	(!confirmAbandon())
	{
		startUpdate();
		return;
	}
	x_abandon(plId, plExecuteOk);
}

function	cancelAbandon()
{
	if	(!lockUpdate())
		return;
	if	(!confirmCancelAbandon())
	{
		startUpdate();
		return;
	}
	x_cancelAbandon(plId, plExecuteOk);
}

function	destroyPlanet()
{
	if	(!lockUpdate())
		return;
	if	(!confirmDestroy())
	{
		startUpdate();
		return;
	}
	x_blowItUp(plId, plExecuteOk);
}

function	cancelDestruction()
{
	if	(!lockUpdate())
		return;
	if	(!confirmCancelDestruction())
	{
		startUpdate();
		return;
	}
	x_cancelDestruction(plId, plExecuteOk);
}

function	destroyTurrets()
{
	if	(!lockUpdate())
		return;
	var	nn = promptDestroyTurrets();
	if	(typeof nn == 'object' && !nn)
	{
		startUpdate();
		return;
	}
	x_destroyTurrets(plId, nn, plExecuteOk);
}
