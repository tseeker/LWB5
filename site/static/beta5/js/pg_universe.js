var	stDiff;
var	ticks;
var	nUpdates;

function	Tick(id,first,interval,last,name)
{
	this.id		= id;
	this.first	= parseInt(first, 10);
	this.interval	= parseInt(interval, 10);
	this.stopTime	= (last != "") ? parseInt(last, 10) : -1;
	this.name	= name;

	this.started	= true;
	this.stDays	= 0;
	this.previous	= 0;
	this.next	= 0;
	this.last	= 0;
	this.remaining	= 0;

	this.compute	= Tick_compute;
	this.draw	= Tick_draw;
}

function	Tick_compute(st)
{
	var	f = this.first % 86400;
	var	n = (st % 86400);
	var	tm = n + ((n<f) ? 86400 : 0) - f;
	var	nx = this.interval - tm % this.interval;

	this.started = (st >= this.first);
	if	(!this.started)
	{
		var	tl = this.first - st;
		this.stDays = Math.floor((tl - (tl % 86400)) / 86400);
		this.next = this.first;
		this.remaining = this.next - (st + this.stDays * 86400);
		this.last = -1;
		return;
	}

	if	(this.stopTime > -1)
	{
		var	s = (this.stopTime - this.first);
		var	m = s % this.interval;
		this.last = this.first + s - m
	}
	else
		this.last = -1;

	if	(this.last != -1 && nx + st > this.last)
	{
		this.next = -1;
		this.previous = this.last;
	}
	else
	{
		this.next = nx + st;
		this.previous = this.next - this.interval;
		this.remaining = nx;
	}
}

function	Tick_draw()
{
	var	str = makeNextText(this.name);
	if	(this.next != -1)
	{
		if	(!this.started)
			str += '<b>' + this.stDays + '</b> ' + getDaysText(this.stDays > 1) + ', ';
		str += '<b>'

		var	rh = (this.remaining - (this.remaining % 3600)) / 3600,
			rm = (this.remaining - rh*3600 - (this.remaining % 60)) / 60,
			rs = this.remaining - (rh*60+rm)*60;
		var	s = rh.toString();
		if	(s.length == 1)
			s = '0' + s;
		str += s + ':';
		s = rm.toString();
		if	(s.length == 1)
			s = '0' + s;
		str += s + ':';
		s = rs.toString();
		if	(s.length == 1)
			s = '0' + s;
		str += s;
	}
	else
		str = '<b>N/A';
	str += '</b>';
	return	str;
}

function initPage() {
	drawUniversePage(document.getElementById('init-data').value);
}

function	drawUniversePage(data)
{
	var	l = data.split('\n');

	// Get universe information
	var	a = l.shift().split('#');
	document.getElementById('npl').innerHTML = formatNumber(a[0]);
	document.getElementById('nnpl').innerHTML = formatNumber(a[1]);
	document.getElementById('nnsys').innerHTML = formatNumber(a[2]);
	document.getElementById('avgf').innerHTML = formatNumber(a[3]);
	document.getElementById('avgt').innerHTML = formatNumber(a[4]);

	// Get protection zone information
//	a = l.shift().split('#');
/*	document.getElementById('sznpl').innerHTML = formatNumber(a[0]);
	document.getElementById('sznnpl').innerHTML = formatNumber(a[1]);
	document.getElementById('sznnsys').innerHTML = formatNumber(a[2]);
	document.getElementById('szavgf').innerHTML = formatNumber(a[3]);
	document.getElementById('szavgt').innerHTML = formatNumber(a[4]);
	// Time left in protection should be drawn in the LD file */

	// Rankings
	a = l.shift().split('#');
	document.getElementById('rknplayers').innerHTML = formatNumber(a[0]);
	document.getElementById('genpts').innerHTML = formatNumber(a[1]);
	document.getElementById('genrank').innerHTML = '#' + formatNumber(a[2]);
	document.getElementById('civpts').innerHTML = formatNumber(a[3]);
	document.getElementById('civrank').innerHTML = '#' + formatNumber(a[4]);
	document.getElementById('finpts').innerHTML = formatNumber(a[5]);
	document.getElementById('finrank').innerHTML = '#' + formatNumber(a[6]);
	document.getElementById('milpts').innerHTML = formatNumber(a[7]);
	document.getElementById('milrank').innerHTML = '#' + formatNumber(a[8]);
	drawRoundRankings(a[9],a[10]);
	document.getElementById('idpts').innerHTML = formatNumber(a[11]);
	document.getElementById('idrank').innerHTML = '#' + formatNumber(a[12]);

	// Time to ticks
	a = l.shift().split('#');
	var	now = Math.round((new Date().getTime()) / 1000);
	var	str = '';
	stDiff = now - parseInt(a.shift(),10);
	ticks = new Array();
	while	(l.length > 0)
	{
		a = l.shift().split('#');
		var	t = new Tick(a.shift(), a.shift(), a.shift(), a.shift(), a.join('#'));
		ticks.push(t);
		t.compute(now - stDiff);
		str += t.draw() + "<br/>";
	}
	document.getElementById('ticks').innerHTML = str;

	nUpdates = 0;
	setTimeout('updateDisplay()', 1000);
}


function	updateDisplay()
{
	var	now = Math.round((new Date().getTime()) / 1000) - stDiff;
	var	i, str = '';
	for	(i=0;i<ticks.length;i++)
	{
		ticks[i].compute(now);
		str += ticks[i].draw() + "<br/>";
	}
	document.getElementById('ticks').innerHTML = str;

	nUpdates ++;
	if	(nUpdates == 12)
		setTimeout('x_getInformation(drawUniversePage)', 1000);
	else
		setTimeout('updateDisplay()', 1000);
}
