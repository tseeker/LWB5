var	autoUpdate, autoLock, onMainPage = false;
var	allies, fleets, locations, players, myself;
var	fAutoId = 0;
var	listLocations, listMode, fDispMode, alliesMode;
var	sType, sText;
var	ppList, apList, opList, mfList, wfList;
var	fSel, fDisp, splitParam;
var	orderFleets, moveTo, moveToLoc, orderMode, waitTime, selCanHS, guessOrders, gDest, gWait, sMovex, sMovey;
var	dMapX, dMapY, newDest, map, loadingMap = false, readingMoveLocation = false;
var	mapCType, mapParm;
var	trajectories, lTraj, trajLock;
var	fleetTrajectory, sales;
var	mapRemember = false;
var	mFleetSelectable, mFleetSelected;
var	wFleetSelectable, wFleetSelected;


function	Ally(id, isMe, fSpace, gSpace, cHaul, bHaul, gaPop, canSell)
{
	this.id		= id;
	this.isMe	= isMe;
	this.gSpace	= gSpace;
	this.fSpace	= fSpace;
	this.cHaul	= cHaul;
	this.bHaul	= bHaul;
	this.gaPop	= gaPop;
	this.canSell	= canSell;

	this.getName	= Ally_getName;

	this.planets	= new Array();
	this.fleets	= new Array();
	this.gasAt	= new Hashtable();

	if	(isMe)
		myself = this;
}

function	Ally_getName()
{
	return	players.get(this.id);
}


function	Fleet(id, owner, gaships, fighters, cruisers, bcruisers, power, mode, upkeep, name)
{
	this.id		= (id == "" ? ("a" + (++fAutoId)) : id);
	this.owner	= owner;
	this.ships	= [gaships,fighters,cruisers,bcruisers];
	this.power	= power;
	this.upkeep	= upkeep;
	this.mode	= mode;
	this.name	= name;
	this.selected	= false;

	this.orders	= null;
}

function	FLocation(id, canMove)
{
	this.oType = 0;
	this.locId = id;
	this.loc = null;
	this.canMove = canMove;
}


function	FMove(to, from, now, eta, hs, reroute)
{
	this.oType = 1;
	this.fromId = from; this.from = null;
	this.toId = to; this.to = null;
	this.curId = now; this.cur = null;
	this.eta = eta; this.wait = null;
	this.hyperspace = (hs == '1');
	this.reroute = parseInt(reroute,10);
}


function	FWait(loc, from, left, spent, minLoss, maxLoss)
{
	this.oType = 2;
	this.fromId = from; this.from = null;
	this.locId = loc; this.loc = null;
	this.spent = spent; this.left = left;
	this.minLoss = minLoss; this.maxLoss = maxLoss;
}


function	FOnSale(loc, sale, ib)
{
	this.oType = 3;
	this.locId = loc; this.loc = null;
	this.saleId = sale;
	this.inBundle = (ib == 1);
}


function	FSold(loc, sale, soldTo, timeLeft, ib)
{
	this.oType = 4;
	this.locId = loc; this.loc = null;
	this.soldTo = soldTo; this.timeLeft = timeLeft;
	this.saleId = sale;
	this.inBundle = (ib == 1);
}


function	FleetLocation(id, opacity, x, y, orbit, name)
{
	this.id		= id;
	this.opacity	= opacity;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.name	= name;

	this.fleets	= new Array();
	this.details	= null;

	this.updateSelection = function () {
		var e = document.getElementById('fsel-p-' + this.id);
		if (!e) {
			return;
		}
		if (this.selectableFleets == 0) {
			e.innerHTML = '&nbsp;';
			return;
		}
		e.innerHTML = '<input type="checkbox" name="psel-all-' + this.id + '" id="psel-all-' + this.id
			+ '" onclick="locations.get(' + this.id + ').selectAll();"'
			+ (this.selectableFleets == this.selectedFleets ? ' checked="checked"' : '')
			+ ' />';
	};

	this.selectAll = function () {
		if (autoLock) {
			setTimeout('locations.get(' + this.id + ').selectAll()', 500);
			return;
		}
		autoLock = true;

		if (this.selectableFleets == 0) {
			autoLock = false;
			return;
		}
		for (var i in this.fleets) {
			var e = document.getElementById('fsel' + this.fleets[i].id);
			if (!e) {
				continue;
			}
			e.checked = this.fleets[i].selected = !(this.selectableFleets == this.selectedFleets);
		}
		if (this.selectableFleets == this.selectedFleets) {
			this.selectedFleets = 0;
		} else {
			this.selectedFleets = this.selectableFleets;
		}
		updateActions();

		autoLock = false;
	};
}

function PlanetDetails(owner, turrets, tPower, pop, vacation, protection, tag) {
	this.owner	= owner;
	this.turrets	= turrets;
	this.tPower	= tPower;
	this.pop	= pop;
	this.vacation	= (vacation == 1);
	this.protection	= (protection == 1);
	this.tag	= tag;
}

function	FilteredLocation(id)
{
	this.id		= id;
	this.fleetLocation	= locations.get(id);
	this.fleets	= new Array();
}


function MapSystem(x, y, opacity, prot) {
	this.x		= x;
	this.y		= y;
	this.opacity	= opacity;
	this.protection	= prot;
	this.locations	= new Array();
}

function	MapLocation(id, x, y, orbit, opacity, ally, name)
{
	this.id		= id;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.color	= parseInt(opacity, 10);
	if	(this.color != 0)
		this.color += 2;
	else
		this.color = parseInt(ally);
	this.name	= name;
}

function	WayPoint(x, y, orbit, eta, name)
{
	this.name	= name;
	this.coords	= "(" + x + "," + y + "," + orbit + ")";
	this.eta	= eta;
}

function	FleetTrajectory(fid, from, to, changed, left, hs, wait)
{
	this.fleetId	= fid;
	this.fleet	= fleets.get(fid);
	this.from	= from;
	this.to		= to;
	this.changed	= parseInt(changed,10);
	this.left	= parseInt(left,10);
	this.hyperspace	= (hs == '1');
	this.wait	= parseInt(wait,10);
	this.waypoints	= new Array();
}

function	FleetWayPoint(id, eta, sys, orb, op, name)
{
	this.id		= id;
	this.eta	= parseInt(eta,10);
	this.system	= sys;
	this.orbit	= orb;
	this.opacity	= op;
	this.name	= name;
}


function	FleetSale(loc)
{
	this.id		= loc;
	this.fleetLocation	= locations.get(loc);
	this.mode	= 0;
	this.player	= '';
	this.price	= 0;
	this.priceText	= '';
	this.expires	= 0;
	this.isValid	= false;
	this.error	= -1;
	this.fleets	= new Array();

	this.drawForm	= FleetSale_drawForm;
	this.update	= FleetSale_update;
	this.validate	= FleetSale_validate;
	this.setMode	= FleetSale_setMode;
	this.setExpire	= FleetSale_setExpire;
}

function	FleetSale_setMode(m)
{
	this.mode = m;
	this.update();
	this.validate();
}

function	FleetSale_setExpire(x)
{
	this.expires = x;
	this.validate();
}

function	FleetSale_update()
{
	var	id = 's' + this.id, c0 = 'sales.get('+this.id+').validate();return true', code = ' onChange="'+c0+'" onKeyUp="'+c0+'" onClick="'+c0+'" ';;
	switch	(this.mode)
	{
	case	0:
		if	(document.getElementById(id+'price'))
			document.getElementById(id+'pril').innerHTML = document.getElementById(id+'pri').innerHTML = '&nbsp;';
		if	(!document.getElementById(id+'target'))
		{
			document.getElementById(id+'tar').innerHTML = '<input type="text" name="'+id+'target" id="'+id+'target"'+code+' />';
			document.getElementById(id+'tarl').innerHTML = '<label for="'+id+'target">' + salesText[0] + '</label>';
		}
		document.getElementById(id+'target').value = this.player;
		break;

	case	1:
		if	(!document.getElementById(id+'price'))
			document.getElementById(id+'pri').innerHTML = '<input type="text" name="'+id+'price" id="'+id+'price"'+code+' />';
		if	(!document.getElementById(id+'target'))
		{
			document.getElementById(id+'tar').innerHTML = '<input type="text" name="'+id+'target" id="'+id+'target"'+code+' />';
			document.getElementById(id+'tarl').innerHTML = '<label for="'+id+'target">' + salesText[0] + '</label>';
		}
		document.getElementById(id+'pril').innerHTML = '<label for="'+id+'price">' + salesText[1] + '</label>';
		document.getElementById(id+'target').value = this.player;
		document.getElementById(id+'price').value = this.priceText;
		break;

	case	2:
	case	3:
		if	(!document.getElementById(id+'price'))
			document.getElementById(id+'pri').innerHTML = '<input type="text" name="'+id+'price" id="'+id+'price"'+code+' />';
		if	(document.getElementById(id+'target'))
			document.getElementById(id+'tar').innerHTML = document.getElementById(id+'tarl').innerHTML = '&nbsp;';
		document.getElementById(id+'pril').innerHTML = '<label for="'+id+'price">' + salesText[this.mode-1] + '</label>';
		document.getElementById(id+'price').value = this.priceText;
		break;
	}
}

function	FleetSale_validate()
{
	this.isValid = false;
	this.error = -1;

	if	(document.getElementById('s'+this.id+'target'))
		this.player = document.getElementById('s'+this.id+'target').value;
	if	(document.getElementById('s'+this.id+'price'))
	{
		this.priceText = document.getElementById('s'+this.id+'price').value;
		this.price = parseInt(this.priceText, 10);
	}

	if	(this.mode < 2 && this.player == '')
	{
		this.error = 1;
		drawSellLinks(false);
		return;
	}
	if	(this.mode != 0 && (isNaN(this.price) || this.price <= 0))
	{
		this.error = 2;
		drawSellLinks(false);
		return;
	}
	if	(this.mode == 3 && this.expires == 0)
	{
		this.error = 3;
		drawSellLinks(false);
		return;
	}

	this.isValid = true;
	checkAllSales();
}


