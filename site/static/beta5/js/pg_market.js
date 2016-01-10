var	currentPage, pageUpdate, layout = -1;
var	pageHandlers = ['Public','Private','Sent'];


//----------------------------------------------------------------------------------------------
// LISTINGS
//----------------------------------------------------------------------------------------------

function	ListingColumn(title,hdSpan,tdcl,custom,dc,sa,sd)
{
	this.title = title;
	this.hdSpan = hdSpan;
	this.colClass = tdcl;
	this.drawContents = (custom ? null : dc);
	this.drawColumn = (custom ? dc : null);
	this.sortAsc = sa;
	this.sortDesc = sd;
}

function	Listing(element,list,varname,slabel)
{
	this.element = element;
	this.list = list;
	this.varName = varname;
	this.searchLabel = slabel;
	this.columns = new Array();
	this.lines = new Array();
	this.sortBy = 0;
	this.sortDir = false;
	this.searchText = '';
	this.perPage = 20;
	this.indices = new Array();
	this.nPages = 0;
	this.page = 0;
	this.emptyText = notFound;
	this.notFound = notFound;

	this.addColumn = Listing_addColumn;

	this.draw = Listing_draw;
	this.searchFor = Listing_searchFor;
	this.updateDisplay = Listing_updateDisplay;
	this.setPage = Listing_setPage;
	this.setSort = Listing_setSort;

	this.updateIndices = Listing_updateIndices;
	this.makeAll = Listing_makeAll;
	this.makeList = Listing_makeList;
	this.makePage = Listing_makePage;
}

function	Listing_addColumn(column, lIdx)
{
	if	(!this.lines[lIdx])
		this.lines[lIdx] = new Array();
	this.columns.push(column);
	this.lines[lIdx].push(this.columns.length - 1);
}

function	Listing_draw()
{
	this.updateIndices();
	document.getElementById(this.element).innerHTML = this.makeAll();
}

function	Listing_searchFor(text)
{
	if	(this.searchText == text)
		return;
	this.searchText = text;
	this.updateDisplay();
}

function	Listing_updateDisplay()
{
	var	vals;
	eval('vals='+this.list);
	if	(vals.length && document.getElementById('selpage_'+this.element))
	{
		this.updateIndices();
		document.getElementById('selpage_'+this.element).innerHTML = this.makePage();
		document.getElementById('lstcontents_'+this.element).innerHTML = this.makeList();
	}
	else
		this.draw();
}

function	Listing_setPage(np)
{
	if	(this.page == np)
		return;
	this.page = np;
	document.getElementById('lstcontents_'+this.element).innerHTML = this.makeList();
}

function	Listing_setSort(ns)
{
	this.sortBy = ns;
	this.sortDir = !this.sortDir;
	this.updateDisplay();
}

function	Listing_updateIndices()
{
	var	i,vals;
	eval('vals='+this.list);

	this.indices = new Array();
	if	(this.searchText == "")
		for	(i=0;i<vals.length;i++)
			this.indices.push(i);
	else
	{
		for	(i=0;i<vals.length;i++)
			if	(vals[i].match(this.searchText))
				this.indices.push(i);
	}

	if	(this.columns.length && this.columns[this.sortBy].sortAsc)
		this.indices.sort(this.sortDir ? this.columns[this.sortBy].sortDesc : this.columns[this.sortBy].sortAsc);

	i = this.indices.length % this.perPage;
	this.nPages = (this.indices.length-i)/this.perPage + (i>0?1:0);
	if	(this.nPages > 0 && this.page >= this.nPages)
		this.page = this.nPages - 1;
}

