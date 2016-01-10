var	listTitle = 'Allies List';
var	emptyList = 'You are not a trusting person, it would seem.';
var	notTrusted = 'Apparently, no one trusts you.';
var	addBanText = 'Blacklist a player';
var	addBanLabel = 'Name of the player to ban:';
var	addBanButton = 'Ban player';
var	blackListTitle = 'Blacklist contents';
var	emptyBlackList = 'Everyone can add you to their Trusted Allies lists.';
var	raHeaders = ['Player', 'Trust Level'];

function        makeAlliesTooltips()
{
        tratt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<30;i++)
			tratt[i] = "";
		return;
	}
	tratt[0] = tt_Dynamic("Use this text field to type in the name of the new allie to add to your trusted allies list");
	tratt[1] = tt_Dynamic("Click here to add this player to your trusted allies list");
	tratt[10] = tt_Dynamic("Click here to remove the selected player(s) from your trusted allies list");
	tratt[11] = tt_Dynamic("Click here to move the selected player up in the list");
	tratt[12] = tt_Dynamic("Click here to move the selected player down in the list");
	tratt[15] = tt_Dynamic("Click here to remove yourself to this player's trusted allies list");
	tratt[16] = tt_Dynamic("Click here to remove yourself to this player's trusted allies list and prevent him from adding you again");
	tratt[20] = tt_Dynamic("Use this checkbox to select the corresponding trusted ally");
	tratt[21] = tt_Dynamic("Use this checkbox to select this player");
}


function	drawAddAlly(value)
{
	var	str = '<h1>Add Ally</h1>';
	str += '<p>Name of the new ally: <input type="text" ' + tratt[0] + 'name="newally" id="newally" value="';
	str += value.replace(/"/g, '&quot;') + '" maxlength="15" size="16" /> <input type="button" ';
	str += 'name="add" ' + tratt[1] + ' value="Add" onClick="addAlly();return false" /></p>';
	return	str;
}

function	drawButtons(cr,cmu,cmd)
{
	var	str = '';
	if	(cr)
		str += '<input type="button" ' + tratt[10] + ' name="del" value="Remove" onClick="removeAllies();return false" />';
	if	(cmu)
		str += '<input type="button" ' + tratt[11] + ' name="mup" value="Up" onClick="moveUp();return false" />';
	if	(cmd)
		str += '<input type="button" ' + tratt[12] + ' name="mdown" value="Down" onClick="moveDown();return false" />';
	if	(str == "")
		str = "&nbsp;";
	return	str;
}

function	drawRButtons()
{
	var	str;
	str = '<input type="button" ' + tratt[15] + ' name="rdel" value="Remove" onClick="removeRAllies();return false" />'
		+ '<input type="button" ' + tratt[16] + ' name="rban" value="Remove and Ban" onClick="removeBanRAllies();return false" />';
	return	str;
}

function	drawBLButtons()
{
	var	str;
	str = '<input type="button" ' + tratt[31] + ' name="bldel" value="Remove" onClick="removeBans();return false" />';
	return	str;
}

function	addAllyConfirm(name)
{
	var	str = 'You are about to add the player named ' + name + ' to your trusted allies list.\n';
	str += 'It will give this player partial control over your fleets when you are not online.\n';
	str += 'Please confirm.';
	return	confirm(str);
}

function	confirmRemoveRAllies(ids)
{
	var	i, n = new Array();
	var	str = 'You are about to remove yourself from the folowing player' + (ids.length > 1 ? "s' " : "'s")
			+ " Trusted Allies list" + (ids.length > 1 ? "s" : "") + ':\n';
	for	(i=0;i<ids.length;i++)
		n.push(idRAllies[ids[i]].name);
	str += n.join(', ') + '\n You will no longer be able to handle ' + (ids.length > 1 ? "these players'" : "this player's")
		+ ' fleets.\nPlease confirm.';
	return	confirm(str);
}

function	confirmBanRAllies(ids)
{
	var	i, n = new Array();
	var	str = 'You are about to remove yourself from the folowing player' + (ids.length > 1 ? "s' " : "'s")
			+ " Trusted Allies list" + (ids.length > 1 ? "s" : "") + ' and add ' + (ids.length > 1 ? 'them' : 'him')
			+ ' to your T.A. blacklist:\n';
	for	(i=0;i<ids.length;i++)
		n.push(idRAllies[ids[i]].name);
	str += n.join(', ') + '\n You will no longer be able to handle ' + (ids.length > 1 ? "these players'" : "this player's")
		+ ' fleets and ' + (ids.length > 1 ? 'they' : 'he') + ' won\'t be able to add you to ' + (ids.length > 1 ? 'their' : 'his')
		+ ' Trusted Allies list again.\nPlease confirm.';
	return	confirm(str);
}

function	commandAlert(ei)
{
	var	str = 'Error\n';
	switch	(ei)
	{
		case 0: str += 'Player not found.'; break;
		case 1: str += 'Please specify the name of the player to add to the list.'; break;
		case 2: str += 'Player not found.'; break;
		case 3: str += 'You can\'t add yourself to your allies list.'; break;
		case 4: str += 'This player is already in your allies list.'; break;
		case 5: str += 'This player is already on 5 other players\' allies list.'; break;
		case 6: str += 'This player is on your enemy list.'; break;
		case 7: str += 'One of the selected players no longer trusts you anyway.'; break;
		case 8: str += 'One of the selected players is already in your T.A. blacklist (this is a bug).'; break;
		case 9: str += 'This player has blacklisted you and can\'t be set as a trusted ally anymore.'; break;
		case 10: str += 'This player has already been removed from the blacklist.'; break;
		case 11: str += 'Please type the name of the player to add to the blacklist.'; break;
		case 12: str += 'This player is already in your blacklist.'; break;
		case 13: str += 'You can\'t add yourself to your blacklist.'; break;
		case 14: str += 'You already have 5 trusted allies.'; break;
		case 200: str += 'You are not allowed to edit your trusted allies list while in vacation mode.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}
