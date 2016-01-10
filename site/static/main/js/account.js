$(function(){
	var _selected;

	var _getSelected = function (text) {
		if (text == '#acc-quit' || $('#acc-vacation') && text == '#acc-vacation') {
			_selected = text;
		} else {
			_selected = '#acc-games';
		}
	};

	var _display = function () {
		$('#acc-quit').css('display', (_selected == '#acc-quit') ? 'block' : 'none');
		if ($('#acc-vacation')) {
			$('#acc-vacation').css('display', (_selected == '#acc-vacation') ? 'block' : 'none');
		}
		$('#acc-games').css('display', (_selected == '#acc-games') ? 'block' : 'none');
		$('#tabs a' + _selected.replace(/acc-/, 'tab-')).addClass('selected');
	};

	_getSelected(location.hash);
	_display();

	$('#tabs a').click(function() {
		$('#tabs a' + _selected.replace(/acc-/, 'tab-')).removeClass('selected');
		_getSelected($(this).attr('href'));
		_display();
	});
});
