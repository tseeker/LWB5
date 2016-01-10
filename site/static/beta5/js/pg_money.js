var	cfUpdate;

var	montt;

function	displayCash(data)
{
	document.getElementById("cfunds").innerHTML = '&euro;' + formatNumber(data);
	cfUpdate = setTimeout('x_getCash(displayCash);', 15000);
}


function displayPage(data) {
	var	a = data.split("\n");
	var	b = a[0].split('#');
	var	str, i, c = '', d = '';

	if (b[0] == "0" && b[1] == "0") {
		if (document.getElementById('tgo')) {
			c = document.getElementById('tamt').value;
			d = document.getElementById('tdst').value;
		}
		str = drawTransferForm();
	} else if (b[0] != "0") {
		str = drawTransferWait(b[0]);
	} else if (b[1] != "0") {
		str = drawTransferVacation();
	}
	document.getElementById('transfer').innerHTML = str;
	if (b[0] == "0" && b[1] == "0") {
		document.getElementById('tamt').value = c;
		document.getElementById('tdst').value = d;
	}

	var	pcnt = parseInt(b[2],10), fcnt = parseInt(b[3], 10);
	document.getElementById('pinc').innerHTML = '&euro;' + formatNumber(b[4]);
	document.getElementById('fupk').innerHTML = '&euro;' + formatNumber(b[5]);
	document.getElementById('dprof').innerHTML = formatNumber(b[6]);

	if	(pcnt > 0)
	{
		str = drawPlanetTableHdr();
		for	(i=0;i<pcnt;i++)
		{
			var	name = a[1+i*2];
			var	pdat = a[(i+1)*2].split('#');
			var	tmp = parseInt(pdat[4], 10) + parseInt(pdat[5], 10);
			str += '<tr><td class="picon"><img class="picon" src="'+staticurl
				+ '/beta5/pics/pl/s/' + pdat[0] + '.png" alt="[P-' + pdat[0] + '" border="0" />'
				+ '</td><td class="pname"><a ' + montt[3] + ' href="planet?id=' + pdat[0] + '">' + name
				+ '</a></td><td>&euro;' + formatNumber(pdat[2]) + '</td><td>' + formatNumber(pdat[7])
				+ '</td><td>&euro;' + formatNumber(pdat[3]) +  '</td><td>&euro;' + formatNumber(pdat[4])
				+ '</td><td>&euro;' + formatNumber(pdat[5]) +  '</td><td>&euro;'
				+ formatNumber(tmp.toString()) + '</td><td>&euro;' + formatNumber(pdat[6])
				+ '</td><td>&euro;' + formatNumber(pdat[1]) + '</td></tr>'; 
		}
		str += drawPlanetTableFtr(b[4]);
	}
	else
		str = drawNoPlanets();
	document.getElementById('planets').innerHTML = str;

	if	(fcnt > 0)
	{
		str = drawFleetTableHdr();
		var	nb = 1 + pcnt*2;
		for	(i=0;i<fcnt;i++)
		{
			var	fname = a[nb+i*3];
			var	lname = a[nb+i*3+1];
			var	fdat = a[nb+i*3+2].split('#');
			var	tmp;
			str += '<tr><td><a  ' + montt[4] + ' href="fleets#fid' + fdat[0] + '">' + fname;
			str += '</td><td>' + lname + '</td><td class="dist">' + fdat[1];
			str += '</td><td class="dist">' + fdat[2] + '</td><td class="upkp">';
			str += '&euro;' + formatNumber(fdat[3]) + '</td></tr>';
		}
		str += drawFleetTableFtr(b[5]);
	}
	else
		str = drawNoFleets();
	document.getElementById('fleets').innerHTML = str;

	setTimeout('x_getCashDetails(displayPage)', 300000);
}


function	transferOk(data)
{
	if	(data == '0')
	{
		if	(cfUpdate)
			clearTimeout(cfUpdate);
		x_getCash(displayCash);
		updateHeader();
		document.getElementById('tamt').value = '';
		document.getElementById('tdst').value = '';
		showTransferOk();
	}
	else
		tfError(parseInt(data, 10));
	document.getElementById('tgo').disabled = false;
}


function	transferFunds()
{
	var	a, b;
	document.getElementById('tgo').disabled = true;

	a = parseInt(document.getElementById('tamt').value, 10);
	if	(isNaN(a) || a <= 0)
	{
		tfError(1);
		document.getElementById('tgo').disabled = false;
		return;
	}

	b = document.getElementById('tdst').value;
	if	(b == '')
	{
		tfError(2);
		document.getElementById('tgo').disabled = false;
		return;
	}

	if	(!confirmTransfer(a, b))
	{
		document.getElementById('tgo').disabled = false;
		return;
	}

	x_transferFunds(b, a, transferOk);
}
