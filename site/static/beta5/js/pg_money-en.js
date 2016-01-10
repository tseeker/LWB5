function        makeMoneyTooltips()
{
        montt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<5;i++)
			montt[i] = "";
		return;
	}
	montt[0] = tt_Dynamic("Use this text field to type in the amount of cash you want to transfer");
	montt[1] = tt_Dynamic("Use this text field to type in the name of the player you want to transfer cash to");
	montt[2] = tt_Dynamic("Click here to perform the cash tranfer");
	montt[3] = tt_Dynamic("Click here to go to this planet's individual page");
	montt[4] = tt_Dynamic("Click here to go to this fleet's individual page");
}


function	drawTransferVacation()
{
	return	'<p>It is impossible to transfer funds while on vacation mode.</p>';
}


function	drawTransferWait(days)
{
	return	'<p>You need to wait another <b>' + days + ' day' + (days > 1 ? 's' : '')
			+ '</b> before being able to transfer funds.</p>';
}


function	drawTransferForm()
{
	var	str = '<form action="?"><p>';
	str += 'Transfer &euro;<input type="text" name="tamt" id="tamt" value="" size="9" ' + montt[0] + ' /> ';
	str += 'to player <input type="text" name="tdst" id="tdst" value="" size="16" maxlength="15" ' + montt[1] + ' />';
	str += '<input type="button" name="tgo" id="tgo" value="Ok" onClick="transferFunds(); return false;"  ' + montt[2] + ' /> ';
	str += '</p></form>';
	return	str;
}


function	drawPlanetTableHdr()
{
	var	str = '<table id="lstpl" class="list" cellspacing="0" cellpadding="0">';
	str += '<tr>';
	str += '<th class="picon"></th>';
	str += '<th class="pname">Planet</th>';
	str += '<th>Base income</th>';
	str += '<th>Industrial Factories</th>';
	str += '<th>Factory Income</th>';
	str += '<th>Factory Upkeep</th>';
	str += '<th>Turret Upkeep</th>';
	str += '<th>Expense</th>';
	str += '<th>Corruption Cost</th>';
	str += '<th>Profit</th>';
	str += '</tr>';
	return	str;
}


function	drawPlanetTableFtr(n)
{
	var	str = '<tr>';
	str += '<th colspan="9" class="tdi">Total Daily Income:</th>';
	str += '<th class="tdi">&euro;' + formatNumber(n) + '</th>';
	str += '</tr></table>';
	return	str;
}


function	drawFleetTableHdr()
{
	var	str = '<table id="lstfl" class="list" cellspacing="0" cellpadding="0">';
	str += '<tr>';
	str += '<th>Name</th>';
	str += '<th>Location</th>';
	str += '<th class="dist">Distance</th>';
	str += '<th class="dist">Delay</th>';
	str += '<th class="upkp">Upkeep</th>';
	str += '</tr>';
	return	str;
}


function	drawFleetTableFtr(n)
{
	var	str = '<tr>';
	str += '<th colspan="4" class="tfu">Total Fleet Upkeep:</th>';
	str += '<th class="tfu">&euro;' + formatNumber(n) + '</th>';
	str += '</tr></table>';
	return	str;
}


function	drawNoPlanets()
{
	return	"<p>You do not own any planet.</p>";
}


function	drawNoFleets()
{
	return	"<p>You do not own any fleet.</p>";
}


function	tfError(en)
{
	var	str;
	switch	(en)
	{
	case	1:
		str = 'You must indicate the amount to transfer.';
		break;
	case	2:
		str = 'You must indicate a player to transfer funds to.';
		break;
	case	3:
		str = 'The target player wasn\'t found. Please check the name.';
		break;
	case	4:
		str = 'You cannot transfer funds to yourself.';
		break;
	case	5:
		str = 'The target player cannot receive money yet.';
		break;
	case	6:
		str = 'You cheat! We already told you you can\'t transfer funds!';
		break;
	case	7:
		str = 'You can\'t transfer funds while under Peacekeeper protection.';
		break;
	case	8:
		str = 'This player is under Peacekeeper protection.';
		break;
	case	9:
		str = 'You don\'t have that much money.';
		break;
	case	10:
		str = 'This player is on vacation.';
		break;
	default:
		str = 'An unkown error has occured: ' + en;
		break;
	}
	alert('Funds Transfer: error\n\n' + str);

}


function	confirmTransfer(am, pl)
{
	return	confirm('Funds Transfer: please confirm\n\nYou are about to transfer '+formatNumber(am.toString())+' euro'+(am>1?'s':'')+'\nto player ' + pl);
}


function	showTransferOk()
{
	alert('Funds have been transfered.');
}