function	Listing_makeAll()
{
	var	vals,str;
	eval('vals='+this.list);
	if	(vals.length)
	{
		var	i;
		str = '<table cellspacing="0" cellpadding="0">';
		str += '<tr><td class="lstctrl">';

		var	s2 = '<select name="perpage" onChange="'+this.varName+'.perPage=parseInt(options[selectedIndex].value,10);'+this.varName+'.updateDisplay()">';
		for	(i=1;i<6;i++)
			s2 += '<option value="'+(i*5)+'"'+(this.perPage==i*5?' selected="selected"':'')+'>'+(i*5)+'</option>';
		s2 += '</select>';

		str += listingText[0].replace(/%1%/,s2) + '</td><td class="lstctrl" id="selpage_'+this.element+'">';
		str += this.makePage() + '</td></tr><tr><td colspan="2" class="lstctrl"><label for="searchtxt">' + this.searchLabel + '</label>';
		str += '<input type="text" name="searchtxt" id="searchtxt_'+this.element+'" value="' + this.searchText.replace(/"/, '&quot;') + '" ';
		s2 = this.varName + '.searchFor(value);return true';
		str += 'onChange="'+s2+'" onKeyUp="'+s2+'" onClick="'+s2+'" /></td></tr>';

		str += '<tr><td colspan="2"><hr/></td></tr>';
		str += '<tr><td colspan="2" id="lstcontents_'+this.element+'">' + this.makeList() + '</td></tr>';

		str += '</table>';
	}
	else
		str = this.emptyText;
	return	str;
}

function	Listing_makePage()
{
	var	str;
	if	(this.nPages == 0)
		str = '&nbsp;';
	else if	(this.nPages == 1)
		str = listingText[1].replace(/%.%/g, '1');
	else
	{
		var	i,s2 = '<select name="page" onChange="'+this.varName+'.setPage(parseInt(options[selectedIndex].value,10))">';
		for	(i=0;i<this.nPages;i++)
			s2 += '<option value="'+i+'"'+(i==this.page?' selected="selected"':'')+'>'+(i+1)+'</option>';
		s2 += '</select>';
		str = listingText[1].replace(/%1%/,s2).replace(/%2%/,this.nPages);
	}
	return	str;
}

function	Listing_makeList()
{
	if	(this.indices.length == 0)
		return	'<center><br/>' + this.notFound + '<br/></center>';

	var	i,j,str = '<table class="list" id="lstcnt_'+this.element+'" cellspacing="0" cellpadding="0">';

	if	(this.lines.length > 1)
		str += '<tr><td><table class="'+this.element+'item" cellspacing="0" cellpadding="0">';
	for	(j=0;j<this.lines.length;j++)
	{
		str += '<tr>';
		for	(i=0;i<this.lines[j].length;i++)
		{var idx = this.lines[j][i];with(this.columns[idx]){
			str += '<th' + (colClass != '' ? (' class="' + colClass + '"') : '');
			if	(hdSpan)
				str += ' colspan="' + hdSpan + '"';
			if	(sortAsc && sortDesc)
				str += ' onClick="'+this.varName+'.setSort('+idx+')"';
			str += '>';
			str += (this.sortBy == idx ? '<b>' : '') + title;
			if	(this.sortBy == idx)
			{
				str += '</b><img src="' + staticurl + '/beta5/pics/';
				str += (this.sortDir ? "up" : "down") + '_' + color + '.gif" alt="';
				str += (this.sortDir ? "up" : "down") + '"/>';
			}
			str += '</th>';
		}}
		str += '</tr>';
	}
	if	(this.lines.length > 1)
		str += '</table></td></tr>';

	var	vals,k,f = this.page * this.perPage;
	var	l = Math.min(f + this.perPage, this.indices.length);
	eval('vals='+this.list);
	for	(i=f;i<l;i++)
	{
		if	(this.lines.length > 1)
			str += '<tr><td><table class="'+this.element+'item" cellspacing="0" cellpadding="0">';
		for	(k=0;k<this.lines.length;k++)
		{
			str += '<tr>';
			for	(j=0;j<this.lines[k].length;j++)
			{
				var	idx = this.lines[k][j];
				if	(this.columns[idx].drawColumn)
					str += this.columns[idx].drawColumn(vals[this.indices[i]]);
				else
				{
					str += '<td' + (this.columns[idx].colClass != '' ? (' class="' + this.columns[idx].colClass + '"') : '');
					if	(this.columns[idx].hdSpan)
						str += ' colspan="' + this.columns[idx].hdSpan + '"';
					str += '>' + this.columns[idx].drawContents(vals[this.indices[i]]);
					str += '</td>';
				}
			}
			str += '</tr>';
		}
		if	(this.lines.length > 1)
			str += '</table></td></tr>';
	}
	str += '</table>';

	return	str;
}


//------------------------------------------------------------------------
// PUBLIC OFFERS OBJECTS
//------------------------------------------------------------------------

var	mapCType, mapX, mapY, mapDist, mapParm;
var	myPlanets, sysType, sysProt, sysPlanets;
var	pOffers, fOffers, sellers;


function	SysPlanet(id,status,own,ally,name)
{
	this.id		= id;
	this.status	= status;
	this.ownership	= own ? 2 : (ally ? 1 : 0);
	this.name	= name;
}

function	Seller(id,isMe,quit,name)
{
	this.id		= id;
	this.isMe	= isMe;
	this.quit	= quit;
	this.name	= name;
}

function	PublicPlanetOffer(offer,expires,id,seller,x,y,orbit,price,auction,pop,tur,fact,name)
{
	this.offer	= offer;
	this.expires	= expires;
	this.id		= id;
	this.seller	= seller;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.price	= price;
	this.auction	= auction;
	this.population	= pop;
	this.turrets	= tur;
	this.fact	= fact;
	this.name	= name;
	this.fleet	= null;
}

function	PublicFleetOffer(offer,expires,id,seller,x,y,orbit,plId,price,auction,sg,sf,sc,sb,name)
{
	this.offer	= offer;
	this.expires	= expires;
	this.id		= id;
	this.seller	= seller;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.plId	= plId;
	this.price	= price;
	this.auction	= auction;
	this.plName	= name;
	this.fleet	= [sg, sf, sc, sb];

	this.match	= PublicFleetOffer_match;
}

function	PublicFleetOffer_match(t)
	{ return this.plName.toLowerCase().indexOf(t.toLowerCase()) != -1; } 


//------------------------------------------------------------------------
// PAGE INIT. & AUTO-UPDATE
//------------------------------------------------------------------------

function	lockUpdate(p)
{
	if	(!pageUpdate || p!=currentPage)
		return	false;
	var	p = pageUpdate;
	pageUpdate = null;
	clearTimeout(p);
	return	true;
}

function	startUpdate()
{
	if	(pageUpdate)
		return;
	pageUpdate = setTimeout('updateData()', 60000);
}

function	drawMenu()
{
	var	i,str = '';
	for	(i=0;i<menuText.length;i++)
	{
		if	(str != '')
			str += ' - ';
		str += (i == currentPage) ? '<b>' : ('<a href="#" onClick="selectPage('+i+');return false">');
		str += menuText[i] + (i == currentPage ? '</b>' : '</a>');
	}
	document.getElementById('mkppsel').innerHTML = str;
}

function	dataReceived(data)
{
	var	s = pageHandlers[currentPage];
	eval('parse' + s + 'Offers(data)');
	if	(layout != currentPage)
	{
		drawMenu();
		eval('draw' + s + 'Layout()');
		layout = currentPage;
	}
	eval('draw' + s + 'Page()');
	startUpdate();
}

function	updateData()
{
	if	(!lockUpdate(currentPage))
	{
		setTimeout('updateData()', 500);
		return;
	}
	var	s = 'x_get' + pageHandlers[currentPage] + 'Offers(dataReceived)';
	eval(s);
}

function	initMarket()
{
	var	e = document.getElementById('mkpfpage');
	if	(!e)
		return;
	currentPage = parseInt(e.innerHTML, 10);

	// Public offers, Planets
	pfsListing = new Listing('plsale', 'pOffers','pfsListing',searchText[0]);
	pfsListing.addColumn(new ListingColumn(psHeaders[0], 2, 'pname', true, drawPSName, sortPSNameAsc, sortPSNameDesc), 0);
	pfsListing.addColumn(new ListingColumn(psHeaders[1], 1, 'coords', false, drawCoords, sortPSCrdAsc, sortPSCrdDesc), 0);
	pfsListing.addColumn(new ListingColumn(psHeaders[2], 1, 'pdat', false, drawPSPop, sortPSPopAsc, sortPSPopDesc), 0);
	pfsListing.addColumn(new ListingColumn(psHeaders[3], 1, 'pdat', false, drawPSTurrets, sortPSTurAsc, sortPSTurDesc), 0);
	pfsListing.addColumn(new ListingColumn(psHeaders[4], 1, 'pdat', false, drawPSFactories, sortPSFacAsc, sortPSFacDesc), 0);
	pfsListing.addColumn(new ListingColumn(psHeaders[5], 1, 'pflt', false, drawPSFleets, sortPSFltAsc, sortPSFltDesc), 0);
	pfsListing.addColumn(new ListingColumn('&nbsp;', 1, 'lspc', false, function () { return '&nbsp;' }, null, null), 1);
	pfsListing.addColumn(new ListingColumn(psHeaders[6], 1, 'seller', false, drawSeller, sortPSSelAsc, sortPSSelDesc), 1);
	pfsListing.addColumn(new ListingColumn(psHeaders[7], 2, 'sexp', false, drawExpire, sortPSExpAsc, sortPSExpDesc), 1);
	pfsListing.addColumn(new ListingColumn(psHeaders[8], 2, 'price', false, drawPrice, sortPSPriAsc, sortPSPriDesc), 1);
	pfsListing.addColumn(new ListingColumn(psHeaders[9], 1, 'action', false, drawPSAction, sortPSActAsc, sortPSActDesc), 1);
	pfsListing.emptyText = noPSales; pfsListing.notFound = noPSalesFound;

	// Public offers, Fleets
	ffsListing = new Listing('flsale','fOffers','ffsListing',searchText[1]);
	ffsListing.addColumn(new ListingColumn(fsHeaders[0], 3, 'pname', true, drawFSLocation, sortFSLocAsc, sortFSLocDesc), 0);
	ffsListing.addColumn(new ListingColumn(fsHeaders[1], 1, 'coords', false, drawCoords, sortFSCrdAsc, sortFSCrdDesc), 0);
	ffsListing.addColumn(new ListingColumn(fsHeaders[2], 1, 'ships', false, drawFSGAShips, sortFSGASAsc, sortFSGASDesc), 0);
	ffsListing.addColumn(new ListingColumn(fsHeaders[3], 1, 'ships', false, drawFSFighters, sortFSFgtAsc, sortFSFgtDesc), 0);
	ffsListing.addColumn(new ListingColumn(fsHeaders[4], 1, 'ships', false, drawFSCruisers, sortFSCruAsc, sortFSCruDesc), 0);
	ffsListing.addColumn(new ListingColumn(fsHeaders[5], 1, 'ships', false, drawFSBCruisers, sortFSBCrAsc, sortFSBCrDesc), 0);
	ffsListing.addColumn(new ListingColumn('&nbsp;', 1, 'lspc', false, function () { return '&nbsp;' }, null, null), 1);
	ffsListing.addColumn(new ListingColumn(fsHeaders[6], 1, 'seller', false, drawSeller, sortFSSelAsc, sortFSSelDesc), 1);
	ffsListing.addColumn(new ListingColumn(fsHeaders[7], 2, 'sexp', false, drawExpire, sortFSExpAsc, sortFSExpDesc), 1);
	ffsListing.addColumn(new ListingColumn(fsHeaders[8], 2, 'price', false, drawPrice, sortFSPriAsc, sortFSPriDesc), 1);
	ffsListing.addColumn(new ListingColumn(fsHeaders[9], 2, 'action', false, drawFSAction, sortFSActAsc, sortFSActDesc), 1);
	ffsListing.emptyText = noFSales; ffsListing.notFound = noFSalesFound;

	// Pending private offers
	proListing = new Listing('prpending', 'prOffers', 'proListing', searchText[0]);
	proListing.addColumn(new ListingColumn(proHeaders[0], 1, 'rdate', false, drawPRReceived, sortPRRecAsc, sortPRRecDesc), 0);
	proListing.addColumn(new ListingColumn(proHeaders[1], 1, 'seller', false, drawSeller, sortPRSelAsc, sortPRSelDesc), 0);
	proListing.addColumn(new ListingColumn(proHeaders[2], 1, 'price', false, drawPRPrice, sortPRPriAsc, sortPRPriDesc), 0);
	proListing.addColumn(new ListingColumn(proHeaders[3], 1, 'sexp', false, drawExpire, sortPRExpAsc, sortPRExpDesc), 0);
	proListing.addColumn(new ListingColumn('&nbsp;', 1, 'lspc', false, function () { return '&nbsp;' }, null, null), 1);
	proListing.addColumn(new ListingColumn(proHeaders[4], 3, 'details', false, drawPRDetails, null, null), 1);
	proListing.emptyText = noPOffers; proListing.notFound = noPOffersFound; proListing.perPage = 5; proListing.sortDir = true;

	// Private offers history
	prhListing = new Listing('prhistory', 'prHistory', 'prhListing', searchText[0]);
	prhListing.addColumn(new ListingColumn(prhHeaders[3], 1, 'rdate', false, drawPHActionDate, sortPHADtAsc, sortPHADtDesc), 0);
	prhListing.addColumn(new ListingColumn(prhHeaders[0], 1, 'rdate', false, drawPRReceived, sortPHRecAsc, sortPHRecDesc), 0);
	prhListing.addColumn(new ListingColumn(prhHeaders[4], 1, 'hstat', false, drawPHStatus, sortPHStaAsc, sortPHStaDesc), 0);
	prhListing.addColumn(new ListingColumn(prhHeaders[1], 1, 'seller', false, drawSeller, sortPHSelAsc, sortPHSelDesc), 0);
	prhListing.addColumn(new ListingColumn(prhHeaders[2], 1, 'price', false, drawPRPrice, sortPHPriAsc, sortPHPriDesc), 0);
	prhListing.addColumn(new ListingColumn('&nbsp;', 1, 'lspc', false, function () { return '&nbsp;' }, null, null), 1);
	prhListing.addColumn(new ListingColumn(prhHeaders[5], 4, 'details', false, drawPRDetails, null, null), 1);
	prhListing.emptyText = noPHist; prhListing.notFound = noPHistFound; prhListing.perPage = 5; prhListing.sortDir = true;

	// Pending sent offers
	sopListing = new Listing('sopending', 'soPending', 'sopListing', searchText[0]);
	sopListing.addColumn(new ListingColumn(sopHeaders[0], 1, 'rdate', false, drawPRReceived, sortSOSntAsc, sortSOSntDesc), 0);
	sopListing.addColumn(new ListingColumn(sopHeaders[1], 1, 'otype', false, drawSentMode, sortSOModAsc, sortSOModDesc), 0);
	sopListing.addColumn(new ListingColumn(sopHeaders[2], 1, 'price', false, drawSentPrice, sortSOPriAsc, sortSOPriDesc), 0);
	sopListing.addColumn(new ListingColumn(sopHeaders[3], 1, 'sexp', false, drawExpire, sortSOExpAsc, sortSOExpDesc), 0);
	sopListing.addColumn(new ListingColumn(sopHeaders[4], 1, 'buyer', false, drawSentBuyer, sortSOBuyAsc, sortSOBuyDesc), 0);
	sopListing.addColumn(new ListingColumn('&nbsp;', 1, 'lspc', false, function () { return '&nbsp;' }, null, null), 1);
	sopListing.addColumn(new ListingColumn(sopHeaders[5], 4, 'details', false, drawPRDetails, null, null), 1);
	sopListing.emptyText = noSOffers; sopListing.notFound = noSOffersFound; sopListing.perPage = 5; sopListing.sortDir = true;

	// Sent offers, history
	sohListing = new Listing('sohistory', 'soHistory', 'sohListing', searchText[0]);
	sohListing.addColumn(new ListingColumn(sohHeaders[0], 1, 'rdate', false, drawPHActionDate, sortSHADtAsc, sortSHADtDesc), 0);
	sohListing.addColumn(new ListingColumn(sohHeaders[1], 1, 'hstat', false, drawPHStatus, sortSHStaAsc, sortSHStaDesc), 0);
	sohListing.addColumn(new ListingColumn(sohHeaders[2], 1, 'rdate', false, drawPRReceived, sortSHSntAsc, sortSHSntDesc), 0);
	sohListing.addColumn(new ListingColumn(sohHeaders[3], 1, 'otype', false, drawSentMode, sortSHModAsc, sortSHModDesc), 0);
	sohListing.addColumn(new ListingColumn(sohHeaders[4], 1, 'price', false, drawSentPrice, sortSHPriAsc, sortSHPriDesc), 0);
	sohListing.addColumn(new ListingColumn(sohHeaders[5], 1, 'buyer', false, drawSentBuyer, sortSHBuyAsc, sortSHBuyDesc), 0);
	sohListing.addColumn(new ListingColumn('&nbsp;', 1, 'lspc', false, function () { return '&nbsp;' }, null, null), 1);
	sohListing.addColumn(new ListingColumn(sohHeaders[6], 5, 'details', false, drawPRDetails, null, null), 1);
	sohListing.emptyText = noSHist; sohListing.notFound = noSHistFound; sohListing.perPage = 5; sohListing.sortDir = true;

	e = document.getElementById('mkpinit').innerHTML;
	if	(e.indexOf('\n') == -1 && e.split('#').length > 2)
	{
		var	s = 'x_get' + pageHandlers[currentPage] + 'Offers(dataReceived)';
		eval(s);
	}
	else
		dataReceived(e);
}

function	selectPage(nb)
{
	if	(!lockUpdate(currentPage))
		return;
	currentPage = nb;
	var	s = 'x_get' + pageHandlers[currentPage] + 'Offers(dataReceived)';
	eval(s);
}


//------------------------------------------------------------------------
// PUBLIC OFFERS DATA & ACTIONS
//------------------------------------------------------------------------

function	parsePublicOffers(data)
{
	var	l = data.split('\n');
	var	a = l.shift().split('#');

	// Get map parameters
	mapCType = parseInt(a.shift(), 10); mapX = parseInt(a.shift(), 10);
	mapY = parseInt(a.shift(), 10); mapDist = parseInt(a.shift(), 10);
	mapParm = (mapCType > 0) ? a.join('#') : '';

	// Get own planet list
	var	i, np = parseInt(l.shift(),10);
	myPlanets = new Hashtable();
	for	(i=0;i<np;i++)
	{
		a = l.shift().split("#");
		myPlanets.put(a[0],a[1]);
	}

	// Selected system data
	a = l.shift().split("#");
	sysType = parseInt(a[0],10); sysProt = (a[1] == '1');
	if	(sysType > -1)
	{
		sysPlanets = new Array();
		for	(i=0;i<6;i++)
		{
			a = l.shift().split("#");
			sysPlanets.push(new SysPlanet(a.shift(),a.shift(),(a.shift()=='1'),(a.shift()=='1'),a.join('#')));
		}
	}

	// Read offers
	pOffers = new Array(); fOffers = new Array(); sellers = new Hashtable();
	a = l.shift().split('#');
	var	s='',nop=parseInt(a[0],10),nof=parseInt(a[1],10);
	if	(nop+nof == 0)
		return;
	np = 0;

	// Planet offers
	for	(i=0;i<nop;i++)
	{
		var	o, f;
		a = l.shift().split('#');
		f = (a[12] == '1'); a.splice(12,1);
		o = new PublicPlanetOffer(
			a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),
			a.shift(),(a.shift()==1),a.shift(),a.shift(),a.shift(),a.join('#')
		);
		pOffers.push(o);
		if	(s.indexOf('#'+o.seller+'#') == -1)
		{
			np++;
			s += '#'+o.seller+'#';
		}
		if	(!f)
			continue;
		o.fleet = l.shift().split('#');
	}

	// Fleet offers
	for	(i=0;i<nof;i++)
	{
		var	o;
		a = l.shift().split('#');
		o = new PublicFleetOffer(
			a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),
			a.shift(),(a.shift()=='1'),a.shift(),a.shift(),a.shift(),a.shift(),a.join('#')
		);
		fOffers.push(o);
		if	(s.indexOf('#'+o.seller+'#') == -1)
		{
			np++;
			s += '#'+o.seller+'#';
		}
	}

	// Sellers
	for	(i=0;i<np;i++)
	{
		a = l.shift().split('#');
		s = a.shift();
		sellers.put(s, new Seller(s, (a.shift()=='1'), (a.shift()=='1'), a.join('#')));
	}
}

