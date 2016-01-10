var	vacation = true;
var	qbMode = false;
var	useCTTB = false;
var	action = 0;
var	milLevel;
var	planets;
var	updTimer;
var	mustHide;
var	miSel, miDir;

var	plstt;

function	BQItem(type, qt, ttb, cttb)
{
	this.type = type;
	this.qt = qt;
	this.ttb = ttb;
	this.cttb = cttb;
	this.selected = false;
}


function	Planet(id,name,x,y,orbit,pop,hap,ind,tur,mil,prof,cor,bqstr)
{
	this.id = id;
	this.name = name;
	this.x = x;
	this.y = y;
	this.orbit = orbit;
	this.pop = pop;
	this.hap = hap;
	this.corruption = cor;
	this.ind = ind;
	this.tur = tur;
	this.mil = mil;
	this.prof = prof;
	this.selected = false;

	var	i,j;
	this.bq = new Array();
	bqd = bqstr.split('#');
	if	(bqd.length == 1)
		return;
	ni = bqd.length / 4;
	for	(i=0;i<ni;i++)
	{
		j=i*4;
		this.bq[i] = new BQItem(bqd[j], bqd[j+1], bqd[j+2], bqd[j+3]);
	}
}


function buildPlanetLine(ipl) {
	var id = planets[ipl].id;
	var csb = ';border-style:solid;border-color:white;border-width: 1px 0px 0px 0px';
	var cs1 = ';text-align:center;vertical-align:top;padding: 4px 0px 0px 0px';
	var cs0 = cs1 + csb;
	var str = '<tr>'
		+ '<td style="width:32px;height:32px' + csb + '" rowspan="2"><img src="' + staticurl
			+ '/beta5/pics/pl/s/' + id + '.png" alt="[P]" style="border-width:0px' + csb + '" /></td>'
		+ '<td colspan="2" style="text-align:left;vertical-align:top' + csb + '">'
			+ '<a href="planet?id=' + id + '">' + planets[ipl].name + '</a></td>'
		+ '<td rowspan="2" style="width:10%;max-width:80px' + cs0 + '">(' + planets[ipl].x
			+ ',' + planets[ipl].y + ',' + planets[ipl].orbit + ')</td>'
		+ '<td rowspan="2" style="width:8%;max-width:60px' + cs0 + '">'
			 + formatNumber(planets[ipl].pop) + '</td>'
		+ '<td style="width:6%;max-width:50px' + cs0 + '">' + drawHappiness(planets[ipl].hap) + '</td>'
		+ '<td style="width:6%;max-width:50px' + cs0 + '">' + formatNumber(planets[ipl].ind) + '</td>'
		+ '<td rowspan="2" style="width:8%;max-width:70px' + cs0 + '">' + formatNumber(planets[ipl].tur) + '</td>'
		+ '<td rowspan="2" style="width:8%;max-width:80px' + cs0 + '">&euro;' + formatNumber(planets[ipl].prof)
			+ '</td>'
		+ '</tr><tr>'
		+ '<td style="width:20px">&nbsp;</td>' + drawBQSummary(planets[ipl].bq)
		+ '<td style="width:6%;max-width:50px' + cs1 + '">' + drawCorruption(planets[ipl].corruption) + '</td>'
		+ '<td style="width:6%;max-width:50px' + cs1 + '">' + formatNumber(planets[ipl].mil) + '</td>'
		+ '</tr>'
	return	str;
}


function	getMilitaryLevel(data)
{
	milLevel = parseInt(data, 10) + 2;
	document.getElementById('jsplanets').innerHTML = drawQuickBuilder();
}


function	buildQuickBuilder()
{
	if	(!milLevel)
		x_getMilitaryLevel(getMilitaryLevel);
	else
		document.getElementById('jsplanets').innerHTML = drawQuickBuilder();
}


function	buildQBList()
{
	var	e = document.getElementById('qb_ftype'), e2 = document.getElementById('qb_planets');
	if	(!(e && e2))
		setTimeout('buildQBList()', 500);
	else
		e2.innerHTML = drawQBList();
}


function	buildPlanetList()
{
	document.getElementById('jsplanets').innerHTML = drawMainDisplay();
}


