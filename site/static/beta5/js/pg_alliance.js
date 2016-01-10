var	alPages = new Array('Main', 'Vote', 'MList', 'PList', 'MStat', 'Pending', 'FAdmin', 'RAdmin');
var	alColours = new Array(null, null, null, null, null, null, null, null);
var	alCheckPriv = new Array(
		new Function('return true'),
		new Function('return lockedAlliances || isDemocracy'),
		new Function('return (alPrivileges[0] > 0)'),
		new Function('return (alPrivileges[0] > 1)'),
		new Function('return (alPrivileges[1] == 1)'),
		new Function('return (alPrivileges[4] == 1) && !lockedAlliances'),
		new Function('return (alPrivileges[5] == 1)'),
		new Function('return (alPrivileges[8] == 1)')
	);

var	lockedAlliances;
var	plAlliance;
var	inAlliance;
var	alTag;
var	alName;
var	hasFocus = -1;
var	alPubInfo = false;
var	gdTimer;
var	puTimer;
var	amPage;
var	myPubInfo = false;
var	alPrivileges;

var	successor;
var	isDemocracy;
var	asSuccessor;
var	asDemocracy;
var	techTrade;
var	asTechTrade;
var	asButtons = false;

var	leCandidates;
var	leSelected;
var	leNewSel = false;
var	leCanSel;
var	leCanApply;
var	leCanCancel;
var	leCanTake;
var	leInAction = false;
var	leSort = 0;
var	leSortDir = false;

var	lsMode = false;
var	lsPlanets = new Array();
var	lsPPerPage = 10;
var	lsPPage = 0;
var	lsPSearchType = 0;
var	lsPSearchText = "";
var	lsPIndices;
var	lsPSort = 1;
var	lsPSortDir = false;
var	lsMembers = new Array();
var	lsMPerPage = 10;
var	lsMPage = 0;
var	lsMRanks;
var	lsMChange;
var	lsMKick;
var	lsMSearchText = "";
var	lsMIndices;
var	lsMSort = 0;
var	lsMSortDir = false;
var	lsMNewRank = false;
var	lsAttacks = new Array();
var	lsAPerPage = 10;
var	lsAPage = 0;
var	lsAIndices;
var	lsASort = 1;
var	lsASortDir = false;
var	lsASearchText = "";
var	lsASearchType = 0;

var	pRequests = new Array();

var	faForums;
var	faEditing = false;
var	faOriginal;
var	faOriACL;
var	faNewPos;
var	faAccess;

var	raRanks = new Array();
var	raForums;
var	raEditing = false;
var	raOriginal;
var	raDeleting = 0;
var	raDemote;

var	alltt;

function	emptyCB(data) {}


function	PubAlliance(id, tag, name, leader, rank, points, nplanets, cx, cy, vicCond)
{
	this.id = id;
	this.tag = tag;
	this.name = name;
	this.leader = leader;
	this.points = points;
	this.rank = rank;
	this.points = points;
	this.nplanets = nplanets;
	this.cx = cx;
	this.cy = cy;
	this.victory = vicCond;
}

function	initAlliancePage()
{
	var	data = document.getElementById('allinit').innerHTML;
	if	(data.indexOf('\n') != -1)
		generalDataReceived(data);
	else	// IE really sucks
		x_getGeneralData(generalDataReceived);
}

function	generalDataReceived(data)
{
	var	a = data.split('\n');
	var	b = a[0].split('#');

	plAlliance = b[0];
	if	(plAlliance == "")
	{
		inAlliance = false;
		drawPageNoAlliance();
	}
	else if	(a.length == 3)
	{
		inAlliance = false;
		alTag = a[1];
		alName = a[2];
		drawPageNoAlliance();
	}
	else
	{
		if	(!inAlliance)
			drawAllianceMemberLayout();

		lockedAlliances = (b[2] == 1);
		b = a[3].split('#');
		myPubInfo = new PubAlliance(b[0], a[1], a[2], a[4], b[5], b[6], b[1], b[2], b[3], b[8]);
		successor = a[5];
		isDemocracy = (b[4] == "1");
		techTrade = b[7];
		alPrivileges = a[6].split('#');
		alColours[4] = (alPrivileges[1] == 1 && a[8] != '0') ? "red" : null;
		alTag = a[1];
		alName = a[2];
		inAlliance = true;

		if (a[7] == 'List') {
			a[7] = 'Main';
		}
		if	(amPage != a[7])
			eval('switchTo'+a[7]+'Page()');
		else if	(amPage == 'Main')
		{
			drawAllianceMenu();
			drawMainPage();
		}
		else
			drawAllianceMenu();
	}

	gdTimer = setTimeout('x_getGeneralData(generalDataReceived)', 30000);
}


function	clearUpdate()
{
	if	(puTimer)
		clearTimeout(puTimer);
	puTimer = false;
}


//--------------------------------------------------
// ALLIANCE INFORMATION
//--------------------------------------------------

function	getAllianceInfo()
{
	var	v = document.getElementById('alinfo').value;
	if	(v == "")
	{
		alertAllianceInfo(0);
		return;
	}
	x_getTagInformation(v, tagInformationReceived);
}

function	tagInformationReceived(data)
{
	if	(data.indexOf('ERR') == 0)
	{
		var	a = data.split('#');
		alPubInfo = false;
		alertAllianceInfo(parseInt(a[1], 10));
	}
	else
	{
		var	a = data.split('\n');
		var	b = a[0].split('#');
		alPubInfo = new PubAlliance(b[0], a[1], a[2], a[3], b[4], b[5], b[1], b[2], b[3], b[6]);
	}
	drawPubAllianceInfo();
}


//--------------------------------------------------
// ALLIANCE CREATION / REQUEST
//--------------------------------------------------

function	createAlliance()
{
	var	t, n;
	t = document.getElementById('actag').value;
	if	(t == "")
	{
		alertCreate(2);
		return;
	}
	n = document.getElementById('acname').value;
	if	(n == "")
	{
		alertCreate(3);
		return;
	}
	x_createAlliance(t, n, allianceCreated);
}

function	allianceCreated(data)
{
	if	(data.indexOf('ERR') == 0)
	{
		var	a = data.split('#');
		alertCreate(parseInt(a[1], 10));
	}
	else
	{
		updateHeader();
		clearTimeout(gdTimer);
		generalDataReceived(data);
	}
}

function	sendJoinRequest()
{
	var	t;
	t = document.getElementById('aljoin').value;
	if	(t == "")
	{
		alertJoin(0);
		return;
	}
	x_sendRequest(t, requestSent);
}

function requestSent(data) {
	if (data.indexOf('ERR') == 0) {
		var	a = data.split('#');
		alertJoin(parseInt(a[1], 10));
	} else {
		clearTimeout(gdTimer);
		generalDataReceived(data);
	}
}


//--------------------------------------------------
// ALLIANCE MEMBER PAGE
//--------------------------------------------------

function drawAllianceMemberLayout() {
	document.getElementById('allpage').innerHTML = '<form action="?" onSubmit="return false"><table cellspacing="0" cellpadding="0">'
		+ '<tr><td class="center" id="alpmenu">&nbsp;</td><td style="width:5%;vertical-align:top"><a href="manual?p=alliance">Help</a></td></tr>'
		+ '<tr><td colspan="2">&nbsp;</td></tr><tr><td id="alpmain" colspan="2">&nbsp;</td></tr></table></form>';
}