function	getTrajectory(fleet, to)
{
	var	fLoc = (fleet.orders.oType != 1 ? fleet.orders.loc : fleet.orders.cur);
	var	key = fLoc.id + ';' + ((fleet.ships[2] != 0 || (fleet.ships[2] == 0 && fleet.ships[3] == 0)) ? 1 : 0) + ';' + fleet.owner + ';' + to.id;
	if	(trajectories.containsKey(key))
		return	trajectories.get(key);
	if	(('#' + lTraj.join('#') + '#').indexOf('#' + key + '#') == -1)
	{
		lTraj.push(key);
		x_getTrajectory(key, gotTrajectory);
	}
	return	null;
}

function	gotTrajectory(data)
{
	if	(data == "")
		return;

	while	(trajLock);
	trajLock = true;

	var	l = data.split('\n');
	var	t = new Array(), key = l.shift(), a, w, eta = 0;

	while	(l.length)
	{
		a = l.shift().split('#');
		w = new WayPoint(a.shift(), a.shift(), a.shift(), (eta += parseInt(a.shift(), 10)), a.join('#'));
		t.push(w);
	}

	trajectories.put(key, t);

	t = new Array();
	for	(a=0;a<lTraj.length;a++)
		if	(lTraj[a] != key)
			t.push(lTraj[a]);
	lTraj = t;
	trajLock = false;
	updateTrajectories();
}


function	initFleets()
{
	fSel = new Array();
	trajectories = new Hashtable();
	lTraj = new Array();

	var	e = document.getElementById('finit');
	if	(!e)
		return;
	e = e.innerHTML;

	var	pm;
	if	(e.charAt(0) == '-')
	{
		pm = "parseMainData";
		e = e.substr(2, e.length - 2);
	}
	else
	{
		var i = 0, s = '';
		while	(e.charAt(i) != ' ')
		{
			s += e.charAt(i);
			i++;
		}
		i ++;
		e = e.substr(i, e.length - i);
		moveTo = parseInt(s, 10);
		pm = "parseMainDataMove";
	}

	map = new Hashtable();
	if	(e.indexOf('\n') == -1) {
		eval("x_getFleetsList("+pm+")");
	} else {
		eval(pm+"(e)");
	}
}


function	parseAllies(l, nAllies)
{
	var	i, a, o;

	allies = new Hashtable();
	for	(i=0;i<nAllies;i++)
	{
		a = l.shift().split('#');
		o = new Ally(a.shift(), (a.shift() == '1'), a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), (a.shift() == '0'));
		allies.put(o.id, o);
	}
}


function	parseFleets(l, nFleets)
{
	var	i, a, o, fs = '#' + fSel.join('#') + '#';

	fleets = new Hashtable();
	var p = new Array();
	for	(i=0;i<nFleets;i++)
	{
		a = l.shift().split('#');
		o = new Fleet(a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.join('#'));
		p.push(o.owner);
		a = l.shift().split('#');

		if	(o.mode < 2)
			o.orders = new FLocation(a[0],a[1]);
		else if	(o.mode == 2)
		{
			o.orders = new FMove(a[0],a[1],a[2],a[4],a[5],a[6]);
			if	(a[3] == 2)
			{
				a = l.shift().split('#');
				o.orders.wait = new FWait(a[0],a[1],a[2],a[3],a[4],a[5]);
				o.mode = a[6];
			}
			else
				o.mode = a[3];
		}
		else if	(o.mode == 3)
		{
			o.orders = new FWait(a[0],a[1],a[2],a[3],a[4],a[5]);
			o.mode = a[6];
		}
		else if	(o.mode == 4)
		{
			o.orders = new FOnSale(a[0], a[1], a[2]);
			o.mode = 0;
		}
		else if	(o.mode == 5)
		{
			o.orders = new FSold(a[0], a[1], a[2], a[3], a[4]);
			o.mode = 0;
		}

		o.selected = (fs.indexOf('#'+o.id+'#') != -1);
		fleets.put(o.id, o);
	}

	return	p;
}


function parsePlanets(l, nPlanets, pl) {
	var	i, a, b, o;

	locations = new Hashtable();
	for (i=0;i<nPlanets;i++) {
		a = l.shift().split('#');
		o = new FleetLocation(a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.join('#'));
		if (o.opacity == 0) {
			a = l.shift().split('#');
			o.details = new PlanetDetails(a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.shift(),
				a.join('#'));
			if (o.details.owner != '') {
				pl.push(o.details.owner);
				a = allies.get(o.details.owner);
				a.planets.push(o);
			}
		}
		locations.put(o.id, o);
	}
}


function	parsePlayers(l, pl)
{
	var	a,o,i,str,np;

	str = ""; np = 0;
	for	(i=0;i<pl.length;i++)
	{
		if	(str.indexOf('#'+pl[i]+'#') != -1)
			continue;
		np++;
		str += '#' + pl[i] + '#';
	}

	players = new Hashtable();
	for	(i=0;i<np;i++)
	{
		a = l.shift().split('#');
		players.put(a[0], a[1]);
	}
}


function	linkMainData()
{
	var	i, f, a, p, fl;

	fl = fleets.keys();
	for	(i=0;i<fl.length;i++)
	{
		f = fleets.get(fl[i]);

		a = allies.get(f.owner);
		if	(a)
			a.fleets.push(f);

		if	(f.orders.oType == 1)
		{
			f.orders.to = locations.get(f.orders.toId);
			f.orders.from = locations.get(f.orders.fromId);
			f.orders.cur = locations.get(f.orders.curId);
			if	(f.orders.wait)
			{
				f.orders.wait.from = f.orders.from;
				f.orders.wait.loc = f.orders.to;
			}
		}
		else if	(f.orders.oType == 2)
		{
			f.orders.loc = locations.get(f.orders.locId);
			f.orders.from = locations.get(f.orders.fromId);
		}
		else
		{
			p = locations.get(f.orders.locId);
			f.orders.loc = p;
			p.fleets.push(f);
			if	(a && p.details && p.details.owner != a.id)
			{
				var g;
				if	(a.gasAt.containsKey(p.id))
					g = a.gasAt.get(p.id);
				else
					g = 0;
				g += parseInt(f.ships[0], 10);
				a.gasAt.put(p.id, g);
			}
		}
	}
}


function	prepareUpdate()
{
	var	i,k;
	fSel = new Array();
	k = fleets.keys();
	for	(i=0;i<k.length;i++)
	{
		var f = fleets.get(k[i]);
		if	(f.selected)
			fSel.push(f.id);
	}
}


function	mainDataUpdate()
{
	if	(autoLock)
	{
		setTimeout('mainDataUpdate()', 500);
		return;
	}
	autoLock = true;

	prepareUpdate();
	x_getFleetsList(parseMainData);
}


// The infamous filtering function
function	applyFilters()
{
	var	i, keys, pln, flt;

	ppList = new Hashtable(); apList = new Hashtable(); opList = new Hashtable();
	mfList = new Array(); wfList = new Array();

	keys = locations.keys(); pln = new Array();
	for	(i=0;i<keys.length;i++)
	{
		var	p = locations.get(keys[i]);

		if	(	(	listLocations == 0
				||	listLocations == 1 && p.details && p.details.owner == myself.id
				||	listLocations == 2 && p.details && p.details.owner != myself.id && p.details.owner != ''
				||	listLocations == 3 && (!p.details || p.details.owner == '')
			) &&	(	sType != 1
				||	sType == 1 && (sText == '' || p.name.toLowerCase().indexOf(sText.toLowerCase()) != -1)
				)
			)
			pln.push(keys[i]);
	}

	keys = fleets.keys(); flt = new Array();
	for	(i=0;i<keys.length;i++)
	{
		var	f = fleets.get(keys[i]);

		if	(	(listMode == 0 || listMode - 1 == f.mode)
			&&	(	fDispMode == 0
				||	fDispMode == 1 && f.orders.oType == 0 && f.orders.canMove == 'Y'
				||	fDispMode == 2 && f.orders.oType == 0 && f.orders.canMove != 'Y'
				||	fDispMode > 2 && fDispMode - 2 == f.orders.oType
			) &&	(	alliesMode == 0
				||	alliesMode == 1 && f.owner == myself.id
				||	alliesMode == 2 && f.owner != myself.id && allies.get(f.owner)
				||	alliesMode == 3 && !allies.get(f.owner)
			) &&	(	sType == 1
				||	sType == 0 && (sText == '' || f.name.toLowerCase().indexOf(sText.toLowerCase()) != -1)
				||	sType == 2 && (sText == '' || players.get(f.owner).toLowerCase().indexOf(sText.toLowerCase()) != -1)
				)
			)
			flt.push(keys[i]);
	}

	if	(fDispMode < 3)
	{
		if	(listLocations < 2)
			for	(i=0;i<myself.planets.length;i++)
				ppList.put(myself.planets[i].id, new FilteredLocation(myself.planets[i].id));
		if	(listLocations == 0 || listLocations == 2)
		{
			keys = allies.keys();
			for	(i=0;i<keys.length;i++)
			{
				var	j, p=allies.get(keys[i]);
				if	(p.isMe)
					continue;
				for	(j=0;j<p.planets.length;j++)
					apList.put(p.planets[j].id, new FilteredLocation(p.planets[j].id));
			}
		}
	}

	var	pStr = '#'+pln.join('#')+'#';
	for	(i=0;i<flt.length;i++)
	{
		var	p, l, f = fleets.get(flt[i]);

		switch	(f.orders.oType)
		{
		case	0:
		case	3:
		case	4:
			p = f.orders.loc;
			if	(pStr.indexOf('#' + p.id + '#') == -1)
				continue;

			if	(!p.details || p.details.owner == "")
				l = opList;
			else if	(p.details && p.details.owner != myself.id)
				l = apList;
			else
				l = ppList;

			p = l.get(f.orders.locId);
			if	(!p)
			{
				p = new FilteredLocation(f.orders.locId);
				l.put(p.id, p);
			}
			p.fleets.push(f.id);
			break;

		case	1:
			if	(	pStr.indexOf('#' + f.orders.toId + '#') == -1
				&&	pStr.indexOf('#' + f.orders.fromId + '#') == -1
				&&	pStr.indexOf('#' + f.orders.curId + '#') == -1
				)
				continue;
			mfList.push(f);
			break;

		case	2:
			if	(	pStr.indexOf('#' + f.orders.locId + '#') == -1
				&&	pStr.indexOf('#' + f.orders.fromId + '#') == -1
				)
				continue;
			wfList.push(f);
			break;
		}
	}
}


