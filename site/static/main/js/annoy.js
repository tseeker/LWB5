function beAnnoying() {
	var _t = parseInt(document.getElementById('redtime').innerHTML, 10);
	_t --;
	if (_t == 0) {
		document.location.href = document.location.href.replace(/\.php.*$/, '.php/main/contrib');
	} else {
		document.getElementById('redtime').innerHTML = _t;
		setTimeout('beAnnoying()', 1000);
	}
}

$(function(){
	setTimeout('beAnnoying()', 1000);
});
