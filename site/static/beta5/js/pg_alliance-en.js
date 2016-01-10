var	alPgTitles = new Array(
			'Main page', 'Leader election', 'Members', 'Planets',
			'Military status', 'Pending requests', 'Forums admin.',
			'Ranks admin.'
		);
var	mbListHeaders = ['Name', 'Rank', 'Last online', 'On vacation'];
var	onlineText = "Currently online";
var	lastOnlinePrefix = "";
var	lastOnlineUnits = ['minute', 'minutes', 'hour', 'hours'];
var	lastOnlineSuffix = " ago";
var	vacationMode = ['No', 'Yes'];

function        makeAllianceTooltips()
{
        alltt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<300;i++)
			alltt[i] = "";
		return;
	}
	alltt[0] = tt_Dynamic("Use this text field to type in the tag of the alliance you wish to join");
	alltt[1] = tt_Dynamic("Click here to send your joining request to the alliance's leadership");
	alltt[2] = tt_Dynamic("Use this text field to type in the tag of the alliance you wish to create");
	alltt[3] = tt_Dynamic("Use this text field to type in the full name of the alliance you wish to create");
	alltt[4] = tt_Dynamic("Click here to create the given alliance");
	alltt[5] = tt_Dynamic("Click here to cancel your joining request");
	alltt[10] = tt_Dynamic("Use this text field to type in the tag of the alliance you wish to get information about");
	alltt[11] = tt_Dynamic("Click here to display the information about the alliance you've provided the tag for");
	alltt[20] = tt_Dynamic("Click here to step down as leader of this alliance");
	alltt[21] = tt_Dynamic("Click here to leave this alliance");
	alltt[22] = tt_Dynamic("Use this radio button to switch the alliance in dictatorial mode");
	alltt[23] = tt_Dynamic("Use this radio button to switch the alliance in democratric mode");
	alltt[24] = tt_Dynamic("Use this text field to type in the name of your successor as leader of the alliance");
	alltt[30] = tt_Dynamic("Click here to update the alliance general settings according to your changes");
	alltt[31] = tt_Dynamic("Click here to reset the alliance general settings to their previous values");
	alltt[40] = tt_Dynamic("Click here to take the presidency of the alliance");
	alltt[41] = tt_Dynamic("Click here to apply to presidency of the alliance");
	alltt[42] = tt_Dynamic("Click here to cancel your candidacy for the presidency of the alliance");
	alltt[43] = tt_Dynamic("Click here to change your vote for the alliance presidency according to what you have selected in the candidates list");
	alltt[50] = tt_Dynamic("Click here to sort the candidates list according to this field and switch between ascending and descending ordering");
	alltt[51] = tt_Dynamic("Use this radio button to abstain in the presidency election");
	alltt[52] = tt_Dynamic("Use this radio button to select which candidate you wish to vote for in the presidency election");
	alltt[60] = tt_Dynamic("Use this drop down list to select the alliance listing you wish to display");
	alltt[61] = tt_Dynamic("Use this drop down list to select the number of planets to display on each page of the listing");
	alltt[62] = tt_Dynamic("Use this radio button if you wish to search for a planet in the listing");
	alltt[63] = tt_Dynamic("Use this radio button if you wish to search for a player in the listing");
	alltt[64] = tt_Dynamic("Use this text field to type in the name of the planet or player you wish to search for in the listing");
	alltt[70] = tt_Dynamic("Click here to order the list according to this field and switch between ascending and descending ordering");
	alltt[71] = tt_Dynamic("Click here to go to this individual planet page");
	alltt[72] = tt_Dynamic("Click here to send a private message to the owner of this planet");
	alltt[73] = tt_Dynamic("Use this drop down list to go to another page of the listing");
	alltt[80] = tt_Dynamic("Use this drop down list to select the number of alliance members to display oneach page of the listing");
	alltt[81] = tt_Dynamic("Use this text field to type in the name of the player you wish to search for in the listing");
	alltt[90] = tt_Dynamic("Click here to kick this member out of the alliance");
	alltt[91] = tt_Dynamic("Click here to change the rank of the selected members to the selected rank");
	alltt[92] = tt_Dynamic("Use this drop down list to choose the rank you want to give to the selected members");
	alltt[100] = tt_Dynamic("Check this checkbox to select this alliance member ");
	alltt[101] = tt_Dynamic("Click here to send a private message to this alliance member");
	alltt[110] = tt_Dynamic("Check this checkbox to select this player requesting to join the alliance");
	alltt[111] = tt_Dynamic("Click here to accept this player into the alliance");
	alltt[112] = tt_Dynamic("Click here to reject this player's joining request");
	alltt[120] = tt_Dynamic("Click here to go to the forum creation form and create a new forum");
	alltt[130] = tt_Dynamic("Click here to go to the forum modification form and edit this forum's settings");
	alltt[131] = tt_Dynamic("Click here to move this forum up in the forums list");
	alltt[132] = tt_Dynamic("Click here to move this forum down in the forums list");
	alltt[133] = tt_Dynamic("Click here to delete this forum and all its contents");
	alltt[140] = tt_Dynamic("Use this text field to type in the name of the forum");
	alltt[141] = tt_Dynamic("Use this radio button if you want everyone to be able to post new threads in this forum");
	alltt[142] = tt_Dynamic("Use this radio button if you want the moderators of the forum to be the only ones allowed to post new threads in it");
	alltt[143] = tt_Dynamic("Use this drop down list to select where to place this forum in the forums list");
	alltt[144] = tt_Dynamic("Use this text area to type in a description for this forum");
	alltt[145] = tt_Dynamic("Click here to validate your changes to this forum");
	alltt[146] = tt_Dynamic("Click here to cancel your changes to this forum");
	alltt[150] = tt_Dynamic("Check this checkbox to select this rank and modify its access level to this forum");
	alltt[160] = tt_Dynamic("Click here to give this access level to the selected rank(s) for this forum");
	alltt[170] = tt_Dynamic("Click here to go to the rank creation form and create a new rank");
	alltt[171] = tt_Dynamic("Click here to display / hide the dtails about this rank");
	alltt[172] = tt_Dynamic("Click here to go to the modification form and edit this rank");
	alltt[173] = tt_Dynamic("Click to delete this rank");
	alltt[180] = tt_Dynamic("Use this text field to type in the rank designation");
	alltt[181] = tt_Dynamic("Use this drop down list to select the level of listing access the members with this rank will have");
	alltt[182] = tt_Dynamic("Check this checkbox if you want members of this rank to see the list of planets under attack");
	alltt[183] = tt_Dynamic("Use this radio button if you want members of this rank to be diplomatic contact for the alliance and to receive all messages sent to the alliance");
	alltt[184] = tt_Dynamic("Use this radio button if you don't want members of this rank to be diplomatic contact for the alliance");
	alltt[185] = tt_Dynamic("Use this radio button if you want members of this rank to be able to vote for leader elections if the alliance is democratic");
	alltt[186] = tt_Dynamic("Use this radio button if you don't want members of this rank to be able to vote for leader elections if the alliance is democratic");
	alltt[187] = tt_Dynamic("Use this radio button if you want members of this rank to be able to apply for presidency");
	alltt[188] = tt_Dynamic("Use this radio button if you don't want members of this rank to be able to apply for presidency");
	alltt[189] = tt_Dynamic("Use this radio button if you want members of this rank to be able to accept or reject new joining requests");
	alltt[190] = tt_Dynamic("Use this radio button if you don't want members of this rank to be able to accept or reject new joining requests");
	alltt[191] = tt_Dynamic("Use this radio button if you want members of this rank to be forum administrators");
	alltt[192] = tt_Dynamic("Use this radio button if you don't want members of this rank to be forum administrators");
	alltt[193] = tt_Dynamic("Click here to accept the changes you've made to that rank");
	alltt[194] = tt_Dynamic("Click here to cancel the changes you've made to that rank and go back to the previous page");
	alltt[195] = tt_Dynamic("Use this radio button if you don't want members of this rank to be able to kick any member out of the alliance");
	alltt[196] = tt_Dynamic("Use this radio button if you want members of this rank to be able to kick any member out of the alliance");
	alltt[197] = tt_Dynamic("Use this radio button if you want members of this rank to be able to kick only members of a specific rank out of the alliance");
	alltt[198] = tt_Dynamic("Use this radio button if you don't want members of this rank to be able to change the rank of any other member of the alliance");
	alltt[199] = tt_Dynamic("Use this radio button if you want members of this rank to be able to change the rank of any other member of the alliance");
	alltt[200] = tt_Dynamic("Use this radio button if you want members of this rank to be able tochange the rank of other members of the alliance only if they have some specific rank");
	alltt[201] = tt_Dynamic("Use this radio button if you don't want members of this rank to have any access to this forum");
	alltt[202] = tt_Dynamic("Use this radio button if you want members of this rank to have access to this forum as normal users");
	alltt[203] = tt_Dynamic("Use this radio button if you want members of this rank to have access to this forum as moderators");
	alltt[204] = tt_Dynamic("Use this drop down list to select to which rank players of the rank you are about to delete are to be demoted to");
	alltt[205] = tt_Dynamic("Click here to confirm the deletion of this rank and the demotion of the players of this rank to the selected rank");
	alltt[206] = tt_Dynamic("Click here to cancel the deletion of this rank and go back to the previous page");
	alltt[210] = tt_Dynamic("Click here to go to this section of the alliance page");
	alltt[220] = tt_Dynamic("Check this checkbox to select this rang and provide the corresponding rights over the members of this rank to the members of rank you are editing");
}



//--------------------------------------------------
// PAGE WHEN PLAYER IS NOT A MEMBER OF AN ALLIANCE
//--------------------------------------------------