function	changeFilters(fVar, fVal)
{
	var	r;
	eval("r = ("+fVar+"==fVal);");
	if	(r)
		return;

	if	(autoLock)
	{
		setTimeout('changeFilters(fVar, fVal)', 500);
		return;
	}
	lockUpdate();

	x_setFilter(fVar, fVal, function(){ });
	eval(fVar + '=fVal;');
	applyFilters();
	drawMainContents();

	unlockUpdate();
}


function selectFleet(id) {
	if (autoLock) {
		setTimeout('selectFleet('+id+')', 500);
		return;
	}
	autoLock = true;

	var	f = fleets.get(id);
	f.selected = !f.selected;
	if (f.orders.oType == 1) {
		mFleetSelected += (f.selected ? 1 : -1);
		updateMovingSelection();
	} else if (f.orders.oType == 2) {
		wFleetSelected += (f.selected ? 1 : -1);
		updateWaitingSelection();
	} else {
		var loc = f.orders.loc;
		loc.selectedFleets += (f.selected ? 1 : -1);
		loc.updateSelection();
	}
	updateActions();

	autoLock = false;
}

function updateWaitingSelection () {
	var e = document.getElementById('f-hssb-sel');
	if (!e) {
		return;
	}
	if (wFleetSelectable == 0) {
		e.innerHTML = '&nbsp;';
		return;
	}
	e.innerHTML = '<input type="checkbox" name="wsel-all" id="wsel-all" onclick="selectAllWaitingFleets()"'
		+ (wFleetSelectable == wFleetSelected ? ' checked="checked"' : '') + ' />';
}

function selectAllWaitingFleets () {
	if (autoLock) {
		setTimeout('selectAllWaitingFleets()', 500);
		return;
	}
	autoLock = true;

	if (wFleetSelectable == 0) {
		autoLock = false;
		return;
	}
	for (var i in wfList) {
		var e = document.getElementById('fsel' + wfList[i].id);
		if (!e) {
			continue;
		}
		e.checked = wfList[i].selected = !(wFleetSelectable == wFleetSelected);
	}
	if (wFleetSelectable == wFleetSelected) {
		wFleetSelected = 0;
	} else {
		wFleetSelected = wFleetSelectable;
	}
	updateActions();

	autoLock = false;
}

function updateMovingSelection () {
	var e = document.getElementById('f-move-sel');
	if (!e) {
		return;
	}
	if (mFleetSelectable == 0) {
		e.innerHTML = '&nbsp;';
		return;
	}
	e.innerHTML = '<input type="checkbox" name="msel-all" id="msel-all" onclick="selectAllMovingFleets()"'
		+ (mFleetSelectable == mFleetSelected ? ' checked="checked"' : '') + ' />';
}

function selectAllMovingFleets () {
	if (autoLock) {
		setTimeout('selectAllMovingFleets()', 500);
		return;
	}
	autoLock = true;

	if (mFleetSelectable == 0) {
		autoLock = false;
		return;
	}
	for (var i in mfList) {
		var e = document.getElementById('fsel' + mfList[i].id);
		if (!e) {
			continue;
		}
		e.checked = mfList[i].selected = !(mFleetSelectable == mFleetSelected);
	}
	if (mFleetSelectable == mFleetSelected) {
		mFleetSelected = 0;
	} else {
		mFleetSelected = mFleetSelectable;
	}
	updateActions();

	autoLock = false;
}



function	updateActions()
{
	var	cRen = false, cTraj = true, cMove = true, cSwitch = true,
		cMerge = true, cDisband = true, cSell = myself.canSell, cCancelSale = myself.canSell;
	var	i, n, ns;

	for	(i=n=ns=0;i<fDisp.length;i++)
	{
		var f = fDisp[i];
		if	(!f.selected)
			continue;
		ns += parseInt(f.ships[0], 10) + parseInt(f.ships[1], 10) + parseInt(f.ships[2], 10) + parseInt(f.ships[3], 10);
		cRen = true;
		cTraj = cTraj && f.orders.oType == 1;
		cMove = cMove && (f.orders.oType < 3 && f.orders.oType != 0 || f.orders.canMove == 'Y');
		cMerge = cMerge && !(f.orders.oType == 3 || f.orders.oType == 4) && (f.orders.oType != 0 || f.orders.canMove == 'Y');
		cSwitch = cSwitch && !(f.orders.oType == 3 || f.orders.oType == 4) && (f.orders.oType != 1 && (
				!f.orders.loc.details || f.orders.loc.details && f.orders.loc.details.owner != f.owner
			) || f.orders.oType == 1 && (
				!f.orders.to.details || f.orders.to.details && f.orders.to.details.owner != f.owner
			));
		cDisband = cDisband && (f.owner == myself.id) && !(f.orders.oType == 3 || f.orders.oType == 4);
		cSell = cSell && (f.owner == myself.id) && (f.orders.oType == 0) && (f.mode == 0) && (f.orders.canMove == 'Y');
		cCancelSale = cCancelSale && (f.orders.oType == 3 || f.orders.oType == 4) && !f.orders.inBundle && (f.owner == myself.id);

		n++;
	}

	if	(!cRen)
	{
		document.getElementById('factions').innerHTML = actionText[0];
		return;
	}

	i = '<a href="#" onClick="clearSelection();return false">' + actionText[9] + '</a>'
		+ ' - <a href="#" onClick="renameSelected();return false">' + actionText[1] + '</a>';
	if	(cTraj && n == 1)
		i += ' - <a href="#" onClick="viewTrajectory();return false">' + actionText[8] + '</a>';
	if	(cMove)
		i += ' - <a href="#" onClick="moveSelected();return false">' + actionText[2] + '</a>';
	if	(cSwitch)
		i += ' - <a href="#" onClick="switchSelected();return false">' + actionText[3] + '</a>';
	if	(cMerge && n > 1)
		i += ' - <a href="#" onClick="mergeSelected();return false">' + actionText[5] + '</a>';
	if	(cMerge && n == 1 && ns > 1)
		i += ' - <a href="#" onClick="splitSelected();return false">' + actionText[4] + '</a>';
	if	(cSell)
		i += ' - <a href="#" onClick="sellSelected();return false">' + actionText[6] + '</a>';
	if	(cCancelSale)
	{
		if	(n == 1)
			i += ' - <a href="#" onClick="viewSaleSelected();return false">' + actionText[11] + '</a>';
		i += ' - <a href="#" onClick="cancelSaleSelected();return false">' + actionText[10] + '</a>';
	}
	if	(cDisband)
		i += ' - <a href="#" onClick="disbandSelected();return false">' + actionText[7] + '</a>';
	document.getElementById('factions').innerHTML = i;
}


function drawFleetLine(fleet,type,hasSel) {
	var	o = allies.get(fleet.owner);
	var	i, str = '<tr class="fl'+type+'">';
	var sc = '';
	if	(hasSel)
	{
		str += '<td class="fsel">';
		if	(o && (fleet.orders.oType != 3 && fleet.orders.oType != 4 || !fleet.orders.inBundle))
		{
			str += '<input type="checkbox" id="fsel'+fleet.id+'" name="fsel'+fleet.id+'" value="1" '
				+ (fleet.selected ? 'checked="checked" ' : '')
				+ ' onClick="selectFleet('+fleet.id+');return true" />';
			fDisp.push(fleet);
			sc = ' onclick="with (document.getElementById(\'fsel'+fleet.id+'\')) { checked = !checked; };'
				+ 'selectFleet('+fleet.id+');return true" ';

			var loc = fleet.orders.loc;
			loc.selectableFleets ++;
			loc.selectedFleets += (fleet.selected ? 1 : 0);
		} else {
			str += '&nbsp;';
		}
		str += '</td>';
	}
	str += '<td';
	if (myself.id != fleet.owner) {
		str += '><a href="message?a=c&ct=0&id=' + fleet.owner + '">';
	} else {
		str += sc + '>';
	}
	str += players.get(fleet.owner);
	str += '</td><td';
	if	(o)
		str += ' onClick="renameFleet(' + fleet.id + ');return false"';
	str += '>' + fleet.name + '</td><td class="fhs"' + sc + '>';

	if	(o)
	{
		var	used = fleet.ships[0] * o.gSpace + fleet.ships[1] * o.fSpace;
		var	haul = fleet.ships[2] * o.cHaul + fleet.ships[3] * o.bHaul;
		if	(haul == 0)
			str += 'N/A';
		else
		{
			var hp = Math.round(used * 100 / haul);
			str += (hp > 200) ? '&gt;200%' : (hp + '%');
		}
	}
	else
		str += '&nbsp;';
	str += '</td>';

	for	(i=0;i<4;i++)
		str += '<td class="fshp"' + sc + '>'+formatNumber(fleet.ships[i])+'</td>';

	str += '<td class="fpwr"' + sc + '>'+formatNumber(fleet.power)+'</td><td class="fstat"' + sc + '>';
	if	(fleet.orders.oType == 0)
		str += (fleet.mode == 1 ? statusText[0] : statusText[1]) + ', ' + (fleet.orders.canMove == 'Y' ? statusText[2] : statusText[3]);
	else if	(fleet.orders.oType == 3)
		str += statusText[4] + (fleet.orders.inBundle ? statusText[6] : '');
	else if	(fleet.orders.oType == 4)
		str += statusText[5];
	str += '</td></tr>';

	return	str;
}