function	drawSystemMap()
{
	var	str = '<table cellspacing="0" cellpadding="0" id="sys';
	switch	(sysType)
	{
		case -1: str += 'unk'; break;
		case 0: str += 'std' + (sysProt ? 'prt' : ''); break;
		case 1: case 2: case 3: case 4: str += 'neb'+sysType; break;
	}
	str += '">';

	var	i;
	if	(sysType == -1)
		for (i=0;i<6;i++) str += '<tr><td>(unknown)</td></tr>';
	else if	(sysType == 0)
	{
		for	(i=0;i<6;i++)
		{with(sysPlanets[i]){
			str += '<tr><td class="pimg"><img src="'+staticurl+'/beta5/pics/';
			if	(status == '0')
				str += 'pl/s/'+id+'.png" alt="[P]" /></td><td class="planet'+(ownership==0?'n':(ownership==1?'a':'o'))+'">';
			else
				str += 'prem_s.png" alt="[P]" /></td><td class="prem">';
			str += '<a href="planet?id='+id+'">' + name + '</a></td></tr>';
		}}
	}
	else
	{
		for	(i=0;i<6;i++)
		{with(sysPlanets[i]){
			str += '<tr><td class="nebzone'+(parseInt(status,10)-1)+'">' + name + '</td></tr>';
		}}
	}

	str += '</table>';
	document.getElementById('mapcnt').innerHTML = str;
}

