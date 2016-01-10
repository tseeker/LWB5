var	menuText = ['Public Offers','Direct Offers','Sent Offers'];
var	searchText = ['Search for planet: ', 'Search for location: '];
var	prhStatus = ['Cancelled', 'Transfer Cancelled', 'Accepted', 'Expired', 'Rejected'];
var	soModes = ['Gift', 'Private Sale', 'Public Sale', 'Auction Sale'];

// Listing texts, defaults
var	notFound = 'No items found.';
var	listingText = ['Display %1% items / page', 'Page %1% / %2%'];
// Planet sales
var	noPSales = '<p>There are no planets for sale in this area.</p>';
var	noPSalesFound = 'No matching planets were found.';
var	psListingText = ['Display %1% planets / page', 'Page %1% / %2%'];
var	psHeaders = ['Planet', 'Coords.', 'Pop.', 'Turrets', 'Fact.', 'Fleets', 'Owner', 'Expiration', 'Price', 'Action'];
// Fleet sales
var	noFSales = '<p>There are no fleets for sale in this area.</p>';
var	noFSalesFound = 'No matching locations were found.';
var	fsListingText = ['Display %1% planets / page', 'Page %1% / %2%'];
var	fsHeaders = ['Location', 'Coords.', 'G.A.S.', 'Fgt.', 'Cru.', 'B.Cru.', 'Owner', 'Expiration', 'Price', 'Action']
// Pending private offers
var	noPOffers = '<p>There are no offers pending our approval.</p>';
var	noPOffersFound = '<b>No offers matching this location.</p>';
var	proHeaders = ['Received', 'Sender', 'Price', 'Expiration', 'Details'];
// Private offers history
var	noPHist = '<p>The history is empty.</p>';
var	noPHistFound = '<b>No archived offers matching this location.</p>';
var	prhHeaders = ['Received', 'Sender', 'Price', 'Last Change', 'Status', 'Details'];
// Pending sent offers
var	noSOffers = '<p>We have not offered to sell or give anything recently.</p>';
var	noSOffersFound = '<b>No offers matching this location.</p>';
var	sopHeaders = ['Date', 'Offer Type', 'Price', 'Expiration', 'To Player', 'Details'];
// Private offers history
var	noSHist = '<p>The history is empty.</p>';
var	noSHistFound = '<b>No archived offers matching this location.</p>';
var	sohHeaders = ['Last Change', 'Status', 'Sent', 'Offer Type', 'Price', 'To Player', 'Details'];


function	drawPublicLayout()
{
	var	str = '<table cellspacing="0" cellpadding="0" id="pubmain"><tr>';

	str += '<td><h1>Planets For Sale</h1><div id="plsale">&nbsp;</div>';
	str += '<p>&nbsp;</p><h1>Fleets For Sale</h1><div id="flsale">&nbsp;</div></td>';

	// Map table
	str += '<td rowspan="5" class="pubmap"><table cellspacing="0" cellpadding="0" id="sysmap">';
	str += '<tr><td>&nbsp;</td><td onClick="moveMapUp()" class="horz"><img src="'+staticurl;
	str += '/beta5/pics/up_'+color+'.gif" alt="Up" /></td><td>&nbsp;</td></tr><tr>';
	str += '<td onClick="moveMapLeft()" class="vert"><img src="'+staticurl+'/beta5/pics/left_';
	str += color+'.gif" alt="Left" /></td><td id="mapcnt">&nbsp;</td><td onClick="moveMapRight()" class="vert">';
	str += '<img src="'+staticurl+'/beta5/pics/right_'+color+'.gif" alt="Right" /></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="moveMapDown()" class="horz"><img src="'+staticurl;
	str += '/beta5/pics/down_'+color+'.gif" alt="Down" /></td><td>&nbsp;</td></tr></table>';

	// Centre map on coords...
	str += '<p>&nbsp;</p><table id="mapctr"><tr>';
	str += '<td onClick="document.getElementById(\'mct0\').checked=true">';
	str += '<input type="radio" name="mct" id="mct0" ' + (mapCType==0?'checked="checked" ':'') + ' /></td>';
	str += '<td onClick="document.getElementById(\'mct0\').checked=true"><label for="mct0">Centre on coordinates</label></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="document.getElementById(\'mct0\').checked=true">';
	str += '(<input type="text" name="cx" id="cx" value="'+mapX+'" size="4" />,<input type="text" name="cy" id="cy"';
	str += ' value="'+mapY+'" size="4" />)</td></tr>';

	// Centre map on own planet...
	str += '<td onClick="document.getElementById(\'mct1\').checked=true">';
	str += '<input type="radio" name="mct" id="mct1" ' + (mapCType==1?'checked="checked" ':'') + ' /></td>';
	str += '<td onClick="document.getElementById(\'mct1\').checked=true"><label for="mct1">Centre on own planet</label></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="document.getElementById(\'mct1\').checked=true">';
	str += '<select name="mcpo" id="mcpo"><option value="0">-----</option>';
	var	i, l = myPlanets.keys();
	for	(i=0;i<l.length;i++)
	{
		str += '<option value="'+l[i]+'"'+(mapCType==1&&l[i]==mapParm?' selected="selected"':'')+'>';
		str += myPlanets.get(l[i]) + '</option>';
	}
	str += '</select></td></tr>';

	// Centre map on planet...
	str += '<td onClick="document.getElementById(\'mct2\').checked=true">';
	str += '<input type="radio" name="mct" id="mct2" ' + (mapCType==2?'checked="checked" ':'') + ' /></td>';
	str += '<td onClick="document.getElementById(\'mct2\').checked=true"><label for="mct2">Centre on planet</label></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="document.getElementById(\'mct2\').checked=true">';
	str += '<input type="text" name="pn" id="mcpn" value="' + (mapCType==2?mapParm:'') + '" /></td></tr>';

	// Distance
	str += '</table><center><br/>Distance: <select name="mcds" id="mcds">';
	for	(i=1;i<8;i++)
		str += '<option value="'+i+'"'+(i==mapDist?' selected="selected"':'')+'>' + i + '</option>';
	str += '</select><br/><br/><input type="button" name="mcup" value="Update Display" onClick="updatePublicDisplay()" /></center>';
	
	str += '</td></tr></table>';
	document.getElementById('mkppage').innerHTML = str;

	// Draw listings
	pfsListing.draw();
	ffsListing.draw();
}


