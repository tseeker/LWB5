var	gameType;
var	map;
var	maptt;

var	centreMode = 1;
var	centreOnTxt;
var	centreOnOwn;

var	plPlanets;
var	inUpdate = true;


//--------------------------------------------------------------------------------------
// PLANETS AND NEBULAS
//--------------------------------------------------------------------------------------

function	Planet(system,id,name,status,relation,tag)
{
	this.id		= id;
	this.name	= name.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
	this.status	= status;
	this.opacity	= (status == "1") ? 1 : 0;
	this.relation	= relation;
	this.tag	= tag;
	this.system	= system;

	this.drawName	= Planet_drawName;
	this.drawTag	= Planet_drawTag;
}

function	Planet_drawName()
{
	var	str = '<tr class="srow">';
	if (this.opacity) {
		str += '<td class="pimg"' + (this.system.protection ? ' style="background-color:#00274f"' : '')
			+ '><img src="' + staticurl + '/beta5/pics/prem_s.png" alt="[P]" /></td><td class="prem"'
			+ (this.system.protection ? ' style="background-color:#00274f"' : '') + '>';
	} else {
		str += '<td class="pimg"' + (this.system.protection ? ' style="background-color:#00274f"' : '')
			+ '><img src="' + staticurl + '/beta5/pics/pl/s/' + this.id
			+ '.png" alt="[P]" /></td><td class="planet'
			+ (this.relation == 0 ? 'n' : (this.relation == 1 ? 'a' : 'o')) + '"'
			+ (this.system.protection ? ' style="background-color:#00274f"' : '') + '>';
	}
	str += '<a href="planet?id='+this.id+'">' + this.name + '</a></td></tr>';
	return	str;
}

function	Planet_drawTag()
{
	var	str = '<tr class="srow">';
	if (this.opacity) {
		str += '<td class="pimg"><img src="' + staticurl
			+ '/beta5/pics/prem_s.png" alt="[P]" /></td><td class="prem">N/A</td></tr>';
	} else {
		str += '<td class="pimg"' + (this.system.protection ? ' style="background-color:#00274f"' : '')
			+ '><img src="' + staticurl + '/beta5/pics/pl/s/' + this.id
			+ '.png" alt="[P]" /></td><td class="planet'
			+ (this.relation == 0 ? 'n' : (this.relation == 1 ? 'a' : 'o')) + '"'
			+ (this.system.protection ? ' style="background-color:#00274f"' : '') + '>'
			+ '[' + this.tag + ']</td></tr>';
	}
	return	str;
}


function	Nebula(id,opacity,name)
{
	this.id		= id;
	this.name	= name;
	this.opacity	= opacity;

	this.drawName	= Nebula_drawName;
	this.drawTag	= Nebula_drawTag;
}

function	Nebula_drawName()
{
	var	str = '<tr class="srow"><td class="nebzone';
	str += this.opacity + '"><a href="planet?id='+this.id+'">' + this.name + '</a></td></tr>';
	return	str;
}

function	Nebula_drawTag()
{
	var	str = '<tr class="srow"><td class="nebzone';
	str += this.opacity + '">N/A</td></tr>';
	return	str;
}


//--------------------------------------------------------------------------------------
// SYSTEMS AND UNCHARTED SPACE
//--------------------------------------------------------------------------------------

function	System(id,x,y,md5,nebula,prot)
{
	this.id			= id;
	this.x			= parseInt(x, 10);
	this.y			= parseInt(y, 10);
	this.md5		= md5;
	this.nebula		= nebula;
	this.protection		= (prot != '0');
	this.planets		= new Array();
	this.update		= Math.round((new Date().getTime()) / 1000);

	this.drawPlanets	= System_drawPlanets;
	this.drawAlliances	= System_drawAlliances;
}

function	System_drawPlanets()
{
	var	str = '<table class="sys';
	if	(this.nebula > 0)
		str += 'neb' + this.nebula;
	else if	(this.prot > 0)
		str += 'prot';
	str += '" cellspacing="0" cellpadding="0">';

	var	i;
	for	(i=0;i<this.planets.length;i++)
		str += this.planets[i].drawName();

	return	str + '</table>';
}

function	System_drawAlliances()
{
	var	str = '<table class="sys';
	if	(this.nebula > 0)
		str += 'neb' + this.nebula;
	else if	(this.prot > 0)
		str += 'prot';
	str += '" cellspacing="0" cellpadding="0">';

	var	i;
	for	(i=0;i<this.planets.length;i++)
		str += this.planets[i].drawTag();

	return	str + '</table>';
}