function drawAllianceMenu() {
	var i, j, str = "";
	for (i = 0, j = 0; i < alPages.length; i ++) {
		if (!alCheckPriv[i]()) {
			continue;
		}

		if (j > 0) {
			str += (j == 5 ? "<br/>" : " - ");
		}
		j ++;

		var colour = alColours[i] ? (" style='color: " + alColours[i] + "' ") : "";
		var t = (amPage == alPages[i]);
		str += (t ? ('<b' + colour + '>')
				: ('<a ' + alltt[210] + colour
					+ ' href="#" onClick="switchTo' + alPages[i] + 'Page();return false">')
			) + alPgTitles[i] + (t ? '</b>' : '</a>');
	}
	document.getElementById('alpmenu').innerHTML = str;
}

// Main page (current alliance + alliance information)

function	switchToMainPage()
{
	clearUpdate();
	x_mainPage(emptyCB);
	amPage = "Main";
	asButtons = (asDemocracy != isDemocracy)
		|| (asSuccessor.toLowerCase() != successor.toLowerCase())
		|| (asTechTrade != techTrade);
	hasFocus = -1;
	drawAllianceMenu();
	drawMainPage();
}

function updateASControls() {
	var nst = (asDemocracy != isDemocracy)
		|| (asSuccessor.toLowerCase() != successor.toLowerCase())
		|| (asTechTrade != techTrade);
	if (nst == asButtons) {
		return;
	}
	asButtons = nst;
	if (asButtons) {
		drawASButtons();
	} else {
		document.getElementById('lasbut').innerHTML = '&nbsp;';
	}
}

function resetAllianceSettings() {
	asSuccessor = successor;
	asDemocracy = isDemocracy;
	asTechTrade = techTrade;
	drawMainPage();
}

function	modifyAllianceSettings()
{
	document.getElementById('lasbut').innerHTML = '&nbsp;';
	x_modifySettings(asDemocracy ? 1 : 0, asTechTrade, asSuccessor, aSettingsModified);
}

function aSettingsModified(data) {
	if (data.indexOf('ERR') == 0) {
		var a = data.split('#');
		alertASettings(parseInt(a[1], 10));
		drawASButtons();
	} else {
		asDemocracy = -1;
		clearTimeout(gdTimer);
		generalDataReceived(data);
	}
}

function asFocus() {
	if (hasFocus == 0 && alPrivileges[8] == 1) {
		document.getElementById('lassucc').focus();
	} else if (hasFocus == 3) {
		document.getElementById('alinfo').focus();
	}
}

function	leaderStepDown()
{
	if	(!confirmLSD())
		return;
	x_leaderStepDown(aSettingsModified);
}

function	leaveAlliance()
{
	var	b;
	if	(alPrivileges[8] == 0)
		b = confirmLeave(0);
	else if	(successor != "")
		b = confirmLeave(1);
	else
		b = confirmLeave(isDemocracy ? 2 : 3);
	if	(!b)
		return;
	x_leaveAlliance(aSettingsModified);
}


// Leader election

function	switchToVotePage()
{
	clearUpdate();
	if (!alCheckPriv[1]()) {
		switchToMainPage();
		return;
	}

	amPage = 'Vote';
	drawAllianceMenu();
	drawLoadingText();
	x_getCandidates(candidatesReceived);
}

function	Candidate(id,nVotes,name)
{
	this.id		= id;
	this.nVotes	= nVotes;
	this.name	= name;
}

function	candidatesReceived(data)
{
	leInAction = false;
	if	(amPage != "Vote")
		return;

	if (data == "") {
		switchToMainPage();
		return;
	} else if (data.indexOf('ERR#') == 0) {
		var a = data.split('#');
		alertElection(parseInt(a[1],10));
	} else {
		var	a = data.split('\n');
		var	ll = a.shift().split('#');
		leCanSel = (ll[0] == '1');
		leCanApply = (ll[1] == '1');
		leCanCancel = (ll[2] == '1');
		leCanTake = (ll[3] == '1');
		leSelected = ll[4];
		if	(leNewSel == false && leCanSel)
			leNewSel = leSelected;

		var	i;
		leCandidates = new Array();
		for	(i=0;i<a.length;i++)
		{
			ll = a[i].split('#');
			leCandidates.push(new Candidate(ll.shift(),ll.shift(),ll.join('#')));
		}
	}

	drawElectionLayout();
	updateCandidateList();
	drawElectionButtons();

	puTimer = setTimeout('if (!leInAction) x_getCandidates(candidatesReceived)', 15000);
}

function	updateCandidateList()
{
	prepareCandidateList();
	drawCandidateList();
}

function	applyForPresidency()
{
	clearUpdate();
	leInAction = true;
	leNewSel = false;
	x_runForPresidency(candidatesReceived);
}

function	takePresidency()
{
	clearUpdate();
	leInAction = true;
	leNewSel = false;
	x_takeControl(controlTaken);
}

function controlTaken(data) {
	if (data.indexOf('ERR#') == 0) {
		alertElection(parseInt((data.split('#'))[1], 10));
	}

	clearUpdate();
	leNewSel = false;
	if (gdTimer) {
		clearTimeout(gdTimer);
	}
	gdTimer = false;
	x_getGeneralData(generalDataReceived);
	setTimeout('waitForGd()', 100);
}

function	waitForGd()
{
	if	(gdTimer)
		x_getCandidates(candidatesReceived);
	else
		setTimeout('waitForGd(data)', 100);
}

function	cancelCandidacy()
{
	clearUpdate();
	leNewSel = false;
	leInAction = true;
	x_cancelCandidate(candidatesReceived);
}

function	changeVote()
{
	clearUpdate();
	var	i = leNewSel;
	leNewSel = false;
	leInAction = true;
	x_setVote(i, candidatesReceived);
}

function	prepareCandidateList()
{
	var	i;
	var	st = new Array('Name', 'Votes');
	var	smet = 'leCandidates.sort(lec_' + st[leSort] + '_' + (leSortDir ? "desc" : "asc") + ')';
	eval(smet);
}


// Member listing

function Player(id, rank, current, lastOnline, onVacation, name) {
	this.id		= id;
	this.name	= name;
	this.rank	= rank;
	this.rankText	= "";
	this.rawRank	= "";
	this.current	= current;
	this.lastOnline	= parseInt(lastOnline, 10);
	this.onVacation	= (onVacation == '1');
	this.selected	= false;

	this.getRawRank = function () {
		if (this.rawRank != '') {
			return this.rawRank;
		}
		if (this.rank == '-') {
			this.rawRank = "+";
		} else {
			for (var i = 0; i < lsMRanks.length && lsMRanks[i].id != this.rank; i ++) {
				// Empty loop
			}
			if (i == lsMRanks.length || lsMRanks[i].name == "-") {
				this.rawRank = "-";
			} else {
				this.rawRank = i;
			}
		}
		return this.rawRank;
	};
}


function MLRank(id, name) {
	this.id		= id;
	this.name	= name;
}


function switchToMListPage() {
	clearUpdate();
	if (!alCheckPriv[2]()) {
		switchToMainPage();
		return;
	}

	amPage = 'MList';
	drawAllianceMenu();
	drawLoadingText();
	getMemberList();
}

function getMemberList() {
	if (amPage != 'MList') {
		return;
	}

	var ids = new Array(), i;
	for (i = 0; i < lsMembers.length; i ++) {
		ids.push(lsMembers[i].id);
	}
	x_getMemberList(ids.join('#'), memberListReceived);
}

function memberListReceived(data) {
	if (data == "") {
		lsMembers = new Array();
	} else if (data.indexOf("ERR#") == 0) {
		alertMemberAction(parseInt((data.split('#'))[1], 10));
	} else {
		parseMemberList(data);
	}
	puTimer = setTimeout('updateMListPage()', 60000);
	prepareMemberList();
	drawMemberList();
}

