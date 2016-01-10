var	thmcrp_hdTimer;
var	thmcrp_stTimer;
var	thmcrp_msgIcon = null, thmcrp_milock = false, thmcrp_dIcon;
var	thm_sTime;


function thmcrp_activateMsgBlink() {
	if (thmcrp_milock) {
		setTimeout('thmcrp_activateMsgBlink()', 500);
		return;
	}

	thmcrp_milock = true;
	if (!thmcrp_msgIcon) {
		thmcrp_dIcon = false;
		thmcrp_msgIcon = setTimeout('thmcrp_blinkMsgBlink()', 200);
	}
	thmcrp_milock = false;
}

function thmcrp_disableMsgBlink() {
	if (thmcrp_milock) {
		setTimeout('thmcrp_disableMsgBlink()', 500);
		return;
	}

	thmcrp_milock = true;
	clearTimeout(thmcrp_msgIcon);
	thmcrp_msgIcon = null;

	var e = document.getElementById('msgmenu');
	if (typeof e.oldbgc != 'undefined') {
		e.bgColor = e.oldbgc;
	}

	thmcrp_milock = false;
}

function thmcrp_blinkMsgBlink() {
	if (thmcrp_milock) {
		setTimeout('thmcrp_blinkMsgBlink()', 50);
		return;
	}
	thmcrp_milock = true;

	var e = document.getElementById('msgmenu').getElementsByTagName('h1');
	e = e[0];
	if (thmcrp_dIcon) {
		e.oldbgc = e.style.backgroundColor;
		e.oldc = e.style.color;
		e.style.backgroundColor = '#6fafcf';
		e.style.color = '#000000';
	} else {
		e.style.backgroundColor = e.oldbgc;
		e.style.color = e.oldc;
	}
	thmcrp_dIcon = !thmcrp_dIcon;
	thmcrp_msgIcon = setTimeout('thmcrp_blinkMsgBlink()', 1000);
	thmcrp_milock = false;
}


function thmcrp_updateTime() {
	thm_sTime ++;
	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	thmcrp_stTimer = setTimeout('thmcrp_updateTime()', 1000);
}


function thmcrp_writeHeader(data) {
	if (thmcrp_stTimer) {
		clearTimeout(thmcrp_stTimer);
	}

	var	a = data.split("#");
	thm_sTime = parseInt(a.shift(), 10);

	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	document.getElementById('jspname').innerHTML = a.shift();
	document.getElementById('jscash').innerHTML = "&euro;" + formatNumber(a.shift());

	if	(a[0] == "1") {
		thmcrp_activateMsgBlink();
	} else {
		thmcrp_disableMsgBlink();
	}
	a.shift();

	if	(a[0] != "" || a.length > 1) {
		document.getElementById('jsalliance').innerHTML = " [<b>" + a.join('#') + "</b>]";
	} else {
		document.getElementById('jsalliance').innerHTML = "";
	}
	thmcrp_stTimer = setTimeout('thmcrp_updateTime()', 1000);
	thmcrp_hdTimer = setTimeout('x_getHeaderData(thmcrp_writeHeader)', 15000);
}


function updateHeader() {
	if (thmcrp_hdTimer) {
		clearTimeout(thmcrp_hdTimer);
	}
	if (thmcrp_stTimer) {
		clearTimeout(thmcrp_stTimer);
	}
	x_getHeaderData(thmcrp_writeHeader);
}