function	drawPublicPage()
{
	drawSystemMap();
	pfsListing.updateDisplay();
	ffsListing.updateDisplay();
}

function	updateMapControls()
{
	var	i;
	for	(i=0;i<3;i++)
		document.getElementById('mct'+i).checked = (i==mapCType);
	document.getElementById('cx').value = mapX;
	document.getElementById('cy').value = mapY;

	var	e = document.getElementById('mcpo');
	for	(i=0;i<e.options.length;i++)
		if	(	(mapCType != 1 && e.options[i].value == 0)
			||	(mapCType == 1 && e.options[i].value == mapParm)
			)
		{
			e.selectedIndex = i;
			break;
		}

	document.getElementById('mcpn').value = (mapCType == 2) ? mapParm : '';
}

function	mapMoved(data)
{
	dataReceived(data);
	updateMapControls();
}

function	moveMap(ctype,parm,dist)
{
	if	(!lockUpdate(0))
		return;
	x_moveMap(ctype,parm,dist,mapMoved);
}

function	moveMapCoords(x,y,dist)
	{ moveMap(0,x+"#"+y,dist); }
function	moveMapUp()
	{ moveMapCoords(mapX,mapY+1,mapDist); }
function	moveMapDown()
	{ moveMapCoords(mapX,mapY-1,mapDist); }
function	moveMapRight()
	{ moveMapCoords(mapX+1,mapY,mapDist); }
function	moveMapLeft()
	{ moveMapCoords(mapX-1,mapY,mapDist); }

function	updatePublicDisplay()
{
	var	e = document.getElementById('mcds');
	var	d = e.options[e.selectedIndex].value;

	if	(document.getElementById('mct0').checked)
	{
		var	x = parseInt(document.getElementById('cx').value, 10);
		var	y = parseInt(document.getElementById('cy').value, 10);
		moveMapCoords(x,y,d);
	}
	else if	(document.getElementById('mct1').checked)
	{
		e = document.getElementById('mcpo');
		var	p = e.options[e.selectedIndex].value;
		if	(p == 0)
			return;
		moveMap(1,p,d);
	}
	else
	{
		e = document.getElementById('mcpn').value;
		if	(e == "")
			return;
		moveMap(2,e,d);
	}
}

function	getPlanetOffer(id)
{
	var	i;
	for	(i=0;i<pOffers.length&&pOffers[i].offer!=id;i++) ;
	return	(i<pOffers.length ? pOffers[i] : null);
}

function	cancelPSale(id)
{
	if	(!lockUpdate(0))
		return;

	var	p = getPlanetOffer(id);
	if	(!(p && confirmPCancel(p)))
	{
		startUpdate();
		return;
	}
	x_cancelSale(p.offer, cancelCallback);
}

function cancelCallback(data) {
	if (data == 'ERR#200') {
		privateError('200');
		startUpdate();
	} else {
		dataReceived(data);
	}
}

function	buyPlanet(id)
{
	if	(!lockUpdate(0))
		return;
	var	p = getPlanetOffer(id);
	if	(!p)
	{
		startUpdate();
		return;
	}
	if	(!confirmBuyPlanet(p))
	{
		startUpdate();
		return;
	}
	x_buyPublic(id, pBought);
}

function	pBought(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		postPBuyError(data.substr(4));
		startUpdate();
	}
	else
	{
		planetBought();
		updateHeader();
		dataReceived(data);
	}
}

function	placePBid(id)
{
	if	(!lockUpdate(0))
		return;
	doPlacePBid(id);
}

function	doPlacePBid(id)
{
	var	b, bok = false;
	while	(!bok)
	{
		var	p = getPlanetOffer(id);
		if	(!p)
		{
			startUpdate();
			return;
		}
		b = confirmBidPlanet(p);
		if	(typeof b == 'object' && !b)
		{
			startUpdate();
			return;
		}
		if	(parseInt(b, 10) <= parseInt(p.price, 10))
			pBidError(p);
		else
			bok = true;
	}
	x_placeBid(p.offer, b, pBidPlaced);
}

function	pBidPlaced(data)
{
	if	(data.indexOf('ERR#') == 0)
		postPBidError(data.substr(4));
	else
	{
		updateHeader();
		dataReceived(data);
	}
}

function	getFleetOffer(id)
{
	var	i;
	for	(i=0;i<fOffers.length&&fOffers[i].offer!=id;i++) ;
	return	(i<fOffers.length ? fOffers[i] : null);
}

function	cancelFSale(id)
{
	if	(!lockUpdate(0))
		return;

	var	p = getFleetOffer(id);
	if	(!(p && confirmFCancel(p)))
	{
		startUpdate();
		return;
	}
	x_cancelSale(p.offer, cancelCallback);
}

function	buyFleet(id)
{
	if	(!lockUpdate(0))
		return;
	var	f = getFleetOffer(id);
	if	(!f)
	{
		startUpdate();
		return;
	}
	if	(!confirmBuyFleet(f))
	{
		startUpdate();
		return;
	}
	x_buyPublic(id, fBought);
}

function	fBought(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		postFBuyError(data.substr(4));
		startUpdate();
	}
	else
	{
		fleetBought();
		updateHeader();
		dataReceived(data);
	}
}

function	placeFBid(id)
{
	if	(!lockUpdate(0))
		return;
	doPlaceFBid(id);
}


function	doPlaceFBid(id)
{
	var	b, bok = false;
	while	(!bok)
	{
		var	p = getFleetOffer(id);
		if	(!p)
		{
			startUpdate();
			return;
		}
		b = confirmBidFleet(p);
		if	(typeof b == 'object' && !b)
		{
			startUpdate();
			return;
		}
		if	(parseInt(b, 10) <= parseInt(p.price, 10))
			fBidError(p);
		else
			bok = true;
	}
	x_placeBid(p.offer, b, fBidPlaced);
}