function parseMemberList(data) {
	var ll = data.split('\n');
	var a, i, j, nr, nm;

	a = ll.shift().split('#');
	nr = parseInt(a[0],10);
	nm = parseInt(a[1],10);
	lsMChange = ll.shift().split('#');
	lsMKick = ll.shift().split('#');

	j = nm; lsMRanks = new Array();
	for (i = 0; i < nr; i ++, j ++) {
		a = ll[j].split('#');
		lsMRanks.push(new MLRank(a[0], a[1]));
	}

	for (i = 0; i < nm; i ++) {
		a = ll[i].split('#');
		if (a[0] == '-') {
			removeMember(a[1]);
		} else if (a[0] == '+') {
			a.shift();
			lsMembers.push(new Player(a.shift(), a.shift(), (a.shift() == "0"), a.shift(), a.shift(),
				a.join('#')));
		} else {
			changeMemberRank(a[1], a[2], a[3], a[3]);
		}
	}

	lsMChange.sort(function(x, y) {
		return getRawRankText(x).toLowerCase() > getRawRankText(y).toLowerCase() ? 1 : -1;
	});
}

function getRank(m) {
	if (m.rankText == "") {
		m.rankText = getRankText(m.getRawRank());
	}
	return m.rankText;
}

function getRawRankText(r) {
	for (var i = 0; i < lsMRanks.length && lsMRanks[i].id != r; i ++) {
		// Empty loop
	}
	return getRankText(i);
}

function getCookedRankText(r) {
	for (var i = 0; i < lsMRanks.length && lsMRanks[i].id != r; i ++) {
		// Empty loop
	}
	return getRankText((i == lsMRanks.length || lsMRanks[i].name == '-') ? '-' : i);
}

function removeMember(id) {
	var	k;
	for	(k=0;k<lsMembers.length && lsMembers[k].id != id;k++)
		;
	if	(k!=lsMembers.length)
		lsMembers.splice(k, 1);
}

function changeMemberRank(id, r, lOnline, vacation) {
	var	k;
	for (k = 0; k < lsMembers.length && lsMembers[k].id != id; k ++) {
		// Empty loop
	}
	if (k != lsMembers.length) {
		lsMembers[k].lastOnline = parseInt(lOnline, 10);
		lsMembers[k].onVacation = (vacation == '1');
		if (r != lsMembers[k].rank) {
			lsMembers[k].rank = r;
			lsMembers[k].rankText = "";
			lsMembers[k].rawRank = "";
		}
	}
}

function prepareMemberList() {
	var i;
	lsMIndices = new Array();
	if (lsMSearchText == "") {
		for (i = 0; i < lsMembers.length; i ++) {
			lsMIndices.push(i);
		}
	} else {
		for (i = 0; i < lsMembers.length; i ++) {
			if (lsMembers[i].name.toLowerCase().indexOf(lsMSearchText.toLowerCase()) != -1) {
				lsMIndices.push(i);
			}
		}
	}

	var st = new Array('Name', 'Rank', 'LastOnline', 'Vacation');
	var smet = 'lsMIndices.sort(mls_' + st[lsMSort] + '_' + (lsMSortDir ? "desc" : "asc") + ')';
	eval(smet);
}

function updateSearchMembers() {
	prepareMemberList();
	drawMemberListContents(false);
}

function changeMembers() {
	var	sel = new Array(), ids = new Array(), i;
	var	n = Math.min((lsMPage+1)*lsMPerPage, lsMIndices.length);
	for	(i=lsMPage*lsMPerPage;i<n;i++)
		if	(lsMembers[lsMIndices[i]].selected)
			sel.push(lsMIndices[i]);

	if	(!confirmChangeRank(sel))
		return;

	for	(i=0;i<lsMembers.length;i++)
		ids.push(lsMembers[i].id);
	for	(i=0;i<sel.length;i++)
		sel[i] = lsMembers[sel[i]].id;
	clearUpdate();
	x_changeMemberRank(sel.join('#'), lsMNewRank, ids.join('#'), memberListReceived);
}

function kickMembers() {
	var	sel = new Array(), ids = new Array(), i;
	var	n = Math.min((lsMPage+1)*lsMPerPage, lsMIndices.length);
	for	(i=lsMPage*lsMPerPage;i<n;i++)
		if	(lsMembers[lsMIndices[i]].selected)
			sel.push(lsMIndices[i]);

	if	(!confirmKick(sel))
		return;

	for	(i=0;i<lsMembers.length;i++)
		ids.push(lsMembers[i].id);
	for	(i=0;i<sel.length;i++)
		sel[i] = lsMembers[sel[i]].id;
	clearUpdate();
	x_kickMembers(sel.join('#'), ids.join('#'), memberListReceived);
}


function drawMemberListPanel(firstMember, mCount, canSelect, kickRanks, changeRanks) {
	var i, str = '<table id="mbrlist" class="list" cellspacing="0" cellpadding="0"><tr>';

	// Headers
	if (canSelect) {
		str += '<td class="mselect">&nbsp;</td>';
	}
	for (i = 0; i < 4; i ++) {
		str += '<th ' + alltt[70] + ' onClick="lsMSort=' + i + ';lsMSortDir=!lsMSortDir;updateSearchMembers()" ';
		str += '>' + (lsMSort == i ? '<b>' : '') + mbListHeaders[i];
		if (lsMSort == i) {
			str += '</b><img src="' + staticurl + '/beta5/pics/' + (lsMSortDir ? "up" : "down")
				+ '_' + color + '.gif" alt="' + (lsMSortDir ? "up" : "down") + '" />';
		}
		str += '</th>';
	}
	str += '</tr>';

	// Contents
	for (i = firstMember; i < firstMember + mCount; i ++) {
		var idx = lsMIndices[i];
		str += '<tr>';
		if (canSelect) {
			str += '<td class="mselect">';
			if ( ( kickRanks.indexOf('#' + lsMembers[idx].rank + '#') != -1
					|| changeRanks.indexOf('#' + lsMembers[idx].rank + '#') != -1
					) && !lsMembers[idx].current ) {
				str += '<input ' + alltt[100] + ' type="checkbox" name="mbrsel" value="'
					+ idx+'" ' + (lsMembers[idx].selected ? ' checked="checked"' : '')
					+ 'onClick="with(lsMembers[' + idx
					+ '])selected=!selected;drawMemberListContents(true)" />';
			}
			str += '&nbsp;';
			str += '</td>';
		}
		str += '<td>';
		if (!lsMembers[idx].current) {
			str += '<a ' + alltt[101] + ' href="message?a=c&ct=0&id=' + lsMembers[idx].id + '">';
		}
		str += lsMembers[idx].name + (lsMembers[idx].current ? '' : '</a>') + '</td><td';
		if (lsMembers[idx].rank == "-") {
			str += ' class="mbleader"';
		}
		str += '>' + getRank(lsMembers[idx]) + '</td><td class="'
			+ (lsMembers[idx].lastOnline ? "mboffline" : "mbonline") + '">'
			+ lastOnline(lsMembers[idx]) + '</td><td class="'
			+ (lsMembers[idx].onVacation ? "mbonvac" : "mbnovac") + '">'
			+ vacationMode[lsMembers[idx].onVacation ? 1 : 0] + '</td></tr>';
	}

	str += '</table>';
	return	str;
}

function lastOnline(member) {
	if (member.lastOnline) {
		var str = lastOnlinePrefix;
		var lo, loUnit;
		if (member.lastOnline > 60) {
			lo = (member.lastOnline - (member.lastOnline % 60)) / 60;
			loUnit = 1;
		} else {
			lo = member.lastOnline;
			loUnit = 0;
		}
		str += formatNumber(lo.toString()) + ' ' + lastOnlineUnits[loUnit * 2 + (lo > 1 ? 1 : 0)]
			+ lastOnlineSuffix;
		return str;
	}
	return onlineText;
}