function	planetListReceived(data)
{
	var	i, j, lines, np, pdat, nbp, sel, sstr, bqsel, bqstr, bqlen, bqlstr;

	// Store selections and build queue lengths as strings
	sel = new Array();
	bqlen = new Array();
	bqsel = new Array();
	if	(planets)
	{
		np = 0;
		for	(i=0;i<planets.length;i++)
		{
			var	id = planets[i].id;
			if	(planets[i].selected)
				sel.push(id);
			bqlen.push(id + '-' + planets[i].bq.length);
			for	(j=0;j<planets[i].bq.length;j++)
			{
				if	(planets[i].bq[j].selected)
					bqsel.push(id + '-' + j);
			}
		}
	}
	sstr = '!' + sel.join('!!') + '!';
	bqlstr = '!' + bqlen.join('!!') + '!';
	bqstr = '!' + bqsel.join('!!') + '!';

	// Regenerate planets array
	planets = new Array();
	lines = data.split('\n');
	np = (lines.length - 1) / 3;
	for	(i=0;i<np;i++)
	{
		pdat = lines[i*3].split('#');
		planets[i] = new Planet(pdat[0], lines[i*3+1], pdat[1], pdat[2], pdat[3], pdat[4], pdat[5], pdat[6], pdat[7], pdat[8], pdat[9], pdat[10], lines[i*3+2]);
		// Restore selections
		if	(sstr.indexOf('!' + pdat[0] + '!') != -1)
			planets[i].selected = true;
		if	(bqlstr.indexOf('!' + pdat[0] + '-' + planets[i].bq.length + '!') != -1)
		{
			for	(j=0;j<planets[i].bq.length;j++)
				planets[i].bq[j].selected = (bqstr.indexOf('!' + pdat[0] + '-' + j + '!') != -1);
		}
	}
	if	(qbMode)
		buildQBList();
	else
		buildPlanetList();
	updTimer = setTimeout('x_getPlanetList(planetListReceived)', 60000);
}


function	initGetMode(data)
{
	var	a = data.split('#');
	qbMode = (a[0] == '1');
	useCTTB = (a[1] == '1');
	vacation = (a[2] == '1');
	if (qbMode) {
		buildQuickBuilder();
	}
	x_getPlanetList(planetListReceived);
}


function	modeChange(data)
{
	if	(qbMode)
	{
		buildQuickBuilder();
		buildQBList();
	}
	else
		buildPlanetList();
}


function	switchTTB()
{
	useCTTB = !useCTTB;
	x_setCumulative(useCTTB ? 1 : 0, modeChange);
}


function	enableQuickBuilder()
{
	qbMode = !qbMode;
	x_setMode(qbMode ? 1 : 0, modeChange);
}


function	qbExecuteOk(data)
{
	if	(data == 'OK')
	{
		if	(action == 5)
			moveItems();
		action = 0;
		if	(updTimer)
			clearTimeout(updTimer);
		if	(qbMode)
		{
			if	(mustHide)
			{
				qbMode = !qbMode;
				x_setMode(qbMode ? 1 : 0, modeChange);
			}
			else
				buildQuickBuilder();
		}
		x_getPlanetList(planetListReceived);
		updateHeader();
	}
	else
		qbError(parseInt(data, 10));
	var b1 = document.getElementById('qbex1'), b2 = document.getElementById('qbex2');
	b1.disabled = false; b2.disabled = false;
}


function	qbExecute(hide)
{
	mustHide = hide;
	var b1 = document.getElementById('qbex1'), b2 = document.getElementById('qbex2');
	b1.disabled = true; b2.disabled = true;
	if	(action == 0)
	{
		qbError(0);
		b1.disabled = false; b2.disabled = false;
		return;
	}

	var	sel;
	if	(action < 4)
	{
		sel = getSelectedPlanets();
		if	(sel.length == 0)
		{
			qbError(1);
			b1.disabled = false; b2.disabled = false;
			return;
		}
	}
	else
	{
		sel = getSelectedItems();
		if	(sel.length == 0)
		{
			qbError(9);
			b1.disabled = false; b2.disabled = false;
			return;
		}
	}

	if	(action == 1)
	{
		var	n, b, t, ts, to;
		n = parseInt(document.getElementById('qb_fcount').value, 10);
		if	(isNaN(n) || n <= 0)
		{
			qbError(3);
			b1.disabled = false; b2.disabled = false;
			return;
		}
		ts = document.getElementById('qb_fdestr');
		to = ts.selectedIndex;
		b = parseInt(ts.options[to].value, 10);
		ts = document.getElementById('qb_ftype');
		to = ts.selectedIndex;
		t = parseInt(ts.options[to].value, 10);
		x_factoryAction(sel.join('#'), b, n, t, qbExecuteOk);
	}
	else if	(action == 2)
	{
		var	n, t, ts, to;
		n = parseInt(document.getElementById('qb_adc').value, 10);
		if	(isNaN(n) || n <= 0)
		{
			qbError(2);
			b1.disabled = false; b2.disabled = false;
			return;
		}
		ts = document.getElementById('qb_adt');
		to = ts.selectedIndex;
		t = parseInt(ts.options[to].value, 10);
		x_addToQueues(sel.join('#'), n, t, qbExecuteOk);
	}
	else if	(action == 3)
		x_flushQueues(sel.join('#'), qbExecuteOk);
	else if	(action == 4)
		x_deleteItems(sel.join('#'), qbExecuteOk);
	else if	(action == 5)
	{
		var	n, t, ts, to;
		ts = document.getElementById('qb_mid');
		to = ts.selectedIndex;
		t = parseInt(ts.options[to].value, 10);
		ts = '!' + sel.join('!') + '!';
		if	(t == 1)
		{
			if	(ts.indexOf('-0!') != -1)
			{
				qbError(11);
				b1.disabled = false; b2.disabled = false;
				return;
			}
		}
		else
		{
			for	(n=0;n<planets.length;n++)
			{
				to = planets[n].bq.length - 1;
				if	(ts.indexOf('!' + planets[n].id + '-' + to + '!') != -1)
				{
					qbError(12);
					b1.disabled = false; b2.disabled = false;
					return;
				}
			}
		}
		miSel = sel;
		miDir = t;
		x_moveItems(sel.join('#'), t, qbExecuteOk);
	}
	else if	(action == 6)
	{
		var	n, t, ts, to;
		n = parseInt(document.getElementById('qb_ric').value, 10);
		if	(isNaN(n) || n <= 0)
		{
			qbError(2);
			b1.disabled = false; b2.disabled = false;
			return;
		}
		ts = document.getElementById('qb_rit');
		to = ts.selectedIndex;
		t = parseInt(ts.options[to].value, 10);
		x_replaceItems(sel.join('#'), n, t, qbExecuteOk);
	}
	else
	{
		qbError(-1);
		b1.disabled = false; b2.disabled = false;
	}
}


