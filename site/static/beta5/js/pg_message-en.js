var	colTitles = new Array('(S)', 'Subject', 'Date', 'From', 'To');
var	msgNotFound = "Message body could not be found.";
var	replyText = "Reply";

function	makeMessagesTooltips()
{
	msgtt = new Array();
	if	(ttDelay == 0)
	{
		var	i;
		for	(i=0;i<86;i++)
			msgtt[i] = "";
		return;
	}
	msgtt[0] = tt_Dynamic("Click here to write a new message");
	msgtt[1] = tt_Dynamic("Click here to go to the folder management page");
	msgtt[2] = tt_Dynamic("Click here to view the contents of this folder");
	msgtt[3] = tt_Dynamic("Click here to go to the forums' page");
	msgtt[10] = tt_Dynamic("Select the type of recipient for this message");
	msgtt[11] = tt_Dynamic("Specify here the name of the recipient of this message");
	msgtt[12] = tt_Dynamic("Specify here the subject of this message");
	msgtt[13] = tt_Dynamic("Specify here the body of this message");
	msgtt[14] = tt_Dynamic("Click here to send this message");
	msgtt[20] = tt_Dynamic("Select the number of messages to display on each page");
	msgtt[21] = tt_Dynamic("Use this check box to switch between threaded / linear view");
	msgtt[30] = tt_Dynamic("Select the page of messages to display");
	msgtt[40] = tt_Dynamic("Click here to delete the selected message(s)");
	msgtt[41] = tt_Dynamic("Click here to move the selected message(s) to the chosen folder");
	msgtt[42] = tt_Dynamic("Select the folder to which you want to move the selected message(s)");
	msgtt[50] = tt_Dynamic("Use this check box to select / unselect this folder");
	msgtt[51] = tt_Dynamic("Click here to rename the selected folder");
	msgtt[60] = tt_Dynamic("Click here to create a new folder");
	msgtt[70] = tt_Dynamic("Click here to flush the contents of the selected folder(s)");
	msgtt[71] = tt_Dynamic("Click here to delete the selected folder(s)");
	msgtt[80] = tt_Dynamic("Use this check box to select / unselect all messages in this folder");
	msgtt[81] = tt_Dynamic("Click here to sort the messages according to this field");
	msgtt[82] = tt_Dynamic("Use this check box to select / unselect this message");
	msgtt[83] = tt_Dynamic("Click here to display / hide the body of this message");
	msgtt[84] = tt_Dynamic("Click here to send a message to this recipient");
	msgtt[85] = tt_Dynamic("Click here to reply to this message");
}


function	drawMainMenu()
{
	var	str = '<table cellspacing="0" cellpadding="0">';
	str += '<tr><td colspan="2">&nbsp;</td></tr>';
	str += '<tr><th colspan="2">Messages</th></tr>';
	str += '<tr><td colspan="2">&nbsp;</td></tr>';

	str += '<tr><td colspan="2">';
	if	(page == "Compose")
		str += '<i>Compose a message</i>';
	else
		str += '<a href="#" onClick="composeNew(); return false"' + msgtt[0] + '>Compose a message</a>';
	str += '</td></tr>';

	str += '<tr><td colspan="2">';
	if	(page == "Folders")
		str += '<i>Folders</i>';
	else
		str += '<a href="#" onClick="manageFolders(); return false"' + msgtt[1] + '>Folders</a>';
	str += '</td></tr>';

	var	i, ic;
	for	(i=0;i<folders.length;i++)
	{
		ic = (page == "Browse" && vFolder == i);
		str += '<tr>';
		if	(i==0)
			str += '<td class="mmspc" rowspan="' + folders.length + '">&nbsp;</td>';
		str += '<td>';
		str += ic ? '<i>' : ('<a href="#" onClick="browseFolder(' + i + '); return false"' + msgtt[2] + ' >');
		if	(folders[i].nCount > 0)
			str += "<b>"
		if	(folders[i].type == 'CUS')
			str += folders[i].name;
		else if	(folders[i].type == 'IN')
			str += 'Inbox';
		else if	(folders[i].type == 'INT')
			str += 'Internal transmissions';
		else
			str += 'Sent messages';
		if	(folders[i].nCount > 0)
			str += " (" + folders[i].nCount + ")</b>";
		str += ic ? '</i>' : '</a>';
		str += '</td></tr>';
	}
	str += '<tr><td colspan="2">&nbsp;</td></tr>';
	str += '<tr><th colspan="2"><a href="forums"' + msgtt[3] + '>Forums</a></th></tr>';
	str += '</table>';

	document.getElementById('mmenu').innerHTML = str;
}