function	drawPageNoAlliance()
{
	var	jd, cd1, cd2, ai;
	var	str = '<form action="?"><table cellspacing="0" cellpadding="0"><tr>';

	// Build join/create pannel
	str += '<td class="div2"><h1>Join an alliance</h1>';
	if	(plAlliance == "")
	{
		str += '<table class="calliance"><tr><td class="brd" rowspan="2">&nbsp;</td>';
		str += '<th>Alliance tag:</th><td>';
		str += '<input type="text" ' + alltt[0] + ' size="6" maxlength="5" name="aljoin" id="aljoin" value="" onFocus="hasFocus=0;return true" /></td></tr>';
		str += '<tr><td colspan="2" class="but">';
		str += '<input type="button" ' + alltt[1] + ' value="Send request" onClick="sendJoinRequest(); return false" />';
		str += '</td></tr></table>';

		str += '<p>&nbsp;</p>';

		str += '<h1>Create a new alliance</h1>';
		str += '<table class="calliance"><tr><td class="brd" rowspan="4">&nbsp;</td><th>';
		str += 'Alliance tag:</th><td><input ' + alltt[2] + ' type="text" size="6" maxlength="5" name="actag" id="actag" value="" onFocus="hasFocus=1;return true" />';
		str += '</td><td>&nbsp;</td></tr>';
		str += '<tr><th>Alliance name:</th><td colspan="2">';
		str += '<input ' + alltt[3] + ' type="text" size="32" maxlength="64" name="acname" id="acname" value="" onFocus="hasFocus=2;return true" />';
		str += '</td></tr>';
		str += '<tr><td colspan="2" class="but">';
		str += '<input type="button" ' + alltt[4] + ' value="Create this alliance" onClick="createAlliance(); return false" /></td>';
		str += '<td>&nbsp;</td></tr></table>';
	}
	else
	{
		str += '<p>Request sent to ' + alName + ' <b>[' + alTag + ']</b> ';
		str += '<input type="button" ' + alltt[5] + ' value="Cancel request" onClick="x_cancelRequest(requestSent); return false" /></p>';
	}

	// Build alliance info pannel
	str += '</td><td style="width:45%">';
	str += drawAllianceInfoPannel();
	str += '</td><td style="width:5%;vertical-align:top"><a href="manual?p=alliance">Help</a></td></tr></table>';

	var	i, e, v = new Array();
	var	a = new Array('aljoin', 'actag', 'acname', 'alinfo');
	for	(i=0;i<a.length;i++)
	{
		e = document.getElementById(a[i]);
		if	(!e)
			v[i] = '';
		else
			v[i] = e.value;
	}

	document.getElementById('allpage').innerHTML = str;

	for	(i=0;i<a.length;i++)
	{
		e = document.getElementById(a[i]);
		if	(!e)
			continue;
		e.value = v[i];
		if	(hasFocus == i)
			e.focus();
	}

	drawPubAllianceInfo();
}

function alertCreate(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += "Multiple spaces are not allowed in alliance tags."; break;
		case 1: str += "Please enter a tag for the alliance to create."; break;
		case 2: str += "Please enter a name for the new alliance."; break;
		case 3: str += "Tag or name too long."; break;
		case 4: str += "This alliance tag already exists."; break;
		case 5: str += "A database error occured on the server."; break;
		case 6: str += "You can't create alliances in this game."; break;
		case 200: str += "You can\'t create an alliance while in vacation mode."; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}