function	drawPSFleets(v)
{
	if	(!v.fleet)
		return	"No fleet";
	return	"G: <b>" + v.fleet[1] + "</b>; F: <b>" + v.fleet[2] + "</b>; C: <b>" + v.fleet[3] + "</b>; B: <b>" + v.fleet[4] + "</b>";
}

function	drawPSAction(v)
{
	var	s = sellers.get(v.seller);
	if	(s.isMe)
		return	'<a href="#" onClick="cancelPSale('+v.offer+');return false">Cancel Sale</a>';
	else if	(v.auction)
		return	'<a href="#" onClick="placePBid('+v.offer+');return false">Place Bid</a>';
	return	'<a href="#" onClick="buyPlanet('+v.offer+');return false">Buy</a>';
}

function	drawFSAction(v)
{
	var	s = sellers.get(v.seller);
	if	(s.isMe)
		return	'<a href="#" onClick="cancelFSale('+v.offer+');return false">Cancel Sale</a>';
	else if	(v.auction)
		return	'<a href="#" onClick="placeFBid('+v.offer+');return false">Place Bid</a>';
	return	'<a href="#" onClick="buyFleet('+v.offer+');return false">Buy</a>';
}

function	drawExpire(v)
{
	if	(v.expires == '')
		return	'Never';
	return	formatDate(v.expires);
}


// Messages for planet sales

function	confirmPCancel(p)
{
	return	confirm('You are about to cancel the public sale of planet ' + p.name + '.\nPlease confirm.');
}

function	confirmBuyPlanet(p)
{
	return	confirm('Are you sure you want to buy planet ' + p.name + ' for\n' + formatNumber(p.price) + ' euros?');
}

function	planetBought()
{
	alert('Planet has been bought. Control will be transfered to you in 4h, unless\nthe transfer is canceled by the current owner or he loses the planet.');
}

function postPBuyError(err) {
	if (err == '0') {
		alert('Unable to buy the planet: the offer has expired.');
	} else if (err == '1') {
		alert('Unable to buy the planet: not enough cash.');
	} else if (err == '200') {
		alert('You can\'t use the marketplace while on vacation.');
	} else if (err == '201') {
		alert('You can\'t use the marketplace while under Peacekeeper protection.');
	} else{
		alert('Unable to buy the planet: unknown error.');
	}
}

function confirmBidPlanet(p) {
	return prompt('Please place your bid for planet ' + p.name + '.\nMinimum bid: ' + formatNumber(p.price) + ' euros.', (parseInt(p.price)+1).toString());
}

