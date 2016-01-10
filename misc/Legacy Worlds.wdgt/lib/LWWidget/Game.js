LWWidget.Game = Base.Comp.inherits({

	constructor: function (path) {
		this.base();
		this.path = path;

		this.addEvent('RequestUpdate');
		this.addSlot('startDataUpdate');

		this.pageElement = 'gDisplay';

		this.autoUpdate = new Base.Timer(30000, true);
		this.autoUpdate.bindEvent('Tick', 'startDataUpdate', this);
		this.autoUpdate.start();

		this.pages = [null, null, null, null, null, null, null, null];
		this.page = null;
		this.data = null;
	},

	destroy: function () {
		this.autoUpdate.destroy();
		if (this.page) {
			this.page.hide();
			this.page = null;
		}
		for (var i in this.pages) {
			if (this.pages[i]) {
				var _p = this.pages[i];
				this.pages[i] = null;
				_p.destroy();
			}
		}
		this.base();
	},


	startDataUpdate: function () {
		this.onRequestUpdate(this.path);
	},

	setData: function (data) {
		this.data = data;
		for (var i in this.pages) {
			if (this.pages[i]) {
				this.pages[i].setData(data);
			}
		}
		this.draw();
	},


	setPage: function (pName) {
		// Check for a valid page name
		if (!LWWidget.Game.pageMap().containsKey(pName)) {
			return;
		}

		// Check if something was changed
		var _p = this.pages[LWWidget.Game.pageMap().get(pName)];
		if (_p && this.page && _p._cid == this.page._cid || !(_p || this.page)) {
			return;
		}

		// Hide current page and show new page
		if (this.page) {
			this.page.hide();
		}
		this.page = _p;
		if (_p) {
			this.page.show();
		}
		this.draw();
	},

	draw: function () {
		var _e = document.getElementById(this.pageElement);
		if (_e) {
			if (this.page) {
				this.page.draw(_e);
			} else {
				_e.innerHTML = '<p style="text-align:center">Page not implemented</p>';
			}
		}
	},

	show: function () {
		this.startDataUpdate();
		this.autoUpdate.start();
		if (this.page) {
			this.page.show();
		}
	},

	hide: function () {
		this.autoUpdate.stop();
		if (this.page) {
			this.page.hide();
		}
	}

}, {

	pageMapInst: null,

	versionsInst: null,

	pageMap: function () {
		if (!LWWidget.Game.pageMapInst) {
			var _h = new Base.Util.Hashtable();
			_h.put('player', 0);
			_h.put('planets', 1);
			_h.put('fleets', 2);
			_h.put('budget', 3);
			_h.put('tech', 4);
			_h.put('msg', 5);
			_h.put('forums', 6);
			_h.put('ticks', 7);
			LWWidget.Game.pageMapInst = _h;
		}
		return LWWidget.Game.pageMapInst;
	},

	versions: function () {
		if (!LWWidget.Game.versionsInst) {
			LWWidget.Game.versionsInst = new Base.Util.Hashtable();
		}
		return LWWidget.Game.versionsInst;
	},

	formatTime: function (ts) {
		var d = new Date(ts * 1000);
		var str, s2;

		s2 = d.getUTCHours().toString();
		if (s2.length == 1) {
			s2 = "0" + s2;
		}
		str  = s2 + ':';

		s2 = d.getUTCMinutes().toString();
		if (s2.length == 1) {
			s2 = "0" + s2;
		}
		str += s2 + ':';

		s2 = d.getUTCSeconds().toString();
		if (s2.length == 1) {
			s2 = "0" + s2;
		}
		str += s2 + ' on ';

		s2 = d.getUTCDate().toString();
		if (s2.length == 1) {
			s2 = "0" + s2;
		}
		str += s2 + '/';

		s2 = (d.getUTCMonth() + 1).toString();
		if (s2.length == 1) {
			s2 = "0" + s2;
		}
		str += s2 + '/' +  d.getUTCFullYear();

		return	str;
	}

});