function updateMListPage() {
	if (!alCheckPriv[2]()) {
		switchToMainPage();
		return;
	}
	hasFocus = 0;
	getPlayerList();
}



// Planet list

function Planet(id, owner, msOwner, name, cx, cy, orbit, fact, tur, ua) {
	this.id			= id;
	this.owner		= owner;
	this.msOwner		= msOwner;
	this.name		= name;
	this.coords		= '(' + cx + ',' + cy + ',' + orbit + ')';
	this.factories		= fact;
	this.turrets		= tur;
	this.underAttack	= ua;
}


function switchToPListPage() {
	clearUpdate();
	if (!alCheckPriv[3]()) {
		switchToMainPage();
		return;
	}

	amPage = 'PList';
	drawAllianceMenu();
	drawLoadingText();
	getPlanetList();
}


function getPlanetList() {
	if (amPage != 'PList') {
		return;
	}

	var ids = new Array(), i;
	for (i = 0; i < lsPlanets.length; i ++) {
		ids.push(lsPlanets[i].id);
	}
	x_getPlanetList(ids.join('#'), planetListReceived);
}

function planetListReceived(data) {
	if (amPage != 'PList') {
		return;
	}

	if (data == "") {
		lsPlanets = new Array();
	} else {
		parsePlanetList(data);
	}
	puTimer = setTimeout('updatePListPage()', 180000);
	preparePlanetList();
	drawPlanetList();
}

function updatePListPage() {
	if (!alCheckPriv[3]()) {
		switchToMainPage();
		return;
	}
	hasFocus = 0;
	getPlanetList();
}

function removePlanet(id) {
	var	k;
	for	(k=0;k<lsPlanets.length && lsPlanets[k].id != id;k++)
		;
	if	(k!=lsPlanets.length)
		lsPlanets.splice(k, 1);
}

function replacePlanet(npl) {
	var	k;
	for	(k=0;k<lsPlanets.length && lsPlanets[k].id != npl.id;k++)
		;
	if	(k!=lsPlanets.length)
		lsPlanets.splice(k, 1, npl);
}

function parsePlanetList(data) {
	var	l = data.split("\n");
	var	i;

	for	(i=0;i<l.length;i++)
	{
		var	a = l[i].split('#');
		if	(a[0] == "-")
			removePlanet(a[1]);
		else
		{
			var	op = a.shift(), npl;
			npl = new Planet(a[0],l[i+1],a[1],l[i+2],a[2],a[3],a[4],a[5],a[6],a[7]);
			if	(op == '+')
				lsPlanets.push(npl);
			else
				replacePlanet(npl);
			i += 2;
		}
	}
}

function preparePlanetList() {
	var	i;

	lsPIndices = new Array();
	if	(lsPSearchText == "")
		for	(i=0;i<lsPlanets.length;i++)
			lsPIndices.push(i);
	else
	{
		for	(i=0;i<lsPlanets.length;i++)
		{
			var	b;
			if	(lsPSearchType == 0)
				b = (lsPlanets[i].name.toLowerCase().indexOf(lsPSearchText.toLowerCase()) != -1);
			else
				b = (lsPlanets[i].owner.toLowerCase().indexOf(lsPSearchText.toLowerCase()) != -1);
			if	(b)
				lsPIndices.push(i);
		}
	}

	var	st = new Array('Coord', 'Planet', 'Owner', 'Fact', 'Trt');
	var	smet = 'lsPIndices.sort(pls_' + st[lsPSort] + '_' + (lsPSortDir ? "desc" : "asc") + ')';
	eval(smet);
}

function updateSearchPlanet() {
	preparePlanetList();
	drawPlanetListContents();
}



// Military status

function switchToMStatPage() {
	clearUpdate();
	if (!alCheckPriv[4]()) {
		switchToMainPage();
		return;
	}

	amPage = 'MStat';
	drawAllianceMenu();
	drawLoadingText();
	getMilitaryList();
}

function updateMStatPage() {
	if (amPage != 'MStat') {
		return;
	}

	if (!alCheckPriv[4]()) {
		switchToMainPage();
		return;
	}

	hasFocus = 0;
	getMilitaryList();
}


function MilitarySituation(pid, pname, coords, oid, oname, attPower, defPower, attackers, defenders) {
	this.planetId	= pid;
	this.planetName	= pname;
	this.coords	= coords;
	this.ownerId	= oid;
	this.ownerName	= oname;
	this.attPower	= attPower;
	this.defPower	= defPower;
	this.attackers	= attackers;
	this.defenders	= defenders;
}

function getMilitaryList() {
	if (amPage != 'MStat') {
		return;
	}
	x_getAttackList(militaryListReceived);
}

function militaryListReceived(data) {
	lsAttacks = new Array();
	if (data != "") {
		parseMilitaryList(data);
	}
	puTimer = setTimeout('updateMStatPage()', 180000);
	prepareMilitaryList();
	drawMilitaryList();
}

function parseMilitaryList(data) {
	var	l = data.split('\n'), st = 0, ca, pn, on, att, def;
	while	(l.length > 0)
	{
		var	s = l.shift();
		if	(st == 0)
		{
			ca = s.split('#');
			ca[4] = parseInt(ca[4], 10);
			ca[5] = parseInt(ca[5], 10);
			on = '';
			att = new Array();
			def = new Array();
			st = 1;
		}
		else if	(st == 1)
		{
			pn = s;
			if	(ca[1] != "")
				st = 2;
			else if	(ca[4] > 0)
				st = 3;
			else if	(ca[5] > 0)
				st = 4;
			else
			{
				lsAttacks.push(new MilitarySituation(ca[0],pn,ca[6],'','',ca[3],ca[2],att,def));
				st = 0;
			}
		}
		else if	(st == 2)
		{
			on = s;
			if	(ca[4] > 0)
				st = 3;
			else if	(ca[5] > 0)
				st = 4;
			else
			{
				lsAttacks.push(new MilitarySituation(ca[0],pn,ca[6],ca[1],on,ca[3],ca[2],att,def));
				st = 0;
			}
		}
		else if	(st == 3)
		{
			def.push(s);
			ca[4] --;
			if	(ca[4] == 0)
			{
				if	(ca[5] > 0)
					st = 4;
				else
				{
					lsAttacks.push(new MilitarySituation(ca[0],pn,ca[6],ca[1],on,ca[3],ca[2],att,def));
					st = 0;
				}
			}
		}
		else if	(st == 4)
		{
			att.push(s);
			ca[5] --;
			if	(ca[5] == 0)
			{
				lsAttacks.push(new MilitarySituation(ca[0],pn,ca[6],ca[1],on,ca[3],ca[2],att,def));
				st = 0;
			}
		}
	}
}

function prepareMilitaryList() {
	var	i;
	lsAIndices = new Array();
	if (lsASearchText == "") {
		for	(i=0;i<lsAttacks.length;i++)
			lsAIndices.push(i);
	} else {
		for (i=0;i<lsAttacks.length;i++) {
			var	b;
			if	(lsASearchType == 0 || alPrivileges[0] < 2)
				b = (lsAttacks[i].planetName.toLowerCase().indexOf(lsASearchText.toLowerCase()) != -1);
			else
				b = (lsAttacks[i].ownerName.toLowerCase().indexOf(lsASearchText.toLowerCase()) != -1);
			if	(b)
				lsAIndices.push(i);
		}
	}

	var	st = new Array('Coords', 'Name', 'Owner', 'Def', 'Att');
	var	smet = 'lsAIndices.sort(als_' + st[lsASort] + '_' + (lsASortDir ? "desc" : "asc") + ')';
	eval(smet);
}