function	Uncharted()
{
	this.md5		= '-';
	this.update		= Math.round((new Date().getTime()) / 1000);
	this.drawPlanets	= Uncharted_getCode;
	this.drawAlliances	= Uncharted_getCode;
}

function	Uncharted_getCode()
{
	var	i, str = '<table class="sysunch" cellspacing="0" cellpadding="0">';
	for	(i=0;i<6;i++)
		str += '<tr class="srow"><td>'+uncharted+'</td></tr>';
	return	str + '</table>';
}


//--------------------------------------------------------------------------------------
// THE MAP
//--------------------------------------------------------------------------------------

function	MapListItem(s,p,o)
{
	this.system	= s;
	this.planet	= p;
	this.orbit	= o;
}

function	Map(mode)
{
	this.systems		= new Hashtable();
	this.cx			= false;
	this.cy			= false;
	this.size		= false;
	this.refresh		= '';
	this.mode		= mode;

	this.list		= new Array();
	this.sortType		= 1;
	this.sortDir		= false;

	this.setParm		= Map_setParm;
	this.checkUpdates	= Map_checkUpdates;
	this.parse		= Map_parse;
	this.clearCache		= Map_clearCache;
	this.draw		= Map_draw;

	this.mapGridBegin	= Map_gridBegin;
	this.mapGridEnd		= Map_gridEnd;
	this.mapListBegin	= Map_listBegin;
	this.mapListEnd		= Map_listEnd;

	this.mapList		= Map_list;
	this.mapPlanets		= Map_planets;
	this.mapAlliances	= Map_alliances;
}

function	Map_setParm(cx,cy,size)
{
	if	(cx == this.cx && cy == this.cy && size == this.size)
		return;

	this.cx = cx;
	this.cy = cy;
	this.size = size;

	var	mod;
	mod = this.size % 2;
	this.minX = this.cx + (mod - this.size) / 2;
	this.maxX = this.cx + (this.size + mod - 2) / 2;
	this.minY = this.cy + (mod - this.size) / 2;
	this.maxY = this.cy + (this.size + mod - 2) / 2;

	this.checkUpdates();
}

function	Map_checkUpdates()
{

	var	i,j,as = new Array(), str;
	var	now = Math.round((new Date().getTime()) / 1000);
	for	(i=this.minX;i<=this.maxX;i++) for (j=this.minY;j<=this.maxY;j++) 
	{
		var	str = i + ',' + j, s = this.systems.get(str);

		if	(!s)
			as.push(str + ',');
		else if	(now - s.update > 60)
		{
			as.push(str + ',' + s.md5);
			s.update = now;
		}
	}
	this.refresh = as.join('#');
}

function	Map_clearCache()
{
	var	i, s, lk = this.systems.keys(), nk = 0;
	var	now = Math.round((new Date().getTime()) / 1000);
	var	Mx=this.maxX+4,mx=this.minX-4, My=this.maxY+4,my=this.minY-4;

	for	(i=0;i<lk.length;i++)
	{
		s = this.systems.get(lk[i]);
		if	(!s || (s.x<=Mx&&s.x>=mx&&s.y<=My&&s.y>=my) || nk > 50 || now-s.update<300)
		{
			nk ++;
			continue;
		}
		this.systems.remove(lk[i]);
	}
}

function	Map_parse(data)
{
	var	l = data.split('\n');
	var	st = 0, s, pdat, i;
	var	mod = '#';

	while	(l.length)
	{
		i = l.shift();
		if	(st == 0)
		{
			var	a = i.split('#');
			if (a[0] == '') {
				s = new Uncharted();
			} else {
				s = new System(a[0], a[1], a[2], a[3], a[4], a[5]);
				st = (a[4] > 0) ? 3 : 1;
			}
			this.systems.put(a[1] + "," + a[2], s);
			mod += a[1] + "," + a[2] + '#';
		} else if (st == 1) {
			pdat = i.split('#');
			st ++;
		}
		else if	(st == 2)
		{
			s.planets.push(new Planet(s, pdat.shift(), i, pdat.shift(), pdat.shift(), pdat.join('#')));
			st = (s.planets.length == 6) ? 0 : 1;
		}
		else if	(st == 3)
		{
			pdat = i.split('#');
			s.planets.push(new Nebula(pdat.shift(), pdat.shift(), pdat.join('#')));
			if	(s.planets.length == 6)
				st = 0;
		}
	}

	this.refresh = "";
}

