var	emptt;

function	empire_write(data)
{
	var	a, plo, flo, plst, i, mi, str, pid, pname;
	a = data.split("\n");

	// Planets overview
	plo = a[0].split('#');
	document.getElementById('plcnt').innerHTML = plo[0];
	if	(plo[1] != 'N/A')
	{
		str = '<b class="phap';
		if	(plo[1] >= 70)
			str += 'ok';
		else if	(plo[1] >= 40)
			str += 'med';
		else if	(plo[1] >= 20)
			str += 'dgr';
		else
			str += 'bad';
		str += '">' + plo[1] + '%</b>';
	}
	else
		str = plo[1];
	document.getElementById('plahap').innerHTML = str;
	if	(plo[9] != 'N/A')
	{
		str = '<b class="phap';
		if	(plo[9] >= 41)
			str += 'bad';
		else if	(plo[9] >= 41)
			str += 'dgr';
		else if	(plo[9] >= 11)
			str += 'med';
		else
			str += 'ok';
		str += '">' + plo[9] + '%</b>';
	}
	else
		str = plo[9];
	document.getElementById('placor').innerHTML = str;
	document.getElementById('plpop').innerHTML = formatNumber(plo[2]);
	document.getElementById('plfct').innerHTML = formatNumber(plo[4]);
	document.getElementById('pltrt').innerHTML = formatNumber(plo[6]);
	empire_planets(plo);

	// Planets list
	plst = a[1].split('#');
	mi = plst.length / 2;
	str = '';
	for	(i=0;i<mi;i++)
	{
		pid = plst[i*2];
		pname = plst[i*2 + 1];
		str += '<a href="planet?id=' + pid + '" ' + emptt[0] + ' >' + pname + '</a>';
		if	(i < mi - 1)
		{
			if	((i-2)%3)
				str += ' - ';
			else
				str += '<br/>';
		}
	}
	if	(str == '')
		str = '<a href="getplanet" ' + emptt[1] + ' >Get a new planet</a>';
	document.getElementById('pllst').innerHTML = str;

	// Fleets
	flo = a[2].split('#');
	document.getElementById('fltot').innerHTML = formatNumber(flo[2]);
	document.getElementById('flupk').innerHTML = formatNumber(flo[3]);
	document.getElementById('flcnt').innerHTML = formatNumber(flo[0]);
	document.getElementById('flbat').innerHTML = formatNumber(flo[1]);
	document.getElementById('flhcnt').innerHTML = formatNumber(flo[4]);
	document.getElementById('flhbat').innerHTML = formatNumber(flo[5]);
	document.getElementById('flocnt').innerHTML = formatNumber(flo[6]);
	document.getElementById('flobat').innerHTML = formatNumber(flo[7]);
	document.getElementById('flomv').innerHTML = formatNumber(flo[8]);
	document.getElementById('flowt').innerHTML = formatNumber(flo[9]);
	document.getElementById('flgas').innerHTML = formatNumber(flo[10]);
	document.getElementById('flfgt').innerHTML = formatNumber(flo[11]);
	document.getElementById('flcru').innerHTML = formatNumber(flo[12]);
	document.getElementById('flbcr').innerHTML = formatNumber(flo[13]);
	tot = parseInt(flo[10], 10) + parseInt(flo[11], 10) + parseInt(flo[12], 10) + parseInt(flo[13], 10);
	document.getElementById('flsht').innerHTML = formatNumber(tot.toString());

	// Research
	var	rd = a[3].split('#');
	var	rbPoints = rd[0], rbPercentage = new Array();
	for	(i=0;i<3;i++)
		rbPercentage[i] = parseInt(rd[i+1], 10);
	var	rbCatPoints = new Array(), s = 0;
	for	(i=0;i<3;i++)
	{
		rbCatPoints[i] = Math.floor(rbPercentage[i] * rbPoints / 100);
		s += rbCatPoints[i];
	}
	for	(i=0;s<rbPoints;i=(i+1)%3)
	{
		rbCatPoints[i] ++;
		s ++;
	}
	document.getElementById('rsbf').innerHTML = rbPercentage[0];
	document.getElementById('rspf').innerHTML = formatNumber(rbCatPoints[0].toString());
	document.getElementById('rsbm').innerHTML = rbPercentage[1];
	document.getElementById('rspm').innerHTML = formatNumber(rbCatPoints[1].toString());
	document.getElementById('rsbc').innerHTML = rbPercentage[2];
	document.getElementById('rspc').innerHTML = formatNumber(rbCatPoints[2].toString());
	displayResearchStatus(rd[4], rd[5]);

	// Money
	var	md = a[4].split('#');
	document.getElementById('minc').innerHTML = formatNumber(md[0]);
	document.getElementById('mprof').innerHTML = formatNumber(md[1]);

	setTimeout('x_getEmpireData(empire_write)', 60000);
}