function	selAllPlanets()
{
	var	i;
	for	(i=0;i<planets.length;i++)
	{
		var name = 'qb_pl' + planets[i].id;
		document.getElementById(name).checked = true;
		planets[i].selected = true;
	}
}

function	selNoPlanets()
{
	var	i;
	for	(i=0;i<planets.length;i++)
	{
		var name = 'qb_pl' + planets[i].id;
		document.getElementById(name).checked = false;
		planets[i].selected = false;
	}
}

function	invertPlanets()
{
	var	i;
	for	(i=0;i<planets.length;i++)
	{
		var name = 'qb_pl' + planets[i].id;
		v = !planets[i].selected;
		document.getElementById(name).checked = v;
		planets[i].selected = v;
	}
}

function	selPlanet(i)
{
	planets[i].selected = !planets[i].selected;
	return	true;
}

function	getSelectedPlanets()
{
	var	i, s = new Array();
	for	(i=0;i<planets.length;i++)
	{
		if	(planets[i].selected)
			s.push(planets[i].id);
	}
	return	s;
}


function selAllAt(planet) {
	var	i;
	for (i = 0; i < planets.length; i ++) {
		if (planets[i].id != planet) {
			continue;
		}
		for (var j = 0; j < planets[i].bq.length; j ++) {
			planets[i].bq[j].selected = true;
			document.getElementById('qb_pl' + planets[i].id + '_i' + j).checked = true;
		}
		break;
	}
}

function selNoneAt(planet) {
	var	i;
	for (i = 0; i < planets.length; i ++) {
		if (planets[i].id != planet) {
			continue;
		}
		for (var j = 0; j < planets[i].bq.length; j ++) {
			planets[i].bq[j].selected = false;
			document.getElementById('qb_pl' + planets[i].id + '_i' + j).checked = false;
		}
		break;
	}
}

function invertSelAt(planet) {
	var	i;
	for (i = 0; i < planets.length; i ++) {
		if (planets[i].id != planet) {
			continue;
		}
		for (var j = 0; j < planets[i].bq.length; j ++) {
			planets[i].bq[j].selected = ! planets[i].bq[j].selected;
			document.getElementById('qb_pl' + planets[i].id + '_i' + j).checked = planets[i].bq[j].selected;
		}
		break;
	}
}

function	selItem(i,j)
{
	planets[i].bq[j].selected = !planets[i].bq[j].selected;
	return	true;
}

function	getSelectedItems()
{
	var	i, j, s = new Array();
	for	(i=0;i<planets.length;i++)
	{
		id = planets[i].id;
		for	(j=0;j<planets[i].bq.length;j++)
		{
			if	(planets[i].bq[j].selected)
				s.push(id+'-'+j);
		}
	}
	return	s;
}

function	moveItems()
{
	var	i, j, s, a, ni;
	s = '';
	for	(i=0;i<miSel.length;i++)
	{
		a = miSel[i].split('-');
		for	(j=0;j<planets.length&&planets[j].id!=a[0];j++)
			;
		a[1] = parseInt(a[1], 10);
		ni = a[1] + (miDir ? -1 : 1);
		if	(s.indexOf('!'+j+'-'+a[1]+'!') == -1)
			planets[j].bq[a[1]].selected = false;
		planets[j].bq[ni].selected = true;
		s += '!'+j+'-'+ni+'!';
	}
}


function	selAction(i)
{
	action = i;
	document.getElementById('qb_ac' + i).checked = true;
}


function drawHappiness(value) {
	var str = '<span class="phap';
	if (value >= 70) {
		str += 'ok';
	} else if (value >= 40) {
		str += 'med';
	} else if (value >= 20) {
		str += 'dgr';
	} else {
		str += 'bad';
	}
	str += '">' + value + '%</span>';
	return str;
}

function drawCorruption(value) {
	var str = '<span class="phap';
	if (value >= 71) {
		str += 'bad';
	} else if (value >= 41) {
		str += 'dgr';
	} else if (value >= 11) {
		str += 'med';
	} else {
		str += 'ok';
	}
	str += '">' + value + '%</span>';
	return str;
}