function	Map_draw()
{
	var	str;
	switch	(this.mode)
	{
		case 0: str = this.mapGridBegin() + this.mapPlanets() + this.mapGridEnd(); break;
		case 1: str = this.mapGridBegin() + this.mapAlliances() + this.mapGridEnd(); break;
		case 2: str = this.mapListBegin() + this.mapList() + this.mapListEnd(); break;
	}
	document.getElementById('jsmapint').innerHTML = str;
}

function	Map_gridBegin()
{
	var	i, tmp = "<table class='map' id='msz" + this.size + "' cellspacing='0'>";
	tmp += "<tr class='hd'><td class='idx'>&nbsp;</td><td class='idx'>-X</td>";
	tmp += "<td colspan='"+this.size+"' class='arrow' onClick='moveUp()' " + maptt[10] + ">";
	tmp += "<img src='"+staticurl+"/beta5/pics/up_"+color+".gif' alt='"+dirs[0]+"' /></a></td>";
	tmp += "<td class='idx'>X</td><td class='idx'>&nbsp;</td></tr>";

	tmp += "<tr class='hd'><td class='idx'>Y</td><td class='idx'></td>";
	for	(i=this.minX;i<=this.maxX;i++)
		tmp += "<td class='mgrcol'>"+i+"</td>";
	tmp += "<td class='idx'></td><td class='idx'>Y</td></tr>";
	return	tmp;
}

function	Map_gridEnd()
{
	var	i, tmp = "<tr class='hd'><td class='idx'>-Y</td><td class='idx'>&nbsp;</td>";
	for	(i=this.minX;i<=this.maxX;i++)
		tmp += "<td class='mgrcol'>"+i+"</td>";
	tmp += "<td class='idx'>&nbsp;</td><td class='idx'>-Y</td></tr>";
	tmp += "<tr class='hd'><td class='idx'>&nbsp;</td><td class='idx'>-X</td>";
	tmp += "<td colspan='"+this.size+"' class='arrow' onClick='moveDown()' " + maptt[13] + ">";
	tmp += "<img src='"+staticurl+"/beta5/pics/down_"+color+".gif' alt='"+dirs[1]+"' /></a></td><td class='idx'>X</td>";
	tmp += "<td class='idx'>&nbsp;</td></tr>";
	tmp += "</table>";
	return	tmp;
}

function	Map_planets()
{
	var	j, i, str = '';
	for	(j=this.maxY;j>=this.minY;j--)
	{
		str += '<tr>';
		if	(j == this.maxY)
		{
			str += "<td rowspan='"+this.size+"' class='arrow' onClick='moveLeft()' " + maptt[11] + ">";
			str += "<img src='"+staticurl+"/beta5/pics/left_"+color+".gif' alt='"+dirs[2]+"' /></td>";
		}
		str += "<td class='hd'>"+j+"</td>";
		for	(i=this.minX;i<=this.maxX;i++)
			str += '<td class="system">' + this.systems.get(i+','+j).drawPlanets() + '</td>';
		str += "<td class='hd'>"+j+"</td>";
		if	(j == this.maxY)
		{
			str += "<td rowspan='"+this.size+"' class='arrow' onClick='moveRight()' " + maptt[12] + ">";
			str += "<img src='"+staticurl+"/beta5/pics/right_"+color+".gif' alt='"+dirs[3]+"' /></td>";
		}
		str += '</tr>';
	}
	return	str;
}

function	Map_alliances()
{
	var	j, i, str = '';
	for	(j=this.maxY;j>=this.minY;j--)
	{
		str += '<tr>';
		if	(j == this.maxY)
		{
			str += "<td rowspan='"+this.size+"' class='arrow' onClick='moveLeft()' " + maptt[11] + ">";
			str += "<img src='"+staticurl+"/beta5/pics/left_"+color+".gif' alt='"+dirs[2]+"' /></td>";
		}
		str += "<td class='hd'>"+j+"</td>";
		for	(i=this.minX;i<=this.maxX;i++)
			str += '<td class="system">' + this.systems.get(i+','+j).drawAlliances() + '</td>';
		str += "<td class='hd'>"+j+"</td>";
		if	(j == this.maxY)
		{
			str += "<td rowspan='"+this.size+"' class='arrow' onClick='moveRight()' " + maptt[12] + ">";
			str += "<img src='"+staticurl+"/beta5/pics/right_"+color+".gif' alt='"+dirs[3]+"' /></td>";
		}
		str += '</tr>';
	}
	return	str;
}