function updateSearchMilitary() {
	prepareMilitaryList();
	drawMilitaryListContents();
}


// Pending requests page

function	switchToPendingPage()
{
	clearUpdate();
	if	(alPrivileges[4] != 1)
	{
		switchToMainPage();
		return;
	}

	amPage = 'Pending';
	drawAllianceMenu();
	drawLoadingText();
	getPendingRequests();
}

function	PendingRequest(id, player)
{
	this.id		= id;
	this.player	= player;
	this.selected	= false;
}

function	getPendingRequests()
{
	if	(amPage != 'Pending')
		return;

	var	ids = new Array(), i;
	for	(i=0;i<pRequests.length;i++)
		ids.push(pRequests[i].id);
	x_getPending(ids.join('#'), pendingRequestsReceived);
}

function	removePendingRequest(id)
{
	var	k;
	for	(k=0;k<pRequests.length && pRequests[k].id != id;k++)
		;
	if	(k!=pRequests.length)
		pRequests.splice(k, 1);
}

function	pendingRequestsReceived(data)
{
	if (data.indexOf("ERR#") == 0) {
		alertPending(parseInt((data.split('#'))[1], 10));
	} else if (data != "") {
		var	l = data.split("\n");
		var	i, j;

		for	(i=0;i<l.length;i++)
		{
			var	a = l[i].split('#');
			if	(a[0] == "-")
				removePendingRequest(a[1]);
			else
			{
				a.shift();
				j = a.shift();
				pRequests.push(new PendingRequest(j, a.join('#')));
			}
		}
	}
	puTimer = setTimeout('getPendingRequests()', 180000);
	drawPendingPage();
}

function	getSelectedRequests()
{
	var	a = new Array(), i;
	for	(i=0;i<pRequests.length;i++)
		if	(pRequests[i].selected)
			a.push(pRequests[i].id);
	return	a;
}

function	acceptRequests()
{
	var	a = getSelectedRequests();
	if	(a.length == 0)
	{
		alertRequests(true);
		return;
	}
	if	(!confirmRequests(true, a.length > 1))
		return;
	if	(puTimer && amPage == "Pending")
	{
		clearUpdate();
		var	ids = new Array(), i;
		for	(i=0;i<pRequests.length;i++)
			ids.push(pRequests[i].id);
		x_acceptRequests(ids.join('#'), a.join('#'), pendingRequestsReceived);
	}
}

function	rejectRequests()
{
	var	a = getSelectedRequests();
	if	(a.length == 0)
	{
		alertRequests(false);
		return;
	}
	if	(!confirmRequests(false, a.length > 1))
		return;
	if	(puTimer && amPage == "Pending")
	{
		clearUpdate();
		var	ids = new Array(), i;
		for	(i=0;i<pRequests.length;i++)
			ids.push(pRequests[i].id);
		x_rejectRequests(ids.join('#'), a.join('#'), pendingRequestsReceived);
	}
}


// Forums administation

function	switchToFAdminPage()
{
	clearUpdate();
	if	(alPrivileges[5] != 1)
	{
		switchToMainPage();
		return;
	}

	amPage = 'FAdmin';
	faEditing = false;
	drawAllianceMenu();
	drawLoadingText();
	x_getForums(forumsReceived);
}

function	AllianceForum(id,order,userPost,name,description)
{
	this.id			= id;
	this.order		= order;
	this.userPost		= userPost;
	this.name		= name;
	this.description	= description;
}

function	ForumACL(id,priv,name)
{
	this.id		= id;
	this.priv	= priv;
	this.name	= name;
	this.selected	= false;
}

function forumsReceived(data) {
	if (amPage != 'FAdmin') {
		return;
	}

	faForums = new Array();
	if (data.indexOf("ERR#") == 0) {
		alertForum(parseInt((data.split('#'))[1], 10));
		puTimer = setTimeout('x_getForums(forumsReceived)', 180000);
		return;
	} else if (data != "") {
		parseForumList(data);
	}

	if (!faEditing) {
		drawForumList();
	} else if (!faEditing.id && faForums.length >= 30) {
		alertMaximumFCount();
		forumEditCancel();
	} else if (!faEditing.id) {
		if	(faNewPos != -1 && !forumById(faNewPos))
			faNewPos = -1;
		updateFPosSelector();
	} else if (faEditing.id) {
		var	f = forumById(faEditing.id);
		if (!f) {
			alertForumDeleted();
			forumEditCancel();
		} else if (faOriginal.name != f.name || faOriginal.userPost != f.userPost || faOriginal.description != f.description) {
			alertForumChanged();
			updateFEditor();
		} else {
			updateFEditor();
		}
	}
	puTimer = setTimeout('x_getForums(forumsReceived)', 180000);
}

function	parseForumList(data)
{
	var	dl = data.split('\n');
	var	st = 0, i = 0, cf = 0;
	var	a;

	while	(i<dl.length)
	{
		if	(st == 0)
		{
			a = dl[i].split('#');
			faForums[cf] = new AllianceForum(a.shift(),a.shift(),(a.shift()=="1"),a.join('#'),'');
			st = 1;
			i ++;
		}
		else if	(st == 1)
		{
			if	(dl[i].indexOf('+#') == 0)
			{
				a = dl[i].split('#');
				a.shift();
				a = a.join('#');
				if	(faForums[cf].description != '')
					faForums[cf].description += '\n';
				faForums[cf].description += a;
				i++;
			}
			else
			{
				cf++;
				st = 0;
			}
		}
	}

	faForums.sort(new Function('a','b','return (a.order > b.order ? 1 : -1)'));
}

function	moveForum(id,up)
{
	x_moveForum(id, up ? 1 : 0, forumsReceived);
}

function	forumById(id)
{
	var	i;
	for	(i=0;i<faForums.length&&faForums[i].id!=id;i++)
		;
	return	(i==faForums.length) ? false : faForums[i];
}

function	deleteForum(id)
{
	var	f = forumById(id);
	if	(!(f && confirmDeleteForum(f.name)))
		return;
	x_delForum(id, forumsReceived);
}

function	createForum()
{
	faEditing = new AllianceForum(false,false,true,'','');
	if	(faForums.length)
		faNewPos = faForums[faForums.length - 1].id;
	else
		faNewPos = -1;
	displayForumEditor();
}

function	editForum(id)
{
	var	f = forumById(id);
	if	(!f)
		return;
	faEditing = new AllianceForum(id,f.order,f.userPost,f.name,f.description);
	faOriginal = new AllianceForum(id,f.order,f.userPost,f.name,f.description);
	displayForumEditor();
}

function	displayForumEditor()
{
	document.getElementById('crforum').innerHTML = '';
	drawForumEditor();
	x_getForumAcl(faEditing.id ? faEditing.id : '', forumAclReceived);
}

function	forumAclReceived(data)
{
	faAccess = new Array();
	if	(data != "")
	{
		var	i, l = data.split('\n');
		for	(i=0;i<l.length;i++)
		{
			var	a = l[i].split('#');
			faAccess.push(new ForumACL(a.shift(), a.shift(), a.join('#')));
		}
		faAccess.sort(new Function('a','b','return a.name.toLowerCase() > b.name.toLowerCase() ? 1 : -1'));
	}
	if	(faEditing.id)
		faOriACL = makeForumACLString();
	drawFAccessManager();
	updateFEditor();
}

function	setFAccessLevel(level)
{
	var	i,cc=0;
	for	(i=0;i<faAccess.length;i++)
	{
		if	(faAccess[i].priv == 3 || !faAccess[i].selected || faAccess[i].priv == level)
			continue;
		cc++;
		faAccess[i].priv = level;
	}
	if	(cc > 0)
	{
		drawFAccessManager();
		updateFEditor();
	}
}