function pBidError(p) {
	alert('Minimum bid for planet ' + p.name + ' is ' + formatNumber(p.price) + ' euros.\nYour bid must be higher than this.');
}

function postPBidError(d) {
	if (d=='0') {
		alert('Unable to place the bid: the offer has expired.');
		startUpdate();
	} else if (d=='1') {
		alert('Unable to place the bid: not enough cash.');
		startUpdate();
	} else if (d == '200') {
		alert('You can\'t use the marketplace while on vacation.');
		startUpdate();
	} else if (err == '201') {
		alert('You can\'t use the marketplace while under Peacekeeper protection.');
		startUpdate();
	} else {
		var	a = d.split('#');
		var	p = getPlanetOffer(a[1]);
		if (!p) {
			alert('An unknown error has occured.');
			startUpdate();
			return;
		}
		alert('Unable to place the bid on planet ' + p.name + '\nSomeone has placed a new bid in the meantime.\nThe minimum price is now ' + formatNumber(a[2]));
		p.price = a[2];
		pfsListing.updateDisplay();
		doPlacePBid(p.offer);
	}
}


// Messages for fleet sales

function	getFleetComposition(f)
{
	var	a = new Array();
	if (f.fleet[0] > 0) a.push(f.fleet[0] + ' GA Ship' + (f.fleet[0] > 1 ? 's' : ''));
	if (f.fleet[1] > 0) a.push(f.fleet[1] + ' Fighter' + (f.fleet[1] > 1 ? 's' : ''));
	if (f.fleet[2] > 0) a.push(f.fleet[2] + ' Cruiser' + (f.fleet[2] > 1 ? 's' : ''));
	if (f.fleet[3] > 0) a.push(f.fleet[3] + ' Battle Cruiser' + (f.fleet[3] > 1 ? 's' : ''));
	return	a.join(', ');
}

function	confirmFCancel(f)
{
	return	confirm(
		'You are about to cancel the public sale of the fleet located at ' + f.plName + '.\n'
		+ 'Fleet composition: ' + getFleetComposition(f) + '.\nPlease confirm.'
	);
}

function	confirmBuyFleet(f)
{
	return	confirm(
		'Are you sure you want to buy the fleet located at ' + f.plName + ' for\n' + formatNumber(f.price) + ' euros?\n'
		+ 'Fleet composition: ' + getFleetComposition(f) + '.\n');
}

function	fleetBought()
{
	alert('The fleet has been bought. Control will be transfered to you in 4h, unless\nthe transfer is canceled by the current owner or the fleet is destroyed.');
}

function postFBuyError(err) {
	if (err == '0') {
		alert('Unable to buy the fleet: the offer has expired.');
	} else if (err == '1') {
		alert('Unable to buy the fleet: not enough cash.');
	} else if (err == '200') {
		alert('You can\'t use the marketplace while on vacation.');
	} else if (err == '201') {
		alert('You can\'t use the marketplace while under Peacekeeper protection.');
	} else {
		alert('Unable to buy the fleet: unknown error.');
	}
}

function confirmBidFleet(f) {
	return	prompt(
		'Please place your bid for the fleet located at ' + f.plName + '.\n'
		+ 'Fleet composition: ' + getFleetComposition(f) + '.\n'
		+ 'Minimum bid: ' + formatNumber(f.price) + ' euros.',
		(parseInt(f.price,10) + 1).toString());
}

function fBidError(f) {
	alert('Minimum bid for fleet at ' + f.plName + ' is ' + formatNumber(f.price) + ' euros.\nYour bid must be higher than this.');
}

function postFBidError(d) {
	if (d=='0') {
		alert('Unable to place the bid: the offer has expired.');
		startUpdate();
	} else if (d=='1') {
		alert('Unable to place the bid: not enough cash.');
		startUpdate();
	} else if (d == '200') {
		alert('You can\'t use the marketplace while on vacation.');
		startUpdate();
	} else if (err == '201') {
		alert('You can\'t use the marketplace while under Peacekeeper protection.');
		startUpdate();
	} else {
		var a = d.split('#');
		var p = getFleetOffer(a[1]);
		if (!p) {
			alert('An unknown error has occured.');
			startUpdate();
			return;
		}
		alert('Unable to place the bid on fleet at ' + f.plName + '\nSomeone has placed a new bid in the meantime.\nThe minimum price is now ' + formatNumber(a[2]));
		p.price = a[2];
		ffsListing.updateDisplay();
		doPlaceFBid(p.offer);
	}
}