function	Map_listBegin()
{
	var	hdTypes = ['Coord','Name','Alliance','Opacity','Protected'];
	var	hdCols = ['', ' class="pname" colspan="2"', '', '', ''];
	var	i,j,o;

	this.list = new Array();
	for	(i=this.minX;i<=this.maxX;i++) for (j=this.minY;j<=this.maxY;j++) 
	{
		var	s = this.systems.get(i+','+j);
		if	(!s || !s.planets)
			continue;
		for	(k=0;k<6;k++)
			this.list.push(new MapListItem(s,s.planets[k],k+1));
	}
	var	smet = 'this.list.sort(spl_' + hdTypes[this.sortType] + '_' + (this.sortDir ? "desc" : "asc") + ')';
	eval(smet);

	var	str = '<table class="list" cellspacing="0" cellpadding="0" id="plist"><tr>';
	if (hdNames.length == 4 && gameType != 1) {
		hdNames.push(lcHeader[gameType]);
	}
	for (i = 0; i < hdNames.length; i ++) {
		str += '<th' + hdCols[i] + ' ' + maptt[20] + ' onClick="map.sortType=' + i
			+ ';map.sortDir=!map.sortDir;map.draw()">';
		if (this.sortType == i) {
			str += '<b>';
		}
		str += hdNames[i];
		if (this.sortType == i) {
			str += '</b><img src="'+staticurl+'/beta5/pics/';
			str += this.sortDir ? "up" : "down";
			str += '_' + color + '.gif" alt="' + (this.sortDir ? dirs[0] : dirs[1]) + '" />';
		}
		str += '</th>';
	}
	str += '</tr>';

	return	str;
}

function	Map_listEnd()
{
	return	'</table>';
}

function Map_list() {
	var	i, str = '';
	for (i = 0; i < this.list.length; i ++) {
		str += '<tr class="';
		if (this.list[i].system.nebula != 0) {
			str += 'nebzone' + this.list[i].planet.opacity + '"';
		} else if (this.list[i].planet.opacity == 0) {
			str += 'planet' + (this.list[i].planet.relation == 0 ? 'n'
				: (this.list[i].planet.relation == 1 ? 'a' : 'o')) + '"';
		} else {
			str += 'prem"';
		}
		str += '><td>(<b>' + this.list[i].system.x + ',' + this.list[i].system.y + '</b>,';
		str += this.list[i].orbit + ')</td>';
		str += '<td';
		if (this.list[i].system.nebula == 0) {
			if (this.list[i].planet.opacity == 0) {
				str += ' class="pimg"><img src="' + staticurl + '/beta5/pics/pl/s/'
					+ this.list[i].planet.id + '.png" alt="[P]" /></td><td class="pname">';
			} else {
				str += ' class="pimg"><img src="' + staticurl
					+ '/beta5/pics/prem_s.png" alt="[P]" /></td><td class="pname">';
			}
		} else {
			str += ' class="pname" colspan="2">';
		}
		str += '<a href="planet?id=' + this.list[i].planet.id + '" ' + maptt[30] + ' >'
			+ this.list[i].planet.name + '</a></td><td>';
		if (this.list[i].system.nebula == 0) {
			str += '[' + this.list[i].planet.tag + ']';
		} else {
			str += '-';
		}
		str += '</td><td>' + this.list[i].planet.opacity;
		if (gameType != 1) {
			str += '</td><td>' + (this.list[i].system.protection == 0 ? yesNo[1] : yesNo[0]);
		}
		str += '</td></tr>';
	}
	return	str;
}


//--------------------------------------------------------------------------------------
// INITIALISATION AND GENERIC FUNCTIONS
//--------------------------------------------------------------------------------------

function	PlayerPlanet(id, name)
{
	this.id = id;
	this.name = name;
}

function	mapInit(data)
{
	gameType = document.getElementById('gametype').innerHTML;

	// Receiving: mode followed by x#y#is_id#id_or_name followed by (id#name)*
	var	l = data.split('\n');
	var	m = parseInt(l.shift(), 10);
	var	a = l.shift().split('#');

	plPlanets = new Array();
	while	(l.length > 0)
	{
		var	p = l.shift().split('#');
		plPlanets.push(new PlayerPlanet(p.shift(), p.join('#')));
	}

	centreOn = (a[2] == 1) ? a[3] : "";
	centreOnOwn = (a[2] == 0) ? a[3] : 0;

	map = new Map(m);
	map.setParm(parseInt(a[0],10),parseInt(a[1],10),5);
	drawMapLayout();

	x_updateData(map.refresh, mapDataReceived);
	setTimeout('maintainMap()', 11000);
}