function alertJoin(ei) {
	var	str = 'Error\n';
	switch (ei) {
		case 0: str += "Please enter the tag of the alliance to send a request to."; break;
		case 1: str += "The alliance could not be found."; break;
		case 2: str += "You have already requested to a the member of an alliance."; break;
		case 3: str += "You can't join alliances in this game."; break;
		case 4: str += "You have already canceled your request to join this alliance."; break;
		case 5: str += "You shouldn't be here - this game has locked allianced."; break;
		case 200: str += "You can\'t join an alliance while in vacation mode."; break;
		case 201: str += "You can\'t cancel a request to join an alliance while in vacation mode."; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}


//--------------------------------------------------
// ALLIANCE INFORMATION PANNEL
//--------------------------------------------------

function	drawAllianceInfoPannel()
{
	var	str = '<h1>Alliance information</h1>';
	str += '<table class="calliance"><tr><td class="brd" rowspan="11">&nbsp;</td>';
	str += '<th>Alliance tag:</th><td>';
	str += '<input type="text" ' + alltt[10] + ' size="6" maxlength="5" name="alinfo" id="alinfo" value="" onFocus="hasFocus=3;return true" /></td></tr>';
	str += '<tr><td colspan="2" class="but">';
	str += '<input type="button" ' + alltt[11] + ' value="Display information" onClick="getAllianceInfo(); return false" />';
	str += '</td></tr><tr><td colspan="2">&nbsp;</td></tr>';
	var	i, a = new Array('tag', 'name', 'ldname', 'rank', 'points', 'nplanets', 'coords');
	for	(i=0;i<a.length;i++)
		str += '<tr><th id="aihd' + a[i] + '">&nbsp;</th><td id="aidt' + a[i] + '">&nbsp;</td></tr>';
	str += '<tr><th id="aihdvict">&nbsp;</th><td id="aidtvict">&nbsp;</td></tr>';
	str += '<tr><td colspan="2" class="but" id="ainfmsg">&nbsp;</td></tr>';
	str += '</table>';
	return	str;
}

function	drawPubAllianceInfo()
{
	if	(alPubInfo && typeof alPubInfo == 'object')
	{
		document.getElementById('aihdtag').innerHTML = 'Alliance tag:';
		document.getElementById('aihdname').innerHTML = 'Alliance name:';
		document.getElementById('aihdldname').innerHTML = 'Leader:';
		document.getElementById('aihdrank').innerHTML = 'Rank:';
		document.getElementById('aihdpoints').innerHTML = 'Points:';
		document.getElementById('aihdnplanets').innerHTML = 'Planets:';
		document.getElementById('aihdcoords').innerHTML = 'Avg. coordinates:';
		document.getElementById('aidttag').innerHTML = '[' + alPubInfo.tag + ']';
		document.getElementById('aidtname').innerHTML = alPubInfo.name;
		document.getElementById('aidtldname').innerHTML = (alPubInfo.leader == "" ? '<span class="noleader">NONE!</span>' : alPubInfo.leader);
		document.getElementById('aidtrank').innerHTML = '#' + formatNumber(alPubInfo.rank);
		document.getElementById('aidtpoints').innerHTML = formatNumber(alPubInfo.points);
		document.getElementById('aidtnplanets').innerHTML = formatNumber(alPubInfo.nplanets);
		if	(alPubInfo.nplanets > 0)
			document.getElementById('aidtcoords').innerHTML = '(' + alPubInfo.cx + ',' + alPubInfo.cy + ')';
		else
			document.getElementById('aidtcoords').innerHTML = 'N/A';
		if (alPubInfo.victory != '') {
			document.getElementById('aihdvict').innerHTML = 'Victory:';
			document.getElementById('aidtvict').innerHTML = alPubInfo.victory + '%';
		}
	}
	else
	{
		var	i, a = new Array('tag', 'name', 'ldname', 'rank', 'points', 'nplanets', 'coords');
		for	(i=0;i<a.length;i++)
		{
			document.getElementById('aihd' + a[i]).innerHTML = '';
			document.getElementById('aidt' + a[i]).innerHTML = '';
		}
		document.getElementById('ainfmsg').innerHTML = '';
	}
}

function	alertAllianceInfo(ei)
{
	var	str = 'Error\n';
	switch	(ei)
	{
		case 0: str += 'Please enter a tag to look up.'; break;
		case 1: str += 'This alliance does not exist.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}


//--------------------------------------------------
// ALLIANCE MEMBER PAGE
//--------------------------------------------------

function	drawLoadingText()
{
	document.getElementById('alpmain').innerHTML = 'Loading page, please wait ...';
}

function drawMainPage() {
	var	jd, cd1, cd2, ai;
	var	str = '<table cellspacing="0" cellpadding="0"><tr>';
	asButtons = false;

	// Build current alliance information
	str += '<td class="div2"><h1>Current alliance</h1>';
	var nsl = 13 + ((successor != "") ? 1 : 0) + (myPubInfo.victory != '' ? 1 : 0);
	str += '<table class="calliance"><tr><td class="brd" rowspan="' + nsl + '">&nbsp;</td>';
	str += '<td colspan="2">You are ';
	if (alPrivileges[8] == "1") {
		str += 'the leader';
	} else {
		str += 'a member';
	}
	str += ' of <b>[' + myPubInfo.tag + ']</b>.</td></tr>';
	if (alPrivileges[8] == 1 && successor != "") {
		str += '<tr><td colspan="2" class="but"><input ' + alltt[20]
			+ ' type="button" value="Step down" onClick="leaderStepDown();return false" /></td></tr>';
	}
	str += '<tr><td colspan="2">&nbsp;</td></tr>';
	str += '<tr><th>Alliance tag:</th><td>[' + myPubInfo.tag + ']</td></tr>';
	str += '<tr><th>Alliance name:</th><td>' + myPubInfo.name + '</td></tr>';
	// FIXME: message link if not leader
	str += '<tr><th>Leader:</th><td' + (myPubInfo.leader == "" ? ' class="noleader">NONE!' : ('>' + myPubInfo.leader))
		+ '</td></tr>';
	if (successor != "") {
		str += '<tr><th>Successor:</th><td>' + successor + '</td></tr>';
	}
	str += '<tr><th>Government:</th><td>' + (isDemocracy ? 'democratic' : 'dictatorial')
		+ '</td></tr><tr><th>Tech. trading:</th><td>';
	if (techTrade == 'N') {
		str += 'disabled';
	} else {
		str += '<a href="techtrade">enabled</a>'
			+ (techTrade == 'R' ? '' : ' (no requests)');
	}
	str += '</td></tr>';
	str += '<tr><th>Rank:</th><td>#'+formatNumber(myPubInfo.rank)+'</td></tr>';
	str += '<tr><th>Points:</th><td>'+formatNumber(myPubInfo.points)+'</td></tr>';
	str += '<tr><th>Planets:</th><td>' + formatNumber(myPubInfo.nplanets) + '</td></tr>';
	str += '<tr><th>Avg. coordinates:</th><td>';
	if	(myPubInfo.nplanets > 0)
		str += '(' + myPubInfo.cx + ',' + myPubInfo.cy + ')';
	else
		str += 'N/A';
	str += '</td></tr>';

	if (myPubInfo.victory != '') {
		str += '<tr><th>Victory:</th><td>' + myPubInfo.victory + '%</td>';
	}

	if (!lockedAlliances) {
		str += '<tr><td colspan="2" class="but"><input type="button" ' + alltt[21] + ' value="Leave the alliance" onClick="leaveAlliance();return false" /></td></tr>';
	}
	str += '</table></td><td>';

	// Alliance settings
	if (alPrivileges[8] == "1") {
		if (typeof asDemocracy != "boolean") {
			asDemocracy = isDemocracy;
			asSuccessor = successor;
			asTechTrade = techTrade;
		}
		str += '<h1>General settings</h1>';
		str += '<table class="calliance"><tr><td class="brd" rowspan="5">&nbsp;</td>';
		if (!lockedAlliances) {
			str += '<th>Government:</th><td><label><input type="radio" ' + alltt[22]
				+ ' name="lasmode" value="1" ' + (asDemocracy ? '' : 'checked="checked"')
				+ ' onClick="asDemocracy=false;updateASControls();return true"/> Dictatorial</label>'
				+ '</td></tr><tr><td>&nbsp;</td><td><label><input type="radio" ' + alltt[23]
				+ ' name="lasmode" value="1" ' + (asDemocracy ? 'checked="checked"' : '')
				+ ' onClick="asDemocracy=true;updateASControls();return true"/> Democratic'
				+ '</label></td></tr>';
		}
		str += '<tr><th>Successor:</th><td><input ' + alltt[24]
			+ ' type="text" name="lassucc" id="lassucc" size="16" maxlength="15" value=""'
			+ ' onKeyUp="asSuccessor=value;updateASControls();return true" onFocus="hasFocus=0;'
			+ 'return true"/></td></tr><tr><th><label for="ttrade">Tech. trading:</label></th>'
			+ '<td><select name="ttrade" id="ttrade" onchange="'
			+ 'asTechTrade=this.options[this.selectedIndex].value;updateASControls();return true">'
			+ '<option value="N"' + (asTechTrade == 'N' ? ' selected="selected"' : '')
			+ '>disabled</option><option value="S"'	+ (asTechTrade == 'S' ? ' selected="selected"' : '')
			+ '>enabled (no requests)</option><option value="R"'
			+ (asTechTrade == 'R' ? ' selected="selected"' : '')
			+ '>enabled (with requests)</option></select></td></tr>'
			+ '<tr><td colspan="2" class="but" id="lasbut">&nbsp;</td></tr>'
			+ '<tr><td colspan="3">&nbsp;</td></tr></table>';
	}

	// Build alliance info pannel
	str += drawAllianceInfoPannel();
	str += '</td></tr></table>';

	var	e = document.getElementById('alinfo'), es;
	if	(e)
		es = e.value;
	else
		es = "";

	document.getElementById('alpmain').innerHTML = str;
	drawPubAllianceInfo();

	if	(alPrivileges[8] == "1")
	{
		document.getElementById('lassucc').value = asSuccessor;
		updateASControls();
	}
	document.getElementById('alinfo').value = es;

	asFocus();
}

function	drawASButtons()
{
	var	str = '<input type="button" ' + alltt[30] + ' name="lasbut1" value="Update" onClick="modifyAllianceSettings();return false" /> ';
	str += '<input type="button" ' + alltt[31] + ' name="lasbut2" value="Reset" onClick="resetAllianceSettings();return false" />';
	document.getElementById('lasbut').innerHTML = str;
}

function alertASettings(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += "You are no longer the leader of this alliance."; break;
		case 1: str += 'The player you selected to be your successor could\nnot be found.'; break;
		case 2: str += 'You can\'t name yourself as your own successor.'; break;
		case 3: str += 'The player you selected to be your successor is not\na member of the alliance.'; break;
		case 4: str += "You can't leave the alliance in this game."; break;
		case 200: str += 'You are not allowed to do that while in vacation mode.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}

function	confirmLSD()
{
	return	confirm(
		'You are about to step down from the head of the\n[' + alTag
		+ '] alliance.\nThis will make ' + successor + ', your successor, the new leader.\n'
		+ 'Please confirm you really want to do this.'
	);
}

function	confirmLeave(lt)
{
	var	str = 'You are about to leave the [' + alTag + '] alliance.\n';
	switch	(lt)
	{
		case 1: str += 'The successor you designated, ' + successor + ', will take your place\nas the head of the alliance.\n'; break;
		case 2: str += 'The alliance will be become leader-less until someone is\nelected to replace you at the head of the alliance.\n'; break;
		case 3: str += 'The alliance will be become a leader-less democracy until\nsomeone is elected to replace you at the head of the alliance.\n'; break;
	}
	str += 'Please confirm.';
	return	confirm(str);
}


//--------------------------------------------------
// LEADER ELECTION
//--------------------------------------------------

function	drawElectionLayout()
{
	var	str = '<h1>Alliance Leader Election</h1>';
	str += '<table cellspacing="0" cellpadding="0">';
	str += '<tr><td class="div2"><h2>Candidates</h2><div id="lecdlist">&nbsp;</div></td>';
	str += '<td class="div2"><h2>Actions</h2><div id="leaclist">&nbsp;</div></td></tr>';
	str += '</table>';
	document.getElementById('alpmain').innerHTML = str;
}

function	drawElectionButtons()
{
	var	str = '<p>';
	if	(leCanSel||leCanTake||leCanApply||leCanCancel)
	{
		if	(leCanTake)
			str += '<input  ' + alltt[40] + ' type="button" name="leTake" value="Take Presidency" onClick="takePresidency();return false" /><br/>';
		if	(leCanApply)
			str += '<input ' + alltt[41] + ' type="button" name="leApply" value="Apply for Presidency" onClick="applyForPresidency();return false" /><br/>';
		else if	(leCanCancel)
			str += '<input ' + alltt[42] + ' type="button" name="leTake" value="Cancel Candidacy" onClick="cancelCandidacy();return false" /><br/>';
		if	(leCanSel && (leNewSel != leSelected))
			str += '<input type="button" ' + alltt[43] + ' name="leChSel" value="Change my Vote" onClick="changeVote();return false" />';
	}
	else
	{
		str += 'There is nothing you can do here, since ';
		if	(alPrivileges[8] == 1)
			str += 'you are the alliance\'s leader.';
		else
			str += 'you don\'t have the right to vote or to apply for presidency.';
	}
	str += '</p>';
	document.getElementById('leaclist').innerHTML = str;
}


function	drawCandidateList()
{
	var	str;

	if	(leCandidates.length > 0)
	{
		str = '<table class="list" id="candlist" cellspacing="0" cellpadding="0"><tr>';

		// Headers
		if	(leCanSel)
			str += '<td class="lesel">&nbsp;</td>';
		str += '<th class="lecname" ' + alltt[50] + ' onClick="leSort=0;leSortDir=!leSortDir;updateCandidateList()" ';
		str += '>' + (leSort == 0 ? '<b>' : '') + 'Name';
		if	(leSort == 0)
		{
			str += '</b><img src="'+staticurl+'/beta5/pics/';
			str += leSortDir ? "up" : "down";
			str += '_' + color + '.gif" alt="' + (leSortDir ? "up" : "down") + '" />';
		}
		str += '</th><th class="lecvotes" ' + alltt[50] + ' onClick="leSort=1;leSortDir=!leSortDir;updateCandidateList()" ';
		str += '>' + (leSort == 1 ? '<b>' : '') + 'Votes';
		if	(leSort == 1)
		{
			str += '</b><img src="'+staticurl+'/beta5/pics/';
			str += leSortDir ? "up" : "down";
			str += '_' + color + '.gif" alt="' + (leSortDir ? "up" : "down") + '" />';
		}
		str += '</th></tr>';

		if	(leCanSel)
		{
			str += '<tr onClick="leNewSel=\'\';document.getElementById(\'lesela\').checked=true;drawElectionButtons()">';
			str += '<td class="lesel"><input type="radio"  ' + alltt[51] + ' name="lecsel" onClick="leNewSel=\'\';drawElectionButtons()" id="lesela" value=""';
			if	(leNewSel == '')
				str += 'checked="checked" ';
			str += ' /></td><td colspan="2"><i>Abstain</i></td></tr>';
		}

		// List
		for	(i=0;i<leCandidates.length;i++)
		{
			str += '<tr';
			if	(leCanSel)
			{
				str += ' onClick="leNewSel=\'' + leCandidates[i].id + '\';document.getElementById(\'lesel'+i+'\').checked=true;';
				str += 'drawElectionButtons()">';
				str += '<td class="lesel"><input ' + alltt[52] + ' type="radio" name="lecsel" onClick="leNewSel=\''+leCandidates[i].id;
				str += '\';drawElectionButtons()" id="lesel'+i+'" value="'+i+'" ';
				if	(leNewSel == leCandidates[i].id)
					str += 'checked="checked" ';
				str += '/></td';
			}
			str += '><td class="lecname">' + leCandidates[i].name + '</td><td class="lecvotes">' + leCandidates[i].nVotes + '</td>';
			str += '</tr>';
		}

		str += '</table>';
	}
	else
		str = '<p>Noone has applied for presidency yet.</p>';
	document.getElementById('lecdlist').innerHTML = str;
}

function alertElection(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += 'You are no longer a member of the alliance.'; break;
		case 1: str += 'You no longer have the authorization to do that.'; break;
		case 2: str += "The player you voted for is no longer a candidate."; break
		case 3: str += "You are already a candidate for presidency."; break
		case 4: str += "You aren\'t a candidate for presidency anymore."; break
		case 5: str += "You missed the occasion to seize control of the alliance."; break;
		case 200: str += 'You are not allowed to use this page while in vacation mode.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}


//--------------------------------------------------
// LISTINGS PAGE
//--------------------------------------------------


function	drawPlanetList()
{
	var	i, str, n, m;

	// Header
	str = '<h1>Alliance planets</h1>';

	// List header with page selector and search bar
	str += '<table cellspacing="0" cellpadding="0"><tr>';
	str += '<td class="lspsel1">Display <select ' + alltt[61] + ' name="lspspp" onChange="lsPPerPage=parseInt(options[selectedIndex].value, 10);drawPlanetList()">';
	for	(i=1;i<6;i++)
		str += '<option' + (i*5 == lsPPerPage ? ' selected="selected"' : '') + ' value="'+(i*5)+'">' + (i*5) + '</option>';
	str += '</select> planets / page</td><td class="lspsel1" id="lstpage">';
	str += '</td></tr><tr><td colspan="2" class="lspsearch">';
	str += 'Search for <label><input type="radio" ' + alltt[62] + ' + name="plstype" value="0" onClick="lsPSearchType=0;updateSearchPlanet()"';
	if	(lsPSearchType == 0)
		str += ' checked="checked"';
	str += ' /> planet</label> or <label><input type="radio" ' + alltt[63] + ' name="plstype" value="1" onClick="lsPSearchType=1;updateSearchPlanet()"';
	if	(lsPSearchType == 1)
		str += ' checked="checked"';
	str += ' /> player</label> <input type="text"  ' + alltt[64] + ' name="plsname" value="" onKeyUp="lsPSearchText=value;updateSearchPlanet()" ';
	str += 'id="plsname"></td></tr></table><hr/><div id="lstcontents">&nbsp;</div>';

	document.getElementById('alpmain').innerHTML = str;
	document.getElementById('plsname').value = lsPSearchText;

	drawPlanetListContents();
}

function	drawPlanetListContents()
{
	var	str, i, n, m;

	// Page handling
	n = lsPIndices.length;
	m = n % lsPPerPage;
	n = (n-m)/lsPPerPage + (m>0?1:0);
	if	(n > 0 && lsPPage >= n)
		lsPPage = n-1;

	// Planet list
	if	(lsPIndices.length == 0)
		str = '<center>&nbsp;<br/>No planets were found.<br/>&nbsp;</center>';
	else
	{
		str = '<table id="plnlist" class="list" cellspacing="0" cellpadding="0"><tr>';
		var	hdNames = ['Coordinates', 'Planet', 'Owner', 'Factories', 'Turrets'];
		var	hdCols = ['class="pcoord"', 'class="pname" colspan="2"', 'class="oname"', 'class="pcoord"', 'class="pcoord"'];
		var	ll = (alPrivileges[0] == 3) ? 5 : 3;
		for	(i=0;i<ll;i++)
		{
			str += '<th ' + alltt[70] + ' onClick="lsPSort=' + i + ';lsPSortDir=!lsPSortDir;updateSearchPlanet()" ';
			str += hdCols[i] + '>' + (lsPSort == i ? '<b>' : '') + hdNames[i];
			if	(lsPSort == i)
			{
				str += '</b><img src="'+staticurl+'/beta5/pics/';
				str += lsPSortDir ? "up" : "down";
				str += '_' + color + '.gif" alt="' + (lsPSortDir ? "up" : "down") + '" />';
			}
			str += '</th>';
		}
		str += '<td>&nbsp;</td></tr>';

		for	(i=lsPPerPage*lsPPage;i<lsPIndices.length&&i<lsPPerPage*(lsPPage+1);i++)
		{
			var	idx = lsPIndices[i];
			str += '<tr';
			if	(lsPlanets[idx].underAttack == 1)
				str += ' class="attack"';
			str += '><td class="pcoord">' + lsPlanets[idx].coords + '</td>';
			str += '<td class="picon"><img src="'+staticurl+'/beta5/pics/pl/s/' + lsPlanets[idx].id + '.png" alt="[' + lsPlanets[idx].id + ']" /></td>';
			str += '<td><a  ' + alltt[71] + ' href="planet?id=' + lsPlanets[idx].id + '">' + lsPlanets[idx].name + '</a></td>';
			str += '<td>' + (lsPlanets[idx].msOwner != "" ? ('<a ' + alltt[72] + ' href="message?a=c&ct=0&id='+lsPlanets[idx].msOwner+'">') : '');
			str += lsPlanets[idx].owner + (lsPlanets[idx].msOwner != "" ? '</a>' : '') + '</td>';
			if	(alPrivileges[0] == 3)
			{
				str += '<td class="pcoord">' + formatNumber(lsPlanets[idx].factories) + '</td>';
				str += '<td class="pcoord">' + formatNumber(lsPlanets[idx].turrets) + '</td>';
			}
		}
		str += '</table>';
	}

	document.getElementById('lstcontents').innerHTML = str;

	// Page selector
	str ='Page ';
	if	(n <= 1)
		str += '1 / 1';
	else
	{
		str += '<select name="lspnp"  ' + alltt[73] + ' onChange="lsPPage=parseInt(this.options[this.selectedIndex].value, 10);updateSearchPlanet()">';
		for	(i=0;i<n;i++)
			str += '<option' + (i==lsPPage ? ' selected="selected"' : '') + ' value="'+i+'">' + (i+1) + '</option>';
		str += '</select> / ' + n;
	}
	document.getElementById('lstpage').innerHTML = str;
}

function drawMemberList() {
	var	i, str, n, m;

	// Header
	str = '<h1>Alliance members</h1>';

	// List header with page selector and search bar
	str += '<table cellspacing="0" cellpadding="0"><tr>';
	str += '<td class="lspsel1">Display <select  ' + alltt[80] + ' name="lspspp" onChange="lsMPerPage=parseInt(this.options[this.selectedIndex].value,10);drawMemberList()">';
	for	(i=1;i<6;i++)
		str += '<option' + (i*5 == lsMPerPage ? ' selected="selected"' : '') + ' value="' + (i*5) + '">' + (i*5) + '</option>';
	str += '</select> members / page</td><td class="lspsel1" id="lstpage">';
	str += '</td></tr><tr><td colspan="2" class="lspsearch">';
	str += 'Search for player <input ' + alltt[81] + ' type="text" name="mlsname" value="" onKeyUp="lsMSearchText=value;updateSearchMembers()" ';
	str += 'id="mlsname"></td></tr></table><hr/><div id="lstcontents">&nbsp;</div><div id="lstcontrols">&nbsp;</div>';

	document.getElementById('alpmain').innerHTML = str;
	document.getElementById('mlsname').value = lsMSearchText;

	drawMemberListContents(false);
}

function drawMemberListContents(ao) {
	var	str = "", i, nPages, m;
	var	csel, kr, cr;

	// Page handling
	nPages = lsMIndices.length;
	m = nPages % lsMPerPage;
	nPages = (nPages - m) / lsMPerPage + (m > 0 ? 1 : 0);
	if (nPages > 0 && lsMPage >= nPages) {
		lsMPage = nPages - 1;
	}

	// Kick/Change rank
	csel = (lsMChange.length > 0 || lsMKick.length > 0);
	kr = '#' + lsMKick.join('#') + '#';
	cr = '#' + lsMChange.join('#') + '#';

	if (!ao) {
		// Page selector
		str ='Page ';
		if (nPages <= 1) {
			str += '1 / 1';
		} else {
			str += '<select ' + alltt[73] + ' name="lsmpbp" onChange="lsMPage=parseInt(this.options[this.selectedIndex].value, 10);updateSearchMembers()">';
			for (i = 0; i < nPages; i ++) {
				str += '<option' + (i == lsMPage ? ' selected="selected"' : '')
					+ ' value="' + i + '">' + (i + 1) + '</option>';
			}
			str += '</select> / ' + nPages;
		}
		document.getElementById('lstpage').innerHTML = str;
	}

	if (! ao) {
		if (nPages == 0 && !ao) {
			str  = '<center>&nbsp;<br/>No members were found.<br/>&nbsp;</center>';
		} else {
			// List length
			var lp;
			if (lsMPage == nPages - 1 && m > 0) {
				lp = m;
			} else {
				lp = lsMPerPage;
			}

			str = drawMemberListPanel(lsMPage * lsMPerPage, lp, csel, kr, cr)
		}
		document.getElementById('lstcontents').innerHTML = str;
	}

	if (!csel) {
		document.getElementById('lstcontrols').innerHTML = '&nbsp;';
		return;
	}

	// Action panel
	var sel = new Array(), ck = !lockedAlliances, cc = true;
	nPages = Math.min((lsMPage + 1) * lsMPerPage, lsMIndices.length);
	for (i = lsMPage * lsMPerPage; i < nPages; i ++) {
		if (!lsMembers[lsMIndices[i]].selected) {
			continue;
		}
		sel.push(lsMIndices[i]);
		ck = ck && (kr.indexOf('#'+lsMembers[lsMIndices[i]].rank+'#') != -1);
		cc = cc && (cr.indexOf('#'+lsMembers[lsMIndices[i]].rank+'#') != -1);
	}
	if (sel.length==0||!(ck||cc)) {
		document.getElementById('lstcontrols').innerHTML = '&nbsp;';
		return;
	}
	if (typeof lsMNewRank == "boolean") {
		lsMNewRank = lsMChange[0];
	}
	str = '<hr/><center>';
	if (!cc && ck) {
		str += '<input type="button" ' + alltt[90] + ' name="ksel" value="Kick" onClick="kickMembers();return false" /> selected members.';
	} else if (!ck && cc) {
		str += '<input type="button" ' + alltt[91] + ' name="csel" value="Change" onClick="changeMembers();return false" /> selected members\' ranks to ';
		str += '<select ' + alltt[92] + ' name="nrsel" onChange="lsMNewRank=options[selectedIndex].value">';
		for	(i=0;i<lsMChange.length;i++)
		{
			str += '<option value="' + lsMChange[i] + '"';
			if	(lsMNewRank == lsMChange[i])
				str += ' selected="selected"';
			str += '>' + getCookedRankText(lsMChange[i]) + '</option>';
		}
		str += '</select>.';
	} else {
		str += '<input type="button" ' + alltt[90] + ' name="ksel" value="Kick" onClick="kickMembers();return false" /> selected members or ';
		str += '<input type="button" ' + alltt[91] + ' name="csel" value="change" onClick="changeMembers();return false" /> their ranks to ';
		str += '<select ' + alltt[92] + ' name="nrsel" onChange="lsMNewRank=options[selectedIndex].value">';
		for (i=0;i<lsMChange.length;i++) {
			str += '<option value="' + lsMChange[i] + '"';
			if	(lsMNewRank == lsMChange[i])
				str += ' selected="selected"';
			str += '>' + getCookedRankText(lsMChange[i]) + '</option>';
		}
		str += '</select>.';
	}
	str += '</center>';
	document.getElementById('lstcontrols').innerHTML = str;
}

function getRankText(r) {
	if (r == "+") {
		return "(Leader)";
	} else if (r == '-') {
		return "<i>(Standard member)</i>";
	}
	return lsMRanks[r].name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

function	confirmChangeRank(players)
{
	var	i, str = 'You are about to modify the following player';
	var	names = new Array();
	i = getCookedRankText(lsMNewRank).replace(/<[^>]+>/g, '');

	str += (players.length > 1 ? 's\'' : '\'s') + ' rank to ' + i + ':\n - ';
	for	(i=0;i<players.length;i++)
		names.push(lsMembers[players[i]].name);
	str += names.join('\n - ') + '\nPlease confirm.';
	return	confirm(str);
}

function	confirmKick(players)
{
	var	i, str = 'You are about to kick the following member';
	var	names = new Array();

	str += (players.length > 1 ? 's' : '') + ' from the alliance:\n - ';
	for	(i=0;i<players.length;i++)
		names.push(lsMembers[players[i]].name);
	str += names.join('\n - ') + '\nPlease confirm.';
	return	confirm(str);
}

function alertMemberAction(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += 'You are no longer a member of the alliance.'; break;
		case 1: str += 'You no longer have the authorization to manage members.'; break;
		case 2: str += "Impossible to kick players in this game, the alliances are locked."; break;
		case 200: str += 'You are not allowed to manage members while in vacation mode.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}

function drawMilitaryList() {
	var	i, str, n, m;

	// Header
	str = '<h1>Military status</h1>';

	// List header with page selector and search bar
	str += '<table cellspacing="0" cellpadding="0"><tr>';
	str += '<td class="lspsel1">Display <select ' + alltt[61] + ' name="lsaspp" onChange="lsAPerPage=parseInt(options[selectedIndex].value, 10);drawMilitaryList()">';
	for	(i=1;i<6;i++)
		str += '<option' + (i*5 == lsAPerPage ? ' selected="selected"' : '') + ' value="'+(i*5)+'">' + (i*5) + '</option>';
	str += '</select> planets / page</td><td class="lspsel1" id="lstpage">';
	str += '</td></tr><tr><td colspan="2" class="lspsearch">';
	str += 'Search for ';
	if	(alPrivileges[0] > 1)
	{
		str += '<label><input ' + alltt[62] + ' type="radio" name="alstype" value="0" onClick="lsASearchType=0;updateSearchMilitary()"';
		if	(lsASearchType == 0)
			str += ' checked="checked"';
		str += ' /> planet</label> or <label><input ' + alltt[63] + ' type="radio" name="alstype" value="1" onClick="lsASearchType=1;updateSearchMilitary()"';
		if	(lsASearchType == 1)
			str += ' checked="checked"';
		str += ' /> player</label>';
	}
	else
		str += 'planet';
	str += ' <input type="text" ' + alltt[64] + ' name="alsname" value="" onKeyUp="lsASearchText=value;updateSearchMilitary()" ';
	str += 'id="alsname"></td></tr></table><hr/><div id="lstcontents">&nbsp;</div>';

	document.getElementById('alpmain').innerHTML = str;
	document.getElementById('alsname').value = lsASearchText;

	drawMilitaryListContents();
}

function drawMilitaryListContents() {
	var	str, i, n, m;

	// Page handling
	n = lsAIndices.length;
	m = n % lsAPerPage;
	n = (n-m)/lsAPerPage + (m>0?1:0);
	if	(n > 0 && lsAPage >= n)
		lsAPage = n-1;

	// Planet list
	if	(lsAIndices.length == 0)
		str = '<center>&nbsp;<br/>No planets are currently under attack.<br/>&nbsp;</center>';
	else
	{
		str = '<table id="plnlist" class="list" cellspacing="0" cellpadding="0"><tr>';
		var	hdNames = ['Coordinates', 'Planet', 'Owner', 'Def. Power', 'Att. Power'];
		var	hdCols = ['class="pcoord"', 'class="pname" colspan="2"', 'class="oname"', 'class="pcoord"', 'class="pcoord"'];
		for	(i=0;i<5;i++)
		{
			if	(alPrivileges[0] < 2 && i == 2)
				continue;
			str += '<th ' + alltt[70] + ' onClick="lsASort=' + i + ';lsASortDir=!lsASortDir;updateSearchMilitary()" ';
			str += hdCols[i] + '>' + (lsASort == i ? '<b>' : '') + hdNames[i];
			if	(lsASort == i)
			{
				str += '</b><img src="'+staticurl+'/beta5/pics/';
				str += lsASortDir ? "up" : "down";
				str += '_' + color + '.gif" alt="' + (lsASortDir ? "up" : "down") + '" />';
			}
			str += '</th>';
		}
		str += '<td>&nbsp;</td></tr>';

		for	(i=lsAPerPage*lsAPage;i<lsAIndices.length&&i<lsAPerPage*(lsAPage+1);i++)
		{
			var	idx = lsAIndices[i];
			str += '<tr';
			if	(lsAttacks[idx].defPower >= 3 * lsAttacks[idx].attPower)
				str += ' class="defence"';
			else if	(lsAttacks[idx].attPower >= 3* lsAttacks[idx].defPower)
				str += ' class="attack"';
			str += '><td class="pcoord">(' + lsAttacks[idx].coords + ')</td>';
			str += '<td class="picon"><img src="'+staticurl+'/beta5/pics/pl/s/' + lsAttacks[idx].planetId + '.png" alt="[' + lsAttacks[idx].planetId + ']" /></td>';
			str += '<td><a  ' + alltt[71] + ' href="planet?id=' + lsAttacks[idx].planetId + '">' + lsAttacks[idx].planetName + '</a></td>';
			if	(alPrivileges[0] > 1)
			{
				str += '<td>' + (lsAttacks[idx].ownerId != "-1" ? ('<a  ' + alltt[72] + ' href="message?a=c&ct=0&id='+lsAttacks[idx].ownerId+'">') : '');
				str += lsAttacks[idx].ownerName + (lsAttacks[idx].ownerId != "-1" ? '</a>' : '') + '</td>';
			}
			if	(lsAttacks[idx].defPower != "")
			{
				str += '<td class="afriendly">' + formatNumber(lsAttacks[idx].defPower) + '</td>';
				str += '<td class="aenemy">' + formatNumber(lsAttacks[idx].attPower) + '</td>';
			}
			else
				str += '<td class="afriendly">?</td><td class="aenemy">?</td>';

			if	(lsAttacks[idx].defenders.length > 0)
			{
				str += '<tr class="deflist"><td>&nbsp;</td><td colspan="' + (alPrivileges[0] > 1 ? 5 : 4) + '">Defenders: ';
				str += lsAttacks[idx].defenders.join(', ') + '</td></tr>';
			}
			if	(lsAttacks[idx].attackers.length > 0)
			{
				str += '<tr class="attlist"><td>&nbsp;</td><td colspan="' + (alPrivileges[0] > 1 ? 5 : 4) + '">Attackers: ';
				str += lsAttacks[idx].attackers.join(', ') + '</td></tr>';
			}
		}
		str += '</table>';
	}

	document.getElementById('lstcontents').innerHTML = str;

	// Page selector
	str ='Page ';
	if	(n <= 1)
		str += '1 / 1';
	else
	{
		str += '<select ' + alltt[73] + ' name="lspnp" onChange="lsPPage=parseInt(this.options[this.selectedIndex].value, 10);updateSearchPlanet()">';
		for	(i=0;i<n;i++)
			str += '<option' + (i==lsPPage ? ' selected="selected"' : '') + ' value="'+i+'">' + (i+1) + '</option>';
		str += '</select> / ' + n;
	}
	document.getElementById('lstpage').innerHTML = str;
}


//--------------------------------------------------
// PENDING REQUESTS PAGE
//--------------------------------------------------

function	drawPendingPage()
{
	if	(amPage != 'Pending')
		return;

	var	str = '<h1>Pending Requests</h1>';
	if	(pRequests.length == 0)
		str += '<p>There are no players requesting to join <b>['+alTag+']</b> at this time.</p>';
	else
	{
		str += '<table cellspacing="0" cellpadding="0" class="prequests">';
		str += '<tr><td class="prblk">&nbsp;</td><th class="prsel">(S)</th><th class="prplayer">Player</th></tr>';
		var	i;
		for	(i=0;i<pRequests.length;i++)
		{
			str += '<tr><td>&nbsp;</td><td class="prsel"><input ' + alltt[110] + ' type="checkbox" name="selp" id="selp'+i+'" ';
			str += 'onClick="pRequests['+i+'].selected=!pRequests['+i+'].selected;"';
			if	(pRequests[i].selected)
				str += ' checked="checked"';
			str += '/></td><td><a href="message?a=c&ct=0&id=' + pRequests[i].id + '">';
			str += pRequests[i].player + '</a></td></tr>';
		}
		str += '<tr><td>&nbsp;</td><td colspan="2" class="prbut">';
		str += '<input type="button" ' + alltt[111] + ' value="Accept" onClick="acceptRequests();return false" /> ';
		str += '<input type="button" ' + alltt[112] + ' value="Reject" onClick="rejectRequests();return false" /> ';
		str += '</td></tr></table>';
	}

	document.getElementById('alpmain').innerHTML = str;
}

function alertRequests(acc) {
	alert('You must select at least one player to ' + (acc ? "accept into" : "reject from") + ' the alliance.');
}

function confirmRequests(acc, plu) {
	var	str = 'Are you sure you want to ' + (acc ? "accept" : "reject");
	str += ' th' + (plu ? 'ese' : 'is') + ' player' + (plu ? 's\'' : '\'s') + ' request' + (plu ? 's' : '');
	str += ' to join [' + alTag + ']?';
	return	confirm(str);
}

function alertPending(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += 'You are no longer a member of the alliance.'; break;
		case 1: str += 'You no longer have the authorization to manage pending requests.'; break;
		case 2: str += 'The alliance has reached its maximal capacity.'; break;
		case 3: str += "You can't manage requests in this game, the alliances are locked."; break;
		case 200: str += 'You are not allowed to manage pending requests while in vacation mode.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}


//--------------------------------------------------
// FORUMS ADMINISTRATION PAGE
//--------------------------------------------------

function        drawForumList()
{
	var     str = '<table cellspacing="0" cellpadding="0">';
	str += '<tr><td class="div2" id="fattl"><h1>Alliance Forums</h1><td id="crforum">&nbsp;</td></tr>';
	str += '<tr><td colspan="2" id="falist">&nbsp;</td></tr></table>';
	document.getElementById('alpmain').innerHTML = str;
	if      (faForums.length < 30)
		drawCreateForumLink();
	if      (faForums.length == 0)
		drawTextNoForums();
	else
		drawRealForumList();
}

function	drawCreateForumLink()
{
	var	str = '<a href="#" ' + alltt[120] + ' onClick="createForum();return false">';
	str += 'Create a forum</a>';
	document.getElementById('crforum').innerHTML = str;
}

function	drawTextNoForums()
{
	document.getElementById('falist').innerHTML = '<p>No forums have been defined for the alliance.</p>';
}

function	drawRealForumList()
{
	var	i, str = '<table class="list" cellspacing="0" cellpadding="0" id="fatbl">';

	str += '<tr><th class="faname">Name &amp; description</th><th class="fauserpost">New threads</th></tr>';
	for	(i=0;i<faForums.length;i++)
	{
		str += '<tr>';
		str += '<td class="faname"><b><u>' + faForums[i].name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</u></b> (';
		str += '<a ' + alltt[130] + ' href="#" onClick="editForum(' + faForums[i].id + ');return false">Edit</a> - ';
		if	(i > 0)
			str += '<a ' + alltt[131] + ' href="#" onClick="moveForum(' + faForums[i].id + ',true);return false">Move up</a> - ';
		if	(i < faForums.length - 1)
			str += '<a ' + alltt[132] + ' href="#" onClick="moveForum(' + faForums[i].id + ',false);return false">Move down</a> - ';
		str += '<a ' + alltt[133] + ' href="#" onClick="deleteForum(' + faForums[i].id + ');return false">Delete</a>)';
		if	(faForums[i].description != '')
			str += '<br/><p>' + faForums[i].description.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/\n/g,"<br/>") + '</p>';
		str += '</td><td class="fauserpost">';
		if	(faForums[i].userPost)
			str += 'Everyone';
		else
			str += 'Moderators only';
		str += '</td></tr>';
	}

	str += '</table>';
	document.getElementById('falist').innerHTML = str;
}

function	cheatAlert()
{
	alert('Possible cheating detected.');
}

function	confirmDeleteForum(name)
{
	var	str = 'You are about to delete the following forum:\n' + name + '\n';
	str += 'The forum\'s topics will be lost and you will not be able to recover them.\nPlease confirm.';
	return	confirm(str);
}

function	alertMaximumFCount()
{
	alert('The alliance has reached its maximum possible count of forums.\nYou will be taken back to the list.');
}

function	drawForumEditor()
{
	document.getElementById('falist').innerHTML = '&nbsp;';

	var	str;
	if	(faEditing.id)
	{
		f = forumById(faEditing.id);
		str = "'" + f.name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + "' forum";
	}
	else
		str = "Create a forum";
	document.getElementById('fattl').innerHTML = '<h1>' + str + '</h1>';

	str  = '<table cellspacing="0" cellpadding="0" id="feditor">';
	str += '<tr><th class="edheader">Forum name:</th><td><input ' + alltt[140] + ' type="text" name="fedit" id="fname" size="48" maxlength="64" value="';
	str += faEditing.name.replace(/"/g, '&quot;') + '" onKeyUp="faEditing.name=value.replace(/&lt;/g, \'<\').replace(/&gt;/g, \'>\').replace(/&amp;/g, \'&\');';
	str += 'updateFEditor()" onChange="faEditing.name=value;updateFEditor()" />';
	str += '</td></tr>';
	str += '<tr><th class="edheader">New threads:</th><td>';
	str += '<label><input ' + alltt[141] + ' type="radio" name="nt" value="1" onClick="faEditing.userPost=true;updateFEditor()" ';
	if	(faEditing.userPost)
		str += 'checked="checked" ';
	str += '/> Everyone</label> <label><input ' + alltt[142] + ' type="radio" name="nt" value="0" onClick="faEditing.userPost=false;updateFEditor()" ';
	if	(!faEditing.userPost)
		str += 'checked="checked" ';
	str += '/> Moderators only</label></td></tr>';
	if	(!faEditing.id)
		str += '<tr><th class="edheader">Initial position:</th><td  ' + alltt[143] + ' id="faeipos">&nbsp;</td></tr>';
	str += '<tr><th class="edheader">Description:</th><td><textarea ' + alltt[144] + ' name="fdesc" onKeyUp="faEditing.description=';
	str += 'value.replace(/&lt;/g, \'<\').replace(/&gt;/g, \'>\').replace(/&amp;/g, \'&\');updateFEditor()"';
	str += ' onChange="faEditing.description=value;updateFEditor()"';
	str += ' rows="10" cols="48">' + faEditing.description.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</textarea></td></tr>';
	str += '<td id="febut" colspan="2"><input type="button" name="feok" value="Ok" id="feok" onClick="forumEditOk();return false" ' + alltt[145] + ' />';
	str += '&nbsp;<input type="button" name="feccl" value="Cancel" onClick="forumEditCancel();return false"  ' + alltt[146] + ' /></td></tr>';
	str += '<tr><td colspan="2"><h3>Forum access</h3></td></tr>';
	str += '<tr><td colspan="2" id="faeaccess"><p>(Loading access list data ...)</p></td></tr>';

	str += '</table>';
	document.getElementById('falist').innerHTML = str;
	document.getElementById('feok').disabled = true;
	if	(!faEditing.id)
		updateFPosSelector();
}

function	updateFPosSelector()
{
	var	i, str = '<select name="fanewpos" onChange="faNewPos=this.options[this.selectedIndex].value">';
	str += '<option value="-1">At the beginning</option>';
	for	(i=0;i<faForums.length;i++)
	{
		str += '<option value="' + faForums[i].id + '"';
		if	(faNewPos == faForums[i].id)
			str += ' selected="selected"';
		str += '>After ' + faForums[i].name + '</option>';
	}
	str += '</select>';
	document.getElementById('faeipos').innerHTML = str;
}

function	drawFAccessManager()
{
	var	lnp = new Array(), lmd = new Array(), lrd = new Array(), ml, i, sc=0;
	for	(i=0;i<faAccess.length;i++)
	{
		if	(faAccess[i].priv == 0)
			lnp.push(i);
		else if	(faAccess[i].priv == 1)
			lrd.push(i);
		else
			lmd.push(i);
	}
	ml = Math.max(lnp.length, Math.max(lmd.length, lrd.length));

	var	n, str = '<table cellspacing="0" cellpadding="0" id="faeacl">';
	str += '<tr><th colspan="2">No access</th><th colspan="2">Standard access</th><th colspan="2">Moderators</th></tr>';
	for	(i=0;i<ml;i++)
	{
		str += '<tr>';
		if	(i<lnp.length)
		{
			n = faAccess[lnp[i]].name == '-' ? '<i>(standard members)</i>' : faAccess[lnp[i]].name;
			str += '<td class="faaepsel"><input ' + alltt[150] + ' type="checkbox" name="nrs" value="'+faAccess[lnp[i]].id+'" onClick="faAccess['+lnp[i]+']';
			str += '.selected=!faAccess['+lnp[i]+'].selected;updateFAccessManager()"' + (faAccess[lnp[i]].selected ? ' checked="checked"' : '');
			str += ' /></td><td>' + n + '</td>';
		}
		else
			str += '<td colspan="2">&nbsp;</td>';
		if	(i<lrd.length)
		{
			n = faAccess[lrd[i]].name == '-' ? '<i>(standard members)</i>' : faAccess[lrd[i]].name;
			str += '<td class="faaepsel"><input ' + alltt[150] + ' type="checkbox" name="rds" value="'+faAccess[lrd[i]].id+'" onClick="faAccess['+lrd[i]+']';
			str += '.selected=!faAccess['+lrd[i]+'].selected;updateFAccessManager()"' + (faAccess[lrd[i]].selected ? ' checked="checked"' : '');
			str += ' /></td><td>' + n + '</td>';
		}
		else
			str += '<td colspan="2">&nbsp;</td>';
		if	(i<lmd.length)
		{
			n = faAccess[lmd[i]].name == '-' ? '<i>(standard members)</i>' : faAccess[lmd[i]].name;
			str += '<td class="faaepsel">';
			if	(faAccess[lmd[i]].priv == 2)
			{
				str += '<input ' + alltt[150] + ' type="checkbox" name="mds" value="'+faAccess[lmd[i]].id+'" onClick="faAccess['+lmd[i]+']';
				str += '.selected=!faAccess['+lmd[i]+'].selected;updateFAccessManager()" '+ (faAccess[lmd[i]].selected ? ' checked="checked"' : '') + '/>';
			}
			else
			{
				str += '&nbsp';
				n = '<b>'+n+'</b>';
			}
			str += '</td><td>' + n + '</td>';
		}
		else
			str += '<td colspan="2">&nbsp;</td>';
		str += '</tr>';
	}
	str += '<tr><td colspan="2" id="faeab0">&nbsp;</td><td colspan="2" id="faeab1">&nbsp;</td><td colspan="2" id="faeab2">&nbsp;</td></tr>';
	str += '</table>';
	document.getElementById('faeaccess').innerHTML = str;

	updateFAccessManager();
}

function	updateFAccessManager()
{
	var	i, sc = 0;
	for	(i=0;i<faAccess.length;i++)
	{
		if	(faAccess[i].selected && faAccess[i].priv < 3)
			sc ++;
	}
	if	(sc == 0)
	{
		for	(i=0;i<3;i++)
			document.getElementById('faeab'+i).innerHTML = '&nbsp;';
		return;
	}

	var	str, btn = ['Remove access', 'Give standard access', 'Make moderators'];
	for	(i=0;i<3;i++)
	{
		str = '<input ' + alltt[160] + ' type="button" name="sfea'+i+'" value="'+btn[i]+'" onClick="setFAccessLevel(' + i + ');return false" />';
		document.getElementById('faeab'+i).innerHTML = str;
	}
}

function alertForum(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += 'You are no longer a member of this alliance.'; break;
		case 1: str += 'The name of this forum is too short.'; break;
		case 2: str += 'A forum with the same name already exists.'; break;
		case 3: str += 'This forum has been deleted while you were editing it.'; break;
		case 4: str += 'You aren\'t a forum administrator anymore.'; break;
		case 5: str += 'The alliance already has 30 forums.'; break;
		case 6: str += 'Probable bug: the forum to insert after wasn\'t transmitted.'; break;
		case 7: str += 'A server error has occured.'; break;
		case 8: str += 'This forum has already been deleted.'; break;
		case 200: str += "You can\'t modify an alliance\'s forums while in vacation mode."; break;
		default: str += 'An unknown error has happened.'; break;
	}
	alert(str);
}

function alertForumChanged() {
	alert('The forum you are editing at this moment has been modified!');
}

function alertForumDeleted() {
	alert('The forum you are editing at this moment has been deleted!');
}


//--------------------------------------------------
// RANKS ADMINISTRATION PAGE
//--------------------------------------------------

function	drawRanksList()
{
	var	i, str = '<table cellspacing="0" cellpadding="0"><tr>';
	str += '<td><h1>Ranks Administration</h1></td><td id="crforum"><a ' + alltt[170] + ' href="#" onClick="createRank();return false">Create a rank</a></td></tr>';
	str += '<tr><td colspan="2"><table class="list" cellspacing="0" cellpadding="0" id="ratbl">';
	str += '<tr><td class="rkshow">&nbsp;</td><th class="rkname">Rank name</th><th class="rkact">Actions</th><th class="rkmembers">Members</th></tr>';
	str += '<tr><th colspan="4"><hr/></th></tr>';
	for	(i=0;i<raRanks.length;i++)
	{
		str += '<tr><td class="rkshow"><a ' + alltt[171] + ' href="#" onClick="showRank('+i+');return false">[ ';
		str += raRanks[i].open ? "-" : "+";
		var n = (raRanks[i].name == "-" ? '<i>(Standard member)</i>' : raRanks[i].name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;'));
		str += ' ]</a></td><td class="rkname"><u>' + n + '</u>';
		if	(raRanks[i].open)
			str += '<p>' + drawRankDetails(raRanks[i]) + '</p>';
		str += '</td><td class="rkact"><a ' + alltt[172] + ' href="#" onClick="editRank('+i+');return false">Edit</a>';
		if	(raRanks[i].name != "-")
			str += ' - <a  ' + alltt[173] + ' href="#" onClick="deleteRank('+i+');return false">Delete</a>';
		str += '</td><td class="rkmembers">' + raRanks[i].players + '</td></tr>';
	}
	str += '</table></td></tr></table>';

	document.getElementById('alpmain').innerHTML = str;
}

function drawRankDetails(r) {
	var	a = new Array();
	if (r.list == "1") {
		a.push("Can access the player list.");
	} else if (r.list == "2") {
		a.push("Can access the planet list.");
	} else if (r.list == "3") {
		a.push("Can access the detailed planet list.");
	}

	if (r.attacks) {
		a.push("Has access to the military situation display.");
	}

	var str;
	switch (r.techTrade) {
		case 0: str = ''; break;
		case 1: str = 'Can submit technology list'; break;
		case 2: str = 'Can submit technology list and requests'; break;
		case 3: str = 'Has access to the alliance\'s technology list'; break;
		case 4: str = 'Can manage the alliance\'s technology list and orders'; break;
	}
	if (str != '') {
		a.push(str);
	}

	if (r.accept && !lockedAlliances) {
		a.push('Can accept new members into the alliance.');
	}
	if (r.canKick && !lockedAlliances) {
		if (r.ranksKick.length == raRanks.length - 1) {
			a.push('Can kick members regardless of their ranks.');
		} else {
			var	str, i, r2;
			str = 'Can kick members with the ';
			for (i = 0; i < r.ranksKick.length; i ++) {
				r2 = rankById(r.ranksKick[i]);
				if (!r2) {
					continue;
				}
				if (i > 0 && i < r.ranksKick.length - 1) {
					str += ', ';
				} else if (i > 0) {
					str += ' or ';
				}
				str += '<b>' + (r2.name == '-' ? 'Standard member' : r2.name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')) + '</b>';
			}
			a.push(str + ' rank' + (r.ranksKick.length > 1 ? 's' : '') + '.');
		}
	}
	if (r.canSet) {
		if (r.ranksChange.length == raRanks.length - 1) {
			a.push('Can modify rank for all members.');
		} else {
			var	str, i, r2;
			str = 'Can modify rank for members with the ';
			for (i = 0; i < r.ranksChange.length; i ++) {
				r2 = rankById(r.ranksChange[i]);
				if (!r2) {
					continue;
				}
				if (i>0&&i<r.ranksChange.length - 1) {
					str += ', ';
				} else if (i > 0) {
					str += ' or ';
				}
				str += '<b>' + (r2.name == '-' ? 'Standard member' : r2.name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')) + '</b>';
			}
			a.push(str + ' rank' + (r.ranksChange.length > 1 ? 's' : '') + '.');
		}
	}
	if (r.forumAdmin) {
		a.push('Can administrate the forums.');
	} else {
		if (r.fRead.length > 0) {
			if (r.fRead.length == raForums.length) {
				a.push('Can access all forums');
			} else {
				var	str, i, f;
				str = 'Can access the following forum'+(r.fRead.length>1?'s':'')+': ';
				for (i = 0; i < r.fRead.length; i ++) {
					f = raForumById(r.fRead[i]);
					if (!f) {
						continue;
					}
					if (i > 0 && i < r.fRead.length - 1) {
						str += ', ';
					} else if (i>0) {
						str += ' and ';
					}
					str += '<b>'+f.name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')+'</b>';
				}
				a.push(str + '.');
			}
		}
		if (r.fMod.length > 0) {
			if (r.fMod.length == raForums.length) {
				a.push('Can moderate all forums');
			} else {
				var	str, i, f;
				str = 'Can moderate the following forum'+(r.fMod.length>1?'s':'')+': ';
				for (i = 0; i < r.fMod.length; i ++) {
					f = raForumById(r.fMod[i]);
					if (!f) {
						continue;
					}
					if (i > 0 && i < r.fMod.length - 1) {
						str += ', ';
					} else if (i>0) {
						str += ' and ';
					}
					str += '<b>'+f.name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')+'</b>';
				}
				a.push(str + '.');
			}
		}
	}
	if (r.diplContact) {
		a.push('Is a diplomatic contact for the alliance.');
	}
	if (isDemocracy && !lockedAlliances && (r.canVote || r.canditate)) {
		var	str = 'Can ';
		if (r.canVote) {
			str += 'vote to elect the leader';
		}
		if (r.candidate) {
			str += (r.canVote ? ' and ' : '') + 'run for presidency of the alliance';
		}
		str += '.';
		a.push(str);
	}

	return	a.join('<br/>');
}


function displayRankEditor() {
	var	i, str = '<table cellspacing="0" cellpadding="0" id="rankedit"><tr>';
	str += '<td colspan="2"><h1>' + (raEditing.id ? 'Edit a Rank' : 'Create a Rank') + '</h1></td></tr>';

	str += '<tr><th>Designation:</th><td>'
		+ (raEditing.name != '-'
			?  ('<input ' + alltt[180] + ' type="text" name="rname" value="'
				+ raEditing.name.replace(/"/g, '&quot;')
				+ '" size="33" maxlength="32" onChange="raEditing.name=value;updateREditor()" '
				+ 'onKeyUp="raEditing.name=value;updateREditor()" />')
			: '<i>Standard member</i>') 
		+ '</td></tr>';

	str += '<tr><th>List access:</th><td><select  ' + alltt[181] + ' name="rlist" onChange="raEditing.list=options[selectedIndex].value;drawRankMgmt();updateREditor()">';
	var	lacc = ['No access', 'Member list', 'Planet list', 'Detailed planet list'];
	for	(i=0;i<4;i++)
	{
		str += '<option value="'+i+'"';
		if	(raEditing.list == i.toString())
			str += ' selected="selected"';
		str += '>' + lacc[i] + '</option>';
	}
	str += '</select></td></tr>';
	str += '<tr><td>&nbsp;</td><td><input ' + alltt[182] + ' type="checkbox" name="rmil" value="1" onClick="raEditing.attacks=!raEditing.attacks;updateREditor()"';
	if	(raEditing.attacks)
		str += ' checked="checked"';
	str += ' /> List of planets under attack</td></tr>';

	// Tech trading
	if (techTrade != 'N') {
		var ttMode = ['No access', 'Submit list', '', 'View list', 'Manage orders'];
		if (techTrade == 'R') {
			ttMode[2] = 'Submit list &amp; requests';
		}
		str += '<tr><th>Tech. trading:</th><td><select name="ttaccess" onChange="raEditing.techTrade='
			+ 'parseInt(this.options[this.selectedIndex].value, 10);drawRankMgmt();updateREditor()">';
		for (i = 0; i < ttMode.length; i ++) {
			if (ttMode[i] == '') {
				continue;
			}
			str += '<option value="' + i + '"' + (raEditing.techTrade == i ? ' selected="selected"' : '')
				+ '>' + ttMode[i] + '</option>';
		}
		str += '</select></td></tr>';
	}

	str += '<tr><th>Diplomatic contact:</th><td>';
	str += '<label><input  ' + alltt[183] + ' type="radio" name="rdipl" value="1" onClick="raEditing.diplContact=true;updateREditor()"'+(raEditing.diplContact?' checked="checked"':'');
	str +=' /> Yes</label> <label><input ' + alltt[184] + ' type="radio" name="rdipl" value="0" onClick="raEditing.diplContact=false;updateREditor()"';
	str += (raEditing.diplContact?'':' checked="checked"')+' /> No</label></td></tr>';

	if (!lockedAlliances) {
		str += '<tr><th>Can vote (*):</th><td>'
			+ '<label><input ' + alltt[185] + ' type="radio" name="rvote" value="1" '
			+ 'onClick="raEditing.canVote=true;updateREditor()"'
			+ (raEditing.canVote ? ' checked="checked"' : '') + ' /> Yes</label> '
			+ '<label><input ' + alltt[186] + 'type="radio" name="rvote" value="0" '
			+ 'onClick="raEditing.canVote=false;updateREditor()"'
			+ (raEditing.canVote ? '' : ' checked="checked"') +' /> No</label>'
			+ '</td></tr><tr><th>Can apply for presidency (*):</th><td>'
			+ '<label><input  ' + alltt[187] + ' type="radio" name="rcnd" value="1" '
			+ 'onClick="raEditing.candidate=true;updateREditor()"'
			+ (raEditing.candidate ? ' checked="checked"' : '') + ' /> Yes</label> <label><input '
			+ alltt[188] + ' type="radio" name="rcnd" value="0"'
			+ ' onClick="raEditing.candidate=false;updateREditor()"'
			+ (raEditing.candidate ? '' : ' checked="checked"') + ' /> No</label></td></tr>'
			+ '<tr><td colspan="2">(*) These privileges only apply if the alliance is democratic.</td></tr>';
	}

	str += '<tr><td colspan="2"><h3>Member management:</h3></td></tr>';
	if (!lockedAlliances) {
		str += '<tr><th>Accept pending requests:</th><td>';
		str += '<label><input ' + alltt[189] + ' type="radio" name="racc" value="1" onClick="raEditing.accept=true;updateREditor()"'+(raEditing.accept?' checked="checked"':'')+' /> Yes</label> ';
		str += '<label><input ' + alltt[190] + ' type="radio" name="racc" value="0" onClick="raEditing.accept=false;updateREditor()"'+(raEditing.accept?'':' checked="checked"')+' /> No</label>';
		str += '</td></tr>';
		str += '<tr><th>Kick members:</th><td id="radmkick">&nbsp;</td></tr>'
	}
	str += '<tr><th>Change ranks:</th><td id="radmchange">&nbsp;</td></tr>'

	str += '<tr><td colspan="2"><h3>Forum access:</h3></td></tr>';
	str += '<tr><th>Forum administrator:</th><td>';
	str += '<label><input ' + alltt[191] + ' type="radio" name="rfadm" value="1" onClick="raEditing.forumAdmin=true;updateREForums();updateREditor()"';
	str += (raEditing.forumAdmin?' checked="checked"':'')+' />';
	str += ' Yes</label> <label><input ' + alltt[192] + ' type="radio" name="rfadm" value="0" onClick="raEditing.forumAdmin=false;updateREForums();updateREditor()"';
	str += (raEditing.forumAdmin?'':' checked="checked"')+' /> No</label>';
	str += '</td></tr>';
	str += '<tr><th id="refperm">&nbsp;</th><td id="radmforums">&nbsp;</td></tr>';

	str += '<tr><td colspan="2">&nbsp;</td></tr>';
	str += '<tr><td colspan="2" class="rebut"><input ' + alltt[193] + ' type="button" value="Ok" name="reok" id="reok" onClick="rankEditOk();return false" />';
	str += ' <input ' + alltt[194] + ' type="button" value="Cancel" name="recancel" onClick="rankEditCancel();return false" />';
	str += '</td></tr>';

	str += '</table>';
	document.getElementById('alpmain').innerHTML = str;
	document.getElementById('reok').disabled = true;
	drawRankMgmt();
	updateREForums();
}


function	drawRankMgmt() {
	if (raEditing.list == 0) {
		if (!lockedAlliances) {
			document.getElementById('radmkick').innerHTML = 'Not available without list access.';
		}
		document.getElementById('radmchange').innerHTML = 'Not available without list access.';
		return;
	}

	var str;
	if (!lockedAlliances) {
		str = '<label><input ' + alltt[195] + ' type="radio" name="rekick" value="0" onClick="raEditing.canKick=0;drawKickList();updateREditor()" ';
		str += (raEditing.canKick == 0 ? ' checked="checked"' : '') + ' /> No</label> ';
		str += '<label><input ' + alltt[196] + ' type="radio" name="rekick" value="2" onClick="raEditing.canKick=2;drawKickList();updateREditor()" ';
		str += (raEditing.canKick == 2 ? ' checked="checked"' : '') + ' /> Yes</label> ';
		if	(raRanks.length > 1 || !raEditing.id)
		{
			str += '<label><input ' + alltt[197] + ' type="radio" name="rekick" value="1" onClick="raEditing.canKick=1;drawKickList();updateREditor()" ';
			str += (raEditing.canKick == 1 ? ' checked="checked"' : '') + ' /> Only specific ranks</label><span id="rekicklist"> </span>';
		}
		document.getElementById('radmkick').innerHTML = str;
		drawKickList();
	}

	str = '<label><input ' + alltt[198] + ' type="radio" name="rechange" value="0" onClick="raEditing.canSet=0;drawChangeList();updateREditor()" ';
	str += (raEditing.canSet == 0 ? ' checked="checked"' : '') + ' /> No</label> ';
	str += '<label><input ' + alltt[199] + ' type="radio" name="rechange" value="2" onClick="raEditing.canSet=2;drawChangeList();updateREditor()" ';
	str += (raEditing.canSet == 2 ? ' checked="checked"' : '') + ' /> Yes</label> ';
	if	(raRanks.length > 1 || !raEditing.id)
	{
		str += '<label><input ' + alltt[200] + ' type="radio" name="rechange" value="1" onClick="raEditing.canSet=1;drawChangeList();updateREditor()" ';
		str += (raEditing.canSet == 1 ? ' checked="checked"' : '') + ' /> Only specific ranks</label><span id="rechangelist"> </span>';
	}
	document.getElementById('radmchange').innerHTML = str;
	drawChangeList();
}

function	drawChangeList()
{
	drawRERankList(raEditing.canSet, raEditing.ranksChange, 'Change', 'change', 'Standard member');
}

function	drawKickList()
{
	drawRERankList(raEditing.canKick, raEditing.ranksKick, 'Kick', 'kick', 'Standard member');
}

function	updateREForums()
{
	if	(raEditing.forumAdmin)
	{
		document.getElementById('refperm').innerHTML = document.getElementById('radmforums').innerHTML = '&nbsp;';
		return;
	}
	else if	(raForums.length == 0)
	{
		document.getElementById('refperm').innerHTML = 'Forum access:';
		document.getElementById('radmforums').innerHTML = 'No forums have been created yet.';
		return;
	}

	var	i, str = '<table cellspacing="0" cellpadding="0" class="reflist">';
	var	rl = '!' + raEditing.fRead.join('!') + '!';
	var	ml = '!' + raEditing.fMod.join('!') + '!';
	for	(i=0;i<raForums.length;i++)
	{
		var	isMod = (ml.indexOf('!' + raForums[i].id + '!') != -1);
		var	isRd = !isMod && (rl.indexOf('!' + raForums[i].id + '!') != -1);
		str += '<tr><td><b>' + raForums[i].name.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</b></td><td>';
		str += '<label><input ' + alltt[201] + ' type="radio" name="facc' + i + '" value="0" onClick="rankSetForum('+i+',0);updateREditor()"';
		str += (isMod || isRd) ? '' : ' checked="checked"';
		str += ' /> No access</label>';
		str += '<label><input ' + alltt[202] + ' type="radio" name="facc' + i + '" value="1" onClick="rankSetForum('+i+',1);updateREditor()"';
		str += (isRd ? ' checked="checked"' : '') + ' /> User</label>';
		str += '<label><input ' + alltt[203] + ' type="radio" name="facc' + i + '" value="2" onClick="rankSetForum('+i+',2);updateREditor()"';
		str += (isMod ? ' checked="checked"' : '') + ' /> Moderator</label>';
		str += '</td></tr>';
	}
	str += '</table>';
	document.getElementById('refperm').innerHTML = 'Forum access:';
	document.getElementById('radmforums').innerHTML = str;
}


function	confirmDeleteRank(name)
{
	return	confirm('You are about to delete the ' + name + ' rank.\nPlease confirm.');
}


function	showRankDeletePage()
{
	var	i, str = '<h1>Confirm Rank Deletion</h1>';
	str += '<p>You are about to delete the <b>' + raDeleting.name + '</b> rank.<br/>';
	str += '<b>' + raDeleting.players + '</b> member' + (raDeleting.players > 1 ? 's' : '') + ' of the alliance ha';
	str += (raDeleting.players > 1 ? 've' : 's') + ' been assigned this rank.<br/>';
	str += 'You may choose to proceed with the deletion of this rank; members will be demoted to the ';
	str += '<select ' + alltt[204] + ' name="rddemo" onChange="raDemote=options[selectedIndex].value">';
	for	(i=0;i<raRanks.length;i++)
	{
		if	(raRanks[i].id == raDeleting.id)
			continue;
		str += '<option value="' + raRanks[i].id + '"';
		if	(raRanks[i].id == raDemote)
			str += ' selected="selected"';
		str += '>' + (raRanks[i].name == "-" ? '(Standard member)' : raRanks[i].name) + '</option>';
	}
	str += '</select> rank.<br/>Please confirm the deletion.<br/><br/>';
	str += '<input ' + alltt[205] + ' type="button" name="rdok" value="Delete this rank" onClick="rankDeleteOk();return false" /> ';
	str += '<input ' + alltt[206] + ' type="button" name="rdcancel" value="Cancel" onClick="rankDeleteCancel();return false" />';
	str += '</p>';
	document.getElementById('alpmain').innerHTML = str;
}


function alertRank(ei) {
	var str = 'Error\n';
	switch (ei) {
		case 0: str += 'You are no longer a member of the alliance.'; break;
		case 1: str += 'This name is too short, it must be at least 4 characters long.'; break;
		case 2: str += 'This name is too long, it must be less than 32 characters long.'; break;
		case 3: str += 'This name is already in use.'; break;
		case 4: str += 'You aren\'t the leader of the alliance anymore.'; break;
		case 5: str += "This rank no longer exists."; break;
		case 6: str += 'The server received invalid data. This may be a bug.'; break;
		case 7: str += 'One of the selected forums has been deleted.'; break;
		case 200: str += 'You can\'t modify the ranks while on vacation.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}
