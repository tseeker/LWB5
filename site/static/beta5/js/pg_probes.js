//--------------------------------------------------------------------
// Initialisation code

var	page;

function	initPage()
{
	var	e = document.getElementById('pinit');
	if	(!e)
		return;
	page = new Page(e.innerHTML);
	page.action(['update']);
}


//--------------------------------------------------------------------
// Page management

function	Page(cPage)
{
	this.action	= Page_action;
	this.init	= Page_init;
	this.update	= Page_update;
	this.updated	= Page_updated;
	this.drawLayout	= Page_drawLayout;
	this.startTimer	= Page_startTimer;
	this.setPage	= Page_setPage;

	this.lockLock	= false;
	this.timerLock	= false;
	this.timer	= null;

	this.menu	= new Menu(cPage);
	this.page	= null;

	this.init();
	this.drawLayout();
}

function	Page_action(args)
{
	var	tl = true;
	while	(tl)
	{
		while	(this.lockLock) ;
		this.lockLock = true;
		tl = this.timerLock;
		if	(!tl)
		{
			clearTimeout(this.timer);
			this.timerLock = true;
		}
		this.lockLock = false;
	}

	var	x = 'this.' + args.shift() + '(', i;
	for	(i=0;i<args.length;i++)
		x += (i>0?',':'') + 'args[' + i + ']';
	eval(x + ')');
}

function	Page_setPage(nPage)
{
	this.menu.cPage = nPage;
	this.init();
//	this.menu.draw();
	this.drawLayout();
	this.update();
}

function	Page_init()
{
	var	classes = [/*'PolicyPage',*/ 'BeaconsPage'];
	var	i;

	for	(i=0;i<classes.length&&this.menu.ids[i]!=this.menu.cPage;i++) ;
	if	(i == classes.length)
		return;
	eval('this.page = new ' + classes[i] + '(this)');
}

function	Page_update()
{
	x_getPageData(this.menu.cPage, function(d) { page.updated(d); });
}

function	Page_updated(data)
{
	if	(this.updateHeader)
	{
		this.updateHeader = false;
		updateHeader();
	}
	this.page.parse(data);
	this.startTimer();
}

function	Page_drawLayout()
{
//	this.menu.draw();
	document.getElementById('prpage').innerHTML = this.page.drawLayout();
}

function	Page_startTimer()
{
	while	(this.lockLock) ;
	this.lockLock = true;
	this.timer = setTimeout("page.action(['update'])", 60000);
	this.timerLock = false;
	this.lockLock = false;
}

function	ajaxCallback(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		var	a = data.split('#');
		a.shift();
		page.updateHeader = false;
		page.page.error(a);
		page.startTimer();
	}
	else
		page.updated(data);
}


//--------------------------------------------------------------------
// Menu

function	Menu(cPage)
{
	this.cPage	= cPage;
	this.ids	= [/*'policy',*/'beacons'];
	this.titles	= pgTitles;

	this.draw	= Menu_draw;
}

function	Menu_draw()
{
	var	str = '', i;

	for	(i=0;i<this.ids.length;i++)
	{
		if	(i > 0)
			str += ' - ';
		if	(this.cPage == this.ids[i])
			str += '<b>';
		else
			str += '<a href="#" onclick="page.action([\'setPage\', \'' + this.ids[i] + '\']);return false">';
		str += this.titles[i] + (this.cPage == this.ids[i] ? "</b>" : "</a>");
	}

	document.getElementById('prmenu').innerHTML = str;
}


//--------------------------------------------------------------------
// Policy page


function	PolicyPage(main)
{
	this.error		= PolicyPage_error;
	this.drawLayout		= PolicyPage_drawLayout;
	this.updateEmpire	= PolicyPage_updateEmpire;
	this.updatePlanetList	= PolicyPage_updatePlanetList;
	this.updatePlanet	= PolicyPage_updatePlanet;

	this.parsePolicy	= function(s) { var p=new Array(),i; for(i=0;i<4;i++) p[i]=s.charAt(i); return p; };
	this.parse		= PolicyPage_parse;
	this.parseMain		= PolicyPage_parseMain;

	this.setEmpPolicy	= PolicyPage_setEmpPolicy;
	this.setPlPolicy	= PolicyPage_setPlPolicy;
	this.togglePlanet	= PolicyPage_togglePlanet;

	this.main		= main;
	this.policy		= null;
	this.planets		= null;
	this.updateHeader	= false;
	this.pLines		= ['ta', 'am', 'ot', 'en'];
}


function	PolicyPage_parse(data)
{
	var	l = data.split('\n');
	var	t = l.shift();
	if	(t == "0")
		this.parseMain(l);
	else if	(t == "1")
	{
		this.policy = this.parsePolicy(l[0]);
		this.updateEmpire();
	}
	else if	(t == "2")
	{
		var i;
		t = l[0].split('#');
		for	(i=0;i<this.planets.length && this.planets[i][0] != t[0];i++) ;
		if	(i<this.planets.length)
		{
			if	(t[1] == "")
				this.planets[i][3] = null;
			else
				this.planets[i][3] = this.parsePolicy(t[1]);
			this.updatePlanet(i);
		}
	}
}