// Private offers

function	drawPrivateLayout()
{
	var	str = '<h1>Pending Offers</h1><div id="prpending">&nbsp;</div><p>&nbsp;</p>';
	str += '<h1>History</h1><div id="prhistory">&nbsp;</div>';
	document.getElementById('mkppage').innerHTML = str;

	// Draw listings
	proListing.draw();
	prhListing.draw();
}

function drawPRPrice(i)
	{ return i.price != 0 ? ('&euro;' + formatNumber(i.price)) : 'Free'; }

function drawPRDetails(i)
{
	var	str = '';
	var	s, j, ts = ['G.A. Ship', 'Fighter', 'Cruiser', 'Battle Cruiser'], r = new Array();
	if	(i.planet[0] != '')
	{
		str  = 'The planet <b>'+i.pName+'</b> at coordinates (<b>' + i.x + ',' + i.y + '</b>,' + i.orbit + '): ';
		str += '<b>' + formatNumber(i.planet[1]) + 'M</b> inhabitants, <b>' + formatNumber(i.planet[2]) + '</b> turrets and <b>';
		str += formatNumber(i.planet[3]) + '</b> factories';
		if	(i.fleet[0] != '')
		{
			str += ' along with a fleet of ';
			for	(j=0;j<4;j++)
			{
				if	(i.fleet[1+j] == 0)
					continue;
				s = '<b>' + formatNumber(i.fleet[1+j]) + '</b> ' + ts[j];
				if	(i.fleet[1+j] > 1)
					s += 's';
				r.push(s);
			}
			s = r.pop();
			str += r.join(', ') + (r.length > 0 ? ' and ' : '') + s;
		}
		str += '.';
	}
	else
	{
		str += 'A fleet of ';
		for	(j=0;j<4;j++)
		{
			if	(i.fleet[1+j] == 0)
				continue;
			s = '<b>' + formatNumber(i.fleet[1+j]) + '</b> ' + ts[j];
			if	(i.fleet[1+j] > 1)
				s += 's';
			r.push(s);
		}
		s = r.pop();
		str += r.join(', ') + (r.length > 0 ? ' and ' : '') + s;
		str += ' located in orbit around <b>' + i.pName + '</b> (<b>' + i.x + ',' + i.y + '</b>,' + i.orbit + ').';
	}
	if	(!i.aDate && i.buyer == null)
	{
		str += '<br/>[ <a href="#" onClick="acceptPrivate('+i.id+');return false">Accept offer</a> - ';
		str += '<a href="#" onClick="declinePrivate('+i.id+');return false">Decline offer</a> ]';
	}
	else if	(!i.aDate && i.buyer != null)
		str += '<br/>[ <a href="#" onClick="cancelSale('+i.id+');return false">Cancel</a> ]';

	return	str;
}

function	confirmBuyPrivate(p)
{
	var	str = 'Are you sure you want to accept this private offer?';
	if	(p.price > 0)
		str += '\nIt will cost you ' + formatNumber(p.price) + ' euros.';
	return	confirm(str);
}

function	confirmDeclinePrivate()
{
	return	confirm('Please confirm that you want to decline this offer.');
}

function privateError(err) {
	if (err == '0') {
		alert('Unable to accept the offer: it has expired.');
	} else if (err == '1') {
		alert('Unable to accept the offer: not enough cash.');
	} else if (err == '200') {
		alert('You can\'t use the marketplace while on vacation.');
	} else if (err == '201') {
		alert('You can\'t use the marketplace while under Peacekeeper protection.');
	} else {
		alert('Unable to accept the offer: unknown error.');
	}
}


// Sent offers

function	drawSentLayout()
{
	var	str = '<h1>Pending Offers</h1><div id="sopending">&nbsp;</div><p>&nbsp;</p>';
	str += '<h1>History</h1><div id="sohistory">&nbsp;</div>';
	document.getElementById('mkppage').innerHTML = str;

	// Draw listings
	sopListing.draw();
	sohListing.draw();
}

function	drawSentBuyer(v)
{
	if	(v.buyer == '')
		return "None";

	var	s = sellers.get(v.buyer);
	return	(s.isMe||s.quit ? '' : ('<a href="message?a=c&ct=0&id='+v.buyer+'">')) + s.name + (s.isMe||s.quit ? '' : '</a>');
}

function	confirmCancelSale()
{
	return	confirm('You are about to cancel an offer. Please confirm.');
}
