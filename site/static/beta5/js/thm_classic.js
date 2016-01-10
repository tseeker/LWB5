var	thmcls_hdTimer;
var	thmcls_stTimer;
var	thmcls_msgIcon = null, thmcls_milock = false, thmcls_dIcon;
var	thm_sTime;


function thmcls_activateMsgBlink() {
	if (thmcls_milock) {
		setTimeout('thmcls_activateMsgBlink()', 500);
		return;
	}

	thmcls_milock = true;
	if (!thmcls_msgIcon) {
		thmcls_dIcon = false;
		thmcls_msgIcon = setTimeout('thmcls_blinkMsgBlink()', 200);
	}
	thmcls_milock = false;
}

function thmcls_disableMsgBlink() {
	if (thmcls_milock) {
		setTimeout('thmcls_disableMsgBlink()', 500);
		return;
	}

	thmcls_milock = true;
	clearTimeout(thmcls_msgIcon);
	thmcls_msgIcon = null;

	var e = document.getElementById('msgmenu');
	if (typeof e.oldbgc != 'undefined') {
		e.bgColor = e.oldbgc;
	}

	thmcls_milock = false;
}

function thmcls_blinkMsgBlink() {
	if (thmcls_milock) {
		setTimeout('thmcls_blinkMsgBlink()', 50);
		return;
	}
	thmcls_milock = true;

	var e = document.getElementById('msgmenu');
	if (thmcls_dIcon) {
		e.oldbgc = e.bgColor;
		e.bgColor = '#3F3F3F';
	} else {
		e.bgColor = e.oldbgc;
	}
	thmcls_dIcon = !thmcls_dIcon;
	thmcls_msgIcon = setTimeout('thmcls_blinkMsgBlink()', 1000);
	thmcls_milock = false;
}


function thmcls_updateTime() {
	thm_sTime ++;
	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	thmcls_stTimer = setTimeout('thmcls_updateTime()', 1000);
}


function thmcls_writeHeader(data) {
	if (thmcls_stTimer) {
		clearTimeout(thmcls_stTimer);
	}

	var	a = data.split("#");
	thm_sTime = parseInt(a.shift(), 10);

	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	document.getElementById('jspname').innerHTML = a.shift();
	document.getElementById('jscash').innerHTML = "&euro;" + formatNumber(a.shift());

	if	(a[0] == "1") {
		thmcls_activateMsgBlink();
	} else {
		thmcls_disableMsgBlink();
	}
	a.shift();

	if	(a[0] != "" || a.length > 1) {
		document.getElementById('jsalliance').innerHTML = " [<b>" + a.join('#') + "</b>]";
	} else {
		document.getElementById('jsalliance').innerHTML = "";
	}
	thmcls_stTimer = setTimeout('thmcls_updateTime()', 1000);
	thmcls_hdTimer = setTimeout('x_getHeaderData(thmcls_writeHeader)', 15000);
}


function updateHeader() {
	if (thmcls_hdTimer) {
		clearTimeout(thmcls_hdTimer);
	}
	if (thmcls_stTimer) {
		clearTimeout(thmcls_stTimer);
	}
	x_getHeaderData(thmcls_writeHeader);
}