function	PolicyPage_parseMain(l)
{
	var	a = l.shift().split('#');
	var	nPlanets = parseInt(a[0],10);
	this.policy = this.parsePolicy(a[1]);

	this.planets = new Array();
	while	(nPlanets > 0)
	{
		a = l.shift().split('#');
		var p = new Array();
		p[0] = a.shift();
		p[1] = '<b>' + a.shift() + ',' + a.shift() + '</b>,' + a.shift();
		if	(a[0] == "")
			a.shift();
		else
			p[3] = this.parsePolicy(a.shift());
		p[2] = a.join('#');
		this.planets.push(p);
		nPlanets --;
	}

	this.updateEmpire();
	this.updatePlanetList();
	for	(a=0;a<this.planets.length;a++)
		this.updatePlanet(a);
}


function	PolicyPage_drawLayout()
{
	var	str, i;
	str = '<h1>' + empPolTitle + '</h1><p>';

	for	(i=0;i<this.pLines.length;i++)
	{
		if	(i>0)
			str += '<br/>';

		var	j, idpfx = "ep" + this.pLines[i];
		for	(j=3;j>0;j--)
			str += '<input type="radio" name="'+idpfx+'" id="'+idpfx+(j-1)+'" value="' + (j-1)
				+ '" onclick="page.action([\'page.setEmpPolicy\','+i+','+(j-1)+']);return true" '
				+' /><label for="'+idpfx+(j-1)+'">' + polValue[j-1] + '</label> ';
		str += polTexts[i]
	}

	str += '</p><h1>' + plaPolTitle + '</h1><div id="plapol">&nbsp;</div>';

	return	str;
}

function	PolicyPage_updateEmpire()
{
	var	i;
	for	(i=0;i<this.pLines.length;i++)
	{
		var	j, idpfx = "ep" + this.pLines[i];
		for	(j=0;j<3;j++)
			document.getElementById(idpfx+j).checked = (this.policy[i] == j);
	}
}

function	PolicyPage_updatePlanetList()
{
	var	str = '';
	if	(this.planets.length == 0)
		str = '<p>'+noPlanets+'</p>';
	else
	{
		for	(i=0;i<this.planets.length;i++)
		{
			var	p = this.planets[i];
			var	pfx = "pl"+p[0];
			str += '<table class="polpl"><tr><th>' + planetTxt + ' <a href="planet?id=' + p[0] + '">'
				+ p[2] + '</a> (' + p[1] + ')</th><td><input type="checkbox" name="' + pfx + 'up" value="1"'
				+ (p[3] ? ' checked="checked"' : '') + ' onclick="page.action([\'page.togglePlanet\','
				+ p[0] +'])" id="' + pfx + 'up" /><label for="' + pfx + 'up">' + polUse + '</label></td></tr>'
				+ '<tr><td colspan="2" id="'+pfx+'t">&nbsp;</td></tr></table>';
		}
	}
	document.getElementById('plapol').innerHTML = str;
}

function	PolicyPage_updatePlanet(index)
{
	var	i, j, p = this.planets[index], pfx = "pl" + p[0], up = (typeof p[3] == "object" && p[3]), e, ht;
	document.getElementById(pfx + 'up').checked = up;
	ht = (document.getElementById(pfx+'t').innerHTML != '&nbsp;');
	e = document.getElementById(pfx+'ta0');

	if	(up && !(e && ht))
	{
		var	str = '<p>';
		for	(i=0;i<this.pLines.length;i++)
		{
			var	idpfx = pfx + this.pLines[i];
			if	(i>0)
				str += '<br/>';
			for	(j=3;j>0;j--)
				str += '<input type="radio" name="'+idpfx+'" id="'+idpfx+(j-1)+'" value="' + (j-1)
					+ '" onclick="page.action([\'page.setPlPolicy\','+p[0]+','+i+','+(j-1)+']);return true"'
					+' /><label for="'+idpfx+(j-1)+'">' + polValue[j-1] + '</label> ';
			str += polTexts[i];
		}
		document.getElementById(pfx+'t').innerHTML = str + '</p>';
	}
	else if	(!up && (e || !ht))
		document.getElementById(pfx+'t').innerHTML = '<p>' + polUseEmpire + '</p>';

	if	(!up)
		return;

	for	(i=0;i<this.pLines.length;i++)
	{
		var	idpfx = pfx + this.pLines[i];
		for	(j=0;j<3;j++)
			document.getElementById(idpfx + j).checked = (j == p[3][i]);
	}
}


function	PolicyPage_setEmpPolicy(elem, val)
{
	if	(this.policy[elem] == val)
	{
		this.main.startTimer();
		return;
	}
	x_setEmpirePolicy(elem, val, ajaxCallback);
}


function	PolicyPage_togglePlanet(id)
{
	x_togglePlanetPolicy(id, ajaxCallback);
}


function	PolicyPage_setPlPolicy(id, elem, val)
{
	x_setPlanetPolicy(id, elem, val, ajaxCallback);
}