function	drawMovingFleetLine(fleet, hasSel)
{
	var	o = allies.get(fleet.owner);
	var	i, str = '<tr><td class="flt"><table class="mfltl">';
	var sc = '';
	str += '<tr class="fl'+(fleet.owner==myself.id?'own':'ally')+'">';

	if (hasSel) {
		str += '<td class="fsel" rowspan="2"><input type="checkbox" id="fsel' + fleet.id + '" name="fsel'
			+ fleet.id + '" value="1" ' + (fleet.selected ? 'checked="checked" ' : '')
			+ ' onClick="selectFleet(' + fleet.id + ');return true" /></td>';
		mFleetSelectable ++;
		mFleetSelected += (fleet.selected ? 1 : 0);
		fDisp.push(fleet);
		sc = ' onclick="with (document.getElementById(\'fsel'+fleet.id+'\')) { checked = !checked; };'
			+ 'selectFleet('+fleet.id+');return true" ';
	}

	str += '<td class="fown" rowspan="2"';
	if (myself.id != fleet.owner) {
		str += '><a href="message?a=c&ct=0&id=' + fleet.owner + '">';
	} else {
		str += sc + '>';
	}
	str += players.get(fleet.owner);
	if	(myself.id != fleet.owner)
		str += '</a>';
	str += '</td><td onClick="renameFleet(' + fleet.id + ');return false"';
	str += ' class="fname" rowspan="2">' + fleet.name + '</td><td class="fhs"' + sc + '>';

	var	used = fleet.ships[0] * o.gSpace + fleet.ships[1] * o.fSpace;
	var	haul = fleet.ships[2] * o.cHaul + fleet.ships[3] * o.bHaul;
	if	(haul == 0)
		str += 'N/A';
	else
	{
		var hp = Math.round(used * 100 / haul);
		str += (hp > 200) ? '&gt;200%' : (hp + '%');
	}
	str += '</td>';

	for	(i=0;i<4;i++)
		str += '<td class="fshp"' + sc + '>'+formatNumber(fleet.ships[i])+'</td>';
	str += '<td class="fpwr"' + sc + '>'+formatNumber(fleet.power)+'</td>';
	str += '<td class="fstat"' + sc + '>' + (fleet.mode == 1 ? statusText[0] : statusText[1]) + '</td></tr>';

	str += '<tr class="fl'+(fleet.owner==myself.id?'own':'ally')+'">';
	str += '<td class="fdest" colspan="2"><a href="planet?id=' + fleet.orders.curId + '">' + fleet.orders.cur.name + '</a>';
	if (fleet.orders.cur.details && fleet.orders.cur.details.tag != '') {
		str += ' [' + fleet.orders.cur.details.tag + ']';
	}
	str += '</td><td class="floc" colspan="2"><a href="planet?id=' + fleet.orders.toId + '">' + fleet.orders.to.name + '</a>';
	if (fleet.orders.to.details && fleet.orders.to.details.tag != '') {
		str += ' [' + fleet.orders.to.details.tag + ']';
	}
	str += '</td><td class="feta"' + sc + '>' + fleet.orders.eta + ' min</td><td class="fstd" colspan="2"' + sc + '>';
	if	(fleet.orders.wait)
	{
		str += fleet.orders.wait.left + 'h (';
		if	(fleet.orders.wait.maxLoss == 0)
			str += '0%';
		else
			str += fleet.orders.wait.minLoss + '% - ' + fleet.orders.wait.maxLoss + '%';
		str += ')';
	}
	else
		str += 'N/A';
	str += '</td></tr></table></td></tr>';

	return	str;
}


function	drawWaitingFleetLine(fleet,hasSel)
{
	var	o = allies.get(fleet.owner);
	var	i, str = '<tr><td class="flt"><table class="wfltl">';
	var sc = '';
	str += '<tr class="fl'+(fleet.owner==myself.id?'own':'ally')+'">';

	if (hasSel) {
		str += '<td class="fsel" rowspan="2"><input type="checkbox" name="fsel' + fleet.id
			+ '" id="fsel'+fleet.id+'" value="1" ' + (fleet.selected ? 'checked="checked" ' : '')
			+ ' onClick="selectFleet(' + fleet.id + ');return true" /></td>';
		wFleetSelectable ++;
		wFleetSelected += (fleet.selected ? 1 : 0);
		fDisp.push(fleet);
		sc = ' onclick="with (document.getElementById(\'fsel'+fleet.id+'\')) { checked = !checked; };'
			+ 'selectFleet('+fleet.id+');return true" ';
	}

	str += '<td class="fown" rowspan="2"';
	if (myself.id != fleet.owner) {
		str += '><a href="message?a=c&ct=0&id=' + fleet.owner + '">';
	} else {
		str += sc + '>';
	}
	str += players.get(fleet.owner);
	if	(myself.id != fleet.owner)
		str += '</a>';
	str += '</td><td onClick="renameFleet(' + fleet.id + ');return false"';
	str += ' class="fname" rowspan="2">' + fleet.name + '</td><td ' + sc + 'class="fhs">';

	var	used = fleet.ships[0] * o.gSpace + fleet.ships[1] * o.fSpace;
	var	haul = fleet.ships[2] * o.cHaul + fleet.ships[3] * o.bHaul;
	if	(haul == 0)
		str += 'N/A';
	else
	{
		var hp = Math.round(used * 100 / haul);
		str += (hp > 200) ? '&gt;200%' : (hp + '%');
	}
	str += '</td>';

	for	(i=0;i<4;i++)
		str += '<td' + sc + ' class="fshp">'+formatNumber(fleet.ships[i])+'</td>';
	str += '<td' + sc + ' class="fpwr">'+formatNumber(fleet.power)+'</td>';
	str += '<td' + sc + ' class="fstat">' + (fleet.mode == 1 ? statusText[0] : statusText[1]) + '</td></tr>';

	str += '<tr class="fl'+(fleet.owner==myself.id?'own':'ally')+'">'
		+ '<td class="floc" colspan="2"><a href="planet?id=' + fleet.orders.locId + '">' + fleet.orders.loc.name + '</a>';
	if (fleet.orders.loc.details && fleet.orders.loc.details.tag != '') {
		str += ' [' + fleet.orders.loc.details.tag + ']';
	}
	str += '</td><td' + sc + ' class="ftime" colspan="2">' + fleet.orders.spent + 'h</td><td' + sc + ' class="ftime" colspan="2">'
		+ fleet.orders.left + 'h</td><td' + sc + ' class="floss">';
	if	(fleet.orders.maxLoss == 0)
		str += '0%';
	else
		str += fleet.orders.minLoss + '% - ' + fleet.orders.maxLoss + '%';
	str += '</td></tr></table></td></tr>';

	return	str;
}


function	drawMainContents()
{
	fDisp = new Array();

	if	(ppList.size() == 0 && apList.size() == 0 && opList.size() == 0 && mfList.length == 0 && wfList.length == 0)
	{
		document.getElementById('fmain').innerHTML = '<center>'+noFleetsFound+'</center>';
		return;
	}

	var	i,sz,keys,j,p,f,str = '';

	if	((sz = ppList.size()) != 0)
	{
		str += '<h1>'+sectionTitles[0]+'</h1>';
		keys = ppList.keys();
		keys.sort(sortPPList);
		for	(i=0;i<sz;i++)
			str += drawPlanetBox(ppList.get(keys[i]));
	}

	if	((sz = apList.size()) != 0)
	{
		str += '<h1>'+sectionTitles[1]+'</h1>';
		keys = apList.keys();
		keys.sort(sortAPList);
		for	(i=0;i<sz;i++)
			str += drawPlanetBox(apList.get(keys[i]));
	}

	if	((sz = opList.size()) != 0)
	{
		str += '<h1>'+sectionTitles[2]+'</h1>';
		keys = opList.keys();
		keys.sort(sortOPList);
		for	(i=0;i<sz;i++)
			str += drawPlanetBox(opList.get(keys[i]));
	}

	if (mfList.length != 0) {
		mFleetSelectable = mFleetSelected = 0;
		str += '<h1>'+sectionTitles[3]+'</h1>';
		str += '<table class="planet"><tr><td class="flt">' + drawMovingHeader(true) + '</td></tr>';
		mfList.sort(sortMovingFleets);
		for (j=0;j<mfList.length;j++) {
			str += drawMovingFleetLine(mfList[j], true);
		}
		str += '</table>';
	}

	if (wfList.length != 0) {
		wFleetSelectable = wFleetSelected = 0;
		str += '<h1>'+sectionTitles[4]+'</h1>';
		str += '<table class="planet"><tr><td class="flt">' + drawWaitingHeader(true) + '</td></tr>';
		wfList.sort(sortWaitingFleets);
		for (j=0;j<wfList.length;j++) {
			str += drawWaitingFleetLine(wfList[j], true);
		}
		str += '</table>';
	}

	document.getElementById('fmain').innerHTML = str;

	keys = locations.keys();
	for (i=0;i<keys.length;i++) {
		locations.get(keys[i]).updateSelection();
	}
	updateMovingSelection();
	updateWaitingSelection();

	updateActions();
}


function	renameFleet(id)
{
	if	(autoLock)
	{
		setTimeout('renameFleet(id)', 500);
		return;
	}
	lockUpdate();

	var	str = "";
	while	(str == "")
	{
		str = getNewFleetName(false);
		if	(typeof str == 'object' && !str)
		{
			unlockUpdate();
			return;
		}
		if	(str == "")
			alertFleetName(0);
		else if	(str.length > 64)
		{
			alertFleetName(1);
			str = "";
		}
		else if	(str.length < 3)
		{
			alertFleetName(2);
			str = "";
		}
	}

	x_renameFleets(id, str, renameCallback);
}


function	renameSelected()
{
	if	(autoLock)
	{
		setTimeout('renameSelected()', 500);
		return;
	}
	lockUpdate();

	var	str = "";
	while	(str == "")
	{
		str = getNewFleetName(false);
		if	(typeof str == 'object' && !str)
		{
			unlockUpdate();
			return;
		}
		if	(str == "")
			alertFleetName(0);
		else if	(str.length > 64)
		{
			alertFleetName(1);
			str = "";
		}
		else if	(str.length < 3)
		{
			alertFleetName(2);
			str = "";
		}
	}

	var	s = new Array();
	for	(i=0;i<fDisp.length;i++)
	{
		if	(fDisp[i].selected)
			s.push(fDisp[i].id);
	}
	x_renameFleets(s.join('#'), str, renameCallback);
}


function	renameCallback(str)
{
	if	(str.indexOf('ERR#') == 0)
	{
		var	l = str.split('#');
		alertFleetName(parseInt(l[1], 10));
		unlockUpdate();
	}
	else
	{
		prepareUpdate();
		parseMainData(str);
	}
}


