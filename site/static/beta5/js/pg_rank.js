var	pages	= ['Summary','General','Details','Alliance','Round','Damage'];
var	comps	= ['summary','genListing','detListing','allListing','rndListing','idrListing'];
var	rkPage	= null;

var	summary, genListing, detListing, allListing, rndListing, idrListing;
var	currentPage = null;

var	rantt;



Rankings = {};
Rankings.Headers = {};
Rankings.makeListing = function(rpc, dataType, nfText, ppText, sText, sMode) {
	var	ls = new Component.Listing(rpc, 3600, dataType, 'list', 'row', 'hdr');
	ls.notFoundText	= nfText;
	ls.getDocID	= function() { return 'rkpage'; }
	ls.getElement	= function() { return document.getElementById('rkpage'); };

	ls.setPageController(new Component.Listing.PageController(Rankings.pageText), true);

	var	c = new Component.Listing.PageSizeController(ppText, 5, 25, 5);
	c.select.select('15');
	ls.setPageSizeController(c, true);

	c = new Component.Listing.SearchController(sText);
	c.addMode(0, sMode);
	c.textField.setMaxLength(15); c.textField.setSize(16);
	ls.setSearchController(c, true);

	return	ls;
}

Rankings.setPage = function (page) { Rankings.setPage.ajax.call(page); }
Rankings.setPage.ajax = new Component.Ajax('setPage');


Rankings.GeneralRankings = function (cache) { Component.Listing.Data.apply(this, [cache]); }
Rankings.GeneralRankings.prototype = new Component.Listing.Data;
Rankings.GeneralRankings.prototype.parse = function (data) {
	var	a = data.shift().split('#');
	var	isMe = (a.shift() == '1');
	var	bb = isMe ? "<b>" : "";
	var	be = isMe ? "</b>" : "";

	this.rank = bb + '#' + formatNumber(a.shift()) + ' / '
		+ formatNumber(a.shift()) + " " + Rankings.pointsText + be;
	this.player = bb + a.join('#') + be;
}


Rankings.DetailedRankings = function (cache) { Component.Listing.Data.apply(this, [cache]); }
Rankings.DetailedRankings.prototype = new Component.Listing.Data;
Rankings.DetailedRankings.prototype.parse = function (data) {
	var	a = data.shift().split('#');
	var	isMe = (a.shift() == '1');
	var	bb = isMe ? "<b>" : "";
	var	be = isMe ? "</b>" : "";

	this.civRank = bb + '#' + formatNumber(a.shift()) + ' / '
		+ formatNumber(a.shift()) + " " + Rankings.pointsText + be;
	this.milRank = bb + '#' + formatNumber(a.shift()) + ' / '
		+ formatNumber(a.shift()) + " " + Rankings.pointsText + be;
	this.finRank = bb + '#' + formatNumber(a.shift()) + ' / '
		+ formatNumber(a.shift()) + " " + Rankings.pointsText + be;
	this.player = bb + a.join('#') + be;
}


Rankings.AllianceRankings = function (cache) { Component.Listing.Data.apply(this, [cache]); }
Rankings.AllianceRankings.prototype = new Component.Listing.Data;
Rankings.AllianceRankings.prototype.parse = function (data) {
	var	a = data.shift().split('#');

	this.rank = '#' + formatNumber(a.shift()) + ' / '
		+ formatNumber(a.shift()) + " " + Rankings.pointsText;
	this.alliance = a.join('#');
}


Rankings.Summary = function () {
	Component.apply(this, [false]);

	this.newSlot('dataReceived');
	this.newSlot('startUpdate');
	this.newSlot('stopUpdate');
	this.newSlot('updateData');

	this.timer	= new Component.Timer(3600*1000, true);
	this.ajax	= new Component.Ajax('getPlayerData');
	this.md5	= '';
	this.loading	= true;

	      this.bindEvent('Hide',		'stopUpdate');
	 this.ajax.bindEvent('Returned',	'dataReceived',	this);
	      this.bindEvent('Show',		'startUpdate');
	this.timer.bindEvent('Tick',		'updateData',	this);
}
Rankings.Summary.prototype = new Component;
Rankings.Summary.prototype.getDocID	= function() { return 'rkpage'; }
Rankings.Summary.prototype.getElement	= function() { return document.getElementById('rkpage'); };
Rankings.Summary.prototype.dataReceived = function (data) {
	this.loading = false;
	if	(data != 'KEEP') {
		var	a = data.split('#');

		this.md5	= a.shift();
		this.ptGen	= a.shift();
		this.rkGen	= a.shift();
		this.ptCiv	= a.shift();
		this.rkCiv	= a.shift();
		this.ptMil	= a.shift();
		this.rkMil	= a.shift();
		this.ptFin	= a.shift();
		this.rkFin	= a.shift();
		this.ptRnd	= a.shift();
		this.rkRnd	= a.shift();
		this.ptDam	= a.shift();
		this.rkDam	= a.shift();
	}
	this.drawAll();
}
Rankings.Summary.prototype.startUpdate = function () {
	this.loading = true;
	this.timer.start();
	this.drawAll();
	this.ajax.call(this.md5);
}
Rankings.Summary.prototype.stopUpdate = function () { this.timer.stop(); }
Rankings.Summary.prototype.updateData = function () { this.ajax.call(this.md5); }
Rankings.Summary.prototype.draw = function () {
	var	e = this.getElement();
	if	(!e)
		return;

	var	str;
	if	(this.loading)
		str = '<div style="text-align:center;">'
			+ Rankings.Summary.loadingText + '</span>';
	else
		str = this.getText();
	e.innerHTML = str;
}



