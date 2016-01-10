$(function() {
	var _viewportHeight = function() {
		return self.innerHeight
			|| jQuery.boxModel && document.documentElement.clientHeight
			|| document.body.clientHeight;
	};
	var _viewportWidth = function() {
		return self.innerWidth
			|| jQuery.boxModel && document.documentElement.clientWidth
			|| document.body.clientWidth;
	};

	var _handleSize = function () {
		var _h = _viewportHeight();
		if (_h < 560) {
			$("#extframe").css('top', '280px');
		} else {
			$("#extframe").css('top', '50%');
		}

		var _w = _viewportWidth();
		if (_w < 950) {
			$(".internal").css('left', '475px');
		} else {
			$(".internal").css('left', '50%');
		}
	};

	$(window).resize(_handleSize);
	_handleSize();
});