function	drawMapLayout()
{
	var	str = "<tr><td style='width:95%' id='jsctrls'>&nbsp;</td><td style='width:5%;vertical-align:top'><a href='manual?p=maps'>Help</a></td></tr>"
		+ "<tr><td colspan='2'><hr/></td></tr><tr><td colspan='2' id='jsmapint'>&nbsp;</td></tr>";
	document.getElementById('jsmap').innerHTML = "<table cellspacing='0' cellpadding='0'>" + str + "</table>";
	drawMapControls();
	setTimeout('x_getPlayerPlanets(playerPlanetsReceived)', 300000);
}

function	playerPlanetsReceived(data)
{
	plPlanets = new Array();
	if	(data != "")
	{
		var	l = data.split('\n');
		while	(l.length > 0)
		{
			var	p = l.shift().split('#');
			plPlanets.push(new PlayerPlanet(p.shift(), p.join('#')));
		}
	}
	drawPlayerList();
	setTimeout('x_getPlayerPlanets(playerPlanetsReceived)', 300000);
}

function	mapDataReceived(data)
{
	if	(data != "")
		map.parse(data);
	map.draw();
	inUpdate = false;
}

function	maintainMap()
{
	if	(inUpdate)
	{
		setTimeout('maintainMap()', 500);
		return;
	}
	inUpdate = true;
	map.clearCache();
	map.checkUpdates();
	refreshMap();
	setTimeout('maintainMap()', 11000);
}

function	refreshMap()
{
	if	(map.refresh == '')
	{
		map.draw();
		inUpdate = false;
	}
	else
		x_updateData(map.refresh, mapDataReceived);
}

function	resultReceived(data)
{
	if	(data == "ERR")
	{
		inUpdate = false;
		return;
	}

	var	a = data.split('#');
	if	(a[2] == 1)
	{
		centreOn = a[3];
		centreMode = 2;
		document.getElementById('cm2').checked = true;
		document.getElementById('cp').value = centreOn;
	}
	else
	{
		centreOnOwn = a[3];
		centreMode = 1;
		document.getElementById('cm1').checked = true;
		drawPlayerList();
	}
	document.getElementById('cx').value = a[0];
	document.getElementById('cy').value = a[1];

	map.setParm(parseInt(a[0],10),parseInt(a[1],10),map.size);
	refreshMap();
}

function	drawPlayerList()
{
	var	i, str = '<select name="plpl" onChange="centreOnOwn=options[selectedIndex].value">';
	str += '<option value="0">-----</option>';
	for	(i=0;i<plPlanets.length;i++)
	{
		str += '<option value="'+plPlanets[i].id+'"'+(plPlanets[i].id==centreOnOwn?' selected="selected"':'')+'>';
		str += plPlanets[i].name.replace(/&/g,'&amp;').replace(/>/g,'&gt;').replace(/</g,'&lt;') + '</option>';
	}
	str += '</select>';
	document.getElementById('plpl').innerHTML = str;
}


//--------------------------------------------------------------------------------------
// CONTROLS
//--------------------------------------------------------------------------------------

function	setSize(s)
{
	if	(inUpdate)
	{
		setTimeout('setSize('+s+')', 500);
		return;
	}
	inUpdate = true;
	map.setParm(map.cx,map.cy,parseInt(s,10));
	refreshMap();
}

function	setMode(m)
{
	if	(inUpdate)
	{
		setTimeout('setMode("'+m+'")', 500);
		return;
	}

	inUpdate = true;
	map.mode = parseInt(m,10);
	map.draw();
	inUpdate = false;
}

function	moveUp()
{
	if	(inUpdate)
	{
		setTimeout('moveUp()', 50);
		return;
	}
	inUpdate = true;
	document.getElementById('cy').value = map.cy + 1;
	document.getElementById('cm0').checked = true;
	centreMode = 0;
	map.setParm(map.cx, map.cy+1, map.size);
	refreshMap();
}

function	moveDown()
{
	if	(inUpdate)
	{
		setTimeout('moveDown()', 50);
		return;
	}
	inUpdate = true;
	document.getElementById('cy').value = map.cy - 1;
	document.getElementById('cm0').checked = true;
	centreMode = 0;
	map.setParm(map.cx, map.cy-1, map.size);
	refreshMap();
}