//--------------------------------------------------------------------
// Beacons page data


function	BP_Planet(id,x,y,orbit,current,upgrade,built,nHs,nMv,name)
{
	this.id		= id;
	this.name	= name;
	this.coords	= "<b>"+x+","+y+"</b>,"+orbit;
	this.cLevel	= parseInt(current,10);
	this.upgrade	= (upgrade == "" ? "" : formatNumber(upgrade));
	this.built	= (built == 1);
	this.nHS	= parseInt(nHs,10);
	this.nMv	= parseInt(nMv,10);

	this.hsFleets	= new Array();
	this.hsMove	= new Array();
}


//--------------------------------------------------------------------
// Beacons page


function	BeaconsPage(main)
{
	this.error		= BeaconsPage_error;
	this.drawLayout		= function() { return ""; };

	this.parse		= BeaconsPage_parse;
	this.parsePlanet	= BeaconsPage_parsePlanet;

	this.drawAll		= BeaconsPage_drawAll;
	this.drawPlanet		= BeaconsPage_drawPlanet;

	this.upgrade		= BeaconsPage_upgrade;
	this.confirmUpgrade	= BeaconsPage_confirmUpgrade;

	this.main		= main;
	this.planets		= new Hashtable();
	this.techLevel		= 0;
}


function	BeaconsPage_parse(data)
{
	var	l = data.split("\n");
	var	mode = l.shift();

	if	(mode == "0")
	{
		var	a = l.shift().split('#');
		this.techLevel = parseInt(a.shift(), 10);
		a = parseInt(a.shift(), 10);
		while	(a > 0)
		{
			this.parsePlanet(l);
			a --;
		}
		this.drawAll();
	}
	else if	(mode == "1")
	{
		var id = this.parsePlanet(l);
		this.drawPlanet(id);
	}
}

function	BeaconsPage_parsePlanet(data)
{
	var	a = data.shift().split('#');
	var	p = new BP_Planet(a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.join('#'));
	this.planets.put(p.id, p);

	if (p.nHS > 0) {
		for (var i = 0; i < p.nHS; i ++) {
			var f = {};
			a = data.shift().split('#');
			f.level = parseInt(a.shift(), 10);
			f.size = a.shift();
			f.ownerID = a.shift();
			f.ownerName = a.shift();
			p.hsFleets.push(f);
		}
	}

	return	p.id;
}


function	BeaconsPage_drawAll()
{
	var	i, keys = this.planets.keys();
	var	str = '';
	for	(i=0;i<keys.length;i++)
		str += '<div id="bppl'+keys[i]+'">&nbsp;</div>';
	document.getElementById('prpage').innerHTML = str;
	for	(i=0;i<keys.length;i++)
		this.drawPlanet(keys[i]);
}

function	BeaconsPage_drawPlanet(id)
{
	var	e = document.getElementById('bppl' + id), p = this.planets.get(id);
	if	(!p)
		return;
	if	(!e)
	{
		this.drawAll();
		return;
	}

	var	str;
	str = '<table class="plbcn"><tr><th class="bcnpl">Planet <a href="planet?id=' + id + '">'
		+ p.name + '</a> (' + p.coords + ')</th><td colspan="2">' + bcnCurrent[p.cLevel] + '</td></tr>';

	if (p.nHS > 0) {
		str += '<tr><th colspan="3" style="font-size:115%">Fleets detected!</th></tr>'
			+ '<tr><th>Information level</th><th>Fleet size</th><th>Fleet owner</th></tr>';
		for (var i = 0; i < p.nHS; i ++) {
			var f = p.hsFleets[i];
			str += '<tr><td style="text-align:center">' + bcnInfoLevel[f.level]
				+ '</td><td style="text-align:center">'
				+ (f.size == '' ? bcnUnknown : formatNumber(f.size))
				+ '</td><td style="text-align:center">'
				+ (f.ownerID == '' ? bcnUnknown : (
					'<a href="message?a=c&ct=0&id=' + f.ownerID + '">' + f.ownerName + '</a>'))
				+ '</td></tr>';
		}
	}

	str += '<tr><td colspan="3">';
	if	(p.upgrade == "")
		str += bcnNoUpgrade;
	else if	(p.built)
		str += bcnAlreadyBuilt;
	else
	{
		var	u = bcnUpgrade;
		u = u.replace(/__B__/, bcnCurrent[p.cLevel+1]);
		u = u.replace(/__C__/, p.upgrade);
		str += u + ' <input type="button" onclick="page.action([\'page.upgrade\',' + id + '])" name="bcnup' + id
			+ '" value="' + bcnUpgradeBtn + '" />';
	}

	str += '</table>';
	e.innerHTML = str;
}


function	BeaconsPage_upgrade(id)
{
	var	p = this.planets.get(id);
	if	(!(p && this.confirmUpgrade(p)))
	{
		this.main.startTimer();
		return;
	}

	this.main.updateHeader = true;
	x_upgradeBeacon(id, ajaxCallback);
}