function	mergeSelected()
{
	if	(autoLock)
	{
		setTimeout('mergeSelected()', 500);
		return;
	}
	lockUpdate();

	var	str = " ";
	while	(str == " ")
	{
		str = getMergedFleetName();
		if	(typeof str == 'object' && !str)
		{
			unlockUpdate();
			return;
		}
		if	(str.length > 64)
		{
			alertFleetMerge(0);
			str = " ";
		}
		else if	(str.length < 3 && str != '')
		{
			alertFleetMerge(1);
			str = " ";
		}
	}

	var	i, s = new Array();
	for	(i=0;i<fDisp.length;i++)
	{
		if	(fDisp[i].selected)
			s.push(fDisp[i].id);
	}
	x_mergeFleets(s.join('#'), str, mergeCallback);
}

function	mergeCallback(str)
{
	if	(str.indexOf('ERR#') == 0)
	{
		var	l = str.split('#');
		alertFleetMerge(parseInt(l[1], 10));
		unlockUpdate();
	}
	else
	{
		clearSelection();
		prepareUpdate();
		parseMainData(str);
	}
}


function	switchSelected()
{
	if	(autoLock)
	{
		setTimeout('switchSelected()', 500);
		return;
	}
	lockUpdate();

	var	s = new Array();
	for	(i=0;i<fDisp.length;i++)
	{
		if	(fDisp[i].selected)
			s.push(fDisp[i].id);
	}
	x_switchStatus(s.join('#'), switchCallback);
}

function	switchCallback(str)
{
	if	(str.indexOf('ERR#') == 0)
	{
		var	l = str.split('#');
		alertFleetSwitch(parseInt(l[1], 10));
		unlockUpdate();
	}
	else
	{
		clearSelection();
		prepareUpdate();
		parseMainData(str);
	}
}


function	disbandSelected()
{
	if	(autoLock)
	{
		setTimeout('disbandSelected()', 500);
		return;
	}
	lockUpdate();

	var	s = new Array();
	for	(i=0;i<fDisp.length;i++)
	{
		if	(fDisp[i].selected)
			s.push(fDisp[i].id);
	}

	if	(confirmDisband(s.length >1))
		x_disbandFleets(s.join('#'), disbandCallback);
	else
		unlockUpdate();
}

function	disbandCallback(str)
{
	if	(str.indexOf('ERR#') == 0)
	{
		var	l = str.split('#');
		alertFleetDisband(parseInt(l[1], 10));
		unlockUpdate();
	}
	else
	{
		prepareUpdate();
		parseMainData(str);
	}
}


function	splitSelected()
{
	if	(autoLock)
	{
		setTimeout('splitSelected()', 500);
		return;
	}
	lockUpdate();

	splitParam = new Array();
	for	(var i=0;i<fDisp.length;i++)
		if	(fDisp[i].selected)
		{
			splitParam[0] = fDisp[i];
			break;
		}

	splitParam[1] = splitParam[3] = splitParam[4] = splitParam[5] = splitParam[6] = 0;
	splitParam[2] = 1; splitParam[9] = '';
	onMainPage = false;
	drawSplitPage();
}

function	setSplitType(type)
{
	if	(type == splitParam[1])
		return;
	splitParam[1] = type;

	var	i, flds = ['sgas','sfgt','scru','sbcr'];
	for	(i=0;i<4;i++)
	{
		var	e = document.getElementById(flds[i]);
		if	(e)
			e.disabled = (type == 1);
	}

	if	(type == 1)
		updateAutoSplit();
	else
	{
		for	(i=0;i<4;i++)
		{
			var	e = document.getElementById(flds[i]);
			if	(e)
				e.value = splitParam[i+3];
		}
		updateSplitHaul();
		updateSplitOk();
	}
}

function	updateAutoSplit()
{
	var	ships = [0,0,0,0];
	var	o = allies.get(splitParam[0].owner);
	var	flds = ['sgas','sfgt','scru','sbcr'];
	var	i;

	for	(i=0;i<4;i++)
	{
		var	e;
		ships[i] = Math.floor(splitParam[0].ships[i] / (splitParam[2] + 1));
		e = document.getElementById(flds[i]);
		if	(e)
		{
			e.disabled = true;
			e.value = ships[i];
		}
	}

	if	(splitParam[0].ships[2] > 0 || splitParam[0].ships[3] > 0)
	{
		splitParam[7] = ships[0] * o.gSpace + ships[1] * o.fSpace;
		splitParam[8] = ships[2] * o.cHaul + ships[3] * o.bHaul;
		document.getElementById('hused').innerHTML = splitParam[7];
		document.getElementById('havail').innerHTML = splitParam[8];
	}

	var	s=0;
	for	(i=0;i<4;i++)
		s += ships[i];

	var	canSplit = (s > 0);
	if	(splitParam[0].orders.oType == 2 || splitParam[0].orders.oType == 1 && splitParam[0].orders.hyperspace)
		canSplit = canSplit && (splitParam[7] <= splitParam[8]);
	document.getElementById('bsplit').disabled = !canSplit;
}

function	setSplitCount(amount)
{
	splitParam[2] = amount;
	if	(splitParam[1] == 0)
	{
		updateSplitHaul();
		updateSplitOk();
	}
	else
		updateAutoSplit();
}

function	setSplitShips(type,amount)
{
	var	flds = ['sgas','sfgt','scru','sbcr'];
	if	(amount < 0 || isNaN(amount))
		return;
	splitParam[type+3] = amount;
	updateSplitHaul();
	updateSplitOk();
}

function	updateSplitHaul()
{
	if	(splitParam[0].ships[2] == 0 && splitParam[0].ships[3] == 0)
	{
		splitParam[7] = splitParam[8] = 0;
		return;
	}
	var	o = allies.get(splitParam[0].owner);
	splitParam[7] = splitParam[3] * o.gSpace + splitParam[4] * o.fSpace;
	splitParam[8] = splitParam[5] * o.cHaul + splitParam[6] * o.bHaul;
	document.getElementById('hused').innerHTML = splitParam[7];
	document.getElementById('havail').innerHTML = splitParam[8];
}

function	updateSplitOk()
{
	var	ships = [0,0,0,0];
	var	diffs = [0,0,0,0];
	var	i,s1=0,s2=0,neg=false;
	for	(i=0;i<4;i++)
	{
		ships[i] = splitParam[i+3] * splitParam[2];
		s1 += ships[i];
		diffs[i] = splitParam[0].ships[i] - ships[i];
		neg = neg || (diffs[i] < 0);
		s2 += diffs[i];
	}

	var	canSplit = (s1 > 0 && !neg && s2 > 0);
	if	(splitParam[0].orders.oType == 2 || splitParam[0].orders.oType == 1 && splitParam[0].orders.hyperspace)
	{
		var	ou, os, o = allies.get(splitParam[0].owner);
		ou = diffs[0] * o.gSpace + diffs[1] * o.fSpace;
		os = diffs[2] * o.cHaul + diffs[3] * o.bHaul;
		canSplit = canSplit && (splitParam[7] <= splitParam[8]) && (ou <= os);
	}
	document.getElementById('bsplit').disabled = !canSplit;
}

function	cancelSplit()
{
	prepareUpdate();
	x_getFleetsList(parseMainData);
}

function	doFleetSplit()
{
	document.getElementById('bsplit').disabled = true;
	if	(splitParam[1] == 0)
		x_manualSplit(splitParam[0].id, splitParam[9], splitParam[2], splitParam[3], splitParam[4], splitParam[5], splitParam[6], splitCallback);
	else
		x_autoSplit(splitParam[0].id, splitParam[9], splitParam[2], splitCallback);
}

function	splitCallback(str)
{
	if (str.indexOf('ERR#') == 0) {
		var	l = str.split('#');
		alertFleetSplit(parseInt(l[1], 10));
		document.getElementById('bsplit').disabled = false;
	} else {
		clearSelection();
		prepareUpdate();
		parseMainData(str);
	}
}


function	moveSelected()
{
	if	(autoLock)
	{
		setTimeout('moveSelected()', 500);
		return;
	}
	lockUpdate();

	orderFleets = new Array();
	for	(var i=0;i<fDisp.length;i++)
		if	(fDisp[i].selected)
			orderFleets.push(fDisp[i].id);
	moveTo = waitTime = -1;
	moveToLoc = null;

	onMainPage = false;
	guessOrders = true;
	drawOrdersPage();
	updateOrdersPage();
}

function	cancelOrders()
{
	prepareUpdate();
	x_getFleetsList(parseMainData);
}

function	setNewOrders()
{
	var	mode, e;
	if	((e = document.getElementById('flmode')))
		mode = e.options[e.selectedIndex].value;
	else
		mode = 0;
	x_setOrders(orderFleets.join('#'), moveTo, waitTime, mode, ordersCallback);
}

function	ordersCallback(str)
{
	if	(str.indexOf('ERR#') == 0)
	{
		var	l = str.split('#');
		alertFleetOrders(parseInt(l[1], 10));
	}
	else
	{
		clearSelection();
		prepareUpdate();
		parseMainData(str);
	}
}

function	updateOrdersPage()
{
	if	(!moveToLoc)
	{
		if	(!readingMoveLocation)
		{
			readingMoveLocation = true;
			x_getLocation(moveTo, gotLocation);
		}
		return;
	}
	document.getElementById('oself').innerHTML = getOSelFleets();
	document.getElementById('oavaf').innerHTML = getOAvailFleets();
	document.getElementById('moveto').innerHTML = getMoveToLine();
	document.getElementById('standby').innerHTML = getStandByLine();
	document.getElementById('odmode').innerHTML = getModeLine();
	document.getElementById('chordc').innerHTML = getOrderLinks();
	updateTrajectories();
}

function	updateTrajectories()
{
	if	(readingMoveLocation || trajLock)
		return;

	var	i, f;
	for	(i=0;i<orderFleets.length;i++)
	{
		var	f = fleets.get(orderFleets[i]);
		var	e = document.getElementById('ft' + f.id);
		if	(!e)
			continue;

		if	(moveTo == -1)
		{
			e.innerHTML = getTrajectoryText(f.id, []);
			continue;
		}

		var	t = getTrajectory(f, moveToLoc);
		e.innerHTML = getTrajectoryText(f.id, t);
	}
}