//----------------------------------------------------------------------------------------------
// GENERAL
//----------------------------------------------------------------------------------------------

function	initPage() {
	var	c;

	summary = new Rankings.Summary();

	// General rankings
	genListing = Rankings.makeListing('getGeneralRankings', Rankings.GeneralRankings,
		Rankings.noPlayersFound, Rankings.playersPerPageText, Rankings.searchPlayerText,
		"player");
	genListing.addField(new Component.Listing.Field(Rankings.Headers.player, true, 'player', 0, 0, 1, 1, 'gname'));
	genListing.addField(c = new Component.Listing.Field(Rankings.Headers.ranking, true, 'rank', 1, 0, 1, 1, 'grank'));
	genListing.dataCache.sortField = c;
	genListing.finalizeLayout();

	// Detailed rankings
	detListing = Rankings.makeListing('getDetailedRankings', Rankings.DetailedRankings,
		Rankings.noPlayersFound, Rankings.playersPerPageText, Rankings.searchPlayerText,
		"player");
	detListing.addField(new Component.Listing.Field(Rankings.Headers.player, true, 'player', 0, 0, 1, 1, 'dname'));
	detListing.addField(new Component.Listing.Field(Rankings.Headers.civRanking, true, 'civRank', 1, 0, 1, 1, 'drank'));
	detListing.addField(new Component.Listing.Field(Rankings.Headers.milRanking, true, 'milRank', 2, 0, 1, 1, 'drank'));
	detListing.addField(new Component.Listing.Field(Rankings.Headers.finRanking, true, 'finRank', 3, 0, 1, 1, 'drank'));
	detListing.finalizeLayout();

	// Alliance rankings
	allListing = Rankings.makeListing('getAllianceRankings', Rankings.AllianceRankings,
		Rankings.noAlliancesFound, Rankings.alliancesPerPageText, Rankings.searchAllianceText,
		"alliance");
	allListing.addField(new Component.Listing.Field(Rankings.Headers.alliance, true, 'alliance', 0, 0, 1, 1, 'dname'));
	allListing.addField(c = new Component.Listing.Field(Rankings.Headers.ranking, true, 'rank', 1, 0, 1, 1, 'drank'));
	allListing.dataCache.sortField = c;
	allListing.finalizeLayout();

	// Round rankings
	rndListing = Rankings.makeListing('getRoundRankings', Rankings.GeneralRankings,
		Rankings.noPlayersFound, Rankings.playersPerPageText, Rankings.searchPlayerText,
		"player");
	rndListing.addField(new Component.Listing.Field(Rankings.Headers.player, true, 'player', 0, 0, 1, 1, 'gname'));
	rndListing.addField(c = new Component.Listing.Field(Rankings.Headers.ranking, true, 'rank', 1, 0, 1, 1, 'grank'));
	rndListing.dataCache.sortField = c;
	rndListing.finalizeLayout();

	// Damage rankings
	idrListing = Rankings.makeListing('getDamageRankings', Rankings.GeneralRankings,
		Rankings.noPlayersFound, Rankings.playersPerPageText, Rankings.searchPlayerText,
		"player");
	idrListing.addField(new Component.Listing.Field(Rankings.Headers.player, true, 'player', 0, 0, 1, 1, 'gname'));
	idrListing.addField(c = new Component.Listing.Field(Rankings.Headers.ranking, true, 'rank', 1, 0, 1, 1, 'grank'));
	idrListing.dataCache.sortField = c;
	idrListing.finalizeLayout();

	switchToPage(document.getElementById('rkinit').innerHTML);
}

function	switchToPage(page)
{
	if	(rkPage == page)
		return;

	if	(currentPage)
		currentPage.hide();

	rkPage = page;
	drawRankingsMenu();

	var	i;
	for	(i=0;i<pages.length;i++) {
		if	(pages[i] == rkPage) {
			eval('currentPage='+comps[i]);
			break;
		}
	}
	currentPage.show();
	Rankings.setPage(page);
}

function	drawRankingsMenu()
{
	var	i, rkm = new Array();
	for	(i=0;i<pages.length;i++)
	{
		var	s = (pages[i] == rkPage) ? '<b>' : ('<a href="#"  ' + rantt[30] + ' onClick="switchToPage(\''+pages[i]+'\');return false">');
		s += menuText[i] + ((pages[i] == rkPage) ? '</b>' : '</a>');
		rkm.push(s);
	}
	document.getElementById('rkpsel').innerHTML = rkm.join(' - ');
}