function	drawComposePage()
{
	var	i, a = new Array('player', 'planet', 'alliance');
	var	str = '<h1>Compose a message</h1><form action="?">';
	str += '<table cellspacing="1" cellpadding="1" class="mcomp">';

	if	(pCompose.sent)
		str += '<tr><td colspan="2"><p><b>Your message has been sent</b></p></td></tr>';
	else if	(pCompose.errors[0] != 0)
	{
		str += '<tr><td>&nbsp;</td><td><b class="err">';
		switch (pCompose.errors[0])
		{
			case 1: str += 'Please specify message recipient.'; break;
			case 2: str += 'Recipient not found.'; break;
		}
		str += '</b></td></tr>';
	}
	str += '<tr><th>Message to:</th><td><select name="mt" id="mcmtype"' + msgtt[10] +' >';
	for	(i=0;i<a.length;i++)
	{
		str += '<option value="' + i + '"';
		if	(i == pCompose.tType)
			str += ' selected="selected"';
		str += '>' + a[i] + '</option>';
	}
	str += '</select><input type="text" name="tn" id="mcmto" size="45" value="'+pCompose.tText+'"' + msgtt[11] + ' /></td></tr>';

	if	(pCompose.errors[1] != 0)
	{
		str += '<tr><td>&nbsp;</td><td><b class="err">';
		switch (pCompose.errors[1])
		{
			case 1: str += 'Please specify message subject.'; break;
			case 2: str += 'Subject is too long (max. 64 characters).'; break;
		}
		str += '</b></td></tr>';
	}
	str += '<tr><th>Subject:</th><td><input type="text" name="ms" id="mcmsub" size="58" maxlength="64" value="'+pCompose.sText+'"' + msgtt[12] + ' /></td></tr>';

	if	(pCompose.errors[2] != 0)
	{
		str += '<tr><td>&nbsp;</td><td><b class="err">';
		switch (pCompose.errors[2])
		{
			case 1: str += 'You are not allowed to send empty messages.'; break;
		}
		str += '</b></td></tr>';
	}
	str += '<tr><th>Message:</th><td><textarea name="mc" id="mcmsg" cols="56" rows="15" wrap="word"' + msgtt[13] + '></textarea></td></tr>';
	str += '<tr><td>&nbsp;</td><td class="mbut"><input type="button" id="mcmsend" ';
	str += 'value="Send message" onClick="sendMessage(); return false;"' + msgtt[14] + ' /></td></tr>';
	str += '</table>';

	if	(pCompose.rId != -1)
		str += '<h2>Replying to...</h2><p>' + pCompose.rText + '</p>';

	str += '</form>';
	document.getElementById('mbody').innerHTML = str;
}



function	drawPageNotFound()
{
	document.getElementById('mbody').innerHTML = '<h1>Folder not found</h1><p>The folder you are trying to browse cannot be found.</p>';
}



function	drawFolderLayout()
{
	var	str = '<h1>"';
	switch	(folders[vFolder].type)
	{
		case "IN": str += "Inbox"; break;
		case "INT": str += "Internal Transmissions"; break;
		case "OUT": str += "Sent Messages"; break;
		case "CUS": str += folders[vFolder].name; break;
	}
	str += '" Folder</h1><div id="fbrowser"><p>Loading browser...</p></div>';
	document.getElementById('mbody').innerHTML = str;
}