function	fBidPlaced(data)
{
	if	(data.indexOf('ERR#') == 0)
		postFBidError(data.substr(4));
	else
	{
		updateHeader();
		dataReceived(data);
	}
}


// Common listing functions
function	drawCoords(v)
	{ return '(<b>' + v.x + ',' + v.y + '</b>,'+v.orbit+')'; }
function	drawSeller(v)
{
	var	s = sellers.get(v.seller);
	return	(s.isMe||s.quit ? '' : ('<a href="message?a=c&ct=0&id='+v.seller+'">')) + s.name + (s.isMe||s.quit ? '' : '</a>');
}
function	drawPrice(v)
	{ return '&euro;' + formatNumber(v.price); }

// Planet sales listing functions
function	drawPSName(v)
{
	var	str = '<td class="pimg"><img src="'+staticurl+'/beta5/pics/pl/s/'+v.id+'.png" alt="[P]" /></td>';
	str += '<td class="pname"><a href="planet?id='+v.id+'">'+v.name+'</td>';
	return	str;
}
function	sortPSNameAsc(a,b)
	{ var ra = pOffers[a].name.toLowerCase(), rb = pOffers[b].name.toLowerCase(); return (ra > rb) ? 1 : -1; }
function	sortPSNameDesc(a,b)
	{ var ra = pOffers[a].name.toLowerCase(), rb = pOffers[b].name.toLowerCase(); return (ra > rb) ? -1 : 1; }

function sortPSCrdAsc(a,b)
{
	var va = pOffers[a]; var vb = pOffers[b];
	return	(va.x>vb.x)?1:(va.x<vb.x?-1:(va.y>vb.y?1:(va.y<vb.y?-1:(va.orbit>vb.orbit?1:-1))));
}
function sortPSCrdDesc(a,b)
{
	var va = pOffers[a]; var vb = pOffers[b];
	return	(va.x>vb.x)?-1:(va.x<vb.x?1:(va.y>vb.y?-1:(va.y<vb.y?1:(va.orbit>vb.orbit?-1:1))));
}

function	drawPSPop(v)
	{ return formatNumber(v.population); }
function	sortPSPopAsc(a,b)
	{ var ra=parseInt(pOffers[a].population,10),rb=parseInt(pOffers[b].population,10); return (ra>rb)?1:(ra<rb?-1:sortPSTurAsc(a,b)); }
function	sortPSPopDesc(a,b)
	{ var ra=parseInt(pOffers[a].population,10),rb=parseInt(pOffers[b].population,10); return (ra>rb)?-1:(ra<rb?1:sortPSTurDesc(a,b)); }

function	drawPSTurrets(v)
	{ return formatNumber(v.turrets); }
function	sortPSTurAsc(a,b)
	{ var ra=parseInt(pOffers[a].turrets,10),rb=parseInt(pOffers[b].turrets,10); return (ra>rb)?1:(ra<rb?-1:sortPSFacAsc(a,b)); }
function	sortPSTurDesc(a,b)
	{ var ra=parseInt(pOffers[a].turrets,10),rb=parseInt(pOffers[b].turrets,10); return (ra>rb)?-1:(ra<rb?1:sortPSFacDesc(a,b)); }

function	drawPSFactories(v)
	{ return formatNumber(v.fact); }
function	sortPSFacAsc(a,b)
	{ var ra=parseInt(pOffers[a].fact,10),rb=parseInt(pOffers[b].fact,10); return (ra>rb)?1:(ra<rb?-1:sortPSFltAsc(a,b)); }
function	sortPSFacDesc(a,b)
	{ var ra=parseInt(pOffers[a].fact,10),rb=parseInt(pOffers[b].fact,10); return (ra>rb)?-1:(ra<rb?1:sortPSFltDesc(a,b)); }

function	getPSFleetSize(a)
{
	if	(pOffers[a].fleet)
		return parseInt(pOffers[a].fleet[1],10) + parseInt(pOffers[a].fleet[2],10)*2 + parseInt(pOffers[a].fleet[3],10)*8 + parseInt(pOffers[a].fleet[4],10)*16;
	return 0;
}
function	sortPSFltAsc(a,b)
	{ var ra=getPSFleetSize(a),rb=getPSFleetSize(b); return (ra>rb)?1:(ra<rb?-1:sortPSSelAsc(a,b)); }
function	sortPSFltDesc(a,b)
	{ var ra=getPSFleetSize(a),rb=getPSFleetSize(b); return (ra>rb)?-1:(ra<rb?1:sortPSSelDesc(a,b)); }

function	sortPSSelAsc(a,b)
	{ var ra = sellers.get(pOffers[a].seller).name.toLowerCase(), rb = sellers.get(pOffers[b].seller).name.toLowerCase(); return (ra>rb)?1:(ra<rb?-1:sortPSExpAsc(a,b)); }
function	sortPSSelDesc(a,b)
	{ var ra = sellers.get(pOffers[a].seller).name.toLowerCase(), rb = sellers.get(pOffers[b].seller).name.toLowerCase(); return (ra>rb)?-1:(ra<rb?1:sortPSExpDesc(a,b)); }

function	sortPSExpAsc(a,b)
{
	var ra = pOffers[a].expires==''?1928448000:parseInt(pOffers[a].expires,10), rb = pOffers[b].expires==''?1928448000:parseInt(pOffers[b].expires,10);
	return	(ra>rb)?1:(ra<rb?-1:sortPSPriAsc(a,b));
}
function	sortPSExpDesc(a,b)
{
	var ra = pOffers[a].expires==''?1928448000:parseInt(pOffers[a].expires,10), rb = pOffers[b].expires==''?1928448000:parseInt(pOffers[b].expires,10);
	return	(ra>rb)?-1:(ra<rb?1:sortPSPriDesc(a,b));
}

function	sortPSPriAsc(a,b)
	{ var ra=parseInt(pOffers[a].price,10),rb=parseInt(pOffers[b].price,10); return (ra>rb)?1:(ra<rb?-1:sortPSActAsc(a,b)); }
function	sortPSPriDesc(a,b)
	{ var ra=parseInt(pOffers[a].price,10),rb=parseInt(pOffers[b].price,10); return (ra>rb)?-1:(ra<rb?1:sortPSActDesc(a,b)); }

function	sortPSActAsc(a,b)
{
	var	va = sellers.get(pOffers[a].seller), vb = sellers.get(pOffers[b].seller);
	if	(va.isMe && !vb.isMe)
		return	1;
	if	(pOffers[a].isAuction && !pOffers[b].isAuction)
		return -1;
	return	sortPSNameAsc(a,b);
}
function	sortPSActDesc(a,b)
{
	var	va = sellers.get(pOffers[a].seller), vb = sellers.get(pOffers[b].seller);
	if	(va.isMe && !vb.isMe)
		return	-1;
	if	(pOffers[a].isAuction && !pOffers[b].isAuction)
		return 1;
	return	sortPSNameDesc(a,b);
}

// Fleet sales listing function
function	drawFSLocation(v)
{
	var	str = '<td class="pimg"><img src="'+staticurl+'/beta5/pics/pl/s/'+v.plId+'.png" alt="[P]" /></td>';
	str += '<td class="pname" colspan="2"><a href="planet?id='+v.plId+'">'+v.plName+'</td>';
	return	str;
}
function	sortFSLocAsc(a,b)
	{ var ra = fOffers[a].plName.toLowerCase(), rb = fOffers[b].plName.toLowerCase(); return (ra>rb)?1:(ra<rb?-1:sortFSGASAsc(a,b)); }
function	sortFSLocDesc(a,b)
	{ var ra = fOffers[a].plName.toLowerCase(), rb = fOffers[b].plName.toLowerCase(); return (ra>rb)?-1:(ra<rb?1:sortFSGASDesc(a,b)); }