function	getOSelFleets()
{
	if	(!orderFleets.length)
	{
		selCanHS = true;
		return	'<p>' + actionText[0] + '</p>';
	}

	var	i,str = '<table class="oself"><tr>' + foSelHeader + '</tr>';
	selCanHS = true; gWait = gDest = -2;
	var n = new Array();
	sMovex = null; sMovey = null;
	orderFleets.sort(sortOrderFleets);
	for	(i=0;i<orderFleets.length;i++)
	{
		var	f = fleets.get(orderFleets[i]), o = allies.get(f.owner);
		var	used = f.ships[0] * o.gSpace + f.ships[1] * o.fSpace;
		var	haul = f.ships[2] * o.cHaul + f.ships[3] * o.bHaul;
		var	canHS = (used <= haul);
		selCanHS = selCanHS && canHS;
		if	(!canHS)
		{
			var	fLoc = (f.orders.oType != 1 ? f.orders.loc : f.orders.cur);
			if	(typeof sMovex == 'object')
			{
				sMovex = fLoc.x; sMovey = fLoc.y;
			}
			else if	(fLoc.x != sMovex || fLoc.y != sMovey)
				continue;
		}
		n.push(f.id);

		if	(guessOrders)
		{
			if	(f.orders.oType == 1)
			{
				if	(gDest == -2 && gWait == -2)
				{
					gDest = (f.orders.toId == f.orders.curId ? -1 : f.orders.toId);
					if	(f.orders.wait)
						gWait = f.orders.wait.left;
					else
						gWait = -1;
				}
				else if	(gDest != f.orders.toId || gWait > 0 && !f.orders.wait || f.orders.wait && gWait != f.orders.wait.left)
					gWait = gDest = -1;
			}
			else if	(f.orders.oType == 2)
			{
				if	(gWait == -2 && gDest == -2)
				{
					gDest = -1;
					gWait = f.orders.left;
				}
				else if	(gDest > 0 || f.orders.left != gWait)
					gWait = gDest = -1;
			}
			else
				gWait = gDest = -1;
			orderMode = parseInt(f.mode, 10);
		}

		var	oc = ' onClick="removeOrdersSelection('+f.id+')"';
		str += '<tr class="' + (f.owner == myself.id ? 'flown' : 'flally') + '"><td class="fown"'+oc+'>'
			+ players.get(f.owner) + '</td><td'+oc+' class="fname">' + f.name + '</td><td'+oc+' class="fhs">';
		if	(haul == 0)
			str += 'N/A';
		else
		{
			var hp = Math.round(used * 100 / haul);
			str += (hp > 200) ? '&gt;200%' : (hp + '%');
		}
		str += '</td><td'+oc+' class="fshp">' + formatNumber(f.ships[0]) + ' / ' + formatNumber(f.ships[1]) + ' / ';
		str += formatNumber(f.ships[2]) + ' / ' + formatNumber(f.ships[3]) + '</td><td'+oc+' class="fpwr">';
		str += formatNumber(f.power) + '</td><td class="ftraj" id="ft' + f.id + '">&nbsp;</td></tr>';
	}
	orderFleets = n;
	str += '</table>';
	guessOrders = false;
	if	(gWait > 0)
		waitTime = gWait;
	if	(gDest > 0)
	{
		moveTo = gDest;
		moveToLoc = locations.get(moveTo);
	}

	return	str;
}

function	getOAvailFleets()
{
	// Fleets are available if:
	//     they are not selected
	// AND (   they are idling AND can move
	//     OR  they are moving
	//     OR  they are waiting in HS )
	// AND (   there is a destination
	//     AND (   the fleet is HS capable
	//         OR  the fleet is not HS capable AND the target is in the same system )
	//     OR there is no destination )
	// AND (   there are HS stand-by orders AND the fleet is HS capable
	//     OR  there are no HS stand-by orders )
	var	i, sst = '#' + orderFleets.join('#') + '#';
	var	afl = new Array(), fk = fleets.keys();

	for	(i=0;i<fk.length;i++)
	{
		if	(sst.indexOf('#'+fk[i]+'#') != -1)
			continue;
		var	f = fleets.get(fk[i]);
		var	o = allies.get(f.owner);
		if	(!o)
			continue;

		var	hs = ((f.ships[0] * o.gSpace + f.ships[1] * o.fSpace) <= (f.ships[2] * o.cHaul + f.ships[3] * o.bHaul));
		var	fLoc = (f.orders.oType != 1 ? f.orders.loc : f.orders.cur);

		if	(	f.orders.oType == 0 && f.orders.canMove != 'Y'
			||	moveTo != -1 && !hs && (fLoc.x != moveToLoc.x || fLoc.y != moveToLoc.y)
			||	waitTime != -1 && !hs
			||	!selCanHS && !hs && (fLoc.x != sMovex || fLoc.y != sMovey)
			||	f.orders.oType >= 3
			)
			continue;
		afl.push(f);
	}

	if	(!afl.length)
		return	'<p>' + noFleetsFound + '</p>';

	var	i,str = '<table class="oavaf"><tr>' + foAvaHeader + '</tr>';
	afl.sort(sortAvailFleets);
	for	(i=0;i<afl.length;i++)
	{
		var	f = afl[i], o = allies.get(f.owner);
		var	used = f.ships[0] * o.gSpace + f.ships[1] * o.fSpace;
		var	haul = f.ships[2] * o.cHaul + f.ships[3] * o.bHaul;

		str += '<tr onClick="addOrdersSelection('+f.id+')" class="' + (f.owner == myself.id ? 'flown' : 'flally') + '"><td class="fown">';
		str += players.get(f.owner) + '</td><td class="fname">' + f.name + '</td><td class="fhs">';
		if	(haul == 0)
			str += 'N/A';
		else
		{
			var hp = Math.round(used * 100 / haul);
			str += (hp > 200) ? '&gt;200%' : (hp + '%');
		}
		str += '</td><td class="fshp">' + formatNumber(f.ships[0]) + ' / ' + formatNumber(f.ships[1]) + ' / ';
		str += formatNumber(f.ships[2]) + ' / ' + formatNumber(f.ships[3]) + '</td><td class="fpwr">';
		str += formatNumber(f.power) + '</td><td class="fco">';
		var m = parseInt(f.mode, 10);
		if	(f.orders.oType == 0)
			str += curOrdersTxt[m] + f.orders.loc.name + curOrdersTxt[2 + m];
		else if	(f.orders.oType == 1)
			str += curOrdersTxt[m+4] + f.orders.to.name + curOrdersTxt[m+6];
		else
			str += curOrdersTxt[m+8] + f.orders.loc.name + curOrdersTxt[m+10];
		str += '</td></tr>';
	}
	str += '</table>';

	return	str;
}

function	removeOrdersSelection(id)
{
	var	n = new Array(), i;
	for	(i=0;i<orderFleets.length;i++)
		if	(orderFleets[i] != id)
			n.push(orderFleets[i]);
	orderFleets = n;
	updateOrdersPage();
}

function	addOrdersSelection(id)
{
	orderFleets.push(id);
	updateOrdersPage();
}

function	setOrdersTime()
{
	var	str = "", nt=waitTime;
	while	(str == "")
	{
		str = getFleetDelay(waitTime == -1 ? "" : waitTime);
		if	(typeof str == 'object' && !str)
			return;
		nt = parseInt(str, 10);
		if	(isNaN(nt))
		{
			fleetDelayError(0);
			str = "";
		}
		else if	(nt < 1)
		{
			fleetDelayError(1);
			str = "";
		}
		else if	(nt > 48)
		{
			fleetDelayError(2);
			str = "";
		}
	}
	waitTime = nt;
	updateOrdersPage();
}

function	cancelOrdersDelay()
{
	waitTime = -1;
	updateOrdersPage();
}

function	removeOrdersDestination()
{
	moveTo = -1;
	updateOrdersPage();
}

function gotLocation(data) {
	var	l = data.split("\n");
	var	a, o;

	a = l.shift().split('#');
	moveToLoc = new FleetLocation(a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.join('#'));
	if (moveToLoc.opacity == 0) {
		a = l.shift().split('#');
		moveToLoc.details = new PlanetDetails(a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.join('#'));
	}
	readingMoveLocation = false;
	updateOrdersPage();
}

function setOrdersDestination() {
	if (mapRemember && typeof sMovex == 'object') {
	} else if (moveTo == -1) {
		var ok = false;
		if (typeof sMovex != 'object') {
			dMapX = parseInt(sMovex,10);
			dMapY = parseInt(sMovey,10);
			ok = true;
		} else {
			if (orderFleets.length) {
				var f = fleets.get(orderFleets[0]);
				if (f) {
					var fLoc = (f.orders.oType != 1 ? f.orders.loc : f.orders.cur);
					dMapX = parseInt(fLoc.x, 10);
					dMapY = parseInt(fLoc.y, 10);
					ok = true;
				}
			}
		}
		if (! ok) {
			if (myself.planets.length) {
				dMapX = parseInt(myself.planets[0].x,10);
				dMapY = parseInt(myself.planets[0].y,10);
			} else {
				var	i = 0, j, k = allies.keys();
				while	(i < k.length)
				{
					var	a = allies.get(k[i]);
					if	(!a.isMe && a.planets.length)
					{
						dMapX = parseInt(a.planets[0].x,10);
						dMapY = parseInt(a.planets[0].y,10);
						break;
					}
					i++;
				}
				if	(i == k.length)
					dMapX = dMapY = 0;
			}
		}
	} else {
		dMapX = parseInt(moveToLoc.x,10);
		dMapY = parseInt(moveToLoc.y,10);
	}
	newDest = moveTo;
	mapCType = 0;

	drawDestinationSelection();
	if	(!map.containsKey(dMapX + ';' + dMapY))
	{
		loadingMap = true;
		x_getMapData(dMapX, dMapY, gotMapData);
	}
	else
	{
		loadingMap = false;
		updateDestinationSelection();
	}
}

function	gotMapData(data)
{
	if	(data == "ERR")
		alertMap();
	else
	{
		var	l = data.split('\n');
		var	a = l.shift().split("#"), i;
		dMapX = parseInt(a[0], 10); dMapY = parseInt(a[1], 10);
		var	system = new MapSystem(dMapX, dMapY, a[2], a[3] == '1');

		for	(i=0;i<6;i++)
		{
			if	(system.opacity != -1)
			{
				a = l.shift().split('#');
				var loc = new MapLocation(a.shift(), dMapX, dMapY, i+1, a.shift(), a.shift(), a.join('#'));
				system.locations.push(loc);
				map.put("p" + loc.id, loc);
				map.put("pn-" + loc.name.toLowerCase(), loc);
			}
			else
				system.locations.push(null);
		}
		map.put(dMapX + ";" + dMapY, system);
	}
	updateDestinationSelection();
	loadingMap = false;
}

