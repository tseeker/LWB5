function	countSelected()
{
	var	n = 0, i = 0, e;
	while	(e = document.getElementById('msel' + i))
	{
		n += e.checked ? 1 : 0;
		i++;
	}
	return	n;
}