function sortFSCrdAsc(a,b)
{
	var va = fOffers[a]; var vb = fOffers[b];
	return	(va.x>vb.x)?1:(va.x<vb.x?-1:(va.y>vb.y?1:(va.y<vb.y?-1:(va.orbit>vb.orbit?1:(va.orbit<vb.orbit?-1:sortFSGASAsc(a,b))))));
}
function sortFSCrdDesc(a,b)
{
	var va = fOffers[a]; var vb = fOffers[b];
	return	(va.x>vb.x)?-1:(va.x<vb.x?1:(va.y>vb.y?-1:(va.y<vb.y?1:(va.orbit>vb.orbit?-1:(va.orbit<vb.orbit?1:sortFSGASDesc(a,b))))));
}

function	drawFSGAShips(v)
	{ return formatNumber(v.fleet[0]); }
function	sortFSGASAsc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[0],10),rb=parseInt(fOffers[b].fleet[0],10); return (ra>rb)?1:(ra<rb?-1:sortFSFgtAsc(a,b)); }
function	sortFSGASDesc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[0],10),rb=parseInt(fOffers[b].fleet[0],10); return (ra>rb)?-1:(ra<rb?1:sortFSFgtDesc(a,b)); }

function	drawFSFighters(v)
	{ return formatNumber(v.fleet[1]); }
function	sortFSFgtAsc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[1],10),rb=parseInt(fOffers[b].fleet[1],10); return (ra>rb)?1:(ra<rb?-1:sortFSCruAsc(a,b)); }
function	sortFSFgtDesc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[1],10),rb=parseInt(fOffers[b].fleet[1],10); return (ra>rb)?-1:(ra<rb?1:sortFSCruDesc(a,b)); }

function	drawFSCruisers(v)
	{ return formatNumber(v.fleet[2]); }
function	sortFSCruAsc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[2],10),rb=parseInt(fOffers[b].fleet[2],10); return (ra>rb)?1:(ra<rb?-1:sortFSBCrAsc(a,b)); }
function	sortFSCruDesc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[2],10),rb=parseInt(fOffers[b].fleet[2],10); return (ra>rb)?-1:(ra<rb?1:sortFSBCrDesc(a,b)); }

function	drawFSBCruisers(v)
	{ return formatNumber(v.fleet[3]); }
function	sortFSBCrAsc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[3],10),rb=parseInt(fOffers[b].fleet[3],10); return (ra>rb)?1:(ra<rb?-1:sortFSSelAsc(a,b)); }
function	sortFSBCrDesc(a,b)
	{ var ra=parseInt(fOffers[a].fleet[3],10),rb=parseInt(fOffers[b].fleet[3],10); return (ra>rb)?-1:(ra<rb?1:sortFSSelDesc(a,b)); }

function	sortFSSelAsc(a,b)
	{ var ra = sellers.get(fOffers[a].seller).name.toLowerCase(), rb = sellers.get(fOffers[b].seller).name.toLowerCase(); return (ra>rb)?1:(ra<rb?-1:sortFSExpAsc(a,b)); }
function	sortFSSelDesc(a,b)
	{ var ra = sellers.get(fOffers[a].seller).name.toLowerCase(), rb = sellers.get(fOffers[b].seller).name.toLowerCase(); return (ra>rb)?-1:(ra<rb?1:sortFSExpDesc(a,b)); }

function	sortFSExpAsc(a,b)
{
	var ra = fOffers[a].expires==''?1928448000:parseInt(fOffers[a].expires,10), rb = fOffers[b].expires==''?1928448000:parseInt(fOffers[b].expires,10);
	return	(ra>rb)?1:(ra<rb?-1:sortFSPriAsc(a,b));
}
function	sortFSExpDesc(a,b)
{
	var ra = fOffers[a].expires==''?1928448000:parseInt(fOffers[a].expires,10), rb = fOffers[b].expires==''?1928448000:parseInt(fOffers[b].expires,10);
	return	(ra>rb)?-1:(ra<rb?1:sortFSPriDesc(a,b));
}

function	sortFSPriAsc(a,b)
	{ var ra=parseInt(fOffers[a].price,10),rb=parseInt(fOffers[b].price,10); return (ra>rb)?1:(ra<rb?-1:sortFSActAsc(a,b)); }
function	sortFSPriDesc(a,b)
	{ var ra=parseInt(fOffers[a].price,10),rb=parseInt(fOffers[b].price,10); return (ra>rb)?-1:(ra<rb?1:sortFSActDesc(a,b)); }

function	sortFSActAsc(a,b)
{
	var	va = sellers.get(fOffers[a].seller), vb = sellers.get(fOffers[b].seller);
	if	(va.isMe && !vb.isMe)
		return	1;
	if	(fOffers[a].isAuction && !fOffers[b].isAuction)
		return -1;
	return	sortFSIdAsc(a,b);
}
function	sortFSActDesc(a,b)
{
	var	va = sellers.get(fOffers[a].seller), vb = sellers.get(fOffers[b].seller);
	if	(va.isMe && !vb.isMe)
		return	-1;
	if	(fOffers[a].isAuction && !fOffers[b].isAuction)
		return 1;
	return	sortFSIdDesc(a,b);
}

function	sortFSIdAsc(a,b)
	{ var ra=parseInt(fOffers[a].id,10),rb=parseInt(fOffers[b].id,10); return (ra>rb)?1:-1; }
function	sortFSIdDesc(a,b)
	{ var ra=parseInt(fOffers[a].id,10),rb=parseInt(fOffers[b].id,10); return (ra>rb)?-1:1; }


//------------------------------------------------------------------------
// PRIVATE OFFERS DATA & ACTIONS
//------------------------------------------------------------------------

var	prOffers, prHistory;
var	proListing, prhListing;

function	PrivateOffer(id, seller, price, x, y, orbit, planet, fleet, started, expires, pName)
{
	this.id		= id;
	this.seller	= seller;
	this.price	= price;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.started	= started;
	this.expires	= expires;
	this.pName	= pName;
	this.planet	= [planet, 0, 0, 0];
	this.fleet	= [fleet, 0, 0, 0, 0];
	this.match	= PrivateOffer_match;
}

function	PrivateHistory(seller, price, x, y, orbit, isPlanet, hasFleet, started, action, aDate, pName)
{
	this.seller = seller;
	this.price = price;
	this.x = x; this.y = y; this.orbit = orbit;
	this.planet = [isPlanet, 0, 0, 0];
	this.fleet = [hasFleet, 0, 0, 0, 0];
	this.started = started; this.action = action; this.aDate = aDate;
	this.pName = pName;

	this.match = PrivateOffer_match;
}

function	PrivateOffer_match(t)
	{ return this.pName.toLowerCase().indexOf(t.toLowerCase()) != -1; } 

function	parsePrivateOffers(data)
{
	var	l = data.split('\n');
	var	i, a = l.shift().split('#');
	var	nOffers = a[0], nHistory = a[1];
	var	s='',ns=0;

	// Read offers data
	prOffers = new Array();
	for	(i=0;i<nOffers;i++)
	{
		a = l.shift().split('#');
		var	o = new PrivateOffer(
				a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),
				a.shift(),a.shift(),a.shift(),a.shift(),a.join('#')
			);
		if	(o.fleet[0] != '')
		{
			a = l.shift().split('#');
			o.fleet[1] = a[0]; o.fleet[2] = a[1]; o.fleet[3] = a[2]; o.fleet[4] = a[3];
		}
		if	(o.planet[0] != '')
		{
			a = l.shift().split('#');
			o.planet[1] = a[0]; o.planet[2] = a[1]; o.planet[3] = a[2];
		}
		if	(s.indexOf('#'+o.seller+'#') == -1)
		{
			s += '#'+o.seller+'#';
			ns ++;
		}
		prOffers.push(o);
	}

	// Read history data
	prHistory = new Array();
	for	(i=0;i<nHistory;i++)
	{
		a = l.shift().split('#');
		var	o = new PrivateHistory(
				a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),(a.shift()=='1'),
				(a.shift()=='1'),a.shift(),a.shift(),a.shift(),a.join('#')
			);
		if	(o.fleet[0])
		{
			a = l.shift().split('#');
			o.fleet[1] = a[0]; o.fleet[2] = a[1]; o.fleet[3] = a[2]; o.fleet[4] = a[3];
		}
		if	(o.planet[0])
		{
			a = l.shift().split('#');
			o.planet[1] = a[0]; o.planet[2] = a[1]; o.planet[3] = a[2];
		}
		if	(s.indexOf('#'+o.seller+'#') == -1)
		{
			s += '#'+o.seller+'#';
			ns ++;
		}
		prHistory.push(o);
	}

	// Sellers
	sellers = new Hashtable();
	for	(i=0;i<ns;i++)
	{
		a = l.shift().split('#');
		s = a.shift();
		sellers.put(s, new Seller(s, false, (a.shift()=='1'), a.join('#')));
	}
}