function	updateFEditor()
{
	var	ok, i;
	ok = (faEditing.name.length >= 4);
	if	(faEditing.id)
		ok = ok && (
			faEditing.name != faOriginal.name || faEditing.description != faOriginal.description
			|| faEditing.userPost != faOriginal.userPost || faOriACL != makeForumACLString()
		);
	document.getElementById('feok').disabled = !ok;
}

function	makeForumACLString()
{
	var	a = new Array(), i;
	for	(i=0;i<faAccess.length;i++)
		if	(faAccess[i].priv != 3 && faAccess[i].priv != 0)
			a.push(faAccess[i].id + '!' + faAccess[i].priv);
	return	a.join('#');
}

function	forumEditOk()
{
	if	(faEditing.id)
		x_changeForum(faEditing.id, faEditing.name, faEditing.userPost ? 1 : 0, faEditing.description, makeForumACLString(), forumEditCallback);
	else
		x_newForum(faEditing.name, faEditing.userPost ? 1 : 0, faNewPos, faEditing.description, makeForumACLString(), forumEditCallback);
}

function	forumEditCancel()
{
	faEditing = false;
	drawForumList();
}

function forumEditCallback(data) {
	if (data.indexOf('ERR#') == 0) {
		var	a = data.split('#');
		a.shift();
		alertForum(parseInt(a[0], 10));
		if (a[0] == 3) {
			clearUpdate();
			faEditing = false;
			x_getForums(forumsReceived);
		}
	} else {
		clearUpdate();
		faEditing = false;
		forumsReceived(data);
	}
}


//--------------------------------------------------
// RANKS MANAGEMENT
//--------------------------------------------------

function	switchToRAdminPage()
{
	clearUpdate();
	if	(alPrivileges[8] != 1)
	{
		switchToMainPage();
		return;
	}

	amPage = 'RAdmin';
	raEditing = false;
	raDeleting = 0;
	drawAllianceMenu();
	drawLoadingText();
	x_getRanks(ranksReceived);
}

function Rank(id, name, players, list, att, srank, kick, accept, fadmin, dipl, vote, cand, techTrade) {
	this.id			= id;
	this.name		= name;
	this.players		= players;
	this.list		= list;
	this.attacks		= att;
	this.canSet		= srank;
	this.canKick		= kick;
	this.accept		= accept;
	this.forumAdmin		= fadmin;
	this.diplContact	= dipl;
	this.canVote		= vote;
	this.candidate		= cand;
	this.techTrade		= parseInt(techTrade, 10);
	this.ranksKick		= new Array();
	this.ranksChange	= new Array();
	this.fRead		= new Array();
	this.fMod		= new Array();
	this.open		= false;
}

function	RForum(id,name)
{
	this.id		= id;
	this.name	= name;
}

function ranksReceived(data) {
	var l = data.split('\n'), a = l.shift().split('#');
	var nf = a[0], nr = a[1], i;

	raForums = new Array();
	for (i = 0; i < nf; i ++) {
		a = l.shift().split('#');
		raForums.push(new RForum(a.shift(), a.join()));
	}
	raForums.sort(new Function('a','b','return a.name.toLowerCase()>b.name.toLowerCase()?1:-1'));

	var rr = new Array();
	for (i = 0; i < nr; i ++) {
		var	r, n, i;
		a = l.shift().split('#');
		r = new Rank(a[0], l.shift(), a[1], a[2], a[3] == "1", a[4] == "1", a[5] == "1",
			a[6] == "1", a[7] == "1", a[8] == "1", a[9] == 1, a[10] == "1", a[11]);
		if (r.canKick) {
			if (l[0] != "") {
				r.ranksKick = l.shift().split('#');
			} else {
				l.shift();
			}
		}
		if (r.canSet) {
			if (l[0] != "") {
				r.ranksChange = l.shift().split('#');
			} else {
				l.shift();
			}
		}
		if (l[0] == "") {
			l.shift();
		} else {
			r.fRead = l.shift().split('#');
		}
		if (l[0] == "") {
			l.shift();
		} else {
			r.fMod = l.shift().split('#');
		}
		n = rankById(r.id);
		if (n) {
			r.open = n.open;
		}
		rr.push(r);
	}
	rr.sort(new Function('a','b','return a.name.toLowerCase()>b.name.toLowerCase()?1:-1'));
	raRanks = rr;

	if (!(raEditing || raDeleting)) {
		drawRanksList();
	}

	puTimer = setTimeout('x_getRanks(ranksReceived)', 180000);
}

function	rankById(id)
{
	var	i;
	for	(i=0;i<raRanks.length&&raRanks[i].id!=id;i++) ;
	return (i == raRanks.length) ? false : raRanks[i];
}

function	raForumById(id)
{
	var	i;
	for	(i=0;i<raForums.length&&raForums[i].id!=id;i++) ;
	return (i == raForums.length) ? false : raForums[i];
}

function	showRank(i)
{
	raRanks[i].open = !raRanks[i].open;
	drawRanksList();
}

function createRank() {
	raEditing = new Rank(false, '', 0, 3, true, 0, 0, false, false, false, true, true, 0);
	displayRankEditor();
}

function editRank(idx) {
	raEditing = new Rank(
		raRanks[idx].id, raRanks[idx].name, raRanks[idx].players, raRanks[idx].list, raRanks[idx].attacks,
		raRanks[idx].canSet ? (raRanks[idx].ranksChange.length == raRanks.length - 1 ? 2 : 1) : 0,
		raRanks[idx].canKick ? (raRanks[idx].ranksKick.length == raRanks.length - 1 ? 2 : 1) : 0,
		raRanks[idx].accept, raRanks[idx].forumAdmin, raRanks[idx].diplContact,
		raRanks[idx].canVote, raRanks[idx].candidate, raRanks[idx].techTrade
	);
	raEditing.ranksChange = raEditing.ranksChange.concat(raRanks[idx].ranksChange);
	raEditing.ranksChange.sort(new Function('a','b','return a-b'));
	raEditing.ranksKick = raEditing.ranksKick.concat(raRanks[idx].ranksKick);
	raEditing.ranksKick.sort(new Function('a','b','return a-b'));
	raEditing.fRead = raEditing.fRead.concat(raRanks[idx].fRead);
	raEditing.fRead.sort(new Function('a','b','return a-b'));
	raEditing.fMod = raEditing.fMod.concat(raRanks[idx].fMod);
	raEditing.fMod.sort(new Function('a','b','return a-b'));

	raOriginal = new Rank(
		raEditing.id, raEditing.name, raEditing.players, raEditing.list, raEditing.attacks,
		raEditing.canSet, raEditing.canKick, raEditing.accept, raEditing.forumAdmin,
		raEditing.diplContact, raEditing.canVote, raEditing.candidate, raEditing.techTrade
	);
	raOriginal.ranksChange = raOriginal.ranksChange.concat(raEditing.ranksChange);
	raOriginal.ranksKick = raOriginal.ranksKick.concat(raEditing.ranksKick);
	raOriginal.fRead = raOriginal.fRead.concat(raEditing.fRead);
	raOriginal.fMod = raOriginal.fMod.concat(raEditing.fMod);

	displayRankEditor();
}