function updateDestinationSelection() {
	updateDestLine();
	if (typeof sMovex == 'object') {
		drawGalacticMap();
	} else {
		drawSystemMap();
	}
}

function	updateDestLine()
{
	if	(newDest == -1)
		document.getElementById('cursel').innerHTML = noSelection;
	else
	{
		var	d = map.get("p" + newDest);
		document.getElementById('cursel').innerHTML = d.name + " (<b>" + d.x + "," + d.y + "</b>," + d.orbit + ")";
	}
}

function drawSystemMap() {
	var sys = map.get(dMapX + ";" + dMapY);
	var str = "<table class='smap'><tr><td class='sys" + sys.opacity + "'>";
	var i;

	for (i = 0; i < 6; i ++) {
		var loc = sys.locations[i];
		if (sys.opacity == 0) {
			var	img = (loc.color == 4) ? "prem_s" : ("pl/s/"+loc.id);
			str += "<tr onClick='setDestination(" + loc.id
				+ ")'><td class='mimg'><img class='mimg' alt='[P]' src='"
				+ staticurl + '/beta5/pics/'+img+".png' /></td><td class='ptype" + loc.color + "'>" +
				loc.name + '</td></tr>';
		} else {
			str += "<tr onClick='setDestination(" + loc.id + ")'><td class='ptype" + loc.color
				+ "'>" + loc.name + "</td></tr>";
		}
	}

	str += '</td></tr></table><p class="smapi">' + smapCoordTxt + '<b>' + dMapX + ' , ' + dMapY + '</b></p>';

	if (sys.protection) {
		str = '<span class="prot">' + str + '</span>';
	}
	document.getElementById('sdmap').innerHTML = str;
}

function	drawGalacticMap()
{
	var	sys = map.get(dMapX + ";" + dMapY);
	var	op = sys.opacity == -1 ? 0 : sys.opacity;
	var	str = "<table class='gmap'><tr><td class='gmcor'>&nbsp;</td><td class='gmvm' onClick='moveMap(0,1)'>"
			+ "<img src='"+staticurl+"/beta5/pics/up_"+color+".gif' alt='"+dirTxt[0]+"' /></td>"
			+ "<td class='gmcor'>&nbsp;</td></tr><tr><td class='gmhm' onClick='moveMap(-1,0)'>"
			+ "<img src='"+staticurl+"/beta5/pics/left_"+color+".gif' alt='"+dirTxt[1]+"' /></td>"
			+ "<td class='sys"+op+"'>" + (sys.protection ? '<span class="prot">' : '')
			+ "<table class='gmmap'>";
	var	i;

	for	(i=0;i<6;i++)
	{
		var	loc = sys.locations[i];
		if	(loc && sys.opacity == 0)
		{
			var	img = (loc.color == 4) ? "prem_s" : ("pl/s/"+loc.id);
			str += "<tr onClick='setDestination("+loc.id+")'><td class='mimg'><img class='mimg' alt='[P]' src='"
				+ staticurl + '/beta5/pics/'+img+".png' /></td><td class='ptype" + loc.color + "'>" +
				loc.name + '</td></tr>';
		}
		else if	(loc)
			str += "<tr onClick='setDestination("+loc.id+")'><td class='ptype"+loc.color+"'>"+loc.name+"</td></tr>";
		else
			str += '<tr><td class="gmun">'+unchartedTxt+'</td></tr>';
	}

	str += "</table>" + (sys.protection ? '</span>' : '')
		+ "</td><td class='gmhm' onClick='moveMap(1,0)'><img src='"+staticurl+"/beta5/pics/right_"
		+color+".gif' alt='"+dirTxt[2]+"' /></td></tr><tr><td class='gmcor'>&nbsp;</td><td class='gmvm' onClick='"
		+"moveMap(0,-1)'><img src='"+staticurl+"/beta5/pics/down_"+color+".gif' alt='"+dirTxt[3]+"' /></td>"
		+"<td class='gmcor'>&nbsp;</td></tr></table>";
	str += drawMapControls();
	document.getElementById('sdmap').innerHTML = str;
}

function	updateGalacticMap()
{
	if	(loadingMap)
		return;

	var	e;

	if	(document.getElementById('mct0').checked)
	{
		var	x = parseInt(document.getElementById('cx').value, 10);
		var	y = parseInt(document.getElementById('cy').value, 10);
		mapCType = 0;
		if	(isNaN(x) || isNaN(y))
			return
		moveMapTo(x,y);
	}
	else if	(document.getElementById('mct1').checked)
	{
		e = document.getElementById('mcpo');
		var	p = e.options[e.selectedIndex].value;
		mapCType = 1;
		if	(p == 0)
			return;
		mapParm = p;
		if	(map.containsKey('p'+p))
		{
			var po = map.get('p'+p);
			moveMapTo(parseInt(po.x,10), parseInt(po.y,10));
		}
		else
		{
			loadingMap = true;
			x_moveMapToId(p, gotMapData);
		}
	}
	else
	{
		e = document.getElementById('mcpn').value;
		mapCType = 2;
		if	(e == "")
			return;
		mapParm = e;
		if	(map.containsKey('pn-'+e.toLowerCase()))
		{
			var po = map.get('pn-'+e.toLowerCase());
			moveMapTo(parseInt(po.x,10), parseInt(po.y,10));
		}
		else
		{
			loadingMap = true;
			x_moveMapToName(e, gotMapData);
		}
	}
}

function	moveMap(dx,dy)
{
	if	(loadingMap)
		return;

	mapCType = 0;
	moveMapTo(dMapX + dx, dMapY + dy);
}

function	moveMapTo(nx, ny)
{
	if	(map.containsKey(nx + ';' + ny))
	{
		dMapX = nx; dMapY = ny;
		drawGalacticMap();
	}
	else
	{
		loadingMap = true;
		x_getMapData(nx, ny, gotMapData);
	}
}

function	setDestination(id)
{
	if	(id == newDest)
		return;
	newDest = id;
	document.getElementById('sdok').disabled = (newDest == moveTo);
	updateDestLine();
}

function	confirmSetDestination()
{
	moveTo = newDest;
	if	(locations.containsKey(moveTo))
		moveToLoc = locations.get(moveTo);
	else
		moveToLoc = null;
	cancelSetDestination();
}

function	cancelSetDestination()
{
	drawOrdersPage();
	updateOrdersPage();
}


function	viewTrajectory()
{
	if	(autoLock)
	{
		setTimeout('viewTrajectory()', 500);
		return;
	}
	lockUpdate();

	var i, f = null;
	for	(i=0;i<fDisp.length;i++)
		if	(fDisp[i].selected)
		{
			f = fDisp[i];
			break;
		}

	if	(!(f && f.orders.oType == 1))
	{
		unlockUpdate();
		return;
	}

	onMainPage = false;
	x_getFleetTrajectory(f.id, gotFleetTrajectory);
}


function	gotFleetTrajectory(data)
{
	if (data == "") {
		alertServerError();
		x_getFleetsList(parseMainData);
		return;
	}

	var	l = data.split('\n');
	var	a, hc = false;

	a = l.shift().split('#');
	fleetTrajectory = new FleetTrajectory(a[0],a[1],a[2],a[3],a[4],a[5],a[6]);
	while	(l.length)
	{
		a = l.shift().split('#');
		fleetTrajectory.waypoints.push(new FleetWayPoint(
			a.shift(), a.shift(), a.shift(), a.shift(), a.shift(), a.join('#')
		));
	}

	drawFleetTrajectory();
}


function	sellSelected()
{
	if	(autoLock)
	{
		setTimeout('sellSelected()', 500);
		return;
	}
	lockUpdate();

	sales = new Hashtable();
	for	(var i=0;i<fDisp.length;i++)
	{
		if	(!fDisp[i].selected || fDisp[i].orders.oType != 0 || fDisp[i].orders.canMove != 'Y' || fDisp[i].mode == 1)
			continue;

		var	s;
		if	(!sales.containsKey(fDisp[i].orders.locId))
		{
			s = new FleetSale(fDisp[i].orders.locId);
			sales.put(s.id, s);
		}
		else
			s = sales.get(fDisp[i].orders.locId);
		s.fleets.push(fDisp[i]);
	}
	if	(sales.isEmpty())
	{
		prepareUpdate();
		x_getFleetsList(parseMainData);
	}

	onMainPage = false;
	drawSellPage();
}

function	drawSellPage()
{
	drawSellLayout();

	var	k = sales.keys(), str = '';
	k.sort(sortSalesKeys);
	for	(var i=0;i<k.length;i++)
		str += sales.get(k[i]).drawForm();

	document.getElementById('sflist').innerHTML = str;

	for	(var i=0;i<k.length;i++)
		sales.get(k[i]).update();
	drawSellLinks(false);
}

function	checkAllSales()
{
	var	k = sales.keys(), str = '', v = true;
	for	(var i=0;v&&i<k.length;i++)
		v = v && sales.get(k[i]).isValid;
	drawSellLinks(v);
}

function	cancelSale()
{
	sales = null;
	prepareUpdate();
	x_getFleetsList(parseMainData);
}