function	drawPrivatePage()
{
	proListing.updateDisplay();
	prhListing.updateDisplay();
}

function	getPrivateOffer(id)
{
	var	i;
	for	(i=0;i<prOffers.length&&prOffers[i].id!=id;i++) ;
	return	(i<prOffers.length ? prOffers[i] : null);
}

function	acceptPrivate(id)
{
	if	(!lockUpdate(1))
		return;
	var	p = getPrivateOffer(id);
	if	(!p)
	{
		startUpdate();
		return;
	}
	if	(!confirmBuyPrivate(p))
	{
		startUpdate();
		return;
	}
	x_buyPrivate(id, privateAction);
}

function	declinePrivate(id)
{
	if	(!lockUpdate(1))
		return;
	var	p = getPrivateOffer(id);
	if	(!p)
	{
		startUpdate();
		return;
	}
	if	(!confirmDeclinePrivate())
	{
		startUpdate();
		return;
	}
	x_declinePrivate(id, privateAction);
}

function	privateAction(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		privateError(data.substr(4));
		startUpdate();
	}
	else
	{
		updateHeader();
		dataReceived(data);
	}
}


// Sorting & display, pending offers
function drawPRReceived(i)
	{ return formatDate(i.started); }
var	sortPRRecLev = 0;
function sortPRRecAsc(a,b)
{
	var rv,ra=parseInt(prOffers[a].started,10),rb=parseInt(prOffers[b].started,10);
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRSelAsc(a,b); }
	sortPRRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPRRecDesc(a,b)
{
	var rv,ra=parseInt(prOffers[a].started,10),rb=parseInt(prOffers[b].started,10);
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRSelDesc(a,b); }
	sortPRRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPRSelAsc(a,b)
{
	var rv,ra = sellers.get(prOffers[a].seller).name.toLowerCase(), rb = sellers.get(prOffers[b].seller).name.toLowerCase();
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRPriAsc(a,b); }
	sortPRRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPRSelDesc(a,b)
{
	var rv,ra = sellers.get(prOffers[a].seller).name.toLowerCase(), rb = sellers.get(prOffers[b].seller).name.toLowerCase();
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRPriDesc(a,b); }
	sortPRRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPRPriAsc(a,b)
{
	var rv,ra=parseInt(prOffers[a].price,10),rb=parseInt(prOffers[b].price,10);
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRExpAsc(a,b); }
	sortPRRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPRPriDesc(a,b)
{
	var rv,ra=parseInt(prOffers[a].price,10),rb=parseInt(prOffers[b].price,10);
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRExpDesc(a,b); }
	sortPRRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPRExpAsc(a,b)
{
	var rv,ra = prOffers[a].expires==''?1928448000:parseInt(prOffers[a].expires,10), rb = prOffers[b].expires==''?1928448000:parseInt(prOffers[b].expires,10);
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRRecAsc(a,b); }
	sortPRRecLev --;
	return	(ra>rb)?1:(ra<rb?-1:rv);
}
function sortPRExpDesc(a,b)
{
	var rv,ra = prOffers[a].expires==''?1928448000:parseInt(prOffers[a].expires,10), rb = prOffers[b].expires==''?1928448000:parseInt(prOffers[b].expires,10);
	sortPRRecLev ++;
	if (ra == rb) { if (sortPRRecLev == 4) rv = 0; else rv = sortPRRecDesc(a,b); }
	sortPRRecLev --;
	return	(ra>rb)?-1:(ra<rb?1:rv);
}

// Sorting & display, offers history
function drawPHActionDate(i)
	{ return formatDate(i.aDate); }
function drawPHStatus(i)
	{ return prhStatus[i.action]; }