function	drawRERankList(can, list, func, lstid, sm)
{
	if	(can != 1 || (raRanks.length == 1 && raEditing.id))
	{
		var	e = document.getElementById('re'+lstid+'list');
		if	(e)
			e.innerHTML = ' ';
		return;
	}

	var	i, n = 0, kl, str = '<table cellspacing="0" cellpadding="1" class="rerlist">';
	kl = '!' + list.join('!') + '!';
	for	(i=0;i<raRanks.length;i++)
	{
		if	(raEditing.id && raRanks[i].id == raEditing.id)
			continue;
		if	(n%3 == 0)
			str += '<tr>';
		str += '<td><label><input ' + alltt[220] + ' type="checkbox" name="re'+lstid+'rank" value="'+i+'" onClick="rankSwitch'+func+'('+i+');updateREditor()"';
		str += (kl.indexOf('!' + raRanks[i].id + '!') == -1 ? '' : ' checked="checked"') + ' /> ';
		str += (raRanks[i].name == '-' ? ('<i>'+sm+'</i>') : raRanks[i].name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')) + '</label></td>';
		if	(n%3 == 2)
			str += '</tr>';
		n++;
	}
	if	(n%3 != 0)
	{
		while	(n%3 != 0)
		{
			str += '<td>&nbsp;</td>';
			n++;
		}
		str += '</tr>';
	}
	str += '</table>';
	document.getElementById('re'+lstid+'list').innerHTML = str;
}

function updateREditor() {
	var	ok;
	if (!raEditing.id || (raEditing.id && raOriginal.name != "-")) {
		ok = (raEditing.name.length > 3);
	} else {
		ok = true;
	}
	if (raEditing.id) {
		raEditing.ranksChange.sort(new Function('a','b','return a-b'));
		raEditing.ranksKick.sort(new Function('a','b','return a-b'));
		raEditing.fRead.sort(new Function('a','b','return a-b'));
		raEditing.fMod.sort(new Function('a','b','return a-b'));
		ok = ok && (
			(raEditing.name != raOriginal.name) || (raEditing.list != raOriginal.list)
			|| (raEditing.attacks != raOriginal.attacks) || (raEditing.canSet != raOriginal.canSet)
			|| (raEditing.canKick != raOriginal.canKick) || (raEditing.accept != raOriginal.accept)
			|| (raEditing.forumAdmin != raOriginal.forumAdmin) || (raEditing.canVote != raOriginal.canVote)
			|| (raEditing.diplContact != raOriginal.diplContact)
			|| (raEditing.candidate != raOriginal.candidate)
			|| (raEditing.techTrade != raOriginal.techTrade)
			|| (raEditing.ranksChange.join('#') != raOriginal.ranksChange.join('#'))
			|| (raEditing.ranksKick.join('#') != raOriginal.ranksKick.join('#'))
			|| (raEditing.fRead.join('#') != raOriginal.fRead.join('#'))
			|| (raEditing.fMod.join('#') != raOriginal.fMod.join('#'))
		);
	}
	document.getElementById('reok').disabled = !ok;
}

function rankEditCancel() {
	raEditing = false;
	drawRanksList();
}

function rankEditOk() {
	var	i, prs, rkick, rchange, fread, fmod;

	if	(raEditing.canSet == 1 && raEditing.ranksChange.length == 0)
		raEditing.canSet = 0;
	if	(raEditing.canKick == 1 && raEditing.ranksKick.length == 0)
		raEditing.canKick = 0;

	prs  = raEditing.list + '#' + (raEditing.attacks ? '1' : '0') + '#';
	prs += (raEditing.canSet != 0 ? '1' : '0') + '#' + (raEditing.canKick != 0 ? '1' : '0') + '#';
	prs += (raEditing.accept ? '1' : '0') + '#' + (raEditing.forumAdmin ? '1' : '0') + '#';
	prs += (raEditing.diplContact ? '1' : '0') + '#' + (raEditing.canVote ? '1' : '0') + '#';
	prs += (raEditing.candidate ? '1' : '0') + '#' + raEditing.techTrade;

	if	(raEditing.canSet == 1)
		rchange = raEditing.ranksChange.join('#');
	else
		rchange = "";

	if	(raEditing.canKick == 1)
		rkick = raEditing.ranksKick.join('#');
	else
		rkick = "";

	if	(raEditing.forumAdmin)
		fread = fmod = "";
	else
	{
		fread = raEditing.fRead.join('#');
		fmod = raEditing.fMod.join('#');
	}

	if (raEditing.id) {
		x_changeRank(raEditing.id, raEditing.name, prs, rkick, rchange, fread, fmod, rankEdited);
	} else {
		x_newRank(raEditing.name, prs, rkick, rchange, fread, fmod, rankEdited);
	}
}

function rankEdited(data) {
	if (data.indexOf('ERR#') == 0) {
		alertRank(parseInt(data.replace(/ERR#/, ''), 10));
	} else {
		raEditing = false;
		clearUpdate();
		ranksReceived(data);
	}
}

function	rankSwitchKick(idx)
{
	var	id = raRanks[idx].id;
	var	ks = '!' + raEditing.ranksKick.join('!') + '!';
	if	(ks.indexOf('!' + id + '!') == -1)
		raEditing.ranksKick.push(id);
	else
	{
		var	i;
		for	(i=0;i<raEditing.ranksKick.length&&raEditing.ranksKick[i]!=id;i++) ;
		if	(i<raEditing.ranksKick.length)
			raEditing.ranksKick.splice(i, 1);
	}
}

function	rankSwitchChange(idx)
{
	var	id = raRanks[idx].id;
	var	cs = '!' + raEditing.ranksChange.join('!') + '!';
	if	(cs.indexOf('!' + id + '!') == -1)
		raEditing.ranksChange.push(id);
	else
	{
		var	i;
		for	(i=0;i<raEditing.ranksChange.length&&raEditing.ranksChange[i]!=id;i++) ;
		if	(i<raEditing.ranksChange.length)
			raEditing.ranksChange.splice(i, 1);
	}
}

function	rankSetForum(idx, l)
{
	var	fid = raForums[idx].id;
	var	isRd = (('!' + raEditing.fRead.join('!') + '!').indexOf('!'+fid+'!') != -1);
	var	isMod = (('!' + raEditing.fMod.join('!') + '!').indexOf('!'+fid+'!') != -1);
	if	(l == 0)
	{
		var	i, a = isRd ? raEditing.fRead : raEditing.fMod;
		for	(i=0;i<a.length&&a[i]!=fid;i++) ;
		a.splice(i, 1);
	}
	else if	(l == 1)
	{
		if	(isMod)
		{
			var	i, a = raEditing.fMod;
			for	(i=0;i<a.length&&a[i]!=fid;i++) ;
			a.splice(i, 1);
		}
		raEditing.fRead.push(fid);
	}
	else
	{
		if	(isRd)
		{
			var	i, a = raEditing.fRead;
			for	(i=0;i<a.length&&a[i]!=fid;i++) ;
			a.splice(i, 1);
		}
		raEditing.fMod.push(fid);
	}
}

function	deleteRank(idx)
{
	var	i;
	for	(i=0;i<raRanks.length&&raRanks[i].name!="-";i++) ;
	if	(i==raRanks.length)
		return;

	if	(raRanks[idx].players == 0 && confirmDeleteRank(raRanks[idx].name))
		x_delRank(raRanks[idx].id, raRanks[i].id, rankDeleted);
	else if	(raRanks[idx].players > 0)
	{
		raDeleting = raRanks[idx];
		raDemote = raRanks[i].id;
		showRankDeletePage();
	}
}

function rankDeleteOk() {
	x_delRank(raDeleting.id, raDemote, rankDeleted);
}

function	rankDeleteCancel()
{
	raDeleting = false;
	drawRanksList();
}

function rankDeleted(data) {
	raDeleting = false;
	clearUpdate();
	if (data.indexOf('ERR#') == 0) {
		alertRank(parseInt((data.split('#'))[1], 10));
	} else {
		ranksReceived(data);
	}
}


//--------------------------------------------------
// LIST SORTING
//--------------------------------------------------

function pls_Coord_asc(a,b)
{
	var ra = lsPlanets[a].coords.toLowerCase(); var rb = lsPlanets[b].coords.toLowerCase();
	return	(ra < rb) ? -1 : 1;
}

function pls_Coord_desc(a,b)
{
	var ra = lsPlanets[a].coords.toLowerCase(); var rb = lsPlanets[b].coords.toLowerCase();
	return	(ra < rb) ? 1 : -1;
}

function pls_Planet_asc(a,b)
{
	var ra = lsPlanets[a].name.toLowerCase(); var rb = lsPlanets[b].name.toLowerCase();
	return	(ra < rb) ? -1 : 1;
}

function pls_Planet_desc(a,b)
{
	var ra = lsPlanets[a].name.toLowerCase(); var rb = lsPlanets[b].name.toLowerCase();
	return	(ra < rb) ? 1 : -1;
}

function pls_Owner_asc(a,b)
{
	var ra = lsPlanets[a].owner.toLowerCase(); var rb = lsPlanets[b].owner.toLowerCase();
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : pls_Planet_asc(a,b));
}

function pls_Owner_desc(a,b)
{
	var ra = lsPlanets[a].owner.toLowerCase(); var rb = lsPlanets[b].owner.toLowerCase();
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : pls_Planet_desc(a,b));
}

function pls_Fact_asc(a,b)
{
	var ra = parseInt(lsPlanets[a].factories, 10); var rb = parseInt(lsPlanets[b].factories, 10);
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : pls_Planet_asc(a,b));
}

function pls_Fact_desc(a,b)
{
	var ra = parseInt(lsPlanets[a].factories, 10); var rb = parseInt(lsPlanets[b].factories, 10);
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : pls_Planet_desc(a,b));
}