function	drawFolderControls(s)
{
	var	i, str = '<form action="?"><table cellspacing="0" cellpadding="0" class="fctrl">';
	str += '<tr class="topctrl"><td id="mppcell">';

	str += 'Messages per page: <select name="mpp" id="fcmpp" onChange="setMsgPerPage();"' + msgtt[20] + ' >';
	for	(i=1;i<6;i++)
	{
		str += '<option value="'+i+'"';
		if	(i*10 == s[0])
			str += ' selected="selected"';
		str += '>'+(i*10)+'</option>';
	}
	str += '</select></td>';

	str += '<td><label><input type="checkbox" name="thr" id="fcthr" ' + msgtt[21] + ' onClick="switchThreaded();"';
	if	(s[3])
		str += ' checked="checked"';
	str += '/>Threaded view</label></td><td id="fcpage">&nbsp;</td></tr>';

	str += '<tr><td id="msglist" colspan="3"><b class="msgmsg">Loading message list ...</b></td></tr>';
	str += '<tr><td id="msgctrl" colspan="3">&nbsp;</td></tr>';
	str += '</table></form>';

	document.getElementById('fbrowser').innerHTML = str;
}


function	drawEmptyFolder()
{
	document.getElementById('fcpage').innerHTML = '&nbsp;';
	document.getElementById('msgctrl').innerHTML = '&nbsp;';
	document.getElementById('msglist').innerHTML = '<b class="msgmsg">There are no messages in this folder.</b>';
}


function	drawPageSelector(npg, np)
{
	var	str, i;
	if	(npg == 1)
		str = 'Page 1 / 1';
	else
	{
		str = 'Page <select name="pgs" id="pgs" onChange="changePage();"' + msgtt[30] + '>';
		for	(i=0;i<npg;i++)
		{
			str += '<option value="' + i + '"';
			if	(np == i)
				str += ' selected="selected"';
			str += '>' + (i+1) + '</option>';
		}
		str += '</select> / ' + npg;
	}
	document.getElementById('fcpage').innerHTML = str;
}


function	genMsgControls(cfolder)
{
	var	i, t, str = '<table cellspacing="0" cellpadding="0" class="msgctrl"><tr>';
	str += '<th>With selected messages:</th>';
	str += '<td><input type="button" id="msgdel" value="Delete"' + msgtt[40] + ' onClick="deleteSelected(); return false"/>';
	str += ' or <input type="button" id="msgmv" value="move"' + msgtt[41] + ' onClick="moveSelected(); return false"/> to ';
	str += '<select name="msgmvt" id="msgmvt"' + msgtt[42] + '><option value="">- select destination folder -</option>';

	for	(i=0;i<folders.length;i++)
	{
		if	(i == vFolder)
			continue;
		t = folders[i].type + '#' + folders[i].id;
		str += '<option value="' + t + '"';
		if	(cfolder == t)
			str += ' selected="selected"';
		str += '>';
		if	(folders[i].type == 'CUS')
			str += folders[i].name;
		else if	(folders[i].type == 'IN')
			str += 'Inbox';
		else if	(folders[i].type == 'INT')
			str += 'Internal transmissions';
		else
			str += 'Sent messages';
		str += '</option>';
	}

	str += '</select></td></tr></table>';
	return	str;
}


function	moveError()
{
	alert('Please select a destination folder');
}


function	confirmDelete(nm)
{
	var	str = 'You are about to delete ' + nm + ' message' + (nm > 1 ? 's' : '') + '.\nAre you sure?';
	return	confirm(str);
}