var	sortPHRecLev = 0;
function sortPHADtAsc(a,b)
{
	var rv,ra=parseInt(prHistory[a].aDate,10),rb=parseInt(prHistory[b].aDate,10);
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHRecAsc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPHADtDesc(a,b)
{
	var rv,ra=parseInt(prHistory[a].aDate,10),rb=parseInt(prHistory[b].aDate,10);
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHRecDesc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPHStaAsc(a,b)
{
	var rv,ra=prhStatus[prHistory[a].action],rb=prhStatus[prHistory[b].action];
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHSelAsc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPHStaDesc(a,b)
{
	var rv,ra=prhStatus[prHistory[a].action],rb=prhStatus[prHistory[b].action];
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHSelDesc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPHRecAsc(a,b)
{
	var rv,ra=parseInt(prHistory[a].started,10),rb=parseInt(prHistory[b].started,10);
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHStaAsc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPHRecDesc(a,b)
{
	var rv,ra=parseInt(prHistory[a].started,10),rb=parseInt(prHistory[b].started,10);
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHStaDesc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPHSelAsc(a,b)
{
	var rv,ra = sellers.get(prHistory[a].seller).name.toLowerCase(), rb = sellers.get(prHistory[b].seller).name.toLowerCase();
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHPriAsc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPHSelDesc(a,b)
{
	var rv,ra = sellers.get(prHistory[a].seller).name.toLowerCase(), rb = sellers.get(prHistory[b].seller).name.toLowerCase();
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHPriDesc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortPHPriAsc(a,b)
{
	var rv,ra=parseInt(prHistory[a].price,10),rb=parseInt(prHistory[b].price,10);
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHADtAsc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortPHPriDesc(a,b)
{
	var rv,ra=parseInt(prHistory[a].price,10),rb=parseInt(prHistory[b].price,10);
	sortPHRecLev ++;
	if (ra == rb) { if (sortPHRecLev == 6) rv = 0; else rv = sortPHADtDesc(a,b); }
	sortPHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}


//------------------------------------------------------------------------
// SENT OFFERS DATA & ACTIONS
//------------------------------------------------------------------------

var	soPending, soHistory;
var	sopListing, sohListing;

function	SentOffer(id, mode, price, buyer, x, y, orbit, planet, fleet, started, expires, pName)
{
	this.id		= id;
	this.mode	= mode;
	this.price	= price;
	this.buyer	= buyer;
	this.x		= x;
	this.y		= y;
	this.orbit	= orbit;
	this.started	= started;
	this.expires	= expires;
	this.pName	= pName;
	this.planet	= [planet, 0, 0, 0];
	this.fleet	= [fleet, 0, 0, 0, 0];
	this.match	= PrivateOffer_match;
}

function	SentHistory(x, y, orbit, mode, isPlanet, hasFleet, buyer, price, started, action, aDate, pName)
{
	this.x = x; this.y = y; this.orbit = orbit;
	this.planet = [isPlanet, 0, 0, 0];
	this.fleet = [hasFleet, 0, 0, 0, 0];
	this.mode = mode; this.buyer = buyer; this.price = price;
	this.started = started; this.action = action; this.aDate = aDate;
	this.pName = pName;
	this.match = PrivateOffer_match;
}

function	parseSentOffers(data)
{
	var	l = data.split('\n');
	var	i, a = l.shift().split('#');
	var	nOffers = a[0], nHistory = a[1];
	var	s='',ns=0;

	// Read offers data
	soPending = new Array();
	for	(i=0;i<nOffers;i++)
	{
		a = l.shift().split('#');
		var	o = new SentOffer(
				a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),
				a.shift(),(a.shift()=='1'),(a.shift()=='1'),a.shift(),a.shift(),a.join('#')
			);
		if	(o.fleet[0])
		{
			a = l.shift().split('#');
			o.fleet[1] = a[0]; o.fleet[2] = a[1]; o.fleet[3] = a[2]; o.fleet[4] = a[3];
		}
		if	(o.planet[0])
		{
			a = l.shift().split('#');
			o.planet[1] = a[0]; o.planet[2] = a[1]; o.planet[3] = a[2];
		}
		if	(o.buyer != '' && s.indexOf('#'+o.buyer+'#') == -1)
		{
			s += '#'+o.buyer+'#';
			ns ++;
		}
		soPending.push(o);
	}

	// Read history data
	soHistory = new Array();
	for	(i=0;i<nHistory;i++)
	{
		a = l.shift().split('#');
		var	o = new SentHistory(
				a.shift(),a.shift(),a.shift(),a.shift(),(a.shift()=='1'),(a.shift()=='1'),
				a.shift(),a.shift(),a.shift(),a.shift(),a.shift(),a.join('#')
			);
		if	(o.fleet[0])
		{
			a = l.shift().split('#');
			o.fleet[1] = a[0]; o.fleet[2] = a[1]; o.fleet[3] = a[2]; o.fleet[4] = a[3];
		}
		if	(o.planet[0])
		{
			a = l.shift().split('#');
			o.planet[1] = a[0]; o.planet[2] = a[1]; o.planet[3] = a[2];
		}
		if	(o.buyer != '' && s.indexOf('#'+o.buyer+'#') == -1)
		{
			s += '#'+o.buyer+'#';
			ns ++;
		}
		soHistory.push(o);
	}

	// Buyers
	sellers = new Hashtable();
	for	(i=0;i<ns;i++)
	{
		a = l.shift().split('#');
		s = a.shift();
		sellers.put(s, new Seller(s, false, (a.shift()=='1'), a.join('#')));
	}
}

function	drawSentPage()
{
	sopListing.updateDisplay();
	sohListing.updateDisplay();
}

function	cancelSale(id)
{
	if	(!lockUpdate(2))
		return;

	if	(!confirmCancelSale())
	{
		startUpdate();
		return;
	}
	x_cancelSale(id, cancelCallback);
}

// Sorting & display, pending offers
function drawSentMode(i)
	{ return soModes[i.mode]; }
function drawSentPrice(i)
	{ return i.price == 0 ? 'N/A' : ('&euro;' + formatNumber(i.price)); }
var	sortSORecLev = 0;
function sortSOSntAsc(a,b)
{
	var rv,ra=parseInt(soPending[a].started,10),rb=parseInt(soPending[b].started,10);
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOModAsc(a,b); }
	sortSORecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSOSntDesc(a,b)
{
	var rv,ra=parseInt(soPending[a].started,10),rb=parseInt(soPending[b].started,10);
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOModDesc(a,b); }
	sortSORecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSOModAsc(a,b)
{
	var rv,ra=soModes[soPending[a].mode],rb=soModes[soPending[b].mode];
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOPriAsc(a,b); }
	sortSORecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSOModDesc(a,b)
{
	var rv,ra=soModes[soPending[a].mode],rb=soModes[soPending[b].mode];
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOPriDesc(a,b); }
	sortSORecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSOPriAsc(a,b)
{
	var rv,ra=parseInt(soPending[a].price,10),rb=parseInt(soPending[b].price,10);
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOexpAsc(a,b); }
	sortSORecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSOPriDesc(a,b)
{
	var rv,ra=parseInt(soPending[a].price,10),rb=parseInt(soPending[b].price,10);
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOExpDesc(a,b); }
	sortSORecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSOExpAsc(a,b)
{
	var rv,ra = soPending[a].expires==''?1928448000:parseInt(soPending[a].expires,10), rb = soPending[b].expires==''?1928448000:parseInt(soPending[b].expires,10);
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOBuyAsc(a,b); }
	sortSORecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSOExpDesc(a,b)
{
	var rv,ra = soPending[a].expires==''?1928448000:parseInt(soPending[a].expires,10), rb = soPending[b].expires==''?1928448000:parseInt(soPending[b].expires,10);
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOBuyDesc(a,b); }
	sortSORecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSOBuyAsc(a,b)
{
	var rv,ra = (soPending[a].buyer == '') ? '' : sellers.get(soPending[a].buyer).name.toLowerCase(),
		rb = (soPending[a].buyer == '') ? '' : sellers.get(soPending[b].buyer).name.toLowerCase();
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOSntAsc(a,b); }
	sortSORecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSOBuyDesc(a,b)
{
	var rv,ra = (soPending[a].buyer == '') ? '' : sellers.get(soPending[a].buyer).name.toLowerCase(),
		rb = (soPending[a].buyer == '') ? '' : sellers.get(soPending[b].buyer).name.toLowerCase();
	sortSORecLev ++;
	if (ra == rb) { if (sortSORecLev == 5) rv = 0; else rv = sortSOSntDesc(a,b); }
	sortSORecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
// Sorting & display, history
var	sortSHRecLev = 0;
function sortSHADtAsc(a,b)
{
	var rv,ra=parseInt(soHistory[a].aDate,10),rb=parseInt(soHistory[b].aDate,10);
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHStaAsc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSHADtDesc(a,b)
{
	var rv,ra=parseInt(soHistory[a].aDate,10),rb=parseInt(soHistory[b].aDate,10);
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHStaDesc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSHStaAsc(a,b)
{
	var rv,ra=prhStatus[soHistory[a].action],rb=prhStatus[soHistory[b].action];
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHSntAsc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSHStaDesc(a,b)
{
	var rv,ra=prhStatus[soHistory[a].action],rb=prhStatus[soHistory[b].action];
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHSntDesc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSHSntAsc(a,b)
{
	var rv,ra=parseInt(soHistory[a].started,10),rb=parseInt(soHistory[b].started,10);
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHModAsc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSHSntDesc(a,b)
{
	var rv,ra=parseInt(soHistory[a].started,10),rb=parseInt(soHistory[b].started,10);
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHModDesc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSHModAsc(a,b)
{
	var rv,ra=soModes[soHistory[a].mode],rb=soModes[soHistory[b].mode];
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHPriAsc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSHModDesc(a,b)
{
	var rv,ra=soModes[soHistory[a].mode],rb=soModes[soHistory[b].mode];
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHPriDesc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSHPriAsc(a,b)
{
	var rv,ra=parseInt(soHistory[a].price,10),rb=parseInt(soHistory[b].price,10);
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHBuyAsc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSHPriDesc(a,b)
{
	var rv,ra=parseInt(soHistory[a].price,10),rb=parseInt(soHistory[b].price,10);
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHBuyDesc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
function sortSHBuyAsc(a,b)
{
	var rv,ra = (soHistory[a].buyer == '') ? '' : sellers.get(soHistory[a].buyer).name.toLowerCase(),
		rb = (soHistory[b].buyer == '') ? '' : sellers.get(soHistory[b].buyer).name.toLowerCase();
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHADtAsc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?1:(ra<rb?-1:rv);
}
function sortSHBuyDesc(a,b)
{
	var rv, ra = (soHistory[a].buyer == '') ? '' : sellers.get(soHistory[a].buyer).name.toLowerCase(),
		rb = (soHistory[b].buyer == '') ? '' : sellers.get(soHistory[b].buyer).name.toLowerCase();
	sortSHRecLev ++;
	if (ra == rb) { if (sortSHRecLev == 6) rv = 0; else rv = sortSHADtDesc(a,b); }
	sortSHRecLev --;
	return (ra>rb)?-1:(ra<rb?1:rv);
}
