function	formatDate(ts)
{
	var	d = new Date(ts * 1000);
	var	str, s2;

	s2 = d.getUTCHours().toString();
	if	(s2.length == 1)
		s2 = "0" + s2;
	str  = s2 + ':';

	s2 = d.getUTCMinutes().toString();
	if	(s2.length == 1)
		s2 = "0" + s2;
	str += s2 + ':';

	s2 = d.getUTCSeconds().toString();
	if	(s2.length == 1)
		s2 = "0" + s2;
	str += s2 + ' on ';

	s2 = d.getUTCDate().toString();
	if	(s2.length == 1)
		s2 = "0" + s2;
	str += s2 + '/';

	s2 = (d.getUTCMonth() + 1).toString();
	if	(s2.length == 1)
		s2 = "0" + s2;
	str += s2 + '/' +  d.getUTCFullYear();

	return	str;
}


Component.Listing.notFoundText = 'No elements found';
Component.Listing.loadingText = 'Loading ...';
Component.Listing.errorText = 'Error: the list couldn\'t be loaded';