function	confirmSale()
{
	var	k = sales.keys(), l = new Array();
	for	(var i=0;i<k.length;i++)
	{
		var	s = sales.get(k[i]), j, fids = new Array(), p = s.player;
		p.replace(/[^A-Za-z0-9_\.\-\+@\/' ]/, ' ');
		for	(j=0;j<s.fleets.length;j++)
			fids.push(s.fleets[j].id);
		l.push(s.id + '#' + s.mode + '#' + s.expires + '#' + s.price + '#' + p + '#' + fids.join('!'));
	}
	x_sellFleets(l.join('*'), fleetsSold);
}

function	fleetsSold(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		var	l = data.split('#');
		l.shift();
		var	ec = parseInt(l.shift(), 10);

		// Remove fleets and sales if required
		if	(ec == 4)
		{
			while	(l.length)
			{
				var	sid = l.shift(), fl = '!' + l.shift() + '!';
				var	s = sales.get(sid);
				if	(!s)
					continue;
				var	f = new Array();
				for	(var i=0;i<s.fleets.length;i++)
				{
					if	(fl.indexOf('!' + s.fleets[i].id + '!') != -1)
						continue;
					f.push(s.fleets[i]);
				}
				if	(f.length)
					s.fleets[i] = f;
				else
					sales.remove(sid);
			}
			if	(sales.isEmpty())
			{
				saleAlert(5, null);
				cancelSale();
				return;
			}
			saleAlert(4, null);

			var	k = sales.keys(), str = '';
			k.sort(sortSalesKeys);
			for	(var i=0;i<k.length;i++)
				str += sales.get(k[i]).drawForm();
			document.getElementById('sflist').innerHTML = str;

			for	(var i=0;i<k.length;i++)
			{
				sales.get(k[i]).update();
				sales.get(k[i]).validate();
			}
		}
		else
			saleAlert(ec, l.shift());
	}
	else
	{
		clearSelection();
		prepareUpdate();
		parseMainData(data);
	}
}


function	cancelSaleSelected()
{
	if	(autoLock)
	{
		setTimeout('cancelSaleSelected()', 500);
		return;
	}
	lockUpdate();

	var	s = new Array();
	for	(i=0;i<fDisp.length;i++)
	{
		if	(fDisp[i].selected)
			s.push(fDisp[i].orders.saleId);
	}

	if	(confirmCancelSale(s.length >1))
		x_cancelSales(s.join('#'), salesCanceled);
	else
		unlockUpdate();
}

function	salesCanceled(str)
{
	if	(str.indexOf('ERR#') == 0)
	{
		var	l = str.split('#');
		alertCancelSales(parseInt(l[1], 10));
		unlockUpdate();
	}
	else
	{
		prepareUpdate();
		parseMainData(str);
	}
}


function	viewSaleSelected()
{
	if	(autoLock)
	{
		setTimeout('viewSaleSelected()', 500);
		return;
	}
	lockUpdate();

	var	id = null;
	for	(i=0;i<fDisp.length;i++)
		if	(fDisp[i].selected)
		{
			id = fDisp[i].orders.saleId;
			break;
		}

	if	(id)
		x_getSaleDetails(id, gotSaleDetails);
	else
		unlockUpdate();
}

function gotSaleDetails(str) {
	if (str == "") {
		alertViewSale();
		unlockUpdate();
	}
	else
	{
		var	t = str.split('\n'), l = t[0].split('#');
		onMainPage = false;
		drawSaleDetails(l, t.length == 2 ? t[1] : null);
	}
}


function	lockUpdate()
{
	clearTimeout(autoUpdate);
	autoLock = true;
}


function	unlockUpdate()
{
	autoUpdate = setTimeout('mainDataUpdate()', 60000);
	autoLock = false;
}


function	clearSelection()
{
	var	i;
	if	(typeof fDisp != 'object')
		return;
	for	(i=0;i<fDisp.length;i++)
	{
		if	(onMainPage)
			document.getElementById('fsel' + fDisp[i].id).checked = false;
		fDisp[i].selected = false;
	}
	if	(onMainPage)
		updateActions();
}


function	parseMainDataInternal(data)
{
	var	l = data.split('\n');
	var	i, a = l.shift().split('#');
	var	nAllies = a.shift(), nFleets = a.shift(), nPlanets = a.shift();

	// Get page parameters
	a = l.shift().split('#');
	listLocations = a.shift(); listMode = a.shift(); fDispMode = a.shift(); alliesMode = a.shift();
	// Get search parameters
	a = l.shift().split('#');
	sType = a.shift(); sText = a.join('#');

	// Parse lists
	parseAllies(l, nAllies);
	var plist = parseFleets(l, nFleets);
	parsePlanets(l, nPlanets, plist);
	parsePlayers(l, plist);

	// Create links between lists
	linkMainData();
}


function	parseMainData(data)
{
	// Do the actual parsing
	parseMainDataInternal(data);

	// Display page
	if	(!onMainPage)
	{
		drawMainLayout();
		if (navigator.userAgent.toLowerCase().indexOf("msie") != -1 && document.all && !(window.opera && document.getElementById)) {
			var ff = document.getElementById('faframe');
			ff.style.position = "absolute";
			var r = document.documentElement ? document.documentElement : document.body, h = r.clientHeight;
			r.onscroll = function () {
				ff.style.top = (r.scrollTop + h - 32) + "px";
			};
			ff.style.top = (r.scrollTop + h - 32) + "px";
		}
		onMainPage = true;
	}
	applyFilters();
	drawMainContents();

	// Get ready to auto-update
	unlockUpdate();
}


function	parseMainDataMove(data)
{
	// Do the actual parsing
	parseMainDataInternal(data);

	// Display page
	autoLock = true;
	onMainPage = false;
	var r = document.documentElement ? document.documentElement : document.body;
	r.onscroll = null;
	guessOrders = true;
	orderFleets = new Array();
	moveToLoc = null; waitTime = -1;
	drawOrdersPage();
	updateOrdersPage();
}


function sortMapPList(ia, ib) {
	var a = locations.get(ia), b = locations.get(ib);
	var oa = allies.get(a.details.owner), ob = allies.get(b.details.owner);
	var pa = players.get(a.details.owner), pb = players.get(b.details.owner);
	return (a.details.owner == b.details.owner
		? (a.x != b.x
			? (parseInt(a.x, 10) - parseInt(b.x, 10))
			: (a.y != b.y
				? (parseInt(a.y, 10) - parseInt(b.y, 10))
				: (parseInt(a.orbit, 10) - parseInt(b.orbit, 10))
			))
		: (oa.isMe ? -1 : (ob.isMe ? 1 : (pa.toLowerCase() > pb.toLowerCase() ? 1 : -1))));
}

function sortPPList(ia,ib) {
	var a=ppList.get(ia).fleetLocation,b=ppList.get(ib).fleetLocation;
	return (a.x!=b.x
			?(parseInt(a.x,10)-parseInt(b.x,10))
			:(a.y!=b.y
				?(parseInt(a.y,10)-parseInt(b.y,10))
				:(parseInt(a.orbit,10)-parseInt(b.orbit,10))
			)
		);
}

function sortAPList(ia,ib) {
	var a=apList.get(ia).fleetLocation,b=apList.get(ib).fleetLocation;
	return (a.details.owner==b.details.owner
			?(a.x!=b.x
				?(parseInt(a.x,10)-parseInt(b.x,10))
				:(a.y!=b.y
					?(parseInt(a.y,10)-parseInt(b.y,10))
					:(parseInt(a.orbit,10)-parseInt(b.orbit,10))
				)
			):(players.get(a.details.owner).toLowerCase()>players.get(b.details.owner).toLowerCase()?1:-1));
}

function	sortOPList(ia,ib)
{
	var a=opList.get(ia).fleetLocation,b=opList.get(ib).fleetLocation;
	return (a.x!=b.x
			?(parseInt(a.x,10)-parseInt(b.x,10))
			:(a.y!=b.y
				?(parseInt(a.y,10)-parseInt(b.y,10))
				:(parseInt(a.orbit,10)-parseInt(b.orbit,10))
			)
		);
}

function	sortPlanetFleets(ia,ib)
{
	var a=fleets.get(ia),b=fleets.get(ib);
	return	(a.owner == b.owner
			? (parseInt(b.power,10)-parseInt(a.power,10))
			: (a.owner == myself.id ? -1 : (b.owner == myself.id ? 1 :
				(a.mode == 0 && b.mode == 1 ? -1 : (a.mode == 1 && b.mode == 0 ? 1 :
					(players.get(a.owner).toLowerCase()>players.get(b.owner).toLowerCase() ? 1 : -1)
				))
			))
		);
}

function	sortMovingFleets(a,b)
{
	return	(a.owner == b.owner
			? (a.orders.eta == b.orders.eta
				? (parseInt(b.power,10)-parseInt(a.power,10))
				: (parseInt(a.orders.eta,10)-parseInt(b.orders.eta,10))
			) : (a.owner == myself.id ? -1 : (b.owner == myself.id ? 1 :
				(players.get(a.owner).toLowerCase()>players.get(b.owner).toLowerCase() ? 1 : -1)
			))
		);
}

function	sortWaitingFleets(a,b)
{
	return	(a.owner == b.owner
			? (a.orders.left == b.orders.left
				? (parseInt(b.power,10)-parseInt(a.power,10))
				: (parseInt(a.orders.left,10)-parseInt(b.orders.left,10))
			) : (a.owner == myself.id ? -1 : (b.owner == myself.id ? 1 :
				(players.get(a.owner).toLowerCase()>players.get(b.owner).toLowerCase() ? 1 : -1)
			))
		);
}

function	sortOrderFleets(ia,ib)
{
	var a=fleets.get(ia),b=fleets.get(ib);
	return	(a.owner == b.owner
			? (parseInt(b.power,10)-parseInt(a.power,10))
			: (a.owner == myself.id ? -1 : (b.owner == myself.id ? 1 :
				(players.get(a.owner).toLowerCase()>players.get(b.owner).toLowerCase() ? 1 : -1)
			))
		);
}

function	sortAvailFleets(a,b)
{
	return	(a.owner == b.owner
			? (a.orders.oType == b.orders.oType && a.mode == b.mode
				? (parseInt(b.power,10)-parseInt(a.power,10))
				: (a.orders.oType * 2 + parseInt(a.mode,10) - b.orders.oType * 2 - parseInt(b.mode,10))
			) : (a.owner == myself.id ? -1 : (b.owner == myself.id ? 1 :
				(players.get(a.owner).toLowerCase()>players.get(b.owner).toLowerCase() ? 1 : -1)
			))
		);
}


function	sortSalesKeys(ia,ib)
{
	var a=locations.get(ia),b=locations.get(ib);
	return (a.x!=b.x
			?(parseInt(a.x,10)-parseInt(b.x,10))
			:(a.y!=b.y
				?(parseInt(a.y,10)-parseInt(b.y,10))
				:(parseInt(a.orbit,10)-parseInt(b.orbit,10))
		));
}
