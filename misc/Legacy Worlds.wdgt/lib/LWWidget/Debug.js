LWWidget.Debug = Base.Comp.inherits({

	constructor: function () {
		this.base();
		this.addSlot('drawLog');

		this.timer = new Base.Timer(1000, true);
		this.timer.bindEvent('Tick', 'drawLog', this);
	},

	show: function () {
		document.getElementById('debugLink').style.display = 'none';
		document.getElementById('mainDiv').style.display = 'none';
		this.lastLength = -1;
		this.draw();
		document.getElementById('debugDiv').style.display = 'block';
		this.timer.start();
	},

	hide: function () {
		this.timer.stop();
		document.getElementById('debugDiv').style.display = 'none';
		document.getElementById('debugLink').style.display = 'block';
		document.getElementById('mainDiv').style.display = 'block';
	},

	draw: function () {
		var _str = '<table id="debugTable"><tr id="dbgHdr"><td><b>Debug mode</b></td>'
			+ '<td style="text-align:right"><a href="#" onclick="Base.Comp.get('
			+ this._cid + ').clear()">Flush log</a> - <a href="#" onclick="Base.Comp.get('
			+ this._cid + ').hide()">Hide</a></td></tr>'
			+ '<tr><td colspan="2"><div style="overflow:auto;height:260px;width:296px;color:white" id="dbgContents">&nbsp;</div></td></tr></table>';
		document.getElementById('debugDiv').innerHTML = _str;
		this.drawLog();
	},

	clear: function () {
		Base.Log.flush();
		this.drawLog();
	},

	drawLog: function () {
		var _e = document.getElementById('dbgContents');
		if (!_e) {
			return;
		}

		var _l = Base.Log.get();
		if (_l.length == this.lastLength) {
			return;
		}
		this.lastLength = _l.length;

		var _s;
		if (_l.length == 0) {
			_s = '<p style="text-align:center">The log is empty</p>';
		} else {
			_s = '<p>';
			for (var i=_l.length;i>0;i--) {
				_s += '[' + i + '] ' + _l[i-1] + '<br/>';
			}
			_s += '</p>';
		}
		_e.innerHTML = _s;
	}

});