function	drawFoldersList()
{
	var	i, str = '<h1>Folders management</h1>';
	str += '<form action="?"><table cellspacing="0" cellpadding="0">';
	str += '<tr class="fldhdr"><td class="fldspc">&nbsp;</td><td class="fldspc">&nbsp;</td>';
	str += '<th class="fldname">Folder name</th><th class="msgcnt">Messages</th>';
	str += '<th class="msgcnt">(New)</th><td class="fldspc">&nbsp;</td></tr>';
	str += '<tr class="fldlst"><td>&nbsp;</td><td colspan="4"><table cellspacing="0" cellpadding="0" class="list">';
	for	(i=0;i<folders.length;i++)
	{
		var	fid = folders[i].type + '#' + folders[i].id;
		str += '<tr><td class="fldspc"><input type="checkbox"' + msgtt[50] + 'name="selfld" value="'+i;
		str += '" id="sfld'+i+'" onClick="switchFSel(\''+fid+'\');return true"';
		if	(folders[i].selected)
			str += ' checked="checked"';
		str += ' /></td><td class="fldname">';
		switch	(folders[i].type)
		{
			case "IN": str += "Inbox"; break;
			case "INT": str += "Internal Transmissions"; break;
			case "OUT": str += "Sent Messages"; break;
			case "CUS": str += '<a href="#"' + msgtt[51] + ' onClick="renameFolder('+folders[i].id+'); return false">' + folders[i].name + '</a>'; break;
		}
		str += '</td><td class="msgcnt">' + folders[i].mCount;
		str += '</td><td class="msgcnt">' + folders[i].nCount;
		str += '</td></tr>';
	}
	str += '</table></td><td class="fldspc">&nbsp;</td></tr>';
	str += '<tr><td>&nbsp;</td><td colspan="4"><table cellspacing="0" cellpadding="0"><tr>';
	str += '<td id="addfld">&nbsp;</td>';
	str += '<td id="chgfld">&nbsp;</td>';
	str += '</tr></table></td><td>&nbsp;</td></tr>';
	str += '</table></form>';
	document.getElementById('mbody').innerHTML = str;
}


function	drawAddFolder()
{
	var	e, str2, str = '<input type="button" name="adfldb" id="adfldb" value="Create"' + msgtt[60] + ' onClick="addFolder(); return false" />';
	str += ' a new folder: <input type="text" name="adfldn" id="adfldn" value="" />';
	e = document.getElementById('adfldn');
	str2 = e ? e.value : "";
	document.getElementById('addfld').innerHTML = str;
	document.getElementById('adfldn').value = str2;
}


function	drawSFControls(cnt, cd)
{
	var	str = '<input type="button" name="flfldb" id="flfldb" value="Flush"' + msgtt[70] + ' onClick="flushFolders(); return false" />';
	if	(cd)
		str += ' or <input type="button" name="rmfldb" id="rmfldb" value="delete"' + msgtt[71] + ' onClick="deleteFolders(); return false" />';
	str += ' selected folder' + (cnt > 1 ? 's' : '') + '.';
	document.getElementById('chgfld').innerHTML = str;
}


function	promptRename(an)
{
	return	prompt("Please enter the new name for this folder.\nOriginal name: " + an, an);
}


function	alertFolderName(mid)
{
	var	str = 'Error\n';
	switch	(mid)
	{
		case	0: str += "Empty folder names are not allowed"; break;
		case	1: str += "Folder name is too long (max. 32 characters)"; break;
	}
	alert(str);
}


function	confirmFDelete(nf, nm)
{
	var	str = 'You are about to delete ' + nf + ' folder' + (nf > 1 ? 's' : '');
	str += '.\nThis will result in the loss of ' + nm + ' message' + (nm > 1 ? 's' : '');
	str += '.\nAre you sure ?';
	return	confirm(str);
}


function	confirmFFlush(nf, nm)
{
	var	str = 'You are about to flush ' + nf + ' folder' + (nf > 1 ? 's' : '');
	str += '.\nThis will result in the loss of ' + nm + ' message' + (nm > 1 ? 's' : '');
	str += '.\nAre you sure ?';
	return	confirm(str);
}