function pls_Trt_asc(a,b)
{
	var ra = parseInt(lsPlanets[a].turrets, 10); var rb = parseInt(lsPlanets[b].turrets, 10);
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : pls_Planet_asc(a,b));
}

function pls_Trt_desc(a,b)
{
	var ra = parseInt(lsPlanets[a].turrets, 10); var rb = parseInt(lsPlanets[b].turrets, 10);
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : pls_Planet_desc(a,b));
}

function mls_Name_asc(a,b) {
	var ra = lsMembers[a].name.toLowerCase(); var rb = lsMembers[b].name.toLowerCase();
	return  (ra < rb) ? -1 : 1;
}

function mls_Name_desc(a,b) {
	var ra = lsMembers[a].name.toLowerCase(); var rb = lsMembers[b].name.toLowerCase();
	return  (ra > rb) ? -1 : 1;
}

function mls_LastOnline_asc(a,b) {
	var ra = lsMembers[a].lastOnline; var rb = lsMembers[b].lastOnline;
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : mls_Name_desc(a,b));
}

function mls_LastOnline_desc(a,b) {
	var ra = lsMembers[a].lastOnline; var rb = lsMembers[b].lastOnline;
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : mls_Name_asc(a,b));
}

function mls_Vacation_asc(a,b) {
	var ra = lsMembers[a].onVacation ? 1 : 0; var rb = lsMembers[b].onVacation ? 1 : 0;
	return (ra == rb ? mls_Name_asc(a,b) : (ra - rb));
}

function mls_Vacation_desc(a,b) {
	var ra = lsMembers[a].onVacation ? 1 : 0; var rb = lsMembers[b].onVacation ? 1 : 0;
	return (ra == rb ? mls_Name_desc(a,b) : (rb - ra));
}

function mls_Rank_asc(a,b) {
	var ra = lsMembers[a].getRawRank(), rb = lsMembers[b].getRawRank();
	return (ra == rb) ? mls_Name_asc(a, b)
		: ((ra == '+' || rb == '-') ? -1
		: ((ra == '-' || rb == '+') ? 1
		: ((getRank(lsMembers[a]).toLowerCase() < getRank(lsMembers[b]).toLowerCase()) ? -1 : 1)));
}

function mls_Rank_desc(a,b) {
	var ra = lsMembers[a].getRawRank(), rb = lsMembers[b].getRawRank();
	return (ra == rb) ? mls_Name_asc(a, b)
		: ((ra == '+' || rb == '-') ? 1
		: ((ra == '-' || rb == '+') ? -1
		: ((getRank(lsMembers[a]).toLowerCase() < getRank(lsMembers[b]).toLowerCase()) ? 1 : -1)));
}

function als_Coords_asc(a,b)
{
	var ra = lsAttacks[a].coords.toLowerCase(); var rb = lsAttacks[b].coords.toLowerCase();
	return	(ra < rb) ? -1 : 1;
}

function als_Coords_desc(a,b)
{
	var ra = lsAttacks[a].coords.toLowerCase(); var rb = lsAttacks[b].coords.toLowerCase();
	return	(ra < rb) ? 1 : -1;
}

function als_Name_asc(a,b)
{
	var ra = lsAttacks[a].planetName.toLowerCase(); var rb = lsAttacks[b].planetName.toLowerCase();
	return	(ra < rb) ? -1 : 1;
}

function als_Name_desc(a,b)
{
	var ra = lsAttacks[a].planetName.toLowerCase(); var rb = lsAttacks[b].planetName.toLowerCase();
	return	(ra < rb) ? 1 : -1;
}

function als_Owner_asc(a,b)
{
	var ra = lsAttacks[a].ownerName.toLowerCase(); var rb = lsAttacks[b].ownerName.toLowerCase();
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : als_Planet_asc(a,b));
}

function als_Owner_desc(a,b)
{
	var ra = lsAttacks[a].ownerName.toLowerCase(); var rb = lsAttacks[b].ownerName.toLowerCase();
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : als_Planet_desc(a,b));
}

function als_Def_asc(a,b)
{
	var ra = parseInt(lsAttacks[a].defPower, 10); var rb = parseInt(lsAttacks[b].defPower, 10);
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : als_Name_asc(a,b));
}

function als_Def_desc(a,b)
{
	var ra = parseInt(lsAttacks[a].defPower, 10); var rb = parseInt(lsAttacks[b].defPower, 10);
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : als_Name_desc(a,b));
}

function als_Att_asc(a,b)
{
	var ra = parseInt(lsAttacks[a].attPower, 10); var rb = parseInt(lsAttacks[b].attPower, 10);
	return  (ra < rb) ? -1 : ((ra > rb) ? 1 : als_Name_asc(a,b));
}

function als_Att_desc(a,b)
{
	var ra = parseInt(lsAttacks[a].attPower, 10); var rb = parseInt(lsAttacks[b].attPower, 10);
	return  (ra < rb) ? 1 : ((ra > rb) ? -1 : als_Name_desc(a,b));
}

function lec_Name_asc(a,b)
{
	var ra = a.name.toLowerCase(), rb = b.name.toLowerCase();
	return	(ra > rb) ? 1 : -1;
}

function lec_Name_desc(a,b)
{
	var ra = a.name.toLowerCase(), rb = b.name.toLowerCase();
	return	(ra < rb) ? 1 : -1;
}

function lec_Votes_asc(a,b)
{
	var ra = parseInt(a.nVotes,10), rb = parseInt(b.nVotes,10);
	return	(ra > rb) ? 1 : ((ra < rb) ? -1 : lec_Name_asc(a,b));
}

function lec_Votes_desc(a,b)
{
	var ra = parseInt(b.nVotes,10), rb = parseInt(a.nVotes,10);
	return	(ra > rb) ? 1 : ((ra < rb) ? -1 : lec_Name_desc(a,b));
}