function	moveLeft()
{
	if	(inUpdate)
	{
		setTimeout('moveLeft()', 50);
		return;
	}
	inUpdate = true;
	document.getElementById('cx').value = map.cx - 1;
	document.getElementById('cm0').checked = true;
	centreMode = 0;
	map.setParm(map.cx-1, map.cy, map.size);
	refreshMap();
}

function	moveRight()
{
	if	(inUpdate)
	{
		setTimeout('moveRight()', 50);
		return;
	}
	inUpdate = true;
	document.getElementById('cx').value = map.cx + 1;
	document.getElementById('cm0').checked = true;
	centreMode = 0;
	map.setParm(map.cx+1, map.cy, map.size);
	refreshMap();
}

function	manualUpdate()
{
	if	(inUpdate)
	{
		setTimeout('manualUpdate()', 500);
		return;
	}
	inUpdate = true;

	if	(centreMode == 0)
	{
		var	tx, nx, ty, ny;
		tx = document.getElementById('cx').value; ty = document.getElementById('cy').value;
		if (tx == '-0') tx = '0';
		if (ty == '-0') ty = '0';
		nx = parseInt(tx,10); ny = parseInt(ty,10);
		if	(tx != nx.toString() || ty != ny.toString())
		{
			inUpdate = false;
			return;
		}
		map.setParm(nx, ny, map.size);
		refreshMap();
	}
	else if	(centreMode == 1)
	{
		var	i;
		for	(i=0;i<plPlanets.length&&plPlanets[i].id!=centreOnOwn;i++) ;
		if	(i==plPlanets.length)
		{
			inUpdate = false;
			return;
		}
		x_findName(plPlanets[i].name, resultReceived);
	}
	else if	(centreMode == 2)
		x_findName(document.getElementById('cp').value, resultReceived);
}


//--------------------------------------------------
// LIST SORTING
//--------------------------------------------------

function spl_Coord_asc(a,b)
{
	var	sup;
	if	(a.system.x!=b.system.x)
		sup = (a.system.x>b.system.x);
	else if	(a.system.y!=b.system.y)
		sup = (a.system.y>b.system.y);
	else
		sup = (a.orbit>b.orbit);
	return	sup?1:-1;
}

function spl_Coord_desc(a,b)
{
	var	sup;
	if	(a.system.x!=b.system.x)
		sup = (a.system.x>b.system.x);
	else if	(a.system.y!=b.system.y)
		sup = (a.system.y>b.system.y);
	else
		sup = (a.orbit>b.orbit);
	return	sup?-1:1;
}

function spl_Name_asc(a,b)
{
	var ra = a.planet.name.toLowerCase(); var rb = b.planet.name.toLowerCase();
	return	(ra < rb) ? -1 : 1;
}

function spl_Name_desc(a,b)
{
	var ra = a.planet.name.toLowerCase(); var rb = b.planet.name.toLowerCase();
	return	(ra > rb) ? -1 : 1;
}

function spl_Alliance_asc(a,b)
{
	var ra, rb;
	ra = (a.system.nebula == 0) ? a.planet.tag.toLowerCase() : "";
	rb = (b.system.nebula == 0) ? b.planet.tag.toLowerCase() : "";
	return	(ra < rb) ? -1 : (ra > rb ? 1 : spl_Name_asc(a,b));
}

function spl_Alliance_desc(a,b)
{
	var ra, rb;
	ra = (a.system.nebula == 0) ? a.planet.tag.toLowerCase() : "";
	rb = (b.system.nebula == 0) ? b.planet.tag.toLowerCase() : "";
	return	(ra < rb) ? 1 : (ra > rb ? -1 : spl_Name_desc(a,b));
}

function spl_Opacity_asc(a,b)
{
	var ra = parseInt(a.planet.opacity, 10); var rb = parseInt(b.planet.opacity, 10);
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : spl_Coord_asc(a,b));
}

function spl_Opacity_desc(a,b)
{
	var ra = parseInt(a.planet.opacity, 10); var rb = parseInt(b.planet.opacity, 10);
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : spl_Coord_desc(a,b));
}

function spl_Protected_asc(a,b) {
	var ra = a.system.protection, rb = b.system.protection;
	return  (ra && !rb) ? -1 : ((rb && ! ra) ? 1 : spl_Coord_asc(a, b));
}

function spl_Protected_desc(a,b) {
	var ra = a.system.protection, rb = b.system.protection;
	return  (ra && !rb) ? 1 : ((rb && ! ra) ? -1 : spl_Coord_desc(a, b));
}
